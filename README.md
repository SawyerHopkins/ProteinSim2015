# ProteinSim2015
A langevin dynamics engine

## Publication

Experimental results and details are published in the The Journal of Chemical Physics:
https://aip.scitation.org/doi/10.1063/1.5092130

The force module for these non pairwise interactions described in this article is also publically available:
https://github.com/SawyerHopkins/NonLinearForce

## Info

PSim2015 is a particle dynamics engine being developed in the Kansas State University Physics Department. 

## Usage

### 1. Configuration File

The config file is used to specific the output path of a simulation, the parameters of the simulation, and the parameters of the input forces. See the wiki page [Settings.cfg](../../wiki/Settings.cfg)

### 2. Running Simulations

Once you have your settings finalized simply run the program, verify that everything looks correct, and let it run.

### 3. Resuming Simulations

Use the option `-r 'path-to-old-sim' XXX` to resume the snapshot in the `path-to-old-sim/snapshots/time-XXX` folder. This method rebuilds the system from the `sysConfig.cfg` in the `path-to-old-sim` folder to reconstruct the system. It uses the `settings.cfg` file in the application directory to load the forces. This allows different forces to be applied to a previously simulated state. The `timeStep` and `temp` variables in `sysConfig.cfg` are overwritten by `settings.cfg`.

### 4. Analysing systems.

Use the `-a 'path-to-old-sim' XXX [OPTIONS]` command to run the specified post analysis on a specified snapshot in the `path-to-old-sim/snapshots/time-XXX` folder.

## Custom Forces

If you need a force or system of forces that do not exist within the project, you can create and import your own.

### 1. Create a new folder for the force.

In this folder create the necessary C++ files and either copy the PSim header files to this folder, or link the `ClusteredCore/include` folder with g++/make.

### 2. Create the force class.

Create a new class which inherits/implements the `PSim::IForce` interface.
```cpp
#include "forceManager.h"
class myForce : public PSim::IForce
```
### 3. Create the force factory.

Since this is c++, we need to make a c compatible factory to load this as an external library. The factory name must be `getForce` and must take a configReader as a parameter. Use the configReader to access any custom configuration parameters in `settings.cfg` file.

```cpp
//Class factories.
extern "C" PSim::IForce* getForce(configReader::config* cfg)
{
	return new myForce(cfg);
}
```

### 4. Implement the interface contract

The `PSim::IForce` interface required on function `nextSystem` to be implemented. This function will have acess to the current time, the particle system, and the cell manager. To see how use the cell and particle systems checkout the documentation on the project website. Optionally take a look at Calibration.cpp. This is a bare bones force which only provides a soft-shell potential.
