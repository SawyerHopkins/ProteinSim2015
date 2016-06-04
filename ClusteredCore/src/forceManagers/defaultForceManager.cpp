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

#include "forceManager.h"

namespace PSim {

/********************************************//**
 *---------------MANAGER CONSTRUCTION-------------
 ***********************************************/

defaultForceManager::defaultForceManager() {
	timeDependent = false;
	omp_set_dynamic(0);
	omp_set_num_threads(1);
}

defaultForceManager::~defaultForceManager() {
	//Free memory from the Force associated with the IForce Pointer.
	for (std::vector<IForce*>::iterator i = flist.begin(); i != flist.end();
			++i) {
		delete[] *i;
	}
	//Free memory of the IForce Pointers.
	delete[] &flist;
	delete[] &timeDependent;
}

/********************************************//**
 *-----------------FORCE MANAGEMENT---------------
 ***********************************************/

void defaultForceManager::addForce(IForce* f) {
	flist.push_back(f);
	if (f->isTimeDependent()) {
		timeDependent = true;
	}
}

void defaultForceManager::getAcceleration(double* sortedParticles, double* particleForce, vector<tuple<int,int>>* particleHashIndex, vector<tuple<int,int>>* cellStartEnd,systemState* state) {
	IForce* currentForce = flist[0];
#pragma omp parallel
	{
#pragma omp for
		for (int index = 0; index < state->nParticles; index++) {
			//Iterates through all forces.
			currentForce->getAcceleration(index, sortedParticles, particleForce, particleHashIndex, cellStartEnd, state);
		}
	}
}

#ifdef WITHPOST
void defaultForceManager::getPostRoutine(double* sortedParticles, double* particleForce, vector<tuple<int,int>>* particleHashIndex, vector<tuple<int,int>>* cellStartEnd,systemState* state) {
	IForce* currentForce = flist[0];
#pragma omp parallel
	{
#pragma omp for
		for (int index = 0; index < state->nParticles; index++) {
			//Iterates through all forces.
			currentForce->postRoutine(index, sortedParticles, particleForce, particleHashIndex, cellStartEnd, state);
		}
	}
}
#endif

}
