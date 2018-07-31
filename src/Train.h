//
// Created by leon on 19.12.17.
//

#ifndef INFOSCREEN_TRAIN_H
#define INFOSCREEN_TRAIN_H


#include <string>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/System/Clock.hpp>

class Train {
public:
	Train();
	virtual ~Train();

	time_t getDeparture() const;
	void setDeparture(time_t departure);
	int getDelay() const;
	void setDelay(int delay);
	void setCancelled(bool);
	const std::string &getPlatform() const;
	void setPlatform(const std::string &platform);
	const std::string &getName() const;
	void setName(const std::string &name);
	std::wstring getDestination() const;
	void setDestination(std::wstring destination);
	bool isCancelled();
	bool isDelayed();

	sf::RenderTexture* render(sf::Font, unsigned int);
	void setIndex(int index);
	int getIndex() const;
	int getPreviousIndex() const;
	const std::string &getID() const;
	void setID(const std::string &id);

	bool isInvalid() const;

	void setInvalid(bool invalid);

	float getAge();
	void markAsUpdated();

private:
	time_t departure;
	int delay;
	sf::Clock lastUpdate;
	std::string platform;
	std::string name;
	std::string id;
	int index = INT32_MAX;
	int prev_index = INT32_MAX;
	std::wstring destination;
	sf::RenderTexture t;
	bool cancelled = false;
	bool invalid = false;
};



#endif //INFOSCREEN_TRAIN_H