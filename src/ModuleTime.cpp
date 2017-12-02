//
// Created by leon on 02.05.16.
//

#include "ModuleTime.h"
#include <iostream>
#include <sstream>

#include <iomanip>
#include <stdio.h>  /* defines FILENAME_MAX */
#ifdef WINDOWS
#include <direct.h>
    #define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif




ModuleTime::ModuleTime(): Module() {

//    char cCurrentPath[FILENAME_MAX];
//
//    GetCurrentDir(cCurrentPath, sizeof(cCurrentPath));
//
//    cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */
//
//    printf ("The current working directory is %s", cCurrentPath);
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
    std::stringstream ss;
//    time %= 24*60*60;
//    int hours = time / (60 * 60);
//    time %= 60*60;
//    int minutes = time / 60;
//    time %= 60;
//    int seconds = time;
    //std::cout << hours;

    float timeAndDateRatio = 0.6;
    int padding = 10;

    text.setCharacterSize(80);
    text.setString("00:00:00");
    sf::FloatRect textrect = text.getLocalBounds();
    float scaleX = (getWidth() - 2*padding) / textrect.width;
    float scaleY = (timeAndDateRatio * getHeight() - 2*padding) / (textrect.height);
    float scale = std::min(scaleX, scaleY);
    text.setCharacterSize(80 * scale);
    ss << std::setw(2) << std::setfill('0') << time->tm_hour << ":";
//    ss << std::setw(2) << std::setfill('0') << (time->tm_sec % 10) << ":";
    ss << std::setw(2) << std::setfill('0') << time->tm_min << ":";
    ss << std::setw(2) << std::setfill('0') << time->tm_sec;
    text.setString(ss.str());
//    text.setCharacterSize(60 + ((int) timet - initialsecs));
//    std::cout << scaleX << " " << scaleY << std::endl;
    text.setColor(sf::Color::White);
    text.setPosition((getWidth() - scale * textrect.width) / 2, 0);
    t->draw(text);


    text.setCharacterSize(80);

    ss.str(std::string());
    ss << time->tm_mday << "." << time->tm_mon << "." << (time->tm_year + 1900);
    text.setString(ss.str());
    textrect = text.getLocalBounds();
    padding = 10;
    scaleX = (getWidth() - 2*padding) / textrect.width;
    scaleY = ((1-timeAndDateRatio) * getHeight() - 2*padding) / textrect.height;
    scale = std::min(scaleX, scaleY);
    text.setCharacterSize(80 * scale);

    text.setColor(sf::Color::White);
    text.setPosition((getWidth() - scale * textrect.width) / 2, getHeight() * timeAndDateRatio + 0);
    t->draw(text);


    
}