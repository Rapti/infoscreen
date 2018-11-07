//
// Created by leon on 11.03.16.
//

#include "Screen.h"
#include "ModuleTime.h"
#include "ModuleTimer.h"
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

    g = new Grid(3, 5);
    t = new ThemeDefault();
    t = new ThemeMovingShapes(ShapeTheme::THEME_4);

}
Screen::~Screen() {
    delete window;
    delete g;
    delete t;
}

sf::RenderWindow* Screen::getWindow() {
    return window;
}




void Screen::run() {
//    g->addModule(new ModuleDatausage(), 0, 0, 3, 1);
    g->addModule(new ModuleRam("leons-pc"), 2, 1, 1, 1);
    g->addModule(new ModuleSwp("leons-pc"), 2, 2, 1, 1);
    g->addModule(new ModuleCpu("leons-pc"), 2, 3, 1, 1);
    g->addModule(new ModuleRam("conciso-laptop-leon"), 1, 2, 1, 1);
//    g->addModule(new ModuleSwp("conciso-laptop-leon"), 1, 3, 1, 1);
    g->addModule(new ModuleCpu("conciso-laptop-leon"), 1, 3, 1, 1);
    g->addModule(new ModuleTime(), 1, 0, 1, 1);
    g->addModule(new ModuleTimer(), 1, 1, 1, 1);
    g->addModule(new ModulePing({"10.4.12.200", "leons-pc", "leons-telefon", "server.raptilic.us"}, {"Bluelou", "Rechner", "Telefon", "Kimsufi"}), 2, 0, 1, 1);
    g->addModule(new ModulePublicTransitStop("Dortmund", "Universität S"), 0, 0, 1, 5);


    std::cout << "Startup finished" << std::endl;
    renderLoop();
}

void Screen::updateSize() {
    g->updateDisplaySize(view.getSize().x, view.getSize().y);
    t->updateDisplaySize(view.getSize().x, view.getSize().y);
}

void Screen::renderLoop() {
    updateSize();
    sf::Color bgc(0,0,128); // Fallback background color
//    ThemeMovingShapes background;
    while (window->isOpen()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event{};
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


			for (auto &listener : listeners) {
				listener->onEvent(event);
			}
        }

        window->clear(bgc);
		t->drawBackgroundTo(window);
        g->drawTo(window);
        window->display();
    }
}

void Screen::addEventListener(EventListener* e) {
	listeners.push_back(e);
}
void Screen::removeEventListener(EventListener* e) {
	for(std::vector<EventListener*>::iterator i = listeners.begin(); i != listeners.end(); ) {
		if(*i == e) {
			listeners.erase(i);
		} else
			++i;
	}
}

Theme* Screen::getTheme() {
    return t;
}

Grid* Screen::getGrid() {
    return g;
}
