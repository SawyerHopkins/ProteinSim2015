CC := g++ # This is the main compiler
ARC := ar

SRCDIR := src
BUILDDIR := build
TARGET := bin/Calibration.so
 
SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -g -O3 -fopenmp -std=c++11 -Wall -fPIC
LIB := -fopenmp -shared -fPIC -L./linked -lcore
INC := -I include

$(TARGET): $(OBJECTS)
	@echo
	@tput setaf 3
	@echo " Linking..."
	@tput setaf 7
	@echo
	@echo " $(CC) $^ -o $(TARGET) $(LIB)"; $(CC) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@tput setaf 3
	@echo " Cleaning...";
	@tput setaf 7 
	@echo " $(RM) -r $(BUILDDIR) $(TARGET) linked/libcore.a"; $(RM) -r $(BUILDDIR) $(TARGET) linked/libcore.a

.PHONY: clean
