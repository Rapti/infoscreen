//
// Created by leon on 14.12.16.
//

#ifndef INFOSCREEN_MODULEEMPTY_H
#define INFOSCREEN_MODULEEMPTY_H
#include "Module.h"


class ModuleEmpty: public Module {
public:
    ModuleEmpty();
    virtual ~ModuleEmpty();
    virtual float getHeight();
private:
protected:
    void draw();
};


#endif //INFOSCREEN_MODULEEMPTY_H
