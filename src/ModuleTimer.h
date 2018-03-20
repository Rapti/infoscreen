//
// Created by leon on 08.03.18.
//

#ifndef INFOSCREEN_MODULETIMER_H
#define INFOSCREEN_MODULETIMER_H


#include <ctime>
#include <SFML/System/Clock.hpp>
#include "Module.h"

class ModuleTimer: public Module {
private:
	struct tm target;
	sf::Clock c;
	sf::Clock lastInput;
	int n = 0;
	void updateTarget();

public:
	ModuleTimer();
	virtual ~ModuleTimer();
	void draw();
	void reset();
	void add(unsigned int);
};


#endif //INFOSCREEN_MODULETIMER_H
