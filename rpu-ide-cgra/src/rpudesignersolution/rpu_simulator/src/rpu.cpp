
#include "rpu.h"
namespace rpu_esl_model
{
#ifdef USE_DLL_MODE
    int creat_rpu_chip(sc_module_name name,string ide_xml_name, string ide_mem_name)
    {
		//string xml_name = "rpu_config.xml";
		//tinyxml2::XMLDocument *m_document = new tinyxml2::XMLDocument();
		//m_document->LoadFile(xml_name.c_str());
		//XMLElement *root = m_document->RootElement();
		//int total_simu_time;
		//int print_period;
		//int top_clock;
		//if (root)
		//{
		//	XMLElement *tb_ele = root->FirstChildElement("tb");
		//	if (tb_ele)
		//	{
		//		top_clock = atoi(tb_ele->Attribute("top_clock"));
		//		total_simu_time = atoi(tb_ele->Attribute("simu_time"));
		//		print_period = atoi(tb_ele->Attribute("print_period"));
		//	}
		//}

		//sc_time time(total_simu_time, SC_US);
		//sc_clock chip_clk("chip_clk", 1000.0 / top_clock, SC_NS);
		////generator
		//generator *m_generate;
		//m_generate = new generator("generate", time);
		////module
		//rpu    *m_rpu;
		//m_rpu = new rpu("rpu");
		////m_rpu = creat_rpu_chip("rpu");
		////string ide_xml_name = "test.xml";
		////string ide_mem_name = "vector_0_init.txt";

		//m_rpu->get_mem_file_name(ide_mem_name);
		//m_rpu->get_xml_name(ide_xml_name);

		////bind
		//sc_fifo<rpu_data_struct >          m_gen2chip_pkt_fifo(64);
		//sc_fifo<rpu_struct >          m_chip2gen_pkt_fifo(64);
		//sc_fifo<rpu_struct >          m_gen2chip_recycle_addr_fifo(64);


		//m_generate->sco_gen2chip_pkt(m_gen2chip_pkt_fifo);
		//m_generate->sci_chip2gen_pkt(m_chip2gen_pkt_fifo);
		//m_generate->chip_in_clk(chip_clk);
		//m_generate->sco_recycle_ring_addr(m_gen2chip_recycle_addr_fifo);

		//m_rpu->sci_gen2chip_pkt(m_gen2chip_pkt_fifo);
		//m_rpu->sco_chip2gen_pkt(m_chip2gen_pkt_fifo);
		//m_rpu->chip_in_clk(chip_clk);
		//m_rpu->sci_recycle_ring_addr(m_gen2chip_recycle_addr_fifo);


		//return total_simu_time;
	}
#else


#endif




    rpu::rpu(sc_module_name name,int bcu_num,int rcu_num):sc_module(name)
    {
        module_init(bcu_num,rcu_num);
        module_connect();

        //SC_METHOD(rcv_pkt);
        //sensitive << (chip_in_clk.pos());
        //dont_initialize();


        //SC_METHOD(send_pkt);
        //sensitive << (chip_in_clk.pos());
        //dont_initialize();

    }

    void rpu::module_init(int bcu_num ,int rcu_num)
    {
        m_input_channel_num = 32;
        m_total_parser_num = PKE_RSA_PARSER_NUM + PKE_SM2_PARSER_NUM + BUK_PARSER_NUM + 1;

        m_top_cfg = new top_cfg();
        m_top_cfg->bcu_num = bcu_num;
        m_top_cfg->rcu_num = rcu_num;

		m_rca_mod = new rca_mod("rca", 0, *m_top_cfg);

    }

    void rpu::module_connect()
    {
    
        //rca
        m_rca_mod->chip_clk(chip_in_clk);

        sci_gen2chip_pkt.resize(m_top_cfg->bcu_num);
        for (int i =0; i< m_top_cfg->bcu_num;++i)
        {
            sci_gen2chip_pkt[i] = new sc_fifo_in<rpu_data_struct>();
        }
        for (int i = 0; i < m_top_cfg->bcu_num; ++i)
        {
            (*m_rca_mod->sci_ahb2rca_cfg_data[i])(*sci_gen2chip_pkt[i]);
        }



    }

    void rpu::rcv_pkt()
    {
        //while (sci_gen2chip_pkt->num_available() > 0)
        //{
        //    req_ring_struct temp_data = sci_gen2chip_pkt->read();
        //    temp_data.time_stamp = sc_time_stamp().to_seconds()*1e9;
        //    m_pcie2ring_addr_req_fifo.nb_write(temp_data);
        //}

        //while (sci_recycle_ring_addr->num_available() > 0)
        //{
        //    req_ring_struct temp_data = sci_recycle_ring_addr->read();
        //    m_pcie2ring_addr_recycle_fifo.nb_write(temp_data);
        //}

    }
    void rpu::send_pkt()
    {
        //    while (m_cmd_mst2pcie_response_fifo.num_available() > 0)
        //    {
        //        req_ring_struct cmd_respnse = m_cmd_mst2pcie_response_fifo.read();
        //        sco_chip2gen_pkt->nb_write(cmd_respnse);
        //        //发送respongse
        //    }
        //
        //    while (m_data_mst2pcie_response_fifo.num_available() > 0)
        //    {
        //        req_ring_struct data_respnse = m_data_mst2pcie_response_fifo.read();
        //        //处理完的数据写回ddr
        //        //sco_chip2gen_pkt->nb_write(data_respnse);
        //
        //
        //    }
    }

    rpu::~rpu()
    {
        delete m_rca_mod;
        m_rca_mod = nullptr;
        delete m_top_cfg;
        m_top_cfg = nullptr;

        while (!sci_gen2chip_pkt.empty()) {
            delete sci_gen2chip_pkt.back(),sci_gen2chip_pkt.pop_back();
        }

        delete m_document;
        m_document = nullptr;
        while (!m_rca_xml_slv.empty()) {
            delete m_rca_xml_slv.back(),m_rca_xml_slv.pop_back();
        }
    }

	ESIM_RET rpu::get_xml_name( vector<string > xml_name,const string &pro_name)
	{
		//m_xml_name = xml_name;
		//xml名字需要通过IDE传递进来
		m_document = new tinyxml2::XMLDocument();
        m_rca_xml_slv.resize(xml_name.size());
        for (int i = 0; i< m_rca_xml_slv.size();++i)
        {
            m_rca_xml_slv[i] = new XmlResolver(xml_name.at(i));
        }


        m_rca_mod->get_xml_cfg(m_rca_xml_slv, pro_name);

		return RET_SUCCESS;
	}

	ESIM_RET rpu::get_mem_file_name(const string& mem_name)
	{
		m_mem_name = mem_name;
		ESIM_RET load_sucess = m_rca_mod->get_mem_file_name(m_mem_name);
		return load_sucess;
	}

	void rpu::get_rch_file_name(const string& rcg_name)
	{
		m_rch_name = rcg_name;
		m_rca_mod->get_rch_file_name(m_rch_name);
	}
	ESIM_RET rpu::get_imd_file_name(const string& imd_name)
	{
		m_imd_name = imd_name;
		ESIM_RET load_sucess = m_rca_mod->get_imd_file_name(m_imd_name);
		return load_sucess;

    }

    void rpu::set_source_map(const map<string, string> *source_map)
    {
        m_rca_mod->set_source_map(source_map);
    }

    rca_mod *rpu::getRca_mod() const
    {
        return m_rca_mod;
    }


}
