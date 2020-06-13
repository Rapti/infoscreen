//
// Created by leon on 13.06.20.
//

#include <rapidjson/document.h>
#include <curl/curl.h>
#include "ModuleOctoprint.h"

ModuleOctoprint::ModuleOctoprint(const string &apiUrl, const string &apiKey) : apiUrl(apiUrl), apiKey(apiKey) {
	cout << "Instantiating Octoprint Module" << endl;
	active = true;
	refreshThread = new std::thread(&ModuleOctoprint::refreshLoop, std::ref(*this));}

ModuleOctoprint::~ModuleOctoprint() {
}

void ModuleOctoprint::draw() {

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

	while(active) {
		rapidjson::Document data;
		try {
			CURL* curl = curl_easy_init();
			std::string result;

			curl_easy_setopt(curl, CURLOPT_URL, apiUrl.c_str());

			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);

			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunction);

			struct curl_slist *chunk = NULL;
			/* Add a custom header */
			chunk = curl_slist_append(chunk, "X-Api-Key: ");
			/* set our custom set of headers */
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

			CURLcode res = curl_easy_perform(curl);


			if(res != CURLE_OK) {
				fprintf(stderr, "curl_easy_perform() failed: %s\n",
						curl_easy_strerror(res));
				curl_easy_cleanup(curl);
				active = false;
				break;
			}
			curl_easy_cleanup(curl);
			data.Parse(result.c_str());
		} catch (std::exception e) {

		}

		if(!data.HasParseError()) {
			m.lock();

			auto job = data["job"].GetObject();
			auto progress = data["progress"].GetObject();

			if(job["file"].IsObject()) {
				auto f = job["file"].GetObject();
				if(f["display"].IsString())
					file = f["display"].GetString();
				else file = "null";
			} else file = "";

			if(progress["completion"].IsDouble())
				jobProgress = progress["completion"].GetDouble();
			else jobProgress = -1;
			if(progress["printTime"].IsInt())
				printTime = progress["printTime"].GetInt();
			else printTime = -1;
			if(progress["printTimeLeft"].IsInt())
				printTimeLeft = progress["printTimeLeft"].GetInt();
			else printTimeLeft = -1;
		}
	}
}

