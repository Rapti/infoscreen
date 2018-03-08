//
// Created by leon on 04.12.17.
//

#include "Grid.h"

Grid::Grid(int w, int h) {
    gridWidth = w;
    gridHeight = h;
}

Grid::~Grid() {
    for(Module* m: modules) {
        delete m;
    }
}

int Grid::getWidth() {
    return gridWidth;
}

int Grid::getHeight() {
    return gridHeight;
}

void Grid::addModule(Module *m, int x, int y, int w, int h) {
    modules.push_back(m);
    m->setX(x);
    m->setY(y);
    m->setWidth(w);
    m->setHeight(h);
    m->setGrid(this);
}

float Grid::getDisplayWidth() {
    return displayWidth;
}

float Grid::getDisplayHeight() {
    return displayHeight;
}

void Grid::updateDisplaySize(int w, int h) {
//    std::cout << "Updating Grid display size to " << w << "x" << h << std::endl;
    displayWidth = w;
    displayHeight = h;
    for(Module* m: modules) {
        m->updateDisplaySize();
    }
}

int Grid::getMargin() const {
    return margin;
}

void Grid::setMargin(int margin) {
    Grid::margin = margin;
}

void Grid::drawTo(sf::RenderTarget *t) {
    for(Module* m: modules) {
        if(m->drawBackground()) {
            sf::RectangleShape rs(sf::Vector2f(m->getDisplayWidth() + 6, m->getDisplayHeight() + 6));
            rs.setOutlineColor(sf::Color(255, 255, 255, 64));
            rs.setOutlineThickness(-3);
            rs.setFillColor(sf::Color(255, 255, 255, 64));
            rs.setPosition(m->getDisplayX() - 3, m->getDisplayY() - 3);
            t->draw(rs);
        }
        sf::Sprite s(m->render()->getTexture());
        s.setPosition(m->getDisplayX(), m->getDisplayY());
        t->draw(s, sf::BlendAdd);
    }
}
