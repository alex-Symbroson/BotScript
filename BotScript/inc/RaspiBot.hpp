
#ifndef _RASPIBOT_HPP_
#define _RASPIBOT_HPP_

#if ISBOT

#    include "include.hpp"
#    include "variables.hpp"

namespace RaspiBot {
    var_int leftMotor = 0, leftMotorTarget = 0, rightMotor = 0,
            rightMotorTarget = 0;

    bool Init();
    bool Call(const char* func, var_lst argv);
    void accelerate_motors();
    void Free();
    PVar& getRes();
} // namespace RaspiBot

#endif // ISBOT

#endif // _RASPIBOT_HPP_
