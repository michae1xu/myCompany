#include "testbench.h"
#include "generator.h"
#include "rpu.h"
#include "rpu_wrapper.h"
#include "csimulatorcontrol.h"
#include <future>
#include <thread>
#pragma warning(disable:W505)
using namespace rpu_esl_model;

#ifndef USE_DLL_MODE
int sc_main(int argc, char*argv[])
{
    if(argc < 2)
    {
        printf("Usag: %s projectname\n", argv[0]);
        return 0;
    }

#if 0
    CSimulatorControl ctrl("rpu_config.xml");
    ctrl.loadSolution(pro_name);

//    ctrl.startSim();
//    /*std::future<bool> =*/ std::async(std::launch::async, &CSimulatorControl::startSim, &ctrl);


#else

 
    map<string, string> source_map;

    string pro_name = string(argv[1]);
    const string &pro_dir_name = pro_name.substr(0, pro_name.find_last_of('/') + 1);

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
#if 1
	//ide_infifo_name = "plaintext_in.txt";
	//ide_mem_name = "vector.txt";
	//ide_xml_name = "sm3-0-0.xml";
	//ide_imd_name = "sm3_imd_init.txt";
#else

#endif

    m_generate->get_source_xml(ide_infifo_name);

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
