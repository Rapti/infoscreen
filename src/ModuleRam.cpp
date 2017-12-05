//
// Created by leon on 16.12.16.
//

#include "ModuleRam.h"
#include "Screen.h"

ModuleRam::ModuleRam(std::string host) : ModuleSystemusage(host) {

}


ModuleRam::~ModuleRam() {

}

void ModuleRam::draw() {
    int xoffset = 0;
    int yoffset = 0;
    float tlheight = getDisplayHeight() - 1.5*yoffset;
    float tlwidth = getDisplayWidth() - 2*xoffset;

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
}