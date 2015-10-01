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
#################################HOST CODE PATHS################################
################################################################################
SRCEXT := cu
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
LIB := -ldl -lc -L./linked -lcore -lkernal -lcudart -lcuda
TARGET := bin/Clustered
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
	$(EXEC) $(NVCC) $(GFLAGS) $^ $(LIB) -o $(TARGET)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	$(EXEC) $(NVCC) $(GFLAGS) $(INC) -o $@ -dc $<

#clean rules:

clean: 
	$(RM) -rf bin linked/libcore.a linked/libkernal.a

cleancpu: 
	$(RM) -rf $(BUILDDIR)

spotless: clean cleancpu
