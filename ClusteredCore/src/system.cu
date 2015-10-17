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
	system::system(configReader::config* cfg, integrators::brownianIntegrator* sysInt, physics::IForce** sysFrc, physics::cuda_Acceleration* acc, int nParts) 
	: particles(new particle[nParts])
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

		utilities::util::writeTerminal("\nLoading Config.\n", utilities::Colour::Green);

		//Set time information
		currentTime = 0;
		dTime = cfg->getParam<float>("timeStep",0.001);

		//Set the random number generator seed.
		seed = cfg->getParam<int>("seed",90210);

		//Sets the system temperature.
		temp = cfg->getParam<float>("temp",1.0);

		//Set the number of particles.
		nParticles = cfg->getParam<int>("nParticles",1000);

		//How often to output snapshots.
		outputFreq = cfg->getParam<int>("outputFreq",int(1.0/dTime));

		//Option to output XYZ format for clusters
		outXYZ = cfg->getParam<int>("XYZ",0);

		//Set the integration method.
		integrator = sysInt;

		//Set the internal forces.
		sysForces = sysFrc;
		forceFactory = acc;

		//Set the concentration.
		float conc = cfg->getParam<float>("conc",0.01);

		//Set the radius.
		float r = cfg->getParam<float>("radius",0.5);

		//Set the mass.
		float m = cfg->getParam<float>("mass",1.0);

		//Set the scale.
		int scale = 0;
		scale = cfg->getParam<int>("scale",4);

		//Create a box based on desired concentration.
		float particleVolume = (4.0/3.0)*atan(1.0)*4.0*r*r*r;
		float adjustedVolume = particleVolume*pow(0.8,3.0);
		float vP = nParticles*particleVolume;
		boxSize = (int) cbrt(vP / conc);

		//Calculates the number of cells needed.
		cellSize = boxSize / scale;
		boxSize = cellSize * scale;
		cellScale = scale;
		numCells = pow(scale,3.0);

		//Sets the actual concentration.
		concentration = vP/pow(boxSize,3.0);
		particlesPerCell = (cellSize*cellSize*cellSize/adjustedVolume);

		std::cout << "---System concentration: " << concentration << "\n";
		std::cout << "---particlesPerCell: " << particlesPerCell << "\n";

		/********************************************//**
		*-----------------COPY VARIABLES-----------------
		************************************************/

		utilities::util::writeTerminal("\n Copying variables to device.\n", utilities::Colour::Green);

		//Copy over system variables to device.
		cudaMalloc((void **)&d_nParticles, sizeof(int));
		cudaMemcpy(d_nParticles, &nParticles, sizeof(int), cudaMemcpyHostToDevice);

		cudaMalloc((void **)&d_boxSize, sizeof(int));
		cudaMemcpy(d_boxSize, &boxSize, sizeof(int), cudaMemcpyHostToDevice);

		cudaMalloc((void **)&d_dTime, sizeof(float));
		cudaMemcpy(d_dTime, &dTime, sizeof(float), cudaMemcpyHostToDevice);

		cudaMalloc((void **)&d_currentTime, sizeof(float));
		cudaMemcpy(d_currentTime, &currentTime, sizeof(float), cudaMemcpyHostToDevice);

		cudaMalloc((void **)&d_cellScale, sizeof(int));
		cudaMemcpy(d_cellScale, &cellScale, sizeof(int), cudaMemcpyHostToDevice);

		cudaMalloc((void **)&d_cellSize, sizeof(int));
		cudaMemcpy(d_cellSize, &cellSize, sizeof(int), cudaMemcpyHostToDevice);

		cudaMalloc((void **)&d_numCells, sizeof(int));
		cudaMemcpy(d_numCells, &numCells, sizeof(int), cudaMemcpyHostToDevice);

		cudaMalloc((void **)&d_particlesPerCell, sizeof(int));
		cudaMemcpy(d_particlesPerCell, &particlesPerCell, sizeof(int), cudaMemcpyHostToDevice);

		cudaMalloc((void **)&integrator, sizeof(*sysInt));
		cudaMemcpy(integrator, &sysInt, sizeof(*sysInt), cudaMemcpyHostToDevice);

		/********************************************//**
		*----------------CREATE PARTICLES----------------
		************************************************/

		utilities::util::writeTerminal("\n Creating particles.\n", utilities::Colour::Green);

		initParticles(r,m);
		int size = nParticles * sizeof(particle);
		cudaMalloc((void **)&d_particles, size);
		cudaMemcpy(d_particles, particles, size, cudaMemcpyHostToDevice);
		checkCuda("copyParticles");

		/********************************************//**
		*----------------CREATE INTEGRATOR----------------
		************************************************/

		utilities::util::writeTerminal("\n Loading integrator.\n", utilities::Colour::Green);
		//Setup variables to copy.
		//This should be automated for a general number of inputs in future builds.
		float* intLocal = new float[7];
		float* intDevice;
		int intSize = 7 * sizeof(float);
		cudaMalloc((void **)&intDevice, intSize);
		intLocal[0] = cfg->getParam<int>("nParticles",1000);
		intLocal[1] = cfg->getParam<int>("velFreq", 1000);
		intLocal[2] = cfg->getParam<float>("temp",1.0);
		intLocal[3] = cfg->getParam<float>("mass",1.0);
		intLocal[4] = cfg->getParam<float>("gamma",0.5);
		intLocal[5] = cfg->getParam<float>("timeStep",0.001);
		intLocal[6] = cfg->getParam<int>("seed",90210);
		cudaMemcpy(intDevice,intLocal,intSize,cudaMemcpyHostToDevice);

		loadIntegrator<<<1,1>>>(integrator, intDevice);
		cudaDeviceSynchronize();
		checkCuda("loadIntegrator");

		testIntegrator<<<1,1>>>(integrator);
		cudaDeviceSynchronize();
		checkCuda("testIntegrator");

		/********************************************//**
		*-----------------CREATE FORCES------------------
		************************************************/

		/********************************************//**
		*------------------CREATE CELLS------------------
		************************************************/

		utilities::util::writeTerminal("\n Creating cell system.\n", utilities::Colour::Green);

		//Create the basis cells.
		void* raw = operator new[]( numCells * sizeof( cell ) );
		cell* basis = static_cast<cell*>( raw );

		for (int i = 0; i < numCells; i++) {
			new(&basis[i]) cell(particlesPerCell);
		}

		dim3 cellBlocks(scale,scale,scale);

		cudaMalloc((void **)&cells, sizeof(basis));
		cudaMemcpy(cells, basis, sizeof(basis), cudaMemcpyHostToDevice);

		//Create cells.
		initCells<<<cellBlocks,1>>>(numCells, cellScale, cells, particlesPerCell);
		cudaDeviceSynchronize();
		checkCuda("initCells");

		//Create cells.
		createNeighborhoods<<<numCells,1>>>(cells);
		cudaDeviceSynchronize();
		checkCuda("createNeighborhoods");

		resetCells<<<numCells,1>>>(cells);
		cudaDeviceSynchronize();
		checkCuda("resetCells");

		updateCells<<<nParticles,1>>>(d_cellScale, d_cellSize, cells, d_particles);
		cudaDeviceSynchronize();
		checkCuda("updateCells");

		std::cout << "Created: " << numCells << " cells from scale: " <<  cellScale << "\n";
		writeSystemInit();

	}

	void system::checkCuda(std::string name)
	{
		std::string err = cudaGetErrorString(cudaGetLastError());
		if (err != "no error")
		{
			utilities::util::writeTerminal("CUDA KERNEL: " + name + " - " + err + "\n", utilities::Colour::Red);
			int i = 0;
			std::cin >> i;
		}
		else
		{
			//utilities::util::writeTerminal("CUDA KERNEL: " + name + " - " + err + "\n", utilities::Colour::Green);
		}
	}

	void system::run(float endTime)
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

		currentTime = 0;

		//Run system until end time.
		while (currentTime < endTime)
		{
			cudaDeviceSynchronize();
			//Get the forces acting on the system.
			forceFactory(sysForces, d_nParticles, d_boxSize, d_cellScale, d_currentTime, cells, d_particles, nParticles);
			cudaDeviceSynchronize();
			checkCuda("forceFactory");
			//Get the next system.
			nextSystem<<<nParticles,1>>>(d_currentTime, d_dTime, d_nParticles, d_boxSize, d_particles, integrator);
			cudaDeviceSynchronize();
			checkCuda("nextSystem");
			//Call cell manager.
			resetCells<<<numCells,1>>>(cells);
			cudaDeviceSynchronize();
			checkCuda("resetCells");
			updateCells<<<nParticles,1>>>(d_cellScale, d_cellSize, cells, d_particles);
			cudaDeviceSynchronize();
			checkCuda("updateCells");
			//Output a snapshot every second.
			if ( (counter % outputFreq) == 0 )
			{
				if (currentTime > 0)
				{
					cudaMemcpy(particles,d_particles, nParticles*sizeof(particle) ,cudaMemcpyDeviceToHost);
					utilities::util::clearLines(10);
					checkCuda("copyFromDevice");
				}
				writeSystemState(tmr);
			}

			//Update loading bar.
			utilities::util::loadBar(currentTime,endTime,counter);

			//Increment counters.
			currentTime += dTime;
			incrementTime<<<1,1>>>(d_dTime,d_currentTime);
			cudaDeviceSynchronize();
			checkCuda("incrementTime");
			counter++;
		}
	}
}