#!/bin/sh

# gt dir path of script
path=$(dirname $0)

# if botsc alias not defined in .bashrc -> append .bashrc, execute to enable alias
[ $(cat ~/.bashrc|egrep "alias botsc" | wc -l) = 0 ] && echo "alias botsc=\'$path/a.out\'" >>~/.bashrc && bash

#compile botsc.cpp if a.out not exist
[ -e $path/a.out ] || c++ $path/botsc.cpp;

#create compile.sh file if not exist -> checks repeatedly wether botsc.cpp changed - case yes -> recompile
[ -e $path/compile.sh ] || echo "LTIME=\`stat -c %Z $path/botsc.cpp\`; while true; do ATIME=\`stat -c %Z $path/botsc.cpp\`; if [ \"\$ATIME\" != \"\$LTIME\" ]; then echo compiling botsc.cpp; c++ -O3 $path/botsc.cpp; echo compiled; LTIME=$ATIME; fi; sleep .5; done" > $path/compile.sh && chmod +x $path/compile.sh

#create default code.bsc file if not exist
[ -e $path/code.bsc ] || echo "print(\"hallo\")" > $path/code.bsc

exit