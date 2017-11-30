//
// Created by leon on 20.12.16.
//

#ifndef INFOSCREEN_MODULESWP_H
#define INFOSCREEN_MODULESWP_H

#include "ModuleSystemusage.h"

class ModuleSwp: public ModuleSystemusage {
public:
    ModuleSwp(std::string);
    virtual ~ModuleSwp();
    void draw();
private:

};


#endif //INFOSCREEN_MODULESWP_H
