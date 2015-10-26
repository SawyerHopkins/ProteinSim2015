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

namespace simulation
{

	/********************************************//**
	*-----------------SYSTEM RECOVERY----------------
	 ***********************************************/

	system* system::loadFromFile(configReader::config* cfg, std::string sysState, std::string timeStamp, integrators::I_integrator* sysInt, physics::forces* sysFcs)
	{
		using namespace std;

		system* oldSys = new system();

		utilities::util::writeTerminal("\n\nLoading recovery state...\n", utilities::Colour::Green);

		cfg->showOutput();
		int bsize = cfg->getParam<int>("boxSize",0);
		int nParts = cfg->getParam<int>("nParticles",0);

		oldSys->particles = new particle*[nParts];

		std::string fullPath = sysState + "/snapshots/time-" + timeStamp + "/recovery.txt";
		cout << "-Reading: " << fullPath << "\n";
		ifstream state;
		state.open(fullPath, ios_base::in);

		// Read in each particle.
		int count = 0;
		for(std::string line; std::getline(state, line);)
		{
			istringstream data(line);

			float x, y, z;
			float x0, y0, z0;
			float fx, fy, fz;
			float fx0, fy0, fz0;
			float m, r;

			data >> x >> y >> z;
			data >> x0 >> y0 >> z0;
			data >> fx >> fy >> fz;
			data >> fx0 >> fy0 >> fz0;
			data >> m >> r;

			// Set each particle to the oldest know position and advance to the newest known position.
			oldSys->particles[count] = new particle(count);
			oldSys->particles[count]->setPos(x0,y0,z0,bsize);
			oldSys->particles[count]->updateForce(fx0,fy0,fz0,0,NULL,false);
			oldSys->particles[count]->nextIter();
			oldSys->particles[count]->setPos(x,y,z,bsize);
			oldSys->particles[count]->updateForce(fx,fy,fz,0,NULL,false);
			oldSys->particles[count]->setMass(m);
			oldSys->particles[count]->setRadius(r);

			count++;
		}

		// Maybe this should throw an error for count != nParts?
		std::cout << "-Found " << count << " / " << nParts << " particles.\n";

		// Pulls in the old system configuration. Don't recalculate it incase the settings.cfg gets changed incorrectly.
		oldSys->trialName = sysState + "/-rewind-" + timeStamp;
		oldSys->nParticles = count;
		oldSys->concentration = cfg->getParam<double>("Concentration",0);
		oldSys->boxSize = bsize;
		oldSys->cellSize = cfg->getParam<int>("cellSize",0);
		oldSys->cellScale = cfg->getParam<int>("cellScale",0);
		oldSys->temp = cfg->getParam<double>("temp",0);
		oldSys->currentTime = 0;
		oldSys->dTime = cfg->getParam<double>("dTime",0);
		oldSys->outputFreq = cfg->getParam<int>("outputFreq",0);
		oldSys->outXYZ = cfg->getParam<int>("outXYZ",0);
		oldSys->cycleHour = cfg->getParam<double>("cycleHour",0);
		oldSys->seed = cfg->getParam<int>("seed",0);

		oldSys->integrator = sysInt;
		oldSys->sysForces = sysFcs;

		oldSys->initCells(oldSys->cellScale);
		cfg->hideOutput();

		//Check that the provided directory exists.
		bool validDir = checkDir(oldSys->trialName);
		if (validDir == true)
		{
			utilities::util::writeTerminal("\nTrial name already exists. Overwrite (y,n): ", utilities::Colour::Magenta);

			//Check user input
			std::string cont;
			std::cin >> cont;

			if (cont != "Y" && cont != "y")
			{
				oldSys->trialName = runSetup();
			}
		}
		else
		{
			//Attempt to make the directory.
			mkdir(oldSys->trialName.c_str(),0777);

			//Check that we were able to make the desired directory.
			validDir = checkDir(oldSys->trialName);
			if (validDir == false)
			{
				oldSys->trialName = runSetup();
			}
		}

		oldSys->writeSystemInit();

		return oldSys;
	}

}