################################################################################
################################GENERAL PATH INFO###############################
################################################################################
SRCDIR := src
INC := -I include
################################################################################
##############################CUDA PATH AND OPTIONS#############################
################################################################################
CUDA_PATH ?= "/usr/local/cuda-7.5"
GPUINC := -I /usr/local/cuda-7.5/include/
HOST_COMPILER ?= g++
NVCC := $(CUDA_PATH)/bin/nvcc -ccbin $(HOST_COMPILER)
GFLAGS := -m64 -arch=sm_50 --std=c++11 --compiler-options '-fPIC'
################################################################################
################################DEVICE CODE PATHS###############################
################################################################################
GPUEXT := cu
GPUBUILDDIR := gpu
GPUSOURCES := $(shell find $(SRCDIR) -type f -name *.$(GPUEXT))
GPUOBJECTS := $(patsubst $(SRCDIR)/%,$(GPUBUILDDIR)/%,$(GPUSOURCES:.$(GPUEXT)=.o))
GPULINKED := gpu/kernal.o
GPUTARGET := bin/libkernal.a
################################################################################
#################################HOST CODE PATHS################################
################################################################################
CPUEXT := cpp
CPUBUILDDIR := cpu
CPUSOURCES := $(shell find $(SRCDIR) -type f -name *.$(CPUEXT))
CPUOBJECTS := $(patsubst $(SRCDIR)/%,$(CPUBUILDDIR)/%,$(CPUSOURCES:.$(CPUEXT)=.o))
CPUTARGET := bin/libcore.a
################################################################################
######################################RULES#####################################
################################################################################

# default

all: gpu cpu

# gpu rules:

gpu: $(GPUOBJECTS)
	$(EXEC) $(NVCC) $(GFLAGS) -lib $(GPUOBJECTS) -o $(GPUTARGET)


$(GPUBUILDDIR)/%.o: $(SRCDIR)/%.$(GPUEXT)
	$(EXEC) $(NVCC) $(GFLAGS) $(INC) -o $@ -dc $<

# cpu rules:

cpu: $(CPUOBJECTS)
	ar -rcs $(CPUTARGET) $^
	ranlib $(CPUTARGET)

$(CPUBUILDDIR)/%.o: $(SRCDIR)/%.$(CPUEXT)
	$(EXEC) $(NVCC) $(GFLAGS) $(INC) -c -o $@ $<

#clean rules:

clean: 
	$(RM) -rf bin

cleancpu: 
	$(RM) -rf $(CPUBUILDIR)

cleangpu: 
	$(RM) -rf $(GPUBUILDDIR)

spotless: clean cleancpu cleangpu
