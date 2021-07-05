#include "rcu.h"
namespace rpu_esl_model
{
    rcu_mod::rcu_mod(sc_module_name name, top_cfg total_top_cfg, int rcu_id,  bcu_mod * mod_t) :sc_module(name), m_top_cfg(total_top_cfg)
    {
        //SC_METHOD(data_process_mth);
        //sensitive << chip_clk.pos();
        //dont_initialize();

		m_print_ctrl_num = 0;
        //m_ififo = ififo;
        //m_ofifo = ofifo;
		m_rcu_id = rcu_id;
        mod_init(mod_t);
		m_bcu_mod = mod_t;


		string rca_name = "rca";
		string rcu_name = "rcu";
		string a = "_debug.log";



		ostringstream oss;
		oss << rcu_name << rcu_id <<  a;
		//rcu_deug_log.open(oss.str());
    }

    void rcu_mod::mod_init(bcu_mod * mod_t)
    {
        m_bfu_mod.resize(m_top_cfg.bfu_num);
        for (int i = 0; i < m_top_cfg.bfu_num; ++i)
        {
            std::string str = "bfu_" + std::to_string(i);
            m_bfu_mod[i] = new bfu_mod(str.c_str(), m_top_cfg, m_rcu_id, i, mod_t);
            m_bfu_mod[i]->get_xml_resolver(m_rca_xml_slv);

            m_bfu_mod[i]->chip_clk(chip_clk);
        }

        m_sbox_mod.resize(m_top_cfg.sbox_num);
        for (int i = 0; i < m_top_cfg.sbox_num; ++i)
        {
            std::string str = "sbox_" + std::to_string(i);
			m_sbox_mod[i] = new sbox_mod(str.c_str(), m_top_cfg, m_rcu_id, i, mod_t);
            m_sbox_mod[i]->get_xml_resolver(m_rca_xml_slv);
            m_sbox_mod[i]->chip_clk(chip_clk);


        }
        m_benes_mod.resize(m_top_cfg.benes_num);
        for (int i = 0; i < m_top_cfg.benes_num; ++i)
        {
            std::string str = "benes_" + std::to_string(i);
			//sc_module_name str_name = str;
            m_benes_mod[i] = new benes_mod(str.c_str(), m_top_cfg, m_rcu_id, i, mod_t);
            m_benes_mod[i]->sci_chip_clk(chip_clk);

        }


		m_out_fifo_type.resize(4, -1);
		m_out_fifo_index.resize(4, -1);
		m_out_fifo_data.resize(4, -1);

    }

    void rcu_mod::data_process_mth()
    {

        stat_pro();
        print_info();

        m_cycle_cnt++;
    }


    void rcu_mod::stat_pro()
    {
        int stat_que_size = m_cmd_fifo.size();

        m_t_cmd_fifo_sum_size += stat_que_size;
        m_p_cmd_fifo_sum_size += stat_que_size;

        if (stat_que_size > m_t_cmd_fifo_max)
        {
            m_t_cmd_fifo_max = stat_que_size;
        }
        if (stat_que_size < m_t_cmd_fifo_min)
        {
            m_t_cmd_fifo_min = stat_que_size;
        }

        if (stat_que_size > m_p_cmd_fifo_max)
        {
            m_p_cmd_fifo_max = stat_que_size;
        }
        if (stat_que_size < m_p_cmd_fifo_min)
        {
            m_p_cmd_fifo_min = stat_que_size;
        }

    }
    void rcu_mod::print_info()
    {
        //第一个周期不统计
        //if (m_cycle_cnt >= m_top_cfg.print_period)
        //{
        //    if (0 == (m_cycle_cnt) % m_top_cfg.print_period)
        //    {
        //        rcu_log << "**********************************************************************************************************************" << endl;
        //        rcu_log << "curr_time : " << m_cycle_cnt * 2 / 1e3 << "US" << endl;
        //        rcu_log << "cmd_dis stat info :     (t_max|t_aver|t_min)        (p_max|p_aver|p_min)        " << endl;
        //        rcu_log << "cmd_fifo : (" << m_t_cmd_fifo_max << "|" << (m_t_cmd_fifo_sum_size / m_cycle_cnt) << "|" << m_t_cmd_fifo_min << ")"
        //            << "            (" << m_p_cmd_fifo_max << " | " << (m_p_cmd_fifo_sum_size / print_period) << " | " << m_p_cmd_fifo_min << ")" << endl;
        //        rcu_log << endl;
        //        //phase 初始化  total不变
        //        m_p_cmd_fifo_sum_size = 0;
        //        m_p_cmd_fifo_max = 0;
        //        m_p_cmd_fifo_min = CMD_FIFO_DEPTH;
        //    }
        //}

    }


    rcu_mod::~rcu_mod()
    {
        while (!m_bfu_mod.empty()) {
            delete m_bfu_mod.back(),m_bfu_mod.pop_back();
        }
        while (!m_sbox_mod.empty()) {
            delete m_sbox_mod.back(),m_sbox_mod.pop_back();
        }
        while (!m_benes_mod.empty()) {
            delete m_benes_mod.back(),m_benes_mod.pop_back();
        }
    }

    void rcu_mod::get_xml_cfg(XMLElement *rca_ele)
    {

        //if (rca_ele)
        //{
        //    //rca_ele->findBCUElement();
        //    XMLElement * bcu_ele = rca_ele->FirstChildElement();
        //    int bcu_id = atoi(bcu_ele->Attribute("id"));

        //    m_rcu_ele = bcu_ele->NextSiblingElement("RCU");

        //    if (NULL != m_rcu_ele)
        //    {           
        //        int rcu_id = atoi(m_rcu_ele->Attribute("id"));
        //        //第一个bfu
        //        //XMLElement * bfu_ele = rcu_ele->FirstChildElement("BFU");
        //        //if (bfu_ele)
        //        //{
        //        //    bfu_process(bfu_ele);
        //        //}
        //        while (NULL != m_rcu_ele->NextSiblingElement("BFU"))
        //        {
        //            XMLElement * bfu_ele = m_rcu_ele->NextSiblingElement("BFU");
        //            //bfu_process(bfu_ele);
        //            int bfu_id = atoi(bfu_ele->Attribute("id"));
        //            m_bfu_mod[bfu_id]->bfu_process();

        //        }

        //    }  
        //}

    }

    void rcu_mod::run_process(int bcu_id, int rcu_id, XMLElement * rcu_ele)
    {
//		int rcu_real_id = m_bcu_mod->m_rcu_order_id;

		m_print_ctrl_num = 0;
        //bfu算子
		for (int i = 0 ; i < 4; ++i)
		{
			m_bfu_ele = rcu_ele->FirstChildElement("BFU");
			if (NULL != m_bfu_ele)
			{
				int bfu_id = atoi(m_bfu_ele->Attribute("id"));
				m_bfu_mod[bfu_id]->run_process(bcu_id, rcu_id, bfu_id, m_bfu_ele, m_print_ctrl_num);


				while (NULL != m_bfu_ele->NextSiblingElement("BFU"))
				{
					m_bfu_ele = m_bfu_ele->NextSiblingElement("BFU");
					bfu_id = atoi(m_bfu_ele->Attribute("id"));
					m_bfu_mod[bfu_id]->run_process(bcu_id, rcu_id, bfu_id, m_bfu_ele, m_print_ctrl_num);

				}
			}


			//sbox
			m_sbox_ele = rcu_ele->FirstChildElement("SBOX");
			if (NULL != m_sbox_ele)
			{
				int sbox_id = atoi(m_sbox_ele->Attribute("id"));
				m_sbox_mod[sbox_id]->run_process(bcu_id, rcu_id, sbox_id, m_sbox_ele, m_print_ctrl_num);


				while (NULL != m_sbox_ele->NextSiblingElement("SBOX"))
				{
					m_sbox_ele = m_sbox_ele->NextSiblingElement("SBOX");
					sbox_id = atoi(m_sbox_ele->Attribute("id"));
					m_sbox_mod[sbox_id]->run_process(bcu_id, rcu_id, sbox_id, m_sbox_ele, m_print_ctrl_num);

				}
			}

			//benes
			m_benes_ele = rcu_ele->FirstChildElement("BENES");
			if (NULL != m_benes_ele)
			{
				int benes_id = atoi(m_benes_ele->Attribute("id"));
				m_benes_mod[benes_id]->run_process(bcu_id, rcu_id, benes_id, m_benes_ele, m_print_ctrl_num);

				while (NULL != m_benes_ele->NextSiblingElement("BENES"))
				{
					m_benes_ele = m_benes_ele->NextSiblingElement("BENES");
					benes_id = atoi(m_benes_ele->Attribute("id"));
					m_benes_mod[benes_id]->run_process(bcu_id, rcu_id, benes_id, m_benes_ele, m_print_ctrl_num);

				}
			}

			m_print_ctrl_num++;

		}

		// ofifo
		m_ofifo_ele = rcu_ele->FirstChildElement("OUT_FIFO");
		if (NULL != m_ofifo_ele)
		{
			int out_fifo_id = atoi(m_ofifo_ele->Attribute("id"));
			run_ofifo_process(bcu_id, rcu_id,out_fifo_id,m_ofifo_ele,0);


			while (NULL != m_ofifo_ele->NextSiblingElement("OUT_FIFO"))
			{
				m_ofifo_ele = m_ofifo_ele->NextSiblingElement("OUT_FIFO");
			    out_fifo_id = atoi(m_ofifo_ele->Attribute("id"));
				run_ofifo_process(bcu_id, rcu_id, out_fifo_id, m_ofifo_ele,0);

			}

		}


    }

	void rcu_mod::run_ofifo_process(int bcu_id, int rcu_id,int out_fifo_id ,XMLElement * m_ofifo_ele,int print_num)
	{
		if (0 == out_fifo_id)
		{
			m_out_fifo_type[out_fifo_id] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, ELEMENT_OUTFIFO, out_fifo_id, AttributeID::BCUOUTFIFO0_Type, 0);
			m_out_fifo_index[out_fifo_id] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, ELEMENT_OUTFIFO, out_fifo_id, AttributeID::BCUOUTFIFO0_Index, 0);
		}

		if (1 == out_fifo_id)
		{
			m_out_fifo_type[out_fifo_id] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, ELEMENT_OUTFIFO, out_fifo_id, AttributeID::BCUOUTFIFO1_Type, 0);
			m_out_fifo_index[out_fifo_id] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, ELEMENT_OUTFIFO, out_fifo_id, AttributeID::BCUOUTFIFO1_Index, 0);
		}

		if (2 == out_fifo_id)
		{
			m_out_fifo_type[out_fifo_id] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, ELEMENT_OUTFIFO, out_fifo_id, AttributeID::BCUOUTFIFO2_Type, 0);
			m_out_fifo_index[out_fifo_id] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, ELEMENT_OUTFIFO, out_fifo_id, AttributeID::BCUOUTFIFO2_Index, 0);
		}

		if (3 == out_fifo_id)
		{
			m_out_fifo_type[out_fifo_id] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, ELEMENT_OUTFIFO, out_fifo_id, AttributeID::BCUOUTFIFO3_Type, 0);
			m_out_fifo_index[out_fifo_id] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, ELEMENT_OUTFIFO, out_fifo_id, AttributeID::BCUOUTFIFO3_Index, 0);
		}

		//如何确定last_rcu_id ?index 和实际bfu编号的转换,map
		int rcu_order_id = m_bcu_mod->m_rcuid2orderid_map[rcu_id];
		int last_order_id = m_bcu_mod->m_rcuid2orderid_map[rcu_id] - 1;
		int last_rcu_id = 0;
		if (last_order_id > 0)
		{
			last_rcu_id = m_bcu_mod->m_orderid2rcuid_map[last_order_id];
		}
		int sbox_id_a = -1;
		int sbox_out_port_id_a = -1;
		int benes_id_a = -1;


		//for (int i = 0; i < 4; ++i)
		//{

			sbox_id_a = m_out_fifo_index[out_fifo_id] / SBOX_OUT_PORT_NUM;
			sbox_out_port_id_a = m_out_fifo_index[out_fifo_id] % SBOX_OUT_PORT_NUM;
			//benes_id 必为0 
			benes_id_a = m_out_fifo_index[out_fifo_id] / BENES_OUT_PORT_NUM;


			switch (m_out_fifo_type[out_fifo_id])
			{
			case InputPort_NULL:
				m_out_fifo_data[out_fifo_id] = -1;
				break;
			case InputPort_CurrentBfuX:
				//if (m_bcu_mod->m_rcu_mod[rcu_id]->m_bfu_mod[m_out_fifo_index[out_fifo_id]]->m_bfu_out_valid)
				//{
				m_out_fifo_data[out_fifo_id] = m_bcu_mod->m_rcu_mod[rcu_order_id]->m_bfu_mod[m_out_fifo_index[out_fifo_id]]->m_bfu_out[0];
				//}
				break;
			case InputPort_CurrentBfuY:
				//if (m_bcu_mod->m_rcu_mod[rcu_id]->m_bfu_mod[m_out_fifo_index[out_fifo_id]]->m_bfu_out_valid)
				//{
				m_out_fifo_data[out_fifo_id] = m_bcu_mod->m_rcu_mod[rcu_order_id]->m_bfu_mod[m_out_fifo_index[out_fifo_id]]->m_bfu_out[1];
				//}
				break;
			case InputPort_CurrentSbox:
				m_out_fifo_data[out_fifo_id] = m_bcu_mod->m_rcu_mod[rcu_order_id]->m_sbox_mod[sbox_id_a]->m_sbox_out[sbox_out_port_id_a];
				break;
			case InputPort_CurrentBenes:
				m_out_fifo_data[out_fifo_id] = m_bcu_mod->m_rcu_mod[rcu_order_id]->m_benes_mod[benes_id_a]->m_benes_out[m_out_fifo_index[out_fifo_id]];
				break;
			case InputPort_LastBfuX:
				m_out_fifo_data[out_fifo_id] = m_bcu_mod->m_rcu_mod[last_order_id]->m_bfu_mod[m_out_fifo_index[out_fifo_id]]->m_bfu_out[0];
				break;
			case InputPort_LastBfuY:
				m_out_fifo_data[out_fifo_id] = m_bcu_mod->m_rcu_mod[last_order_id]->m_bfu_mod[m_out_fifo_index[out_fifo_id]]->m_bfu_out[1];
				break;
			case InputPort_LastBenes:
				m_out_fifo_data[out_fifo_id] = m_bcu_mod->m_rcu_mod[last_order_id]->m_benes_mod[benes_id_a]->m_benes_out[m_out_fifo_index[out_fifo_id]];
				break;
			case InputPort_LastSbox:
				m_out_fifo_data[out_fifo_id] = m_bcu_mod->m_rcu_mod[last_order_id]->m_sbox_mod[sbox_id_a]->m_sbox_out[sbox_out_port_id_a];
				break;
			case InputPort_InFifo:
				////从input fifo中获取值，移步 bfu 里做还是rcu里做，需要考虑一下
				//if (m_bcu_mod->m_ififo->size() > 0)
				//{
				//	m_out_fifo_data[out_fifo_id] = m_bcu_mod->m_ififo->front().input_data;
				//	m_bcu_mod->m_is_used_imd = true;
				//	m_bcu_mod->m_is_used_infifo = true;
				//	m_bcu_mod->m_ififo->pop_front();
				//	//is_infifo_used = true;
				//}
				m_out_fifo_data[out_fifo_id] = m_bcu_mod->m_infifo_pre_read.at(m_out_fifo_index[out_fifo_id]);
				m_bcu_mod->m_is_used_infifo = true;
				break;
			case InputPort_Mem:
				//ElementType element = ELEMENT_OUTMEM;
				//int mem_offset = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, 0, AttributeID::RCUMEMRDOFFSET, 0);
				m_out_fifo_data[out_fifo_id] = m_bcu_mod->m_mem_mod->m_mem_ram[m_bcu_mod->m_mem_mod->m_mem_read_addr[m_out_fifo_index[out_fifo_id]]];
				break;				
			}
			//o_fifo_date.input_data = m_out_fifo_data[out_fifo_id];
			if (-1 != m_out_fifo_data[out_fifo_id])
			{
				m_bcu_mod->m_ofifo->push_back(m_out_fifo_data[out_fifo_id]);
				cout << "[OPERATOR OFIFO OUT ID :" << out_fifo_id << " ]"<<hex << m_out_fifo_data[out_fifo_id] << endl;
				//cout << hex << " ,a:" << a << ",b:" << b << ",t:" << t << ",";
				//cout << "x:" << out[0] << ",y:" << out[1] << dec << endl;
				m_bcu_mod->m_cout_out_log << "[OPERATOR OFIFO OUT ID:" << out_fifo_id << " ]"<<hex << m_out_fifo_data[out_fifo_id] << endl;
			}

	
	}


    void rcu_mod::get_xml_resolver(XmlResolver *xml_resolver)
    {
        m_rca_xml_slv = xml_resolver;

        for (int i = 0; i < m_top_cfg.bfu_num; ++i)
        {
            m_bfu_mod[i]->get_xml_resolver(m_rca_xml_slv);
        }

        for (int i = 0; i < m_top_cfg.sbox_num; ++i)
        {
            m_sbox_mod[i]->get_xml_resolver(m_rca_xml_slv);
        }

        for (int i = 0; i < m_top_cfg.benes_num; ++i)
        {
            m_benes_mod[i]->get_xml_resolver(m_rca_xml_slv);
        }


    }


    std::shared_ptr<vector<int>> rcu_mod::getOut_fifo_data()
	{
        return std::make_shared<std::vector<int>>(m_bcu_mod->m_ofifo->cbegin(),m_bcu_mod->m_ofifo->cend());
	}
}


