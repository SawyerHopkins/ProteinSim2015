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

#include "system.h"
#include <dlfcn.h>

using namespace std;
using namespace PSim;

void runAnalysis(std::queue<std::string>* analysisArgs)
{
	if (analysisArgs->size() < 2)
	{
		util::writeTerminal("\nNo testing parameters defined.\n", Colour::Red);
		return;
	}
	if (analysisArgs->size() < 3)
	{
		util::writeTerminal("\nNo analysis routines defined.\n", Colour::Magenta);
		return;
	}

	string analysisName = PSim::util::tryPop(analysisArgs);
	string timeStamp = PSim::util::tryPop(analysisArgs);

	config* cfg =new config(analysisName + "/sysConfig.cfg");

	util::writeTerminal("\nLoading particle system.\n", Colour::Green);
	PSim::system* sys = PSim::system::loadAnalysis(cfg, analysisName, timeStamp,NULL);
	sys->analysisManager(analysisArgs);
}
