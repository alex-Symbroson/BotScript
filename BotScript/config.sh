
# gt dir path of script
path=$(dirname $0)

# if botsc alias not defined in .bashrc -> append .bashrc, execute to enable alias
[ $(cat ~/.bashrc|egrep "alias botsc" | wc -l) = 0 ] && echo "alias botsc=\'$path/a.out\'" >> ~/.bashrc && bash
[ $(cat ~/.bashrc|egrep "alias compile" | wc -l) = 0 ] && echo "alias compile='function f() { c++ -std=c++11 -O3 -lwiringPi -pthread -o $path/src/\$1.out $path/src/\$1.cpp; [ \$2 ] && $path/src/\$1.out; }; f'" >> ~/.bashrc && bash

#create default code.bsc file if not exist
[ -e $path/src/code.bsc ] || echo "print(\"hallo\")" > $path/src/code.bsc

exit