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

	__global__
	void brownianIntegrator::nextSystem(double *time, double *dt, int *nParticles, int *boxSize, simulation::cell**** cells, simulation::particle** items, physics::forces* f)
	{
		//Checks what method is needed.
		if (time == 0)
		{
			firstStep(time, dt, nParticles, boxSize, items, f);
		}
		else
		{
			normalStep(time, dt, nParticles, boxSize, items, f);
		}
	}

	__device__
	void brownianIntegrator::firstStep(double time, double dt, int nParticles, int boxSize, simulation::particle** items, physics::forces* f)
	{
		//Add 4 threads to the team.
		for (int i=0; i < nParticles; i++)
		{

			//SEE GUNSTEREN AND BERENDSEN 1981 EQ 2.26

			memCorrX[i] = 0.0;
			memCorrY[i] = 0.0;
			memCorrZ[i] = 0.0;

			memX[i] = (*Dist)(*gen);
			memY[i] = (*Dist)(*gen);
			memZ[i] = (*Dist)(*gen);

			double m = 1.0/items[i]->getMass();
			double xNew = items[i]->getX() + (items[i]->getVX() * coEff1 * dt) + (items[i]->getFX() * coEff3 * dt * dt * m) + (sig1 * memX[i]);
			double yNew = items[i]->getY() + (items[i]->getVY() * coEff1 * dt) + (items[i]->getFY() * coEff3 * dt * dt * m) + (sig1 * memY[i]);
			double zNew = items[i]->getZ() + (items[i]->getVZ() * coEff1 * dt) + (items[i]->getFZ() * coEff3 * dt * dt * m) + (sig1 * memZ[i]);
			items[i]->setPos(xNew,yNew,zNew,boxSize);

		}
	}

	__device__
	void brownianIntegrator::normalStep(double time, double dt, int nParticles, int boxSize, simulation::particle** items, physics::forces* f)
	{

		double dt2 = dt * dt;
		//SEE GUNSTEREN AND BERENDSEN 1981 EQ 2.26
		//New random walk.
		memCorrX[i] = (*Dist)(*tgens[omp_get_thread_num()]);
		memCorrY[i] = (*Dist)(*tgens[omp_get_thread_num()]);
		memCorrZ[i] = (*Dist)(*tgens[omp_get_thread_num()]);

		//Correlation to last random walk.
		memCorrX[i] = sig2 * ((corr * memX[i])+(dev * memCorrX[i]));
		memCorrY[i] = sig2 * ((corr * memY[i])+(dev * memCorrY[i]));
		memCorrZ[i] = sig2 * ((corr * memZ[i])+(dev * memCorrZ[i]));

		memX[i] = (*Dist)(*tgens[omp_get_thread_num()]);
		memY[i] = (*Dist)(*tgens[omp_get_thread_num()]);
		memZ[i] = (*Dist)(*tgens[omp_get_thread_num()]);

		double m = 1.0/items[i]->getMass();

		double x0 = items[i]->getX0();
		double y0 = items[i]->getY0();
		double z0 = items[i]->getZ0();

		//Run the integration routine.
		double xNew = ((1.0+coEff0) * items[i]->getX());
		xNew -= (coEff0 * x0);
		xNew += (m * dt2 * coEff1 * items[i]->getFX());
		xNew += (m * dt2 * coEff2 * (items[i]->getFX() - items[i]->getFX0()));
		xNew += (sig1 * memX[i]) + (coEff0 * memCorrX[i]);

		double yNew = ((1.0+coEff0) * items[i]->getY()) ;
		yNew -= (coEff0 * y0);
		yNew += (m * dt2 * coEff1 * items[i]->getFY());
		yNew += (m * dt2 * coEff2 * (items[i]->getFY() - items[i]->getFY0()));
		yNew += (sig1 * memY[i]) + (coEff0 * memCorrY[i]);

		double zNew = ((1.0+coEff0) * items[i]->getZ());
		zNew -= (coEff0 * z0);
		zNew += (m * dt2 * coEff1 * items[i]->getFZ());
		zNew += (m * dt2 * coEff2 * (items[i]->getFZ() - items[i]->getFZ0()));
		zNew += (sig1 * memZ[i]) + (coEff0 * memCorrZ[i]);

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

		items[i]->setPos(xNew, yNew, zNew, boxSize);

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
	void brownianIntegrator::velocityStep(simulation::particle** items, int i, double xNew0, double yNew0, double zNew0, double dt, double boxSize)
	{

		double m = 1.0/items[i]->getMass();

		//Current position and previous position are already PBC safe.
		//Their difference is also already PBC safe.
		double dx0 = items[i]->getX() - items[i]->getX0();
		double dy0 = items[i]->getY() - items[i]->getY0();
		double dz0 = items[i]->getZ() - items[i]->getZ0();

		//Make the new position PBC safe.
		double xNew = utilities::util::safeMod(xNew0,boxSize);
		double yNew = utilities::util::safeMod(yNew0,boxSize);
		double zNew = utilities::util::safeMod(zNew0,boxSize);

		//Make the difference between the new position and the current position PBC safe.
		double x0 = utilities::util::safeMod0(items[i]->getX(), xNew, boxSize);
		double y0 = utilities::util::safeMod0(items[i]->getY(), yNew, boxSize);
		double z0 = utilities::util::safeMod0(items[i]->getZ(), zNew, boxSize);

		//Take the difference.
		double dx = xNew - x0;
		double dy = yNew - y0;
		double dz = zNew - z0;

		//Precompute.
		double dt2 = dt * dt;
		double dt3 = dt * dt2;

		//Run the integration routine.
		double vxNew = dx + dx0;
		vxNew += (m * dt2 * goy2 * items[i]->getFX());
		vxNew -= (m * dt3 * goy3 * (items[i]->getFX() - items[i]->getFX0()));
		vxNew += (memCorrX[i] - sig1*memX[i]);
		vxNew *= (hn * dtInv);

		double vyNew = dy + dy0;
		vyNew += (m * dt2 * goy2 * items[i]->getFY());
		vyNew -= (m * dt3 * goy3 * (items[i]->getFY() - items[i]->getFY0()));
		vyNew += (memCorrY[i] - sig1*memY[i]);
		vyNew *= (hn * dtInv);

		double vzNew = dz + dz0;
		vzNew += (m * dt2 * goy2 * items[i]->getFZ());
		vzNew -= (m * dt3 * goy3 * (items[i]->getFZ() - items[i]->getFZ0()));
		vzNew += (memCorrZ[i] - sig1*memZ[i]);
		vzNew *= (hn * dtInv);

		//Set the velocities.
		items[i]->setVX(vxNew);
		items[i]->setVY(vyNew);
		items[i]->setVZ(vzNew);

	}
}