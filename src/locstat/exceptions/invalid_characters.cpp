#include "invalid_characters.h"

namespace locstat
{
    invalid_characters::invalid_characters()
    {
        const char *def_msg = "invalid characters";
        int msg_len = strlen(def_msg);
        this->_what = new char[msg_len + 1];
        memcpy(this->_what, def_msg, msg_len);
    }
}