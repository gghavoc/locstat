#include "invalid_input.h"

namespace locstat
{
    invalid_input::invalid_input()
    {
        const char *def_msg = "invalid input";
        int msg_len = strlen(def_msg);
        this->_what = new char[msg_len + 1];
        memcpy(this->_what, def_msg, msg_len);
    }
}