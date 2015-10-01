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
#include <stdio.h> 

namespace integrators
{
	__device__
	void brownianIntegrator::cudaLoad(float * vars)
	{
		//Set the number of particles.
		memSize = vars[0];
		velFreq = vars[1];
		velCounter = 0;

		//Create he memory blocks for mem and memCoor
		memX = new float[memSize];
		memY = new float[memSize];
		memZ = new float[memSize];
		memCorrX = new float[memSize];
		memCorrY = new float[memSize];
		memCorrZ = new float[memSize];

		//Sets the system temperature.
		temp = vars[2];

		//Set the mass.
		mass = vars[3];

		//Sets the system drag.
		gamma = vars[4];

		//Sets the integration time step.
		dt = vars[5];
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
		rSeed = vars[6];
	}

	__host__ __device__
	void brownianIntegrator::setupHigh()
	{
		//Coefficents for High Gamma.
		//SEE GUNSTEREN AND BERENDSEN 1981
		float ty = 2.0*y;

		coEff0 = exp(-y);
		float aa1 = 1.0-coEff0;
		float aa2 = 0.5*y*(1.0+coEff0)-aa1;
		float aa3 = y-aa1;
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

	__host__ __device__
	void brownianIntegrator::setupLow()
	{
		//Coefficents for Low Gamma (from series expansion).
		//SEE GUNSTEREN AND BERENDSEN 1981
		float y1 = y;
		float y2 = y1*y1;
		float y3 = y2*y1;
		float y4 = y3*y1;
		float y5 = y4*y1;
		float y6 = y5*y1;
		float y7 = y6*y1;
		float y8 = y7*y1;
		float y9 = y8*y1;

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

	__host__ __device__
	void brownianIntegrator::setupZero()
	{
		//Special case coefficents.
		//SEE GUNSTEREN AND BERENDSEN 1981
		coEff0 = 1.0;
		coEff1 = 1.0;
		coEff2 = 0.0;
		coEff3 = 0.5;
	}

	__host__ __device__
	float brownianIntegrator::getWidth(float y)
	{
		return (2*y) - 3.0 + (4.0*exp(-y)) - exp(-2.0*y);
	}

	__device__
	void brownianIntegrator::cudaTest()
	{
		printf("%f\n",gamma);
	}

	__device__
	void brownianIntegrator::nextSystem(float *time, float *dt, int *nParticles, int *boxSize, simulation::particle* items)
	{
		//Checks what method is needed.
		if (time == 0)
		{
			firstStep(*time, *dt, *nParticles, *boxSize, items);
		}
		else
		{
			normalStep(*time, *dt, *nParticles, *boxSize, items);
		}
	}

	__device__
	void brownianIntegrator::firstStep(float time, float dt, int nParticles, int boxSize, simulation::particle* items)
	{
		int i= (blockDim.x * blockIdx.x) + threadIdx.x;
		//SEE GUNSTEREN AND BERENDSEN 1981 EQ 2.26

		//memCorrX[i] = 0.0;
		//memCorrY[i] = 0.0;
		//memCorrZ[i] = 0.0;

		//memX[i] = (*Dist)(*gen);
		//memY[i] = (*Dist)(*gen);
		//memZ[i] = (*Dist)(*gen);

		float m = 1.0/items[i].getMass();
		float xNew = items[i].getX() + (items[i].getVX() * coEff1 * dt) + (items[i].getFX() * coEff3 * dt * dt * m); //+ (sig1 * memX[i]);
		float yNew = items[i].getY() + (items[i].getVY() * coEff1 * dt) + (items[i].getFY() * coEff3 * dt * dt * m); //+ (sig1 * memY[i]);
		float zNew = items[i].getZ() + (items[i].getVZ() * coEff1 * dt) + (items[i].getFZ() * coEff3 * dt * dt * m); //+ (sig1 * memZ[i]);
		items[i].setPos(xNew,yNew,zNew,boxSize);
	}

	__device__
	void brownianIntegrator::normalStep(float time, float dt, int nParticles, int boxSize, simulation::particle* items)
	{

		int i = blockIdx.x;

		float dt2 = dt * dt;
		//SEE GUNSTEREN AND BERENDSEN 1981 EQ 2.26
		//New random walk.
		//memCorrX[i] = (*Dist)(*tgens[0]);
		//memCorrY[i] = (*Dist)(*tgens[0]);
		//memCorrZ[i] = (*Dist)(*tgens[0]);

		//Correlation to last random walk.
		//memCorrX[i] = sig2 * ((corr * memX[i])+(dev * memCorrX[i]));
		//memCorrY[i] = sig2 * ((corr * memY[i])+(dev * memCorrY[i]));
		//memCorrZ[i] = sig2 * ((corr * memZ[i])+(dev * memCorrZ[i]));

		//memX[i] = (*Dist)(*tgens[0]);
		//memY[i] = (*Dist)(*tgens[0]);
		//memZ[i] = (*Dist)(*tgens[0]);

		float m = 1.0/items[i].getMass();

		float x0 = items[i].getX0();
		float y0 = items[i].getY0();
		float z0 = items[i].getZ0();

		//Run the integration routine.
		float xNew = ((1.0+coEff0) * items[i].getX());
		xNew -= (coEff0 * x0);
		xNew += (m * dt2 * coEff1 * items[i].getFX());
		xNew += (m * dt2 * coEff2 * (items[i].getFX() - items[i].getFX0()));
		//xNew += (sig1 * memX[i]) + (coEff0 * memCorrX[i]);

		float yNew = ((1.0+coEff0) * items[i].getY()) ;
		yNew -= (coEff0 * y0);
		yNew += (m * dt2 * coEff1 * items[i].getFY());
		yNew += (m * dt2 * coEff2 * (items[i].getFY() - items[i].getFY0()));
		//yNew += (sig1 * memY[i]) + (coEff0 * memCorrY[i]);

		float zNew = ((1.0+coEff0) * items[i].getZ());
		zNew -= (coEff0 * z0);
		zNew += (m * dt2 * coEff1 * items[i].getFZ());
		zNew += (m * dt2 * coEff2 * (items[i].getFZ() - items[i].getFZ0()));
		//zNew += (sig1 * memZ[i]) + (coEff0 * memCorrZ[i]);

		//Velocity is not needed for brownianIntegration.
		//Run velocity integration at the same frequency as
		//the temperature/energy analysis routine.
		//-------------------------------------------------
		//For best perfomance use
		//velFreq = outputFreq.
		//-------------------------------------------------
		//If using a velocity dependant force use
		//velFreq = 0.
		//-------------------------------------------------
		//For all other cases do whatever.

		if (velFreq == 0)
		{
			velocityStep(items, i, xNew, yNew, zNew, dt, boxSize);
		}
		else if (velCounter == velFreq)
		{
			velocityStep(items, i, xNew, yNew, zNew, dt, boxSize);
		}

		items[i].setPos(xNew,yNew,zNew,boxSize);

		//Manage velocity output counter.
		if (velCounter == velFreq)
		{
			velCounter = 0;
		}
		else
		{
			velCounter++;
		}
	}

	__device__
	void brownianIntegrator::velocityStep(simulation::particle* items, int i, float xNew0, float yNew0, float zNew0, float dt, float boxSize)
	{
		float m = 1.0/items[i].getMass();

		//Current position and previous position are already PBC safe.
		//Their difference is also already PBC safe.
		float dx0 = items[i].getX() - items[i].getX0();
		float dy0 = items[i].getY() - items[i].getY0();
		float dz0 = items[i].getZ() - items[i].getZ0();

		//Make the new position PBC safe.
		float xNew = utilities::util::safeMod(xNew0,boxSize);
		float yNew = utilities::util::safeMod(yNew0,boxSize);
		float zNew = utilities::util::safeMod(zNew0,boxSize);

		//Make the difference between the new position and the current position PBC safe.
		float x0 = utilities::util::safeMod0(items[i].getX(), xNew, boxSize);
		float y0 = utilities::util::safeMod0(items[i].getY(), yNew, boxSize);
		float z0 = utilities::util::safeMod0(items[i].getZ(), zNew, boxSize);

		//Take the difference.
		float dx = xNew - x0;
		float dy = yNew - y0;
		float dz = zNew - z0;

		//Precompute.
		float dt2 = dt * dt;
		float dt3 = dt * dt2;

		//Run the integration routine.
		float vxNew = dx + dx0;
		vxNew += (m * dt2 * goy2 * items[i].getFX());
		vxNew -= (m * dt3 * goy3 * (items[i].getFX() - items[i].getFX0()));
		vxNew += (memCorrX[i] - sig1*memX[i]);
		vxNew *= (hn * dtInv);

		float vyNew = dy + dy0;
		vyNew += (m * dt2 * goy2 * items[i].getFY());
		vyNew -= (m * dt3 * goy3 * (items[i].getFY() - items[i].getFY0()));
		vyNew += (memCorrY[i] - sig1*memY[i]);
		vyNew *= (hn * dtInv);

		float vzNew = dz + dz0;
		vzNew += (m * dt2 * goy2 * items[i].getFZ());
		vzNew -= (m * dt3 * goy3 * (items[i].getFZ() - items[i].getFZ0()));
		vzNew += (memCorrZ[i] - sig1*memZ[i]);
		vzNew *= (hn * dtInv);

		//Set the velocities.
		items[i].setVX(vxNew);
		items[i].setVY(vyNew);
		items[i].setVZ(vzNew);
	}
}