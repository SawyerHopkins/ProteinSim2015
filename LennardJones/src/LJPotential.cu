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

__device__
LennardJones::LennardJones(float * vars)
{
	kT = vars[0];

	//Get the radius
	radius = vars[1];

	//Get the mass
	mass = vars[2];

	//Get the well depth
	yukStr = vars[3];

	//Get the well depth
	ljNum = vars[4];

	//Get the cutoff range
	cutOff = vars[5];
	cutOffSquared = cutOff*cutOff;

	//Get the debye length for the system.
	debyeLength = vars[6];
	debyeInv = 1.0 / debyeLength;

	output = true;
}

LennardJones::LennardJones(configReader::config* cfg)
{
	//Sets the name
	//name = "Lennard Jones";

	kT = cfg->getParam<float>("kT", 10.0);

	//Get the radius
	radius = cfg->getParam<float>("radius",0.5);

	//Get the mass
	mass = cfg->getParam<float>("mass",1.0);

	//Get the well depth
	yukStr = cfg->getParam<float>("yukawaStrength",8.0);

	//Get the well depth
	ljNum = cfg->getParam<int>("ljNum",18.0);

	//Get the cutoff range
	cutOff = cfg->getParam<float>("cutOff",2.5);
	cutOffSquared = cutOff*cutOff;

	//Get the debye length for the system.
	debyeLength = cfg->getParam<float>("debyeLength",0.5);
	debyeInv = 1.0 / debyeLength;

	output = true;

	size = sizeof(*this);

	utilities::util::writeTerminal("---Lennard Jones Potential successfully added.\n\n", utilities::Colour::Cyan);
}

__device__
void LennardJones::cudaTest()
{
	printf("kT: %f\n",kT);
	printf("radius: %f\n",radius);
	printf("mass: %f\n",mass);
	printf("yukStr: %f\n",yukStr);
	printf("ljNum: %d\n",ljNum);
	printf("cutoff: %f\n",cutOff);
	printf("debyeLength: %f\n",debyeLength);
}

__device__
void LennardJones::iterCells(int* boxSize, particle* index, cell* itemCell)
{
	float pot = 0;
	int i = 0;
	int max = itemCell->gridCounter;

	while (i < max)
	{
		particle* it = itemCell->members[i];

		if (it->getName() != index->getName())
		{
			//Distance between the two particles. 
			float rSquared = utilities::util::pbcDist(index->getX(), index->getY(), index->getZ(), 
																it->getX(), it->getY(), it->getZ(),
																*boxSize);

			//If the particles are in the potential well.
			if (rSquared < cutOffSquared)
			{
				float r = sqrt(rSquared);

				//If the particles overlap there are problems.
				float size = (index->getRadius() + it->getRadius());
				if(r< (0.8*size) )
				{
					debugging::error::throwParticleOverlapError(index->getName(), it->getName(), r);
				}

				//-------------------------------------
				//-----------FORCE CALCULATION---------
				//-------------------------------------

				//Predefinitions.
				float RadiusOverR = (size / r);
				float rOverDebye = (r * debyeInv);
				float rInv = (1.0  / r);
				float DebyeShift = (debyeLength + r);
				float yukExp = std::exp(-rOverDebye);
				//float LJ = std::pow(RadiusOverR,ljNum);
				float LJ = utilities::util::powBinaryDecomp(RadiusOverR,ljNum);

				//Attractive LJ.
				float attract = ((2.0*LJ) - 1.0);
				attract *= (4.0*ljNum*rInv*LJ);

				//Repulsive Yukawa.
				float repel = yukExp;
				repel *= (rInv*rInv*DebyeShift*yukStr);

				float fNet = -kT*(attract+repel);

				if ((fNet*fNet) > (80*80))
				{
					printf("%f %d %d %d %d %f %f\n", fNet, it->getName(), index->getName(), i, max, rSquared, cutOffSquared);
				}

				//Positive is attractive; Negative repulsive.
				//fNet = -fNet;

				//-------------------------------------
				//---------POTENTIAL CALCULATION-------
				//-------------------------------------

				if (r < 1.1)
				{
					float ljPot = (LJ - 1.0);
					ljPot *= (4.0*LJ);

					float yukPot = yukExp;
					yukPot *= (debyeLength*yukStr*rInv);

					pot += (kT*yukPot);
					pot += (kT*ljPot);
				}

				//-------------------------------------
				//------NORMALIZATION AND SETTING------
				//-------------------------------------

				//Normalize the force.
				float unitVec[3] {0.0,0.0,0.0};
				utilities::util::unitVectorAdv(index->getX(), index->getY(), index->getZ(), 
													it->getX(), it->getY(), it->getZ(),
													unitVec, r, *boxSize);

				//Updates the acceleration.;
				float fx = fNet*unitVec[0];
				float fy = fNet*unitVec[1];
				float fz = fNet*unitVec[2];

				//If the force is infinite then there are worse problems.
				if (isnan(fNet))
				{
					//This error should only get thrown in the case of numerical instability.
					debugging::error::throwInfiniteForce();
				}

				//Add to the net force on the particle.
				if (r < 1.1)
				{
					index->updateForce(fx,fy,fz,pot,it);
				}
				else
				{
					index->updateForce(fx,fy,fz,pot,it,false);
				}
			}
		}
		i++;
	}
}

__device__
void LennardJones::getAcceleration(int* nPart, int* boxSize, int* cellScale ,float* time, simulation::cell* cells, simulation::particle* items)
{
	int index = (blockDim.x * blockIdx.x) + threadIdx.x;
	if (index >= *nPart) return;

	int cellIndex = items[index].getCX() + (items[index].getCY() * (*cellScale)) + (items[index].getCZ() * (*cellScale) * (*cellScale));

	int i = 0;
	while (i < 27)
	{
		iterCells(boxSize, &(items[index]), cells[cellIndex].getNeighbor(i));
		i++;
	}
}