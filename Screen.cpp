//
// Created by leon on 11.03.16.
//

#include "Screen.h"
#include "ModuleTime.h"
#include "ModuleEmpty.h"
#include "ModuleRam.h"
#include "ModuleSwp.h"
#include "ModuleCpu.h"
#include "ModuleDatausage.h"
#include "ModuleInvisible.h"


Screen* Screen::singleton;

Screen::Screen() {
    std::cout << "Screen Constructor begin" << std::endl;

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
    columns = new std::list<std::list<Module*>*>;
    for(int i = 0; i < colcount; ++i) {
        columns->push_back(new std::list<Module*>);
    }

    bg = new sf::Texture;

//    bg->loadFromFile("/home/leon/ClionProjects/Infoscreen/res/images/nature-trees-blur-blurred.jpg");
    bg->loadFromFile("/home/leon/ClionProjects/Infoscreen/res/images/Amazing-night-sky-blurred.jpg");
    bgs = new sf::Sprite;
    bgs->setTexture(*bg);

    std::cout << "Screen Constructor end" << std::endl;
}
Screen::~Screen() {
    std::cout << "Screen Destructor begin" << std::endl;
    delete window;
    for(std::list<Module*>* column: *columns) {
        for(Module *m: *column) {
            delete m;
        }
        delete column;
    }
    delete columns;
    std::cout << "Screen Destructor end" << std::endl;
}

sf::RenderWindow* Screen::getWindow() {
    return window;
}


void Screen::addModule(Module* m) {
    std::cout << "Adding Module" << std::endl;
    if (colcount < 1) return;
    std::list<Module*>* shortest = columns->front();
    int shortestHeight = 0;
    for(Module *m: *shortest) {
        shortestHeight += m->getHeight() + margin;
    }
    for(std::list<Module*>* column: *columns) {
        int height = 0;
        for(Module *m: *column) {
            height += m->getHeight() + margin;
        }
        if (height < shortestHeight) {
            shortest = column;
            shortestHeight = height;
        }
    }
    shortest->push_back(m);
    std::cout << "Module Added" << std::endl;
}

void Screen::run() {
    addModule(new ModuleDatausage());
    addModule(new ModuleInvisible());
    addModule(new ModuleInvisible());
    addModule(new ModuleRam("gaming-pc"));
//    addModule(new ModuleRam("laptop"));
    addModule(new ModuleSwp("gaming-pc"));
//    addModule(new ModuleSwp("laptop"));
    addModule(new ModuleCpu("gaming-pc"));
//    addModule(new ModuleCpu("laptop"));
    addModule(new ModuleTime());
//    addModule(new ModuleTime());
//    addModule(new ModuleTime());
//    addModule(new ModuleEmpty());
//    addModule(new ModuleEmpty());
//    addModule(new ModuleTime());
//    addModule(new ModuleTime());
//    addModule(new ModuleTime());
//    addModule(new ModuleTime());


    std::cout << "All Modules added" << std::endl;

//    std::thread t(&Screen::renderLoop, std::ref(*this));
//    t.join();
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
    for(std::list<Module*>* column: *columns) {
        for(Module *m: *column) {
            m->updateSize();
        }
    }
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

        float x = margin;
        for(std::list<Module*>* column: *columns) {
            float y = margin;
            for(Module *m: *column) {
                //sf::RenderTexture* t = m->render();
                if(m->drawBackground()) {
                    sf::RectangleShape rs(sf::Vector2f(m->getWidth() + 6, m->getHeight() + 6));
                    rs.setOutlineColor(sf::Color(255, 255, 255, 64));
                    rs.setOutlineThickness(-3);
                    rs.setFillColor(sf::Color(255, 255, 255, 64));
                    rs.setPosition(x - 3, y - 3);
                    window->draw(rs);
                }
                sf::Sprite s(m->render()->getTexture());
                s.setPosition(x, y);
                window->draw(s, sf::BlendAdd);
                y += m->getHeight() + margin;
            }
            x += (view.getSize().x - (colcount + 1) * margin) / colcount + margin;
//            x += 400;
//            std::cout << view.getSize().y << std::endl;
        }
        window->display();
    }
}