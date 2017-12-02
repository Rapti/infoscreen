//
// Created by leon on 13.11.17.
//

#ifndef INFOSCREEN_MODULEDATAUSAGE_H
#define INFOSCREEN_MODULEDATAUSAGE_H


#include <thread>
#include <mutex>
#include <regex>
#include "Module.h"

class ModuleDatausage: public Module {
public:
    ModuleDatausage();
    virtual ~ModuleDatausage();
    std::mutex* mutex;
    virtual void draw();
    virtual float getWidth();
private:
    int upload;
    int download;
    int limit;

    sf::Color bgcolor = sf::Color(255, 255, 255, 64);
    sf::Color linecolor = sf::Color::White;
    sf::RectangleShape rect;
    sf::RectangleShape line;
    sf::RectangleShape dayline;
    sf::VertexArray tri;

    void refreshLoop();
    int regmatch(std::string, std::regex);
    bool active;
    std::thread* thread;
    std::string exec(const char* cmd);
};


#endif //INFOSCREEN_MODULEDATAUSAGE_H
