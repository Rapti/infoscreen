//
// Created by leon on 21.12.16.
//

#ifndef INFOSCREEN_MODULECPU_H
#define INFOSCREEN_MODULECPU_H

#include "ModuleSystemusage.h"

class ModuleCpu: public ModuleSystemusage {
public:
    ModuleCpu(std::string host);
    virtual ~ModuleCpu();
    void draw();
};


#endif //INFOSCREEN_MODULECPU_H
