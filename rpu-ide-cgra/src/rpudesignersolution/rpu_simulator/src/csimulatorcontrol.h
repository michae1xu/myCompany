#ifndef __CSIMULATORCONTROL_H__
#define __CSIMULATORCONTROL_H__

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <memory>
#include "sysc/communication/sc_fifo.h"
#include "sysc/communication/sc_clock.h"
#ifdef USE_DLL_MODE
#include "rpu_global.h"
#endif

#include "export_typedef.h"

namespace rpu_esl_model {

class generator;
class rpu;
}

using namespace rpu_esl_model;

namespace rpu_esl_model
{
#ifndef USE_DLL_MODE
class CSimulatorControl
#else
class RPUSIMSHARED_EXPORT CSimulatorControl
#endif
{
public:

    static CSimulatorControl* getInstance(const string &pro_name,const std::string &config_filename = std::string());
    static void releaseInstance();

    CSimulatorControl(const CSimulatorControl&) = delete;
    CSimulatorControl& operator =(const CSimulatorControl&) = delete;

    int getCurCycle() ;
    bool getIsBusy() const;
    bool getIsRunMode() const;
    bool getIsDebugMode() const;
    ESIM_RET getLastRet() const;
    static string getRetMsg();

    // 仿真控制
    ESIM_RET loadProject(const std::string& pro_name);                          // 初始化仿真
    ESIM_RET startSim();                                                        // 开始仿真
    ESIM_RET startSimDebug();                                                   // 开始仿真调试
    ESIM_RET stopSim();                                                         // 停止仿真

    // 调试控制
    ESIM_RET debug_next();                                                      // 下一步
    ESIM_RET debug_jump(int cycle);                                             // 下一步
    ESIM_RET debug_continue();                                                  // 继续运行，遇到断点停止
//    ESIM_RET debug_finish();                                                  // 结束调试
    ESIM_RET debug_run();                                                       // 继续运行，忽略断点

    ESIM_RET debug_breakList(int coreIndex, vector<SBreak> &breakHandleList);   // 获取断点列表
    ESIM_RET debug_breakAdd(int coreIndex, const SBreak &breakHandle);          // 增加断点，并返回断点ID
    ESIM_RET debug_breakRemove(int coreIndex, const SBreak &breakHandle);       // 移除断点
    ESIM_RET debug_breakRemoveAll(int coreIndex);                               // 移除所有断点
//    ESIM_RET debug_break_disable(int coreIndex, SBreak &breakHandle);         // disable断点
//    ESIM_RET debug_break_disable_all(int coreIndex, );                        // disable所有断点
//    ESIM_RET debug_break_enable(int coreIndex, SBreak &breakHandle);          // enable断点
//    ESIM_RET debug_break_enable_all(int coreIndex, );                         // enable所有断点

    // 获取结果
    bool getIsFinish();                                                         // 仿真调试过程中，是否还有下一周期
    ESIM_RET getPEValue(int coreIndex, SPEValue &peValue);
    ESIM_RET getAllPEValue(int coreIndex, vector<SPEValue> &peValueList);
    ESIM_RET getRchValue(int coreIndex, vector<int> *&v);
    ESIM_RET getImdValue(int coreIndex, vector<int> *&v);
    ESIM_RET getLfsrValue(int coreIndex, vector<int> *&v);
    ESIM_RET getOutFifoValue(int coreIndex, std::shared_ptr<vector<int> > &v);
    ESIM_RET getMemoryValue(int coreIndex, std::shared_ptr<vector<int> > &v);


private:
    explicit CSimulatorControl(const string &pro_name, const std::string &config_filename);
    ~CSimulatorControl();

    static std::mutex s_mutex;
    static CSimulatorControl* s_instance;

    int m_curCycle;
    bool m_isBusy;
    bool m_isDebugMode;
    bool m_isFinish ;
    ESIM_RET m_lastRet;
    sc_core::sc_time *m_time = nullptr;
    generator *m_generate = nullptr;
    rpu *m_rpu = nullptr;
    std::map<std::string, std::string> m_source_map;

    SSimConfig m_simConfig;

    rpu_data_struct dfajldk;
    vector<sc_core::sc_fifo<rpu_data_struct> * > m_gen2chip_pkt_fifo;
    sc_core::sc_fifo<rpu_struct > *m_chip2gen_pkt_fifo = nullptr;
    sc_core::sc_fifo<rpu_struct > *m_gen2chip_recycle_addr_fifo = nullptr;
    sc_core::sc_clock *m_chip_clk = nullptr;

    void setDebugMode(bool debugMode = true);
};

}
#endif // CSIMULATORCONTROL_H
