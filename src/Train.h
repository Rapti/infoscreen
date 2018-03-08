//
// Created by leon on 19.12.17.
//

#ifndef INFOSCREEN_TRAIN_H
#define INFOSCREEN_TRAIN_H


#include <string>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

class Train {
public:
	Train();
	virtual ~Train();

	time_t getDeparture() const;
	void setDeparture(time_t departure);
	int getDelay() const;
	void setDelay(int delay);
	const std::string &getPlatform() const;
	void setPlatform(const std::string &platform);
	const std::string &getName() const;
	void setName(const std::string &name);
	std::wstring getDestination() const;
	void setDestination(std::wstring destination);
	bool isCancelled();
	bool isDelayed();

	sf::RenderTexture* render(sf::Font, unsigned int);

private:
	time_t departure;
	int delay;
	std::string platform;
	std::string name;
	std::wstring destination;
	sf::RenderTexture t;
	bool cancelled;
};



#endif //INFOSCREEN_TRAIN_H