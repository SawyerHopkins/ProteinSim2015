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

namespace simulation
{

	int system::numClusters(int xyz)
	{
		/*
		//A map of all the particles in the system by name.
		std::map<int,particle*> selectionPool;

		//Add all the particles to the map.
		for (int i=0; i<nParticles; i++)
		{
			selectionPool[particles[i]->getName()] = particles[i];
		}

		//Create a vector of clusters.
		int totalSize = 0;
		std::vector<std::vector<particle*>> clusterPool;

		//While we still have particles in pList look for clusters.
		while(!selectionPool.empty())
		{
			//The cluster candidate.
			std::vector<particle*> candidate;

			//Get the base particle.
			particle* p = selectionPool.begin()->second;

			//Recreate a searching pool
			std::vector<particle*> recursionPool;
			//Add the base particle to the recursive search.
			recursionPool.push_back(p);

			//Recurse.
			while(!recursionPool.empty())
			{
				//Grab a particle to recurse through.
				particle* r = recursionPool.back();
				//Remove the particle from the recurse pool.
				recursionPool.pop_back();

				//If the particle is still in the selection pool.
				if (selectionPool.count(r->getName()))
				{

					//Remove the particle from the selection pool.
					selectionPool.erase(r->getName());
					//Add the particle to the cluster candidate
					candidate.push_back(r);

					//If the particle has interacting particles, add those to the recurse pool.
					if (!(r->getInteractions().empty()))
					{
						recursionPool.insert(recursionPool.end(), r->getInteractionsBegin(), r->getInteractionsEnd());
					}
				}
			}

			//If the candidate is larger than 4 elements, add it to the cluster pool.
			if(candidate.size() > 4)
			{
				totalSize += candidate.size();
				clusterPool.push_back(candidate);
			}

		}

		int avgSize = 0;

		if (clusterPool.size() > 0)
		{
			avgSize = totalSize / clusterPool.size();
		}

		if (xyz > 0)
		{
			//Create the file name.
			std::string outName = std::to_string(int(std::round(currentTime)));
			std::string dirName = trialName + "/snapshots/time-" + outName; 
			mkdir(dirName.c_str(),0777);

			int clustName = 0;
			for(auto clustIT = clusterPool.begin(); clustIT != clusterPool.end(); ++clustIT)
			{
				//Open the file steam.
				std::ofstream myFile;
				std::string fileName = trialName + "/snapshots/time-" + outName + "/clust" + std::to_string(clustName) + ".xyz";
				myFile.open(fileName);

				//Number line.
				myFile << clustIT->size() << "\n";
				//Comment line.
				myFile << "Cluster: " << clustName << "\n";
				//Position lines.
				for (auto partIT = clustIT->begin(); partIT != clustIT->end(); ++partIT)
				{
					myFile << "H " << (*partIT)->getX() << " " << (*partIT)->getY() << " " << (*partIT)->getZ() << "\n";
				}
				myFile.close();
				clustName++;
			}

		}

		std::cout << "\n" << "#Clusters: " << clusterPool.size() << "\n";

		return avgSize;

		//return clusterPool.size();
		*/
		return 0;
	}

	double system::getTemperature()
	{
		double vtot = 0;

		for (int i = 0; i < nParticles; i++)
		{
			//Add the totat velocity squares.
			double vx = particles[i]->getVX();
			double vy = particles[i]->getVY();
			double vz = particles[i]->getVZ();
			vtot += (vx*vx);
			vtot += (vy*vy);
			vtot += (vz*vz);
		}

		//Important physics
		vtot = vtot / 3.0;
		vtot = vtot / nParticles;
		//Average
		return vtot;
	}

}

