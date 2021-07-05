
#include "bcu.h"

namespace rpu_esl_model
{


bcu_mod::bcu_mod(sc_module_name name, top_cfg total_top_cfg,int bcu_id) :sc_module(name), m_top_cfg(total_top_cfg)
{
	SC_METHOD(data_process_mth);
    sensitive << chip_clk.pos();
	dont_initialize();

	SC_METHOD(notify_process_mth);
	sensitive << chip_clk.pos();
	dont_initialize();


	mod_init();
	m_bcu_id = bcu_id;
} 


void bcu_mod::mod_init()
{
    m_cycle_cnt = 0;
	m_cycle_cnt_notify = 0;
	m_on_step_mode = false;
    m_pc_addr = 0;
    m_jump_type = NOMARL;
    m_branch_jump_addr = 0;
    m_direct_jump_addr = 0;
    m_advanced_jump_addr = 0;
	m_first_point_flg = true;
	m_imd_mode = 0;
    //顶层传入
    print_period = m_top_cfg.print_period;
    m_cfg_ram.resize(CFG_RAM_TOTAL_ADDR);
    m_tcm.resize(TRAM_TOTAL_ADDR);
    pe_cfg_data.resize(PE_NUM);
    core_result.resize(PE_NUM);
    //core_bcu_log.open("core_bcu.log");
    m_ififo = new deque<rpu_data_struct>();
    m_ofifo = new deque<int >();
	m_loop_start_rcu_id = -1;
	m_loop_end_rcu_id = -1;
	m_loop_start_rcu_id_2 = -1;
	m_loop_end_rcu_id_2 = -1;

	m_rcu_order_id = 0;
	m_is_used_imd = false;
	m_is_used_infifo = false;
	m_bcu_simu_end = false;

	tinyxml2::XMLDocument *m_document;
	m_document = new tinyxml2::XMLDocument();

	//m_document->LoadFile(m_xml_name.c_str());
	//XMLElement * root = m_document->RootElement();
	m_rcu_ele = new XMLElement(m_document);

    m_cfg_ram.resize(CFG_RAM_NUM);
    for (int i = 0; i < CFG_RAM_NUM; ++i)
    {
        m_cfg_ram[i].resize(CFG_RAM_TOTAL_ADDR);
    }

	//1层循环
	m_loop_start.resize(m_top_cfg.rcu_num);
	m_loop_end.resize(m_top_cfg.rcu_num);
	m_loop_times.resize(m_top_cfg.rcu_num);
	m_loop_enable.resize(m_top_cfg.rcu_num,0);
	m_loop_mode.resize(m_top_cfg.rcu_num, 0);

	//二层循环
	m_loop_mode_2.resize(m_top_cfg.rcu_num, 0);
	m_loop_times_2.resize(m_top_cfg.rcu_num, 0);
	m_loop_start_2.resize(m_top_cfg.rcu_num,-1);
	m_loop_end_2.resize(m_top_cfg.rcu_num,-1);

	m_rcu_mod.resize(m_top_cfg.rcu_num);
	m_first_loop_flg.resize(m_top_cfg.rcu_num,1);
	m_in_loop_1.resize(m_top_cfg.rcu_num, 0);
    m_in_loop_2.resize(m_top_cfg.rcu_num, 0);
	m_mem.resize(256);
	m_infifo_pre_read.resize(16,0);

    for (int i = 0; i < m_top_cfg.rcu_num; ++i)
    {
        std::string str = "rcu_mod_" + std::to_string(i);
        m_rcu_mod[i] = new rcu_mod(str.c_str(),m_top_cfg, i,this);
        //m_rcu_mod[i]->get_xml_resolver(m_rca_xml_slv);
    }


    //bind
    for (int i = 0; i < m_top_cfg.rcu_num; ++i)
    {
        m_rcu_mod[i]->chip_clk(chip_clk);
    }

	m_sreg_mod = new sreg_mod("sreg", m_top_cfg, 0,this);
	m_sreg_mod->sci_chip_clk(chip_clk);

	m_mem_mod = new mem_mod("mem",m_top_cfg,0,this);
	m_mem_mod->sci_chip_clk(chip_clk);

	m_rcuid2orderid_map.resize(m_top_cfg.rcu_num);
	m_orderid2rcuid_map.resize(m_top_cfg.rcu_num);

	m_rcuxmlid2need_map.resize(m_top_cfg.rcu_num);;
	m_needid2rcuxml_map.resize(m_top_cfg.rcu_num);;


	string bcu_name = "bcu_0";
	string a = "_debug.log";
	ostringstream oss;
	oss << bcu_name  << a;
	m_cout_out_log.open(oss.str());
	oss.clear();
} 



void bcu_mod::rcv_data_pro()
{
    if (sci_ahb2bcu_cfg_data->num_available() > 0)
    {
        rpu_data_struct tmp_read_data = sci_ahb2bcu_cfg_data->read();
        if (m_ififo->size() <= 16 )
        {
            m_ififo->push_back(tmp_read_data);
        }
    }
    
}

void bcu_mod::notify_process_mth()
{
	if (0 == m_cycle_cnt_notify % 10)
	{
		//notify_debug_by_step_test();
	}

	m_cycle_cnt_notify++;
}



void bcu_mod::data_process_mth()
{
    rcv_data_pro();

	XMLElement * bcu_ele = m_rca_xml_slv->findBCUElement(m_bcu_id);
	if (NULL != bcu_ele && !m_bcu_simu_end)
	{
		run_process(m_bcu_id, bcu_ele);
	}

	if (m_bcu_simu_end)
	{
		sc_stop();
	}


	 m_cycle_cnt++;	
}

void bcu_mod::notify_debug_by_step_test()
{
	m_one_step_test.notify();
	//return RET_SUCCESS;
}

void bcu_mod::set_debug_by_step_mode(bool mode)
{
    m_on_step_mode = mode;
}


void bcu_mod::stat_pro()
{
    /*int stat_que_size = m_latency_que.size();

    m_t_latency_que_sum_size += stat_que_size;
    m_p_latency_que_sum_size += stat_que_size;



    if (stat_que_size > m_t_latency_que_max)
    {
        m_t_latency_que_max = stat_que_size;
    }
    if (stat_que_size < m_t_latency_que_min)
    {
        m_t_latency_que_min = stat_que_size;
    }

    if (stat_que_size > m_p_latency_que_max)
    {
        m_p_latency_que_max = stat_que_size;
    }
    if (stat_que_size < m_p_latency_que_min)
    {
        m_p_latency_que_min = stat_que_size;
    }*/

}
void bcu_mod::print_info()
{
    //第一个周期不统计
    //if (m_cycle_cnt >= print_period)
    //{
    //    if (0 == (m_cycle_cnt) % print_period)
    //    {
    //        core_bcu_log << "**********************************************************************************************************************" << endl;
    //        core_bcu_log << "curr_time : " << m_cycle_cnt * 2 / 1e3 << "us" << endl;
    //        core_bcu_log << "data_mst stat info :     (t_max|t_aver|t_min)        (p_max | p_aver | p_min)        " << endl;
    //        core_bcu_log << "latency_que : (" << m_t_latency_que_max << "|" << (m_t_latency_que_sum_size / m_cycle_cnt) << "|" << m_t_latency_que_min << ")"
    //            << "            (" << m_p_latency_que_max << " | " << (m_p_latency_que_sum_size / print_period) << " | " << m_p_latency_que_min << ")" << endl;
    //        core_bcu_log << endl;
    //        //phase 初始化  total不变
    //        m_p_latency_que_sum_size = 0;
    //        m_p_latency_que_max = 0;
    //        m_p_latency_que_min = data_mst_fifo_depth;
    //    }
    //}

}



bcu_mod::~bcu_mod()
{
	m_cout_out_log.close();

}

void bcu_mod::get_xml_cfg(XMLElement *rca_root)
{
    if (rca_root)
    {
        XMLElement *bcu_ele = rca_root->FirstChildElement("BCU");
		if (bcu_ele)
		{
			m_imd_mode = atoi(bcu_ele->Attribute("imd_mode128_32"));
		}
    }
}

void bcu_mod::get_xml_resolver(XmlResolver *xml_resolver, XMLElement *rca_ele)
{
    m_rca_xml_slv =xml_resolver;
	m_rca_ele = rca_ele;

	//m_mem_ele = m_rcu_ele->FirstChildElement("MEM_INPUT");
	//m_mem_mod->get_xml_cfg(bcu_id, rcu_id, m_mem_ele);

	m_sreg_mod->get_xml_resolver(m_rca_xml_slv);
	m_mem_mod->get_xml_resolver(m_rca_xml_slv);


    for (int i = 0; i < m_top_cfg.rcu_num; ++i)
    {
        m_rcu_mod[i]->get_xml_resolver(m_rca_xml_slv);
    }

	//做个rcu_id顺序排序和xml中顺序的映射map
    XMLElement * bcu_ele = m_rca_xml_slv->findBCUElement(m_bcu_id);
	XMLElement * rcu_ele = bcu_ele->FirstChildElement("RCU");
	int rcu_id = atoi(rcu_ele->Attribute("id"));
	m_need_id = 0;
	m_needid2rcuxml_map[m_need_id] = rcu_id;
	m_rcuxmlid2need_map[rcu_id] = m_need_id;

	while (rcu_ele->NextSiblingElement("RCU"))
	{
		m_need_id++;
		rcu_ele = rcu_ele->NextSiblingElement("RCU");
		rcu_id = atoi(rcu_ele->Attribute("id"));

		m_needid2rcuxml_map[m_need_id] = rcu_id;
		m_rcuxmlid2need_map[rcu_id] = m_need_id;

	}

}



void bcu_mod::set_bcu_cfg(bcu_cfg_data)
{

}


ESIM_RET bcu_mod::get_mem_file_name(string mem_name)
{
	ESIM_RET load_mem_xml_sucess = m_mem_mod->get_mem_file_name(mem_name);	;
	return load_mem_xml_sucess;
}


ESIM_RET bcu_mod::get_imd_file_name(string imd_name)
{
	m_imd_file_name = imd_name;

	//读取mem文件内容，并赋值给m_mem
	string line;
	int data_num = 0;
	const char* p = m_imd_file_name.c_str();
	ifstream fin(p);
	if (!fin.is_open()) {
		cout << "FILE open failed" << " file name :" << imd_name << endl;
        g_retMsg = string(u8"Resource file: ") + imd_name + u8" open failed";
        return RET_CRITICAL;
	}

	char str[1024];
	//UINT32 idate[4];
	in_date.resize(4, 0);
	int addr = 0;
	FILE *fp;
	fp = fopen(m_imd_file_name.c_str(), "r");
	bool valid_line = true;
	while (fgets(str, 1023, fp) != NULL)
	{
		//sscanf(str, "%x %x %x %x %x\n", &addr, &in_date[0], &in_date[1], &in_date[2], &in_date[3]);
		//if (0 != in_date[0])
		//{
		//	m_imd.push_back(in_date[0]);
		//	m_imd.push_back(in_date[1]);
		//	m_imd.push_back(in_date[2]);
		//	m_imd.push_back(in_date[3]);
		//}
		valid_line = true;
		for (int i = 0; i < strlen(str); ++i)
		{
			//if ("#" == str[i])
			if (35 == str[i])
			{
				valid_line = false;
				break;
			}
		}
		if (valid_line)
		{
			sscanf(str, "%x %x\n", &addr, &in_date[0]);
			m_imd.push_back(in_date[0]);
		}
	}

	fclose(fp);
    return RET_SUCCESS;
}



void bcu_mod::run_process(int bcu_id, XMLElement * bcu_ele)
{
	if (m_cycle_cnt < 5000)
	{
		return;
	}
	m_loop_real_rcu_id = 0;
	m_is_used_imd = false;
	m_is_used_infifo = false;

	//infifo预读
	int pre_read_size = 4;
	if (m_ififo->size()<4)
	{
		pre_read_size = m_ififo->size();
	}
	for (int i = 0 ; i < pre_read_size;++i)
	{
		m_infifo_pre_read[i] = m_ififo->at(i).input_data;
	}

	//RCU算核启动
	if (m_first_point_flg)
	{
		m_rcu_ele = m_rca_xml_slv->findRCUElement(m_bcu_id,0);
		//m_rcu_ele = bcu_ele->FirstChildElement("RCU");
	}

	if (NULL != m_rcu_ele)
	{
        int rcu_id = atoi(m_rcu_ele->Attribute("id"));

        m_mem_ele = m_rcu_ele->FirstChildElement("MEM_INPUT");
        //区分多个循环和单个循环的条件判断不一样
        //if (m_mem_mod->m_need_read_cfg)
        //{
        m_mem_mod->get_xml_cfg(bcu_id, rcu_id);

        if (m_mem_mod->m_need_update_read_addr)
        {
            m_mem_mod->mem_read_addr_update();
            m_mem_mod->mem_write_addr_update();
        }

        //下一拍更新，in_loop
        if (m_loop_start_rcu_id >= 0)
        {
            if (m_loop_times[m_loop_start_rcu_id] <= 1 && (m_loop_end_rcu_id == rcu_id))
            {
                //循环结束清0
                m_mem_mod->m_intel_read_offset = 0;
                m_mem_mod->m_intel_write_offset = 0;
            }
        }

        //m_rcu_mod[rcu_id]->run_process(bcu_id, rcu_id, m_rcu_ele);
        //bfu sbox benes ofifo

        //完成映射表生成，给后续选择上一行算子id使用
        m_orderid2rcuid_map[m_rcu_order_id] = rcu_id;
        m_rcuid2orderid_map[rcu_id] = m_rcu_order_id;
        //需要先更新映射关系再计算rcu
        m_rcu_mod[m_rcu_order_id]->run_process(bcu_id, rcu_id, m_rcu_ele);

        //sreg
        m_sreg_ele = m_rcu_ele->FirstChildElement("RCH_INPUT");
        if (NULL != m_sreg_ele)
        {
            m_sreg_mod->get_xml_cfg(bcu_id, rcu_id, m_sreg_ele);
            int sreg_id = atoi(m_sreg_ele->Attribute("id"));
            m_sreg_mod->run_process(bcu_id, rcu_id, sreg_id, m_sreg_ele);


            while (NULL != m_sreg_ele->NextSiblingElement("RCH_INPUT"))
            {
                m_sreg_ele = m_sreg_ele->NextSiblingElement("RCH_INPUT");
                sreg_id = atoi(m_sreg_ele->Attribute("id"));
                m_sreg_mod->run_process(bcu_id, rcu_id, sreg_id, m_sreg_ele);

            }
            m_sreg_mod->sreg_run_process();
        }

        //mem_cfg

        if (NULL != m_mem_ele)
        {
            int mem_id = atoi(m_mem_ele->Attribute("id"));
            m_mem_mod->run_process(bcu_id, rcu_id, mem_id, m_mem_ele);


            while (NULL != m_mem_ele->NextSiblingElement("MEM_INPUT"))
            {
                m_mem_ele = m_mem_ele->NextSiblingElement("MEM_INPUT");
                mem_id = atoi(m_mem_ele->Attribute("id"));
                m_mem_mod->run_process(bcu_id, rcu_id, mem_id, m_mem_ele);


            }
        }

        m_mem_mod->mem_run_process(rcu_id);


		//loop判断，1，2次循环
        // -------------------------------------
		if (m_first_loop_flg[rcu_id] == 1)
		{
			m_loop_ele = m_rcu_ele->FirstChildElement("LOOP");
			if (NULL == m_loop_ele)
			{
				if (NULL != m_rcu_ele->NextSiblingElement("RCU"))
				{
					m_rcu_ele = m_rcu_ele->NextSiblingElement("RCU");
					m_first_point_flg = false;
					//rcu_id = atoi(m_rcu_ele->Attribute("id"));
				}
				return;
			}

			//外层循环
			if (NULL != m_loop_ele->Attribute("LOOP_start_end2"))
			{
				m_loop_mode_2[rcu_id] = atoi(m_loop_ele->Attribute("LOOP_start_end2"));
				m_loop_times_2[rcu_id] = atoi(m_loop_ele->Attribute("LOOP_times2"));
			}

			//内层循环
			m_loop_mode[rcu_id] = atoi(m_loop_ele->Attribute("LOOP_start_end"));
			m_loop_times[rcu_id] = atoi(m_loop_ele->Attribute("LOOP_times"));

			m_first_loop_flg[rcu_id] = 0;

			//外层循环参数置位
			if (m_loop_mode_2[rcu_id] > 0)
			{
				m_in_loop_2[rcu_id] = 1;
				if (m_loop_mode_2[rcu_id] == 1)
				{
					m_loop_start_2[rcu_id] = 1;
					m_loop_end_2[rcu_id] = 0;
					m_loop_start_rcu_id_2 = rcu_id;
				}
				else if (m_loop_mode_2[rcu_id] == 2)
				{
					m_loop_start_2[rcu_id] = 0;
					m_loop_end_2[rcu_id] = 1;
					m_loop_end_rcu_id_2 = rcu_id;
				}
				else if (m_loop_mode_2[rcu_id] == 3)
				{
					m_loop_start_rcu_id_2 = rcu_id;
					m_loop_end_rcu_id_2 = rcu_id;

				}
				else
				{
				}
			}

            //内层循环参数置位
			//if (m_loop_times[rcu_id] > 1 && m_loop_mode[rcu_id] > 0)
			if (m_loop_mode[rcu_id] > 0)
			{
				m_in_loop_1[rcu_id] = 1;
				if (m_loop_mode[rcu_id] == 1)
				{
					m_loop_start[rcu_id] = 1;
					m_loop_end[rcu_id] = 0;
					m_loop_start_rcu_id = rcu_id;
				}
				else if (m_loop_mode[rcu_id] == 2)
				{
					m_loop_start[rcu_id] = 0;
					m_loop_end[rcu_id] = 1;
					m_loop_end_rcu_id = rcu_id;
				}
				else if (m_loop_mode[rcu_id] == 3)
				{
					//下一个RCU不变
					//m_rcu_ele = m_rcu_ele;
					m_loop_start_rcu_id = rcu_id;
					m_loop_end_rcu_id = rcu_id;

				}
				else
				{
				}
			}
		
		}

        if (m_loop_start_rcu_id_2 > 0 && m_loop_start_rcu_id > 0)
        {
            if (1 == m_in_loop_2[m_loop_start_rcu_id_2] && (m_in_loop_1[m_loop_start_rcu_id] == 0) )
            {
                //继续判断内层状态提取
                m_loop_ele = m_rcu_ele->FirstChildElement("LOOP");
                if (NULL != m_loop_ele)
                {
                    if (NULL != m_loop_ele->Attribute("LOOP_start_end"))
                    {
                        m_loop_mode[rcu_id] = atoi(m_loop_ele->Attribute("LOOP_start_end"));
                        m_loop_times[rcu_id] = atoi(m_loop_ele->Attribute("LOOP_times"));
                    }
                    //if (m_loop_times[rcu_id] > 1 && m_loop_mode[rcu_id] > 0)
                    if (m_loop_mode[rcu_id] > 0)
                    {
                        m_in_loop_1[rcu_id] = 1;
                        if (m_loop_mode[rcu_id] == 1)
                        {
                            m_loop_start[rcu_id] = 1;
                            m_loop_end[rcu_id] = 0;
                            m_loop_start_rcu_id = rcu_id;
                        }
                        else if (m_loop_mode[rcu_id] == 2)
                        {
                            m_loop_start[rcu_id] = 0;
                            m_loop_end[rcu_id] = 1;
                            m_loop_end_rcu_id = rcu_id;
                        }
                        else if (m_loop_mode[rcu_id] == 3)
                        {
                            //下一个RCU不变
                            //m_rcu_ele = m_rcu_ele;
                            m_loop_start_rcu_id = rcu_id;
                            m_loop_end_rcu_id = rcu_id;

                        }
                        else
                        {
                        }
                    }

                }
               
            }
 

        }



        //if (m_loop_start_rcu_id >= 0)
        //{
        //    if (m_loop_times[m_loop_start_rcu_id] <= 1 && (m_loop_end_rcu_id == rcu_id))
        //    {
        //        m_in_loop_1[m_loop_start_rcu_id] = 0;
        //    }
        //}
   

		//只有内层循环
		if (m_loop_mode_2[rcu_id] == 0)
		{
			//确定了start和end以后，开始循环加载RCU ，更新loop_time 和对应的RCU_id
			if (m_loop_end_rcu_id == rcu_id && (m_loop_times[m_loop_start_rcu_id] > 1))
			{
				XMLElement *  tmp_ele;
				if (NULL != m_rca_xml_slv->findRCUElement(m_bcu_id, m_loop_start_rcu_id))
				{
					tmp_ele = m_rca_xml_slv->findRCUElement(m_bcu_id, m_loop_start_rcu_id);
					m_rcu_ele = tmp_ele;
				}
				else
				{
					cout << "xml cfg erro " << endl;
				}

				m_loop_times[m_loop_start_rcu_id]--;

				m_mem_mod->m_need_update_read_addr = true;
				m_mem_mod->m_need_update_write_addr = true;
				m_mem_mod->m_intel_num_add = true;

				m_mem_mod->m_intel_read_offset += m_mem_mod->m_mem_read_offset;
				m_mem_mod->m_intel_write_offset += m_mem_mod->m_mem_write_offset;


			}
			else
			{
				m_rcu_ele = m_rca_xml_slv->findRCUElement(m_bcu_id, (rcu_id + 1));

				if (NULL == m_rcu_ele)
				{
					m_bcu_simu_end = true;
				}

				//第一次和跳出循环不用更新
				m_mem_mod->m_need_update_read_addr = false;
				m_mem_mod->m_need_update_write_addr = false;

				if (m_mem_mod->m_intel_num_add && (m_loop_times[m_loop_start_rcu_id] >= 1))
				{
					m_mem_mod->m_need_update_read_addr = true;
					m_mem_mod->m_need_update_write_addr = true;
				}


				if (m_loop_end_rcu_id == rcu_id && (m_loop_times[m_loop_start_rcu_id] == 1))
				{
					m_mem_mod->m_intel_num_add = false;
                    m_in_loop_1[m_loop_start_rcu_id] = 0;
				}
			}

		}
		//存在外层循环
		else
		{
			//进一步判断是否存在内层循环
			//存在内层循环
			//if (m_loop_mode[rcu_id]== 1 || m_loop_mode[rcu_id] == 3)
			if (m_loop_mode[rcu_id]> 0 )
			{
				if (m_loop_end_rcu_id == rcu_id && (m_loop_times[m_loop_start_rcu_id] > 1))
				{
					XMLElement *  tmp_ele;
					if (NULL != m_rca_xml_slv->findRCUElement(m_bcu_id, m_loop_start_rcu_id))
					{
						tmp_ele = m_rca_xml_slv->findRCUElement(m_bcu_id, m_loop_start_rcu_id);
						m_rcu_ele = tmp_ele;
					}
					else
					{
						cout << "xml cfg erro " << endl;
					}

					m_loop_times[m_loop_start_rcu_id]--;

					m_mem_mod->m_need_update_read_addr = true;
					m_mem_mod->m_need_update_write_addr = true;
					m_mem_mod->m_intel_num_add = true;

					m_mem_mod->m_intel_read_offset += m_mem_mod->m_mem_read_offset;
					m_mem_mod->m_intel_write_offset += m_mem_mod->m_mem_write_offset;


				}
				else
				{
					m_rcu_ele = m_rca_xml_slv->findRCUElement(m_bcu_id, (rcu_id + 1));

					if (NULL == m_rcu_ele)
					{
						m_bcu_simu_end = true;
					}

					//第一次和跳出循环不用更新
					m_mem_mod->m_need_update_read_addr = false;
					m_mem_mod->m_need_update_write_addr = false;

					if (m_mem_mod->m_intel_num_add && (m_loop_times[m_loop_start_rcu_id] >= 1))
					{
						m_mem_mod->m_need_update_read_addr = true;
						m_mem_mod->m_need_update_write_addr = true;
					}


					if (m_loop_end_rcu_id == rcu_id && (1== m_loop_times[m_loop_start_rcu_id]))
					{
						m_mem_mod->m_intel_num_add = false;
                        m_in_loop_1[m_loop_start_rcu_id] = 0;
					}
				}

				//退出内存循环后判断外层循环状态
				if (m_loop_end_rcu_id == rcu_id && (1 == m_loop_times[m_loop_start_rcu_id]))
				{
					if ((m_loop_end_rcu_id_2 == rcu_id) && (m_loop_times_2[m_loop_start_rcu_id_2] > 1))
					{
						//跳回外层循环起点
						XMLElement *  tmp_ele;
						if (NULL != m_rca_xml_slv->findRCUElement(m_bcu_id, m_loop_start_rcu_id_2))
						{
							tmp_ele = m_rca_xml_slv->findRCUElement(m_bcu_id, m_loop_start_rcu_id_2);
							m_rcu_ele = tmp_ele;
						}
						else
						{
							cout << "xml cfg erro " << endl;
						}
						m_loop_times_2[m_loop_start_rcu_id_2]--;

					}
					else
					{
						//顺序执行下一次重构
						m_rcu_ele = m_rca_xml_slv->findRCUElement(m_bcu_id, (rcu_id + 1));

                        if (m_loop_times_2[m_loop_start_rcu_id_2] == 1 && (m_loop_end_rcu_id_2 == rcu_id))
                        {
                            m_in_loop_2[m_loop_start_rcu_id_2] = 0;
                        }

						if (NULL == m_rcu_ele)
						{
							m_bcu_simu_end = true;
						}
					}

				}
				
			}
			//不存在内层循环，只有外层循环
			else 
			{
				if ((m_loop_end_rcu_id_2 == rcu_id) && (m_loop_times_2[m_loop_start_rcu_id_2] > 1))
				{
					//跳回外层循环起点
					XMLElement *  tmp_ele;
					if (NULL != m_rca_xml_slv->findRCUElement(m_bcu_id, m_loop_start_rcu_id_2))
					{
						tmp_ele = m_rca_xml_slv->findRCUElement(m_bcu_id, m_loop_start_rcu_id_2);
						m_rcu_ele = tmp_ele;
					}
					else
					{
						cout << "xml cfg erro " << endl;
					}
					m_loop_times_2[m_loop_start_rcu_id_2]--;

				}
				else
				{
					//顺序执行下一次重构
					m_rcu_ele = m_rca_xml_slv->findRCUElement(m_bcu_id, (rcu_id + 1));


                    if (m_loop_times_2[m_loop_start_rcu_id_2] == 1 && (m_loop_end_rcu_id_2 == rcu_id))
                    {
                        m_in_loop_2[m_loop_start_rcu_id_2] = 0;
                    }

					if (NULL == m_rcu_ele)
					{
						m_bcu_simu_end = true;
					}
				}

			}
		
		}



		m_first_point_flg = false;
  
		m_loop_real_rcu_id++;
		m_rcu_order_id++;

		if (m_is_used_infifo)
		{
			for (int i= 0;i< pre_read_size;++i)
			{
				m_ififo->pop_front();
			}
			m_is_used_infifo = false;
		}

		if (m_is_used_imd)
		{
			if (0 == m_imd_mode)
			{
				for (int i = 0; i < IMD_OUT_PORT_NUM; ++i)
				{
					//128bit模式
					U32 pop_date = m_imd.front();
					m_imd.pop_front();
					m_imd.push_back(pop_date);
				}
			}
			else
			{
				//32bit模式
				U32 pop_date = m_imd.front();
				m_imd.pop_front();
				m_imd.push_back(pop_date);
			}
		}
	}
	if (m_on_step_mode)
	{
		next_trigger(m_one_step_test);
	}

}
	void bcu_mod::loop_process(XMLElement *m_rcu_ele,int rcu_id)
	{
		//loop判断
		//<LOOP LOOP_times = "26" LOOP_start_end = "3" / >
		if (m_first_loop_flg[rcu_id] == 1)
		{
			m_loop_ele = m_rcu_ele->FirstChildElement("LOOP");
			if (NULL == m_loop_ele)
			{
				if (NULL != m_rcu_ele->NextSiblingElement("RCU"))
				{
					m_rcu_ele = m_rcu_ele->NextSiblingElement("RCU");
					m_first_point_flg = false;
					//rcu_id = atoi(m_rcu_ele->Attribute("id"));
				}
				return;
			}
			m_loop_mode[rcu_id] = atoi(m_loop_ele->Attribute("LOOP_start_end"));
			m_loop_times[rcu_id] = atoi(m_loop_ele->Attribute("LOOP_times"));
	
			m_first_loop_flg[rcu_id] = 0;

			if (m_loop_times[rcu_id] > 1 && m_loop_mode[rcu_id]>0)
			{
				m_in_loop_1[rcu_id] = 1;
				if (m_loop_mode[rcu_id] == 1)
				{
					m_loop_start[rcu_id] = 1;
					m_loop_end[rcu_id] = 0;
					m_loop_start_rcu_id = rcu_id;
				}
				else if (m_loop_mode[rcu_id] == 2)
				{
					m_loop_start[rcu_id] = 0;
					m_loop_end[rcu_id] = 1;
					m_loop_end_rcu_id = rcu_id;
				}
				else if (m_loop_mode[rcu_id] == 3)
				{
					//下一个RCU不变
					//m_rcu_ele = m_rcu_ele;
					m_loop_start_rcu_id = rcu_id;
					m_loop_end_rcu_id = rcu_id;

				}
				else
				{
				}
			}
		}
		
		//确定了start和end以后，开始循环加载RCU ，更新loop_time 和对应的RCU_id
		if (m_loop_end_rcu_id == rcu_id && (m_loop_times[m_loop_start_rcu_id] > 1))
		{
			XMLElement *  tmp_ele;
			if (NULL != m_rca_xml_slv->findRCUElement(m_bcu_id,m_loop_start_rcu_id))
			{
				tmp_ele = m_rca_xml_slv->findRCUElement(m_bcu_id, m_loop_start_rcu_id);
				m_rcu_ele = tmp_ele;
			}
			else
			{
				cout << "xml cfg erro " << endl;
			}
			m_loop_times[m_loop_start_rcu_id]--;

			//if (m_loop_times[m_loop_start_rcu_id] <= 1)
			//{
			//	m_in_loop[m_loop_start_rcu_id] = 0;
			//}

		}
		else
		{
			if (NULL != m_rcu_ele->NextSiblingElement("RCU"))
			{
				m_rcu_ele = m_rcu_ele->NextSiblingElement("RCU");
				int tmp_rcu_id = atoi(m_rcu_ele->Attribute("id"));
				int a = 0;
			}
		}

		int tmp_rcu_id = atoi(m_rcu_ele->Attribute("id"));

		m_first_point_flg = false;

	}

	vector<rcu_mod *>* bcu_mod::getRcu_mod()
	{
        return &m_rcu_mod;
    }

    const map<string, string> *bcu_mod::get_source_map() const
    {
        return m_source_map;
    }

    void bcu_mod::set_source_map(const map<string, string> *source_map)
    {
        m_source_map = source_map;
    }

	int* bcu_mod::get_lastorder_id()
	{
		return &m_rcu_order_id;
	}

	vector<int> *bcu_mod::get_mem_data()
	{
		return &(m_mem_mod->m_mem_ram);
	}


}
