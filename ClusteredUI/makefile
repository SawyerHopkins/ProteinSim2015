CUDA_PATH ?= "/usr/local/cuda-7.5"
GPUINC := -I /usr/local/cuda-7.5/include/
HOST_COMPILER ?= g++
NVCC := $(CUDA_PATH)/bin/nvcc -ccbin $(HOST_COMPILER)

CC := g++ # This is the main compiler
SRCDIR := src
BUILDDIR := build
TARGET := bin/Clustered
 
SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -O3 -std=c++11
LIB := -ldl -lc -L./linked -lcore -lkernal -lcudart
KERNAL := linked/kernal.a
INC := -I include

$(TARGET): $(OBJECTS)
	@echo
	@tput setaf 3
	@echo " Linking..."
	@tput setaf 7
	@echo
	$(EXEC) $(NVCC) -m64 -arch=sm_50 --std=c++11 --compiler-options '-fPIC -O3' -o $(TARGET) $^ $(LIB) 

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	$(EXEC) $(NVCC) --std=c++11 --compiler-options '-fPIC -O3' $(INC) -m64 -arch=sm_50 -o $@ -c $<
