################################################################################
################################GENERAL PATH INFO###############################
################################################################################
SRCDIR := src
BUILDDIR := build
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
SRCEXT := cu
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
LIB := -L./linked -lcore -lkernal
TARGET := bin/LJ.so
################################################################################
######################################RULES#####################################
################################################################################

# default

$(TARGET): $(OBJECTS)
	@echo
	@tput setaf 3
	@echo " Linking..."
	@tput setaf 7
	@echo
	$(EXEC) $(NVCC) $(GFLAGS) -shared $^ $(LIB) -o $(TARGET) -lcuda -lcudart

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	$(EXEC) $(NVCC) $(GFLAGS) $(INC) -o $@ -dc $<

#clean rules:

clean: 
	$(RM) -rf bin linked/libcore.a linked/libkernal.a

cleangpu: 
	$(RM) -rf $(BUILDDIR)

spotless: clean cleangpu
