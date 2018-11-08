//
// Created by leon on 08.03.18.
//

#ifndef INFOSCREEN_MODULETIMER_H
#define INFOSCREEN_MODULETIMER_H


#include <ctime>
#include <SFML/System/Clock.hpp>
#include "Module.h"
#include "EventListener.h"

class ModuleTimer: public Module, public EventListener {
private:
	struct tm target;
	sf::Clock c;
	sf::Clock lastInput;
	sf::Clock lastStrLenChange;
	int strLen = 11;
	float lastStrSize = 0;
	int n = 0;
	void updateTarget();
	bool active;

	float charWidth100 = 0;
	float charHeight100 = 0;
	float charSpacing100 = 0;
	int padding = 10;
	int testSize = 100;

	float animationDuration = 0.5; // in seconds

	void setStrLen(int length);
	float getTextScale();
	float getCharWidth();
	float getCharHeight();

	int n2s(int n);
	int s2n(int s);

protected:
	void onEvent(sf::Event) override;

public:
	ModuleTimer();
	~ModuleTimer() override;
	void draw() override;
	void reset();
	void add(int);
	void addMinute();
	void removeMinute();
};



#endif //INFOSCREEN_MODULETIMER_H
