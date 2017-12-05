//
// Created by leon on 16.12.16.
//

#include "ModuleCpu.h"
#include "Screen.h"

ModuleCpu::ModuleCpu(std::string host) : ModuleSystemusage(host) {
    normalizationValues = new int[normalizationTime / updateInterval];
}


ModuleCpu::~ModuleCpu() {
    delete normalizationValues;
}

void ModuleCpu::draw() {
    int xoffset = 0;
    int yoffset = 0;
    float tlheight = getDisplayHeight() - 1.5*yoffset;
    float tlwidth = getDisplayWidth() - 2*xoffset;

    mutex->lock();
    if (totalcpu > 0) {
        for(int i = 0; i < arrlength; ++i) {
            float cputime = 0;
            if(cpu[(i + 1 + *tlindex) % arrlength] == -1)
                cputime = 0;
            else {
                int a = 0;
                for (int j = 0; j < normalizationTime / updateInterval && i - j >= 0; ++j) {
                    if (cpu[(i + 1 + *tlindex - j) % arrlength] != -1) {
                        cputime += cpu[(i + 1 + *tlindex - j) % arrlength];
                        ++a;
                    }
                }
                cputime /= a;
            }
            float y = tlheight -
                      ((cputime / *totalcpu) *
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