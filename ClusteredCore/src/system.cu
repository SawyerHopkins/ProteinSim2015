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
#include "globals.cuh"

namespace simulation
{
	system::system(configReader::config* cfg, integrators::I_integrator* sysInt, physics::IForce* sysFcs, int nParts) : particles(new particle[nParts])
	{
		/********************************************//**
		*------------------SETUP OUTPUT------------------
		************************************************/

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

		/********************************************//**
		*-------------------LOAD INPUT-------------------
		************************************************/

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
		numCells = pow(scale,3.0);

		//Sets the actual concentration.
		concentration = vP/pow(boxSize,3.0);
		particlesPerCell = 10;

		std::cout << "---System concentration: " << concentration << "\n";

		/********************************************//**
		*-----------------COPY VARIABLES-----------------
		************************************************/

		//Copy over system variables to device.
		cudaMalloc((void **)&d_nParticles, sizeof(int));
		cudaMemcpy(d_nParticles, &nParticles, sizeof(int), cudaMemcpyHostToDevice);

		cudaMalloc((void **)&d_boxSize, sizeof(int));
		cudaMemcpy(d_boxSize, &boxSize, sizeof(int), cudaMemcpyHostToDevice);

		cudaMalloc((void **)&d_dTime, sizeof(double));
		cudaMemcpy(d_dTime, &dTime, sizeof(double), cudaMemcpyHostToDevice);

		cudaMalloc((void **)&d_currentTime, sizeof(double));
		cudaMemcpy(d_currentTime, &currentTime, sizeof(double), cudaMemcpyHostToDevice);

		cudaMalloc((void **)&d_cellScale, sizeof(int));
		cudaMemcpy(d_cellScale, &cellScale, sizeof(int), cudaMemcpyHostToDevice);

		cudaMalloc((void **)&d_cellSize, sizeof(int));
		cudaMemcpy(d_cellSize, &cellSize, sizeof(int), cudaMemcpyHostToDevice);

		cudaMalloc((void **)&d_numCells, sizeof(int));
		cudaMemcpy(d_numCells, &numCells, sizeof(int), cudaMemcpyHostToDevice);

		cudaMalloc((void **)&d_particlesPerCell, sizeof(int));
		cudaMemcpy(d_particlesPerCell, &particlesPerCell, sizeof(int), cudaMemcpyHostToDevice);

		cudaMalloc((void *****)&cells, sizeof(cell****));
		/********************************************//**
		*----------------CREATE PARTICLES----------------
		************************************************/

		initParticles(r,m);

		/********************************************//**
		*------------------CREATE CELLS------------------
		************************************************/

		dim3 cellBlocks(scale,scale,scale);

		//Create cells.
		initCells<<<1,1>>>(numCells, cellScale, cells, particlesPerCell);
		cudaDeviceSynchronize();
		printf("Device Variable Copying:\t%s\n", cudaGetErrorString(cudaGetLastError()));

		resetCells<<<cellBlocks,1>>>(cells);
		cudaDeviceSynchronize();
		printf("Device Variable Copying:\t%s\n", cudaGetErrorString(cudaGetLastError()));

		updateCells<<<numCells,1>>>(d_cellScale, d_cellSize, cells, d_particles);
		cudaDeviceSynchronize();
		printf("Device Variable Copying:\t%s\n", cudaGetErrorString(cudaGetLastError()));

		std::cout << "Created: " << numCells << " cells from scale: " <<  cellScale << "\n";
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
			getAcceleration<<<nParticles,1>>>(d_nParticles,d_boxSize,d_currentTime,cells,d_particles,d_sysForces);
			printf("Device Variable Copying:\t%s\n", cudaGetErrorString(cudaGetLastError()));
			//cudaDeviceSynchronize();
			//Get the next system.
			//nextSystem<<<nParticles,1>>>(d_currentTime, d_dTime, d_nParticles, d_boxSize, cells, d_particles, d_sysForces, d_integrator);
			//cudaDeviceSynchronize();
			//Call cell manager.
			///resetCells<<<cellScale,1>>>(cells);
			//cudaDeviceSynchronize();
			//updateCells<<<numCells,1>>>(d_cellScale, d_cellSize, cells, d_particles);
			cudaDeviceSynchronize();
			printf("Device Variable Copying:\t%s\n", cudaGetErrorString(cudaGetLastError()));
			//Output a snapshot every second.
			if ( (counter % outputFreq) == 0 )
			{
				cudaMemcpy(particles,d_particles, nParticles*sizeof(particle) ,cudaMemcpyDeviceToHost);
				printf("Device Variable Copying:\t%s\n", cudaGetErrorString(cudaGetLastError()));
				int q = 0;
				std::cin >> q;
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