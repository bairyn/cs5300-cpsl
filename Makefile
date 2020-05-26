# Default target: build all.
.PHONY: default
default: all

# Get debugging settings.
DEBUG ?= 0
# Chain of conditionals: c.f. https://stackoverflow.com/a/11659542
# Override opt_debug to an empty string if debugging is disabled and to a
# non-empty string if it is enabled.
ifeq      ($(DEBUG),)
	override opt_debug =
else ifeq ($(DEBUG),0)
	override opt_debug =
else
	override opt_debug = 1
endif

# Configurable settings.  (OPT_* can contain optimization and debugging flags.)

PREFIX ?= /usr
BINDIR ?= $(PREFIX)/bin

CC ?= gcc
CXX ?= g++
LD ?= ld
LINKER ?= $(if $(opt_debug),$(DEBUG_LINKER),$(RELEASE_LINKER))
FLEX ?= flex
# To simplify linking with profiling, use g++ with -pg rather than ld with gcrt1.o and -lc_p.
DEBUG_LINKER ?= g++
# Using ld directly to link seems to require additional configuration to not
# generate tons of errors.
#RELEASE_LINKER ?= $(LD)
RELEASE_LINKER ?= g++

ALL_CFLAGS      ?= $(BASE_CFLAGS)      $(CFLAGS)      $(WARN_CFLAGS)      $(OPT_CFLAGS)      $(EXTRA_CFLAGS)
ALL_CXXFLAGS    ?= $(BASE_CXXFLAGS)    $(CXXFLAGS)    $(WARN_CXXFLAGS)    $(OPT_CXXFLAGS)    $(EXTRA_CXXFLAGS)
ALL_LINKERFLAGS ?= $(BASE_LINKERFLAGS) $(LINKERFLAGS) $(WARN_LINKERFLAGS) $(OPT_LINKERFLAGS) $(EXTRA_LINKERFLAGS)
ALL_FLEXFLAGS   ?= $(BASE_FLEXFLAGS)   $(FLEXFLAGS)   $(WARN_FLEXFLAGS)   $(OPT_FLEXFLAGS)   $(EXTRA_FLEXFLAGS)

CFLAGS ?= -x c -std=c99
BASE_CFLAGS ?= -Isrc
WARN_CFLAGS ?= -Wall
OPT_CFLAGS ?= $(if $(opt_debug),$(DEBUG_CFLAGS),$(RELEASE_CFLAGS))
DEBUG_CFLAGS ?= -g -pg -DDEBUG=1
RELEASE_CFLAGS ?= -O2 -DRELEASE=1
EXTRA_CFLAGS ?=

CXXFLAGS ?= -x c++ -std=c++17
BASE_CXXFLAGS ?= -Isrc
WARN_CXXFLAGS ?=
OPT_CXXFLAGS ?= $(if $(opt_debug),$(DEBUG_CXXFLAGS),$(RELEASE_CXXFLAGS))
DEBUG_CXXFLAGS ?= -g -pg -DDEBUG=1
RELEASE_CXXFLAGS ?= -O2 -DRELEASE=1
EXTRA_CXXFLAGS ?=

LDFLAGS ?=
BASE_LDFLAGS ?=
WARN_LDFLAGS ?=
OPT_LDFLAGS ?= $(if $(opt_debug),$(DEBUG_LDFLAGS),$(RELEASE_LDFLAGS))
# If using ld directly, this resource may be useful:
# https://ftp.gnu.org/old-gnu/Manuals/gprof-2.9.1/html_chapter/gprof_2.html
DEBUG_LDFLAGS ?= -pg
RELEASE_LDFLAGS ?=
EXTRA_LDFLAGS ?=

LINKERFLAGS ?= $(LDFLAGS)
BASE_LINKERFLAGS ?= $(BASE_LDFLAGS)
WARN_LINKERFLAGS ?= $(WARN_LDFLAGS)
OPT_LINKERFLAGS ?= $(OPT_LDFLAGS)
DEBUG_LINKERFLAGS ?= $(DEBUG_LDFLAGS)
RELEASE_LINKERFLAGS ?= $(RELEASE_LDFLAGS)
EXTRA_LINKERFLAGS ?= $(DEBUG_LDFLAGS)

FLEXFLAGS ?=
BASE_FLEXFLAGS ?=
WARN_FLEXFLAGS ?=
OPT_FLEXFLAGS ?= $(if $(opt_debug),$(DEBUG_FLEXFLAGS),$(RELEASE_FLEXFLAGS))
DEBUG_FLEXFLAGS ?=
RELEASE_FLEXFLAGS ?=
EXTRA_FLEXFLAGS ?=

# Constants and variables.

# Compiler executable file name.
EXEC = cpsl-cc

SRC_DIR = src

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
	install -m 0775 -- "$(BUILD_DIR)/$(EXEC)" "$(BUILD_DIR)/dist$(BINDIR)/$(EXEC)"

# compile: build the compiler executable, cpsl-cc, in _build/cpsl-cc.
.PHONY: compile
compile: $(BUILD_DIR)/$(EXEC)

OBJS = $(C_OBJS) $(CXX_OBJS) $(C_GEN_OBJS) $(CXX_GEN_OBJS)

C_OBJS = \
	#$(BUILD_DIR)/foo.o \
	#

C_GEN_OBJS = \
	#$(BUILD_DIR)/foo.o \
	#

CXX_OBJS = \
	$(BUILD_DIR)/cli.o \
	$(BUILD_DIR)/lexer.o \
	$(BUILD_DIR)/main.o \
	#

CXX_GEN_OBJS = \
	$(BUILD_DIR)/scanner.yy.o \
	#

$(BUILD_DIR)/$(EXEC): $(OBJS) $(BUILD_DIR)
	$(LINKER) $(ALL_LINKERFLAGS) -o "$@" $(OBJS)

$(BUILD_DIR)/scanner.yy.cc: $(SRC_DIR)/scanner.flex $(BUILD_DIR)
	$(FLEX) $(FLEXFLAGS) $(EXTRA_FLEXFLAGS) -o "$@" "$<"

# https://stackoverflow.com/a/16263002
$(C_OBJS): $(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(BUILD_DIR)
	$(CC) $(ALL_CFLAGS) -o "$@" -c "$<"

$(CXX_OBJS): $(BUILD_DIR)/%.o: $(SRC_DIR)/%.cc $(BUILD_DIR)
	$(CXX) $(ALL_CXXFLAGS) -o "$@" -c "$<"

$(C_GEN_OBJS): $(BUILD_DIR)/%.o: $(BUILD_DIR)/%.c $(BUILD_DIR)
	$(CC) $(ALL_CFLAGS) -o "$@" -c "$<"

$(CXX_GEN_OBJS): $(BUILD_DIR)/%.o: $(BUILD_DIR)/%.cc $(BUILD_DIR)
	$(CXX) $(ALL_CXXFLAGS) -o "$@" -c "$<"
