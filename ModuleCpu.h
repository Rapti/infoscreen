//
// Created by leon on 21.12.16.
//

#ifndef INFOSCREEN_MODULECPU_H
#define INFOSCREEN_MODULECPU_H

#include "ModuleSystemusage.h"

class ModuleCpu: public ModuleSystemusage {
public:
    ModuleCpu(std::string);
    virtual ~ModuleCpu();
    void draw();
private:
    int normalizationTime = 2000;
    int* normalizationValues;
};


#endif //INFOSCREEN_MODULECPU_H
