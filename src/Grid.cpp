//
// Created by leon on 04.12.17.
//

#include "Grid.h"
#include "Screen.h"

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
    mask.create(w, h);
    mask.clear(sf::Color::Transparent);
    redrawMask = true;
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
			sf::RectangleShape rs(sf::Vector2f(m->getDisplayWidth(), m->getDisplayHeight()));
			rs.setPosition(m->getDisplayX() - 0, m->getDisplayY() - 0);
			if(redrawMask)
				mask.draw(rs);
			rs.setFillColor(Screen::singleton->getTheme()->getModuleBG());
			rs.setOutlineThickness(3);
			rs.setOutlineColor(Screen::singleton->getTheme()->getModuleOutline());
			t->draw(rs);
        }
        sf::Texture tex = m->render()->getTexture();
        sf::Sprite s(tex);
//        sf::Image img = tex.copyToImage();
//        sf::Color col = img.getPixel(1, 1);
//        std::cout << std::to_string(col.r) << std::endl;
        s.setPosition(m->getDisplayX(), m->getDisplayY());
        t->draw(s, sf::BlendAlpha);
    }
	redrawMask = false;
}

const sf::Texture &Grid::getMask() const {
    return mask.getTexture();
}
