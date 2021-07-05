#include "csimulatorcontrol.h"
#include <string>
#include "testbench.h"
#include "generator.h"
#include "rpu.h"
#include "rpu_wrapper.h"

using namespace std;

#ifdef USE_DLL_MODE
int sc_main(int , char*[])
{
    return 0;
}
#endif

namespace rpu_esl_model {
std::mutex CSimulatorControl::s_mutex;
CSimulatorControl* CSimulatorControl::s_instance = nullptr;

CSimulatorControl::CSimulatorControl(const string &pro_name, const string &config_filename)
{

//    int argc = 0;
//    char** argv = nullptr;
//    sc_core::sc_elab_and_sim( argc, argv );
    m_curCycle = 0;
    m_isBusy = false;
    m_isDebugMode = false;
    m_isFinish = false;
    m_lastRet = RET_SUCCESS;

    tinyxml2::XMLDocument *xml_document = new tinyxml2::XMLDocument();
    xml_document->LoadFile(config_filename.c_str());
    XMLElement *root = xml_document->RootElement();
    int rcu_num = 256;
    int bcu_num = 1;
    if (root)
    {
        XMLElement *tb_ele = root->FirstChildElement("tb");
        if (tb_ele)
        {

			const char *c = tb_ele->Attribute("top_clock");
            m_simConfig.top_clock = c ? atoi(c) : 500;
			c = tb_ele->Attribute("simu_time");
            m_simConfig.total_simu_time = c ? atoi(c) : 1000;
			c = tb_ele->Attribute("print_period");
            m_simConfig.print_period = c ? atoi(c) : 5000;
			c = tb_ele->Attribute("rcu_num");
            rcu_num = c ? atoi(c) : 256;
            //c = tb_ele->Attribute("bcu_num");
            //bcu_num = c ? atoi(c) : 1;
        }
    }

    delete xml_document;

    int bcu_xml_num = 0;
    tinyxml2::XMLDocument *pro_document = new tinyxml2::XMLDocument();
    pro_document->LoadFile(pro_name.c_str());
    root = pro_document->RootElement();
    if (root)
    {
        XMLElement* ele = root->FirstChildElement("XML");
        while (ele != NULL)
        {
            const char* xml_filename = ele->Attribute("value");
            int   xml_bcu_id = atoi(ele->Attribute("sort"));
            int   select = atoi(ele->Attribute("select"));

            if (1 == select)
            {
                bcu_xml_num++;
            }

            ele = ele->NextSiblingElement("XML");
            //XMLElement* src = root->FirstChildElement("SRC");

        }
    }
    delete pro_document;

    if (bcu_xml_num >= bcu_num)
    {
        bcu_num = bcu_xml_num;
    }


    m_time = new sc_time(m_simConfig.total_simu_time, SC_US);
	stringstream  sss;
	sss << "generator";
	string aa = sss.str();
	const char *generate_name = aa.c_str();
    m_generate = new rpu_esl_model::generator(generate_name, *m_time, bcu_num);


	sss << "rpu";
	aa = sss.str();
	const char *rpu_name = aa.c_str();
    m_rpu = new rpu(rpu_name, bcu_num ,rcu_num);


    m_generate->set_rca_ptr(m_rpu->m_rca_mod);


    m_gen2chip_pkt_fifo.resize(bcu_num);
    for (int i = 0; i < bcu_num; ++i)
    {
        m_gen2chip_pkt_fifo[i] = new sc_fifo<rpu_data_struct >(64);

    }
    m_chip2gen_pkt_fifo = new sc_fifo<rpu_struct >(64);
    m_gen2chip_recycle_addr_fifo = new sc_fifo<rpu_struct >(64);


	sss << "clk";
	aa = sss.str();
	const char *clk_name = aa.c_str();
    m_chip_clk = new sc_core::sc_clock(clk_name, 1000.0 / m_simConfig.top_clock, SC_NS);

    for (int i = 0; i < bcu_num; ++i)
    {
        (*m_generate->sco_gen2chip_pkt[i])(*m_gen2chip_pkt_fifo[i]);
        (*m_rpu->sci_gen2chip_pkt[i])(*m_gen2chip_pkt_fifo[i]);
    }


    m_generate->sci_chip2gen_pkt(*m_chip2gen_pkt_fifo);
    m_generate->chip_in_clk(*m_chip_clk);
    m_generate->sco_recycle_ring_addr(*m_gen2chip_recycle_addr_fifo);


    m_rpu->sco_chip2gen_pkt(*m_chip2gen_pkt_fifo);
    m_rpu->chip_in_clk(*m_chip_clk);
    m_rpu->sci_recycle_ring_addr(*m_gen2chip_recycle_addr_fifo);

    printf("Simu Percent : %0.2f%%           speed :  %d cycle/s \r", sc_time_stamp().to_double() / (*m_time).to_double() * 100, 1000);

}

CSimulatorControl::~CSimulatorControl()
{
    // mem leak
    delete m_time;
    m_time = nullptr;
    delete m_rpu;
    m_rpu = nullptr;
    delete m_generate;
    m_generate = nullptr;

    while (!m_gen2chip_pkt_fifo.empty()) {
        delete m_gen2chip_pkt_fifo.back(),m_gen2chip_pkt_fifo.pop_back();
    }
    delete m_chip2gen_pkt_fifo;
    m_chip2gen_pkt_fifo = nullptr;
    delete m_gen2chip_recycle_addr_fifo;
    m_gen2chip_recycle_addr_fifo = nullptr;
    delete m_chip_clk;
    m_chip_clk = nullptr;

////    delete m_gen2chip_pkt_fifo;
//    delete m_chip2gen_pkt_fifo;
//    delete m_gen2chip_recycle_addr_fifo;
//    delete m_chip_clk;

//	m_generate = NULL;
//	m_rpu = NULL;
//	//m_gen2chip_pkt_fifo = NULL;
//	m_chip2gen_pkt_fifo = NULL;
//	m_gen2chip_recycle_addr_fifo = NULL;
//    m_chip_clk = NULL;
}

CSimulatorControl *CSimulatorControl::getInstance(const string &pro_name,const string &config_filename)
{
    printf("sim get instance\n");
    if(s_instance == nullptr)
    {
        s_mutex.lock();
        if(s_instance == nullptr)
        {
            try
            {
                s_instance = new CSimulatorControl(pro_name,config_filename);
            }
            catch(std::bad_alloc)
            {
                s_instance = nullptr;
                printf("sim get instance bad alloc\n");
            }
        }
        s_mutex.unlock();
    }
    return s_instance;
}

void CSimulatorControl::releaseInstance()
{
    printf("sim release instance\n");
    if(s_instance)
    {
        s_mutex.lock();
        if(s_instance)
        {
            delete s_instance;
            s_instance = nullptr;

            if(sc_is_running())
                sc_stop();
            sc_get_curr_simcontext()->reset();
        }
        s_mutex.unlock();
    }
}

ESIM_RET CSimulatorControl::loadProject(const string &pro_name)
{
#if defined(_MSC_VER)
#if defined(WIN32)
    const string &pro_dir_name = pro_name.substr(0, pro_name.find_last_of('\\') + 1);
#else
    const string &pro_dir_name = pro_name.substr(0, pro_name.find_last_of('/') + 1);
#endif
#elif defined(__GNUC__)
    const string &pro_dir_name = pro_name.substr(0, pro_name.find_last_of('/') + 1);
#endif

    vector<string > ide_xml_name;
    ide_xml_name.resize(16);
    string ide_mem_name;
    string ide_imd_name;
    //    string ide_rch_name;
    vector<string >ide_infifo_name;
    ide_infifo_name.resize(16);
    {
        tinyxml2::XMLDocument *pro_document = new tinyxml2::XMLDocument();
        pro_document->LoadFile(pro_name.c_str());
        XMLElement *root = pro_document->RootElement();
        if(root)
        {
            XMLElement* ele = root->FirstChildElement("XML");
            while (ele !=NULL)
            {
                const char* xml_filename = ele->Attribute("value");
                int   xml_bcu_id = atoi(ele->Attribute("sort"));
                int   select = atoi(ele->Attribute("select"));

                if (xml_filename == NULL)
                {
                    printf("XML file parse error!\n");
                    g_retMsg = string(u8"工程文件“") + pro_name + u8"”解析失败";
                    delete pro_document;
                    return ESIM_RET::RET_CRITICAL;
                }
                if (xml_bcu_id == -1)
                {
                    xml_bcu_id = 0;
                }
                ide_xml_name[xml_bcu_id] = pro_dir_name + string(xml_filename);

                m_source_map.clear();
                XMLElement* src = root->FirstChildElement("SRC");
                while (src != NULL)
                {
                    const char* src_filename = src->Attribute("value");
                    if (src_filename == NULL)
                    {
                        printf("XML file parse error!\n");
                        g_retMsg = string(u8"工程文件“") + pro_name + u8"”解析失败";
                        delete pro_document;
                        return ESIM_RET::RET_CRITICAL;
                    }
                    string src_key(":/");
                    string src_value(src_filename);
                    string::size_type index = src_value.find_last_of("/");
                    if (index == string::npos)
                    {
                        src_key += src_value;
                    }
                    else
                    {
                        src_key += src_value.substr(index + 1);
                    }
                    m_source_map.insert({ src_key, pro_dir_name + src_value });
                    src = src->NextSiblingElement("SRC");

                    if (src_value.find(".memory") != string::npos) // FIX ME
                    {
                        ide_mem_name = pro_dir_name + src_value;
                    }
                    else if (src_value.find(".imd") != string::npos)
                    {
                        ide_imd_name = pro_dir_name + src_value;
                    }
                    //                else if(src_value.find(".rch") != string::npos)
                    //                {
                    //                    ide_rch_name = pro_dir_name + src_value;
                    //                }
                    else if (src_value.find(".fifo") != string::npos)
                    {
                        ide_infifo_name[xml_bcu_id] = pro_dir_name + src_value;
                    }
                }

                ele = ele->NextSiblingElement("XML");
            }
            
        }
        delete pro_document;
    }
	ESIM_RET load_infifo_xml_sucess = RET_SUCCESS;
	ESIM_RET load_mem_xml_sucess = RET_SUCCESS;
	ESIM_RET load_ide_xml_sucess = RET_SUCCESS;
	ESIM_RET load_imd_xml_sucess = RET_SUCCESS;

    m_rpu->set_source_map(&m_source_map);
	load_infifo_xml_sucess = m_generate->get_source_xml(ide_infifo_name);
	load_mem_xml_sucess = m_rpu->get_mem_file_name(ide_mem_name);
	load_ide_xml_sucess = m_rpu->get_xml_name(ide_xml_name,pro_name);
	load_imd_xml_sucess = m_rpu->get_imd_file_name(ide_imd_name);
	if (RET_SUCCESS != load_infifo_xml_sucess)
	{
		return load_infifo_xml_sucess;
	}
	if (RET_SUCCESS != load_mem_xml_sucess)
	{
		return load_mem_xml_sucess;
	}
	if (RET_SUCCESS != load_ide_xml_sucess)
	{
		return load_ide_xml_sucess;
	}
	if (RET_SUCCESS != load_imd_xml_sucess)
	{
		return load_imd_xml_sucess;
	}

    return ESIM_RET::RET_SUCCESS;
}

void CSimulatorControl::setDebugMode(bool debugMode)
{
    m_isDebugMode = debugMode;
    auto *bcu_mod_v = m_rpu->getRca_mod()->getBcu_mod();
    for(auto i = 0; i < bcu_mod_v->size(); ++i)
    {
        auto* bcu_mod = bcu_mod_v->at(i);
        bcu_mod->set_debug_by_step_mode(debugMode);
    }
    m_generate->set_debug_by_step_mode(debugMode);
}

int CSimulatorControl::getCurCycle()
{
    const auto *bcu_mod_v = m_rpu->getRca_mod()->getBcu_mod();
    if (bcu_mod_v->size() > 0)
    {
        //获取BCU_0的cycle就行了，仿真cycle是一致的
        auto* bcu_mod = bcu_mod_v->at(0);
        //m_curCycle = bcu_mod->m_cycle_cnt;
        //m_curCycle = bcu_mod->m_rcu_order_id;
        m_curCycle = bcu_mod->m_rcu_order_id - 1;
    }
    return m_curCycle;
}

bool CSimulatorControl::getIsBusy() const
{
    return m_isBusy;
}

bool CSimulatorControl::getIsRunMode() const
{
    return !m_isDebugMode;
}

bool CSimulatorControl::getIsDebugMode() const
{
    return m_isDebugMode;
}

ESIM_RET CSimulatorControl::getLastRet() const
{
    return m_lastRet;
}

string CSimulatorControl::getRetMsg()
{
    return g_retMsg;
}

ESIM_RET CSimulatorControl::startSim()
{
    printf("start SIM\n");
    m_curCycle = 0;
    m_isBusy = true;
    sc_start(*m_time);

    //while (sc_pending_activity())
    //{
    //    sc_start(sc_time_to_pending_activity());
    //}
    //sc_start();
    //sc_start(*m_time);
    printf("start SIM end\n");
    return ESIM_RET::RET_SUCCESS;
}

ESIM_RET CSimulatorControl::startSimDebug()
{
    setDebugMode();
    return startSim();
}

ESIM_RET CSimulatorControl::stopSim()
{
    m_isBusy = false;
    m_isDebugMode = false;
//    if(!getIsFinish())
    if(sc_is_running())
        sc_stop();

//    sc_initialize();
//    sc_get_curr_simcontext()->reset();
//    sc_get_curr_simcontext()->initialize(true);
    return ESIM_RET::RET_SUCCESS;
}

ESIM_RET CSimulatorControl::debug_next()
{
    auto *bcu_mod_v = m_rpu->getRca_mod()->getBcu_mod();

    if (bcu_mod_v->size() > 0)
    {
        //auto* bcu_mod = bcu_mod_v->at(bcu_mod_v->size() - 1);
        auto* bcu_mod = bcu_mod_v->at(0);
        bcu_mod->notify_debug_by_step_test();
        //
        m_generate->notify_debug_by_step_test();
    }
    //sc_start();
    return ESIM_RET::RET_SUCCESS;
}

ESIM_RET CSimulatorControl::debug_jump(int cycle)
{
    return ESIM_RET::RET_SUCCESS;
}

ESIM_RET CSimulatorControl::debug_continue()
{
    return ESIM_RET::RET_SUCCESS;
}

//ESIM_RET CSimulatorControl::debug_finish()
//{
//    return ESIM_RET::RET_SUCCESS;
//}

ESIM_RET CSimulatorControl::debug_run()
{
    return ESIM_RET::RET_SUCCESS;
}

ESIM_RET CSimulatorControl::debug_breakList(int coreIndex, vector<SBreak> &breakHandleList)
{
    return ESIM_RET::RET_SUCCESS;
}

ESIM_RET CSimulatorControl::debug_breakAdd(int coreIndex, const SBreak &breakHandle)
{
    return ESIM_RET::RET_SUCCESS;
}

ESIM_RET CSimulatorControl::debug_breakRemove(int coreIndex, const SBreak &breakHandle)
{
    return ESIM_RET::RET_SUCCESS;
}

ESIM_RET CSimulatorControl::debug_breakRemoveAll(int coreIndex)
{
    return ESIM_RET::RET_SUCCESS;
}

bool CSimulatorControl::getIsFinish()
{
    const auto *bcu_mod_v = m_rpu->getRca_mod()->getBcu_mod();
    if (bcu_mod_v->size() > 0)
    {
        m_isFinish = true;
        for (auto i = 0; i < bcu_mod_v->size(); ++i)
        {
            auto* bcu_mod = bcu_mod_v->at(i);
            m_isFinish &= bcu_mod->m_bcu_simu_end ;
        }
    } 
    return m_isFinish;
}
//使用此函数需要传递coreindex,rcu_index,peindex
ESIM_RET CSimulatorControl::getPEValue(int coreIndex, SPEValue &peValue)
{
    const auto *bcu_mod_v = m_rpu->getRca_mod()->getBcu_mod();
    if (coreIndex > bcu_mod_v->size())
    {
        return ESIM_RET::RET_FATAL;
    }
    const auto* rcu_mod_v = bcu_mod_v->at(coreIndex)->getRcu_mod();

    if (peValue.index.rcuIndex > rcu_mod_v->size())
    {
        return ESIM_RET::RET_FATAL;
    }

    int order_id = bcu_mod_v->at(coreIndex)->m_rcuid2orderid_map[peValue.index.rcuIndex];
    if (order_id < 0)
    {
        return ESIM_RET::RET_FATAL;
    }
    auto *rcu_mod = rcu_mod_v->at(order_id);

    if (peValue.index.peIndex < 0 || peValue.index.peIndex >12)
    {
        return ESIM_RET::RET_FATAL;
    }
    if (peValue.index.peIndex < 8)
    {
        //bfu    
        peValue.inPort[0] = (rcu_mod->m_bfu_mod[peValue.index.peIndex])->input_a_in;
        peValue.inPort[1] = (rcu_mod->m_bfu_mod[peValue.index.peIndex])->input_b_in;
        peValue.inPort[2] = (rcu_mod->m_bfu_mod[peValue.index.peIndex])->input_t_in;
        peValue.outPort[0] = (rcu_mod->m_bfu_mod[peValue.index.peIndex])->m_bfu_out[0];
        peValue.outPort[1] = (rcu_mod->m_bfu_mod[peValue.index.peIndex])->m_bfu_out[1];
    }
    else if (peValue.index.peIndex >= 8 && peValue.index.peIndex < 11)
    {
        //sbox
        uint32_t  sbox_id = peValue.index.peIndex - 8;
        peValue.inPort[0] = (rcu_mod->m_sbox_mod[sbox_id])->m_sbox_in_date;
        peValue.outPort[0] = (rcu_mod->m_sbox_mod[sbox_id])->m_sbox_out[0];
        peValue.outPort[1] = (rcu_mod->m_sbox_mod[sbox_id])->m_sbox_out[1];
        peValue.outPort[2] = (rcu_mod->m_sbox_mod[sbox_id])->m_sbox_out[2];
        peValue.outPort[3] = (rcu_mod->m_sbox_mod[sbox_id])->m_sbox_out[3];

    }
    else
    {
        //benes
        uint32_t  benes_id = peValue.index.peIndex - 12;
        peValue.inPort[0] = (rcu_mod->m_benes_mod[benes_id])->m_input_benes[0];
        peValue.inPort[1] = (rcu_mod->m_benes_mod[benes_id])->m_input_benes[1];
        peValue.inPort[2] = (rcu_mod->m_benes_mod[benes_id])->m_input_benes[2];
        peValue.inPort[3] = (rcu_mod->m_benes_mod[benes_id])->m_input_benes[3];

        peValue.outPort[0] = (rcu_mod->m_benes_mod[benes_id])->m_benes_out[0];
        peValue.outPort[1] = (rcu_mod->m_benes_mod[benes_id])->m_benes_out[1];
        peValue.outPort[2] = (rcu_mod->m_benes_mod[benes_id])->m_benes_out[2];
        peValue.outPort[3] = (rcu_mod->m_benes_mod[benes_id])->m_benes_out[3];
    }

    return ESIM_RET::RET_SUCCESS;
}

//使用此函数获取当前重构所有算子的输入输出值
ESIM_RET CSimulatorControl::getAllPEValue(int coreIndex, vector<SPEValue> &peValueList)
{
    const auto *bcu_mod_v = m_rpu->getRca_mod()->getBcu_mod();
    if (coreIndex > bcu_mod_v->size())
    {
        return ESIM_RET::RET_FATAL;
    }
    const auto* rcu_mod_v = bcu_mod_v->at(coreIndex)->getRcu_mod();

    //int  order_id = bcu_mod_v->at(coreIndex)->m_rcu_order_id;
    int  order_id = bcu_mod_v->at(coreIndex)->m_rcu_order_id - 1;
    int  rcu_id = 0;
    if (order_id >=0)
    {
        rcu_id = bcu_mod_v->at(coreIndex)->m_orderid2rcuid_map[order_id];
    }
    auto *rcu_mod = rcu_mod_v->at(order_id);

    //for (int i = 0; i < rcu_mod_v->size(); ++i)
    //{
    peValueList.resize(13);
    for (int j = 0; j < 13; ++j)
    {
        peValueList[j].index.bcuIndex = coreIndex;
        peValueList[j].index.rcuIndex = rcu_id;
        peValueList[j].index.peIndex = j;

        if (j < 8)
        {
            //bfu    
            peValueList[j].inPort[0] = (rcu_mod->m_bfu_mod[j])->input_a_in;
            peValueList[j].inPort[1] = (rcu_mod->m_bfu_mod[j])->input_b_in;
            peValueList[j].inPort[2] = (rcu_mod->m_bfu_mod[j])->input_t_in;
            peValueList[j].outPort[0] = (rcu_mod->m_bfu_mod[j])->m_bfu_out[0];
            peValueList[j].outPort[1] = (rcu_mod->m_bfu_mod[j])->m_bfu_out[1];
        }
        else if (j >= 8 && j <= 11)
        {
            //sbox
            uint32_t  box_id = j - 8;
            peValueList[j].inPort[0] = (rcu_mod->m_sbox_mod[box_id])->m_sbox_in_date;
            peValueList[j].outPort[0] = (rcu_mod->m_sbox_mod[box_id])->m_sbox_out[0];
            peValueList[j].outPort[1] = (rcu_mod->m_sbox_mod[box_id])->m_sbox_out[1];
            peValueList[j].outPort[2] = (rcu_mod->m_sbox_mod[box_id])->m_sbox_out[2];
            peValueList[j].outPort[3] = (rcu_mod->m_sbox_mod[box_id])->m_sbox_out[3];

        }
        else
        {
            //benes
            uint32_t  benes_id = j - 12;
            peValueList[j].inPort[0] = (rcu_mod->m_benes_mod[benes_id])->m_input_benes[0];
            peValueList[j].inPort[1] = (rcu_mod->m_benes_mod[benes_id])->m_input_benes[1];
            peValueList[j].inPort[2] = (rcu_mod->m_benes_mod[benes_id])->m_input_benes[2];
            peValueList[j].inPort[3] = (rcu_mod->m_benes_mod[benes_id])->m_input_benes[3];

            peValueList[j].outPort[0] = (rcu_mod->m_benes_mod[benes_id])->m_benes_out[0];
            peValueList[j].outPort[1] = (rcu_mod->m_benes_mod[benes_id])->m_benes_out[1];
            peValueList[j].outPort[2] = (rcu_mod->m_benes_mod[benes_id])->m_benes_out[2];
            peValueList[j].outPort[3] = (rcu_mod->m_benes_mod[benes_id])->m_benes_out[3];
        }

    }
    //}


    return ESIM_RET::RET_SUCCESS;
}

ESIM_RET CSimulatorControl::getRchValue(int coreIndex, vector<int> *&v)
{
    return ESIM_RET::RET_SUCCESS;
}

ESIM_RET CSimulatorControl::getImdValue(int coreIndex, vector<int> *&v)
{
    return ESIM_RET::RET_SUCCESS;
}

ESIM_RET CSimulatorControl::getLfsrValue(int coreIndex, vector<int> *&v)
{
    return ESIM_RET::RET_SUCCESS;
}

ESIM_RET CSimulatorControl::getOutFifoValue(int coreIndex, std::shared_ptr<vector<int>> &v)
{
#if 1
    // TODO HXM 请把outFifo输出写到这里
    const auto *bcu_mod_v = m_rpu->getRca_mod()->getBcu_mod();
    if(bcu_mod_v->size() > 0)
    {
        const auto* rcu_mod_v = bcu_mod_v->at(coreIndex)->getRcu_mod();
        if(rcu_mod_v->size() > 0)
        {
			if ((*bcu_mod_v->at(bcu_mod_v->size() - 1)->get_lastorder_id())>=1)
			{
				auto *rcu_mod = rcu_mod_v->at((*bcu_mod_v->at(bcu_mod_v->size() - 1)->get_lastorder_id()) - 1);
                v = rcu_mod->getOut_fifo_data();
			}
        }
    }
#else
    v = new vector<int>(12);
    for(decltype (v->size()) i = 0; i < v->size(); ++i)
    {
        (*v)[i] = i;
    }
#endif
    ESIM_RET ret = RET_SUCCESS;
    if(v == nullptr)
    {
        ret = RET_WARNING;
        g_retMsg = string(u8"获取OutFIFO数据错误");
    }
    return ret;
}

ESIM_RET CSimulatorControl::getMemoryValue(int coreIndex, std::shared_ptr<vector<int>> &v)
{
#if 1
    // TODO HXM 请把memory输出写到这里
	const auto *bcu_mod_v = m_rpu->getRca_mod()->getBcu_mod();
	if (bcu_mod_v->size() > 0)
	{
		//const auto* rcu_mod_v = bcu_mod_v->at(bcu_mod_v->size() - 1)->getRcu_mod();		
		v = bcu_mod_v->at(bcu_mod_v->size() - 1)->get_mem_data();
	}

#else
    v = new vector<int>(12);
    for(decltype (v->size()) i = 0; i < v->size(); ++i)
    {
        (*v)[i] = i + 12;
    }
#endif
    ESIM_RET ret = RET_SUCCESS;
    if(v == nullptr)
    {
        ret = RET_WARNING;
        g_retMsg = string(u8"获取OutFIFO数据错误");
    }
    return ret;
}

}




