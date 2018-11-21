//
// Created by leon on 05.12.17.
//

#ifndef INFOSCREEN_MODULEPUBLICTRANSIT_H
#define INFOSCREEN_MODULEPUBLICTRANSIT_H


#include <thread>
#include <mutex>
#include "Module.h"
#include "rapidjson/document.h"
#include "Train.h"

class ModulePublicTransitStop: public Module {
private:
    std::string city;
    std::string stop;
	std::list<Train*> trains;
	std::vector<std::string> types;


	void refreshLoop();
	bool active;
	std::thread* thread;
	std::mutex* mutex;
	sf::Clock clock;

public:
    ModulePublicTransitStop(const std::string &city, const std::string &stop);
    ModulePublicTransitStop(const std::string &city, const std::string &stop, std::vector<std::string> types);

    virtual ~ModulePublicTransitStop();
	virtual void draw();

};


#endif //INFOSCREEN_MODULEPUBLICTRANSIT_H
