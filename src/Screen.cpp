//
// Created by leon on 11.03.16.
//

#include "Screen.h"
#include "ModuleTime.h"
#include "ModuleRam.h"
#include "ModuleSwp.h"
#include "ModuleCpu.h"
#include "ModuleDatausage.h"
#include "ModulePing.h"
#include "ModulePublicTransitStop.h"


Screen* Screen::singleton;

Screen::Screen() {

    delete Screen::singleton;
    Screen::singleton = this;
//    window = new sf::RenderWindow(sf::VideoMode(1920, 1200), "Test", sf::Style::None);
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    window = new sf::RenderWindow(sf::VideoMode(1366, 768), "Infoscreen", sf::Style::Default, settings);
    window->setVerticalSyncEnabled(true);
//    window = new sf::RenderWindow(sf::VideoMode(800, 600), "Test");
    view.reset(sf::FloatRect(0, 0, 1366, 768));
    window->setView(view);

    bg = new sf::Texture;

//    bg->loadFromFile("/home/leon/ClionProjects/Infoscreen/res/images/nature-trees-blur-blurred.jpg");
    bg->loadFromFile("/home/leon/ClionProjects/Infoscreen/res/images/Amazing-night-sky-blurred.jpg");
    bgs = new sf::Sprite;
    bgs->setTexture(*bg);
    std::cout << "About to create Grid" << std::endl;
    g = new Grid(3, 5);
    std::cout << "Grid created" << std::endl;

}
Screen::~Screen() {
    delete window;
    delete g;
}

sf::RenderWindow* Screen::getWindow() {
    return window;
}




void Screen::run() {
    g->addModule(new ModuleDatausage(), 0, 0, 3, 1);
    g->addModule(new ModuleRam("gaming-pc"), 0, 1, 1, 1);
    g->addModule(new ModuleSwp("gaming-pc"), 0, 2, 1, 1);
    g->addModule(new ModuleCpu("gaming-pc"), 0, 3, 1, 1);
    g->addModule(new ModuleTime(), 0, 4, 1, 1);
    g->addModule(new ModulePing({"10.4.12.200", "gaming-pc", "einspluseins", "server.raptilic.us"}, {"Bluelou", "PC", "Telefon", "Kimsufi"}), 1, 1, 1, 1);
    g->addModule(new ModulePublicTransitStop("Dortmund", "Universität S"), 2, 1, 1, 4);


    std::cout << "All Modules added" << std::endl;
    renderLoop();
}

void Screen::updateSize() {
    float scale = 0;
    float scaleX = (float) view.getSize().x / bg->getSize().x;
    float scaleY = (float) view.getSize().y / bg->getSize().y;
    if(scaleX > scaleY)
        scale = scaleX;
    else scale = scaleY;
    bgs->setScale(scale, scale);
    g->updateDisplaySize(view.getSize().x, view.getSize().y);
}

void Screen::renderLoop() {
    updateSize();
    while (window->isOpen()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window->pollEvent(event)) {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                window->close();
            if (event.type == sf::Event::Resized) {
                view.reset(sf::FloatRect(0, 0, event.size.width, event.size.height));
                view.setSize(event.size.width, event.size.height);
                window->setView(view);
                updateSize();
            }
        }

        window->clear(sf::Color::Blue);
        window->draw(*bgs);

        g->drawTo(window);

        window->display();
    }
}