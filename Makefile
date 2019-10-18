SRCDIR ?= src
OBJDIR ?= obj
BINDIR ?= bin

CXX ?= c++
CXXFLAGS ?= -Wall -std=c++17 -fpic -I$(SRCDIR) 
LDFLAGS ?= -L$(BINDIR)
ENSUREDIR ?= @mkdir -p

all: build test

clean:
	rm -rf $(BINDIR) $(OBJDIR)

.PHONY: all clean

include $(SRCDIR)/Makefrag
