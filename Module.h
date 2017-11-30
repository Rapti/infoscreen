//
// Created by leon on 02.05.16.
//

#ifndef INFOSCREEN_MODULE_H
#define INFOSCREEN_MODULE_H

#include <SFML/Graphics.hpp>

class Module {
public:
    Module();
    virtual ~Module();

    sf::RenderTexture* t;
    sf::RenderTexture* render();

    virtual float getWidth();
    virtual float getHeight();
    virtual bool drawBackground();

    void updateSize();

    const int defaultHeight = 128;

protected:
    int x, y, w, h;
    virtual void draw() = 0;
    int initialsecs = 0;
    sf::Font f;
    bool drawBg = true;
};


#endif //INFOSCREEN_MODULE_H
