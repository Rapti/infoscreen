//
// Created by leon on 02.05.16.
//

#ifndef INFOSCREEN_MODULE_H
#define INFOSCREEN_MODULE_H

#include <SFML/Graphics.hpp>
#include "Grid.h"

class Grid;

class Module {
public:
    Module();
    virtual ~Module();

    sf::RenderTexture* t;
    sf::RenderTexture* render();

    int getWidth();
    int getHeight();
    float getDisplayWidth();
    float getDisplayHeight();
    int getX();
    int getY();
    float getDisplayX();
    float getDisplayY();

    virtual bool drawBackground();

    void updateDisplaySize();

    void setGrid(Grid*);

    const int defaultHeight = 128;

protected:
public:
    void setX(int x);

    void setY(int y);

    void setWidth(int w);

    void setHeight(int h);

protected:
    int x, y, w, h;
    virtual void draw() = 0;
    int initialsecs = 0;
    sf::Font f;
    bool drawBg = true;
    Grid* grid;
};


#endif //INFOSCREEN_MODULE_H
