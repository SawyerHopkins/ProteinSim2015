#include <math.h>
#include <string.h>
#include <iostream>
#include <chrono>
#include <thread>

#include "point.h"
#include "integrator.h"
#include "force.h"
#include "utilities.h"

using namespace std;

/*-----------------------------------------*/
/*----------FUNCTION DECLARATIONS----------*/
/*-----------------------------------------*/
/*----See function for full description----*/
/*-----------------------------------------*/

static inline void debug(mathTools::points* pt);

/*-----------------------------------------*/
/*--------------PROGRAM MAIN---------------*/
/*-----------------------------------------*/

int main(int argc, char **argv)
{
	/*-------------Variables-------------*/

	//Initialize random number generator.
	srand (time(NULL));
	//Set the maximum time.
	float endTime = 10000;
	//Set the time step for the integrator.
	float timeStep = .001;
	//Set the number of particles.
	float nParticles = 100;
	//Set drag coefficent.
	float gamma = 1.0;

	/*-------------Setup-------------*/

	//Create the integrator.
	integrators::verlet * difeq = new integrators::verlet(timeStep);

	//Creates the particle system.
	mathTools::points * pt = new mathTools::points(nParticles, 0.5);
	//Initialize the particle system with random position and velocity.
	pt->init(0.02);

	//Creates a force manager.
	physics::forces * force = new physics::forces();
	force->addForce(new physics::aggForce(.46,1.1)); //Adds the aggregation force.
	//force->addForce(new physics::dragForce(gamma)); //Adds drag.
	force->addForce(new physics::brownianForce(gamma,1.0,1.0,timeStep,nParticles)); //Adds brownian dynamics.

	cout << "Number of Particles: " << pt->arrSize << "\n";
	cout << "Box Size: " << pt->getBoxSize() << "\n";

	pt->writeSystem("initSys");

	/*-------------Iterator-------------*/
	while(difeq->getSystemTime() < endTime)
	{
		for (int i =0; i < pt->arrSize; i++)
		{
			difeq->nextSystem(i,pt,force);
			difeq->advanceTime();
		}
		//debug(pt);
		//std::this_thread::sleep_for(std::chrono::milliseconds(3000));
		utilities::loadBar(difeq->getSystemTime(),endTime);
	}

	pt->writeSystem("finSys");

	//Debug code 0 -> No Error:
	return 0;
}

/*-----------------------------------------*/
/*--------------AUX FUNCTIONS--------------*/
/*-----------------------------------------*/

//Writes the state of the system to the console.
void debug(mathTools::points* pt)
{
	/*-------------Debugging-------------*/
	/*-Out the position of each particle-*/
	for (int i = 0; i < pt->arrSize; i++)
	{
		pt->writePosition(i);
	}
	cout << "\n";
	//cout << "\n" << dist(pt->getX(0),pt->getX(1),pt->getY(0),pt->getY(1),pt->getZ(0),pt->getZ(1)) << "\n\n";
}