
#ifndef _PCH_HPP_
#define _PCH_HPP_

#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <forward_list> //alternative linked list container
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map> //k:v object container
#include <vector>        //alternative array based list container

#ifdef _DEBUG
#    include "nvwa/debug_new.h"
#endif

#include "macros.hpp"

using namespace std;

#endif //_PCH_HPP_
