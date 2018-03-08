//
// Created by leon on 16.12.16.
//

#include <sstream>
#include "ModuleSwp.h"

ModuleSwp::ModuleSwp(std::string host) : ModuleSystemusage(host) {

}

ModuleSwp::~ModuleSwp() {

}

void ModuleSwp::draw() {
	int xoffset = 0;
	int yoffset = 0;
	float tlheight = getDisplayHeight() - 1.5*yoffset;
	float tlwidth = getDisplayWidth() - 2*xoffset;

	mutex->lock();
	if (!snapshots->empty()) {
		SystemusageSnapshot* last;
		for(std::list<SystemusageSnapshot*>::iterator i = snapshots->begin(); i != snapshots->end();) {
			std::list<sf::Vector2f*> points;
			for(; i != snapshots->end();) {
				if(*i) {
					if(last == nullptr || (last->getAge() - (*i)->getAge()).asSeconds() < 3) {
						points.push_back(new sf::Vector2f((1 - (*i)->getAge().asSeconds() / 180.0) * getDisplayWidth(),
														  (1 - (float) (*i)->getSwp() / (*i)->getTotalswp()) *
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
		text.setFillColor(sf::Color(255, 255, 255, 128));

		text.setString("000.00 GB");
		text.setCharacterSize(80);
		sf::FloatRect textrect = text.getLocalBounds();
		float scaleX = (getDisplayWidth() - 2*padding) / textrect.width;
		float scaleY = (getDisplayHeight() - 2*padding) / (textrect.height);
		float scale = std::min(scaleX, scaleY) / 1.6;
		text.setCharacterSize(80 * scale);

		std::stringstream ss;
		ss << bytesToHumanReadableFormat(snapshots->back()->getSwp());
		text.setString(ss.str());
		text.setPosition(padding, padding - 6);
		this->t->draw(text);
//        ss << " / ";
		ss.str(std::string());
		ss << bytesToHumanReadableFormat(snapshots->back()->getTotalswp());
		mutex->unlock();


		text.setString(ss.str());
		textrect = text.getLocalBounds();
		text.setPosition(getDisplayWidth() - textrect.width - padding *2, getDisplayHeight() - textrect.height - padding*2 - 6);
		this->t->draw(text);
    } else
        mutex->unlock();
}