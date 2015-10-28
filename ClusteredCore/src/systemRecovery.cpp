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
	using namespace std;

	/********************************************//**
	*-----------------SYSTEM RECOVERY----------------
	 ***********************************************/

	int system::particlesInFile(std::string sysState, std::string timeStamp)
	{
		std::string fullPath = sysState + "/snapshots/time-" + timeStamp + "/recovery.txt";
		ifstream state;
		state.open(fullPath, ios_base::in);

		int count = 0;
		for(std::string line; std::getline(state, line);)
		{
			count++;
		}
		return count;
	}

	int system::readParticles(system* oldSys, std::string sysState, std::string timeStamp, int bsize)
	{
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
		return count;
	}

	void system::readSettings(system* oldSys, configReader::config* cfg)
	{
		cfg->showOutput();
		oldSys->concentration = cfg->getParam<double>("Concentration",0);
		oldSys->cellSize = cfg->getParam<int>("cellSize",0);
		oldSys->cellScale = cfg->getParam<int>("cellScale",0);
		oldSys->temp = cfg->getParam<double>("temp",0);
		oldSys->currentTime = 0;
		oldSys->dTime = cfg->getParam<double>("dTime",0);
		oldSys->outputFreq = cfg->getParam<int>("outputFreq",0);
		oldSys->outXYZ = cfg->getParam<int>("outXYZ",0);
		oldSys->cycleHour = cfg->getParam<double>("cycleHour",0);
		oldSys->seed = cfg->getParam<int>("seed",0);
		oldSys->boxSize = cfg->getParam<int>("boxSize",0);
		cfg->hideOutput();
	}

	void system::createRewindDir(system* oldSys)
	{
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
	}

	system* system::loadFromFile(configReader::config* cfg, std::string sysState, std::string timeStamp, integrators::I_integrator* sysInt, physics::forces* sysFcs)
	{
		utilities::util::writeTerminal("\n\nLoading recovery state...\n", utilities::Colour::Green);

		system* oldSys = new system();

		int bsize = cfg->getParam<int>("boxSize",0);
		int nParts = cfg->getParam<int>("nParticles",0);

		// Load in the particles.
		oldSys->particles = new particle*[nParts];
		int count = readParticles(oldSys, sysState, timeStamp, bsize);

		// Maybe this should throw an error for count != nParts?
		std::cout << "-Found " << count << " / " << nParts << " particles.\n";

		// Pulls in the old system configuration. Don't recalculate it incase the settings.cfg gets changed incorrectly.
		readSettings(oldSys, cfg);
		oldSys->trialName = sysState + "/-rewind-" + timeStamp;
		oldSys->nParticles = count;
		oldSys->integrator = sysInt;
		oldSys->sysForces = sysFcs;

		oldSys->initCells(oldSys->cellScale);
		createRewindDir(oldSys);
		oldSys->writeSystemInit();
		return oldSys;
	}

	system* system::loadAnalysis(configReader::config* cfg, std::string sysState, std::string timeStamp)
	{
		system* oldSys = new system();

		// Read in each particle.
		int nParts = particlesInFile(sysState, timeStamp);
		oldSys->particles = new particle*[nParts];

		int bsize = cfg->getParam<int>("boxSize",0);
		int count = readParticles(oldSys, sysState, timeStamp, bsize);
		std::cout << "-Found " << count << " / " << nParts << " particles.\n";

		readSettings(oldSys, cfg);
		oldSys->nParticles = count;
		oldSys->trialName = sysState + "/-analysis-" + timeStamp;

		oldSys->initCells(oldSys->cellScale);
		createRewindDir(oldSys);
		return oldSys;
	}
}