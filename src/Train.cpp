//
// Created by leon on 19.12.17.
//

#include <SFML/Graphics/Text.hpp>
#include <cmath>
#include "Train.h"

Train::Train() {
	delay = 0;
	cancelled = false;
}

Train::~Train() {

}

time_t Train::getDeparture() const {
	return departure;
}

void Train::setDeparture(time_t departure) {
	Train::departure = departure;
}

int Train::getDelay() const {
	return delay;
}

void Train::setDelay(int delay) {
	Train::delay = delay;
}

const std::string &Train::getPlatform() const {
	return platform;
}

void Train::setPlatform(const std::string &platform) {
	Train::platform = platform;
}

const std::string &Train::getName() const {
	return name;
}

void Train::setName(const std::string &name) {
	Train::name = name;
}

const std::string &Train::getDestination() const {
	return destination;
}

void Train::setDestination(const std::string &destination) {
	Train::destination = destination;
}

sf::RenderTexture* Train::render(sf::Font f, unsigned int w) {

	unsigned int h = w / 2;
	t.clear(sf::Color::Transparent);
	if(t.getSize().x != w) {
		t.create(w, h);
	}

	const int testSize = 80;
	const float ypadding = 0.1;
	const int padding = 5;
	sf::Text text;
	text.setFont(f);
	text.setCharacterSize(testSize);
	text.setString("HB1 Dortmund Eichlinghofen H-Bahn");
	float tw = text.getLocalBounds().width;


	float scale = (w - 2*padding) / tw;
	float textSize = testSize * scale;
	text.setCharacterSize(textSize);

	text.setString(getName());
	text.setPosition(padding, padding);
	t.draw(text);

	return &t;
}

bool Train::isDelayed() {
	return delay != 0;
}
bool Train::isCancelled() {
	return cancelled;
}
