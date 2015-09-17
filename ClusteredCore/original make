CC := g++ # This is the main compiler
ARC := ar

SRCDIR := src
BUILDDIR := build
TARGET := bin/libcore.a
 
SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -g -O3 -fopenmp -std=c++11 -Wall -fPIC
LIB := -fopenmp
INC := -I include

$(TARGET): $(OBJECTS)
	@echo
	@tput setaf 3
	@echo " Creating static library..."
	@tput setaf 7
	@echo
	@echo " $(ARC) -rcs $(TARGET) $^"; $(ARC) -rcs $(TARGET) $^
	@echo

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@tput setaf 3
	@echo " Cleaning..."; 
	@tput setaf 7
	@echo " $(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)

.PHONY: clean
