//
// Created by leon on 16.12.16.
//

#ifndef INFOSCREEN_MODULERAM_H
#define INFOSCREEN_MODULERAM_H

#include "ModuleSystemusage.h"

class ModuleRam: public ModuleSystemusage {
public:
	explicit ModuleRam(std::string);
	~ModuleRam() override;
    void draw() override;

};


#endif //INFOSCREEN_MODULERAM_H
