#ifndef EXPORT_TYPEDEF_H
#define EXPORT_TYPEDEF_H

#include <string>
//#include <tchar.h>
#include <deque>
#include "rpu_global.h"
#include "sysc/datatypes/int/sc_int.h"

using namespace std;
//using namespace rpu_esl_model;

namespace rpu_esl_model {

extern string g_retMsg;

void setRetMsg(const string &retMsg);

enum  ESIM_RET {
    RET_SUCCESS,
    RET_INFOMATION,
    RET_WARNING,
    RET_CRITICAL,
    RET_FATAL
};


#ifndef USE_DLL_MODE
string enumTostr(ESIM_RET s);
#else
string RPUSIMSHARED_EXPORT enumTostr(ESIM_RET s);
#endif

struct SSimConfig {
    int total_simu_time = 10000;
    int print_period = 50000;
    int top_clock = 500;
};


struct SPEIndex {
    int bcuIndex = 0;
    int rcuIndex = 0;
    int peIndex = 0;

    bool operator ==(const SPEIndex &i) const
    {
        return (bcuIndex == i.bcuIndex && rcuIndex == i.rcuIndex && peIndex == i.peIndex);
    }
    bool operator <(const SPEIndex &right) const
    {
        return ((bcuIndex < right.bcuIndex) ||
                (bcuIndex == right.bcuIndex && rcuIndex < right.rcuIndex) ||
                (bcuIndex == right.bcuIndex && rcuIndex == right.rcuIndex && peIndex < right.peIndex));
    }
    string getString(){
        char s[20] = {0};
        sprintf(s, "PE[%02d, %02d, %02d]", bcuIndex, rcuIndex, peIndex);
        return string(s);
    }
};

struct SPEValue {
    SPEIndex index;
    uint32_t inPort[4] = {0};
    uint32_t outPort[4] = {0};
};

struct SBreak {
    int32_t breadHandle = 0;   // 断点唯一ID
    SPEIndex index;            // 算子唯一索引ID
    bool inPort[4] = {false};  // 算子输入口断点
    bool outPort[4] = {false}; // 算子输出口断点
    bool isEnable = true;      // 断点使能标志
};

#ifndef USE_DLL_MODE
typedef class rpu_struct
#else
typedef class RPUSIMSHARED_EXPORT rpu_struct
#endif
    {
    public:
        int  req_no;
        bool response;
        int  req_data;
        int  cmd_type;
        int  latency;
        int  ring_addr;
        double time_stamp;

        rpu_struct()
        {
            req_no = -1;
            response = false;
            req_data = -1;
            cmd_type = -1;
            latency = 0;
            ring_addr = -1;
            time_stamp = 0;
        }

        friend ostream& operator<<(ostream& os, const rpu_struct& t){
            return  os;
        };
    } parser2cmd_ctrl_struct;



#ifndef USE_DLL_MODE
class rpu_data_struct
#else
class RPUSIMSHARED_EXPORT rpu_data_struct
#endif
    {
    public:
        int packet_no;
        uint32_t input_data;
        int latency;
        int core2crsbar_legth;
        int next_id;
        int cfg_addr;
        int bfu_a;
        int bfu_b;
        int bfu_t;
        int clc_mode;

        rpu_data_struct()
        {
            packet_no = -1;
            input_data = 0x0;
            latency = 0;
            core2crsbar_legth = 0;
            next_id = -1;
            bfu_a = 0;
            bfu_b = 0;
            bfu_t = 0;
        }

        friend ostream& operator<<(ostream& os, const rpu_data_struct& t){
            return  os;
        };
    };
}


#endif // EXPORT_TYPEDEF_H
