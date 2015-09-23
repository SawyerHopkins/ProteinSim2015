CUDA_PATH ?= "/usr/local/cuda-7.5"
GPUINC := -I /usr/local/cuda-7.5/include/
HOST_COMPILER ?= g++
NVCC := $(CUDA_PATH)/bin/nvcc -ccbin $(HOST_COMPILER)

CC := g++ 

SRCDIR := src
BUILDDIR := build
TARGET := bin/LJ.so
 
SRCEXT := cu
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
TEST := $(shell find $(BUILDDIR) -type f -name *.o)
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -g -O3 -std=c++11 -fPIC
GFLAGS := -m64 -arch=sm_50 --std=c++11 --compiler-options '-fPIC'
LIB := -dlink -L./linked -lcore -lkernal
LIBSO := -shared -L./linked -lcore -lkernal -lcudart
INC := -I include
EXTRA := linked/libkernal.so
GPUTARGET := build/gpu.o

$(TARGET): $(OBJECTS)
	@echo
	@tput setaf 3
	@echo " Linking..."
	@tput setaf 7
	@echo
	$(EXEC) $(NVCC) -m64 -arch=sm_50 --std=c++11 --compiler-options '-fPIC -O3' -o $(GPUTARGET) $(TEST) $(LIB) 
	$(EXEC) $(NVCC) -m64 -arch=sm_50 --std=c++11 --compiler-options '-fPIC -O3' -o $(TARGET) $(TEST) $(LIBSO) 

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	$(EXEC) $(NVCC) $(GFLAGS) $(INC) -o $@ -dc $<


