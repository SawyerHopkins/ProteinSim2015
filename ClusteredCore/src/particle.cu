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

namespace simulation
{
	/********************************************//**
	*--------------SYSTEM CONSTRUCTION---------------
	************************************************/

	__device__ __host__
	particle::particle(int pid)
	{
		init(pid);
	}

	__device__ __host__
	particle::~particle()
	{
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
		delete &potential;
	}

	__device__ __host__
	void particle::init(int pid)
	{
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
		potential = 0;
	}

	/********************************************//**
	*---------------SYSTEM MANAGEMENT----------------
	************************************************/

	__device__ __host__
	void particle::setX(float val, int boxSize)
	{
		float xTemp = x;
		//Update current position.
		x = utilities::util::safeMod(val, boxSize);
		//Set lat position.
		x0 = utilities::util::safeMod0(xTemp,x,boxSize);
		if ((x < 0.0) || (x >= boxSize))
		{
			debugging::error::throwParticleBoundsError(x,y,z);
		}
	}

	__device__ __host__
	void particle::setY(float val, int boxSize)
	{
		float yTemp = y;
		//Update current position.
		y = utilities::util::safeMod(val, boxSize);
		//Set lat position.
		y0 = utilities::util::safeMod0(yTemp,y,boxSize);
		if ((y < 0.0) || (y >= boxSize))
		{
			debugging::error::throwParticleBoundsError(x,y,z);
		}
	}

	__device__ __host__
	void particle::setZ(float val, int boxSize)
	{
		float zTemp = z;
		//Update current position.
		z = utilities::util::safeMod(val, boxSize);
		//Set lat position.
		z0 = utilities::util::safeMod0(zTemp,z,boxSize);
		if ((z < 0.0) || (z >= boxSize))
		{
			debugging::error::throwParticleBoundsError(x,y,z);
		}
	}

	__device__ __host__
	void particle::setPos(float xVal, float yVal, float zVal, int boxSize)
	{
		//Update all the positions.
		setX(xVal,boxSize);
		setY(yVal,boxSize);
		setZ(zVal,boxSize);
	}

	__device__ __host__
	void particle::updateForce(float xVal, float yVal, float zVal, float pot, particle* p, bool countPair)
	{
		//Add to potential.
		potential+=pot;


		//Add to coordination number.
		if (countPair == true)
		{
			coorNumber++;
		}

		//Increment the existing value of force.
		fx += xVal;
		fy += yVal;
		fz += zVal;
	}

	__device__ __host__
	void particle::nextIter()
	{

		//Reset coordination number;
		coorNumber = 0;

		//Reset potential;
		potential = 0;

		//Set the old force before clearing the current force.
		fx0 = fx;
		fy0 = fy;
		fz0 = fz;
		fx = 0.0;
		fy = 0.0;
		fz = 0.0;
	}

	__device__
	void particle::copyDummy(particle* dummy)
	{
		name = dummy->getName();

		x = dummy->getX();
		y = dummy->getY();
		z = dummy->getZ();

		x0 = dummy->getX0();
		y0 = dummy->getY0();
		z0 = dummy->getZ0();

		fx = dummy->getFX();
		fy = dummy->getFY();
		fz = dummy->getFZ();
		
		fx0 = dummy->getFX0();
		fy0 = dummy->getFY0();
		fz0 = dummy->getFZ0();

		vx = dummy->getVX();
		vy = dummy->getVY();
		vz = dummy->getVZ();

		r = dummy->getRadius();
		m = dummy->getMass();

		cx = dummy->getCX();
		cy = dummy->getCY();
		cz = dummy->getCZ();

		coorNumber = 0;
		potential = 0;

	}
}

