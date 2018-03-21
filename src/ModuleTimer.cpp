//
// Created by leon on 08.03.18.
//

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Text.hpp>
#include <sstream>
#include <iomanip>
#include "ModuleTimer.h"

ModuleTimer::ModuleTimer() {
}

ModuleTimer::~ModuleTimer() {

}

void ModuleTimer::reset() {
	n = 0;
}

void ModuleTimer::add(int x) {
	if(lastInput.getElapsedTime().asSeconds() > 3)
		reset();
	n = 10 * n + x;
	if(n == x)
		c.restart();
	lastInput.restart();
	updateTarget();
}

void ModuleTimer::updateTarget() {

}

void ModuleTimer::draw() {
	sf::Text text;
	text.setFont(monospace);

	std::stringstream ss;

	int padding = 10;
	int testsize = 80;

	text.setCharacterSize(testsize);
	text.setString("00:00:00:00");
	sf::FloatRect textrect = text.getLocalBounds();
	float scaleX = (getDisplayWidth() - 2*padding) / textrect.width;
	float scaleY = (getDisplayHeight() - 2*padding) / (textrect.height);
	float scale = std::min(scaleX, scaleY);
	text.setCharacterSize(testsize * scale);
	text.setFillColor(sf::Color::White);

	int h, m, s, ms;

	int x = n;
	h = x / 10000; x %= 10000;
	m = x / 100; x %= 100;
	s = x;
	ms = 0;

	if(n == 0 || lastInput.getElapsedTime().asSeconds() > 3) {
		int target = (s + 60 * m + 3600 * h) * 1000;
		int elapsed = c.getElapsedTime().asMilliseconds();
		if(elapsed > target) {
			h = m = s = ms = 0;
			text.setFillColor(sf::Color(255, 255, 255, 64));
		} else {
			int remaining = target - elapsed;
			h = remaining / 3600000; remaining %= 3600000;
			m = remaining / 60000; remaining %= 60000;
			s = remaining / 1000; remaining %= 1000;
			ms = remaining / 10;
		}
	}

	ss << std::setw(2) << std::setfill('0') << h << ":";
	ss << std::setw(2) << std::setfill('0') << m << ":";
	ss << std::setw(2) << std::setfill('0') << s << ":";
	ss << std::setw(2) << std::setfill('0') << ms;
	text.setString(ss.str());

	text.setPosition((getDisplayWidth() - scale * textrect.width) / 2, 0);
	t->draw(text);
}

void ModuleTimer::onEvent(sf::Event e) {
	if(e.type == sf::Event::TextEntered) {
		char c = static_cast<char>(e.text.unicode);
		if(isdigit(c))
			add(atoi(&c));
		if(c == '+')
			addMinute();
		if(c == '-')
			removeMinute();
	}
}

void ModuleTimer::addMinute() {
	if(n == 0)
		c.restart();
	n += 100;
}

void ModuleTimer::removeMinute() {
	if(n > 100)
		n -= 100;
	else
		n = 0;
}