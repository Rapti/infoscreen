//
// Created by leon on 16.12.16.
//

#include "ModuleSwp.h"
#include "Screen.h"

ModuleSwp::ModuleSwp(std::string host): ModuleSystemusage(host) {

}


ModuleSwp::~ModuleSwp() {

}

void ModuleSwp::draw() {
    int xoffset = 0;
    int yoffset = 0;
    float tlheight = getHeight() - 1.5*yoffset;
    float tlwidth = getWidth() - 2*xoffset;
    mutex->lock();
    if (totalswp > 0) {
        for(int i = 0; i < arrlength; ++i) {
            float s = swp[(i + 1 + *tlindex) % arrlength];
            if(s == -1)
                s = 0;
            float y = tlheight -
                      ((s / *totalswp) *
                       tlheight);
            if (y > tlheight)
                y = tlheight;
            bg[2 * i].position = sf::Vector2f(xoffset + (tlwidth / (arrlength-1)) * i, yoffset + y);
        }
        mutex->unlock();
        ModuleSystemusage::draw();
    } else
        mutex->unlock();
}