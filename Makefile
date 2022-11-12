CPP := g++
CPPFLAGS := -std=c++14 -Wall -g

LIB := -L./ -L/usr/lib/ -L/usr/local/lib/
INC := -I./ -I/usr/local/include/gsl

LDLIBS := -lgsl -lcblas

SRCS := $(wildcard BlockSim/*.cpp)
OBJS := $(patsubst %.cpp,%.o,$(SRCS))

STRAT_SRCS := $(wildcard StratSim/*.cpp)
STRAT_OBJS := $(patsubst %.cpp,%.o,$(STRAT_SRCS))

all: strat selfish

strat: $(STRAT_SRCS) $(OBJS)
	$(CPP) $(CPPFLAGS) $(INC) $(LDLIBS) $(LIB) -o $@ $^

selfish: SelfishSim/main.cpp $(OBJS)
	$(CPP) $(CPPFLAGS) $(INC) $(LDLIBS) -o $@ $^

%.o: %.cpp
	$(CPP) $(CPPFLAGS) $(INC) -o $@ -c $<

clean:
	rm -rf BlockSim/*.o *.o strat selfish

.PHONY: all clean

