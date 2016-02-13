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

#include "diagnostics.h"

using namespace std;

namespace PSim
{
void Diagnostics::startErrorLog(int _code, string _name) {
	cout << "Error " << _code << ": " << _name << "\n";
	addToChatter();
}
void Diagnostics::logErrorMessage(string _message) {
	cout << "---" << _message << "\n";
	addToChatter();
}

void Diagnostics::endErrorLog() {
	cout << "-------END ERROR-------";
	addToChatter();
}
void Diagnostics::consoleMessage(string _message, int level) {
	string spacer = "";
	for (int i = 0; i < level; i++) spacer = spacer + "-";
	cout << spacer << _message << "\n";
	addToChatter();
}

void Diagnostics::consolePrompt(string _message) {
	cout << _message;
}

void Diagnostics::clearChatter(int count) {
	int numLines = (count < 0) ? chatter : count;
	if (numLines > 0) {
		for (int i = 0; i < (numLines+1); i++) {
			//beginning of line.
			std::cout << "\r";
			//clear line.
			std::cout << "\033[K";
			if (i < (numLines))
			{
				std::cout << "\033[A";
			}
		}
	}
	resetChatterCount();
}
}
