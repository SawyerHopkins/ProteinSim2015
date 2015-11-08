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

#include "cell.h"

namespace PSim {

PeriodicGrid::PeriodicGrid() {
	//Set neighbors as self until initialized by the system.
	top = this;
	bot = this;
	front = this;
	back = this;
	left = this;
	right = this;
}

PeriodicGrid::~PeriodicGrid() {
	delete[] &members;
}

void PeriodicGrid::addMember(particle* p) {
	//Adds member with name as the key.
	members[p->getName()] = p;
}

void PeriodicGrid::removeMember(particle* p) {
	//Removes the member by associated key.
	members.erase(p->getName());
}

void PeriodicGrid::createNeighborhood() {
	//Add the cross section at the cell.
	neighbors.push_back(left);
	neighbors.push_back(right);
	neighbors.push_back(top);
	neighbors.push_back(bot);
	neighbors.push_back(top->left);
	neighbors.push_back(top->right);
	neighbors.push_back(bot->left);
	neighbors.push_back(bot->right);

	//Adds the cross section in front of the cell.
	neighbors.push_back(front);
	neighbors.push_back(front->left);
	neighbors.push_back(front->right);
	neighbors.push_back(front->top);
	neighbors.push_back(front->bot);
	neighbors.push_back(front->top->left);
	neighbors.push_back(front->top->right);
	neighbors.push_back(front->bot->left);
	neighbors.push_back(front->bot->right);

	//Adds the cross section behind of the cell.
	neighbors.push_back(back);
	neighbors.push_back(back->left);
	neighbors.push_back(back->right);
	neighbors.push_back(back->top);
	neighbors.push_back(back->bot);
	neighbors.push_back(back->top->left);
	neighbors.push_back(back->top->right);
	neighbors.push_back(back->bot->left);
	neighbors.push_back(back->bot->right);

	//Adds the parent cell to the end of the vector.
	neighbors.push_back(this);
}

}

