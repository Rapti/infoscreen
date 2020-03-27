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
#include "ModuleLabel.h"
#include "ModuleDiskusage.h"


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
	window->setPosition(sf::Vector2i(0,0));
    g = new Grid(60, 60);
    t = new ThemeDefault();
    t = new ThemeMovingShapes(ShapeTheme::THEME_4, "dottie", 17, 22, 24);

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
//    g->addModule(new ModuleDatausage(), 12, 43, 48, 7);
	g->addModule(new ModuleLabel("Pumpkin"), 48, 15, 12, 4);
    g->addModule(new ModuleCpu("pumpkin"), 48, 19, 12, 8);
    g->addModule(new ModuleRam("pumpkin"), 48, 27, 12, 8);
    g->addModule(new ModuleSwp("pumpkin"), 48, 35, 12, 8);
	g->addModule(new ModuleLabel("Fred"), 36, 15, 12, 4);
    g->addModule(new ModuleCpu("fred"),    36, 19, 12, 8);
    g->addModule(new ModuleRam("fred"),    36, 27, 12, 8);
    g->addModule(new ModuleSwp("fred"),    36, 35, 12, 8);
	g->addModule(new ModuleLabel("Tabitha"), 24, 15, 12, 4);
	g->addModule(new ModuleCpu("tabitha"), 24, 19, 12, 8);
	g->addModule(new ModuleRam("tabitha"), 24, 27, 12, 8);
	g->addModule(new ModuleSwp("tabitha"), 24, 35, 12, 8);
	g->addModule(new ModuleLabel("Dottie"), 12, 15, 12, 4);
    g->addModule(new ModuleCpu("dottie"),  12, 19, 12, 8);
    g->addModule(new ModuleRam("dottie"),  12, 27, 12, 8);
    g->addModule(new ModuleSwp("dottie"),  12, 35, 12, 8);
    g->addModule(new ModuleTime(), 12, 0, 24, 15);
    g->addModule(new ModuleTimer(), 36, 0, 24, 15);
    g->addModule(new ModulePing({"pumpkin", "dottie", "server.raptilic.us", "leons-telefon", "google.de"}, {"Pumpkin", "Dottie", "Tabitha", "Telefon", "Internet"}), 48, 50, 12, 10);
    g->addModule(new ModulePublicTransitStop(20000131, {"S-Bahn", "R-Bahn"}), 0, 0, 12, 60); // Dortmund HBF
    g->addModule(new ModulePublicTransitStop(20000454), 12, 43, 12, 17); // Dortmund Leopoldstraße
    g->addModule(new ModulePublicTransitStop(20000362), 24, 43, 12, 17); // Dortmund Nordmarkt
    g->addModule(new ModuleDiskusage("pumpkin", {"/"}), 36, 43, 12, 17);

    // Stop ID herausfinden:
    // http://openservice-test.vrr.de/static02/XML_STOPFINDER_REQUEST?sessionID=0&requestID=0&outputFormat=JSON&anguage=DE&type_sf=stop&place_sf=Dortmund&name_sf= [Name]

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
            if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
				window->close();
				delete g;
			} else if (event.type == sf::Event::Resized) {
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
