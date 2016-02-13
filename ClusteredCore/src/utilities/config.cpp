/*The MIT License (MIT)

 Copyright (c) [2015] [Sawyer Hopkins]

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.*/

#include "config.h"

namespace PSim {

void config::parseLine(std::string& line) {
	if (line.at(0) != '#') {
		//Get the position of the equal sign.
		int pos = line.find("=");
		//Add the left of the equal sign to the key.
		string key = line.substr(0, pos);
		//Remove any leading or trailing white space.
		//No Regex in g++ 4.8 :'(
		while (std::isspace(*key.begin())) {
			key.erase(key.begin());
		}
		while (std::isspace(*key.rbegin())) {
			key.erase(key.length() - 1);
		}
		//key = regex_replace(key, std::regex("(^\\s+)"), "");
		//key = regex_replace(key, std::regex("(\\s+$)"), "");
		//Add the right of the equal sign to the value.
		line.erase(0, pos + 1);
		string val = line;
		while (std::isspace(*val.begin())) {
			val.erase(val.begin());
		}
		while (std::isspace(*val.rbegin())) {
			val.erase(val.length() - 1);
		}
		//val = regex_replace(val, std::regex("(^\\s+)"), "");
		//val = regex_replace(val, std::regex("(\\s+$)"), "");
		options[key] = val;
	}
}

config::config(string fileName) {
	hideOutput();

	//Open the config file.
	ifstream cfg;
	cfg.open(fileName, std::ios_base::in);

	for (std::string line; getline(cfg, line);) {
		parseLine(line);
	}

}

config::~config() {
}

template<typename T> T config::getParam(string key, T def) {
	//Checks for key in the file.
	T val = def;
	bool exists = containsKey(key);

	//If it exists grab the value.
	if (exists == true) {
		val = dynamic_cast<T>(options[key]);
	}

	//If we want output then write what happened.
	if (suppressOutput == false) {
		if (exists == false) {
			chatterBox.consoleMessage("Option: '" + key  + "' missing", 1);
			chatterBox.consoleMessage("Using default", 1);
		}
		chatterBox.consoleMessage(key + ": " + val, 3);
	}

	return val;

}

template<> int config::getParam<int>(string key, int def) {
	int val = def;
	bool exists = containsKey(key);

	if (exists == true) {
		val = std::stoi(options[key], nullptr);
	}

	if (suppressOutput == false) {
		if (exists == false) {
			chatterBox.consoleMessage("Option: '" + key  + "' missing", 1);
			chatterBox.consoleMessage("Using default", 1);
		}
		chatterBox.consoleMessage(key + ": " + to_string(val), 3);
	}

	return val;

}

template<> float config::getParam<float>(string key, float def) {
	float val = def;
	bool exists = containsKey(key);

	if (exists == true) {
		val = std::stof(options[key], nullptr);
	}

	if (suppressOutput == false) {
		if (exists == false) {
			chatterBox.consoleMessage("Option: '" + key  + "' missing", 1);
			chatterBox.consoleMessage("Using default", 1);
		}
		chatterBox.consoleMessage(key + ": " + to_string(val), 3);
	}

	return val;

}

template<> double config::getParam<double>(string key, double def) {
	double val = def;
	bool exists = containsKey(key);

	if (exists == true) {
		val = std::stod(options[key], nullptr);
	}

	if (suppressOutput == false) {
		if (exists == false) {
			chatterBox.consoleMessage("Option: '" + key  + "' missing", 1);
			chatterBox.consoleMessage("Using default", 1);
		}
		chatterBox.consoleMessage(key + ": " + to_string(val), 3);
	}

	return val;

}

template<> string config::getParam<string>(string key, string def) {
	string val = def;
	bool exists = containsKey(key);

	if (exists == true) {
		val = options[key];
	}

	if (suppressOutput == false) {
		if (exists == false) {
			chatterBox.consoleMessage("Option: '" + key  + "' missing", 1);
			chatterBox.consoleMessage("Using default", 1);
		}
		chatterBox.consoleMessage(key + ": " + val, 3);
	}

	return val;

}

}

