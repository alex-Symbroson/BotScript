
	# build, source, include directories, precompiled header
DIR_BUILD = build/
DIR_SRC   = src/
DIR_INC   = inc/
PCH       = headers.hpp

	# compiler flags
CFLAGS  = -I $(DIR_INC) -Wall -std=c++11 -O3

	# linker flags
LDFLAGS  = -pthread -lwiringPi

	# clean extensions
FILES_CLEAN = *.o

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
.PHONY: all all_init clean run

all: all_init $(DIR_INC)$(PCH).gch $(OUTPUT).out
	@printf "\nCompilation done - No Errors.\n"

all_init:
	@printf "\n#### compiling project\n"
	mkdir -p $(DIR_BUILD)

%.out: $(OBJ)
	@printf "\n#### linking project\n"
	g++ $(CFLAGS) $(LDFLAGS) $(OBJ) -o $@

$(DIR_INC)%.gch: $(DIR_INC)$(PCH)
	@echo "\n#### compiling precompiled header"
	g++ -o $@ $(CFLAGS) $(PHC) $<

$(DIR_BUILD)%.o: $(DIR_SRC)%.cpp $(HDR)
	@printf "\n#### compiling \"$@\" based on \"$^\"\n"
	g++ -o $@ -c $(CFLAGS) $<

run: all
	@printf "\n#### executing $(OUTPUT).out\n"
	./$(OUTPUT).out
clean:
	@printf "\n#### cleaning project\n"
	rm -f $(foreach file,$(FILES_CLEAN),$(DIR_BUILD)$(file) $(DIR_INC)$(PCH).gch) $(OUTPUT).out
