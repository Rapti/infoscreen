//
// Created by leon on 22.07.18.
//

#include "Background.h"

Background::Background() {
	w = h = 0;
	c.restart();
}

Background::~Background() = default;

void Background::renderTo(sf::RenderTarget* canvas) {
	if(w * h == 0) return; // don't do anything if width or height is 0
	update(c.restart().asMicroseconds() / 1000000F);
	render(canvas);
}

void Background::updateDisplaySize(unsigned int w, unsigned int h) {
	this->w = w;
	this->h = h;
}
