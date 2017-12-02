//
// Created by leon on 15.12.16.
//

#include "ModuleSystemusage.h"
#include "Screen.h"
#include <sstream>
#define _USE_MATH_DEFINES
#include <math.h>


//int* ModuleSystemusage::cpu;
//int* ModuleSystemusage::mem;
//int* ModuleSystemusage::swp;
//int* ModuleSystemusage::totalmem;
//int* ModuleSystemusage::totalswp;
//int* ModuleSystemusage::tlindex;
//bool* ModuleSystemusage::active;
//std::thread* ModuleSystemusage::thread;
//std::mutex* ModuleSystemusage::mutex;

std::unordered_map<std::string, std::list<ModuleSystemusage*>*> ModuleSystemusage::map;

ModuleSystemusage::ModuleSystemusage(std::string host): Module() {
    ModuleSystemusage::host = host;
    if(map.count(host)) {
        ModuleSystemusage* ref = map[host]->front();
        cpu = ref->cpu;
        mem = ref->mem;
        swp = ref->swp;
        totalmem = ref->totalmem;
        totalswp = ref->totalswp;
        totalcpu = ref->totalcpu;
        tlindex = ref->tlindex;
        mutex = ref->mutex;
        thread = ref->thread;
    } else {
        map[host] = new std::list<ModuleSystemusage*>;
        lastcputime = 0;
        lastcputotal = 0;
        cpu = new double[arrlength];
        mem = new int[arrlength];
        swp = new int[arrlength];
        for(int i = 0; i < arrlength; ++i) {
            cpu[i] = mem[i] = swp[i] = -1;

        }
        totalmem = new int(0);
        totalswp = new int(0);
        totalcpu = new int(0);
        tlindex = new int(0);
        mutex = new std::mutex;
        active = true;
        thread = new std::thread(&ModuleSystemusage::refreshLoop, std::ref(*this));
    }
    map[host]->push_back(this);
    bg = sf::VertexArray(sf::TrianglesStrip, arrlength * 2);
    for(int i = 0; i < arrlength * 2; ++i) {
        bg[i].color = bgcolor;
    }
    line.setOrigin(1.5, 1.5);
}

ModuleSystemusage::~ModuleSystemusage() {
    map[host]->remove(this);
    if (!map.count(host)) {
        active = false;
        thread->join();
        mutex->lock();
        delete thread;
        delete cpu;
        delete mem;
        delete swp;
        delete map[host];
        map.erase(host);
        mutex->unlock();
        delete mutex;
    }
}





void ModuleSystemusage::draw() {
//    sf::Text text;
//    text.setFont(f);
//    std::stringstream ss;
//    ss << "Mem: " << (mem[tlindex] / 1048576.0) << " GB" << std::endl;
//    ss << "Swp: " << (swp[tlindex] / 1048576.0) << " GB" << std::endl;
//    text.setString(ss.str());
//    text.setCharacterSize(30);
//    text.setColor(sf::Color::White);
//    text.setPosition(10, 10);
//    t->draw(text);
//    totalmem = arrlength;

    int xoffset = 0;
    int yoffset = 0;
    float tlheight = getHeight() - 1.5*yoffset;
    float tlwidth = getWidth() - 2*xoffset;


    for(int i = 0; i < arrlength; ++i) {
        bg[2*i+1].position = sf::Vector2f(xoffset + (tlwidth / (arrlength-1)) * i, yoffset + tlheight);
    }
    t->draw(bg);
    for(int i = 1; i < arrlength; ++i) {
        sf::Vector2f p1 = bg[2*i-2].position;
        sf::Vector2f p2 = bg[2*i].position;
        line.setPosition(p1.x - 1.5, p1.y - 1.5);
        line.setSize(sf::Vector2f(std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2)) + 3, 3));
        line.setRotation(std::atan((p2.y - p1.y) / (p2.x - p1.x)) * 180/M_PIl);
        t->draw(line);
//            Screen::singleton->window->draw(line);
    }

}

void ModuleSystemusage::refreshLoop() {
//    const char* cmd = ("ssh " + host + " ~/catest.sh").c_str();
    sf::Clock clock;
    std::string str = "ssh "+host+" free\\; cat /proc/stat \\| head -n1";
    const char* cmd = str.c_str();
    while(active) {
        std::string result = exec(cmd);
        mutex->lock();
        *tlindex = (*tlindex + 1) % arrlength;
        std::string word;

        std::istringstream iss(result, std::istringstream::in);

        while(iss >> word) {
            if(word == "Mem:" || word == "Speicher:")
                break;
        }
        iss >> *totalmem;        // total
        iss >> mem[*tlindex];    // Used
        iss >> mem[*tlindex];    // Free
        iss >> mem[*tlindex];    // Shared
        iss >> mem[*tlindex];    // Cache
        iss >> mem[*tlindex];    // Available
        mem[*tlindex] = *totalmem - mem[*tlindex];

        for(int i = 0; i < arrlength; ++i)
            if(mem[i] > *totalmem)
                *totalmem = mem[i];

        while(iss >> word)
            if(word == "Swap:")
                break;
        iss >> *totalswp;
        for(int i = 0; i < arrlength; ++i)
            if(swp[i] > *totalswp)
                *totalswp = swp[i];
        iss >> swp[*tlindex];



        while(iss >> word) {
            if(word == "cpu")
                break;
        }

        int user;
        int nice;
        int system;
        int idle;
        int iowait;
        int irq;
        int softirq;

        iss >> user;
        iss >> nice;
        iss >> system;
        iss >> idle;
        iss >> iowait;
        iss >> irq;
        iss >> softirq;

        int cputime = user + nice + system;
        double total = user + nice + system + idle + iowait + irq + softirq;

        if(lastcputime > 0) {
            cpu[*tlindex] = (cputime - lastcputime) / (total - lastcputotal);
//            std::cout << "Idle: " << idle - lastidle << "   Total: " << total - lastcputime << "  CPU: " << cpu[*tlindex] << std::endl;
//            std::cout << "CPU: " <<  cpu[*tlindex];
            *totalcpu = 1;
        }

        lastcputime = cputime;
        lastcputotal = total;

        mutex->unlock();

        int d = updateInterval - clock.getElapsedTime().asMilliseconds();
        if (d > 0)
            std::this_thread::sleep_for(std::chrono::milliseconds(d));
        clock.restart();
    }
}

std::string ModuleSystemusage::exec(const char* cmd) {
//        {std::cout << "Error opening pipe" << std::endl; return "";}

        char buffer[128];
        std::string result = "";
        std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
        if(!pipe) {std::cout << "Error opening pipe" << std::endl; return "";}
        if (!pipe) throw std::runtime_error("popen() failed!");
        while (!feof(pipe.get())) {
            if (fgets(buffer, 128, pipe.get()) != NULL)
                result += buffer;
        }
        return result;
}