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

#include <stack>
#include "system.h"

namespace PSim {
int system::avgClusterSize(std::vector<std::vector<particle*>> clusterPool,
		int xyz) {
	int totalSize = 0;
	int avgSize = 0;

	for (auto clustIT = clusterPool.begin(); clustIT != clusterPool.end();
			++clustIT) {
		totalSize += clustIT->size();
	}

	if (clusterPool.size() > 0) {
		avgSize = totalSize / clusterPool.size();
	}

	if (xyz > 0) {
		//Create the file name.
		std::string outName = std::to_string(int(std::round(currentTime)));
		std::string dirName = trialName + "/snapshots/time-" + outName;
		mkdir(dirName.c_str(), 0777);

		int clustName = 0;
		for (auto clustIT = clusterPool.begin(); clustIT != clusterPool.end();
				++clustIT) {
			//Open the file steam.
			std::ofstream myFile;
			std::string fileName = trialName + "/snapshots/time-" + outName
					+ "/clust" + std::to_string(clustName) + ".xyz";
			myFile.open(fileName);

			//Number line.
			myFile << clustIT->size() << "\n";
			//Comment line.
			myFile << "Cluster: " << clustName << "\n";
			//Position lines.
			for (auto partIT = clustIT->begin(); partIT != clustIT->end();
					++partIT) {
				myFile << "H " << (*partIT)->getX() << " " << (*partIT)->getY()
						<< " " << (*partIT)->getZ() << "\n";
			}
			myFile.close();
			clustName++;
		}

	}

	std::cout << "\n" << "#Clusters: " << clusterPool.size() << "\n";

	return avgSize;
}

void system::clusterSizeHistogram(
		std::vector<std::vector<particle*>> clusterPool) {
	std::map<int, int> histo;

	for (auto clustIT = clusterPool.begin(); clustIT != clusterPool.end();
			++clustIT) {
		int key = clustIT->size();
		if (histo.count(key)) {
			histo[key] = histo[key] + 1;
		} else {
			histo[key] = 1;
		}
	}

	std::ofstream myFile;
	std::string fileName = trialName + "/clusterHistogram.txt";
	myFile.open(fileName);

	for (auto it = histo.begin(); it != histo.end(); ++it) {
		myFile << it->first << " " << it->second << "\n";
	}
}

void system::coordinationHistogram() {
	std::map<int, int> histo;

	for (int index = 0; index < nParticles; index++) {
		int key = particles[index]->getCoorNumber();
		if (histo.count(key)) {
			histo[key] = histo[key] + 1;
		} else {
			histo[key] = 1;
		}
	}

	std::ofstream myFile;
	std::string fileName = trialName + "/coordinationHistogram.txt";
	myFile.open(fileName);

	for (auto it = histo.begin(); it != histo.end(); ++it) {
		myFile << it->first << " " << it->second << "\n";
	}
}
}
