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

#include "error.h"

namespace PSim {

void error::throwInitializationError() {
	chatterBox.startErrorLog(7701, "Could not create initial system.");
	chatterBox.logErrorMessage("Try decreasing particle density.");
	chatterBox.endErrorLog();
	exit(7701);
}

void error::throwCellBoundsError(int cx, int cy, int cz) {
	chatterBox.startErrorLog(7702, "Unable to find: cells[" + tos(cx) + "][" + tos(cy) + "][" + tos(cz) + "].");
	chatterBox.endErrorLog();
	exit(7702);
}

void error::throwParticleBoundsError(type3<double>* pos, int name) {
	chatterBox.startErrorLog(7703, "Particle out of bounds.");
	chatterBox.logErrorMessage("Particle: " + tos(name) + " : " + tos(pos->x) + "," + tos(pos->y) + "," + tos(pos->z));
	chatterBox.endErrorLog();
	exit(7703);
}

void error::throwParticleOverlapError(int nameI, int nameJ, double r) {
	chatterBox.startErrorLog(7704, "Significant particle overlap. Consider time-step reduction");
	chatterBox.logErrorMessage("First offender: " + tos(nameI));
	chatterBox.logErrorMessage("Second offender: " + tos(nameJ));
	chatterBox.logErrorMessage("Distance: " + tos(r));
	chatterBox.endErrorLog();
	exit(7704);
}

void error::throwInfiniteForce() {
	chatterBox.startErrorLog(7705, "Bad news bears; Numerically unstable system");
	chatterBox.logErrorMessage("Attempt reduction of concentration or decreased time step.");
	chatterBox.endErrorLog();
	exit(7705);
}

void error::throwInputError() {
	chatterBox.startErrorLog(7706, "Invalid input file");
	chatterBox.endErrorLog();
	exit(7706);
}

}

