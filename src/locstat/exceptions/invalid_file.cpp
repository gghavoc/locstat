#include "invalid_file.h"

namespace locstat
{
    invalid_file::invalid_file()
    {
        const char *def_msg = "invalid file";
        int msg_len = strlen(def_msg);
        this->_what = new char[msg_len + 1];
        memcpy(this->_what, def_msg, msg_len);
    }
}