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

#include "particle.h"

namespace PSim {

/********************************************//**
 *--------------SYSTEM CONSTRUCTION---------------
 ************************************************/

particle::particle(int pid) {
	//Set the initial parameters.
	name = pid;

	coorNumber = 0;

	x = 0.0;
	y = 0.0;
	z = 0.0;

	x0 = 0.0;
	y0 = 0.0;
	z0 = 0.0;

	fx = 0.0;
	fy = 0.0;
	fz = 0.0;

	fx0 = 0.0;
	fy0 = 0.0;
	fz0 = 0.0;

	vx = 0.0;
	vy = 0.0;
	vz = 0.0;

	//For debugging.
	cx = -1;
	cy = -1;
	cz = -1;

	r = 0.0;
	m = 0.0;

	coorNumber = 0;

	//Make sure interactions is clear.
	//std::vector<particle*>().swap(interactions);

}

particle::~particle() {
	delete &x;
	delete &y;
	delete &z;
	delete &x0;
	delete &y0;
	delete &z0;
	delete &fx;
	delete &fy;
	delete &fz;
	delete &fx0;
	delete &fy0;
	delete &fz0;
	delete &vx;
	delete &vy;
	delete &vz;
	delete &m;
	delete &r;
	delete &cx;
	delete &cy;
	delete &cz;
	delete &name;
	delete &coorNumber;
}

/********************************************//**
 *---------------SYSTEM MANAGEMENT----------------
 ************************************************/

void particle::setX(double val, double boxSize) {
	double xTemp = x;
	//Update current position.
	x = PSim::util::safeMod(val, boxSize);
	//Set lat position.
	x0 = PSim::util::safeMod0(xTemp, x, boxSize);
	if ((x < 0.0) || (x >= boxSize)) {
		PSim::error::throwParticleBoundsError(x, y, z, name);
	}
}

void particle::setY(double val, double boxSize) {
	double yTemp = y;
	//Update current position.
	y = PSim::util::safeMod(val, boxSize);
	//Set lat position.
	y0 = PSim::util::safeMod0(yTemp, y, boxSize);
	if ((y < 0.0) || (y >= boxSize)) {
		PSim::error::throwParticleBoundsError(x, y, z, name);
	}
}

void particle::setZ(double val, double boxSize) {
	double zTemp = z;
	//Update current position.
	z = PSim::util::safeMod(val, boxSize);
	//Set lat position.
	z0 = PSim::util::safeMod0(zTemp, z, boxSize);
	if ((z < 0.0) || (z >= boxSize)) {
		PSim::error::throwParticleBoundsError(x, y, z, name);
	}
}

void particle::setPos(double xVal, double yVal, double zVal, double boxSize) {
	//Update all the positions.
	setX(xVal, boxSize);
	setY(yVal, boxSize);
	setZ(zVal, boxSize);
}

void particle::updateForce(double xVal, double yVal, double zVal,
		particle* p, bool countPair) {
	//Add to coordination number.
	if (countPair == true) {
		coorNumber++;
		interactions.push_back(p);
	}

	//Increment the existing value of force.
	fx += xVal;
	fy += yVal;
	fz += zVal;
}

float particle::calculatePotential() {
	float dx = x-x0;
	float dy = y-y0;
	float dz = z-z0;

	float px = (x0*dx) + (0.5*dx*(fx-fx0));
	float py = (y0*dy) + (0.5*dy*(fy-fy0));
	float pz = (z0*dz) + (0.5*dz*(fz-fz0));

	return px+py+pz;
}

void particle::nextIter() {

	//Reset interacting particles.
	interactions.clear();
	interactions.shrink_to_fit();
	//Minimize memory allocations by making a guess at how many slots we need.
	interactions.reserve(coorNumber);

	//Reset coordination number;
	coorNumber = 0;

	//Set the old force before clearing the current force.
	fx0 = fx;
	fy0 = fy;
	fz0 = fz;
	fx = 0.0;
	fy = 0.0;
	fz = 0.0;
}
}

