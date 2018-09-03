
	# build, source, include directories, precompiled header
DIR_BUILD = build/
DIR_SRC   = src/
DIR_INC   = inc/
PCH       = headers.hpp

	# check for pi and bot
ISPI  = $(shell [ -e ispi  ] || echo false > ispi;  cat ispi)
ISBOT = $(shell [ -e isbot ] || echo false > isbot; cat isbot)
ISDBG = $(shell [ -e isdbg ] || echo false > isdbg; cat isdbg)

	# compiler flags
CFLAGS  = -I. -I$(DIR_INC) -Wall -std=c++11 -fPIE -D ISPI=$(ISPI) -D ISBOT=$(ISBOT) # -O3

	# linker flags
LDFLAGS  = -pthread -pie

GLOBDEP = ispi isbot isdbg

ifeq ($(ISPI),true)
	LDFLAGS += -lwiringPi
endif

ifeq ($(ISBOT),true)
	CFLAGS += $(shell python2.7-config --includes)
	LDFLAGS += $(shell python2.7-config --libs)
endif

ifeq ($(ISDBG),true)
	CFLAGS += -Invwa -g
	LDFLAGS += nvwa/debug_new.cpp
endif

	# clean extensions
FILES_CLEAN = *.o

	# output file name
OUTPUT = main

	# list of source files
SRC = $(wildcard $(DIR_SRC)*.cpp)

	# list of header files
HDR = $(wildcard $(DIR_INC)*.hpp)

	# list of object files
OBJ = $(patsubst $(DIR_SRC)%,$(DIR_BUILD)%,$(foreach source, $(basename $(SRC)),$(source).o))

	# ignore implicit chain and therefore deletion of object files
.SECONDARY: $(OBJECTS_NORMAL)

	# define phony targets for make commands
.PHONY: all all_init clean run

all: all_init $(DIR_INC)$(PCH).gch $(OUTPUT).out
	@printf "\nCompilation done - No Errors.\n"

all_init:
	@echo "pi: $(ISPI) bot: $(ISBOT) dbg:$(ISDBG)"
	@printf "\nbasepth: $(foreach name, $(basename $(notdir $(SRC))), $(DIR_BUILD)$(name).o)\n"
	@printf "\n#### compiling project\n"
	mkdir -p $(DIR_BUILD)

%.out: $(OBJ) $(GLOBDEP)
	@printf "\n#### linking project\n"
	g++ $(OBJ) -o $@ $(LDFLAGS)

$(DIR_INC)%.gch: $(DIR_INC)$(PCH)
	@echo "\n#### compiling precompiled header"
	g++ -o $@ $< $(CFLAGS)

$(DIR_BUILD)%.o: $(DIR_SRC)%.cpp $(HDR) $(GLOBDEP)
	@printf "\n#### compiling \"$@\"\n"
	g++ -o $@ -c $(CFLAGS) $<

run: all
	@printf "\n#### executing $(OUTPUT).out\n"
	./$(OUTPUT).out

clean:
	@printf "\n#### cleaning project\n"
	rm -f $(DIR_BUILD)*.o $(DIR_INC)*.gch $(OUTPUT).out
