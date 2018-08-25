//
// Created by leon on 16.12.16.
//

#include <sstream>
#include <iomanip>
#include "ModuleCpu.h"
#include "Screen.h"

ModuleCpu::ModuleCpu(std::string host) : ModuleSystemusage(host) {
}
ModuleCpu::~ModuleCpu() = default;

void ModuleCpu::draw() {

	const float normalisationSeconds = 2;

	mutex->lock();
	if (!snapshots->empty()) {
		SystemusageSnapshot* last = nullptr;
		double displayvalue = -1;
		for (auto i = ++snapshots->begin(); i != snapshots->end();) {
			std::list<sf::Vector2f*> points;
			for (; i != snapshots->end();) {
				if (*i) {
					if (last == nullptr || (last->getAge().asSeconds() - (*i)->getAge().asSeconds()) < MIN_GAP_WIDTH) {
						int j = 0;
						SystemusageSnapshot* s = *i;
						while (i != ++snapshots->begin()) {
							--i;
							++j;
							if(*i == nullptr) {
								++i;
								--j;
								break;
							}
							if (((*i)->getAge() - s->getAge()).asSeconds() > normalisationSeconds) {
								break;
							}
						}
						s = *i;
						while (j-- > 0) ++i;

						if(s != *i) {
							points.push_back(
									new sf::Vector2f(
											3 + (1 - (*i)->getAge().asSeconds() / cycleDuration) * getDisplayWidth(),
											(1 - (*i)->getCpuSince(s)) * getDisplayHeight()));
							if(i == --snapshots->end())
								displayvalue = (*i)->getCpuSince(s);
						}
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
			if (points.size() > 1)
				ModuleSystemusage::draw(points);
			for (sf::Vector2f* v: points) delete v;
			points.clear();
		}

		if(displayvalue != -1) {
			int padding = 10;
			sf::Text text;
			text.setFont(monospace);
			text.setFillColor(Screen::singleton->getTheme()->getTextTertiary());

			text.setString("100 %");
			text.setCharacterSize(80);
			sf::FloatRect textrect = text.getLocalBounds();
			float scaleX = (getDisplayWidth() - 2 * padding) / textrect.width;
			float scaleY = (getDisplayHeight() - 2 * padding) / (textrect.height);
			float scale = std::min(scaleX, scaleY);
			text.setCharacterSize((unsigned int) (80 * scale));

			std::stringstream ss;
			ss << std::setw(3) << std::setfill(' ') << (int) (displayvalue * 100) << " %";
			mutex->unlock();
			text.setString(ss.str());
			text.setPosition(0, padding - 6 - (80 * scale * 0.18F));
			t->draw(text);
		}
	}
	mutex->unlock();
}