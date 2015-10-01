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

#include "utilities.h"

using namespace std;

namespace utilities
{
	void util::loadBar(float x0, int n, long counter, int w)
	{
		/**************************************************************************************//**
		*----------------------------------------SOURCE FROM---------------------------------------
		*------------------------------------------------------------------------------------------
		*---https://www.ross.click/2011/02/creating-a-progress-bar-in-c-or-any-other-console-app---
		*------------------------------------------------------------------------------------------
		******************************************************************************************/
		//if ( (x != n) && (x % (n/100+1) != 0) ) return;

		int x = (int)x0;

		//Choose when to update console.
		if ( (x != n) && (counter % 100 != 0) ) return;

		float ratio  =  x/(float)n;
		int   c      =  ratio * w;

		cout.precision(4);

		cout << setw(3) << (int)(ratio*100) << "% [";
		for (int x=0; x<c; x++) cout << "=";
		for (int x=c; x<w; x++) cout << " ";
		cout << "] - " << x0 << "\r" << flush;
	}

	void util::setTerminalColour(Colour c)
	{
		switch (c)
		{
			case Black :
				std::cout << __BLACK;
				break;
			case Red :
				std::cout << __RED;
				break;
			case Green :
				std::cout << __GREEN;
				break;
			case Brown :
				std::cout << __BROWN;
				break;
			case Blue :
				std::cout << __BLUE;
				break;
			case Magenta :
				std::cout << __MAGENTA;
				break;
			case Cyan :
				std::cout << __CYAN;
				break;
			case Grey :
				std::cout << __GREY;
				break;
			case Normal :
				std::cout << __NORMAL;
				break;
		}
	}

	void util::writeTerminal(std::string text, utilities::Colour c = Normal)
	{
		//Change colour, write text, reset colour.
		setTerminalColour(c);
		std::cout << text;
		setTerminalColour(Normal);
	}

	void util::clearLines(int numLines)
	{
		if (numLines > 0)
		{
			for (int i = 0; i < (numLines); i++)
			{
				//beginning of line.
				std::cout << "\r";
				//clear line.
				std::cout << "\033[K";
				//up one line
				if (i < (numLines - 1))
				{
					std::cout << "\033[A";
				}
			}
		}
	}
}

