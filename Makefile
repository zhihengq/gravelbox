SRCDIR ?= src
OBJDIR ?= obj
BINDIR ?= bin

CXX ?= c++
CXXFLAGS ?= -Wall -std=c++17 -fpic -I$(SRCDIR)
LDFLAGS ?= -fpie -L$(BINDIR)
ENSUREDIR ?= @mkdir -p

GRAVELBOX_OBJS ?= main trace/tracer parser/parser parser/argtypes ui/pinentry_ui
TEST_CLI_UI_OBJS ?= ui/test ui/cli_ui ui/pwd

HEADERS := $(wildcard src/*.h) $(wildcard src/**/*.h)

ifdef RELEASE
	CXXFLAGS+= -DNDEBUG -O3
else
	CXXFLAGS+= -g -Og -fsanitize=address,undefined
	LDFLAGS+= -fsanitize=address,undefined
endif

all: build

build: $(BINDIR)/gravelbox $(BINDIR)/test_cli_ui

test: $(BINDIR)/test_cli_ui
	$(BINDIR)/test_cli_ui

doc:
	doxygen Doxyfile

clean:
	rm -rf $(BINDIR) $(OBJDIR) doc

.PHONY: all build test doc clean

$(BINDIR)/gravelbox: $(patsubst %,$(OBJDIR)/%.o,$(GRAVELBOX_OBJS))
	$(ENSUREDIR) $(dir $@)
	$(CXX) $(LDFLAGS) -lboost_program_options -lboost_iostreams -ljsoncpp $^ -o $@

$(BINDIR)/test_cli_ui: $(patsubst %,$(OBJDIR)/%.o,$(TEST_CLI_UI_OBJS))
	$(ENSUREDIR) $(dir $@)
	$(CXX) $(LDFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	$(ENSUREDIR) $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@
