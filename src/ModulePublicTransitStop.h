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


	void refreshLoop();
	bool active;
	std::thread* thread;
	std::mutex* mutex;

public:
    ModulePublicTransitStop(const std::string &city, const std::string &stop);

    virtual ~ModulePublicTransitStop();
	virtual void draw();

};


#endif //INFOSCREEN_MODULEPUBLICTRANSIT_H
