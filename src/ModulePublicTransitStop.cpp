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


ModulePublicTransitStop::ModulePublicTransitStop(const int stopID) {
	this->stopID = stopID;
	mutex = new std::mutex;
	thread = new std::thread(&ModulePublicTransitStop::refreshLoop, std::ref(*this));
}

ModulePublicTransitStop::ModulePublicTransitStop(const int stopID, std::vector<std::string> types): ModulePublicTransitStop(stopID) {
	this->types = types;
}


ModulePublicTransitStop::ModulePublicTransitStop(const std::string &city, const std::string &stop): Module() {
    this->city = city;
    this->stop = stop;
	mutex = new std::mutex;
	thread = new std::thread(&ModulePublicTransitStop::refreshLoop, std::ref(*this));
}


ModulePublicTransitStop::ModulePublicTransitStop(const std::string &city, const std::string &stop, std::vector<std::string> types): ModulePublicTransitStop(city, stop) {
	this->types = types;
}

ModulePublicTransitStop::~ModulePublicTransitStop() {
	active = false;
	thread->detach();
	delete thread;
	delete mutex;
}

void ModulePublicTransitStop::draw() {
	mutex->lock();
	bool useTimeline = false;
	const int testSize = 80;
	const float ypadding = 0.5;
	const int padding = 5;
	const int timelinewidth = useTimeline? 50:5;
	sf::Text text;
	text.setFont(natural);
	text.setCharacterSize(testSize);
	text.setString("HB1 Dortmund Eichlinghofen H-Bahn");
	float tw = text.getLocalBounds().width;
	float lw = 3;

	float scale = (getDisplayWidth() - 3*padding - timelinewidth) / tw;
	float textSize = testSize * scale;
	text.setCharacterSize(textSize);

	float entryHeight = (2 * textSize + lw) * (1+ypadding);

	struct timeval tv{};
	gettimeofday(&tv, NULL);
	time_t now = tv.tv_sec;

	float animationDuration = 1; // in seconds

	for(auto i = trains.begin(); i != trains.end(); ++i) {
		auto train = *i;
		float y = 0;
		if (train->getPreviousIndex() == INT32_MAX)
			y = train->getIndex() * entryHeight;
		else
			y = (train->getIndex() + (train->getPreviousIndex() - train->getIndex()) * (fmax((animationDuration -
																							  train->getAge()) /
																							 animationDuration, 0))) *
				entryHeight;
		text.setFillColor(Screen::singleton->getTheme()->getTextPrimary());
		text.setString(train->getName() + " ");
		text.setPosition(timelinewidth, padding + y);
		t->draw(text);

		text.setFillColor(Screen::singleton->getTheme()->getTextSecondary());
		text.setPosition(timelinewidth + text.getGlobalBounds().width, text.getPosition().y);
		text.setString(train->getDestination());
		t->draw(text);

		text.setFillColor(Screen::singleton->getTheme()->getTextPrimary());
		text.setString("Gleis " + train->getPlatform());
		text.setPosition(getDisplayWidth() - text.getLocalBounds().width - 2 * padding, text.getPosition().y);
		t->draw(text);

		text.setFillColor(Screen::singleton->getTheme()->getTextPrimary());
		std::stringstream ss;
		struct tm tm;
		time_t time = train->getDeparture();
		tm = *localtime(&time);
		ss << std::setw(2) << std::setfill('0') << tm.tm_hour << ":" << std::setw(2) << std::setfill('0') << tm.tm_min
		   << " ";
		text.setPosition(timelinewidth, text.getPosition().y + textSize + padding);
		text.setString(ss.str());
		t->draw(text);

		if (train->isDelayed()) {
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

		if (train->isCancelled()) {
			text.setString(L"fällt aus");
			text.setFillColor(Screen::singleton->getTheme()->getTextError());
		} else {
			float minutesf = (difftime(time, now) - (tv.tv_usec / 1000000.0)) / 60 + train->getDelay();
//			float minutesf = difftime(time, now) / 60 + train->getDelay();
			int minutesi = ceil(minutesf);
			if (minutesi == 0)
				text.setString("jetzt");
			else {
				ss.str("");
				if (minutesi < 0)
					ss << "vor ";
				if (minutesi > 0)
					ss << "in ";
				ss << abs(minutesi) << " Minute";
				if (abs(minutesi) != 1)
					ss << "n";
				text.setString(ss.str());
			}
			int pixelsperminute = entryHeight;
			if (minutesi == 0)
				minutesf = 0;
			else if (minutesi < 0)
				minutesf += 1;
			float circley = (0.5 + minutesf) * pixelsperminute;
//			if(train->getPreviousDelay() == INT32_MAX)
//				y = train->getDelay() * entryHeight;
//			else
//				y = (train->getIndex() + (train->getPreviousIndex() - train->getIndex()) * (fmax((animationDuration -
//																								  train->getAge()) / animationDuration, 0))) * entryHeight;
			if (useTimeline && circley <= getDisplayHeight()) {
				sf::CircleShape circle;
				int radius = 10;
				circle.setRadius(radius);
//				circle.setFillColor(sf::Color(255, 255, 255, 128));
				circle.setOrigin(radius, radius);
				circle.setPosition(0, circley);
				t->draw(circle);

				sf::RectangleShape rect;
				sf::Vector2f p1 = circle.getPosition();
				sf::Vector2f p2(timelinewidth - 3, y + 0.5 * entryHeight);
				rect.setOrigin(1.5, 1.5);
//			rect.setPosition(p1.x + 1.5F, p1.y - 1.5F);
				rect.setPosition(p1);
				rect.setSize(
						sf::Vector2f((float) std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2)) + 3, 3));
				rect.setRotation((float) (std::atan((p2.y - p1.y) / (p2.x - p1.x)) * 180 / M_PIl));
				rect.setFillColor(sf::Color(255, 255, 255, 128));
				t->draw(rect);
			}

			text.setFillColor(Screen::singleton->getTheme()->getTextPrimary());
		}
		text.setPosition(getDisplayWidth() - text.getLocalBounds().width - 2 * padding, text.getPosition().y);
		t->draw(text);

		if (lw > 0) {
			sf::RectangleShape rs;
			rs.setFillColor(Screen::singleton->getTheme()->getTextDisabled());
			rs.setSize(sf::Vector2f(getDisplayWidth(), lw));
			rs.setPosition(0, text.getPosition().y + textSize + padding);

			t->draw(rs);
		}
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
		rapidjson::Document data;
		try {
			CURL* curl = curl_easy_init();

			std::stringstream ss;
			if(stopID > 0)
				ss << "http://openservice-test.vrr.de/static02/XML_DM_REQUEST?outputFormat=JSON&mode=direct&sessionID=0&requestID=0&language=DE&locationServerActive=1&useRealtime=1&limit=100&type_dm=stopID&name_dm=" << stopID;
			char* url = new char[ss.str().length() +1];
			std::strcpy(url, ss.str().c_str());
			std::string result;

			curl_easy_setopt(curl, CURLOPT_URL, url);

			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);

			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunction);

			CURLcode res = curl_easy_perform(curl);


			if(res != CURLE_OK) {
				fprintf(stderr, "curl_easy_perform() failed: %s\n",
						curl_easy_strerror(res));
				curl_easy_cleanup(curl);

				std::this_thread::sleep_for(std::chrono::seconds(60));
				continue;
			}
			curl_easy_cleanup(curl);
			data.Parse(result.c_str());
		} catch (std::exception e) {

		}

		if(!data.HasParseError()) {
//		std::cout << "result: " << result << std::endl;
			mutex->lock();

			for (auto i = trains.begin(); i != trains.end();) {
				if ((*i)->getIndex() < 0 && (*i)->getPreviousIndex() < 0) { // Remove trains that are off-screen.
					delete *i;
					i = trains.erase(i);
				} else {
					(*i)->setInvalid(true); // will be set to false again if the train still appears in the next json
					++i;
				}
			}

			if (false && !trains.empty()) {
				trains.front()->setInvalid(true);
				for (auto train: trains)
					train->markAsUpdated();
			} else {
				for (rapidjson::Value::ValueIterator i = data["departureList"].Begin();
					 i != data["departureList"].End(); ++i) {
					Train* t = nullptr;
					rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<>> &jsonTrain = *i;
					rapidjson::GenericObject<false, rapidjson::GenericValue<rapidjson::UTF8<char>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>>::ValueType> jsonLine = jsonTrain["servingLine"].GetObject();
					rapidjson::GenericObject<false, rapidjson::GenericValue<rapidjson::UTF8<char>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>>::ValueType> jsonTime = jsonTrain["dateTime"].GetObject();


					if (!types.empty()) {
						bool accept = false;
						for (auto type = types.begin(); type != types.end(); ++type) {
							if (jsonLine["name"].GetString() == *type) {
								accept = true;
								break;
							}
						}
						if (!accept)
							continue;
					}


					struct std::tm tm;
					tm.tm_sec = 0;
					tm.tm_min = 0;
					tm.tm_hour = 0;
					tm.tm_mday = 0;
					tm.tm_wday = 0;
					tm.tm_yday = 0;
					tm.tm_mon = 0;
					tm.tm_year = 0;
//				tm.tm_hour = jsonTime["hour"].GetInt();
//				tm.tm_mday = jsonTime["day"].GetInt();
////				tm.tm_wday = jsonTime["minute"].GetInt();
////				tm.tm_yday = jsonTime["minute"].GetInt();
//				tm.tm_mon = jsonTime["month"].GetInt();
//				tm.tm_year = jsonTime["year"].GetInt();
					tm.tm_isdst = -1;

					std::stringstream ss;
					ss << std::setw(2) << std::setfill('0') << jsonTime["day"].GetString() << ".";
					ss << std::setw(2) << std::setfill('0') << jsonTime["month"].GetString() << ".";
					ss << std::setw(4) << std::setfill('0') << jsonTime["year"].GetString() << " ";
					ss << std::setw(2) << std::setfill('0') << jsonTime["hour"].GetString() << ":";
					ss << std::setw(2) << std::setfill('0') << jsonTime["minute"].GetString();
//					mktime(&tm);
//					std::cout << ss.str() << std::endl;
					ss >> std::get_time(&tm, "%d.%m.%Y %H:%M");
					time_t time = mktime(&tm);
//					struct tm* localtm = gmtime(&time);
//					time = mktime(localtm);

					std::string id =
							std::string(jsonLine["stateless"].GetString()) + " " + jsonTime["hour"].GetString() + ":" +
							jsonTime["minute"].GetString();
//				std::cout << id << std::endl;
//				continue;
					for (auto candidate: trains) {
						if (candidate->getID().compare(id) == 0) {
							t = candidate;
							break;
						}
					}
					if (t == nullptr) {
						t = new Train();
						t->setID(id);
						trains.push_back(t);
					}
					t->setInvalid(false);
					if (jsonTrain.HasMember("platform")) {
						t->setPlatform(jsonTrain["platform"].GetString());
					} else {
						t->setPlatform("");
					}
					t->setName(jsonLine["symbol"].GetString());
					std::string str(jsonLine["direction"].GetString());


					std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
					std::wstring wide = converter.from_bytes(str);

					t->setDestination(wide);

//			std::cout << ss.str() << std::endl;
//			std::cout << asctime(localtime(&time)) << std::endl;
					t->setDeparture(time);
					int delay = 0;
					if (jsonLine.HasMember("delay") && !jsonLine["delay"].IsNull()) {
						if (jsonLine["delay"].IsString())
							delay = atoi(jsonLine["delay"].GetString());
						else delay = jsonLine["delay"].GetInt();
					}
					t->setCancelled(delay == -9999);
					t->setDelay(delay == -9999? 0 : delay);
					t->markAsUpdated();
				}


//			for(int i = 0; i < 20; ++i) {  // Create some testing trains
//				time_t now;
//				std::time(&now);
//				struct tm* tm = std::localtime(&now);
//				tm->tm_min += i;
//				now = std::mktime(tm);
//				std::string id = "lole" + std::to_string(tm->tm_hour) + ":" + std::to_string(tm->tm_min);
//				std::cout << id << std::endl;
//				Train* t = nullptr;
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
//					t->setDeparture(now);
//					t->setName("CAT");
//					t->setDestination(L"Kadsen");
//					trains.push_back(t);
//				}
//				if(i == 3)
//					t->setDelay(rand() % 5);
//				t->setInvalid(false);
//			}


			}
			trains.sort([](Train* a, Train* b) {
				if (a->isInvalid() && !b->isInvalid()) return true;
				if (b->isInvalid() && !a->isInvalid()) return false;
				if (a->getDeparture() + a->getDelay() * 60 == b->getDeparture() + b->getDelay() * 60)
					return a->getDeparture() < b->getDeparture();
				return a->getDeparture() + a->getDelay() * 60 < b->getDeparture() + b->getDelay() * 60;
			});
			int i = 0;
			time_t now;
			std::time(&now);
			for (Train* t: trains) {
				t->markAsUpdated();
				if (t->isInvalid() || ceil(difftime(t->getDeparture(), now) / 60) + t->getDelay() < -1) {
					--i;
//				std::cout << "Train " << t->getID() << " is invalid" << std::endl;
					time_t time = t->getDeparture();
					struct tm tm;
					tm = *localtime(&time);
//					std::cout << ceil(difftime(t->getDeparture(), now) / 60) + t->getDelay() << std::endl;
//					std::cout << tm.tm_mday << "." << tm.tm_mon << "." << tm.tm_year << " " << tm.tm_hour << ":"
//							  << tm.tm_min << " " << t->getDelay() << std::endl;
				}
			}
			for (Train* t: trains) {
				t->setIndex(i++);
			}

			mutex->unlock();

		}

		int d = 15000 - clock.getElapsedTime().asMilliseconds();
		if (d > 0)
			std::this_thread::sleep_for(std::chrono::milliseconds(d));
		clock.restart();
	}
}