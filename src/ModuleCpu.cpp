//
// Created by leon on 16.12.16.
//

#include <sstream>
#include <iomanip>
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
        int padding = 10;
        sf::Text text;
        text.setFont(f);
        text.setFillColor(sf::Color(255, 255, 255, 128));

        text.setString("100 %");
        text.setCharacterSize(80);
        sf::FloatRect textrect = text.getLocalBounds();
        float scaleX = (getDisplayWidth() - 2*padding) / textrect.width;
        float scaleY = (getDisplayHeight() - 2*padding) / (textrect.height);
        float scale = std::min(scaleX, scaleY);
        text.setCharacterSize(80 * scale);

        std::stringstream ss;
        ss << std::setw(3) << std::setfill(' ') << (int) (cpu[*tlindex] * 100) << " %";
		mutex->unlock();
        text.setString(ss.str());
        text.setPosition(0, padding - 6 - (80 * scale * 0.18));
        t->draw(text);

        ModuleSystemusage::draw();
    } else
        mutex->unlock();
}