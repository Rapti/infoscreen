//
// Created by leon on 20.02.18.
//

#ifndef INFOSCREEN_MODULEPING_H
#define INFOSCREEN_MODULEPING_H

#include <mutex>
#include <thread>
#include "Module.h"


class Host {
private:
	std::string hostname;
	std::string alias;
	bool up = false;

public:
	Host(std::string);
	Host(std::string, std::string);
	virtual ~Host();

	void ping();
	const std::string getName()const;
	const bool isUp()const;
};

class ModulePing: public Module {
private:

	std::list<Host*> hosts;
	std::mutex* mutex;
	void refreshLoop();
	bool active;
	std::thread* thread;

public:
	ModulePing(std::vector<std::string>);
	ModulePing(std::vector<std::string> hosts, std::vector<std::string> aliases);
	virtual ~ModulePing();
	virtual void draw();
};



#endif //INFOSCREEN_MODULEPING_H
