//
// Created by leon on 02.05.16.
//

#include "ModuleTime.h"
#include "Screen.h"
#include <sstream>
#include <chrono>

#include <iomanip>

#ifdef WINDOWS
#include <direct.h>
    #define GetCurrentDir _getcwd
#else

#include <evdns.h>

#define GetCurrentDir getcwd
#endif




ModuleTime::ModuleTime() : Module() {

}

ModuleTime::~ModuleTime() {
}


void ModuleTime::draw() {
    sf::Text text;
    text.setFont(monospace);
    time_t timet;
    std::time(&timet);
    if(initialsecs == 0)
        initialsecs = (int) timet;
    tm* time = localtime(&timet);

    //Uncomment for hundredths of a second
//    struct timeval tv;
//    gettimeofday(&tv, nullptr);
//    unsigned long long hs =
//            (unsigned long long)(tv.tv_sec) * 100 +
//            (unsigned long long)(tv.tv_usec) / 100;

    std::stringstream ss;

    float timeAndDateRatio = 0.6;
    int paddingX = 20;
    int paddingY = 10;

    text.setCharacterSize(80);
	text.setFillColor(Screen::singleton->getTheme()->getTextPrimary());

    text.setString("00:00:00");
	{
		sf::FloatRect textrect = text.getLocalBounds();
		float scaleX = (getDisplayWidth() - 2 * paddingX) / textrect.width;
		float scaleY = (timeAndDateRatio * getDisplayHeight() - 2 * paddingY) / (textrect.height);
		float scale = std::min(scaleX, scaleY);
		float x = (getDisplayWidth() - scale * textrect.width) / 2;
		float y = (getDisplayHeight() * timeAndDateRatio - scale * textrect.height) / 2;
		text.setPosition(x, y);
		text.move(x - text.getGlobalBounds().left, y - text.getGlobalBounds().top); // Because somehow setPosition doesn't really set the desired position
		text.setScale(scale, scale);
	}

	ss << std::setw(2) << std::setfill('0') << time->tm_hour << ":";
	ss << std::setw(2) << std::setfill('0') << time->tm_min << ":";
	ss << std::setw(2) << std::setfill('0') << time->tm_sec;
//    ss << ":" << std::setw(2) << std::setfill('0') << (hs % 100); // Uncomment this to add hundredths of a second to the clock
	text.setString(ss.str());
    t->draw(text);

	ss.str(std::string());
	ss << time->tm_mday << "." << time->tm_mon + 1 << "." << (time->tm_year + 1900);
	text.setString(ss.str());
	text.setFillColor(Screen::singleton->getTheme()->getTextSecondary());

	{
		sf::FloatRect textrect = text.getLocalBounds();
		float scaleX = (getDisplayWidth() - 2 * paddingX) / textrect.width;
		float scaleY = ((1 - timeAndDateRatio) * getDisplayHeight() - 2 * paddingY) / textrect.height;
		float scale = std::min(scaleX, scaleY);
		text.setScale(scale, scale);

		float x = (getDisplayWidth() - scale * textrect.width) / 2;
		float y = getDisplayHeight() * timeAndDateRatio + (getDisplayHeight() * (1-timeAndDateRatio) - scale * textrect.height) / 2;
		text.setPosition(x, y);
		text.move(x - text.getGlobalBounds().left, y -  text.getGlobalBounds().top); // Because somehow setPosition doesn't really set the desired position
	}

    t->draw(text);


    
}