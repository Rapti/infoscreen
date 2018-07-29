//
// Created by leon on 27.07.18.
//

#ifndef INFOSCREEN_THEMEDEFAULT_H
#define INFOSCREEN_THEMEDEFAULT_H


#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include "Theme.h"

class ThemeDefault: public Theme {
public:
	ThemeDefault();
	void drawBackgroundTo(sf::RenderTarget* canvas);

	void updateDisplaySize(unsigned int w, unsigned int h) override;

private:
	sf::Texture bgt;
	sf::Sprite bgs;
};


#endif //INFOSCREEN_THEMEDEFAULT_H
