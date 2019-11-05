//
// Created by leon on 13.10.19.
//

#ifndef INFOSCREEN_MODULEDISKUSAGE_H
#define INFOSCREEN_MODULEDISKUSAGE_H


#include "ModuleSystemusage.h"

class ModuleDiskusage: public ModuleSystemusage {
public:
	ModuleDiskusage(const std::string &host, std::vector<std::string> disks);
	void draw() override;
private:
	std::vector<std::string> disks;
};


#endif //INFOSCREEN_MODULEDISKUSAGE_H
