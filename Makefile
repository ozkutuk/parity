CXX := g++
CXXFLAGS := -Wall -Werror -Wextra -pedantic -std=c++11
LDLIBS := -lncurses

BINDIR := bin
SRCS := main.cpp
OBJS := $(patsubst %.cpp,$(BINDIR)/%.o,$(SRCS))

all: $(BINDIR)/main

$(BINDIR)/main: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDLIBS) $(OBJS) -o $(BINDIR)/main

$(BINDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS)

.PHONY: clean
