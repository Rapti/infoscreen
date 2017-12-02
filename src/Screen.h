//
// Created by leon on 11.03.16.
//

#ifndef INFOSCREEN_SCREEN_H
#define INFOSCREEN_SCREEN_H
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <chrono>
#include <thread>
#include <iostream>
#include <list>
#include "Module.h"


class Screen {
private:
//    std::list<Module*> modules;
    std::list<std::list<Module*>*>* columns;
    void renderLoop();
    sf::Texture* bg;
    sf::Sprite* bgs;
public:
    Screen();
    virtual ~Screen();
    void run();
    void addModule(Module* m);
    void updateSize();
    sf::RenderWindow* getWindow();

    const int margin = 16;
    const int colcount = 3;

    static Screen* singleton;
    sf::View view;
    sf::RenderWindow* window;
};


#endif //INFOSCREEN_SCREEN_H
