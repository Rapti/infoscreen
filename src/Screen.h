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
    void renderLoop();
    sf::Texture* bg;
    sf::Sprite* bgs;
    Grid* g;
public:
    Screen();
    virtual ~Screen();
    void run();
    void updateSize();
    sf::RenderWindow* getWindow();

    static Screen* singleton;
    sf::View view;
    sf::RenderWindow* window;
};


#endif //INFOSCREEN_SCREEN_H
