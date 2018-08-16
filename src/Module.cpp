//
// Created by leon on 02.05.16.
//

#include "Module.h"
#include "Screen.h"

std::mutex Module::fontmutex;
sf::Font Module::monospace;
sf::Font Module::natural;

Module::Module() {
    t = new sf::RenderTexture;
    t->setSmooth(true);
	if(fontmutex.try_lock()) {
		monospace.loadFromFile("/usr/share/fonts/DroidSansMono.ttf");
		natural.loadFromFile("/usr/share/fonts/WindowsFonts/arial.ttf");
	}
}

Module::~Module() {
    delete t;
}

float Module::getDisplayWidth() {
    return ((grid->getDisplayWidth() - grid->getMargin()) * ((float) w / grid->getWidth()) - grid->getMargin());
}

float Module::getDisplayHeight() {
    return ((grid->getDisplayHeight() - grid->getMargin()) * ((float) h / grid->getHeight()) - grid->getMargin());
}

bool Module::drawBackground() {return true;}

sf::RenderTexture* Module::render() {
    t->clear(sf::Color(255, 255, 255, 0));
    draw();
    t->display();
    return t;
}

void Module::updateDisplaySize() {
    t->create(getDisplayWidth(), getDisplayHeight(), sf::ContextSettings(0, 0, 4));
}

int Module::getWidth() {
    return w;
}

int Module::getHeight() {
    return h;
}

void Module::setGrid(Grid* g) {
    grid = g;
}

int Module::getX() {
    return x;
}

int Module::getY() {
    return y;
}

float Module::getDisplayX() {
    return x * ((grid->getDisplayWidth() - grid->getMargin()) / grid->getWidth()) + grid->getMargin();
}

float Module::getDisplayY() {
    return y * ((grid->getDisplayHeight() - grid->getMargin()) / grid->getHeight()) + grid->getMargin();
}

void Module::setX(int x) {
    Module::x = x;
}

void Module::setY(int y) {
    Module::y = y;
}

void Module::setWidth(int w) {
    Module::w = w;
}

void Module::setHeight(int h) {
    Module::h = h;
}
