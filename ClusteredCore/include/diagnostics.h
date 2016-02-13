#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H
#include <string>
#include <iostream>
#include <exception>
#include "structs/type3.h"

namespace PSim {

using namespace std;

/**
 * @class Diagnostics
 * @author Sawyer Hopkins
 * @date 02/12/16
 * @file Diagnostics.h
 * @brief Standardized messaging client.
 */
class Diagnostics {

protected:
	int chatter;
	void addToChatter() { chatter+=1; }
public:
	//Header Version.
	static const int version = 1;
	Diagnostics() { chatter = 0;}
	~Diagnostics() { delete &chatter; };
	void clearChatter(int count = -1);
	void endErrorLog();
	void consolePrompt(string _message);
	void logErrorMessage(string _message);
	void resetChatterCount() { chatter = 0; };
	void startErrorLog(int _code, string _name);
	void consoleMessage(string _message, int level = 0);
};

}

#endif // DIAGNOSTICS_H
