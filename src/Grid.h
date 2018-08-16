//
// Created by leon on 04.12.17.
//

#ifndef INFOSCREEN_GRID_H
#define INFOSCREEN_GRID_H


#include "Module.h"
#include <iostream>
#include <list>

class Module;

class Grid {

private:
public:
    int getWidth();
    int getHeight();
    float getDisplayWidth();
    float getDisplayHeight();
    void addModule(Module *, int x, int y, int w, int h);
    void updateDisplaySize(int w, int h);
    void drawTo(sf::RenderTarget *);
	const sf::Texture &getMask() const;

    Grid(int, int);
    ~Grid();

	int getMargin() const;

	void setMargin(int margin);

private:
    int gridWidth = 0;
    int gridHeight = 0;
    int displayWidth = 0;
    int displayHeight = 0;
    int margin = 16;
    sf::RenderTexture mask;
	bool redrawMask = true;

    std::list<Module*> modules;
};


#endif //INFOSCREEN_GRID_H
