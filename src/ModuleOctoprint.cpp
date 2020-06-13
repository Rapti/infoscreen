//
// Created by leon on 13.06.20.
//

#include <rapidjson/document.h>
#include <curl/curl.h>
#include <sstream>
#include <iomanip>
#include "ModuleOctoprint.h"
#include "Screen.h"

ModuleOctoprint::ModuleOctoprint(const string &apiUrl, const string &apiKey) : apiUrl(apiUrl), apiKey(apiKey), jobProgress(-1), printTime(-1), printTimeLeft(-1) {
	active = true;
	refreshThread = new std::thread(&ModuleOctoprint::refreshLoop, std::ref(*this));}

ModuleOctoprint::~ModuleOctoprint() {
}

void ModuleOctoprint::draw() {
	int outline = 3;

	sf::Text text;
	text.setFillColor(Screen::singleton->getTheme()->getTextPrimary());
	text.setFont(monospace);
	text.setCharacterSize((getDisplayHeight() / 2 - 2*outline)*0.6);

	m.lock();

	sf::RectangleShape rect;
	rect.setSize(sf::Vector2f(getDisplayWidth(), outline));
	rect.setOrigin(0.0, outline / 2.0);
	rect.setPosition(0, getDisplayHeight() / 2);
	rect.setFillColor(Screen::singleton->getTheme()->getDiagramFill());
	t->draw(rect);
	float barPos = (getDisplayWidth()-outline) * jobProgress;
	if(jobProgress != -1) {
		// Horizontal white bar
		rect.setSize(sf::Vector2f(getDisplayWidth() * jobProgress, outline));
		rect.setFillColor(Screen::singleton->getTheme()->getDiagramLine());
		t->draw(rect);

		// Vertical white bar
		rect.setOrigin(0.0, 0.0);
		rect.setPosition(barPos, 0);
		rect.setSize(sf::Vector2f(outline, (getDisplayHeight()+outline) / 2));
		t->draw(rect);

		// Fill
		rect.setOrigin(0.0, 0.0);
		rect.setPosition(0, 0);
		rect.setFillColor(Screen::singleton->getTheme()->getDiagramFill());
		rect.setSize(sf::Vector2f(getDisplayWidth() * jobProgress, getDisplayHeight() / 2));
		t->draw(rect);

		std::stringstream ss;

		if(printTime != -1) {
			int hours = printTime / 3600;
			int minutes = (printTime / 60) % 60;
			int seconds = printTime % 60;

			if(hours > 0) {
				ss << hours << ":" << setw(2) << setfill('0');
			}
			ss << minutes << ":" << setw(2) << setfill('0') << seconds;

			text.setString(ss.str());
			text.setPosition(outline, outline);
			auto bounds = text.getGlobalBounds();
			if(bounds.left + bounds.width + outline > barPos) {
				text.setPosition(barPos + 2*outline, outline);
			}
			t->draw(text);
			ss.str("");
		}
		if(printTimeLeft != -1) {
			int hours = printTimeLeft / 3600;
			int minutes = (printTimeLeft / 60) % 60;
			int seconds = printTimeLeft % 60;

			if(hours > 0) {
				ss << hours << ":" << setw(2) << setfill('0');
			}
			ss << minutes << ":" << setw(2) << setfill('0') << seconds;

			text.setString(ss.str());
			auto bounds = text.getGlobalBounds();
			if(barPos + bounds.width + 3*outline > getDisplayWidth()) {
				text.setPosition(barPos - 2*outline - bounds.width, outline);
			} else {
				text.setPosition(getDisplayWidth() - 2*outline - bounds.width, outline);
			}
			t->draw(text);
			ss.str("");
		}

		ss << floor(jobProgress * 100) << "," << ((int) floor(jobProgress * 1000)) % 10 << " %";
		text.setString(ss.str());
		auto bounds = text.getGlobalBounds();
		float desiredPos = (getDisplayWidth() - bounds.width) / 2;
		if(jobProgress <= 0.5 && barPos + 2*outline > desiredPos) {
			desiredPos = barPos + 2*outline;
		} else if(jobProgress > 0.5 && barPos < desiredPos + bounds.width + outline) {
			desiredPos = barPos - bounds.width - outline;
		}
		text.setPosition(desiredPos, outline);
		t->draw(text);
	}

	text.setFont(natural);
	float leftOffset = outline;
	if(!status.empty()) {
		text.setString(status);
		text.setPosition(leftOffset, getDisplayHeight() / 2 + outline);
		t->draw(text);
		leftOffset = leftOffset * 2 + text.getLocalBounds().width;
	}
	if(!displayFile.empty()) {
		text.setString(displayFile);
		text.setPosition(max(leftOffset, getDisplayWidth() - 2*outline - text.getLocalBounds().width), getDisplayHeight() / 2 + outline);
		t->draw(text);
	}

	m.unlock();
}

static size_t writefunction(char *data, size_t size, size_t nmemb,
							std::string *writerData) {
	if(writerData == NULL)
		return 0;
	writerData->append(data, size*nmemb);

	return size * nmemb;
}

void ModuleOctoprint::refreshLoop() {

	char apiKeyHeader[apiKey.length() +12];
	strcpy(apiKeyHeader, ("X-Api-Key: " + apiKey).c_str());

	char progressApiUrl[apiUrl.length() +5];
	strcpy(progressApiUrl, (apiUrl + "/job").c_str());

	while(active) {
		clock.restart();
		rapidjson::Document data;
		try {
			CURL* curl = curl_easy_init();
			std::string result;

			curl_easy_setopt(curl, CURLOPT_URL, progressApiUrl);

			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);

			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunction);

			struct curl_slist *chunk = NULL;
			/* Add a custom header */
			chunk = curl_slist_append(chunk, apiKeyHeader);
			/* set our custom set of headers */
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

			CURLcode res = curl_easy_perform(curl);


			if(res != CURLE_OK) {
				fprintf(stderr, "curl_easy_perform() failed: %s\n",
						curl_easy_strerror(res));
				curl_easy_cleanup(curl);
				cout << progressApiUrl << endl;

				std::this_thread::sleep_for(std::chrono::seconds(30));
				continue;
			}
			curl_easy_cleanup(curl);

			data.Parse(result.c_str());
		} catch (std::exception e) {

		}

		if(!data.HasParseError()) {
			m.lock();

			auto job = data["job"].GetObject();
			auto progress = data["progress"].GetObject();

			if(data["state"].IsString()) {
				status = data["state"].GetString();
				if(status.find_first_of("(") != string::npos)
					status.erase(status.find_first_of("("), string::npos);
				if(status.find_first_of(":") != string::npos)
					status.erase(status.find_first_of(":"), string::npos);
			}
			else status = "";

			if(job["file"].IsObject()) {
				auto f = job["file"].GetObject();
				if(f["display"].IsString()) {
					displayFile = f["display"].GetString();
					if(displayFile.ends_with(".gcode"))
						displayFile.erase(displayFile.length() - 6, string::npos);
				} else displayFile = "";
				if(f["name"].IsString())
					file = f["name"].GetString();
				else file = "";
			} else {
				file = "";
				displayFile = "";
			}

			if(progress["completion"].IsDouble())
				jobProgress = progress["completion"].GetDouble();
			else jobProgress = -1;
			if(progress["printTime"].IsInt())
				printTime = progress["printTime"].GetInt();
			else printTime = -1;
			if(progress["printTimeLeft"].IsInt())
				printTimeLeft = progress["printTimeLeft"].GetInt();
			else printTimeLeft = -1;
			m.unlock();
		} else {
			cout << "JSON Parse Error!" << endl;
			cout << data.GetParseError() << endl;
		}
		int waitFor = jobProgress == -1? 60000 : 500 - clock.getElapsedTime().asMilliseconds();
		if(waitFor > 0)
			std::this_thread::sleep_for(std::chrono::milliseconds(waitFor));
	}
}

