CXX := g++
CXXFLAGS := -Wall -Wextra -Weffc++ -pedantic -std=c++11 -g
LDLIBS := -lncurses

BINDIR := bin
SRCS := main.cpp
OBJS := $(patsubst %.cpp,$(BINDIR)/%.o,$(SRCS))

all: $(BINDIR)/main

$(BINDIR)/main: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(LDLIBS) -o $(BINDIR)/main

$(BINDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS)

.PHONY: clean
