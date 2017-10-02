
	# build, source, include directories
DIR_BUILD = build/
DIR_SRC   = src/
DIR_INC   = inc/

	# compiler flags
CFLAGS  = -I inc -Wall -std=c++11 -O3

	# linker flags
LDFLAGS  = -pthread -lwiringPi

	# clean extensions
FILES_CLEAN = *.o *.out

	# output file name
OUTPUT = main

	# list of source files
SRC = $(wildcard $(foreach dir,$(DIR_SRC), $(dir)*.cpp))

	# list of header files
HDR = $(wildcard $(foreach dir,$(DIR_INC), $(dir)*.hpp))

	# list of object files
OBJ = $(patsubst $(DIR_SRC)%,$(DIR_BUILD)%,$(foreach source, \
  $(basename $(SRC)),$(source).o))

	# ignore implicit chain and therefore deletion of object files
.SECONDARY: $(OBJECTS_NORMAL)

	# define phony targets for make commands
.PHONY: all all_init clean

all: all_init $(OUTPUT).out
	@echo
	@echo Compilation done - No Errors.

all_init:
	@echo
	@echo "#### compiling project \"make all\""
	@echo
	mkdir -p $(DIR_BUILD)

%.out: $(OBJ)
	@echo
	@mkdir -p $(@D)
	@echo "#### linking project"
	g++ $(CFLAGS) $(LDFLAGS) $(OBJ) -o $@

$(DIR_BUILD)%.o: $(DIR_SRC)%.cpp $(HDR)
	@echo
	@mkdir -p $(@D)
	@echo "#### compiling \"$@\" based on \"$^\""
	g++ -o $@ -c $(CFLAGS) -I "$(dir $*)/" $<

clean:
	@echo
	@echo "####" cleaning project \"make clean\"
	-rm -f -R $(foreach file,$(FILES_CLEAN),$(DIR_BUILD)$(file))