//
// Created by leon on 27.07.18.
//

#include <SFML/Graphics/Texture.hpp>
#include "ThemeDefault.h"

ThemeDefault::ThemeDefault(): Theme() {
	 ModuleBG = sf::Color(255, 255, 255, 64);
	 ModuleOutline = sf::Color(255, 255, 255, 64);
	 TextPrimary = sf::Color::White;
	 TextSecondary = sf::Color(255, 255, 255, 128);
	 TextTertiary = sf::Color(255, 255, 255, 64);
	 TextError = sf::Color(255,0,0,255);
	 TextDisabled = sf::Color(255, 255, 255, 32);
	 DiagramFill = sf::Color(255, 255, 255, 32);
	 DiagramLine = sf::Color::White;

	 bgt.loadFromFile("/usr/share/images/Amazing-night-sky-blurred.jpg");
	 bgs.setTexture(bgt);

//	 ModuleBG = sf::Color(0,0,0,0);
//	 ModuleOutline = sf::Color(0,0,0,0);
//	 TextPrimary = sf::Color(0,0,0,0);
//	 TextSecondary = sf::Color(0,0,0,0);
//	 TextError = sf::Color(0,0,0,0);
//	 TextDisabled = sf::Color(0,0,0,0);
//	 DiagramFill = sf::Color(0,0,0,0);
//	 DiagramLine = sf::Color(0,0,0,0);
}

void ThemeDefault::drawBackgroundTo(sf::RenderTarget* rt) {
	rt->draw(bgs);
}

void ThemeDefault::updateDisplaySize(unsigned int w, unsigned int h) {
	Theme::updateDisplaySize(w, h);

	float scale = 0;
	float scaleX = (float) w / bgt.getSize().x;
	float scaleY = (float) h / bgt.getSize().y;
	if(scaleX > scaleY)
		scale = scaleX;
	else scale = scaleY;
	bgs.setScale(scale, scale);
}
