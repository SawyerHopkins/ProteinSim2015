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

#include "AOPotential.h"
using namespace simulation;

AOPotential::~AOPotential()
{
	delete &kT;
	delete &cutOff;
	delete &coEff1;
	delete &coEff2;
}

AOPotential::AOPotential(configReader::config* cfg)
{
	//Sets the name
	name = "AOPotential";

	//Set vital variables.

	//Sets the system drag.
	kT = cfg->getParam<double>("kT",0.261);

	//Sets the integration time step.
	dt = cfg->getParam<double>("timeStep",0.001);

	//Get force range cutoff.
	cutOff = cfg->getParam<double>("cutOff",1.1);

	//Create secondary variables.
	a1=-kT*(cutOff/(cutOff-1.0))*(cutOff/(cutOff-1.0))*(cutOff/(cutOff-1.0));
	a2=-3.0/(2.0*cutOff);
	a3=1.0/(2.0*cutOff*cutOff*cutOff);

	coEff1 = -a1*a2;
	coEff2 = -3.0*a1*a3;

	utilities::util::writeTerminal("---AO Potential successfully added.\n\n", utilities::Colour::Cyan);

}

void AOPotential::iterCells(int boxSize, double time, particle* index, cell* itemCell)
{
	double pot = 0;

	for(std::map<int,particle*>::iterator it=itemCell->getBegin(); it != itemCell->getEnd(); ++it)
	{
		if (it->second->getName() != index->getName())
		{
			//Distance between the two particles.
			double rSquared = utilities::util::pbcDist(index->getX(), index->getY(), index->getZ(), 
																it->second->getX(), it->second->getY(), it->second->getZ(),
																boxSize);

			double rCutSquared = cutOff*cutOff;

			//If the particles are in the potential well.
			if (rSquared <= rCutSquared)
			{
				double r = sqrt(rSquared);

				//If the particles overlap there are problems.
				double size = (index->getRadius() + it->second->getRadius());
				if(r< (0.8*size) )
				{
					debugging::error::throwParticleOverlapError(index->getName(), it->second->getName(), r);
				}

				//Math
				double rInv=1.0/r; 
				double r_36=pow(rInv,36);
				double r_38=r_36/rSquared;
				double fNet=36.0*r_38+coEff1*rInv+coEff2*r; 

				//We need to switch the sign of the force.
				//Positive for attractive; negative for repulsive.
				fNet=-fNet;

				//Update net potential.
				pot += r_36+a1*(1.0+a2*r+a3*r*rSquared);

				//Normalize the force.
				double unitVec[3] {0.0,0.0,0.0};
				utilities::util::unitVectorAdv(index->getX(), index->getY(), index->getZ(), 
													it->second->getX(), it->second->getY(), it->second->getZ(),
													unitVec, r, boxSize);

				//Updates the acceleration.;
				double fx = fNet*unitVec[0];
				double fy = fNet*unitVec[1];
				double fz = fNet*unitVec[2];

				//If the force is infinite then there are worse problems.
				if (isnan(fNet))
				{
					//This error should only get thrown in the case of numerical instability.
					debugging::error::throwInfiniteForce();
				}

				//Add to the net force on the particle.
				index->updateForce(fx,fy,fz,pot,it->second);
			}
		}
	}
}

void AOPotential::getAcceleration(int index, int nPart, int boxSize, double time, cell* itemCell, particle** items)
{
	//Iter across all neighboring cells.
	for(auto it = itemCell->getFirstNeighbor(); it != itemCell->getLastNeighbor(); ++it)
	{
		iterCells(boxSize,time,items[index],*it);
	}
}