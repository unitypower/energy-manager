//============================================================================
// Name        : Agile_Parser.cpp
// Author      : Jim McKenzie
// Version     :
// Copyright   : Your copyright notice
// Description :
//============================================================================

#include <iostream>
#include <string.h>
#include <curl/curl.h>
#include <sstream>


using namespace std;

#define PORT 80
#define MY_USER_AND_PWD "sk_live_3BDVj6dEjmKs9mjXljfyoVZy:"
#define YOUR_URL "https://api.octopus.energy/v1/products/AGILE-18-02-21/electricity-tariffs/E-1R-AGILE-18-02-21-A/standard-unit-rates/"

const char *tariff_array [48][48];
bool tariff_24(string sample);


size_t WriteCallback(char *contents, size_t size, size_t nmemb, void *userp)
{
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int main(int argc, char * argv[])
{
    curl_global_init(CURL_GLOBAL_ALL);

    CURL* easyhandle = curl_easy_init();
    string readBuffer;

    curl_easy_setopt(easyhandle, CURLOPT_URL,YOUR_URL);
    curl_easy_setopt(easyhandle, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(easyhandle, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(easyhandle, CURLOPT_USERPWD, MY_USER_AND_PWD);
    curl_easy_setopt(easyhandle, CURLOPT_PROXYPORT, 8080L);
    curl_easy_setopt(easyhandle, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(easyhandle, CURLOPT_WRITEDATA, &readBuffer);

    curl_easy_perform(easyhandle);

    //cout << readBuffer << endl;

    if (tariff_24(readBuffer) == false){
    	cout << "Parse Failed" << endl;
    }

    return 0;
}

bool tariff_24(string sample){

	cout << "Fed to tariff_24" << sample << endl;

	istringstream iss(sample);

	string item;
	string date;
	string time;
	string price;

	int state = 0;
	int period = 0;


	while (getline(iss, item, '"')){
		cout << item << endl;


		if (state == 1){  //extracts half hourly price string

			// truncates to number only
			item.erase (0,1);
			item.erase (item.end()-1);
			price = item;

			//cout << "half hourly price:" << item << endl;
		}

		if (state == 4){  //extracts half hour start time

			// truncates to number only
			date = item.substr (0,10);
			time = item.substr (11,5);
			//cout << "date:" << date << endl;
			//cout << "start time:" << time << endl;
		}
		if (state == 2){
			state = 4;
		}
		else{
			state = 0;
		}


		if (item == "value_inc_vat" && state != 4){
			state = 1;
		}
		else if (item == "valid_from" && state != 4) {
			state = 2;
		}
		else if (item == "valid_to" && state != 4){
			state = 3;
		}
		else if (state != 4) {
			state = 0;
		}

		//cout << state << endl;
		tariff_array [period] = price.c_str();


	}
	return true;

}
