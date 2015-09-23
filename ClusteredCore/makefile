SRCDIR := src
################################################################################
CUDA_PATH ?= "/usr/local/cuda-7.5"
GPUINC := -I /usr/local/cuda-7.5/include/
HOST_COMPILER ?= g++
NVCC := $(CUDA_PATH)/bin/nvcc -ccbin $(HOST_COMPILER)

GPUEXT := cu
GPUBUILDDIR := gpu
GPUSOURCES := $(shell find $(SRCDIR) -type f -name *.$(GPUEXT))
GPUOBJECTS := $(patsubst $(SRCDIR)/%,$(GPUBUILDDIR)/%,$(GPUSOURCES:.$(GPUEXT)=.o))
GPUTARGET := gpu/kernal.o
GPULINKED := bin/libkernal.so
################################################################################
CC := g++
CFLAGS := -std=c++11
GFLAGS := -m64 -arch=sm_50 --std=c++11 --compiler-options '-fPIC'
INC := -I include

CPUEXT := cpp
CPUBUILDDIR := cpu
CPUSOURCES := $(shell find $(SRCDIR) -type f -name *.$(CPUEXT))
CPUOBJECTS := $(patsubst $(SRCDIR)/%,$(CPUBUILDDIR)/%,$(CPUSOURCES:.$(CPUEXT)=.o))
CPUTARGET := bin/libcore.a
################################################################################


all: gpu cpu

# gpu rules

gpu: $(GPUOBJECTS)
	$(EXEC) $(NVCC) $(GFLAGS) -dlink $(GPUOBJECTS) -o $(GPUTARGET)
	g++ -shared -o $(GPULINKED) $(GPUOBJECTS) $(GPUTARGET) -L/usr/local/cuda/lib64 -lcudart


$(GPUBUILDDIR)/%.o: $(SRCDIR)/%.$(GPUEXT)
	@mkdir -p $(GPUBUILDDIR)
	$(EXEC) $(NVCC) $(GFLAGS) $(INC) -o $@ -dc $<

# cpu rules

cpu: $(CPUOBJECTS)
	ar -rcs $(CPUTARGET) $^

$(CPUBUILDDIR)/%.o: $(SRCDIR)/%.$(CPUEXT)
	@mkdir -p $(CPUBUILDDIR)
	$(EXEC) $(NVCC) $(GFLAGS) $(INC) -c -o $@ $<
