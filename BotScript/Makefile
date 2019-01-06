
	# build, source, include directories, precompiled header
DIR_BUILD = build/
DIR_SRC   = src/
DIR_INC   = inc/
PCH       = headers.hpp

	# check for pi bot and debug mode
ISBOT = $(shell [ -e isbot ] || echo true  > isbot; cat isbot)
ISDBG = $(shell [ -e isdbg ] || echo false > isdbg; cat isdbg)

	# compiler flags
CFLAGS  = -I. -I$(DIR_INC) -Wall -std=c++11 -fPIE -D ISBOT=$(ISBOT)

	# linker flags
LDFLAGS  = -pthread -pie

	# global compile dependencies
GLOBDEP = isbot isdbg Makefile

	# link Python and WiringPi
ifeq ($(ISBOT),true)
	CFLAGS += $(shell python3.4-config --includes)
	LDFLAGS += $(shell python3.4-config --libs) -lwiringPi
endif

	# debug tools
ifeq ($(ISDBG),true)
	CFLAGS += -g # -Invwa
	LDFLAGS += # nvwa/debug_new.cpp
else
	CFLAGS += -O1
endif

	# clean extensions
FILES_CLEAN = $(DIR_BUILD) $(DIR_INC)*.gch $(OUTPUT) */*.pyc */*/*.pyc

	# output file name
OUTPUT = BotScript

	# list of source files
SRC = $(wildcard $(DIR_SRC)*.cpp)

	# list of header files
HDR = $(wildcard $(DIR_INC)*.hpp)

	# list of object files
OBJ = $(patsubst $(DIR_SRC)%,$(DIR_BUILD)%,$(foreach source, $(basename $(SRC)),$(source).o))

	# ignore implicit chain and therefore deletion of object files
.SECONDARY: $(OBJECTS_NORMAL)

	# define phony targets for make commands
.PHONY: all run clean all_init program

all: all_init program
	@printf "\nCompilation done - No Errors.\n"

all_init:
	@echo "pi: $(ISPI) bot: $(ISBOT) dbg:$(ISDBG)"
	@printf "\n#### initialize project\n"
	mkdir -p $(DIR_BUILD)

program: $(DIR_INC)$(PCH).gch $(OUTPUT)
	@printf "\n#### compiling project\n"

$(OUTPUT): $(OBJ) $(GLOBDEP)
	@printf "\n#### linking project\n"
	g++ $(OBJ) -o $@ $(LDFLAGS)

$(DIR_INC)%.gch: $(DIR_INC)$(PCH)
	@echo "\n#### compiling precompiled header"
	g++ -o $@ $< $(CFLAGS)

$(DIR_BUILD)%.o: $(DIR_SRC)%.cpp $(HDR) $(GLOBDEP)
	@printf "\n#### compiling \"$@\"\n"
	g++ -o $@ -c $(CFLAGS) $<

run: all
	@printf "\n#### executing $(OUTPUT)\n"
	./$(OUTPUT) $(ARGS)

clean:
	@printf "\n#### cleaning project\n"
	rm -rf $(FILES_CLEAN)
