//
// Created by leon on 27.07.18.
//

#include "ThemeMovingShapes.h"

ThemeMovingShapes::ThemeMovingShapes(): Theme() {
	blur = 0;
	filter.setSize(sf::Vector2f(w, h));
	filter.setFillColor(sf::Color(0, 0, 0, 16));

	ModuleBG = sf::Color(255, 255, 255, 64);
	ModuleOutline = sf::Color(255, 255, 255, 64);
	TextPrimary = sf::Color::White;
	TextSecondary = sf::Color(255, 255, 255, 128);
	TextTertiary = sf::Color(255, 255, 255, 64);
	TextError = sf::Color(255, 64, 64, 255);
	TextDisabled = sf::Color(255, 255, 255, 32);
	DiagramFill = sf::Color(255, 255, 255, 32);
	DiagramLine = sf::Color::White;
}

ThemeMovingShapes::~ThemeMovingShapes() {
	for(auto dot: shapes) {
		delete dot;
	}
}

void ThemeMovingShapes::updateDisplaySize(unsigned int w, unsigned int h) {
	Theme::updateDisplaySize(w, h);
	filter.setSize(sf::Vector2f(w, h));
}

void ThemeMovingShapes::drawBackgroundTo(sf::RenderTarget* canvas) {
	update();
	canvas->clear(bgc);
	for(auto shape: shapes) {
		canvas->draw(*shape, sf::BlendAdd);
	}
//	canvas->draw(filter);
}

void ThemeMovingShapes::update() {
	float factor = c.restart().asSeconds();
	if(shapes.empty()) generate();
	for(auto shape: shapes) {
		shape->update(factor, w, h);
	}
}

void ThemeMovingShapes::generate() {
	if(w * h == 0) return;
	while(shapes.size() < 1000) {
		shapes.push_back(new Shape(w, h));
	}
}

Shape::Shape(float screenw, float screenh) {
	reset(screenw, screenh, true);
}

Shape::~Shape() {}

void Shape::reset(float screenw, float screenh, bool anywhere) {
	float radius = rand() % 150 + 10;
	setRadius(radius);
	setOrigin(radius, radius);
	setPointCount(3);
	if(anywhere) {
		setPosition(rand() % int(screenw + 2 * radius) - radius, rand() % int(screenh + 2 * radius) - radius);
	} else {
		switch(rand() % 4) {
			case 0:
				setPosition(-radius, rand() % int(screenh + 2 * radius) - radius); break;
			case 1:
				setPosition(screenw + radius, rand() % int(screenh + 2 * radius) - radius); break;
			case 2:
				setPosition(rand() % int(screenw + 2 * radius) - radius, -radius); break;
			case 3:
				setPosition(rand() % int(screenw + 2 * radius) - radius, screenh + radius); break;
		}
	}
	sf::Uint8 brightness = 64;
//	switch(rand() % 3) {
//		case 0:
//			setFillColor(sf::Color(brightness, 0, 0));
//			break;
//		case 1:
//			setFillColor(sf::Color(0, brightness, 0));
//			break;
//		case 2:
//			setFillColor(sf::Color(0, 0, brightness));
//			break;
//		default: break;
//	}
	switch(rand() % 8) {
		case 0:
			setFillColor(sf::Color(brightness, brightness, brightness, 64));
			break;
		case 1:
			setFillColor(sf::Color(138 / 4, 43 / 4, 226 / 4, 64));
			break;
		default:
			setFillColor(sf::Color(brightness, brightness / 2, brightness / 4, 64));
	}

//	else
//		setFillColor(sf::Color(255, 255, 255, 64));
	xspeed = ((rand() % 2) * 2 - 1) * 10;
	yspeed = ((rand() % 2) * 2 - 1) * 10;
	rspeed = rand() % 10 - 5;
}

void Shape::update(float factor, float screenw, float screenh) {
	this->setRotation(this->getRotation() + rspeed * factor);
//	yspeed += yacc * factor;
	setPosition(getPosition().x + xspeed * factor, getPosition().y + yspeed * factor);
	if(getPosition().x < -getRadius() - 5 || getPosition().x > screenw + getRadius() + 5 || getPosition().y < -getRadius() - 5 || getPosition().y > screenh + getRadius() + 5) {
		reset(screenw, screenh, false);
	}
}
