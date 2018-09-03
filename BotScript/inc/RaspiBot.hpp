
#ifndef _RASPIBOT_HPP_
#define _RASPIBOT_HPP_

#if ISBOT

#    include "include.hpp"
#    include "variables.hpp"

namespace RaspiBot {
    bool Init();
    bool Call(const char *func, var_lst argv);
    void Free();
} // namespace RaspiBot

#endif // ISBOT

#endif // _RASPIBOT_HPP_
