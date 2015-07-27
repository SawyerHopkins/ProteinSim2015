/*The MIT License (MIT)

Copyright (c) <2015> <Sawyer Hopkins>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.*/

#include "system.h"
#include <dlfcn.h>

using namespace std;

/**
 * @brief Run a new simulation.
 */
void runScript()
{
	/*----------------CFG-----------------*/

	cout << "Looking for configuration file.\n\n";
	configReader::config * cfg =new configReader::config("settings.cfg");
	cfg->showOutput();

	/*-------------INTEGRATOR-------------*/

	//Create the integrator.
	cout << "Creating integrator.\n";
	integrators::brownianIntegrator * difeq = new integrators::brownianIntegrator(cfg);

	/*---------------FORCES---------------*/

	//Creates a force manager.
	cout << "Adding required forces.\n";

	void* forceLib = dlopen("./AOPot.so", RTLD_LAZY);

	if (!forceLib)
	{
		cout << "\n\n" << "Error loading in force library.\n\n";
		return;
	}

	dlerror();

	physics::create_Force* factory = (physics::create_Force*) dlsym(forceLib,"getForce");
	const char* err = dlerror();

	if (err)
	{
		cout << "\n\n" << "Could not find symbol: getForce\n\n";
		return;
	}

	physics::IForce* loadForce = factory(cfg);

	physics::forces * force = new physics::forces();
	//force->addForce(new physics::AOPotential(cfg)); //Adds the aggregation force.
	//force->addForce(new physics::Yukawa(cfg));
	force->addForce(loadForce);

	int num_threads = cfg->getParam<double>("threads",1);
	force->setNumThreads(num_threads);

	int num_dyn = cfg->getParam<double>("omp_dynamic",0);
	force->setDynamic(num_dyn);


	int num_dev = cfg->getParam<double>("omp_device",0);
	force->setDevice(num_dev);

	/*---------------SYSTEM---------------*/

	cout << "Creating particle system.\n";
	//Creates the particle system.
	simulation::system * sys = new simulation::system(cfg, difeq, force);

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
	cout << "System initialization complete. Press y/n to continue: ";
	std::string cont;
	cin >> cont;

	if (cont != "Y" && cont != "y")
	{
		exit(100);
	}

	cout << "Starting integration.\n";

	int endTime = cfg->getParam<double>("endTime",1000);

	sys->run(endTime);

	//Write the final system.
	cout << "\n" << "Integration complete.\n\n Writing final system to file.";
	sys->writeSystem("/finSys");
}