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
class Disk;
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
	const long nicedcpu;
	const long totalcpu;
	const long mem;
	const long totalmem;
	const long swp;
	const long totalswp;
	const std::list<Disk*>* disks;
public:
	long getCpu() const;
	long getNicedCpu() const;
	long getTotalcpu() const;
	long getMem() const;
	long getTotalmem() const;
	long getSwp() const;
	long getTotalswp() const;
	const std::list<Disk*>* getDisks() const;
	float getCpuSince(SystemusageSnapshot*) const;

public:
	SystemusageSnapshot(sf::Clock, long cpu, long nicedcpu, long totalcpu, long ram, long totalram, long swp, long totalswp, std::list<Disk*>* disks);
	virtual ~SystemusageSnapshot();

	sf::Time getAge();

};

class Disk {
private:
	const std::string source;
	const std::string mountpoint;
	const long capacity;
	const long used;
public:
	Disk(std::string source, std::string mountpoint, const long capacity, const long used);
	virtual ~Disk();
public:
	const std::string &getSource() const;
	const std::string &getMountpoint() const;
	const long getCapacity() const;
	const long getUsed() const;
	const double getUsedPercentage() const;

	bool is(std::string path);
};

#endif //INFOSCREEN_MODULESYSTEMUSAGE_H