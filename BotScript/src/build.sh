
header="include variables pins threads builtins";
main="BotScript"; s=$(stat -c %Y build.sh); out="";
if [ "$(stat -c %Y $main.cpp)" != "$s" ]; then chd=true; else chd=false; fi;

function exec() { echo "$ $1">&0; $1; }

for f in $header; do
	if [ "$(stat -c %Y $f.cpp)" != "$s" ] || [ ! -e $f.o ]
	then exec "g++ -std=c++11 -O3 -c $f.cpp"; changed=true;
	fi;
	out="$out $f.o";
done

if [ "$chd" = "true" ] || [ ! -e $main.out ];
then exec "g++ -std=c++11 -O3 $main.cpp -lwiringPi -pthread $out -o $main.out";
fi;
touch *;
