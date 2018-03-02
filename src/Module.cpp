//
// Created by leon on 02.05.16.
//

#include "Module.h"
#include "Screen.h"

Module::Module() {
    t = new sf::RenderTexture;
//    sf::ContextSettings settings;
//    settings.antialiasingLevel = 8;

    t->setSmooth(true);
    t->display();
    f.loadFromFile("res/fonts/DroidSansMono.ttf");
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
    t->clear(sf::Color::Transparent);
    draw();
    t->display();
    return t;
}

void Module::updateDisplaySize() {
    std::cout << "Updating Module display size to " << getDisplayWidth() << "x" << getDisplayHeight() << std::endl;
    t->create(getDisplayWidth(), getDisplayHeight());
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
