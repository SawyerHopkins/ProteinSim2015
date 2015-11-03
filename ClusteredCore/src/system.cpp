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
	*-------------CONSTRUCTOR/DESTRUCTOR-------------
	************************************************/

	system::system(configReader::config* cfg, integrators::I_integrator* sysInt, physics::forces* sysFcs)
	{

		//Sets the trial name
		trialName = cfg->getParam<std::string>("trialName", "");

		if (trialName == "")
		{
			runSetup();
		}
		else
		{
			//Check that the provided directory exists.
			bool validDir = checkDir(trialName);
			if (validDir == true)
			{
				utilities::util::writeTerminal("\nTrial name already exists. Overwrite (y,n): ", utilities::Colour::Magenta);

				//Check user input
				std::string cont;
				std::cin >> cont;

				if (cont != "Y" && cont != "y")
				{
					trialName = runSetup();
				}
			}
			else
			{
				//Attempt to make the directory.
				mkdir(trialName.c_str(),0777);

				//Check that we were able to make the desired directory.
				validDir = checkDir(trialName);
				if (validDir == false)
				{
					trialName = runSetup();
				}
			}
		}

		//Set time information
		currentTime = 0;
		dTime = cfg->getParam<double>("timeStep",0.001);

		//Set the random number generator seed.
		seed = cfg->getParam<int>("seed",90210);

		//Sets the system temperature.
		temp = cfg->getParam<double>("temp",1.0);

		//Set the number of particles.
		nParticles = cfg->getParam<int>("nParticles",1000);

		//How often to output snapshots.
		outputFreq = cfg->getParam<int>("outputFreq",int(1.0/dTime));

		//Option to output XYZ format for clusters
		outXYZ = cfg->getParam<int>("XYZ",0);

		//Set the integration method.
		integrator = sysInt;

		//Set the internal forces.
		sysForces = sysFcs;

		//Set the concentration.
		double conc = cfg->getParam<double>("conc",0.01);

		//Set the radius.
		double r = cfg->getParam<double>("radius",0.5);

		//Set the mass.
		double m = cfg->getParam<double>("mass",1.0);

		//Set the scale.
		int scale = 0;
		scale = cfg->getParam<int>("scale",4);

		//Create a box based on desired concentration.
		double vP = nParticles*(4.0/3.0)*atan(1.0)*4.0*r*r*r;
		boxSize = (int) cbrt(vP / conc);

		//Calculates the number of cells needed.
		cellSize = boxSize / scale;
		boxSize = cellSize * scale;
		cellScale = scale;
		int numCells = pow(scale,3.0);

		//Sets the actual concentration.
		concentration = vP/pow(boxSize,3.0);

		std::cout << "---System concentration: " << concentration << "\n";

		//Create particles.
		initParticles(r,m);

		//Create cells.
		initCells(cellScale);
		std::cout << "Created: " << numCells << " cells from scale: " <<  cellScale << "\n";

		writeSystemInit();
	}

	system::~system()
	{
		//Deletes the particles
		for (int i=0; i < nParticles; i++)
		{
			delete particles[i];
		}
		delete[] particles;

		//Delete the constants.
		delete &nParticles;
		delete &concentration;
		delete &boxSize;
		delete &cellSize;
		delete &temp;
		delete &currentTime;
		delete &dTime;
		delete &seed;
		delete[] integrator;
		delete[] sysForces;
	}

	void system::run(double endTime)
	{
		cycleHour = (endTime / dTime) / 3600.0;
		//Create the snapshot name.
		std::string snap = trialName + "/snapshots";
		mkdir(snap.c_str(),0777);

		//Create the movie folder
		std::string mov = trialName + "/movie";
		mkdir(mov.c_str(),0777);

		//Debugging counter.
		int counter = 0;

		//Diagnostics timer.
		debugging::timer* tmr = new debugging::timer();
		tmr->start();

		bool quenched = false;

		//Run system until end time.
		while (currentTime < endTime)
		{
			if ((currentTime > (0.5)*endTime) && (quenched == false))
			{
				for (std::vector<physics::IForce*>::iterator it = sysForces->getBegin(); it != sysForces->getEnd(); ++it)
				{
					(*it)->quench();
				}
				quenched = true;
			}

			//Get the forces acting on the system.
			sysForces->getAcceleration(nParticles,boxSize,currentTime,cells,particles);
			//Get the next system.
			integrator->nextSystem(currentTime, dTime, nParticles, boxSize, cells, particles, sysForces);
			//Call cell manager.
			updateCells();

			//Output a snapshot every second.
			if ( (counter % outputFreq) == 0 )
			{
				if (currentTime > 0)
				{
					utilities::util::clearLines(14);
				}
				writeSystemState(tmr);
			}

			//Update loading bar.
			utilities::util::loadBar(currentTime,endTime,counter);

			//Increment counters.
			currentTime += dTime;
			counter++;
		}
	}

}

