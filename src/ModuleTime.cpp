//
// Created by leon on 02.05.16.
//

#include "ModuleTime.h"
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
    text.setFont(f);
    time_t timet;
    std::time(&timet);
    if(initialsecs == 0)
        initialsecs = (int) timet;
    tm* time = localtime(&timet);

    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned long long hs =
            (unsigned long long)(tv.tv_sec) * 100 +
            (unsigned long long)(tv.tv_usec) / 100;

    std::stringstream ss;

    float timeAndDateRatio = 0.6;
    int padding = 10;

    text.setCharacterSize(80);
    text.setString("00:00:00");
    sf::FloatRect textrect = text.getLocalBounds();
    float scaleX = (getDisplayWidth() - 2*padding) / textrect.width;
    float scaleY = (timeAndDateRatio * getDisplayHeight() - 2*padding) / (textrect.height);
    float scale = std::min(scaleX, scaleY);
    text.setCharacterSize(80 * scale);
    ss << std::setw(2) << std::setfill('0') << time->tm_hour << ":";
    ss << std::setw(2) << std::setfill('0') << time->tm_min << ":";
    ss << std::setw(2) << std::setfill('0') << time->tm_sec;
//    ss << ":" << std::setw(2) << std::setfill('0') << (hs % 100); // Uncomment this to add hundreds of a second to the clock
    text.setString(ss.str());

    text.setFillColor(sf::Color::White);
    text.setPosition((getDisplayWidth() - scale * textrect.width) / 2, 0);
    t->draw(text);


    text.setCharacterSize(80);

    ss.str(std::string());
    ss << time->tm_mday << "." << time->tm_mon + 1 << "." << (time->tm_year + 1900);
    text.setString(ss.str());
    textrect = text.getLocalBounds();
    padding = 10;
    scaleX = (getDisplayWidth() - 2*padding) / textrect.width;
    scaleY = ((1-timeAndDateRatio) * getDisplayHeight() - 2*padding) / textrect.height;
    scale = std::min(scaleX, scaleY);
    text.setCharacterSize(80 * scale);

    text.setFillColor(sf::Color::White);
    text.setPosition((getDisplayWidth() - scale * textrect.width) / 2, getDisplayHeight() * timeAndDateRatio + 0);
    t->draw(text);


    
}