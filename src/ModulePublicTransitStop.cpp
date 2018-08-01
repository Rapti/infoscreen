//
// Created by leon on 05.12.17.
//

#include <sstream>
#include "ModulePublicTransitStop.h"
#include "Screen.h"
#include <curl/curl.h>
#include <iomanip>
#include <cmath>
#include <codecvt>
#include <ctime>
#include <algorithm>

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

	const int testSize = 80;
	const float ypadding = 0.1;
	const int padding = 5;
	sf::Text text;
	text.setFont(natural);
	text.setCharacterSize(testSize);
	text.setString("HB1 Dortmund Eichlinghofen H-Bahn");
	float tw = text.getLocalBounds().width;
	float lw = 3;

	float scale = (getDisplayWidth() - 3*padding) / tw;
	float textSize = testSize * scale;
	text.setCharacterSize(textSize);

	float entryHeight = 2 * textSize + 3 * padding + lw;

	time_t now;
	std::time(&now);

	float animationDuration = 1; // in seconds

	for(auto i = trains.begin(); i != trains.end(); ++i) {
		auto train = *i;
		float y = 0;
		if(train->getPreviousIndex() == INT32_MAX)
			y = train->getIndex() * entryHeight;
		else
			y = (train->getIndex() + (train->getPreviousIndex() - train->getIndex()) * (fmax((animationDuration -
				train->getAge()) / animationDuration, 0))) * entryHeight;
		text.setFillColor(Screen::singleton->getTheme()->getTextPrimary());
		text.setString(train->getName() + " ");
		text.setPosition(padding, padding + y);
		t->draw(text);

		text.setFillColor(Screen::singleton->getTheme()->getTextSecondary());
		text.setPosition(padding + text.getGlobalBounds().width, text.getPosition().y);
		text.setString(train->getDestination());
		t->draw(text);

		text.setFillColor(Screen::singleton->getTheme()->getTextPrimary());
		std::stringstream ss;
		struct tm tm;
		time_t time = train->getDeparture();
		tm = *localtime(&time);
		ss << std::setw(2) << std::setfill('0') << tm.tm_hour << ":" << std::setw(2) << std::setfill('0') << tm.tm_min << " ";
		text.setPosition(padding, text.getPosition().y + textSize + padding);
		text.setString(ss.str());
		t->draw(text);

		if(train->isDelayed()) {
			text.setFillColor(Screen::singleton->getTheme()->getTextError());
			text.setPosition(text.getPosition().x + text.getLocalBounds().width, text.getPosition().y);
			ss.str("");
			ss << "+" << train->getDelay();
			text.setString(ss.str());
			t->draw(text);
		}


//		text.setFillColor(Screen::singleton->getTheme()->getTextPrimary());
//		text.setPosition(text.getPosition().x + text.getLocalBounds().width, text.getPosition().y);
//		ss.str("");
//		ss << " " << train->getPreviousIndex() << " " << train->getIndex();
//		text.setString(ss.str());
//		t->draw(text);

		if(train->isCancelled()) {
			text.setString(L"fällt aus");
			text.setFillColor(Screen::singleton->getTheme()->getTextError());
		} else {
			int minutes = ceil(difftime(time, now) / 60) + train->getDelay();
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
			text.setFillColor(Screen::singleton->getTheme()->getTextPrimary());
		}
		text.setPosition(getDisplayWidth() - text.getLocalBounds().width - padding, text.getPosition().y);
		t->draw(text);

		sf::RectangleShape rs;
		rs.setFillColor(Screen::singleton->getTheme()->getModuleOutline());
		rs.setSize(sf::Vector2f(getDisplayWidth(), lw));
		rs.setPosition(0, text.getPosition().y + textSize + padding);

		t->draw(rs);

		if(y > getDisplayHeight())
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
//		std::cout << "curlcode: " << res << std::endl;


		if(res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));
			curl_easy_cleanup(curl);
			break;
		}

		curl_easy_cleanup(curl);

//		std::cout << "result: " << result << std::endl;
		rapidjson::Document data;
		data.Parse(result.c_str());
		mutex->lock();

		for(auto i = trains.begin(); i != trains.end();) {
			if((*i)->getIndex() < 0 && (*i)->getPreviousIndex() < 0) { // Remove trains that are off-screen.
				delete *i;
				i = trains.erase(i);
			} else {
				(*i)->setInvalid(true); // will be set to false again if the train still appears in the next json
				++i;
			}
		}

		if(false && !trains.empty()) {
			trains.front()->setInvalid(true);
			for(auto train: trains)
				train->markAsUpdated();
		} else {
//			for (rapidjson::Value::ValueIterator i = data["raw"].Begin(); i != data["raw"].End(); ++i) {
//				Train* t = nullptr;
//				rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<>> &jsonTrain = *i;
//				std::string id = std::string(jsonTrain["lineref"]["identifier"].GetString()) + " " +
//								 jsonTrain["sched_time"].GetString();
//				for (auto candidate: trains) {
////				std::cout << "Comparing " << id << " to " << candidate->getID() << std::endl;
//					if (candidate->getID().compare(id) == 0) {
//						t = candidate;
//						break;
//					}
//				}
//				if (t == nullptr) {
//					t = new Train();
//					t->setID(id);
//					trains.push_back(t);
//				}
//				t->setInvalid(false);
//				t->setName(jsonTrain["line"].GetString());
//				std::string str(jsonTrain["destination"].GetString());
//
//
//				std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
//				std::wstring wide = converter.from_bytes(str);
//
//				t->setDestination(wide);
//
//				struct std::tm tm;
//				tm.tm_sec = 0;
//				tm.tm_min = 0;
//				tm.tm_hour = 0;
//				tm.tm_mday = 0;
//				tm.tm_wday = 0;
//				tm.tm_yday = 0;
//				tm.tm_mon = 0;
//				tm.tm_year = 0;
//				tm.tm_isdst = 0;
//				std::stringstream ss;
//				ss << jsonTrain["sched_date"].GetString();
//				ss << " ";
//				ss << jsonTrain["sched_time"].GetString();
//				mktime(&tm);
//				ss >> std::get_time(&tm, "%d.%m.%Y %H:%M");
//				tm.tm_sec = 0;
//				time_t time = mktime(&tm);
//				struct tm* localtm = gmtime(&time);
//				time = mktime(localtm);
////			std::cout << ss.str() << std::endl;
////			std::cout << asctime(localtime(&time)) << std::endl;
//				t->setDeparture(time);
//
//				if (!jsonTrain["delay"].IsNull()) {
//					if (jsonTrain["delay"].IsString())
//						t->setDelay(atoi(jsonTrain["delay"].GetString()));
//					else t->setDelay(jsonTrain["delay"].GetInt());
//				}
//				if (!jsonTrain["is_cancelled"].IsNull()) {
//					if (jsonTrain["is_cancelled"].IsString())
//						t->setCancelled(atoi(jsonTrain["is_cancelled"].GetString()) == 1);
//					else if (jsonTrain["is_cancelled"].IsInt())
//						t->setCancelled(jsonTrain["is_cancelled"].GetInt() == 1);
//					else t->setCancelled(jsonTrain["is_cancelled"].GetBool());
//				}
//				t->markAsUpdated();
//			}


			for(int i = 0; i < 20; ++i) {  // Create some testing trains
				time_t now;
				std::time(&now);
				struct tm* tm = std::localtime(&now);
				tm->tm_min += i;
				now = std::mktime(tm);
				std::string id = "lole" + std::to_string(tm->tm_hour) + ":" + std::to_string(tm->tm_min);
				std::cout << id << std::endl;
				Train* t = nullptr;
				for (auto candidate: trains) {
//				std::cout << "Comparing " << id << " to " << candidate->getID() << std::endl;
					if (candidate->getID().compare(id) == 0) {
						t = candidate;
						break;
					}
				}
				if (t == nullptr) {
					t = new Train();
					t->setID(id);
					t->setDeparture(now);
					t->setName("CAT");
					t->setDestination(L"Kadsen");
					trains.push_back(t);
				}
				if(i == 3)
					t->setDelay(rand() % 5);
				t->setInvalid(false);
			}


		}
		trains.sort( []( Train *a, Train *b ) {
			if(a->isInvalid() && !b->isInvalid()) return true;
			if(b->isInvalid() && !a->isInvalid()) return false;
			if(a->getDeparture() + a->getDelay() * 60 == b->getDeparture() + b->getDelay() * 60)
				return a->getDeparture() < b->getDeparture();
			return a->getDeparture() + a->getDelay() * 60 < b->getDeparture() + b->getDelay() * 60;
		});
		int i = 0;
		time_t now;
		std::time(&now);
		for(Train* t: trains)  {
			t->markAsUpdated();
			if(t->isInvalid() || ceil(difftime(t->getDeparture(), now) / 60) + t->getDelay() < -1) {
				--i;
//				std::cout << "Train " << t->getID() << " is invalid" << std::endl;
				std::cout << ceil(difftime(t->getDeparture(), now) / 60) + t->getDelay() << std::endl;
			}
		}
		for(Train* t: trains)  {
			t->setIndex(i++);
		}
		mutex->unlock();

		int d = 5000 - clock.getElapsedTime().asMilliseconds();
		if (d > 0)
			std::this_thread::sleep_for(std::chrono::milliseconds(d));
		clock.restart();
	}
}

