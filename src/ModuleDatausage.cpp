//
// Created by leon on 13.11.17.
//

#include <iostream>
#include <cmath>
#include <iomanip>

#include "ModuleDatausage.h"
#include "Screen.h"

ModuleDatausage::ModuleDatausage() : Module() {
    upload = 0;
    download = 0;
    limit = 0;
    rect.setFillColor(bgcolor);
    rect.setPosition(0, 0);
    line.setFillColor(linecolor);
    line.setPosition(0, 0);
    dayline.setFillColor(bgcolor);
    dayline.setPosition(0, 0);
    tri = sf::VertexArray(sf::Triangles, 6);
    for(int i = 0; i < 6; ++i) {
        tri[i].color = linecolor;
    }
    mutex = new std::mutex();
    active = true;
    thread = new std::thread(&ModuleDatausage::refreshLoop, std::ref(*this));
}
ModuleDatausage::~ModuleDatausage() {
    active = false;
    delete mutex;
    delete thread;
}



void ModuleDatausage::refreshLoop() {
    sf::Clock clock;
    std::string str = "curl -s 10.4.11.1/traffic.php | head -n1";
    const char *cmd = str.c_str();
    while (active) {
        std::string result = exec(cmd);
        mutex->lock();

        std::string unit;

        upload = extractint(result, std::regex("gesendet[.: ]+(\\d+)"));
        unit = extractstring(result, std::regex("gesendet[.: ]+\\d+ (Bytes|KiloBytes|MegaBytes)"));
        if(unit == "Bytes") {
            upload /= 1024 * 1024;
        } else if(unit == "KiloBytes") {
            upload /= 1024;
        }
        download = extractint(result, std::regex("empfangen[.: ]+(\\d+)"));
        unit = extractstring(result, std::regex("empfangen[.: ]+\\d+ (Bytes|KiloBytes|MegaBytes)"));
        if(unit == "Bytes") {
            download /= 1024 * 1024;
        } else if(unit == "KiloBytes") {
            download /= 1024;
        }
        limit = extractint(result, std::regex("Transfer Limit[.: ]+(\\d+)"));

//        std::cout << "Gesendet: " << upload << "    Empfangen: " << download << "   Limit: " << limit << std::endl;


        mutex->unlock();


        int d = 10000 - clock.getElapsedTime().asMilliseconds();
        if (d > 0)
            std::this_thread::sleep_for(std::chrono::milliseconds(d));
        clock.restart();
    }
}

int ModuleDatausage::extractint(std::string s, std::regex e) {
    return atoi(extractstring(s, e).c_str());
}

std::string ModuleDatausage::extractstring(std::string s, std::regex e) {
    std::smatch m;
    if (std::regex_search (s, m, e)) {
        int i = 0;
        for (std::string x:m) {
            if (i++ == 0) continue;
            return x;
        }
    }
}

void ModuleDatausage::draw() {

    int xoffset = 0;
    int yoffset = 0;
    float tlheight = getDisplayHeight() - 2*yoffset;
    float tlwidth = getDisplayWidth() - 2*xoffset;

    mutex->lock();
    if (limit > 0) {


        std::time_t ttime = std::time(0);
        std::tm * tm = std::localtime(&ttime);
        float week = (60 * 60 * 24 * 7);
        //float weekprogress = ((time - (60*60*24)) % week) * 1.0 / week;
        float weekprogress = ((((tm->tm_wday + 2) % 7) * 24 + tm->tm_hour) * 60 + tm->tm_min) * 60 + tm->tm_sec;


        dayline.setSize(sf::Vector2f(1, tlheight));
        for(int i = 1; i < 7; ++i) {
            dayline.setPosition(tlwidth * i / 7, 0);
            t->draw(dayline);
        }

        
        rect.setPosition(xoffset, yoffset);
        int total = upload + download;
        rect.setSize(sf::Vector2f(tlwidth * (total * 1.0 / limit), tlheight));
        t->draw(rect);
        line.setSize(sf::Vector2f(3, tlheight));
        line.setPosition(xoffset + tlwidth * (upload * 1.0 / limit), yoffset);
        t->draw(line);
        line.setPosition(xoffset + tlwidth * (total * 1.0 / limit), yoffset);
        t->draw(line);


        int timediffsecs;
        if(total > limit)
            timediffsecs = weekprogress - (week);
        else
            timediffsecs = weekprogress - (week * total / limit);


        weekprogress *= tlwidth / week;
        int trianglesize = 9;
        tri[0].position = sf::Vector2f(xoffset + weekprogress - trianglesize, yoffset);
        tri[1].position = sf::Vector2f(xoffset + weekprogress + trianglesize, yoffset);
        tri[2].position = sf::Vector2f(xoffset + weekprogress, trianglesize + yoffset);
        tri[3].position = sf::Vector2f(xoffset + weekprogress - trianglesize, tlheight + yoffset);
        tri[4].position = sf::Vector2f(xoffset + weekprogress + trianglesize, tlheight + yoffset);
        tri[5].position = sf::Vector2f(xoffset + weekprogress, tlheight + yoffset - trianglesize);

        t->draw(tri);


        sf::Text text;
        text.setFont(f);
        if(download + upload >= limit) {
            text.setString("rip");
        } else {
            std::stringstream ss;
            if(limit - upload - download >= 1024) {
                ss << (limit - total) * 100 / 1024 / 100.0;
                ss << " GB verbleiben";
            } else {
                ss << (limit - total);
                ss << " MB verbleiben";
            }
            ss << std::endl;

            if(timediffsecs < 0) {
                ss << "-";
                timediffsecs *= -1;
            }
//            ss << floor(timediffsecs / (24 * 60 * 60)) << ":";
//            timediffsecs %= 24 * 60 * 60;
            ss << std::setw(2) << std::setfill('0') << floor(timediffsecs / (60 * 60)) << ":";
            timediffsecs %= 60 * 60;
            ss << std::setw(2) << std::setfill('0') << floor(timediffsecs / 60) << ":";
            timediffsecs %= 60;
            ss << std::setw(2) << std::setfill('0') << timediffsecs;


            text.setString(ss.str());

        }
        float pos = xoffset + tlwidth * (total * 1.0 / limit) + 20;
        float textwidth = text.getLocalBounds().width + 40;

        if(tlwidth - (xoffset + tlwidth * (total * 1.0 / limit)) < textwidth) {
            pos -= textwidth;
            if (pos < xoffset + tlwidth * (upload * 1.0 / limit))
                pos = xoffset + tlwidth * (upload * 1.0 / limit) - textwidth;
            if (pos + textwidth > tlwidth)
                pos = tlwidth - textwidth;
        }

        text.setPosition(pos, yoffset + 20);
        t->draw(text);
    }
    mutex->unlock();
}


std::string ModuleDatausage::exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if(!pipe) {std::cout << "Error opening pipe" << std::endl; return "";}
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
    }
    return result;
}