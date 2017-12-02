//
// Created by leon on 14.11.17.
//

#include "ModuleInvisible.h"
#include "Screen.h"

ModuleInvisible::ModuleInvisible(): Module() {

}

ModuleInvisible::~ModuleInvisible() {
}



bool ModuleInvisible::drawBackground() {return false;}



void ModuleInvisible::draw() {

    t->clear(sf::Color(255, 255, 255, 0));
}