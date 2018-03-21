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
	int n = 0;
	void updateTarget();

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
