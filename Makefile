SRCDIR ?= src
OBJDIR ?= obj
BINDIR ?= bin

CXX ?= c++
CC ?= cc
CXXFLAGS ?= -Wall -std=c++17 -fpic -I$(SRCDIR)
CFLAGS ?= -Wall -std=c11 -fpic -I$(SRCDIR)
LDFLAGS ?= -fpie -L$(BINDIR)
ENSUREDIR ?= @mkdir -p

SANITIZERS ?= -fsanitize=address,undefined

ifdef CXXEXTRA
CXXFLAGS += $(CXXEXTRA)
endif

ifdef LDEXTRA
LDFLAGS += $(LDEXTRA)
endif

GRAVELBOX_OBJS ?= main modules trace/tracer parser/parser parser/argtypes config/file_config ui/pinentry_ui ui/pinentry_conn
TEST_CLI_UI_OBJS ?= ui/test ui/cli_ui ui/pwd

HEADERS := $(wildcard src/*.h) $(wildcard src/**/*.h)

ifdef RELEASE
CXXFLAGS += -DNDEBUG -O3
CFLAGS += -DNDEBUG -O3
else
CXXFLAGS += -g -O0 $(SANITIZERS)
CFLAGS += -g -O0 $(SANITIZERS)
LDFLAGS += $(SANITIZERS)
endif

all: build

BUILD_LIST := gravelbox
build: $(patsubst %,$(BINDIR)/%,$(BUILD_LIST))

TARGET_LIST := print multi-threaded
targets: $(patsubst %,$(BINDIR)/%,$(TARGET_LIST))

test: $(BINDIR)/test_cli_ui
	$(BINDIR)/test_cli_ui

doc:
	doxygen Doxyfile

clean:
	rm -rf $(BINDIR) $(OBJDIR) doc

.PHONY: all build test doc clean


# Executables

$(BINDIR)/gravelbox: $(patsubst %,$(OBJDIR)/%.o,$(GRAVELBOX_OBJS))
	$(ENSUREDIR) $(dir $@)
	$(CXX) $(LDFLAGS) $^ -o $@ -lboost_program_options -lboost_iostreams -ljsoncpp -lcrypto

$(BINDIR)/test_cli_ui: $(patsubst %,$(OBJDIR)/%.o,$(TEST_CLI_UI_OBJS))
	$(ENSUREDIR) $(dir $@)
	$(CXX) $(LDFLAGS) $^ -o $@

$(BINDIR)/print: $(OBJDIR)/c/targets/print.o
	$(ENSUREDIR) $(dir $@)
	$(CC) $(LDFLAGS) $^ -o $@

$(BINDIR)/multi-threaded: $(OBJDIR)/targets/multi-threaded.o
	$(ENSUREDIR) $(dir $@)
	$(CXX) $(LDFLAGS) $^ -o $@ -lpthread


# Generic build rules for object files

$(OBJDIR)/c/%.o: $(SRCDIR)/%.c $(HEADERS)
	$(ENSUREDIR) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	$(ENSUREDIR) $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@
