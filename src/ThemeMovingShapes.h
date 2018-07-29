//
// Created by leon on 27.07.18.
//

#ifndef INFOSCREEN_BACKGROUNDMOVINGDOTS_H
#define INFOSCREEN_BACKGROUNDMOVINGDOTS_H


#include <SFML/Graphics/CircleShape.hpp>
#include "Theme.h"
#include <list>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Clock.hpp>

class Shape;

class ThemeMovingShapes: public Theme {
public:
	ThemeMovingShapes();

	void drawBackgroundTo(sf::RenderTarget* canvas) override;
	void update();

	~ThemeMovingShapes() override;

	void updateDisplaySize(unsigned int w, unsigned int h) override;

protected:
	std::list<Shape*> shapes;
	void generate();
	sf::RectangleShape filter;
	int corners = 3;
	int blur;
	sf::Color bgc = sf::Color::Black;
	sf::Clock c;
};

class Shape: public sf::CircleShape {
public:
	Shape(float screenw, float screenh);
	virtual ~Shape();
	void reset(float screenw, float screenh, bool anywhere);
	void update(float factor, float screenw, float screenh);
private:
	float yspeed;
	float xspeed;
	float rspeed;
};

#endif //INFOSCREEN_BACKGROUNDMOVINGDOTS_H
