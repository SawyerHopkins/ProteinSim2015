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

AOPotential::~AOPotential()
{
	delete &kT;
	delete &cutOff;
	delete &coEff1;
	delete &coEff2;
}

AOPotential::AOPotential(config* cfg)
{
	//Sets the name
	name = "AOPotential";

	//Set vital variables.

	wellDepth = cfg->getParam<double>("wellDepth", 0.261);
	kT = cfg->getParam<double>("kT",1.0);

	//Sets the integration time step.
	dt = cfg->getParam<double>("timeStep",0.001);

	//Get force range cutoff.
	cutOff = cfg->getParam<double>("cutOff",1.1);

	//Create secondary variables.
	a1=-kT*wellDepth*(cutOff/(cutOff-1.0))*(cutOff/(cutOff-1.0))*(cutOff/(cutOff-1.0));
	a2=-3.0/(2.0*cutOff);
	a3=1.0/(2.0*cutOff*cutOff*cutOff);

	coEff1 = -a1*a2;
	coEff2 = -3.0*a1*a3;

	PSim::util::writeTerminal("---AO Potential successfully added.\n\n", PSim::Colour::Cyan);

}

type3<double> AOPotential::iterCells(int index, int hash, double* sortedParticles, vector<tuple<int,int>>* cellStartEnd, systemState* state)
{
	int start = get<0>((*cellStartEnd)[hash]);

	type3<double> cellForce = type3<double>();

	if (start != 0xffffffff) {
		int end = get<1>((*cellStartEnd)[hash]);
		for (int i=start; i<end; i++) {
			if (i != index) {
				int indexOffset = 4*index;
				int iOffset = 4*i;

				double rSquared = PSim::util::pbcDist(sortedParticles[indexOffset], sortedParticles[indexOffset+1], sortedParticles[indexOffset+2],
																	sortedParticles[iOffset], sortedParticles[iOffset+1], sortedParticles[iOffset+2],
																	state->boxSize);

				//If the particles are in the potential well.
				double rCutSquared = cutOff*cutOff;
				if (rSquared < rCutSquared)
				{
					double r = sqrt(rSquared);
					//If the particles overlap there are problems.
					double size = (sortedParticles[indexOffset+3] + sortedParticles[iOffset+3]);
					if(r< (0.8*size) )
					{
						PSim::error::throwParticleOverlapError(hash, i, index, r);
					}

					//-------------------------------------
					//-----------FORCE CALCULATION---------
					//-------------------------------------

					//Math
					double rInv=1.0/r;
					double r_36=pow(rInv,36);
					double r_38=r_36/rSquared;
					double fNet=36.0*r_38+coEff1*rInv+coEff2*r;

					//We need to switch the sign of the force.
					//Positive for attractive; negative for repulsive.
					fNet=-fNet;

					//If the force is infinite then there are worse problems.
					if (std::isnan(fNet))
					{
						//This error should only get thrown in the case of numerical instability.
						PSim::error::throwInfiniteForce();
					}

					//-------------------------------------
					//------NORMALIZATION AND SETTING------
					//-------------------------------------

					//Normalize the force.
					double unitVec[3] {0.0,0.0,0.0};
					PSim::util::unitVectorAdv(sortedParticles[indexOffset], sortedParticles[indexOffset+1], sortedParticles[indexOffset+2],
														sortedParticles[iOffset], sortedParticles[iOffset+1], sortedParticles[iOffset+2],
														unitVec, r, state->boxSize);

					//Updates the acceleration.;
					cellForce.x += fNet*unitVec[0];
					cellForce.y += fNet*unitVec[1];
					cellForce.z += fNet*unitVec[2];
				}
			}
		}
	}
	return cellForce;
}

void AOPotential::getAcceleration(int index, double* sortedParticles, double* particleForce, vector<tuple<int,int>>* particleHashIndex, vector<tuple<int,int>>* cellStartEnd, systemState* state)
{
	int hash = 0;
	int indexOffset = index*4;
	type3<int> cell = type3<int>();
	type3<int> cRef = type3<int>();
	double netForce[3] = {0.0,0.0,0.0};
	int scale = state->cellScale;
	int cellScaleSq = scale*scale;
	int realIndex = 3*get<1>((*particleHashIndex)[index]);

	cell.x = floor(sortedParticles[indexOffset] / state->cellSize);
	cell.y = floor(sortedParticles[indexOffset+1] / state->cellSize);
	cell.z = floor(sortedParticles[indexOffset+2] / state->cellSize);

	for (int x=-1; x<=1; x++) {
		for (int y=-1; y<=1; y++) {
			for (int z=-1; z<=1; z++) {
				cRef.x = cell.x + x;
				cRef.y = cell.y + y;
				cRef.z = cell.z + z;

				cRef.x = cRef.x % scale;
				cRef.y = cRef.y % scale;
				cRef.z = cRef.z % scale;

				cRef.x = (cRef.x < 0) ? cRef.x + scale : cRef.x;
				cRef.y = (cRef.y < 0) ? cRef.y + scale : cRef.y;
				cRef.z = (cRef.z < 0) ? cRef.z + scale : cRef.z;

				hash = cRef.x + (scale * cRef.y) + (cellScaleSq * cRef.z);

				type3<double> result = iterCells(index, hash, sortedParticles, cellStartEnd, state);
				netForce[0] += result.x;
				netForce[1] += result.y;
				netForce[2] += result.z;
			}
		}
	}

	particleForce[realIndex] = netForce[0];
	particleForce[realIndex+1] = netForce[1];
	particleForce[realIndex+2] = netForce[2];
}
