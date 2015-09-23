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

namespace simulation
{

	__device__
	cell::cell(int cellParts) : members(new particle*[cellParts])
	{
		maxMem = cellParts;
		gridCounter = 0;
		//Set neighbors as self until initialized by the system.
		top = this;
		bot = this;
		front = this;
		back = this;
		left = this;
		right = this;
	}

	__device__
	cell::~cell()
	{
		delete[] &members;
		delete[] &neighbors;
	}

	__device__
	int cell::findIndex(particle* p)
	{
		int i =0;
		bool search = true;
		while (search)
		{
			if (members[i]->getName() == p->getName()) return i;
		}
		return -1;
	}

	__device__
	void cell::createNeighborhood()
	{
		//Add the cross section at the cell.
		neighbors[0] = left;
		neighbors[1] = right;
		neighbors[2] = top;
		neighbors[3] = bot;
		neighbors[4] = top->left;
		neighbors[5] = top->right;
		neighbors[6] = bot->left;
		neighbors[7] = bot->right;

		//Adds the cross section in front of the cell.
		neighbors[8] = front;
		neighbors[9] = front->left;
		neighbors[10] = front->right;
		neighbors[11] = front->top;
		neighbors[12] = front->bot;
		neighbors[13] = front->top->left;
		neighbors[14] = front->top->right;
		neighbors[15] = front->bot->left;
		neighbors[16] = front->bot->right;

		//Adds the cross section behind of the cell.
		neighbors[17] = back;
		neighbors[18] = back->left;
		neighbors[19] = back->right;
		neighbors[20] = back->top;
		neighbors[21] = back->bot;
		neighbors[22] = back->top->left;
		neighbors[23] = back->top->right;
		neighbors[24] = back->bot->left;
		neighbors[25] = back->bot->right;

		//Adds the parent cell to the end of the vector.
		neighbors[26]=this;
	}

}

