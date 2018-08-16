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
#include "EventListener.h"
#include "ThemeDefault.h"
#include "ThemeMovingShapes.h"


class Screen {
private:
    void renderLoop();
    sf::Texture* bg;
    sf::Sprite* bgs;
    Grid* g;
	std::vector<EventListener*> listeners;
	Theme* t;
public:
    Screen();
    virtual ~Screen();
    void run();
    void updateSize();
    sf::RenderWindow* getWindow();
	void addEventListener(EventListener*);

    static Screen* singleton;
    sf::View view;
    sf::RenderWindow* window;
    Theme* getTheme();
    Grid* getGrid();

	void removeEventListener(EventListener* e);
};


#endif //INFOSCREEN_SCREEN_H
