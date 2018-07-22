//
// Created by leon on 22.07.18.
//

#ifndef INFOSCREEN_BACKGROUND_H
#define INFOSCREEN_BACKGROUND_H


#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Clock.hpp>

class Background {
public:
	Background();
	virtual ~Background();
	void renderTo(sf::RenderTarget* canvas);
	void updateDisplaySize(unsigned int w, unsigned int h);
protected:
	unsigned int w, h;
	sf::Clock c;
	virtual void update(float factor) = 0;
	virtual void render(sf::RenderTarget* canvas) = 0;
};


#endif //INFOSCREEN_BACKGROUND_H
