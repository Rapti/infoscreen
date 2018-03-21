//
// Created by leon on 21.03.18.
//

#include "EventListener.h"
#include "Screen.h"


EventListener::EventListener() {
	Screen::singleton->addEventListener(this);
}
EventListener::~EventListener() {
	Screen::singleton->removeEventListener(this);
}
