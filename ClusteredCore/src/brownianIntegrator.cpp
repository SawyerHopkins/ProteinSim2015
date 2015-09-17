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

		//Sets the name
		name = "brownianIntegrator";

		//Set the number of particles.
		memSize = cfg->getParam<int>("nParticles",1000);
		velFreq = cfg->getParam<int>("velFreq", 1000);
		velCounter = 0;

		//Create he memory blocks for mem and memCoor
		memX = new double[memSize];
		memY = new double[memSize];
		memZ = new double[memSize];
		memCorrX = new double[memSize];
		memCorrY = new double[memSize];
		memCorrZ = new double[memSize];

		//Sets the system temperature.
		temp = cfg->getParam<double>("temp",1.0);

		//Set the mass.
		mass = cfg->getParam<double>("mass",1.0);

		//Sets the system drag.
		gamma = cfg->getParam<double>("gamma",0.5);

		//Sets the integration time step.
		dt = cfg->getParam<double>("timeStep",0.001);
		dtInv = 1.0 / dt;

		//Create vital variables
		y = gamma*dt;

		setupHigh(cfg);
		if (gamma < 0.05)
		{
			setupLow(cfg);
		}
		if (gamma == 0)
		{
			setupZero(cfg);
		}

		double gamma2 = gamma*gamma;

		sig1   =  sqrt(+temp*sig1/gamma2);
		sig2   =  sqrt(-temp*sig2/gamma2);
		corr = (temp/(gamma2)) * (gn/(sig1*sig2));
		dev = sqrt(1.0 - (corr*corr));

		//Set the random number generator seed.
		int rSeed = 0;
		rSeed = cfg->getParam<int>("seed",90210);

		//Creates the random device.
		gen = new std::mt19937(rSeed);
		Dist = new std::normal_distribution<double>(0.0,1.0);

		//Create thread safe random devices.
		for (int i = 0; i < omp_get_max_threads(); i++)
		{
			double tSeed = (*Dist)(*gen);
			tgens[i] = new std::mt19937(tSeed);
		}

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

		delete gen;
		delete Dist;
	}

	void brownianIntegrator::setupHigh(configReader::config* cfg)
	{
		//Coefficents for High Gamma.
		//SEE GUNSTEREN AND BERENDSEN 1981
		double ty = 2.0*y;

		coEff0 = exp(-y);
		double aa1 = 1.0-coEff0;
		double aa2 = 0.5*y*(1.0+coEff0)-aa1;
		double aa3 = y-aa1;
		coEff1 = aa1/y;
		coEff2 = aa2/(y*y);
		coEff3 = aa3/(y*y);


		sig1 =  2.0*y-3.0+4.0*exp(-y)-exp(-ty);
		sig2 = -2.0*y-3.0+4.0*exp( y)-exp(ty);

		gn  = exp(y)-ty-exp(-y);

		goy2 = gn/(y*y);
		goy3 = gn/(y*y*y);

		hn  = y/(exp(y)-exp(-y));
	}

	void brownianIntegrator::setupLow(configReader::config* cfg)
	{
		//Coefficents for Low Gamma (from series expansion).
		//SEE GUNSTEREN AND BERENDSEN 1981
		double y1 = y;
		double y2 = y1*y1;
		double y3 = y2*y1;
		double y4 = y3*y1;
		double y5 = y4*y1;
		double y6 = y5*y1;
		double y7 = y6*y1;
		double y8 = y7*y1;
		double y9 = y8*y1;

		coEff1 = 1.0-0.5*y1+(1.0/6.0)*y2-(1.0/24.0)*y3
           +(1.0/120.0)*y4;
		coEff2 = (1.0/12.0)*y1-(1.0/24.0)*y2+(1.0/80.0)
           *y3-(1.0/360.0)*y4;
		coEff3 = 0.5-(1.0/6.0)*y1+(1.0/24.0)*y2-(1.0/120.0)*y3;

		sig1 = +(2.0/3.0)*y3-0.5*y4+(7.0/30.0)*y5-(1.0/12.0)
            *y6+(31.0/1260.0)*y7
            -(1.0/160.0)*y8+(127.0/90720.0)*y9;
		sig2 = -(2.0/3.0)*y3-0.5*y4-(7.0/30.)*y5-(1.0/12.0)
            *y6-(31.0/1260.0)*y7
            -(1.0/160.0)*y8-(127.0/90720.0)*y9;

		goy2 = (1.0/3.0)*y1+(1.0/60.0)*y3;
		goy3 = 1.0/3.0+(1.0/60.0)*y2;

		hn = 0.5-(1.00/12.0)*y2+(7.0/720.0)*y4;
		gn = (1.0/3.0)*y3+(1.0/60.0)*y5;
	}

	void brownianIntegrator::setupZero(configReader::config* cfg)
	{
		//Special case coefficents.
		//SEE GUNSTEREN AND BERENDSEN 1981
		coEff0 = 1.0;
		coEff1 = 1.0;
		coEff2 = 0.0;
		coEff3 = 0.5;
	}

	double brownianIntegrator::getWidth(double y)
	{
		return (2*y) - 3.0 + (4.0*exp(-y)) - exp(-2.0*y);
	}

}