//
// Created by leon on 02.05.16.
//

#include "Module.h"
#include "Screen.h"

Module::Module() {
    t = new sf::RenderTexture;
    w = 500;
    h = defaultHeight;
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    t->create(getWidth(), getHeight());
    t->setSmooth(true);
    t->display();
    f.loadFromFile("res/fonts/DroidSansMono.ttf");
}

Module::~Module() {
    delete t;
}

float Module::getWidth() {
//    return 550;
//    return (Screen::singleton->view.getSize().x - (2 * Screen::singleton->margin)) / Screen::singleton->colcount;
    return (Screen::singleton->view.getSize().x - (Screen::singleton->colcount + 1) * Screen::singleton->margin) / Screen::singleton->colcount;
}

float Module::getHeight() {
    return h;
}

bool Module::drawBackground() {return true;}

sf::RenderTexture* Module::render() {
    //t->create(getWidth(), getHeight());
    t->display();
    t->clear(sf::Color::Transparent);
//    sf::RectangleShape rs(sf::Vector2f(getWidth() + 6, getHeight() + 6));
//    rs.setOutlineColor(sf::Color(255, 255, 255, 255));
//    rs.setOutlineThickness(-3);
//    rs.setFillColor(sf::Color(255, 255, 255, 127));
//    rs.setPosition(x - 3, y - 3);
//    t->draw(rs, sf::BlendMultiply);
    draw();
    return t;
}

void Module::updateSize() {
    t->create(getWidth(), getHeight());
}