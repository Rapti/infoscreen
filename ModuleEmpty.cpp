//
// Created by leon on 14.12.16.
//

#include "ModuleEmpty.h"
#include "Screen.h"

ModuleEmpty::ModuleEmpty(): Module() {

}

ModuleEmpty::~ModuleEmpty() {
}

float ModuleEmpty::getHeight() {
    return defaultHeight * 2 + Screen::singleton->margin;
}



void ModuleEmpty::draw() {

}