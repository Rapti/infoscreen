//
// Created by leon on 27.07.18.
//

#ifndef INFOSCREEN_BACKGROUNDMOVINGDOTS_H
#define INFOSCREEN_BACKGROUNDMOVINGDOTS_H


#include <SFML/Graphics/CircleShape.hpp>
#include "Theme.h"
#include "GaussianBlur.h"
#include <list>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <thread>

class Shape;

enum ShapeTheme {
	THEME_1,
	THEME_2,
	THEME_3,
	THEME_4
};

class ThemeMovingShapes: public Theme {
public:
	ThemeMovingShapes(ShapeTheme st);
	ThemeMovingShapes(ShapeTheme st, std::string host, int pin_red, int pin_green, int pin_blue);

	void drawBackgroundTo(sf::RenderTarget* canvas) override;

	void update();

	~ThemeMovingShapes() override;

	void updateDisplaySize(unsigned int w, unsigned int h) override;

protected:
	std::list<Shape*> shapes;
	void generate(bool anywhere);
	sf::RectangleShape filter;
	int corners = 3;
	int blurRadius = 90;
	sf::Color bgc = sf::Color::Black;
	sf::Clock c;
	sf::RenderTexture* blurTexture1 = nullptr;
	sf::RenderTexture* blurTexture2 = nullptr;
	sf::RenderTexture* blurTexture3 = nullptr;
	sf::Shader* blurShader = nullptr;
	sf::Shader* maskShader = nullptr;
	sf::RenderStates rs;
	float shapesPer1000SquarePixels = 1;
	int requiredShapesCount();
	bool moreShapesNeeded();
	bool tooManyShapes();
	ShapeTheme st;

	void ledUpdateLoop();

	std::string host;
	int pin_red;
	int pin_blue;
	int pin_green;
	std::thread* thread;
	bool active = false;

private:
	void drawBackgroundTo(sf::RenderTarget* canvas, int blur);
};

class Shape: public sf::CircleShape {
public:
	Shape();
	virtual ~Shape();
	virtual void reset(float screenw, float screenh, bool anywhere);
	bool update(float factor, float screenw, float screenh);
	int r, g, b;
protected:
	float yspeed;
	float xspeed;
	float rspeed;
	bool invalid;
public:
	bool isInvalid() const;

public:
	void setYspeed(float yspeed);

	void setXspeed(float xspeed);

	void setRspeed(float rspeed);
};

class ShapeStyle1: public Shape {
public:
	ShapeStyle1(float w, float h): Shape() {};
	void reset(float screenw, float screenh, bool anywhere);
};

class ShapeStyle2: public Shape {
public:
	ShapeStyle2(float w, float h): Shape() {};
	void reset(float screenw, float screenh, bool anywhere);
};

class ShapeStyle3: public Shape {
public:
	ShapeStyle3(float w, float h): Shape() {};
	void reset(float screenw, float screenh, bool anywhere);
};

class ShapeStyle4: public Shape {
public:
	ShapeStyle4(float w, float h): Shape() {};
	void reset(float screenw, float screenh, bool anywhere);
	static sf::Time getClockTime();
private:
	static sf::Clock clock;
};







#endif //INFOSCREEN_BACKGROUNDMOVINGDOTS_H
