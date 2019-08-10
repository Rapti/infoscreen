//
// Created by leon on 15.12.16.
//

#include "ModuleRam.h"
#include "Screen.h"
#include <sstream>
#include <cmath>
#include <iomanip>


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
    std::string str = "ssh "+host+" free -b \\; cat /proc/stat \\| head -n1";
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
		long totalmem = -1, mem = -1, totalswp = -1, swp = -1, totalcpu = -1, cpu = -1, dump = -1;
        iss >> totalmem;        // total
        iss >> dump;    // Used
        iss >> dump;    // Free
        iss >> dump;    // Shared
        iss >> dump;    // Cache
        iss >> mem;    // Available
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

        cpu = user + nice + system;
        totalcpu = user + nice + system + idle + iowait + irq + softirq;


		mutex->lock();
		while(!snapshots->empty() && (snapshots->front() == nullptr || snapshots->front()->getAge().asSeconds() > 200)) {
			delete snapshots->front();
			snapshots->pop_front();
		}

		if(cpu >= 0 && totalcpu >= 0 && mem >= 0 && totalmem >= 0 && swp >= 0 && totalswp >= 0)
			snapshots->push_back(new SystemusageSnapshot(c, cpu, totalcpu, mem, totalmem, swp, totalswp));
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

void ModuleSystemusage::draw(std::list<sf::Vector2f*> points) {
	if(points.size() < 2) return;
	int xoffset = 0;
	int yoffset = 0;
	float tlheight = getDisplayHeight() - 1.5F*yoffset;

	sf::VertexArray bg = sf::VertexArray(sf::TrianglesStrip, points.size() * 2);
	int i = 0;
	for (auto &point : points) {
		bg[2*i].position = sf::Vector2f(xoffset + point->x, tlheight);
		bg[2*i+1].position = sf::Vector2f(xoffset + point->x, yoffset + point->y);
		bg[2*i].color = bg[2*i+1].color = Screen::singleton->getTheme()->getDiagramFill();
		++i;
	}
	t->draw(bg);
	for(i = 1; i < points.size(); ++i) {
		sf::Vector2f p1 = bg[2*i-1].position;
		sf::Vector2f p2 = bg[2*i+1].position;
		line.setPosition(p1.x - 1.5F, p1.y - 1.5F);
		line.setSize(sf::Vector2f((float) std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2)) + 3, 3));
		line.setRotation((float) (std::atan((p2.y - p1.y) / (p2.x - p1.x)) * 180/M_PIl));
		t->draw(line);
	}
}

SystemusageSnapshot::SystemusageSnapshot(sf::Clock c,
										 long cpu, long totalcpu,
										 long mem, long totalmem,
										 long swp, long totalswp):
		c(c),
		cpu(cpu), totalcpu(totalcpu),
		mem(mem), totalmem(totalmem),
		swp(swp), totalswp(totalswp) {
}

SystemusageSnapshot::~SystemusageSnapshot() = default;

sf::Time SystemusageSnapshot::getAge() {
	return c.getElapsedTime();
}

const long SystemusageSnapshot::getCpu() const {
	return cpu;
}

const long SystemusageSnapshot::getTotalcpu() const {
	return totalcpu;
}

const long SystemusageSnapshot::getMem() const {
	return mem;
}

const long SystemusageSnapshot::getTotalmem() const {
	return totalmem;
}

const long SystemusageSnapshot::getSwp() const {
	return swp;
}

const long SystemusageSnapshot::getTotalswp() const {
	return totalswp;
}

const float SystemusageSnapshot::getCpuSince(SystemusageSnapshot* s) const {
	return (float) (cpu - s->cpu) / (totalcpu - s->totalcpu);
}

