//
// Created by leon on 27.07.18.
//

#include <cmath>
#include <iostream>
#include <SFML/Graphics/Sprite.hpp>
#include <sstream>
#include "ThemeMovingShapes.h"
#include "Screen.h"
#include "main.h"


// hue: 0-360°; sat: 0.f-1.f; val: 0.f-1.f
sf::Color hsv(int hue, float sat, float val) {
	hue %= 360;
	while(hue<0) hue += 360;

	if(sat<0.f) sat = 0.f;
	if(sat>1.f) sat = 1.f;

	if(val<0.f) val = 0.f;
	if(val>1.f) val = 1.f;

	int h = hue/60;
	float f = float(hue)/60-h;
	float p = val*(1.f-sat);
	float q = val*(1.f-sat*f);
	float t = val*(1.f-sat*(1-f));

	switch(h) {
		default:
		case 0:
		case 6: return sf::Color(val*255, t*255, p*255);
		case 1: return sf::Color(q*255, val*255, p*255);
		case 2: return sf::Color(p*255, val*255, t*255);
		case 3: return sf::Color(p*255, q*255, val*255);
		case 4: return sf::Color(t*255, p*255, val*255);
		case 5: return sf::Color(val*255, p*255, q*255);
	}
}

ThemeMovingShapes::ThemeMovingShapes(ShapeTheme st) : Theme() {
	this->st = st;

//	blurRadius = 0;
	filter.setSize(sf::Vector2f(w, h));
	int filterstrength = 200;
	filter.setFillColor(sf::Color(filterstrength, filterstrength, filterstrength));


//	ModuleBG = sf::Color(255, 255, 255, 48);
	ModuleBG = sf::Color(0, 0, 0, /*48*/ 0);
//	ModuleBG = sf::Color(255, 255, 255, 0);
//	ModuleOutline = sf::Color(255, 255, 255, 48);
	ModuleOutline = sf::Color(255, 255, 255, 0);
	TextPrimary = sf::Color::White;
	TextSecondary = sf::Color(255, 255, 255, 128);
	TextTertiary = sf::Color(255, 255, 255, 64);
	TextError = sf::Color(255,0,0,255);
	TextDisabled = sf::Color(255, 255, 255, 32);
	DiagramFill = sf::Color(255, 255, 255, 32);
	DiagramLine = sf::Color::White;

//	ModuleBG = sf::Color(0, 0, 0, 16);
//	ModuleOutline = sf::Color(0, 0, 0, 16);
//	TextPrimary = sf::Color::Black;
//	TextSecondary = sf::Color(0, 0, 0, 128);
//	TextTertiary = sf::Color(0, 0, 0, 64);
//	TextError = sf::Color(255, 64, 64, 255);
//	TextDisabled = sf::Color(0, 0, 0, 32);
//	DiagramFill = sf::Color(0, 0, 0, 64);
//	DiagramLine = sf::Color::Black;

}

ThemeMovingShapes::ThemeMovingShapes(ShapeTheme st, std::string host, int pin_red, int pin_green, int pin_blue): ThemeMovingShapes(st) {
	this->host = host;
	this->pin_red = pin_red;
	this->pin_green = pin_green;
	this->pin_blue = pin_blue;
	active = true;
	thread = new std::thread(&ThemeMovingShapes::ledUpdateLoop, std::ref(*this));
}


ThemeMovingShapes::~ThemeMovingShapes() {
	active = false;
	for(auto shape: shapes) {
		delete shape;
	}
	delete blurShader;
	delete maskShader;
	delete blurTexture1;
	delete blurTexture2;
	delete blurTexture3;
}

void ThemeMovingShapes::ledUpdateLoop() {
	while(active) {
		std::this_thread::sleep_for(std::chrono::milliseconds(5000));

		int r = 0, g = 0, b = 0;

		if (shapes.size() > 0) {
			for (auto shape: shapes) {
				r += shape->r;
				g += shape->g;
				b += shape->b;
			}
			r /= shapes.size();
			g /= shapes.size();
			b /= shapes.size();
		}
		std::stringstream ss;
		ss << "ssh -x " << host;
		ss << " pigs p " << pin_red << " " << r << "\\; ";
		ss << " pigs p " << pin_green << " " << g << "\\; ";
		ss << " pigs p " << pin_blue << " " << b;
		exec(ss.str().c_str());
	}
}

void ThemeMovingShapes::updateDisplaySize(unsigned int w, unsigned int h) {
	Theme::updateDisplaySize(w, h);
	filter.setSize(sf::Vector2f(w, h));
	if(blurTexture1 != nullptr)
		blurTexture1->create(w, h, sf::ContextSettings(0, 0, 8));
	if(blurTexture2 != nullptr)
		blurTexture2->create(w, h);
	if(blurTexture3 != nullptr)
		blurTexture3->create(w, h);
	generate(true);
}

void ThemeMovingShapes::drawBackgroundTo(sf::RenderTarget* canvas) {
	update();
	drawBackgroundTo(canvas, blurRadius);
}

void ThemeMovingShapes::drawBackgroundTo(sf::RenderTarget* canvas, int blurRadius) {
	if(w * h == 0) return; // don't do anything if width or height is 0
 	if(blurRadius == 0) {
		canvas->clear(bgc);
		for(auto shape: shapes) {
			canvas->draw(*shape, sf::BlendAdd);
		}
		canvas->draw(filter, sf::BlendMultiply);
	} else {
		if(blurTexture1 == nullptr) {
			blurTexture1 = new sf::RenderTexture;
			blurTexture1->create(w, h, sf::ContextSettings(0, 0, 8));
		}
		if(blurTexture2 == nullptr) {
			blurTexture2 = new sf::RenderTexture;
			blurTexture2->create(w, h, sf::ContextSettings(0, 0, 0));
		}
		if(blurTexture3 == nullptr) {
			blurTexture3 = new sf::RenderTexture;
			blurTexture3->create(w, h, sf::ContextSettings(0, 0, 0));
		}
		if(blurShader == nullptr) {
			blurShader = new sf::Shader;
			blurShader->loadFromMemory(GenerateGaussFunctionCode(int(std::floor((blurRadius + 1) / 4) * 4 - 1), true), sf::Shader::Fragment);
			blurShader->setUniform("texture", sf::Shader::CurrentTexture);
			rs.shader = blurShader;
		}
		if(maskShader == nullptr) {
			maskShader = new sf::Shader;
			maskShader->loadFromMemory("uniform sampler2D source;\n"
							  "uniform sampler2D mask;\n"
		                      "void main() {\n"
						"vec4 sourceColor = texture2D(source, gl_TexCoord[0].xy);\n"
	  "float maskAlpha = texture2D(mask, gl_TexCoord[0].xy).a;\n"
                             	 "gl_FragColor = vec4(sourceColor.rgb, sourceColor.a * maskAlpha);\n"
//                             	 "gl_FragColor = texture2D(mask, gl_TexCoord[0].xy);\n"
					 		  "}", sf::Shader::Fragment);
			maskShader->setUniform("source", sf::Shader::CurrentTexture);
		}
		drawBackgroundTo(blurTexture1, 0);
		blurTexture1->display();
		sf::Sprite s;
		s.setTexture(blurTexture1->getTexture());
		canvas->draw(s);
		blurShader->setUniform("dir", sf::Vector2f(1, 0));
		blurShader->setUniform("size", (float) w);
		blurTexture2->draw(s, rs);
		blurTexture2->display();
		s.setTexture(blurTexture2->getTexture());
		blurShader->setUniform("dir", sf::Vector2f(0, 1));
		blurShader->setUniform("size", (float) h);
		blurTexture3->draw(s, rs);
		blurTexture3->display();
		s.setTexture(blurTexture3->getTexture());
		maskShader->setUniform("mask", Screen::singleton->getGrid()->getMask());
		canvas->draw(s, maskShader);
	}
}



void ThemeMovingShapes::update() {
	float factor = c.restart().asSeconds();
	for(auto i =  shapes.begin(); i != shapes.end();) {
		Shape* s = *i;
		s->update(factor, w, h);
		if(s->isInvalid()) {
			delete s;
			i = shapes.erase(i);
		} else {
			++i;
		}
	}
	generate(false);
}

void ThemeMovingShapes::generate(bool anywhere) {
	if(w * h == 0) return;
	while(moreShapesNeeded()) { // roughly one shape per 1000 square pixels
		Shape* s;
		switch(st) {
			case THEME_1:
//				blurRadius = 400;
				shapesPer1000SquarePixels = 2;
				s = new ShapeStyle1(w, h); break;
			case THEME_2:
				s = new ShapeStyle2(w, h); break;
			case THEME_3:
				s = new ShapeStyle3(w, h); break;
			case THEME_4:
				shapesPer1000SquarePixels = 1;
				s = new ShapeStyle4(w, h); break;
		}
		s->reset(w, h, anywhere);
		shapes.push_back(s);
	}
}

int ThemeMovingShapes::requiredShapesCount() {
	return w*h / 1000 * shapesPer1000SquarePixels;
}

bool ThemeMovingShapes::moreShapesNeeded() {
	return shapes.size() < requiredShapesCount();
}

bool ThemeMovingShapes::tooManyShapes() {
	return shapes.size() > requiredShapesCount();
}

Shape::Shape() {
	invalid = false;
}

Shape::~Shape() {}

void Shape::reset(float screenw, float screenh, bool anywhere) {
	float radius = rand() % 150 + 10;
	setRadius(radius);
	setRotation(rand() % 360);
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

	float r = rand() / (RAND_MAX / 1.0);
//	std::cout << sin(r * M_2_PI) << std::endl;
	xspeed = sin(r * M_PI * 2) * 10;
	yspeed = cos(r * M_PI * 2) * 10;

//	setXspeed(((rand() % 2) * 2 - 1) * 10);
//	setYspeed(((rand() % 2) * 2 - 1) * 10);
	setRspeed(rand() % 10 - 5);
}

bool Shape::update(float factor, float screenw, float screenh) {
	this->rotate(rspeed * factor);
	move(xspeed * factor, yspeed * factor);
	if(getPosition().x < -getRadius() - 5 || getPosition().x > screenw + getRadius() + 5 || getPosition().y < -getRadius() - 5 || getPosition().y > screenh + getRadius() + 5) {
		invalid = true;
	}
	return true;
}

void Shape::setYspeed(float yspeed) {
	Shape::yspeed = yspeed;
}

void Shape::setXspeed(float xspeed) {
	Shape::xspeed = xspeed;
}

void Shape::setRspeed(float rspeed) {
	Shape::rspeed = rspeed;
}

bool Shape::isInvalid() const {
	return invalid;
}


void ShapeStyle1::reset(float screenw, float screenh, bool anywhere) {
	float brightness = 0.1;
	switch(rand() % 3) {
		case 0:
			setFillColor(sf::Color(255 * brightness, 0, 0));
			break;
		case 1:
			setFillColor(sf::Color(0, 255 * brightness, 0));
			break;
		case 2:
			setFillColor(sf::Color(0, 0, 255 * brightness));
			break;
		default:
			break;
	}
	Shape::reset(screenw, screenh, anywhere);
}

void ShapeStyle2::reset(float screenw, float screenh, bool anywhere) {
	float brightness = 0.25;
	switch (rand() % 8) {
		case 0:
			setFillColor(sf::Color(64, 64, 64, 64));
			break;
		case 1:
			setFillColor(sf::Color(138 * brightness, 43 * brightness, 226 * brightness, 64));
			break;
		default:
			setFillColor(sf::Color(64, 64 * (brightness * 2), 64 * brightness, 64));
			break;
	}
	Shape::reset(screenw, screenh, anywhere);
}

void ShapeStyle3::reset(float screenw, float screenh, bool anywhere) {
	float brightness = 0.1;
	sf::Color c;
	sf::Color b(255 * brightness, 255 * brightness, 255 * brightness);
	switch (rand() % 8) {
		case 0:
			c = sf::Color(64, 64, 255);
			break;
		case 1:
			c = sf::Color(64, 64, 255);
			break;
		default:
			c = sf::Color(64, 64, 255);
			break;
	}
	setFillColor(c * b);
	Shape::reset(screenw, screenh, anywhere);
	setPointCount(4);
}

sf::Clock ShapeStyle4::clock;
void ShapeStyle4::reset(float screenw, float screenh, bool anywhere) {
	float brightness = 0.1;

	float hue = clock.getElapsedTime().asSeconds() / -2.5;

	if(anywhere) {
		hue = rand() % 360;
		brightness = 0.15;
	}

	sf::Color c = hsv(hue, 1, 1);
	r = c.r;
	g = c.g;
	b = c.b;

	c = hsv(hue, 1, brightness);
//	std::cout << clock.getElapsedTime().asMilliseconds() << std::endl;
//	sf::Color b(255 * brightness, 255 * brightness, 255 * brightness);
	setFillColor(c);
	Shape::reset(screenw, screenh, anywhere);
	xspeed *= 0.5;
	yspeed *= 0.5;
}

sf::Time ShapeStyle4::getClockTime() {
	return clock.getElapsedTime();
}
