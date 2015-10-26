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

#include "LJPotential.h"
using namespace simulation;

LennardJones::~LennardJones()
{
	delete[] &cutOff;
	delete[] &debyeLength;
	delete[] &kT;
	delete[] &radius;
	delete[] &yukStr;
	delete[] &mass;
	delete[] &ljNum;
}

LennardJones::LennardJones(configReader::config* cfg)
{
	//Sets the name
	name = "Lennard Jones";

	kT = cfg->getParam<double>("kT", 10.0);

	//Get the radius
	radius = cfg->getParam<double>("radius",0.5);

	//Get the mass
	mass = cfg->getParam<double>("mass",1.0);

	//Get the well depth
	yukStr = cfg->getParam<double>("yukawaStrength",8.0);

	//Get the well depth
	ljNum = cfg->getParam<int>("ljNum",18.0);

	//Get the cutoff range
	cutOff = cfg->getParam<double>("cutOff",2.5);
	cutOffSquared = cutOff*cutOff;

	//Get the debye length for the system.
	debyeLength = cfg->getParam<double>("debyeLength",0.5);
	debyeInv = 1.0 / debyeLength;

	output = true;

	utilities::util::writeTerminal("---Lennard Jones Potential successfully added.\n\n", utilities::Colour::Cyan);
}

void LennardJones::iterCells(int boxSize, double time, particle* index, cell* itemCell)
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

			//If the particles are in the potential well.
			if (rSquared < cutOffSquared)
			{
				double r = sqrt(rSquared);

				//If the particles overlap there are problems.
				double size = (index->getRadius() + it->second->getRadius());
				if(r< (0.8*size) )
				{
					debugging::error::throwParticleOverlapError(index->getName(), it->second->getName(), r);
				}

				//-------------------------------------
				//-----------FORCE CALCULATION---------
				//-------------------------------------

				//Predefinitions.
				double RadiusOverR = (size / r);
				double rOverDebye = (r * debyeInv);
				double rInv = (1.0  / r);
				double DebyeShift = (debyeLength + r);
				double yukExp = std::exp(-rOverDebye);
				//double LJ = std::pow(RadiusOverR,ljNum);
				double LJ = utilities::util::powBinaryDecomp(RadiusOverR,ljNum);

				//Attractive LJ.
				double attract = ((2.0*LJ) - 1.0);
				attract *= (4.0*ljNum*rInv*LJ);

				//Repulsive Yukawa.
				double repel = yukExp;
				repel *= (rInv*rInv*DebyeShift*yukStr);

				double fNet = -kT*(attract+repel);

				//Positive is attractive; Negative repulsive.
				//fNet = -fNet;

				//-------------------------------------
				//---------POTENTIAL CALCULATION-------
				//-------------------------------------

				if (r < 1.1)
				{
					double ljPot = (LJ - 1.0);
					ljPot *= (4.0*LJ);

					double yukPot = yukExp;
					yukPot *= (debyeLength*yukStr*rInv);

					pot += (kT*yukPot);
					pot += (kT*ljPot);
				}

				//-------------------------------------
				//------NORMALIZATION AND SETTING------
				//-------------------------------------

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
				if (r < 1.1)
				{
					index->updateForce(fx,fy,fz,pot,it->second);
				}
				else
				{
					index->updateForce(fx,fy,fz,pot,it->second,false);
				}
			}
		}
	}
}

void LennardJones::quench()
{
	kT=3.0;
	cutOff = 1.5;
	cutOffSquared = cutOff*cutOff;
	debyeLength = 1.0/6.0;
	debyeInv = 1.0 / debyeLength;
	quenched=true;
}

void LennardJones::getAcceleration(int index, int nPart, int boxSize, double time, simulation::cell* itemCell, simulation::particle** items)
{
	for(auto it = itemCell->getFirstNeighbor(); it != itemCell->getLastNeighbor(); ++it)
	{
		iterCells(boxSize,time,items[index],*it);
	}
}

