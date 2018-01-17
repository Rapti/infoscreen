//
// Created by leon on 16.12.16.
//

#include <sstream>
#include "ModuleSwp.h"
#include "Screen.h"

ModuleSwp::ModuleSwp(std::string host) : ModuleSystemusage(host) {

}

ModuleSwp::~ModuleSwp() {

}

void ModuleSwp::draw() {
    int xoffset = 0;
    int yoffset = 0;
    float tlheight = getDisplayHeight() - 1.5*yoffset;
    float tlwidth = getDisplayWidth() - 2*xoffset;
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

		int padding = 10;
        sf::Text text;
        text.setFont(f);
        text.setFillColor(sf::Color(255, 255, 255, 128));

		text.setString("000.00 GB");
		text.setCharacterSize(80);
		sf::FloatRect textrect = text.getLocalBounds();
		float scaleX = (getDisplayWidth() - 2*padding) / textrect.width;
		float scaleY = (getDisplayHeight() - 2*padding) / (textrect.height);
		float scale = std::min(scaleX, scaleY) / 1.6;
		text.setCharacterSize(80 * scale);

        std::stringstream ss;
        ss << bytesToHumanReadableFormat(swp[*tlindex]);
		text.setString(ss.str());
		text.setPosition(padding, padding - 6);
		t->draw(text);
//        ss << " / ";
		ss.str(std::string());
        ss << bytesToHumanReadableFormat(*totalswp);
        mutex->unlock();


        text.setString(ss.str());
		textrect = text.getLocalBounds();
		text.setPosition(getDisplayWidth() - textrect.width - padding *2, getDisplayHeight() - textrect.height - padding*2 - 6);
        t->draw(text);
        ModuleSystemusage::draw();
    } else
        mutex->unlock();
}