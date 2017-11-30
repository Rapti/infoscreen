//
// Created by leon on 14.11.17.
//

#ifndef INFOSCREEN_MODULEINVISIBLE_H
#define INFOSCREEN_MODULEINVISIBLE_H


#include "Module.h"

class ModuleInvisible: public Module {
public:
    ModuleInvisible();

    virtual ~ModuleInvisible();
    virtual bool drawBackground();

private:
protected:
    void draw();
};


#endif //INFOSCREEN_MODULEINVISIBLE_H
