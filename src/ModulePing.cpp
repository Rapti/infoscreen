//
// Created by leon on 20.02.18.
//

#include <sstream>
#include "ModulePing.h"
#include <cmath>

ModulePing::ModulePing(std::vector<std::string> hosts): ModulePing(hosts, hosts) {}

ModulePing::ModulePing(std::vector<std::string> hosts, std::vector<std::string> aliases) {
	for(int i = 0; i < hosts.size(); ++i) {
		if(i < aliases.size())
			this->hosts.push_back(new Host(hosts[i], aliases[i]));
		else
			this->hosts.push_back(new Host(hosts[i]));
	}
	active = true;
	mutex = new std::mutex;
	thread = new std::thread(&ModulePing::refreshLoop, std::ref(*this));
}



ModulePing::~ModulePing() {
	active = false;
	thread->join();
	delete thread;
	mutex->lock();
	for(const Host* h: hosts) {
		delete h;
	}
	mutex->unlock();
	delete mutex;
}

void ModulePing::refreshLoop() {
	while(active) {
//		std::cout << "Refreshing Pings" << std::endl;
		mutex->lock();
		for (Host* h: hosts) {
			std::thread(&Host::ping, std::ref(*h)).detach();
		}
		mutex->unlock();

		std::this_thread::sleep_for(std::chrono::milliseconds(10000));
	}
}

void ModulePing::draw() {
	mutex->lock();

	if(hosts.empty()) {
		mutex->unlock();
		return;
	}

	float maxw = 0, maxh = 0;
	const int testSize = 80;
	const float ypadding = 0.1;
	const int padding = 5;
	sf::Text text;
	text.setFont(monospace);
	text.setCharacterSize(testSize);
	maxh = testSize;

	for(const Host* h: hosts) {

		text.setString(h->getName() + " Offline");
		sf::FloatRect rect = text.getLocalBounds();
		if(rect.width > maxw) {maxw = rect.width;}
		if(rect.height > maxh) {maxh = rect.height;}

	}

	float scaleX = (getDisplayWidth() - 2*padding) / maxw;
	float scaleY = (getDisplayHeight() - 2*padding) / (maxh * hosts.size() + maxh * ypadding * (hosts.size() - 1));
	float scale = fmin(scaleX, scaleY);
	float textSize = testSize * scale;
	text.setCharacterSize(textSize);

	int i = 0;
	for(const Host* h: hosts) {
		text.setFillColor(sf::Color::White);
		text.setPosition(padding, i * (maxh * scale * (ypadding + 1)));
		text.setString(h->getName());
		t->draw(text);

		if(h->isUp()) {
			text.setString("Online");
			text.setFillColor(sf::Color::White);
		} else {
			text.setString("Offline");
			text.setFillColor(sf::Color::Red);
		}
		sf::FloatRect rect = text.getLocalBounds();
		text.setPosition(getDisplayWidth() - rect.width - padding * 2, i * (maxh * scale * (ypadding + 1)));
		t->draw(text);

		++i;
	}

	mutex -> unlock();

}



Host::Host(std::string hostname): Host(hostname, hostname) {}
Host::Host(std::string hostname, std::string alias): hostname(hostname), alias(alias) {
	if(alias.empty())
		alias = hostname;
}
Host::~Host() {}

void Host::ping() {
//	std::cout << "Refreshing " << getName() << std::endl;
	up = system(("ping -c1 " + hostname).c_str()) == 0;
//	std::cout << getName() << " is " << (up? "up":"down") << std::endl;
}

const std::string Host::getName() const {
	return alias;
}

const bool Host::isUp() const {
	return up;
}
