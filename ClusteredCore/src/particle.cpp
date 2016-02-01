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

	pos = type3<double>();
	pos0 = type3<double>();
	vel = type3<double>();
	frc = type3<double>();
	frc0 = type3<double>();
	cll = type3<int>(-1,-1,-1);

	r = 0.0;
	m = 0.0;

	coorNumber = 0;

	//Make sure interactions is clear.
	//std::vector<particle*>().swap(interactions);

}

particle::~particle() {
	delete &pos;
	delete &pos0;
	delete &vel;
	delete &frc;
	delete &frc0;
	delete &cll;
	delete &m;
	delete &r;
	delete &name;
	delete &coorNumber;
}

/********************************************//**
 *---------------SYSTEM MANAGEMENT----------------
 ************************************************/

void particle::setX(double val, double boxSize) {
	double xTemp = pos.x;
	//Update current position.
	pos.x = PSim::util::safeMod(val, boxSize);
	//Set lat position.
	pos0.x = PSim::util::safeMod0(xTemp, pos.x, boxSize);
	if ((pos.x < 0.0) || (pos.x >= boxSize)){
		PSim::error::throwParticleBoundsError(&pos, name);
	}
}

void particle::setY(double val, double boxSize) {
	double yTemp = pos.y;
	//Update current position.
	pos.y = PSim::util::safeMod(val, boxSize);
	//Set lat position.
	pos0.y = PSim::util::safeMod0(yTemp, pos.y, boxSize);
	if ((pos.y < 0.0) || (pos.y >= boxSize)) {
		PSim::error::throwParticleBoundsError(&pos, name);
	}
}

void particle::setZ(double val, double boxSize) {
	double zTemp = pos.z;
	//Update current position.
	pos.z = PSim::util::safeMod(val, boxSize);
	//Set lat position.
	pos0.z = PSim::util::safeMod0(zTemp, pos.z, boxSize);
	if ((pos.z < 0.0) || (pos.z >= boxSize)) {
		PSim::error::throwParticleBoundsError(&pos, name);
	}
}

void particle::setPos(type3<double>* pos, int boxSize) {
	//Update all the positions.
	setX(pos->x, boxSize);
	setY(pos->y, boxSize);
	setZ(pos->z, boxSize);
}

void particle::updateForce(type3<double>* pos,
		particle* p, bool countPair) {
	//Add to coordination number.
	if (countPair == true) {
		coorNumber++;
		interactions.push_back(p);
	}

	//Increment the existing value of force.
	frc.x += pos->x;
	frc.y += pos->y;
	frc.z += pos->z;
}

float particle::calculatePotential() {
	float dx = pos.x-pos0.x;
	float dy = pos.y-pos0.y;
	float dz = pos.z-pos0.z;

	float px = (pos0.x*dx) + (0.5*dx*(frc.x-frc0.x));
	float py = (pos0.y*dy) + (0.5*dy*(frc.y-frc0.y));
	float pz = (pos0.z*dz) + (0.5*dz*(frc.z-frc0.z));

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
	frc0.x = frc.x;
	frc0.y = frc.y;
	frc0.z = frc.z;
	frc.x = 0.0;
	frc.y = 0.0;
	frc.z = 0.0;
}
}

