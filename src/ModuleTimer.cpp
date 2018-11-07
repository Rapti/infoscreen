//
// Created by leon on 08.03.18.
//

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Text.hpp>
#include <sstream>
#include <iomanip>
#include <cstring>
#include "ModuleTimer.h"
#include "Screen.h"
#include "RichText.h"

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
	active = true;
}

void ModuleTimer::updateTarget() {

}

void ModuleTimer::draw() {
	sf::Text text;
	text.setFont(monospace); // Must be a monospace font or everything will start to look weird.

	std::stringstream ss;

	int padding = 10;
	int testsize = 80;

	int h, m, s, ms;

	{
		int x = n;
		h = x / 10000; x %= 10000;
		m = x / 100; x %= 100;
		s = x;
		ms = 0;
	}

	bool inputActive = lastInput.getElapsedTime().asSeconds() <= 3;

	int target = (s + 60 * m + 3600 * h) * 1000;
	int elapsed = c.getElapsedTime().asMilliseconds();
	if(n == 0 || !inputActive) {
		if(elapsed > target) {
			h = m = s = ms = 0;
			active = false;
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

	std::string string = ss.str();

	text.setCharacterSize(testsize);
	std::string zerostring = "00:00:00:00";
	text.setString(zerostring);
	sf::FloatRect textrect = text.getLocalBounds();
	float scaleX = (getDisplayWidth() - 2*padding) / textrect.width;
	float scaleY = (getDisplayHeight() - 2*padding) / (textrect.height);
	float scale = std::min(scaleX, scaleY);
	text.setCharacterSize(testsize * scale);
	textrect = text.getLocalBounds();
	text.setFillColor(Screen::singleton->getTheme()->getTextPrimary());
	float charWidth = textrect.width / zerostring.length();

	float x = (getDisplayWidth() - textrect.width) / 2;
	float y = (getDisplayHeight() - textrect.height) / 2;
	text.setPosition(x, y);
	text.move(x - text.getGlobalBounds().left, y - text.getGlobalBounds().top); // Because somehow setPosition doesn't really set the desired position

	if(active) {
		int sublength = 0;
		while(sublength < string.length() && (string[sublength] == '0' || string[sublength] == ':')) {
			++sublength;
		}
		if (sublength > 0 && sublength < string.length()) {
			text.setString(string.substr(0, sublength));
			string = string.substr(sublength);
			text.setFillColor(Screen::singleton->getTheme()->getTextDisabled());
			t->draw(text);
		}
		if (string.length() > 3 && inputActive) {
			string.resize(string.length() - 3);
			text.setString(":00");
			text.setFillColor(Screen::singleton->getTheme()->getTextDisabled());
			text.move(charWidth * 8, 0);
			t->draw(text);
			text.move(charWidth * -8, 0);
		}
		text.setString(string);
		text.setFillColor(Screen::singleton->getTheme()->getTextPrimary());
		text.move(charWidth * sublength, 0);
		t->draw(text);
	} else {
		text.setString(string);
		if(elapsed - target < 5000 && (elapsed - target) % 1000 < 500)
			text.setFillColor(Screen::singleton->getTheme()->getTextPrimary());
		else
			text.setFillColor(Screen::singleton->getTheme()->getTextDisabled());
		t->draw(text);
	}
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
	if(!active) {
		c.restart();
		active = true;
		n = 100;
	} else
		n += 100;
}

void ModuleTimer::removeMinute() {
	if(n > 100)
		n -= 100;
	else {
		n = 0;
		c.restart();
		active = false; // This is here intentionally to prevent the sound from playing.
	}
}
