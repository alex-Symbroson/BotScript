
# BotScript
script language with optimized PIN access features

*Info:*
	detected allocation error on intel structure (works on arm structure)
	error line: BotScript/src/Variables.cpp/line 176
		during executing toFunction()
	-> added output of 'v' content in variable::variable

	analysis:
		some vector seem to have negative size
		maybe because of no default value?

linux shell commands:
- switch directory:
	- cd ~/Downloads/BotScript/BotScript-master/BotScript
- compile:
	- make

Dependencies:
- c:
	- math.h
	- time.h
	- stdio.h
	- softPwm.h  (part of wiringPi)
	- pthread.h
	- wiringPi.h    ( -> https://github.com/WiringPi/WiringPi )
- c++:
	- string
	- vector
	- iostream
	- forward_list
	- unordered_map
