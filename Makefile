SRCDIR ?= src
OBJDIR ?= obj
BINDIR ?= bin

CXX ?= c++
CXXFLAGS ?= -Wall -std=c++17 -fpic -I$(SRCDIR)
LDFLAGS ?= -fpie -L$(BINDIR)
ENSUREDIR ?= @mkdir -p

ifdef DEBUG
	CXXFLAGS+= -g -Og
else
	CXXFLAGS+= -DNDEBUG -O3
endif

all: build test

clean:
	rm -rf $(BINDIR) $(OBJDIR)

.PHONY: all clean

include $(SRCDIR)/Makefrag
