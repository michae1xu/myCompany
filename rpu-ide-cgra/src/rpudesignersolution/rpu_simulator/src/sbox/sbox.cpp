#include "sbox.h"
namespace rpu_esl_model
{


    //************************************
    // Method:    cmd_mst
    // FullName:  cmd_mst::cmd_mst
    // Access:    public 
    // Returns:   
    // Qualifier: :sc_module(name), m_top_cfg(total_top_cfg)
    // Parameter: sc_module_name name
    // Parameter: top_cfg total_top_cfg
    //************************************
    sbox_mod::sbox_mod(sc_module_name name, top_cfg total_top_cfg, int rcu_id, int sbox_id, bcu_mod *mod_t) :sc_module(name), m_top_cfg(total_top_cfg)
    {
        SC_METHOD(data_process_mth);
        sensitive << chip_clk.pos();
        dont_initialize();

        m_bcu_mod = mod_t;
        mod_init(rcu_id, sbox_id);
    }

    //************************************
    // Method:    mod_init
    // FullName:  cmd_mst::mod_init
    // Access:    public 
    // Returns:   void
    // Qualifier:
    //************************************
    void sbox_mod::mod_init(int rcu_id, int sbox_id)
    {
        m_cycle_cnt = 0;
        //m_cmd_mst_ready_status = true;
        m_t_latency_que_sum_size = 0;
        m_t_latency_que_max = 0;
        m_t_latency_que_min = CMD_MST_FIFO_DEPTH;

        m_p_latency_que_sum_size = 0;
        m_p_latency_que_max = 0;
        m_p_latency_que_min = CMD_MST_FIFO_DEPTH;
        m_mode == SINGLE_MOD;
        m_outbuffer.resize(BCU_NUM);
        m_inbuffer.resize(BCU_NUM);
        //sco_crssbar2bcu_data.resize(BCU_NUM);
        //sci_bcu2crsbar_req_data.resize(BCU_NUM);
        //sci_bcu2crsbar_res_data.resize(BCU_NUM);
        m_core2crsbar_legth.resize(BCU_NUM);
        for (int i = 0; i < BCU_NUM; ++i)
        {
            m_outbuffer[i] = new deque<rpu_data_struct>();
            m_outbuffer[i]->clear();
            m_inbuffer[i] = new deque<rpu_data_struct>();
            m_inbuffer[i]->clear();

        }
        m_outbuff_rr_ptr = 0;

        m_sbox_out.resize(SBOX_OUT_PORT_NUM, -1);
        m_sbox_last_out.resize(SBOX_OUT_PORT_NUM, -1);
        m_ififo = m_bcu_mod->m_ififo;
        m_ofifo = m_bcu_mod->m_ofifo;

        print_period = m_top_cfg.print_period;

		m_sbox_out_valid = false;

		m_rcu_id = rcu_id;
		m_sbox_id = sbox_id;
        m_sbox_in_date = 0;

		string rca_name = "rca";
		string rcu_name = "rcu";
		string bfu_name = "_sbox";
		string a = "_debug.log";

        ostringstream oss;
        oss << rcu_name << rcu_id << bfu_name << sbox_id << a;
        //m_sbox_debug_log.open(oss.str());
    }

    //************************************
    // Method:    data_process_mth
    // FullName:  cmd_mst::data_process_mth
    // Access:    public 
    // Returns:   void
    // Qualifier:
    //************************************
    void sbox_mod::data_process_mth()
    {

    }

    void sbox_mod::stat_pro()
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
    void sbox_mod::print_info()
    {
        //第一个周期不统计
        if (m_cycle_cnt >= print_period)
        {
            if (0 == (m_cycle_cnt) % print_period)
            {
                //cmd_mst_log << "**********************************************************************************************************************" << endl;
                //cmd_mst_log << "curr_time : " << m_cycle_cnt * 2 / 1e3 << "US" << endl;
                //cmd_mst_log << "cmd_mst stat info :     (total_max | total_aver | total_min)        (phase_max | phase_aver | phase_min)        " << endl;
                //cmd_mst_log << "latency_que : (" << m_t_latency_que_max << "|" << (m_t_latency_que_sum_size / m_cycle_cnt) << "|" << m_t_latency_que_min << ")"
                //    << "            (" << m_p_latency_que_max << " | " << (m_p_latency_que_sum_size / print_period) << " | " << m_p_latency_que_min << ")" << endl;
                //cmd_mst_log << endl;
                //phase 初始化  total不变
                m_p_latency_que_sum_size = 0;
                m_p_latency_que_max = 0;
                m_p_latency_que_min = CMD_MST_FIFO_DEPTH;
            }
        }

    }


    sbox_mod::~sbox_mod()
    {
        while (!m_inbuffer.empty()) {
            delete m_inbuffer.back(),m_inbuffer.pop_back();
        }
        while (!m_outbuffer.empty()) {
            delete m_outbuffer.back(),m_outbuffer.pop_back();
        }
    }

    void sbox_mod::get_xml_cfg(XMLElement *root_elel)
    {
        if (root_elel)
        {
            XMLElement *cmd_mst_ele = root_elel->FirstChildElement("cmd_mst");
        }
    }



    void sbox_mod::run_process(int bcu_id, int rcu_id, int sbox_id, XMLElement * sbox_ele,int print_num)
    {
        bool is_infifo_used = false;


        ElementType element = ElementType::ELEMENT_SBOX;
        int element_id = sbox_id;
        int input_a_type = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::SboxInput_Type, 0);
        int input_a_index = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::SboxInput_Index, 0);
        int sbox_mod = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::SboxMode, 0);
        const char * input_src = m_rca_xml_slv->getElementAttributeStr(bcu_id, rcu_id, element_id, AttributeID::SboxSrc);

		int id_0 = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::SboxCNT0, 0);
		int id_1 = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::SboxCNT1, 0);
		int id_2 = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::SboxCNT2, 0);
		int id_3 = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::SboxCNT3, 0);


        int  input_a = -1;
		//如何确定last_rcu_id ?index 和实际bfu编号的转换,map
		int rcu_order_id = m_bcu_mod->m_rcuid2orderid_map[rcu_id];
		int last_order_id = m_bcu_mod->m_rcuid2orderid_map[rcu_id]-1;
		int last_rcu_id = 0;
		if (last_order_id > 0)
		{
			last_rcu_id = m_bcu_mod->m_orderid2rcuid_map[last_order_id];
		}

		int sbox_id_a = input_a_index / SBOX_OUT_PORT_NUM;
		int sbox_out_port_id_a = input_a_index % SBOX_OUT_PORT_NUM;
		//benes_id 必为0 
		int benes_id_a = input_a_index / BENES_OUT_PORT_NUM;

		switch (input_a_type)
		{
		case InputPort_NULL:
			input_a = 0;
			m_last_in_que.push_back(input_a);
			break;
		case InputPort_CurrentBfuX:			
			//if (m_bcu_mod->m_rcu_mod[rcu_id]->m_bfu_mod[input_a_index]->m_bfu_out_valid)
			//{
				input_a = m_bcu_mod->m_rcu_mod[rcu_order_id]->m_bfu_mod[input_a_index]->m_bfu_out[0];
				m_last_in_que.push_back(input_a);
				break;
			//}
		case InputPort_CurrentBfuY:
			//if (m_bcu_mod->m_rcu_mod[rcu_id]->m_bfu_mod[input_a_index]->m_bfu_out_valid)
			//{
				input_a = m_bcu_mod->m_rcu_mod[rcu_order_id]->m_bfu_mod[input_a_index]->m_bfu_out[1];
				m_last_in_que.push_back(input_a);
			//}
			break; 
		case InputPort_CurrentSbox:
				input_a = m_bcu_mod->m_rcu_mod[rcu_order_id]->m_sbox_mod[sbox_id_a]->m_sbox_out[sbox_out_port_id_a];
				m_last_in_que.push_back(input_a);
			break;
		case InputPort_CurrentBenes:
			//benes_id 必为0 
			input_a = m_bcu_mod->m_rcu_mod[rcu_order_id]->m_benes_mod[benes_id_a]->m_benes_out[input_a_index];
			m_last_in_que.push_back(input_a);
			break;
		case InputPort_LastBfuX:
			//if (m_bcu_mod->m_rcu_mod[rcu_id]->m_bfu_mod[input_a_index]->m_bfu_out_valid)
			//{
				input_a = m_bcu_mod->m_rcu_mod[last_order_id]->m_bfu_mod[input_a_index]->m_bfu_out[0];
				m_last_in_que.push_back(input_a);
			//}
			break;
		case InputPort_LastBfuY:
			//if (m_bcu_mod->m_rcu_mod[rcu_id]->m_bfu_mod[input_a_type]->m_bfu_out_valid)
			//{
				input_a = m_bcu_mod->m_rcu_mod[last_order_id]->m_bfu_mod[input_a_index]->m_bfu_out[1];
				m_last_in_que.push_back(input_a);
			//}
			break;
		case InputPort_LastBenes:
			input_a = m_bcu_mod->m_rcu_mod[last_order_id]->m_benes_mod[benes_id_a]->m_benes_out[input_a_index];
			m_last_in_que.push_back(input_a);
			break;
		case InputPort_LastSbox:
			input_a = m_bcu_mod->m_rcu_mod[last_order_id]->m_sbox_mod[sbox_id_a]->m_sbox_out[sbox_out_port_id_a];
			m_last_in_que.push_back(input_a);
			break;
		case InputPort_InFifo:
			//从input fifo中获取值，移步 bfu 里做还是rcu里做，需要考虑一下
			//if (m_ififo->size() > 0)
			//{
			//	input_a = m_ififo->front().input_data;
			//	m_last_in_que.push_back(input_a);
			//	m_bcu_mod->m_is_used_infifo = true;
			//	if (print_num >= 3)
			//	{
			//		m_bcu_mod->m_ififo->pop_front();
			//	}

			//}
			input_a = m_bcu_mod->m_infifo_pre_read.at(input_a_index);
			m_last_in_que.push_back(input_a);
			m_bcu_mod->m_is_used_infifo = true;
			break;
		case InputPort_Mem:
			//ElementType tmp_element = ELEMENT_OUTMEM;
			//int mem_offset = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, tmp_element, 0, AttributeID::RCUMEMRDOFFSET, 0);
			input_a = m_bcu_mod->m_mem_mod->m_mem_ram[m_bcu_mod->m_mem_mod->m_mem_read_addr[input_a_index]];
			m_last_in_que.push_back(input_a);
			break;
		case InputPort_Imd:
			input_a = m_bcu_mod->m_imd[input_a_index];
			m_last_in_que.push_back(input_a);
			m_bcu_mod->m_is_used_imd = true;
            break;
		case InputPort_Rch:
			input_a = m_bcu_mod->m_sreg_mod->m_sreg_out_data[input_a_index];
			m_last_in_que.push_back(input_a);
            break;
        case InputPort_Lfsr:
            if (m_bcu_mod->m_rcv_lfsr_rst_que.size() > 0)
            {
                input_a = m_bcu_mod->m_rcv_lfsr_rst_que.at(input_a_index);
                m_last_in_que.push_back(input_a);
                m_bcu_mod->m_is_used_lfsr = true;
            }
            break;
		}
        //m_rcu_ele = sbox_ele;
//        char * input_src_t = new char[strlen(input_src) + 1];
//        strcpy(input_src_t, input_src);

		//:/src 替换成 src
//		input_src_t = del_char(input_src_t, ':');
//		input_src_t = del_char(input_src_t, '/');
		string input_src_t;
		if (strcmp(input_src, "") != 0 && (strcmp(input_src, "unset") != 0))
		{
			input_src_t = m_bcu_mod[bcu_id].get_source_map()->at(string(input_src));
		}
		else
		{
			
		}

		if (m_last_in_que.size()>0)
		{
            m_sbox_in_date = m_last_in_que.front();
            int* tmp_sbox_out = run_sbox(input_a, sbox_mod, input_src_t.c_str(), id_0, id_1, id_2, id_3,print_num);
			m_sbox_out_valid = true;
			m_last_in_que.pop_front();

			for (int i = 0; i < SBOX_OUT_PORT_NUM; ++i)
			{
				//保存之前输出为last
				m_sbox_last_out[i] = m_sbox_out[i];
				//更新最新的输出结果
				m_sbox_out[i] = tmp_sbox_out[i];

				//m_sbox_debug_log << "sbox out " << i << " = " <<hex<< m_sbox_out[i] << endl;
			}
            delete[] tmp_sbox_out;
		}
		else
		{
			m_sbox_out_valid = false;
		}
    }

    int* sbox_mod::run_sbox(int a, int mode, const char *src, int id0, int id1, int id2, int id3,int print_num)
    {
        //------------4x1k-sbox-mem----------//
		vector<unsigned int > mem0;
		vector<unsigned int > mem1;
		vector<unsigned int > mem2;
		vector<unsigned int > mem3;
		mem0.resize(1024);
		mem1.resize(1024);
		mem2.resize(1024);
		mem3.resize(1024);
        //unsigned	int mem1[1024] = { 0 };
        //unsigned	int mem2[1024] = { 0 };
        //unsigned	int mem3[1024] = { 0 };
        //---- ------------------------------//
        unsigned int mem0_out;
        unsigned int mem1_out;
        unsigned int mem2_out;
        unsigned int mem3_out;

        unsigned char mem0_out_xor;
        unsigned char mem1_out_xor;
        unsigned char mem2_out_xor;
        unsigned char mem3_out_xor;

        int mem0_adr;
        int mem1_adr;
        int mem2_adr;
        int mem3_adr;

        unsigned int data_4bit_spli;
        unsigned int data_8bit_spli;
        unsigned int data_8bit_xor_spli;
        unsigned int data_9bit_spli;

        //------------------------------------//
        //  static int sbox_out[4]={0};
        int * sbox_out = new int[4];
        //------------------------------------//
        //-----------read-sbox-address-generate-------------//
		string line;
		int data_num = 0;
		char str[1024];
		//UINT32 idate[4];
		in_date.resize(4, 0);
		int addr = 0;
		FILE *fp;
		fp = fopen(src, "r");
		//int line_num = 0;
		bool valid_line = true;
		if (NULL != fp)
		{
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
					sscanf(str, "%x %x %x %x %x\n", &addr, &in_date[0]);

					int mem_sel = (addr / 4) / 256;
					int mem_addr = (addr / 4) % 256;
					//int mem_sel = (addr/4) / 256;
					//int mem_addr = (addr/4) % 256;
					switch (mem_sel)
					{
					case 0:
						mem0[mem_addr] = in_date[0];
						break;
					case 1:
						mem1[mem_addr] = in_date[0];
						break;
					case 2:
						mem2[mem_addr] = in_date[0];
						break;
					case 3:
						mem3[mem_addr] = in_date[0];
						break;
					default:
						break;
					}
				}

			}

		}


		switch (mode)
        {
        case 0://mode 0 4-->4
            mem0_adr = (a & 0xf) | (id0 << 8);
            mem1_adr = ((a >> 4) & 0xf) | (id1 << 8);
            mem2_adr = ((a >> 8) & 0xf) | (id2 << 8);
            mem3_adr = ((a >> 12) & 0xf) | (id3 << 8);
            break;
        case 1://mode 1 
            mem0_adr = (a & 0x3f) | (id0 << 8);
            mem1_adr = ((a >> 6) & 0x3f) | (id1 << 8);
            mem2_adr = ((a >> 12) & 0x3f) | (id2 << 8);
            mem3_adr = ((a >> 18) & 0x3f) | (id3 << 8);
            break;
        case 2://mode 2
            mem0_adr = (a & 0xff) | (id0 << 8);
            mem1_adr = ((a >> 8) & 0xff) | (id1 << 8);
            mem2_adr = ((a >> 16) & 0xff) | (id2 << 8);
            mem3_adr = ((a >> 24) & 0xff) | (id3 << 8);
            break;
        case 3://mode 3
            mem0_adr = (a & 0xff) | (id0 << 8);
            mem1_adr = ((a >> 8) & 0xff) | (id1 << 8);
            mem2_adr = ((a >> 16) & 0xff) | (id2 << 8);
            mem3_adr = ((a >> 24) & 0xff) | (id3 << 8);
            break;
        case 4://mode 4
            mem0_adr = (a & 0xff) | (id0 << 8);
            mem1_adr = ((a >> 8) & 0xff) | (id1 << 8);
            mem2_adr = ((a >> 16) & 0xff) | (id2 << 8);
            mem3_adr = ((a >> 24) & 0xff) | (id3 << 8);
            break;
        case 5://mode 5
            mem0_adr = (a & 0xff) | (id0 << 8);
            mem1_adr = ((a >> 8) & 0xff) | (id1 << 8);
            mem2_adr = ((a >> 16) & 0xff) | (id2 << 8);
            mem3_adr = ((a >> 24) & 0xff) | (id3 << 8);
            break;
        default:
            mem0_adr = 0;
            mem1_adr = 0;
            mem2_adr = 0;
            mem3_adr = 0;
            break;
        }
        //---------------------------//
        mem0_out = mem0[mem0_adr];
        mem1_out = mem1[mem1_adr];
        mem2_out = mem2[mem2_adr];
        mem3_out = mem3[mem3_adr];


        //--xor-per-byte-in-mem-output-------//
        mem0_out_xor = (unsigned char)(((mem0_out >> 24) & 0xff) ^ ((mem0_out >> 16) & 0xff) ^ ((mem0_out >> 8) & 0xff) ^ (mem0_out & 0xff));
        mem1_out_xor = (unsigned char)(((mem1_out >> 24) & 0xff) ^ ((mem1_out >> 16) & 0xff) ^ ((mem1_out >> 8) & 0xff) ^ (mem1_out & 0xff));
        mem2_out_xor = (unsigned char)(((mem2_out >> 24) & 0xff) ^ ((mem2_out >> 16) & 0xff) ^ ((mem2_out >> 8) & 0xff) ^ (mem2_out & 0xff));
        mem3_out_xor = (unsigned char)(((mem3_out >> 24) & 0xff) ^ ((mem3_out >> 16) & 0xff) ^ ((mem3_out >> 8) & 0xff) ^ (mem3_out & 0xff));
        //-----4 bit splice----//
        data_4bit_spli = ((mem3_out & 0xf) << 12) | ((mem2_out & 0xf) << 8) | ((mem1_out & 0xf) << 4) | (mem0_out & 0xf);
        //-----4-bit-splice----//
        data_8bit_spli = ((mem3_out & 0xff) << 24) | ((mem2_out & 0xff) << 16) | ((mem1_out & 0xff) << 8) | (mem0_out & 0xff);
        data_8bit_xor_spli = (mem3_out_xor << 24) | (mem2_out_xor << 16) | (mem1_out_xor << 8) | (mem0_out_xor);
        //------------------------//
        if (a & 0x100) data_9bit_spli = (mem0_out >> 16) & 0x1ff;
        else        data_9bit_spli = mem0_out & 0x1ff;
        //------------------------//
        switch (mode)
        {
        case 0://mode 0 4-->4
            sbox_out[0] = data_4bit_spli;
            sbox_out[1] = 0;
            sbox_out[2] = 0;
            sbox_out[3] = 0;
            break;
        case 1://mode 1 
            sbox_out[0] = data_4bit_spli;
            sbox_out[1] = 0;
            sbox_out[2] = 0;
            sbox_out[3] = 0;
            break;
        case 2://mode 2
            sbox_out[0] = data_8bit_spli;
            sbox_out[1] = 0;
            sbox_out[2] = 0;
            sbox_out[3] = 0;
            break;
        case 3://mode 3
            sbox_out[0] = data_8bit_xor_spli;
            sbox_out[1] = 0;
            sbox_out[2] = 0;
            sbox_out[3] = 0;
            break;
        case 4://mode 4
            sbox_out[0] = mem0_out;
            sbox_out[1] = mem1_out;
            sbox_out[2] = mem2_out;
            sbox_out[3] = mem3_out;
            break;
        case 5://mode 5
            sbox_out[0] = data_9bit_spli;
            sbox_out[1] = 0;
            sbox_out[2] = 0;
            sbox_out[3] = 0;
            break;
        default:
            sbox_out[0] = 0;
            sbox_out[1] = 0;
            sbox_out[2] = 0;
            sbox_out[3] = 0;
            break;
        }
		if (print_num >=3)
		{
			cout << "[OPERATOR RCU " << m_rcu_id << " SBOX " << m_sbox_id << "]";
			cout << hex << "a:" << a << ",";
			cout << hex << "out[0]:(" << sbox_out[0] << "," << sbox_out[1] << "," << sbox_out[2] << "," << sbox_out[3] << ")" << dec << endl;
			
			m_bcu_mod->m_cout_out_log << "[OPERATOR RCU " << m_rcu_id << " SBOX " << m_sbox_id << "]";
			m_bcu_mod->m_cout_out_log << hex << "a:" << a << ",";
			m_bcu_mod->m_cout_out_log << hex << "out[0]:(" << sbox_out[0] << "," << sbox_out[1] << "," << sbox_out[2] << "," << sbox_out[3] << ")" << dec << endl;
		}
		if (NULL != fp)
		{
			fclose(fp);
		}
        return sbox_out;
    }


    void sbox_mod::get_xml_resolver(XmlResolver *xml_resolver)
    {
        m_rca_xml_slv = xml_resolver;
    }



	char * sbox_mod::del_char(char* str, char ch)
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


	//int sbox_mod::ch16toi(char ch)
	//{
	//	if (ch >= '0'&&ch <= '9')
	//		return ch - '0';

	//	if (ch >= 'a'&&ch <= 'f')
	//		return ch - 'a' + 10;

	//	if (ch >= 'A'&&ch <= 'F')
	//		return ch - 'A' + 10;
	//}

	//long sbox_mod::hex2dec(char* str)
	//{
	//	char *p;
	//	long x = 0;

	//	for (p = str; *p != '\0'; p++)
	//	{
	//		x = x * 16 + ch16toi(*p);
	//	}
	//	delete p;
	//	return x;
	//}


}
