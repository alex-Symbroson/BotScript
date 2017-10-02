###############################################################################
#                                                                             #
# Makefile                                                                    #
# ========                                                                    #
#                                                                             #
# Version: 1.5.0                                                              #
# Date   : 31.01.17                                                           #
# Author : Peter Weissig                                                      #
#                                                                             #
# For help or bug report please visit:                                        #
#   https://github.com/peterweissig/robolib                                   #
###############################################################################

# Target for compiling and linking (without extension)
# e.g. "main" for main.c
# If TARGET is empty ("") all source files (*.cpp) within the current folder
# will be used.
TARGET =

# Flag for debugging (extended output during processing)
# If not set ("") nothing changes.
# Otherwise additional output will be generated.
DEBUG = 1

###############################################################################
# additional identifiers
PATH_SUB_BUILD = build/
PATH_SUB_SRC   = src/
PATH_SUB_INC   = inc/

# list of file extensions for cleaning
FILES_CLEAN = *.o *.out

###############################################################################
# get current directory (for opening a new console - ..._IN_NEW_TERMINAL)
PATH_CUR = $(shell pwd)/
ifneq ($(DEBUG),)
  $(info current dir: $(PATH_CUR))
endif

###############################################################################
# output, source-files, object-files
# name of output file
ifeq (,$(TARGET))
  OUTPUT = main
else
  OUTPUT = $(TARGET)
endif
ifneq ($(DEBUG),)
  $(info output file: $(OUTPUT))
endif

# source file(s)
ifneq (,$(TARGET))
  SRC = $(TARGET).cpp
else
  # main directory for sourcefiles
  PATH_SOURCE = $(PATH_SUB_SRC)
  # search for subdirectories
  #PATH_SOURCE+= $(filter $(PATH_SUB_SRC)%/, $(wildcard $(PATH_SUB_SRC)*/))

  # list of all source files within found paths
  SRC = $(wildcard $(foreach dir,$(PATH_SOURCE), $(dir)*.cpp))
endif
ifneq ($(DEBUG),)
  $(info source files: $(SRC))
endif

# header file(s)
  # main directory for sourcefiles
  PATH_HEADER = $(PATH_SUB_INC)
  # search for subdirectories
  #PATH_HEADER+= $(filter $(PATH_SUB_INC)%/, $(wildcard $(PATH_SUB_INC)*/))

  # list of all source files within found paths
  HDR = $(wildcard $(foreach dir,$(PATH_HEADER), $(dir)*.hpp))
  #HDR+= $(wildcard $(foreach dir,$(PATH_HEADER), $(dir)*.h))
ifneq ($(DEBUG),)
  $(info header files: $(HDR))
endif

# list of object files
OBJ = $(patsubst $(PATH_SUB_SRC)%,$(PATH_SUB_BUILD)%,$(foreach source, \
  $(basename $(SRC)),$(source).o))
ifneq ($(DEBUG),)
  $(info object files: $(OBJ))
endif

###############################################################################

# compiler flags
CFLAGS  = -I inc -Wall -std=c++11 -O3

# linker flags
LDFLAGS  = -pthread -lwiringPi

###############################################################################
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
	mkdir -p $(PATH_SUB_BUILD)

%.out: $(OBJ)
	@echo
	@mkdir -p $(@D)
	@echo "#### linking project"
	g++ $(CFLAGS) $(LDFLAGS) $(OBJ) --output $@

$(PATH_SUB_BUILD)%.o: $(PATH_SUB_SRC)%.cpp $(HDR)
	@echo
	@mkdir -p $(@D)
	@echo "#### compiling \"$@\" based on \"$^\""
	g++ -o $@ -c $(CFLAGS) -I "$(dir $*)/" $<

clean:
	@echo
	@echo "####" cleaning project \"make clean\"
	-rm -f -R $(foreach file,$(FILES_CLEAN),$(PATH_SUB_BUILD)$(file))
	#-rm -f -R $(foreach file,$(FILES_CLEAN),$(PATH_SUB_BUILD)*/$(file))

###############################################################################
