SRCDIR ?= src
OBJDIR ?= obj
BINDIR ?= bin

CXX ?= c++
CXXFLAGS ?= -Wall -std=c++17 -fpic -I$(SRCDIR)
LDFLAGS ?= -fpie -L$(BINDIR)
ENSUREDIR ?= @mkdir -p

ifdef RELEASE
	CXXFLAGS+= -DNDEBUG -O3
else
	CXXFLAGS+= -g -Og
endif

all: build doc test

doc:
	doxygen Doxyfile

clean:
	rm -rf $(BINDIR) $(OBJDIR) doc

.PHONY: all doc clean

include $(SRCDIR)/Makefrag
