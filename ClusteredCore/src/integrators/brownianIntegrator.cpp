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

namespace PSim {

brownianIntegrator::brownianIntegrator(configReader::config* cfg) {

	//Sets the name
	name = "brownianIntegrator";

	//Set the number of particles.
	memSize = cfg->getParam<int>("nParticles", 1000);
	velFreq = cfg->getParam<int>("velFreq", 1000);
	velCounter = 0;

	//Create he memory blocks for mem and memCoor
	memX = new double[memSize];
	memY = new double[memSize];
	memZ = new double[memSize];
	memCorrX = new double[memSize];
	memCorrY = new double[memSize];
	memCorrZ = new double[memSize];

	//Setup the particle random number generators.
	rndGens = new fastRandom*[memSize];

	for (int i=0; i<memSize; i++) {
		rndGens[i] = new fastRandom();
	}

	//Sets the system temperature.
	temp = cfg->getParam<double>("temp", 1.0);

	//Set the mass.
	mass = cfg->getParam<double>("mass", 1.0);

	//Sets the system drag.
	gamma = cfg->getParam<double>("gamma", 0.5);

	//Sets the integration time step.
	dt = cfg->getParam<double>("timeStep", 0.001);
	dtInv = 1.0 / dt;

	//Create vital variables
	y = gamma * dt;

	setupHigh(cfg);
	if (gamma < 0.05) {
		setupLow(cfg);
	}
	if (gamma == 0) {
		setupZero(cfg);
	}

	double gamma2 = gamma * gamma;

	sig1 = sqrt(+temp * sig1 / gamma2);
	sig2 = sqrt(-temp * sig2 / gamma2);
	corr = (temp / (gamma2)) * (gn / (sig1 * sig2));
	dev = sqrt(1.0 - (corr * corr));

	seed = cfg->getParam<int>("seed", 90210);

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

brownianIntegrator::~brownianIntegrator() {
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

void brownianIntegrator::setupHigh(configReader::config* cfg) {
	//Coefficients for High Gamma.
	//SEE GUNSTEREN AND BERENDSEN 1981
	double ty = 2.0 * y;

	coEff0 = exp(-y);
	double aa1 = 1.0 - coEff0;
	double aa2 = 0.5 * y * (1.0 + coEff0) - aa1;
	double aa3 = y - aa1;
	coEff1 = aa1 / y;
	coEff2 = aa2 / (y * y);
	coEff3 = aa3 / (y * y);

	sig1 = 2.0 * y - 3.0 + 4.0 * exp(-y) - exp(-ty);
	sig2 = -2.0 * y - 3.0 + 4.0 * exp(y) - exp(ty);

	gn = exp(y) - ty - exp(-y);

	goy2 = gn / (y * y);
	goy3 = gn / (y * y * y);

	hn = y / (exp(y) - exp(-y));
}

void brownianIntegrator::setupLow(configReader::config* cfg) {
	//Coefficients for Low Gamma (from series expansion).
	//SEE GUNSTEREN AND BERENDSEN 1981
	double y1 = y;
	double y2 = y1 * y1;
	double y3 = y2 * y1;
	double y4 = y3 * y1;
	double y5 = y4 * y1;
	double y6 = y5 * y1;
	double y7 = y6 * y1;
	double y8 = y7 * y1;
	double y9 = y8 * y1;

	coEff1 = 1.0 - 0.5 * y1 + (1.0 / 6.0) * y2 - (1.0 / 24.0) * y3
			+ (1.0 / 120.0) * y4;
	coEff2 = (1.0 / 12.0) * y1 - (1.0 / 24.0) * y2 + (1.0 / 80.0) * y3
			- (1.0 / 360.0) * y4;
	coEff3 = 0.5 - (1.0 / 6.0) * y1 + (1.0 / 24.0) * y2 - (1.0 / 120.0) * y3;

	sig1 = +(2.0 / 3.0) * y3 - 0.5 * y4 + (7.0 / 30.0) * y5 - (1.0 / 12.0) * y6
			+ (31.0 / 1260.0) * y7 - (1.0 / 160.0) * y8
			+ (127.0 / 90720.0) * y9;
	sig2 = -(2.0 / 3.0) * y3 - 0.5 * y4 - (7.0 / 30.) * y5 - (1.0 / 12.0) * y6
			- (31.0 / 1260.0) * y7 - (1.0 / 160.0) * y8
			- (127.0 / 90720.0) * y9;

	goy2 = (1.0 / 3.0) * y1 + (1.0 / 60.0) * y3;
	goy3 = 1.0 / 3.0 + (1.0 / 60.0) * y2;

	hn = 0.5 - (1.00 / 12.0) * y2 + (7.0 / 720.0) * y4;
	gn = (1.0 / 3.0) * y3 + (1.0 / 60.0) * y5;
}

void brownianIntegrator::setupZero(configReader::config* cfg) {
	//Special case coefficients.
	//SEE GUNSTEREN AND BERENDSEN 1981
	coEff0 = 1.0;
	coEff1 = 1.0;
	coEff2 = 0.0;
	coEff3 = 0.5;
}

double brownianIntegrator::getWidth(double y) {
	return (2 * y) - 3.0 + (4.0 * exp(-y)) - exp(-2.0 * y);
}

int brownianIntegrator::nextSystem(double time, double dt, int nParticles,
		int boxSize, PSim::PeriodicGrid**** cells, PSim::particle** items,
		PSim::defaultForceManager* f) {
	//Checks what method is needed.
	if (time == 0) {
		firstStep(time, dt, nParticles, boxSize, items, f);
	} else {
		normalStep(time, dt, nParticles, boxSize, items, f);
	}
	return 0;
}

int brownianIntegrator::firstStep(double time, double dt, int nParticles,
		int boxSize, PSim::particle** items, PSim::defaultForceManager* f) {
	//Add 4 threads to the team.
#pragma omp parallel
{
#pragma omp for
	for (int i = 0; i < nParticles; i++) {

		//SEE GUNSTEREN AND BERENDSEN 1981 EQ 2.26

		memCorrX[i] = 0.0;
		memCorrY[i] = 0.0;
		memCorrZ[i] = 0.0;

		int threadSeed = seed*(i+1);

		memX[i] = rndGens[i]->g250(threadSeed);
		memY[i] = rndGens[i]->g250(threadSeed);
		memZ[i] = rndGens[i]->g250(threadSeed);

		double m = 1.0 / items[i]->getMass();
		double xNew = items[i]->getX() + (items[i]->getVX() * coEff1 * dt)
				+ (items[i]->getFX() * coEff3 * dt * dt * m) + (sig1 * memX[i]);
		double yNew = items[i]->getY() + (items[i]->getVY() * coEff1 * dt)
				+ (items[i]->getFY() * coEff3 * dt * dt * m) + (sig1 * memY[i]);
		double zNew = items[i]->getZ() + (items[i]->getVZ() * coEff1 * dt)
				+ (items[i]->getFZ() * coEff3 * dt * dt * m) + (sig1 * memZ[i]);
		items[i]->setPos(xNew, yNew, zNew, boxSize);

	}
}
	return 0;
}

int brownianIntegrator::normalStep(double time, double dt, int nParticles,
		int boxSize, PSim::particle** items, PSim::defaultForceManager* f) {

	double dt2 = dt * dt;
	double c0 = 1.0 + coEff0;
#pragma omp parallel
{
#pragma omp for
	for (int i = 0; i < nParticles; i++) {
		//SEE GUNSTEREN AND BERENDSEN 1981 EQ 2.26
		//New random walk.

		int threadSeed = seed*(i+1);

		memCorrX[i] = rndGens[i]->g250(threadSeed);
		memCorrY[i] = rndGens[i]->g250(threadSeed);
		memCorrZ[i] = rndGens[i]->g250(threadSeed);

		//Correlation to last random walk.
		memCorrX[i] = sig2 * ((corr * memX[i]) + (dev * memCorrX[i]));
		memCorrY[i] = sig2 * ((corr * memY[i]) + (dev * memCorrY[i]));
		memCorrZ[i] = sig2 * ((corr * memZ[i]) + (dev * memCorrZ[i]));

		memX[i] = rndGens[i]->g250(threadSeed);
		memY[i] = rndGens[i]->g250(threadSeed);
		memZ[i] = rndGens[i]->g250(threadSeed);

		double m = 1.0 / items[i]->getMass();
		double c1 = m * dt2 * coEff1;
		double c2 = m * dt2 * coEff2;

		//Run the integration routine.
		double xNew = (c0 * items[i]->getX());
		xNew -= (coEff0 * items[i]->getX0());
		xNew += (c1 * items[i]->getFX());
		xNew += (c2 * (items[i]->getFX() - items[i]->getFX0()));
		xNew += (sig1 * memX[i]) + (coEff0 * memCorrX[i]);

		double yNew = (c0 * items[i]->getY());
		yNew -= (coEff0 * items[i]->getY0());
		yNew += (c1 * items[i]->getFY());
		yNew += (c2 * (items[i]->getFY() - items[i]->getFY0()));
		yNew += (sig1 * memY[i]) + (coEff0 * memCorrY[i]);

		double zNew = (c0 * items[i]->getZ());
		zNew -= (coEff0 * items[i]->getZ0());
		zNew += (c1 * items[i]->getFZ());
		zNew += (c2 * (items[i]->getFZ() - items[i]->getFZ0()));
		zNew += (sig1 * memZ[i]) + (coEff0 * memCorrZ[i]);

		//Velocity is not needed for brownianIntegration.
		//Run velocity integration at the same frequency as
		//the temperature/energy analysis routine.
		//-------------------------------------------------
		//For best performance use
		//velFreq = outputFreq.
		//-------------------------------------------------
		//If using a velocity dependent force use
		//velFreq = 0.
		//-------------------------------------------------
		//For all other cases do whatever.
		(velFreq == 0 || velCounter == velFreq) ? velocityStep(items, i, xNew, yNew, zNew, dt, boxSize) :items[i]->setPos(xNew, yNew, zNew, boxSize);
	}
}
	//Manage velocity output counter.
	(velCounter == velFreq) ? velCounter = 0 : velCounter++;

	return 0;

}

void brownianIntegrator::velocityStep(PSim::particle** items, int i,
		double xNew0, double yNew0, double zNew0, double dt, double boxSize) {

	double m = 1.0 / items[i]->getMass();

	//Current position and previous position are already PBC safe.
	//Their difference is also already PBC safe.
	double dx0 = items[i]->getX() - items[i]->getX0();
	double dy0 = items[i]->getY() - items[i]->getY0();
	double dz0 = items[i]->getZ() - items[i]->getZ0();

	//Make the new position PBC safe.
	double xNew = PSim::util::safeMod(xNew0, boxSize);
	double yNew = PSim::util::safeMod(yNew0, boxSize);
	double zNew = PSim::util::safeMod(zNew0, boxSize);

	//Make the difference between the new position and the current position PBC safe.
	double x0 = PSim::util::safeMod0(items[i]->getX(), xNew, boxSize);
	double y0 = PSim::util::safeMod0(items[i]->getY(), yNew, boxSize);
	double z0 = PSim::util::safeMod0(items[i]->getZ(), zNew, boxSize);

	//Take the difference.
	double dx = xNew - x0;
	double dy = yNew - y0;
	double dz = zNew - z0;

	//Pre-compute.
	double dt2 = dt * dt;
	double dt3 = dt * dt2;

	double g2 = m * dt2 * goy2;
	double g3 = m * dt3 * goy3;
	double g4 = hn * dtInv;

	//Run the integration routine.
	double vxNew = dx + dx0;
	vxNew += (g2 * items[i]->getFX());
	vxNew -= (g3 * (items[i]->getFX() - items[i]->getFX0()));
	vxNew += (memCorrX[i] - sig1 * memX[i]);
	vxNew *= g4;

	double vyNew = dy + dy0;
	vyNew += (g2 * items[i]->getFY());
	vyNew -= (g3 * (items[i]->getFY() - items[i]->getFY0()));
	vyNew += (memCorrY[i] - sig1 * memY[i]);
	vyNew *= g4;

	double vzNew = dz + dz0;
	vzNew += (g2 * items[i]->getFZ());
	vzNew -= (g3 * (items[i]->getFZ() - items[i]->getFZ0()));
	vzNew += (memCorrZ[i] - sig1 * memZ[i]);
	vzNew *= g4;

	//Set the velocities.
	items[i]->setVX(vxNew);
	items[i]->setVY(vyNew);
	items[i]->setVZ(vzNew);
	items[i]->setPos(xNew, yNew, zNew, boxSize);
}

}
