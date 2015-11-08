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

PSim::defaultForceManager* loadForces(configReader::config* cfg)
{
	//Creates a force manager.
	util::writeTerminal("Adding required forces.\n", Colour::Green);

	std::string forceName = cfg->getParam<std::string>("force","");
	std::string fileName = "./" + forceName + ".so";

	//Opens the force library.
	void* forceLib = dlopen(fileName.c_str(), RTLD_LAZY);

	//Throw error if the library does not exist.
	if (!forceLib)
	{
		util::writeTerminal("\n\nError loading in force library.\n\n", Colour::Red);
		exit(100);
	}

	dlerror();

	//Make a factory to create the force instance.
	PSim::create_Force* factory = (PSim::create_Force*) dlsym(forceLib,"getForce");
	const char* err = dlerror();

	//If the force is not properly implemented.
	if (err)
	{
		util::writeTerminal("\n\nCould not find symbol: getForce\n\n", Colour::Red);
		exit(100);
	}

	//Create a new force instance from the factory.
	PSim::IForce* loadForce = factory(cfg);

	//Add the force to the force manager.
	PSim::defaultForceManager* force = new PSim::defaultForceManager();
	force->addForce(loadForce);

	util::writeTerminal("Creating force manager.\n", Colour::Green);
	int num_threads = cfg->getParam<double>("threads",1);
	force->setNumThreads(num_threads);

	int num_dyn = cfg->getParam<double>("omp_dynamic",0);
	force->setDynamic(num_dyn);

	//Does not work on GCC 4.8 and below.
	int num_dev = cfg->getParam<double>("omp_device",0);
	force->setDevice(num_dev);

	return force;
}

PSim::system* loadSystem(configReader::config* cfg, std::string aName, std::string timeStamp, PSim::brownianIntegrator* difeq, PSim::defaultForceManager* force)
{
	util::writeTerminal("\nLoading particle system.\n", Colour::Green);
	configReader::config* sysCfg =new configReader::config(aName + "/sysConfig.cfg");
	PSim::system* sys = PSim::system::loadFromFile(sysCfg, aName, timeStamp, difeq, force);

	double newTimeStep = cfg->getParam<double>("timeStep",0);
	double newTemp = cfg->getParam<double>("temp",0);

	if (newTimeStep > 0) { 
		util::writeTerminal("\nReloading timestep: " + std::to_string(sys->setdTime(newTimeStep)) + " -> " + std::to_string(newTimeStep) + "\n", Colour::Magenta);
	}
	if (newTemp > 0) {
		util::writeTerminal("\nReloading system temperature: " + std::to_string(sys->setdTime(newTemp)) + " -> " + std::to_string(newTemp) + "\n", Colour::Magenta);
	}
	return sys;
}

PSim::system* buildSystem(configReader::config* cfg, PSim::brownianIntegrator* difeq, PSim::defaultForceManager* force)
{
	util::writeTerminal("\nCreating particle system.\n", Colour::Green);
	//Creates the particle system.
	PSim::system* sys = new PSim::system(cfg, difeq, force);

	//Output the stats.
	cout << "---Number of Particles: " << sys->getNParticles() << "\n";
	cout << "---Box Size: " << sys->getBoxSize() << "\n";
	cout << "---Cell Size: " << sys->getCellSize() << "\n\n";
 
	//Write the initial system.
	cout << "Writing initial system to file.\n\n";
	sys->writeSystem("/initSys");
	return sys;
}

/**
 * @brief Run a new simulation.
 */
void runScript(string aName, string timeStamp)
{
	std::string settingPath = "settings.cfg";

	/*----------------CFG-----------------*/

	util::writeTerminal("Looking for configuration file.\n\n", Colour::Green);
	configReader::config* cfg =new configReader::config(settingPath);
	cfg->showOutput();

	/*---------------FORCES---------------*/

	PSim::defaultForceManager* force = loadForces(cfg);

	/*-------------INTEGRATOR-------------*/

	//Create the integrator.
	util::writeTerminal("Creating integrator.\n", Colour::Green);
	PSim::brownianIntegrator * difeq = new PSim::brownianIntegrator(cfg);

	/*---------------SYSTEM---------------*/

	PSim::system* sys = NULL;

	if (aName != "") {
		sys = loadSystem(cfg, aName, timeStamp, difeq, force);
	} else {
		sys = buildSystem(cfg, difeq, force);
	}

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

	int endTime = cfg->getParam<double>("endTime",1000);

	sys->run(endTime);

	//Write the final system.
	util::writeTerminal("\nIntegration complete.\n\n Writing final system to file.", Colour::Green);
	sys->writeSystem("/finSys");
}