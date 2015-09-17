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
					runSetup();
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
					runSetup();
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
		initCells<<1,1>>(numCells, cellScale);
		std::cout << "Created: " << numCells << " cells from scale: " <<  cellScale << "\n";

		//Copy over system variables to device.
		cudaMalloc((void **)&d_nParticles, sizeof(int));
		cudaMalloc((void **)&d_boxSize, sizeof(int));
		cudaMalloc((void **)&d_dTime, sizeof(double));
		cudaMalloc((void **)&d_currentTime, sizeof(double));

		cudaMemcpy(d_nParticles, &nParticles, sizeof(int), cudaMemcpyHostToDevice);
		cudaMemcpy(d_boxSize, &boxSize, sizeof(int), cudaMemcpyHostToDevice);
		cudaMemcpy(d_dTime, &dTime, sizeof(double), cudaMemcpyHostToDevice);
		cudaMemcpy(d_currentTime, &currentTime, sizeof(double), cudaMemcpyHostToDevice);

		writeSystemInit();

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

		//Run system until end time.
		while (currentTime < endTime)
		{
			//Get the forces acting on the system.
			d_sysForces->getAcceleration<<<1,1>>>(d_nParticles,d_boxSize,d_currentTime,&cells,&d_particles);
			//Get the next system.
			d_integrator->nextSystem<<<1,1>>>(currentTime, dTime, nParticles, boxSize, &cells, &particles, d_sysForces);
			//Call cell manager.
			updateCells<<<1,1>>>();

			//Output a snapshot every second.
			if ( (counter % outputFreq) == 0 )
			{
				cudaMemcpy(particles,d_particles, nParticles*sizeof(particle) ,cudaMemcpyDeviceToHost);
				if (currentTime > 0)
				{
					utilities::util::clearLines(13);
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