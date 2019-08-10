//
// Created by leon on 15.12.16.
//

#ifndef INFOSCREEN_MODULESYSTEMUSAGE_H
#define INFOSCREEN_MODULESYSTEMUSAGE_H
#include "Module.h"
#include <thread>
#include <mutex>
#include <list>
#include <unordered_map>

class SystemusageSnapshot;
static const int MIN_GAP_WIDTH = 10;

class ModuleSystemusage: public Module {

public:
	explicit ModuleSystemusage(const std::string& host);
	~ModuleSystemusage() override;
protected:
    static std::unordered_map<std::string, std::list<ModuleSystemusage*>*> map;
    std::string host;
    static const int cycleDuration = 180; // three minutes
    static const int updateInterval = 200;
    std::list<SystemusageSnapshot*>* snapshots;
    sf::RectangleShape line;
    std::mutex* mutex;
	void draw() override;
	void draw(std::list<sf::Vector2f*>);

	std::string bytesToHumanReadableFormat(long bytes) const;

private:
    void refreshLoop();
    bool active;
    std::thread* thread;
    std::string exec(const char* cmd);
};


class SystemusageSnapshot {
private:
	const sf::Clock c;
	const long cpu;
	const long totalcpu;
	const long mem;
	const long totalmem;
	const long swp;
	const long totalswp;
public:
	const long getCpu() const;
	const long getTotalcpu() const;
	const long getMem() const;
	const long getTotalmem() const;
	const long getSwp() const;
	const long getTotalswp() const;
	const float getCpuSince(SystemusageSnapshot*) const;

public:
	SystemusageSnapshot(sf::Clock, long, long, long, long, long, long);
	virtual ~SystemusageSnapshot();

	sf::Time getAge();

};

#endif //INFOSCREEN_MODULESYSTEMUSAGE_H