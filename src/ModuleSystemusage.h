//
// Created by leon on 15.12.16.
//

#ifndef INFOSCREEN_MODULESYSTEMUSAGE_H
#define INFOSCREEN_MODULESYSTEMUSAGE_H
#include "Module.h"
#include <thread>
#include <mutex>
#include <list>
#include <unordered_map>


class ModuleSystemusage: public Module {
public:
    ModuleSystemusage(std::string host);
    virtual ~ModuleSystemusage();
protected:
    static std::unordered_map<std::string, std::list<ModuleSystemusage*>*> map;
    std::string host;
    static const int cycleDuration = 180000;
    static const int updateInterval = 200;
    static const int arrlength = cycleDuration / updateInterval;
    int lastcputime;
    int lastcputotal;
    double* cpu;
    int* mem;
    int* swp;
    int* totalmem;
    int* totalswp;
    int* totalcpu;
    int* tlindex;
    sf::Color bgcolor = sf::Color(255, 255, 255, 64);
    sf::Color linecolor = sf::Color::White;
    sf::RectangleShape line;
    sf::VertexArray bg;
    std::mutex* mutex;
    virtual void draw();
private:
    void refreshLoop();
    bool active;
    std::thread* thread;
    std::string exec(const char* cmd);
};


#endif //INFOSCREEN_MODULESYSTEMUSAGE_H
