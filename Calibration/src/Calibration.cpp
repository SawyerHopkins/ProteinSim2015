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

#include "Calibration.h"

Calibration::~Calibration()
{
	delete &cutOff;
}

Calibration::Calibration(config* cfg)
{
	//Sets the name
	name = "Calibration";

	//Get force range cutoff.
	cutOff = cfg->getParam<double>("cutOff",1.1);

	PSim::util::writeTerminal("---Calibration Force successfully added.\n\n", PSim::Colour::Cyan);
}

void Calibration::iterCells(int boxSize, double time, particle* index, PeriodicGrid* itemCell)
{
	for(std::map<int,particle*>::iterator it=itemCell->getBegin(); it != itemCell->getEnd(); ++it)
	{
		if (it->second->getName() != index->getName())
		{
			//Distance between the two particles.
			double rSquared = PSim::util::pbcDist(index->getX(), index->getY(), index->getZ(), 
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
					PSim::error::throwParticleOverlapError(index->getName(), it->second->getName(), r);
				}

				//Math
				double rInv=1.0/r; 
				double r_37=36.0*pow(rInv,37);
				double fNet=r_37;

				//We need to switch the sign of the force.
				//Positive for attractive; negative for repulsive.
				fNet=-fNet;


				//Normalize the force.
				double unitVec[3] {0.0,0.0,0.0};
				PSim::util::unitVectorAdv(index->getX(), index->getY(), index->getZ(), 
													it->second->getX(), it->second->getY(), it->second->getZ(),
													unitVec, r, boxSize);


				type3<double>* frc = new type3<double>(fNet*unitVec[0],fNet*unitVec[1],fNet*unitVec[2]);

				//If the force is infinite then there are worse problems.
				if (std::isnan(fNet))
				{
					//This error should only get thrown in the case of numerical instability.
					PSim::error::throwInfiniteForce();
				}

				//Add to the net force on the particle.
				index->updateForce(frc,it->second);
			}
		}
	}
}

void Calibration::getAcceleration(int index, PSim::PeriodicGrid* itemCell, PSim::particle** items, systemState* state)
{
	//Iter across all neighboring cells.
	for(auto it = itemCell->getFirstNeighbor(); it != itemCell->getLastNeighbor(); ++it)
	{
		iterCells(state->boxSize, state->currentTime ,items[index],*it);
	}
}
