//
// Created by leon on 14.12.16.
//

#include "ModuleEmpty.h"

ModuleEmpty::ModuleEmpty() : Module() {

}

ModuleEmpty::~ModuleEmpty() {
}

void ModuleEmpty::draw() {
    t->clear(sf::Color(255, 255, 255, 0));
}