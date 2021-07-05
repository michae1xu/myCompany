#include "export_typedef.h"

namespace rpu_esl_model {

#define enumTostr_value(v) case v:return #v

    string g_retMsg;

    string enumTostr(ESIM_RET s)
    {
        switch (s)
        {
            enumTostr_value(ESIM_RET::RET_SUCCESS);
            enumTostr_value(ESIM_RET::RET_INFOMATION);
            enumTostr_value(ESIM_RET::RET_WARNING);
            enumTostr_value(ESIM_RET::RET_CRITICAL);
            enumTostr_value(ESIM_RET::RET_FATAL);
        }
        return string();
    }

    void setRetMsg(const string &retMsg)
    {
        g_retMsg = retMsg;
    }
}
