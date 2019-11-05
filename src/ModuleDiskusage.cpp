//
// Created by leon on 13.10.19.
//

#include "ModuleDiskusage.h"
#include "Screen.h"

#include <utility>
#include <iomanip>
#include <sstream>

ModuleDiskusage::ModuleDiskusage(const std::string &host, std::vector<std::string> disks) : ModuleSystemusage(host), disks(std::move(disks)) {}

void ModuleDiskusage::draw() {
	mutex->lock();
	if (!snapshots->empty() && !disks.empty()) {
		std::vector<std::pair<Disk*, std::string>> foundDisks;
		auto* snapshot = snapshots->back();

		for(auto* disk: *snapshot->getDisks()) {
			for(const auto& diskname: disks) {
				if(disk->is(diskname)) {
					std::cout << "Found disk " << diskname << std::endl;
					foundDisks.emplace_back(disk, diskname);
					break;
				}
			}
		}
		if(foundDisks.empty()) {
//			std::cout << "No disks found" << std::endl;
		} else {
			sf::Text text;
			text.setCharacterSize(20);
			text.setFont(monospace);
			text.setFillColor(Screen::singleton->getTheme()->getTextSecondary());
			for (const auto &pair: foundDisks) {
				text.setString(pair.second);
				text.setPosition(10, 10);
				t->draw(text);
				std::stringstream ss;
				ss << std::setw(3) << std::setfill(' ') << pair.first->getUsedPercentage() << " %";
				text.setString(ss.str());
				text.setPosition(getDisplayWidth() - text.getGlobalBounds().width - 10, 10);
				t->draw(text);
			}
		}
	} else {
		std::cout << "Not drawing disk usage" << std::endl;
	}
	mutex->unlock();
}
