
#include "mem_mod.h"

namespace rpu_esl_model
{
	//************************************
	// Method:    cmd_ctrl
	// FullName:  cmd_ctrl::cmd_ctrl
	// Access:    public 
	// Returns:   
	// Qualifier: :sc_module(name), m_top_cfg(total_top_cfg)
	// Parameter: sc_module_name name
	// Parameter: top_cfg total_top_cfg
	//************************************
	mem_mod::mem_mod(sc_module_name name, top_cfg total_top_cfg, int mem_id, bcu_mod *mod_t) :sc_module(name), m_top_cfg(total_top_cfg)
	{
        SC_THREAD(data_process_mth);
        sensitive << sci_chip_clk.pos();
        dont_initialize();

		m_bcu_mod = mod_t;
		//m_rcu_id = rcu_id;
		m_mem_id = mem_id;
		mod_init(mem_id);
	}

	//************************************
	// Method:    mod_init
	// FullName:  cmd_ctrl::mod_init
	// Access:    public 
	// Returns:   void
	// Qualifier:
	//************************************
	void mem_mod::mod_init(int mem_id)
	{
		m_last_in_que.resize(MEM_OUT_PORT_NUM);
		m_last_in_que_latency.resize(MEM_OUT_PORT_NUM);

		m_write_mem_que.resize(MEM_OUT_PORT_NUM);

		m_mem_out_data.resize(MEM_OUT_PORT_NUM);
		m_last_mem_out_date.resize(MEM_OUT_PORT_NUM);

		m_in_date_valid.resize(MEM_OUT_PORT_NUM, 0);
		m_mem_write_addr_org_deque.resize(MEM_OUT_PORT_NUM);

		m_dest.resize(MEM_IN_PORT_NUM);

		m_need_update_read_addr = false;
		m_need_update_write_addr = false;
		m_intel_num_add = false;
		m_intel_read_offset = 0;
        m_intel_write_offset = 0;

		str0 = new char();
		str1 = new char();
		str2 = new char();
		str3 = new char();


		//m_last_in_que.clear();
		//m_ififo = m_bcu_mod->m_ififo;
		//m_ofifo = m_bcu_mod->m_ofifo;

		string rca_name = "rca";
		string rcu_name = "rcu";
		string mem_name = "_mem";
		string a = "_debug.log";

		ostringstream oss;
		oss << rcu_name << 0 << mem_name << mem_id << a;
		//m_mem_debug_log.open(oss.str());

	}

	//************************************
	// Method:    data_process_mth
	// FullName:  cmd_ctrl::data_process_mth
	// Access:    public 
	// Returns:   void
	// Qualifier:
	//************************************
    void mem_mod::data_process_mth()
    {
        t = sc_get_current_process_handle();

        while (true)
        {
            for (int i = 0; i < MEM_OUT_PORT_NUM; ++i)
            {
                if (m_write_mem_que[i].size() > 0)
                {
                    //m_mem_ram[m_mem_write_addr_org * 4 + i] = m_write_mem_que[i].front();
                    int mem_write_addr_org = 0;
                    if (m_mem_write_addr_org_deque[i].size() > 0)
                    {
                        mem_write_addr_org = m_mem_write_addr_org_deque[i].front();
                    }
                    m_mem_ram[mem_write_addr_org * 4 + i] = m_write_mem_que[i].front();
                    //if (m_bcu_mod->m_loop_mode[m_bcu_mod->m_orderid2rcuid_map[m_bcu_mod->m_rcu_order_id]] > 0)
                    //{
                        //m_mem_write_addr = m_mem_write_addr_org + m_mem_write_offset;

                    //阈值暂时没用
                    //if (m_mem_write_addr > m_mem_write_thd)
                    //{
                    //	m_mem_write_addr = m_mem_write_addr_org;
                    //}

                //}
                    m_mem_write_addr_org_deque[i].pop_front();
                    m_write_mem_que[i].pop_front();
                }
            }


            for (int i = 0; i < MEM_OUT_PORT_NUM; ++i)
            {
                if (m_last_in_que_latency[i].size() > 0)
                {
                    m_write_mem_que[i].push_back(m_last_in_que_latency[i].front());
                    m_last_in_que_latency[i].pop_front();
                }
            }

            if (m_bcu_mod->m_bcu_simu_end)
            {
                //t.resume();
                //sc_stop();
                //break;
            }


            if (m_bcu_mod->m_on_step_mode)
            {
                t.suspend();
                //sc_pause();
                //wait(m_bcu_mod->m_one_step_event);
                wait();
            }
            else
            {
                wait();
            }


        }
    
    }



	ESIM_RET mem_mod::get_mem_file_name(string mem_name)
	{
		//读取mem文件内容，并赋值给m_mem
		string line;
		int data_num = 0; 
		ifstream fin(mem_name.c_str());
		if (!fin.is_open()) {
			cout << "FILE open failed" << " file name :" << mem_name << endl;
            g_retMsg = string(u8"资源文件“") + mem_name + string(u8"”打开失败");
            return RET_CRITICAL;
		}

		char str[1024];
		//UINT32 idate[4];
		vector<int > in_date;
		in_date.resize(4, 0);
		int addr = 0;
		FILE *fp;
		bool valid_line = true;
		fp = fopen(mem_name.c_str(), "r");
		while (fgets(str, 1023, fp) != NULL)
		{
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
				sscanf(str, "%x %x %x %x %x\n", &addr, &in_date[0], &in_date[1], &in_date[2], &in_date[3]);
				m_mem_ram.push_back(in_date[0]);
				m_mem_ram.push_back(in_date[1]);
				m_mem_ram.push_back(in_date[2]);
				m_mem_ram.push_back(in_date[3]);
			}

	
		}

		while (m_mem_ram.size() < 256)
		{
			m_mem_ram.push_back(0);
		}

		fclose(fp);

		return RET_SUCCESS;
	}



	//************************************
	// Method:    rr_process
	// FullName:  cmd_ctrl::rr_process
	// Access:    public 
	// Returns:   void
	// Qualifier:
	//************************************
	void mem_mod::mem_run_process(int rcu_id)
	{
		for (int i = 0; i < MEM_OUT_PORT_NUM; i++)
		{
			if (m_last_in_que[i].size() > 0)
			{
				m_in_date_valid[i] = 1;

				if (-1 != m_last_in_que[i].front())
				{
					m_last_in_que_latency[i].push_back(m_last_in_que[i].front());
					//m_mem_ram[m_mem_write_addr * 4 + i] = m_last_in_que[i].front();
					//m_mem_debug_log << "write mem  addr =  " << (m_mem_write_addr * 4 + i) << " ,date = " << hex << m_last_in_que[i].front() << endl;
					m_last_in_que[i].pop_front();

					m_mem_write_addr_org_deque[i].push_back(m_mem_write_addr);

				}

			}
		}



	}


	//************************************
	// Method:    stat_pro
	// FullName:  cmd_ctrl::stat_pro
	// Access:    public 
	// Returns:   void
	// Qualifier:
	//************************************
	void mem_mod::stat_pro()
	{

	}
	//************************************
	// Method:    print_info
	// FullName:  cmd_ctrl::print_info
	// Access:    public 
	// Returns:   void
	// Qualifier:
	//************************************
	void mem_mod::print_info()
	{

	}

	//************************************
	// Method:    get_xml_cfg
	// FullName:  cmd_ctrl::get_xml_cfg
	// Access:    public 
	// Returns:   void
	// Qualifier:s
	// Parameter: XMLElement * zorov3_root
	//************************************
	void mem_mod::get_xml_cfg(int bcu_id, int rcu_id)
	{
		//inmem
		//<MEM_WRITE offset = "2" mask = "3" threashold = "0" addr = "16" mode = "1" / >

		ElementType element = ELEMENT_INMEM;
		m_mem_write_offset = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, 0, AttributeID::RCUMEMWTOFFSET, 0);
		m_mem_write_mode = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, 0, AttributeID::RCUMEMWTMODE, 0);
		m_mem_write_mask = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, 0, AttributeID::RCUMEMWTMASK, 0);
		m_mem_write_thd = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, 0, AttributeID::RCUMEMWTTHREASHOLD, 0);
		m_mem_write_addr = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, 0, AttributeID::RCUMEMWTADDR, 0);


		//outmem
		//<MEM_READ mode128_32 = "1" offset = "0" addr3 = "0" addr4 = "0" addr1 = "0" addr2 = "0" threashold = "0" mode = "0" / >
		m_mem_read_addr.resize(MEM_OUT_PORT_NUM);

		element = ELEMENT_OUTMEM;
		m_mem_read_offset = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, 0, AttributeID::RCUMEMRDOFFSET, 0);
		m_mem_read_mode = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, 0, AttributeID::RCUMEMRDMODE, 0);
		m_mem_read_128or32 = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, 0, AttributeID::RCUMEMRDMODE128_32, 0);
		m_mem_read_thd = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, 0, AttributeID::RCUMEMRDTHREASHOLD, 0);

		m_mem_read_addr[0] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, 0, AttributeID::RCUMEMRDADDR1, 0);
		m_mem_read_addr[1] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, 0, AttributeID::RCUMEMRDADDR2, 0);
		m_mem_read_addr[2] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, 0, AttributeID::RCUMEMRDADDR3, 0);
		m_mem_read_addr[3] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, 0, AttributeID::RCUMEMRDADDR4, 0);

		//int *a = dec2hex(m_mem_read_addr[0]);
		//int a = (atoi(_itoa(m_mem_read_addr[0], str0, 16)));
		//int b = (atoi(_itoa(m_mem_read_addr[1], str0, 16)));
		//int c = (atoi(_itoa(m_mem_read_addr[2], str0, 16)));
		//int d = (atoi(_itoa(m_mem_read_addr[3], str0, 16)));


		m_mem_read_addr_oring.resize(MEM_OUT_PORT_NUM);
		for (int i= 0 ; i < MEM_OUT_PORT_NUM ;++i)
		{
			m_mem_read_addr_oring[i] = m_mem_read_addr[i];
		}

	}

	void mem_mod::mem_read_addr_update()
	{
		if (0 == m_mem_read_mode)
		{
			//递减
			if (m_mem_read_offset == 0)
			{
				return;
			}

			for (int i = 0 ; i < 4 ;++i)
			{
				m_mem_read_addr[i] -= ( m_intel_read_offset);
				if (m_mem_read_addr[i]<0)
				{
					m_mem_read_addr[i] = 0;
				}
			}
	
		}
		else if (1 == m_mem_read_mode)
		{
			//递增
			m_mem_read_addr[0] = m_mem_read_addr[0]+  m_intel_read_offset;
			m_mem_read_addr[1] = m_mem_read_addr[1]+  m_intel_read_offset;
			m_mem_read_addr[2] = m_mem_read_addr[2]+  m_intel_read_offset;
			m_mem_read_addr[3] = m_mem_read_addr[3]+  m_intel_read_offset;
		}
	}


	void mem_mod::mem_write_addr_update()
	{
		if (0 == m_mem_write_mode)
		{
			////递减
			//m_mem_write_addr -= (m_mem_write_offset + m_intel_offset);
			//if (m_mem_write_addr < 0)
			//{
			//	m_mem_write_addr = 0;
			//}
		}
		else
		{
			m_mem_write_addr +=  m_intel_write_offset;
		}
	}



	void mem_mod::mem_read_addr_reset()
	{
		for (int i = 0; i < MEM_OUT_PORT_NUM; ++i)
		{
			m_mem_read_addr[i] = m_mem_read_addr_oring[i];
		}

	}


	mem_mod::~mem_mod()
	{
        delete str0;
        delete str1;
        delete str2;
        delete str3;
	}

	void mem_mod::run_process(int bcu_id, int rcu_id, int mem_id, XMLElement * mem_ele)
	{
		m_is_infifo_used = false;
		ElementType element = ElementType::ELEMENT_INMEM;

		int element_id = mem_id;

		vector<int> mem_input_type;
		vector<int> mem_input_index;
		mem_input_type.resize(MEM_OUT_PORT_NUM, -1);
		mem_input_index.resize(MEM_OUT_PORT_NUM, -1);

		mem_input_type[0] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::RCUMEM0_Type, 0);
		mem_input_index[0] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::RCUMEM0_Index, 0);
		mem_input_type[1] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::RCUMEM1_Type, 0);
		mem_input_index[1] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::RCUMEM1_Index, 0);
		mem_input_type[2] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::RCUMEM2_Type, 0);
		mem_input_index[2] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::RCUMEM2_Index, 0);
		mem_input_type[3] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::RCUMEM3_Type, 0);
		mem_input_index[3] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::RCUMEM3_Index, 0);


		//      const char * input_src = m_rca_xml_slv->getElementAttributeStr(bcu_id, rcu_id, element_id, AttributeID::BenesSrc);;
		//      vector<int> input_a;
		//      input_a.resize(BENES_OUT_PORT_NUM, -1);

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

		sbox_id_a = mem_input_index[mem_id] / SBOX_OUT_PORT_NUM;
		sbox_out_port_id_a = mem_input_index[mem_id] % SBOX_OUT_PORT_NUM;
		//benes_id 必为0 
		benes_id_a = mem_input_index[mem_id] / BENES_OUT_PORT_NUM;

		int input_a = -1;

		switch (mem_input_type[mem_id])
		{
		case InputPort_NULL:
			input_a = -1;
			//m_last_in_que[0].push_back(input_a);
			break;
		case InputPort_CurrentBfuX:
			//if (m_bcu_mod->m_rcu_mod[rcu_id]->m_bfu_mod[benes_input_index[i]]->m_bfu_out_valid)
			//{
			input_a = m_bcu_mod->m_rcu_mod[rcu_order_id]->m_bfu_mod[mem_input_index[mem_id]]->m_bfu_out[0];
			m_last_in_que[mem_id].push_back(input_a);
			break;
			//}
		case InputPort_CurrentBfuY:
			input_a = m_bcu_mod->m_rcu_mod[rcu_order_id]->m_bfu_mod[mem_input_index[mem_id]]->m_bfu_out[1];
			m_last_in_que[mem_id].push_back(input_a);
			break;
		case InputPort_CurrentSbox:
			input_a = m_bcu_mod->m_rcu_mod[rcu_order_id]->m_sbox_mod[sbox_id_a]->m_sbox_out[sbox_out_port_id_a];
			m_last_in_que[mem_id].push_back(input_a);
			break;
		case InputPort_CurrentBenes:
			input_a = m_bcu_mod->m_rcu_mod[rcu_order_id]->m_benes_mod[benes_id_a]->m_benes_out[mem_input_index[mem_id]];
			m_last_in_que[mem_id].push_back(input_a);
			break;
		case InputPort_LastBfuX:
			input_a = m_bcu_mod->m_rcu_mod[last_order_id]->m_bfu_mod[mem_input_index[mem_id]]->m_bfu_out[0];
			m_last_in_que[mem_id].push_back(input_a);
			break;
		case InputPort_LastBfuY:
			input_a = m_bcu_mod->m_rcu_mod[last_order_id]->m_bfu_mod[mem_input_index[mem_id]]->m_bfu_out[1];
			m_last_in_que[mem_id].push_back(input_a);
			break;
		case InputPort_LastBenes:
			input_a = m_bcu_mod->m_rcu_mod[last_order_id]->m_benes_mod[benes_id_a]->m_benes_out[mem_input_index[mem_id]];
			m_last_in_que[mem_id].push_back(input_a);
			break;
		case InputPort_LastSbox:
			input_a = m_bcu_mod->m_rcu_mod[last_order_id]->m_sbox_mod[sbox_id_a]->m_sbox_out[sbox_out_port_id_a];
			m_last_in_que[mem_id].push_back(input_a);
			break;
		case InputPort_InFifo:
			//从input fifo中获取值，移步 bfu 里做还是rcu里做，需要考虑一下
			//if (m_bcu_mod->m_ififo->size() > 0)
			//{
			//	input_a = m_bcu_mod->m_ififo->front().input_data;
			//	m_last_in_que[mem_id].push_back(input_a);
			//	m_bcu_mod->m_is_used_infifo = true;
			//	m_bcu_mod->m_ififo->pop_front();

			//}
			input_a = m_bcu_mod->m_infifo_pre_read.at(mem_input_index[mem_id]);
			m_last_in_que[mem_id].push_back(input_a);
			m_bcu_mod->m_is_used_infifo = true;

			break;
		case InputPort_Mem:
			//element = ELEMENT_OUTMEM;
			//int mem_offset = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, 0, AttributeID::RCUMEMRDOFFSET, 0);
//            input_a = m_bcu_mod->m_mem_mod->m_mem_ram[m_bcu_mod->m_rcu_mod[rcu_id]->m_mem_offset];
			//m_last_in_que[sreg_id].push_back(input_a);
			//if (0 == m_bcu_mod->m_rcu_mod[rcu_id]->m_mem_mod)
			//{
			//	m_bcu_mod->m_rcu_mod[rcu_id]->m_mem_offset;
			//}
			//else
			//{
			//	m_bcu_mod->m_rcu_mod[rcu_id]->m_mem_offset += m_bcu_mod->m_rcu_mod[rcu_id]->m_mem_step;
			//}
			break;
		case InputPort_Imd:
			input_a = m_bcu_mod->m_imd[mem_input_index[mem_id]];
			m_last_in_que[mem_id].push_back(input_a);
			m_bcu_mod->m_is_used_imd = true;
			break;
		case InputPort_Rch:
			input_a = m_bcu_mod->m_sreg_mod->m_sreg_out_data[mem_input_index[mem_id]];
			m_last_in_que[mem_id].push_back(input_a);
			break;
        case InputPort_Lfsr:
            if (m_bcu_mod->m_rcv_lfsr_rst_que.size() > 0)
            {
                input_a = m_bcu_mod->m_rcv_lfsr_rst_que.at(mem_input_index[mem_id]);
                m_last_in_que[mem_id].push_back(input_a);
                m_bcu_mod->m_is_used_lfsr = true;
            }
            break;
		}

	}

	void mem_mod::get_xml_resolver(XmlResolver *xml_resolver)
	{
		m_rca_xml_slv = xml_resolver;


	}

	char * mem_mod::del_char(char* str, char ch)
	{
		unsigned char i = 0, j = 0;
		while (str[i] != '\0')
		{
			if (str[i] != ch)//只有在bai不是空格的情况下目du标才会移动赋值
			{
				str[j++] = str[i];
			}
			i++;//源一zhi直移动

		}
		str[j] = '\0';
		return str;

	}


	int * mem_mod::dec2hex(int input_data)
	{
		int y, i;
		int d, n = 1;
		int * r;
		char h[16] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
		y = input_data;
		while (input_data >= 10)
		{
			input_data /= 10;
			++n;
		}
		r = new int[n];
		for (i = 0; y >= 16; ++i)
		{
			r[i] = y % 16;
			y /= 16;
		}
		r[i++] = y;

		for (i = (n - 1); i >= 0; --i)
		{
			cout << h[r[i]];
		}
		return r;
	}


}
