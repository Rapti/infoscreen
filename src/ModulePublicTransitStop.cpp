//
// Created by leon on 05.12.17.
//

#include <sstream>
#include "ModulePublicTransitStop.h"
#include <curl/curl.h>
#include <iomanip>
#include <cmath>

ModulePublicTransitStop::ModulePublicTransitStop(const std::string &city, const std::string &stop): Module() {
    this->city = city;
    this->stop = stop;
	mutex = new std::mutex;
	thread = new std::thread(&ModulePublicTransitStop::refreshLoop, std::ref(*this));
}

ModulePublicTransitStop::~ModulePublicTransitStop() {
	active = false;
	thread->detach();
	delete thread;
	delete mutex;
}

void ModulePublicTransitStop::draw() {
	mutex->lock();
	float y = 0;

	const int testSize = 80;
	const float ypadding = 0.1;
	const int padding = 5;
	sf::Text text;
	text.setFont(f);
	text.setCharacterSize(testSize);
	text.setString("HB1 Dortmund Eichlinghofen H-Bahn");
	float tw = text.getLocalBounds().width;


	float scale = (getDisplayWidth() - 3*padding) / tw;
	float textSize = testSize * scale;
	text.setCharacterSize(textSize);

	time_t now;
	std::time(&now);

	for(std::list<Train*>::iterator i = trains.begin(); i != trains.end(); ++i) {
		text.setFillColor(sf::Color::White);
		text.setString((*i)->getName() + " ");
		text.setPosition(padding, padding + y);
		t->draw(text);

		text.setFillColor(sf::Color(255, 255, 255, 160));
		text.setPosition(padding + text.getGlobalBounds().width, text.getPosition().y);
		text.setString((*i)->getDestination());
		t->draw(text);

		text.setFillColor(sf::Color::White);
		std::stringstream ss;
		struct tm tm;
		time_t time = (*i)->getDeparture();
		tm = *localtime(&time);
		ss << std::setw(2) << std::setfill('0') << tm.tm_hour << ":" << std::setw(2) << std::setfill('0') << tm.tm_min << " ";
		text.setPosition(padding, text.getPosition().y + textSize + padding);
		text.setString(ss.str());
		t->draw(text);

		if((*i)->isDelayed()) {
			text.setFillColor(sf::Color::Red);
			text.setPosition(text.getPosition().x + text.getLocalBounds().width, text.getPosition().y);
			ss.str("");
			ss << "+" << (*i)->getDelay();
			text.setString(ss.str());
			t->draw(text);
		}

		if((*i)->isCancelled()) {
			text.setString("fällt aus");
			text.setFillColor(sf::Color::Red);
		} else {
			int minutes = ceil(difftime(time, now) / 60);
			if(minutes == 0)
				text.setString("jetzt");
			else {
				ss.str("");
				if (minutes < 0)
					ss << "vor ";
				if (minutes > 0)
					ss << "in ";
				ss << abs(minutes) << " Minute";
				if(abs(minutes) != 1)
					ss << "n";
				text.setString(ss.str());
			}
			text.setFillColor(sf::Color::White);
		}
		text.setPosition(getDisplayWidth() - text.getLocalBounds().width - padding, text.getPosition().y);
		t->draw(text);

		sf::RectangleShape rs;
		rs.setFillColor(sf::Color(255, 255, 255, 64));
		rs.setSize(sf::Vector2f(getDisplayWidth(), 3));
		rs.setPosition(0, text.getPosition().y + textSize + padding);

		t->draw(rs);

		y = rs.getPosition().y + 3;

		if(y > getDisplayWidth())
			break;
	}
	mutex->unlock();
}

static size_t writefunction(char *data, size_t size, size_t nmemb,
				   std::string *writerData) {
	if(writerData == NULL)
		return 0;
	writerData->append(data, size*nmemb);

	return size * nmemb;
}

void ModulePublicTransitStop::refreshLoop() {
	sf::Clock clock;
	active = true;
	while(active) {
		CURL* curl = curl_easy_init();

		const char* esc_city = curl_easy_escape(curl, city.c_str(), city.length());
		const char* esc_stop = curl_easy_escape(curl, stop.c_str(), stop.length());
		std::stringstream ss;
		ss << "https://vrrf.finalrewind.org/" << esc_city << "/" << esc_stop << ".json";
//		const char* url = ss.str().c_str();
		char* url = new char[ss.str().length() +1];
		std::strcpy(url, ss.str().c_str());
//		std::cout << url<< std::endl;
//		ss.str("");
		std::string result;

		curl_easy_setopt(curl, CURLOPT_URL, url);

		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
//		std::cout << "pointer: " << writefunction << std::endl;

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunction);

		CURLcode res = curl_easy_perform(curl);
		std::cout << "curlcode: " << res << std::endl;


		if(res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));
		}

		curl_easy_cleanup(curl);

//		mutex->lock();
//		std::cout << "result: " << result << std::endl;
		rapidjson::Document data;
		data.Parse(result.c_str());
//		std::cout << data["preformatted"][0][1].GetString() << std::endl;
		mutex->lock();

		for(rapidjson::Value::ValueIterator i =  data["raw"].Begin(); i != data["raw"].End(); ++i) {
			Train* t = new Train();
			t->setName((*i)["line"].GetString());
			t->setDestination((*i)["destination"].GetString());

			struct std::tm tm;
			std::stringstream ss;
			ss << (*i)["sched_date"].GetString();
			ss << " ";
			ss << (*i)["sched_time"].GetString();
			ss >> std::get_time(&tm, "%d.%m.%Y %H:%M");
			time_t time = mktime(&tm);
			t->setDeparture(time);

			if(!(*i)["delay"].IsNull())
				t->setDelay(atoi((*i)["delay"].GetString()));
//			std::cout << asctime(localtime(&time)) << std::endl;
			trains.push_back(t);
		}
		mutex->unlock();
//		active = false;
		int d = 30000 - clock.getElapsedTime().asMilliseconds();
		if (d > 0)
			std::this_thread::sleep_for(std::chrono::milliseconds(d));
		clock.restart();
	}
}

