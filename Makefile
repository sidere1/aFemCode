# preparing different actions depending of the OS 
UNAME := $(shell uname)


SOURCES := $(wildcard *.cpp)
OBJECTS := $(patsubst %.cpp,%.o,$(SOURCES))
DEPENDS := $(patsubst %.cpp,%.d,$(SOURCES))

ifeq ($(UNAME), Darwin)
CXXFLAGS := -std=c++11 -g -O3 -I/opt//homebrew/Cellar/eigen/3.4.0_1/lib/
endif
ifeq ($(UNAME), Linux)
CXXFLAGS := -std=c++11 -g -O3 -fopenmp
endif

# ADD MORE WARNINGS!
WARNING := -Wall -Wextra -pedantic -Wtautological-overlap-compare

# .PHONY means these rules get executed even if
# files of those names exist.
.PHONY: all clean


# The first rule is the default, ie. "make",
# "make all" and "make parking" mean the same
all: prog
ifeq ($(UNAME), Darwin)
	/usr/bin/time -lp ./prog ~/Documents/femCases/stupidCircle/setup/main	
endif
ifeq ($(UNAME), Linux)
	/usr/bin/time -v ./prog ~/Documents/femCases/stupidCircle/setup/main	
endif
clean:
	$(RM) $(OBJECTS) $(DEPENDS) prog

# Linking the executable from the object files
prog: $(OBJECTS)
	$(CXX) $(WARNING) $(CXXFLAGS) $^ -o $@

-include $(DEPENDS)

%.o: %.cpp Makefile
	$(CXX) $(WARNING) $(CXXFLAGS) -MMD -MP -c $< -o $@
