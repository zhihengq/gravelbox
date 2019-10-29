SRCDIR ?= src
OBJDIR ?= obj
BINDIR ?= bin

CXX ?= c++
CXXFLAGS ?= -Wall -std=c++17 -fpic -I$(SRCDIR)
LDFLAGS ?= -fpie -L$(BINDIR)
ENSUREDIR ?= @mkdir -p

GRAVELBOX_OBJS ?= main trace/tracer

HEADERS := $(wildcard src/**/*.h)

ifdef RELEASE
	CXXFLAGS+= -DNDEBUG -O3
else
	CXXFLAGS+= -g -Og
endif

all: build doc test

build: $(BINDIR)/gravelbox

test:

doc:
	doxygen Doxyfile

clean:
	rm -rf $(BINDIR) $(OBJDIR) doc

.PHONY: all build test doc clean

$(BINDIR)/gravelbox: $(patsubst %,$(OBJDIR)/%.o,$(GRAVELBOX_OBJS))
	$(ENSUREDIR) $(dir $@)
	$(CXX) $(LDFLAGS) -lboost_program_options $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	$(ENSUREDIR) $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@
