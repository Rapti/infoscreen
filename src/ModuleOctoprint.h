//
// Created by leon on 13.06.20.
//

#ifndef INFOSCREEN_MODULEOCTOPRINT_H
#define INFOSCREEN_MODULEOCTOPRINT_H

using namespace std;

#include <thread>
#include "Module.h"

class ModuleOctoprint: public Module {
private:
	const string apiUrl;
	const string apiKey;
	string status;
	string file;
	wstring displayFile;
	double jobProgress;
	int printTime;
	int printTimeLeft;

	mutex m;
	bool active;
	thread* refreshThread;
	sf::Clock clock;

	void refreshLoop();

public:
	ModuleOctoprint(const string &apiUrl, const string &apiKey);
	virtual ~ModuleOctoprint();
	virtual void draw();
};


#endif //INFOSCREEN_MODULEOCTOPRINT_H
