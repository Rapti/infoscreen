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
	sf::Glyph g = monospace.getGlyph('0', testSize, false);
	charWidth100 = g.advance;
	charHeight100 = g.bounds.height;
	charSpacing100 = g.advance - g.bounds.width;
	setStrLen(11);
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
			setStrLen(11);
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

	int sublength = 0;
	while(sublength < string.length() && (string[sublength] == '0' || string[sublength] == ':')) {
		++sublength;
	}
	if(inputActive) {
		setStrLen(11);
	} else if(active) {
		setStrLen(std::max(4, 11 - sublength));
	}

	text.setString("00:00:00:00");
	text.setCharacterSize(testSize);
	float scale = getTextScale();
	text.setScale(scale, scale);

	float charWidth = getCharWidth();

	float x = (getDisplayWidth() - text.getGlobalBounds().width) / 2;
	float y = (getDisplayHeight() - text.getGlobalBounds().height) / 2;
	if(x < padding)
		x = x * 2 - padding;

	text.setPosition(x, y);
	text.move(x - text.getGlobalBounds().left, y - text.getGlobalBounds().top); // Because somehow setPosition doesn't really set the desired position

	if(active) {
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
		if(n != 0 && elapsed - target < 5000 && (elapsed - target) % 1000 < 500)
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
		std::cout << n << " ";
		std::cout << n << std::endl;
	}
}

void ModuleTimer::addMinute() {
	if(!active) {
		c.restart();
		active = true;
		n = 100;
	} else
		n = s2n(n2s(n) + 60);
}

void ModuleTimer::removeMinute() {
	n = s2n(n2s(n) - 60);
	if(n <= 0) {
		n = 0;
		c.restart();
		active = false; // This is here intentionally to prevent the sound from playing.
	}
}

int ModuleTimer::n2s(int n) { // Convert h:m:s representation to number of seconds
	int h, m;

	int x = n;
	h = x / 10000; x %= 10000;
	m = x / 100; x %= 100;

	return x + 60 * m + 3600 * h;
}

int ModuleTimer::s2n(int s) { // Convert number of seconds to h:m:s representation
	int h, m;
	h = s / 3600; s %= 3600;
	m = s / 60; s %= 60;
	return s + 100 * m + 10000 * h;
}

void ModuleTimer::setStrLen(int length) {
	if(strLen != length) {
		lastStrSize = getTextScale();
		strLen = length;
		lastStrLenChange.restart();
	}
}

float ModuleTimer::getTextScale() {
//	strLen = 11;
	float scaleX = (getDisplayWidth() - 2*padding) / (charWidth100 * strLen - charSpacing100);
	float scaleY = (getDisplayHeight() - 2*padding) / (charHeight100);
	float scale = std::min(scaleX, scaleY);
	if(lastStrSize == 0 || lastStrLenChange.getElapsedTime().asSeconds() > animationDuration) {
		return scale;
	}
	float animationProgress = std::pow(lastStrLenChange.getElapsedTime().asSeconds() / animationDuration, (float) 1/4);
	return animationProgress * scale + (1-animationProgress) * lastStrSize;
}

float ModuleTimer::getCharWidth() {
	return charWidth100 * getTextScale();
}

float ModuleTimer::getCharHeight() {
	return charHeight100 * getTextScale();
}
