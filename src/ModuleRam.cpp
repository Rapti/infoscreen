//
// Created by leon on 16.12.16.
//

#include "ModuleRam.h"
#include "Screen.h"

ModuleRam::ModuleRam(std::string host): ModuleSystemusage(host) {

}


ModuleRam::~ModuleRam() {

}

void ModuleRam::draw() {
    int xoffset = 0;
    int yoffset = 0;
    float tlheight = getHeight() - 1.5*yoffset;
    float tlwidth = getWidth() - 2*xoffset;

//    sf::RectangleShape r(sf::Vector2f(tlwidth, tlheight));
//    r.setOutlineColor(sf::Color::White);
//    r.setOutlineThickness(-3);
//    r.setFillColor(sf::Color::Transparent);
//
//    r.setPosition(xoffset, yoffset);
//    t->draw(r);
    mutex->lock();
    if (totalmem > 0) {
        for(int i = 0; i < arrlength; ++i) {
            float m = mem[(i + 1 + *tlindex) % arrlength];
            if(m == -1)
                m = 0;
            float y = tlheight -
                      ((m / *totalmem) *
                       tlheight);
            if (y > tlheight)
                y = tlheight;
            bg[2 * i].position = sf::Vector2f(xoffset + (tlwidth / (arrlength-1)) * i, yoffset + y);
        }
        mutex->unlock();
        ModuleSystemusage::draw();
    } else
        mutex->unlock();
//    yoffset += tlheight + 10;
//    r.setPosition(xoffset, yoffset);
//    t->draw(r);
//    if (totalswp > 0) {
//        for(int i = 0; i < arrlength; ++i) {
//            float y = tlheight -
//                      (((float) swp[(i + *tlindex) % arrlength] / *totalswp) *
//                       tlheight);
//            if (y > tlheight)
//                y = tlheight;
//            swpv[2 * i].position = sf::Vector2f(xoffset + (tlwidth / arrlength) * i, yoffset + y);
//            swpv[2*i+1].position = sf::Vector2f(xoffset + (tlwidth / arrlength) * i, yoffset + tlheight);
//            swpl[2 * i].position = sf::Vector2f(xoffset + (tlwidth / (arrlength-1)) * i, yoffset + y);
//            swpl[2*i+1].position = sf::Vector2f(xoffset + (tlwidth / (arrlength-1)) * i, yoffset + y - 3);
//            swpv[2 * i].color = c;
//            swpv[2*i+1].color = c;
//            swpl[2 * i].color = sf::Color::White;
//            swpl[2*i+1].color = sf::Color::White;
//        }
//        t->draw(swpv);
//        t->draw(swpl);
//    }
}