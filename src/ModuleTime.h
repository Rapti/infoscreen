//
// Created by leon on 02.05.16.
//

#ifndef INFOSCREEN_MODULETIME_H
#define INFOSCREEN_MODULETIME_H
#include "Module.h"

class ModuleTime: public Module {
public:
    ModuleTime();
    virtual ~ModuleTime();
private:
protected:
    void draw();
};


#endif //INFOSCREEN_MODULETIME_H
