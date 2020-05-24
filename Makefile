# Default target: build all.
.PHONY: default
default: all

# Configurable settings.
CFLAGS ?= -Wall -x c -std=c99
EXTRA_CFLAGS ?= -O2
CXXFLAGS ?= -Wall -x c++ -std=c++17
EXTRA_CXXFLAGS ?= -O2
LDFLAGS ?=
EXTRA_LDFLAGS ?=
FLEXFLAGS ?=
EXTRA_FLEXFLAGS ?=
CC ?= gcc
CXX ?= g++
FLEX ?= flex
LD ?= ld

PREFIX ?= /usr
BINDIR ?= $(PREFIX)/bin

# Constants.
# Compiler executable file name.
EXEC = cpsl-cc

# all: compile and dist.
.PHONY: all
all: compile dist

# clean
.PHONY: clean
clean:
	rm -rf -- "$(BUILD_DIR)"

# Build directories.
BUILD_DIR = _build
BUILD_DIRECTORIES = $(BUILD_DIR) $(BUILD_DIR)/dist $(BUILD_DIR)/dist$(BINDIR)
$(BUILD_DIR):
$(BUILD_DIR)/dist: $(BUILD_DIR)
$(BUILD_DIR)/dist$(BINDIR): $(BUILD_DIR) $(BUILD_DIR)/dist
$(BUILD_DIRECTORIES):
	install -d -m 0775 -- "$@"

# dist: Create dist filesystem structure.
.PHONY: dist
dist: compile $(BUILD_DIR)/dist $(BUILD_DIR)/dist$(BINDIR)
	install -m 0664 -- "$(BUILD_DIR)/$(EXEC)" "$(BUILD_DIR)/dist$(BINDIR)/$(EXEC)"

# compile: build the compiler executable, cpsl-cc, in _build/cpsl-cc.
.PHONY: compile
compile: $(BUILD_DIR)/$(EXEC)

$(BUILD_DIR)/$(EXEC): $(OBJS) $(BUILD_DIR)
	$(LD) $(LDFLAGS) $(LDFLAGS_EXTRA) -o "$@" -- $(OBJS)

OBJS = $(C_OBJS) $(CXX_OBJS)

C_OBJS = \
	#$(BUILD_DIR)/foo.o \
	#

CXX_OBJS = \
	$(BUILD_DIR)/scanner.yy.o \
	$(BUILD_DIR)/lexer.o \
	$(BUILD_DIR)/main.o \
	#

$(BUILD_DIR)/scanner.yy.o: src/scanner.flex
	$(FLEX) $(FLEXFLAGS) $(EXTRA_FLEXFLAGS) -o "$@" "$<"

$(C_OBJS):
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -o "$@" -c "$(shell basename -- "$@")"

$(CXX_OBJS):
	$(CXX) $(CXXFLAGS) $(EXTRA_CXXFLAGS) -o "$@" -c "$(shell basename -- "$@")"
