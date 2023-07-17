# preparing different actions depending of the OS 
UNAME := $(shell uname)


SOURCES := $(wildcard *.cpp)
OBJECTS := $(patsubst %.cpp,%.o,$(SOURCES))
DEPENDS := $(patsubst %.cpp,%.d,$(SOURCES))

ifeq ($(UNAME), Darwin)
CXXFLAGS := -std=c++14 -g -O3
LIBS = -I/opt/homebrew/Cellar/hdf5/1.14.1/include/ -I/opt/homebrew/Cellar/eigen/3.4.0_1/lib/ -I/opt/homebrew/Cellar/hdf5/1.14.1/include/hdf5/serial
endif
ifeq ($(UNAME), Linux)
CXXFLAGS := -std=c++11 -g -O3 -fopenmp
endif

# ADD MORE WARNINGS!
WARNING := -Wall -Wextra -pedantic

# .PHONY means these rules get executed even if
# files of those names exist.
.PHONY: all clean


# The first rule is the default, ie. "make",
# "make all" and "make parking" mean the same
all: prog
ifeq ($(UNAME), Darwin)
#	/usr/bin/time -lp ./prog ~/Documents/femCases/stupidCircle/setup/main	
	time ./prog ~/Documents/femCases/stupidCircle/setup/main	
endif
ifeq ($(UNAME), Linux)
	/usr/bin/time -v ./prog ~/Documents/femCases/stupidCircle/setup/main	
endif

test:prog_test
ifeq ($(UNAME), Darwin)
	/usr/bin/time -lp tests/prog_test 10
endif
ifeq ($(UNAME), Linux)
	/usr/bin/time -v tests/prog_test 10
endif

clean:
	$(RM) $(OBJECTS) $(DEPENDS) prog prog_test

# Linking the executable from the object files
prog: $(OBJECTS)
	$(CXX) $(WARNING) $(CXXFLAGS) $^ -o $@

prog_test: $(OBJECTS) tests/main.o
	$(CXX) $(WARNING) $(CXXFLAGS) $(LIBS) Element.o FemCase.o Mesh.o Node.o PointLoad.o Setup.o fLinSys.o fMatrix.o tests/main.o  -o tests/prog_test

-include $(DEPENDS)

%.o: %.cpp Makefile
	$(CXX) $(WARNING) $(CXXFLAGS) $(LIBS) -MMD -MP -c $< -o $@
