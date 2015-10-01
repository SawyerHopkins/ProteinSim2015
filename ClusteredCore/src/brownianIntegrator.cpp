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

#include "integrator.h"

namespace integrators
{
	brownianIntegrator::brownianIntegrator(configReader::config* cfg)
	{
		//Set the number of particles.
		memSize = cfg->getParam<int>("nParticles",1000);
		velFreq = cfg->getParam<int>("velFreq", 1000);
		velCounter = 0;

		//Create he memory blocks for mem and memCoor
		memX = new float[memSize];
		memY = new float[memSize];
		memZ = new float[memSize];
		memCorrX = new float[memSize];
		memCorrY = new float[memSize];
		memCorrZ = new float[memSize];

		//Sets the system temperature.
		temp = cfg->getParam<float>("temp",1.0);

		//Set the mass.
		mass = cfg->getParam<float>("mass",1.0);

		//Sets the system drag.
		gamma = cfg->getParam<float>("gamma",0.5);

		//Sets the integration time step.
		dt = cfg->getParam<float>("timeStep",0.001);
		dtInv = 1.0 / dt;

		//Create vital variables
		y = gamma*dt;

		setupHigh();
		if (gamma < 0.05)
		{
			setupLow();
		}
		if (gamma == 0)
		{
			setupZero();
		}

		float gamma2 = gamma*gamma;

		sig1   =  sqrt(+temp*sig1/gamma2);
		sig2   =  sqrt(-temp*sig2/gamma2);
		corr = (temp/(gamma2)) * (gn/(sig1*sig2));
		dev = sqrt(1.0 - (corr*corr));

		//Set the random number generator seed.
		int rSeed = 0;
		rSeed = cfg->getParam<int>("seed",90210);

		std::cout.precision(7);
		std::cout << "\n---y: " << y;
		std::cout << "\n---sig1: " << sig1;
		std::cout << "\n---sig2: " << sig2;
		std::cout << "\n---coor: " << corr;
		std::cout << "\n---dev: " << dev;
		std::cout << "\n---c0: " << coEff0;
		std::cout << "\n---c1: " << coEff1;
		std::cout << "\n---c2: " << coEff2;
		std::cout << "\n---c3: " << coEff3;
		std::cout << "\n---goy2: " << goy2;
		std::cout << "\n---goy3: " << goy3;
		std::cout << "\n---Brownian integrator successfully added.\n\n";
	}

	brownianIntegrator::~brownianIntegrator()
	{
		delete &mass;
		delete &temp;
		delete &memSize;

		delete &gamma;
		delete &dt;
		delete &y;

		delete &coEff0;
		delete &coEff1;
		delete &coEff2;
		delete &coEff3;

		delete[] memX;
		delete[] memY;
		delete[] memZ;

		delete[] memCorrX;
		delete[] memCorrY;
		delete[] memCorrZ;

		delete &sig1;
		delete &sig2;
		delete &corr;
		delete &dev;
	}
}