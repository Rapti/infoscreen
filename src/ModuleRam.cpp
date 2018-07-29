//
// Created by leon on 16.12.16.
//

#include <sstream>
#include "ModuleRam.h"
#include "Screen.h"

ModuleRam::ModuleRam(std::string host) : ModuleSystemusage(host) {}
ModuleRam::~ModuleRam() = default;

void ModuleRam::draw() {
    mutex->lock();
    if (!snapshots->empty()) {
		SystemusageSnapshot* last = nullptr;
		for(auto i = snapshots->begin(); i != snapshots->end();) {
			std::list<sf::Vector2f*> points;
			for(; i != snapshots->end();) {
				if(*i && *i != nullptr) {
					if(last == nullptr || (last->getAge() - (*i)->getAge()).asSeconds() < 3) {
						points.push_back(new sf::Vector2f((1 - (*i)->getAge().asSeconds() / (float) 180) * getDisplayWidth(),
														  (1 - (float) (*i)->getMem() / (*i)->getTotalmem()) *
														  getDisplayHeight()));
						last = *i;
						++i;
					} else {
						last = nullptr;
						break;
					}
				} else {
					++i;
					break;
				}
			}
			if(points.size() > 1)
				ModuleSystemusage::draw(points);
			for(sf::Vector2f* v: points) delete v;
			points.clear();
		}


		int padding = 10;
		sf::Text text;
		text.setFont(monospace);
		text.setFillColor(Screen::singleton->getTheme()->getTextTertiary());

		text.setString("000.00 GB");
		text.setCharacterSize(80);
		sf::FloatRect textrect = text.getLocalBounds();
		float scaleX = (getDisplayWidth() - 2*padding) / textrect.width;
		float scaleY = (getDisplayHeight() - 2*padding) / (textrect.height);
		float scale = std::min(scaleX, scaleY) / 1.6F;
		text.setCharacterSize((unsigned int) (80 * scale));

		std::stringstream ss;
		ss << bytesToHumanReadableFormat(snapshots->back()->getMem());
		text.setString(ss.str());
		text.setPosition(padding, padding - 6);
		this->t->draw(text);
//        ss << " / ";
		ss.str(std::string());
		ss << bytesToHumanReadableFormat(snapshots->back()->getTotalmem());
		mutex->unlock();

		text.setString(ss.str());
		textrect = text.getLocalBounds();
		text.setPosition(getDisplayWidth() - textrect.width - padding *2, getDisplayHeight() - textrect.height - padding*2 - 6);
		this->t->draw(text);
    } else
        mutex->unlock();
}