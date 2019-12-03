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
CXXFLAGS += -DNDEBUG -O3 -flto
CFLAGS += -DNDEBUG -O3 -flto
LDFLAGS += -O3 -flto
else
CXXFLAGS += -g -O0 $(SANITIZERS)
CFLAGS += -g -O0 $(SANITIZERS)
LDFLAGS += -g -O0 $(SANITIZERS)
endif

all: build

BUILD_LIST := gravelbox
build: $(patsubst %,$(BINDIR)/%,$(BUILD_LIST))

TARGET_LIST := print print32 multi-threaded multi-threaded32 int80 segfault segfault32
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

$(BINDIR)/print: $(OBJDIR)/targets/print.o
	$(ENSUREDIR) $(dir $@)
	$(CC) $(LDFLAGS) $^ -o $@

$(BINDIR)/print32: $(OBJDIR)/m32/targets/print.o
	$(ENSUREDIR) $(dir $@)
	$(CC) $(LDFLAGS) -m32 $^ -o $@

$(BINDIR)/multi-threaded: $(OBJDIR)/targets/multi-threaded.o
	$(ENSUREDIR) $(dir $@)
	$(CXX) $(LDFLAGS) $^ -o $@ -lpthread

$(BINDIR)/multi-threaded32: $(OBJDIR)/m32/targets/multi-threaded.o
	$(ENSUREDIR) $(dir $@)
	$(CXX) $(LDFLAGS) -m32 $^ -o $@ -lpthread

$(BINDIR)/int80: $(OBJDIR)/targets/int80.o
	$(ENSUREDIR) $(dir $@)
	$(CC) -no-pie $^ -o $@

$(BINDIR)/segfault: $(SRCDIR)/targets/segfault.c
	$(ENSUREDIR) $(dir $@)
	$(CC) -std=c11 -O0 $^ -o $@

$(BINDIR)/segfault32: $(SRCDIR)/targets/segfault.c
	$(ENSUREDIR) $(dir $@)
	$(CC) -std=c11 -O0 -m32 $^ -o $@


# Generic build rules for object files

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(HEADERS)
	$(ENSUREDIR) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	$(ENSUREDIR) $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.S
	$(ENSUREDIR) $(dir $@)
	$(CC) -c $< -o $@

$(OBJDIR)/m32/%.o: $(SRCDIR)/%.c $(HEADERS)
	$(ENSUREDIR) $(dir $@)
	$(CC) $(CFLAGS) -m32 -c $< -o $@

$(OBJDIR)/m32/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	$(ENSUREDIR) $(dir $@)
	$(CXX) $(CXXFLAGS) -m32 -c $< -o $@
