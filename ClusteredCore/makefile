SRCDIR := src
LINKDIR := linked
LINKOBJECTS := $(shell find $(LINKDIR) -type f -name *.o)
################################################################################
CUDA_PATH ?= "/usr/local/cuda-7.5"
GPUINC := -I /usr/local/cuda-7.5/include/
HOST_COMPILER ?= g++
NVCC          := $(CUDA_PATH)/bin/nvcc -ccbin $(HOST_COMPILER)

GPUEXT := cu
GPUBUILDDIR := gpu
GPUSOURCES := $(shell find $(SRCDIR) -type f -name *.$(GPUEXT))
GPUOBJECTS := $(patsubst $(SRCDIR)/%,$(GPUBUILDDIR)/%,$(GPUSOURCES:.$(GPUEXT)=.o))
GPUTARGET := gpu/kernal.o
GPULINKED := linked/kernal.so
################################################################################
CC := g++
CFLAGS := -std=c++11
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
	$(EXEC) $(NVCC) -arch=sm_50 --compiler-options '-fPIC' -dlink $(GPUOBJECTS) -o $(GPUTARGET)
	g++ -shared -o $(GPULINKED) $(GPUOBJECTS) $(GPUTARGET) -L/usr/local/cuda-7.5/lib64 -lcudart

$(GPUBUILDDIR)/%.o: $(SRCDIR)/%.$(GPUEXT)
	@mkdir -p $(GPUBUILDDIR)
	$(EXEC) $(NVCC) --std=c++11 --compiler-options '-fPIC' -dc $(INC) $(INCLUDES) -m64 -arch=sm_50 -o $@ -c $<

# cpu rules

cpu: $(CPUOBJECTS)
	ar -rcs $(CPUTARGET) $^ $(GPUTARGET)	


$(CPUBUILDDIR)/%.o: $(SRCDIR)/%.$(CPUEXT)
	@mkdir -p $(CPUBUILDDIR)
	$(EXEC) $(NVCC) --std=c++11 --compiler-options '-fPIC -O3' --shared $(INC) $(INCLUDES) -m64 -arch=sm_50 -o $@ -c $<
