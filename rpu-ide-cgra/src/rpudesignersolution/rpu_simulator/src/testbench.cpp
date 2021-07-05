#include "testbench.h"
#include "generator.h"
#include "rpu.h"
#include "rpu_wrapper.h"
#include "csimulatorcontrol.h"
#include <future>
#include <thread>
#if defined(WIN32)
//#include <windows.h>
#endif

//#define CRTDBG_MAP_ALLOC
//#include "stdlib.h"
//#include "crtdbg.h"

using namespace rpu_esl_model;

#ifndef USE_DLL_MODE
int sc_main(int argc, char*argv[])
{
    if(argc < 2)
    {
        printf("Usag: %s projectname\n", argv[0]);
        return 0;
    }

    setbuf(stdout, NULL);

    string pro_name = string(argv[1]);

    auto checkRet = [](rpu_esl_model::ESIM_RET ret) -> bool {
        switch (ret) {
        case ESIM_RET::RET_SUCCESS:
        case ESIM_RET::RET_INFOMATION:
        case ESIM_RET::RET_WARNING:
            return true;
        case ESIM_RET::RET_CRITICAL:
        case ESIM_RET::RET_FATAL:
            return false;
        default:
            break;
        }
        return false;
    };

#if 1
    for(int i = 0; i < 5; i++)
    {
        printf("==>malloc sim\r\n");
        auto* ctrl =  CSimulatorControl::getInstance(pro_name,"rpu_config.xml");
        ESIM_RET ret = ctrl->loadProject(pro_name);
        if(!checkRet(ret))
            return -1;
        printf("==>start sim in debug mode...\r\n");
#if 1
        thread simThread(&CSimulatorControl::startSim, ctrl);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        simThread.join();
        ctrl->stopSim();

        printf("==>get current cycle: %d\r\n", ctrl->getCurCycle());
        printf("==>get outfifo value...\r\n");
        std::shared_ptr<std::vector<int>> outFifo;
        ret = ctrl->getOutFifoValue(0, outFifo);
        if (!checkRet(ret))
            return -4;

        if (outFifo && outFifo->size() > 0)
        {
            printf("OutFifo value: %d\n", outFifo.get()->at(0));
        }

        printf("==>get memory value...\r\n");
        std::shared_ptr<std::vector<int>> memory = nullptr;
        ret = ctrl->getMemoryValue(0, memory);
        if (!checkRet(ret))
            return -5;

        if (memory && memory->size() > 0)
        {
            printf("Memory value: %d\n", memory.get()->at(0));
        }

#else
        thread simThread(&CSimulatorControl::startSimDebug, ctrl);
        //std::this_thread::sleep_for(std::chrono::milliseconds(5000));

        string cmd;
        const string prompt = "> ";
        cout << prompt;
        while (getline(cin, cmd))
        {
            cout << cmd;

            if (cmd == string("quit"))
            {
                cout << endl;
                break;
            }
            else if (cmd == string("n"))
            {
                printf("==>debug next...\r\n");
                ret = ctrl->debug_next();
                if (!checkRet(ret))
                    return -3;

                printf("==>get current cycle: %d\r\n", ctrl->getCurCycle());
                printf("==>get outfifo value...\r\n");
                std::vector<int> *outFifo = nullptr;
                ret = ctrl->getOutFifoValue(0, outFifo);
                if (!checkRet(ret))
                    return -4;

                if (outFifo && outFifo->size() > 0)
                {
                    printf("OutFifo value: %d\n", outFifo->at(0));
                }

                printf("==>get memory value...\r\n");
                std::vector<int> *memory = nullptr;
                ret = ctrl->getMemoryValue(0, memory);
                if (!checkRet(ret))
                    return -5;

                if (memory && memory->size() > 0)
                {
                    printf("Memory value: %d\n", memory->at(0));
                }

//                printf("==>get PE array value...\r\n");
//                vector<SPEValue> peValueList;
//                ret = ctrl->getAllPEValue(0, peValueList);
//                if (!checkRet(ret))
//                    return -5;

//                if (peValueList.size() > 0)
//                {
//                    printf("PE value: %d\n", peValueList.at(0).inPort[0]);
//                }
            }
            else if (cmd == string("c"))
            {
                printf("==>current cycle...\r\n");
                int cycle = ctrl->getCurCycle();
                printf("cycle: %d\n", cycle);
            }
            else if (cmd == string("f"))
            {
                printf("==>is finish...\r\n");
                bool isFinish = ctrl->getIsFinish();
                printf("isFinish: %d\n", isFinish);
            }
            cout << prompt;
        }

        simThread.join();

        printf("==>stop sim\r\n");
        ret = ctrl->stopSim();
        if (!checkRet(ret))
            return -6;
# endif
        printf("==>release sim\r\n");
        CSimulatorControl::releaseInstance();

    }
#else


#if defined(_MSC_VER)
#if defined(WIN32)
    const string &pro_dir_name = pro_name.substr(0, pro_name.find_last_of('\\') + 1);
#else
    const string &pro_dir_name = pro_name.substr(0, pro_name.find_last_of('/') + 1);
#endif
#elif defined(__GNUC__)
    const string &pro_dir_name = pro_name.substr(0, pro_name.find_last_of('/') + 1);
#endif

    map<string, string> source_map;

    string ide_xml_name;
    string ide_mem_name;
    string ide_imd_name;
//    string ide_rch_name;
    string ide_infifo_name;
    {
        tinyxml2::XMLDocument *pro_document = new tinyxml2::XMLDocument();
        pro_document->LoadFile(pro_name.c_str());
        XMLElement *root = pro_document->RootElement();
        if(root)
        {
            XMLElement* ele = root->FirstChildElement("XML");
            const char* xml_filename = ele->Attribute("value");
            if(xml_filename == NULL)
            {
                printf("XML file parse error!\n");
                return -2;
            }
            ide_xml_name = pro_dir_name + string(xml_filename);

            XMLElement* src = root->FirstChildElement("SRC");
            while(src != NULL)
            {
                const char* src_filename = src->Attribute("value");
                if(src_filename == NULL)
                {
                    printf("XML file parse error!\n");
                    return -2;
                }
                string src_key(":/");
                string src_value(src_filename);
                string::size_type index = src_value.find_last_of("/");
                if(index == string::npos)
                {
                    src_key += src_value;
                }
                else
                {
                    src_key += src_value.substr(index + 1);
                }
                source_map.insert({src_key, pro_dir_name + src_value});
                src = src->NextSiblingElement("SRC");

                if(src_value.find(".memory") != string::npos) // FIX ME
                {
                    ide_mem_name = pro_dir_name + src_value;
                }
                else if(src_value.find(".imd") != string::npos)
                {
                    ide_imd_name = pro_dir_name + src_value;
                }
//                else if(src_value.find(".rch") != string::npos)
//                {
//                    ide_rch_name = pro_dir_name + src_value;
//                }
                else if(src_value.find(".fifo") != string::npos)
                {
                    ide_infifo_name = pro_dir_name + src_value;
                }
            }
        }
        delete pro_document;
    }

    string xml_name = "rpu_config.xml";
    tinyxml2::XMLDocument *xml_document = new tinyxml2::XMLDocument();
    xml_document->LoadFile(xml_name.c_str());
    XMLElement *root = xml_document->RootElement();
    int total_simu_time;
    int print_period;
    int top_clock;
    if (root)
    {
        XMLElement *tb_ele = root->FirstChildElement("tb");
        if (tb_ele)
        {
            top_clock = atoi(tb_ele->Attribute("top_clock"));
            total_simu_time = atoi(tb_ele->Attribute("simu_time"));
            print_period = atoi(tb_ele->Attribute("print_period"));
        }
    }
    else {
        cout << "rpu_config.xml not exist!" << endl;
        exit(-1);
    }
    delete xml_document;

    sc_time time(total_simu_time, SC_US);
    sc_clock chip_clk("chip_clk", 1000.0 / top_clock, SC_NS);

    generator *m_generate = new generator("generate", time);
    rpu *m_rpu = new rpu("rpu");

    //bind
    sc_fifo<rpu_data_struct >          m_gen2chip_pkt_fifo(64);
    sc_fifo<rpu_struct >          m_chip2gen_pkt_fifo(64);
    sc_fifo<rpu_struct >          m_gen2chip_recycle_addr_fifo(64);


    m_generate->sco_gen2chip_pkt(m_gen2chip_pkt_fifo);
    m_generate->sci_chip2gen_pkt(m_chip2gen_pkt_fifo);
    m_generate->chip_in_clk(chip_clk);
    m_generate->sco_recycle_ring_addr(m_gen2chip_recycle_addr_fifo);

    m_rpu->sci_gen2chip_pkt(m_gen2chip_pkt_fifo);
    m_rpu->sco_chip2gen_pkt(m_chip2gen_pkt_fifo);
    m_rpu->chip_in_clk(chip_clk);
    m_rpu->sci_recycle_ring_addr(m_gen2chip_recycle_addr_fifo);

    printf("Simu Percent : %0.2f%%           speed :  %d cycle/s \r", sc_time_stamp().to_double() / time.to_double() * 100, 1000);

    m_rpu->set_source_map(&source_map);
    m_generate->get_source_xml(&ide_infifo_name);
    m_rpu->get_mem_file_name(ide_mem_name);
    m_rpu->get_xml_name(ide_xml_name);
    m_rpu->get_imd_file_name(ide_imd_name);

    sc_start(time);

    delete xml_document;
    delete m_rpu;
#endif
    return 0;
}

#endif

class SimCtrlRAII
{
public:
    explicit SimCtrlRAII(rpu_esl_model::CSimulatorControl **simCtrl = nullptr) :
        m_simCtrlP(simCtrl)
    {
        m_simCtrl = rpu_esl_model::CSimulatorControl::getInstance("rpu_config.xml");
        if(m_simCtrlP)
            *m_simCtrlP = m_simCtrl;
    }
    ~SimCtrlRAII()
    {
        m_simCtrl = nullptr;
        if(m_simCtrlP)
            *m_simCtrlP = nullptr;
        rpu_esl_model::CSimulatorControl::releaseInstance();
    }

    rpu_esl_model::CSimulatorControl *getSimCtrl() const
    {
        return m_simCtrl;
    }

private:
    rpu_esl_model::CSimulatorControl* m_simCtrl = nullptr;
    rpu_esl_model::CSimulatorControl** m_simCtrlP = nullptr;
};


