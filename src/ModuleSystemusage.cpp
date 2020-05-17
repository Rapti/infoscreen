//
// Created by leon on 15.12.16.
//

#include "ModuleRam.h"
#include "Screen.h"
#include "ModuleSystemusage.h"

#include <sstream>
#include <cmath>
#include <iomanip>
#include <utility>


std::unordered_map<std::string, std::list<ModuleSystemusage*>*> ModuleSystemusage::map;

ModuleSystemusage::ModuleSystemusage(const std::string& host) : Module() {
    ModuleSystemusage::host = host;
    if(map.count(host)) {
        ModuleSystemusage* ref = map[host]->front();
        snapshots = ref->snapshots;
        mutex = ref->mutex;
        thread = ref->thread;
    } else {
        map[host] = new std::list<ModuleSystemusage*>;
        snapshots = new std::list<SystemusageSnapshot*>;
        mutex = new std::mutex;
        active = true;
        thread = new std::thread(&ModuleSystemusage::refreshLoop, std::ref(*this));
    }
    map[host]->push_back(this);
    line.setOrigin(1.5, 1.5);
	line.setFillColor(Screen::singleton->getTheme()->getDiagramLine());
}

ModuleSystemusage::~ModuleSystemusage() {
    map[host]->remove(this);
    if (!map.count(host)) {
        active = false;
        thread->join();
        mutex->lock();
        delete thread;
		while(!snapshots->empty()) {
			delete snapshots->front();
			snapshots->pop_front();
		}
		delete snapshots;
        delete map[host];
        map.erase(host);
        mutex->unlock();
        delete mutex;
    }
}





void ModuleSystemusage::draw() {}

void ModuleSystemusage::refreshLoop() {
    sf::Clock clock;
    std::string str = "ssh -x "+host+R"( free -b \; cat /proc/stat \| head -n1 \; df --block-size=1 --output=source,target,size,avail)";
    const char* cmd = str.c_str();
    while(active) {
        std::string result = exec(cmd);
		sf::Clock c;

        std::string word;

        std::istringstream iss(result, std::istringstream::in);

        while(iss >> word) {
            if(word == "Mem:" || word == "Speicher:")
                break;
        }
		long totalmem = -1, mem = -1, totalswp = -1, swp = -1, totalcpu = -1, cpu = -1, nicedcpu = -1, dump = -1;
        iss >> totalmem;        // total
        iss >> dump;   			// Used
        iss >> dump;   			// Free
        iss >> dump;   			// Shared
        iss >> dump;   			// Cache
        iss >> mem;    			// Available
        mem = totalmem - mem;


        while(iss >> word)
            if(word == "Swap:")
                break;
        iss >> totalswp;
        iss >> swp;



        while(iss >> word) {
            if(word == "cpu")
                break;
        }

        long user;
        long nice;
        long system;
        long idle;
        long iowait;
        long irq;
        long softirq;

        iss >> user;
        iss >> nice;
        iss >> system;
        iss >> idle;
        iss >> iowait;
        iss >> irq;
        iss >> softirq;

        cpu = user + system;
        nicedcpu = cpu + nice;
        totalcpu = nicedcpu + idle + iowait + irq + softirq;

		while(iss >> word) {
			if(word == "Avail" || word == "Verf.")
				break;
		}

		auto* disks = new std::list<Disk*>();

		while(iss >> word) {
			std::string source;
			std::string mountpoint;
			long capacity;
			long used;

			source = word;
			iss >> mountpoint;
			iss >> capacity;
			iss >> used;

//			std::cout << "Source: " << source << " Mountpoint: " << mountpoint << " Capacity: " << capacity << " Used: " << used << std::endl;

			disks->push_back(new Disk(source, mountpoint, capacity, used));
		}

		mutex->lock();
		while(!snapshots->empty() && (snapshots->front() == nullptr || snapshots->front()->getAge().asSeconds() > 200)) {
			delete snapshots->front();
			snapshots->pop_front();
		}

		if(cpu >= 0 && totalcpu >= 0 && mem >= 0 && totalmem >= 0 && swp >= 0 && totalswp >= 0)
			snapshots->push_back(new SystemusageSnapshot(c, cpu, nicedcpu, totalcpu, mem, totalmem, swp, totalswp, disks));
        mutex->unlock();

        int d = updateInterval - clock.getElapsedTime().asMilliseconds();
        if (d > 0)
            std::this_thread::sleep_for(std::chrono::milliseconds(d));
        clock.restart();
    }
}

std::string ModuleSystemusage::exec(const char* cmd) {
        char buffer[128];
        std::string result;
        std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
        if(!pipe) {std::cout << "Error opening pipe" << std::endl; return "";}
        if (!pipe) throw std::runtime_error("popen() failed!");
        while (!feof(pipe.get())) {
            if (fgets(buffer, 128, pipe.get()) != nullptr)
                result += buffer;
        }
        return result;
}

std::string ModuleSystemusage::bytesToHumanReadableFormat(const long bytes) const {
	std::stringstream ss;
	if(bytes > 1024 * 1024 * 1024) {
		ss << 100 * bytes / (1024 * 1024 * 1024) / (float) 100;
		ss << " GB";
	} else if(bytes > 1024 * 1024) {
		ss  << 100 * bytes / (1024 * 1024) / (float) 100;
		ss << " MB";
	} else if(bytes > 1024) {
		ss << 100 * bytes / 1024 / (float) 100;
		ss << " KB";
	} else {
		ss << std::setw(6) << std::setfill(' ') << bytes;
		ss << " B";
	}
	return ss.str();
}

// z component contains cpu time of niced processes when drawing cpu meter
void ModuleSystemusage::draw(std::list<sf::Vector3f*> points) {
	if(points.size() < 2) return;
	float tlheight = getDisplayHeight();

	sf::VertexArray bg = sf::VertexArray(sf::TrianglesStrip, points.size() * 2);
	int i = 0;
	for (auto &point : points) {
		bg[2*i].position = sf::Vector2f(point->x, tlheight);
		bg[2*i+1].position = sf::Vector2f(point->x, point->y);
		bg[2*i].color = bg[2*i+1].color = Screen::singleton->getTheme()->getDiagramFill();
		++i;
	}
	t->draw(bg);
	sf::Vector3f* last = points.front();
	for(auto point: points) {
		if(last->z >= 0 && point->z >= 0 && (last->y != last->z || point->y != point->z)) {
//			line.setFillColor(Screen::singleton->getTheme()->getDiagramFill());
			drawLine(last->x, last->z, point->x, point->z);
//			line.setFillColor(Screen::singleton->getTheme()->getDiagramLine());
		} else
			drawLine(last->x, last->y, point->x, point->y);
		last = point;
	}
}

void ModuleSystemusage::drawLine(float x1, float y1, float x2, float y2) {
	sf::Vector2f p1 = sf::Vector2f(x1, y1);
	sf::Vector2f p2 = sf::Vector2f(x2, y2);
	line.setPosition(p1.x - 1.5F, p1.y - 1.5F);
	line.setSize(sf::Vector2f((float) std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2)) + 3, 3));
	line.setRotation((float) (std::atan((p2.y - p1.y) / (p2.x - p1.x)) * 180/M_PIl));
	t->draw(line);
}

SystemusageSnapshot::SystemusageSnapshot(sf::Clock c,
										 long cpu, long nicedcpu, long totalcpu,
										 long mem, long totalmem,
										 long swp, long totalswp,
										 std::list<Disk*>* disks):
		c(c),
		cpu(cpu), nicedcpu(nicedcpu), totalcpu(totalcpu),
		mem(mem), totalmem(totalmem),
		swp(swp), totalswp(totalswp),
		disks(disks) {
}

SystemusageSnapshot::~SystemusageSnapshot() {
	for(auto* disk: *disks) {
		delete disk;
	}
	delete disks;
};

sf::Time SystemusageSnapshot::getAge() {
	return c.getElapsedTime();
}

long SystemusageSnapshot::getCpu() const {
	return cpu;
}

long SystemusageSnapshot::getNicedCpu() const {
	return nicedcpu;
}

long SystemusageSnapshot::getTotalcpu() const {
	return totalcpu;
}

long SystemusageSnapshot::getMem() const {
	return mem;
}

long SystemusageSnapshot::getTotalmem() const {
	return totalmem;
}

long SystemusageSnapshot::getSwp() const {
	return swp;
}

long SystemusageSnapshot::getTotalswp() const {
	return totalswp;
}

const std::list<Disk*>* SystemusageSnapshot::getDisks() const {
	return disks;
}
float SystemusageSnapshot::getCpuSince(SystemusageSnapshot* s) const {
	return (float) (cpu - s->cpu) / (totalcpu - s->totalcpu);
}
float SystemusageSnapshot::getNicedCpuSince(SystemusageSnapshot* s) const {
	return (float) (nicedcpu - s->nicedcpu) / (totalcpu - s->totalcpu);
}


Disk::Disk(std::string source, std::string mountpoint, const long capacity, const long used) : source(std::move(source)), mountpoint(std::move(mountpoint)),
																				  capacity(capacity), used(used) {}

Disk::~Disk() {
	std::cout << "Disk " << source << ": " << getUsedPercentage() << " % used" << std::endl;
}

const std::string &Disk::getSource() const {
	return source;
}

const std::string &Disk::getMountpoint() const {
	return mountpoint;
}

const long Disk::getCapacity() const {
	return capacity;
}

const long Disk::getUsed() const {
	return used;
}

const double Disk::getUsedPercentage() const {
	return ((double) used / capacity) * 100;
}

bool Disk::is(std::string path) {
	return source == path || mountpoint == path;
};
