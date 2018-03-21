//
// Created by leon on 21.03.18.
//

#ifndef INFOSCREEN_EVENTLISTENER_H
#define INFOSCREEN_EVENTLISTENER_H


#include <SFML/Window/Event.hpp>

class EventListener {
public:
	EventListener();
	virtual ~EventListener();

	virtual void onEvent(sf::Event) = 0;

};


#endif //INFOSCREEN_EVENTLISTENER_H
