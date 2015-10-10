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

#include "ui.h"
#include <dlfcn.h>

using namespace std;
using namespace utilities;

/**
 * @brief Run a new simulation.
 */
void runScript()
{
	/*----------------CFG-----------------*/

	util::writeTerminal("Looking for configuration file.\n\n", Colour::Green);
	configReader::config * cfg =new configReader::config("settings.cfg");
	cfg->showOutput();

	/*-------------INTEGRATOR-------------*/

	//Create the integrator.
	util::writeTerminal("Creating integrator.\n", Colour::Green);
	integrators::brownianIntegrator * difeq = new integrators::brownianIntegrator(cfg);

	/*---------------FORCES---------------*/

	//Create the force.
	util::writeTerminal("Creating forces.\n", Colour::Green);
	std::string forceName = cfg->getParam<std::string>("force","");
	std::string fileName = "./" + forceName + ".so";
	void* forceLib = dlopen(fileName.c_str(), RTLD_LAZY);
	
	//Throw error if the library does not exist.
	if (!forceLib)
	{
		util::writeTerminal("\n\nError loading in force library.\n\n", Colour::Red);
		return;
	}

	dlerror();

	//Make a factory to create the force instance.
	physics::create_CudaForce* buildFactory = (physics::create_CudaForce*) dlsym(forceLib,"getCudaForce");
	const char* err = dlerror();

	//If the force is not properly implemented.
	if (err)
	{
		util::writeTerminal("\n\nCould not find symbol: getForce\n\n", Colour::Red);
		return;
	}

	//Setup variables to copy.
	//This should be automated for a general number of inputs in future builds.
	float* frcLocal = new float[7];
	float* frcDevice;
	int frcSize = 7 * sizeof(float);
	cudaMalloc((void **)&frcDevice, frcSize);
	frcLocal[0] = cfg->getParam<float>("kT", 10.0);
	frcLocal[1] = cfg->getParam<float>("radius",0.5);
	frcLocal[2] = cfg->getParam<float>("mass",1.0);
	frcLocal[3] = cfg->getParam<float>("yukawaStrength",8.0);
	frcLocal[4] = cfg->getParam<int>("ljNum",18.0);
	frcLocal[5] = cfg->getParam<float>("cutOff",2.5);
	frcLocal[6] = cfg->getParam<float>("debyeLength",0.5);
	cudaMemcpy(frcDevice,frcLocal,frcSize,cudaMemcpyHostToDevice);

	std::cout << "\n";

	//Create a new force instance from the factory.
	physics::IForce** loadForce;
	cudaMalloc(&loadForce, sizeof(physics::IForce**));
	buildFactory(loadForce, frcDevice);

	//Run a test routine to ensure everything is loaded corrently.
	physics::cuda_Test* testFactory = (physics::cuda_Test*) dlsym(forceLib,"runCudaTest");

	err = dlerror();

	//If the force is not properly implemented.
	if (err)
	{
		util::writeTerminal("\n\nCould not find symbol: getForce\n\n", Colour::Red);
		return;
	}

	testFactory(loadForce);

	physics::cuda_Acceleration* accFactory = (physics::cuda_Acceleration*) dlsym(forceLib,"runAcceleration");

	err = dlerror();

	//If the force is not properly implemented.
	if (err)
	{
		util::writeTerminal("\n\nCould not find symbol: getForce\n\n", Colour::Red);
		return;
	}

	/*---------------SYSTEM---------------*/

	util::writeTerminal("\nCreating particle system.\n", Colour::Green);

	//Set the number of particles.
	int nParticles = cfg->getParam<int>("nParticles",0);

	if (nParticles == 0)
	{
		util::writeTerminal("\n\nSystem must start with more than zero particles.", Colour::Red);
		return;
	}
	//Creates the particle system.
	simulation::system * sys = new simulation::system(cfg, difeq, loadForce, accFactory, nParticles);

	/*---------------RUNNING--------------*/

	//Output the stats.
	cout << "---Number of Particles: " << sys->getNParticles() << "\n";
	cout << "---Box Size: " << sys->getBoxSize() << "\n";
	cout << "---Cell Size: " << sys->getCellSize() << "\n\n";
 
	//Write the initial system.
	cout << "Writing initial system to file.\n\n";
	sys->writeSystem("/initSys");

	/*-------------Iterator-------------*/

	//Allow user to check system settings before running.
	//Comment this section out if running without terminal access.
	util::writeTerminal("System initialization complete. Press y/n to continue: ", Colour::Blue);
	std::string cont;
	cin >> cont;

	if (cont != "Y" && cont != "y")
	{
		exit(100);
	}

	util::writeTerminal("Starting integration.\n", Colour::Green);

	int endTime = cfg->getParam<float>("endTime",1000);

	sys->run(endTime);

	//Write the final system.
	util::writeTerminal("\nIntegration complete.\n\n Writing final system to file.", Colour::Green);
	//sys->writeSystem("/finSys");
}