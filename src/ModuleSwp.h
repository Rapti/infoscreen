//
// Created by leon on 20.12.16.
//

#ifndef INFOSCREEN_MODULESWP_H
#define INFOSCREEN_MODULESWP_H

#include "ModuleSystemusage.h"

class ModuleSwp: public ModuleSystemusage {
public:
	explicit ModuleSwp(std::string host);
	~ModuleSwp() override;
    void draw() override;
private:

};


#endif //INFOSCREEN_MODULESWP_H
