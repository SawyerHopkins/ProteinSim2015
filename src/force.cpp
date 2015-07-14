/*The MIT License (MIT)

Copyright (c) <2015> <Sawyer Hopkins>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.*/

#include "force.h"

namespace physics
{

	/********************************************//**
	*---------------MANAGER CONSTRUCTION-------------
	 ***********************************************/

	forces::forces()
	{
		timeDependent=false;
	}

	forces::~forces()
	{
		//Free memory from the Force associated with the IForce Pointer.
		for (std::vector<IForce*>::iterator i = flist.begin(); i != flist.end(); ++i)
		{
			delete[] *i;
		}
		//Free memory of the IForce Pointers.
		delete[] &flist;
		delete[] &timeDependent;
	}

	/********************************************//**
	*-----------------FORCE MANAGEMENT---------------
	 ***********************************************/

	void forces::addForce(IForce* f)
	{
		flist.push_back(f);
		if (f->isTimeDependent())
		{
			timeDependent=true;
		}
	}

	void forces::getAcceleration(int nPart, int boxSize, double time, simulation::cell**** cells, simulation::particle** items)
	{
		#pragma omp parallel
		{
			#pragma omp for schedule(dynamic)
			for (int index = 0; index < nPart; index++)
			{
				//Resets the force on the particle.
				items[index]->nextIter();

				simulation::particle* p = items[index];
				simulation::cell* itemCell = cells[p->getCX()][p->getCY()][p->getCZ()];

				//Iterates through all forces.
				for (std::vector<IForce*>::iterator it = flist.begin(); it != flist.end(); ++it)
				{
					(*it)->getAcceleration(index, nPart, boxSize, time, itemCell, items);
				}
			}
		}
	}

}
