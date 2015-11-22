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

#include "system.h"

namespace PSim {

void system::allocateCells(int scale) {
	cells = new PeriodicGrid***[scale];
	for (int i = 0; i < scale; i++) {
		cells[i] = new PeriodicGrid**[scale];
		for (int j = 0; j < scale; j++) {
			cells[i][j] = new PeriodicGrid*[scale];
			for (int k = 0; k < scale; k++) {
				cells[i][j][k] = new PeriodicGrid();
			}
		}
	}
}

void system::setCellNeighbors(int scale) {
	//Set the cell neighbors.
	for (int x = 0; x < scale; x++) {
		for (int y = 0; y < scale; y++) {
			for (int z = 0; z < scale; z++) {
				int left = x - 1;
				int right = x + 1;
				int top = y - 1;
				int bot = y + 1;
				int front = z - 1;
				int back = z + 1;
				if (x == 0) {
					left = (scale - 1);
				} else if (x == (scale - 1)) {
					right = 0;
				}

				if (y == 0) {
					top = (scale - 1);
				} else if (y == (scale - 1)) {
					bot = 0;
				}

				if (z == 0) {
					front = (scale - 1);
				} else if (z == (scale - 1)) {
					back = 0;
				}

				//Set the 6 principle cells next the current cell.
				//Access diagonals through combinations of these six.
				cells[x][y][z]->left = cells[left][y][z];
				cells[x][y][z]->right = cells[right][y][z];
				cells[x][y][z]->top = cells[x][top][z];
				cells[x][y][z]->bot = cells[x][bot][z];
				cells[x][y][z]->front = cells[x][y][front];
				cells[x][y][z]->back = cells[x][y][back];
			}
		}
	}

	//Check the cell neighborhood mapping.
	for (int x = 0; x < scale; x++) {
		for (int y = 0; y < scale; y++) {
			for (int z = 0; z < scale; z++) {
				cells[x][y][z]->createNeighborhood();
			}
		}
	}
}

void system::assignCellParticles() {
	//Assign the particle to their starting cell.
	for (int i = 0; i < nParticles; i++) {
		int cx = particles[i]->getX() / cellSize;
		int cy = particles[i]->getY() / cellSize;
		int cz = particles[i]->getZ() / cellSize;
		//Tell the particle what cell its in, then add to cell.
		particles[i]->setCell(cx, cy, cz);
		cells[cx][cy][cz]->addMember(particles[i]);
	}
}

/********************************************//**
 *------------------SYSTEM INIT-------------------
 ************************************************/

void system::initCells(int scale) {
	//Create the cells.
	allocateCells(scale);
	//Set the cell neighbors.
	setCellNeighbors(scale);
	//Assign the particle to their starting cell.
	assignCellParticles();
}

void system::initParticles(double r, double m) {
	particles = new particle*[nParticles];

	//If there is no inital seed create one.
	if (seed == 0) {
		std::random_device rd;
		seed = rd();
	}
	//Setup random uniform distribution generator.
	std::mt19937 gen(seed);
	std::uniform_real_distribution<double> distribution(0.0, 1.0);

	//Iterates through all points.
	for (int i = 0; i < nParticles; i++) {
		particles[i] = new particle(i);

		particles[i]->setX(distribution(gen) * boxSize, boxSize);
		particles[i]->setY(distribution(gen) * boxSize, boxSize);
		particles[i]->setZ(distribution(gen) * boxSize, boxSize);

		particles[i]->setRadius(r);
		particles[i]->setMass(m);

	}

	std::cout << "---Added " << nParticles
			<< " particles. Checking for overlap.\n\n";

	//Checks the system for overlap.
	initCheck(&gen, &distribution);

	std::cout << "\n\n---Overlap resolved. Creating Maxwell distribution.\n";

	//Set initial velocity.
	maxwellVelocityInit(&gen, &distribution);

	std::cout
			<< "---Maxwell distribution created. Creating cell assignment.\n\n";

}

void system::initCheck(std::mt19937* gen,
		std::uniform_real_distribution<double>* distribution) {
	//Keeps track of how many resolutions we have attempted.
	int counter = 0;

	//Search each particle for overlap.
	for (int i = 0; i < nParticles; i++) {
		PSim::util::loadBar(i, nParticles, i);
		//Is the problem resolved?
		bool resolution = false;
		//If not loop.
		while (resolution == false) {
			//Assume resolution.
			resolution = true;
			for (int j = 0; j < nParticles; j++) {
				//Exclude self interation.
				if (i != j) {
					//Gets the distance between the two particles.

					double radius = PSim::util::pbcDist(particles[i]->getX(),
							particles[i]->getY(), particles[i]->getZ(),
							particles[j]->getX(), particles[j]->getY(),
							particles[j]->getZ(), boxSize);

					//Gets the sum of the particle radius.
					double r = particles[i]->getRadius()
							+ particles[j]->getRadius();

					//If the particles are slightly closer than twice their radius resolve conflict.
					if (radius < 1.1 * r) {
						//Update resolution counter.
						counter++;

						//Throw warnings if stuck in resolution loop.
						if (counter > 10 * nParticles) {
							PSim::error::throwInitializationError();
						}

						//Assume new system in not resolved.
						resolution = false;

						//Set new uniform random position.
						particles[i]->setX((*distribution)(*gen) * boxSize,
								boxSize);
						particles[i]->setY((*distribution)(*gen) * boxSize,
								boxSize);
						particles[i]->setZ((*distribution)(*gen) * boxSize,
								boxSize);
					}
				}
			}
		}
	}

}

void system::maxwellVelocityInit(std::mt19937* gen,
		std::uniform_real_distribution<double>* distribution) {
	double r1, r2;
	double vsum, vsum2;
	double sigold, vsig, ratio;
	int i;

	//Set the initial velocities.
	for (i = 0; i < nParticles; i++) {
		r1 = (*distribution)(*gen);
		r2 = (*distribution)(*gen);

		particles[i]->setVX(sqrt(-2.0 * log(r1)) * cos(8.0 * atan(1) * r2));
	}

	for (i = 0; i < nParticles; i++) {
		r1 = (*distribution)(*gen);
		r2 = (*distribution)(*gen);
		particles[i]->setVY(sqrt(-2.0 * log(r1)) * cos(8.0 * atan(1) * r2));
	}

	for (i = 0; i < nParticles; i++) {
		r1 = (*distribution)(*gen);
		r2 = (*distribution)(*gen);
		particles[i]->setVZ(sqrt(-2.0 * log(r1)) * cos(8.0 * atan(1) * r2));
	}

	//Normalize the initial velocities according to the system temperature.
	vsum = 0;
	vsum2 = 0;

	for (i = 0; i < nParticles; i++) {
		double vx = particles[i]->getVX();
		vsum = vsum + vx;
		vsum2 = vsum2 + (vx * vx);
	}
	vsum = vsum / nParticles;
	vsum2 = vsum2 / nParticles;
	sigold = sqrt(vsum2 - (vsum * vsum));

	vsig = sqrt(temp);
	ratio = vsig / sigold;

	for (i = 0; i < nParticles; i++) {
		particles[i]->setVX(ratio * (particles[i]->getVX() - vsum));
	}

	//maxwell for vy//
	vsum = 0;
	vsum2 = 0;

	for (i = 0; i < nParticles; i++) {
		double vy = particles[i]->getVY();
		vsum = vsum + vy;
		vsum2 = vsum2 + (vy * vy);
	}
	vsum = vsum / nParticles;
	vsum2 = vsum2 / nParticles;
	sigold = sqrt(vsum2 - (vsum * vsum));

	vsig = sqrt(temp);
	ratio = vsig / sigold;

	for (i = 0; i < nParticles; i++) {
		particles[i]->setVY(ratio * (particles[i]->getVY() - vsum));
	}

	//maxwell for vz//
	vsum = 0;
	vsum2 = 0;

	for (i = 0; i < nParticles; i++) {
		double vz = particles[i]->getVZ();
		vsum = vsum + vz;
		vsum2 = vsum2 + (vz * vz);
	}
	vsum = vsum / nParticles;
	vsum2 = vsum2 / nParticles;
	sigold = sqrt(vsum2 - (vsum * vsum));

	vsig = sqrt(temp);
	ratio = vsig / sigold;

	for (i = 0; i < nParticles; i++) {
		particles[i]->setVZ(ratio * (particles[i]->getVZ() - vsum));
	}
}

std::string system::dirPrompt() {
	//Set the output directory.
	string outDir = "";
	bool validDir = 0;

	//Check that we get a real directory.
	while (!validDir) {
		cout << "Working directory: ";
		cin >> outDir;

		//Check that the directory exists.
		struct stat fileCheck;
		if (stat(outDir.c_str(), &fileCheck) != -1) {
			if (S_ISDIR(fileCheck.st_mode)) {
				validDir = 1;
			}
		}

		if (validDir == 0) {
			PSim::util::writeTerminal("\nInvalid Directory\n\n",
					PSim::Colour::Red);
		}

	}

	//Set the name of the trial.
	string tName = "";
	bool acceptName = 0;

	//Check that no trials get overwritten by accident.
	while (!acceptName) {
		validDir = 0;

		cout << "\n" << "Trial Name: ";
		cin >> tName;

		//Check input format.
		if (outDir.back() == '/') {
			tName = outDir + tName;
		} else {
			tName = outDir + "/" + tName;
		}

		//Check that the directory exists.
		struct stat fileCheck;
		if (stat(tName.c_str(), &fileCheck) != -1) {
			if (S_ISDIR(fileCheck.st_mode)) {
				validDir = 1;
			}
		}

		if (validDir == 1) {
			PSim::util::writeTerminal(
					"\nTrial name already exists. Overwrite (y,n): ",
					PSim::Colour::Magenta);

			//Check user input
			std::string cont;
			cin >> cont;

			if (cont == "Y" || cont == "y") {
				acceptName = 1;
			}
		} else {
			acceptName = 1;
		}

	}

	//Output the directory.
	cout << "\n" << "Data will be saved in: " << tName << "\n\n";
	mkdir(tName.c_str(), 0777);

	return tName;
}

bool system::checkDir(std::string path) {
	bool validDir = 0;
	struct stat fileCheck;
	if (stat(path.c_str(), &fileCheck) != -1) {
		if (S_ISDIR(fileCheck.st_mode)) {
			validDir = 1;
		}
	}
	return validDir;
}

void system::verifyPath() {
	//Check that the provided directory exists.
	bool validDir = checkDir(trialName);
	if (validDir == true) {
		PSim::util::writeTerminal(
				"\nTrial name already exists. Overwrite (y,n): ",
				PSim::Colour::Magenta);
		//Check user input
		std::string cont;
		std::cin >> cont;
		if (cont != "Y" && cont != "y") {
			trialName = dirPrompt();
		}
	} else {
		//Attempt to make the directory.
		mkdir(trialName.c_str(), 0777);
		//Check that we were able to make the desired directory.
		validDir = checkDir(trialName);
		if (validDir == false) {
			trialName = dirPrompt();
		}
	}
}

}
