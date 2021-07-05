
#include "sreg_mod.h"

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
	sreg_mod::sreg_mod(sc_module_name name, top_cfg total_top_cfg, int sreg_id, bcu_mod *mod_t) :sc_module(name), m_top_cfg(total_top_cfg)
    {
        SC_METHOD(data_process_mth);
        sensitive << sci_chip_clk.pos();
        dont_initialize();

        m_bcu_mod = mod_t;
		//m_rcu_id = rcu_id;
		m_sreg_id = sreg_id;
		m_rcu_id = 0;

        mod_init(sreg_id);
    }

    //************************************
    // Method:    mod_init
    // FullName:  cmd_ctrl::mod_init
    // Access:    public 


    // Returns:   void
    // Qualifier:
    //************************************
    void sreg_mod::mod_init(int sreg_id)
    {       

		//m_sreg_out.resize(SREG_IN_PORT_NUM, -1);
		//m_sreg_last_out.resize(SREG_IN_PORT_NUM, -1);

		m_last_in_que.resize(SREG_IN_PORT_NUM);

		m_sreg_out_data.resize(SREG_OUT_PORT_NUM);
		m_last_sreg_out_date.resize(SREG_OUT_PORT_NUM);

		m_in_date_valid.resize(SREG_IN_PORT_NUM,0);

		m_dest.resize(SREG_IN_PORT_NUM);

	    //m_last_in_que.clear();
        //m_ififo = m_bcu_mod->m_ififo;
        //m_ofifo = m_bcu_mod->m_ofifo;

		string rca_name = "rca";
		string rcu_name = "rcu";
		string sreg_name = "_sreg";
		string a = "_debug.log";

		ostringstream oss;
		oss << rcu_name << 0 << sreg_name << sreg_id << a;
		//m_sreg_debug_log.open(oss.str());

    }

    //************************************
    // Method:    data_process_mth
    // FullName:  cmd_ctrl::data_process_mth
    // Access:    public 
    // Returns:   void
    // Qualifier:
    //************************************
    void sreg_mod::data_process_mth()
    {

    }

    //************************************
    // Method:    rr_process
    // FullName:  cmd_ctrl::rr_process
    // Access:    public 
    // Returns:   void
    // Qualifier:
    //************************************
    void sreg_mod::sreg_run_process()
    {
		vector<int > input_date;
		input_date.resize(SREG_IN_PORT_NUM, 0);

		for (int i = 0 ; i < SREG_IN_PORT_NUM ;i++)
		{
			if (m_last_in_que[i].size() > 0)
			{
				m_in_date_valid[i] = 1;
				input_date[i] = m_last_in_que[i].front();
			}
			else
			{
				m_in_date_valid[i] = 0;
			}
		}
		   		 
		m_word_enable = (m_in_date_valid[3] << 3) | (m_in_date_valid[2] << 2) | (m_in_date_valid[1] << 1)  | (m_in_date_valid[0]);

	    if (m_word_enable > 0)
		{
		    update_sreg(input_date[0], input_date[1], input_date[2], input_date[3], m_word_enable,m_dest[0], m_dest[1], m_dest[2], m_dest[3], m_rch_mode);
			m_sreg_out_valid = true;

			for (int i = 0; i < SREG_IN_PORT_NUM; i++)
			{
				if (m_last_in_que[i].size() > 0)
				{
					m_last_in_que[i].pop_front();
				}
			}
		}
		else
		{
			m_sreg_out_valid = false;
		}


		for (int i = 0; i < SREG_OUT_PORT_NUM; ++i)
		{
			//保存之前输出为last
			m_last_sreg_out_date[i] = m_sreg_out_data[i];
			//m_sreg_debug_log << "sreg out " << i << " = " << hex << m_sreg_out_data[i] << endl;
		}



    }


    //************************************
    // Method:    stat_pro
    // FullName:  cmd_ctrl::stat_pro
    // Access:    public 
    // Returns:   void
    // Qualifier:
    //************************************
    void sreg_mod::stat_pro()
    {

    }
    //************************************
    // Method:    print_info
    // FullName:  cmd_ctrl::print_info
    // Access:    public 
    // Returns:   void
    // Qualifier:
    //************************************
    void sreg_mod::print_info()
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
    void sreg_mod::get_xml_cfg(int bcu_id, int rcu_id,  XMLElement * sreg_ele)
    {
		ElementType element = ElementType::ELEMENT_RCH;
		//<RCH addr3 = "2" addr4 = "3" addr1 = "0" addr2 = "1" mode = "0" / >
		m_rch_mode = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, 0, AttributeID::RCURCH_Mode, 0);
		//sreg_input_index = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::RCURCH_Index, 0);


		m_dest[0] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, 0, AttributeID::RCURCH_DEST0, 0);
		m_dest[1] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, 0, AttributeID::RCURCH_DEST1, 1);
		m_dest[2] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, 0, AttributeID::RCURCH_DEST2, 2);
		m_dest[3] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, 0, AttributeID::RCURCH_DEST3, 3);
    }


	sreg_mod::~sreg_mod()
    {
    }

    void sreg_mod::run_process(int bcu_id, int rcu_id, int sreg_id, XMLElement * sreg_ele)
    {
		m_is_infifo_used = false;
		ElementType element = ElementType::ELEMENT_RCH;

		int element_id = sreg_id;

		int sreg_input_type = -1;
	    int sreg_input_index = -1;

		sreg_input_type  = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::RCURCH_Type, 0);
		sreg_input_index = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::RCURCH_Index, 0);


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

		sbox_id_a = sreg_input_index / SBOX_OUT_PORT_NUM;
	    sbox_out_port_id_a = sreg_input_index % SBOX_OUT_PORT_NUM;
	    //benes_id 必为0 
	    benes_id_a = sreg_input_index / BENES_OUT_PORT_NUM;

		 int input_a = -1;

            switch (sreg_input_type)
            {
            case InputPort_NULL:
                input_a = 0;
				//m_last_in_que[sreg_id].push_back(input_a);
                break;
            case InputPort_CurrentBfuX:
				//if (m_bcu_mod->m_rcu_mod[rcu_id]->m_bfu_mod[benes_input_index[i]]->m_bfu_out_valid)
				//{
					input_a = m_bcu_mod->m_rcu_mod[rcu_order_id]->m_bfu_mod[sreg_input_index]->m_bfu_out[0];
					m_last_in_que[sreg_id].push_back(input_a);
					break;
				//}
            case InputPort_CurrentBfuY:
                input_a = m_bcu_mod->m_rcu_mod[rcu_order_id]->m_bfu_mod[sreg_input_index]->m_bfu_out[1];
				m_last_in_que[sreg_id].push_back(input_a);
                break;
            case InputPort_CurrentSbox:
                input_a = m_bcu_mod->m_rcu_mod[rcu_order_id]->m_sbox_mod[sbox_id_a]->m_sbox_out[sbox_out_port_id_a];
				m_last_in_que[sreg_id].push_back(input_a);
				break;
            case InputPort_CurrentBenes:
                input_a = m_bcu_mod->m_rcu_mod[rcu_order_id]->m_benes_mod[benes_id_a]->m_benes_out[sreg_input_index];
				m_last_in_que[sreg_id].push_back(input_a);
				break;
            case InputPort_LastBfuX:
                input_a = m_bcu_mod->m_rcu_mod[last_order_id]->m_bfu_mod[sreg_input_index]->m_bfu_out[0];
				m_last_in_que[sreg_id].push_back(input_a);
				break;
            case InputPort_LastBfuY:
                input_a = m_bcu_mod->m_rcu_mod[last_order_id]->m_bfu_mod[sreg_input_index]->m_bfu_out[1];
				m_last_in_que[sreg_id].push_back(input_a);
				break;
            case InputPort_LastBenes:
                input_a = m_bcu_mod->m_rcu_mod[last_order_id]->m_benes_mod[benes_id_a]->m_benes_out[sreg_input_index];
				m_last_in_que[sreg_id].push_back(input_a);
				break;
            case InputPort_LastSbox:
				input_a = m_bcu_mod->m_rcu_mod[last_order_id]->m_sbox_mod[sbox_id_a]->m_sbox_out[sbox_out_port_id_a];
				m_last_in_que[sreg_id].push_back(input_a);
				break;
            case InputPort_InFifo:
                //从input fifo中获取值，移步 bfu 里做还是rcu里做，需要考虑一下
				//if (m_ififo->size() > 0)
				//{
				//	input_a = m_ififo->front().input_data;
				//	m_last_in_que[sreg_id].push_back(input_a);
				//	m_bcu_mod->m_is_used_infifo = true;
				//	m_bcu_mod->m_ififo->pop_front();

				//}
				input_a = m_bcu_mod->m_infifo_pre_read.at(sreg_input_index);
				m_last_in_que[sreg_id].push_back(input_a);
				m_bcu_mod->m_is_used_infifo = true;

                break;
            case InputPort_Mem:
                //element = ELEMENT_OUTMEM;
                //int mem_offset = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, 0, AttributeID::RCUMEMRDOFFSET, 0);
                input_a = m_bcu_mod->m_mem_mod->m_mem_ram[m_bcu_mod->m_mem_mod->m_mem_read_addr[sreg_input_index]];
				m_last_in_que[sreg_id].push_back(input_a);
                break;
			case InputPort_Imd:
				input_a = m_bcu_mod->m_imd[sreg_input_index];
				m_last_in_que[sreg_id].push_back(input_a);
				m_bcu_mod->m_is_used_imd = true;
				break;
			case InputPort_Rch:
				input_a = m_bcu_mod->m_sreg_mod->m_sreg_out_data[sreg_input_index];
				m_last_in_que[sreg_id].push_back(input_a);
				break;
            case InputPort_Lfsr:
                if (m_bcu_mod->m_rcv_lfsr_rst_que.size() > 0)
                {
                    input_a = m_bcu_mod->m_rcv_lfsr_rst_que.at(sreg_input_index);
                    m_last_in_que[sreg_id].push_back(input_a);
                    m_bcu_mod->m_is_used_lfsr = true;
                }
                break;
            }
      
    }

    void sreg_mod::get_xml_resolver(XmlResolver *xml_resolver)
    {
        m_rca_xml_slv = xml_resolver;


    }

	char * sreg_mod::del_char(char* str, char ch)
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



	void sreg_mod::update_sreg(int in0, int in1, int in2, int in3, int word_enable, int dest0, int dest1, int dest2, int dest3, int mode)
	{
		int word_en[16];
		bool is_cache_mode = (mode == CACHE_MODE);
		bool is_sreg_1x16 = (mode == SREG_MODE_1X16);
		bool is_sreg_4x4 = (mode == SREG_MODE_4X4);


		bool in_valid[4];
		in_valid[0] = word_enable % 2;
		in_valid[1] = word_enable % 4 / 2;
		in_valid[2] = word_enable % 8 / 4;
		in_valid[3] = word_enable % 16 / 8;

		unsigned int in[4];
		in[0] = in0;
		in[1] = in1;
		in[2] = in2;
		in[3] = in3;
		if (is_sreg_1x16) 
		{
			if (word_enable == 0xF)
			{
				for (int i = 0; i < 12; i++) 
				{
					m_sreg_out_data[i] = m_sreg_out_data[i + 4];
				}
				m_sreg_out_data[12] = in[0];
				m_sreg_out_data[13] = in[1];
				m_sreg_out_data[14] = in[2];
				m_sreg_out_data[15] = in[3];
			}
		    else if (word_enable == 0x3)
			{
				for (int i = 0; i < 14; i++)
				{
					m_sreg_out_data[i] = m_sreg_out_data[i + 2];
				}
				m_sreg_out_data[14] = in[0];
				m_sreg_out_data[15] = in[1];
			}
			else if (word_enable == 0x1)
			{
				for (int i = 0; i < 15; i++)
				{
					m_sreg_out_data[i] = m_sreg_out_data[i + 1];
				}
				m_sreg_out_data[15] = in[0];
			}
		}
		else if (is_sreg_4x4) 
		{
				for (int j = 0; j < 4; j++) 
				{
					if (in_valid[j]) 
					{
						for (int i = 0; i < 3; i++) m_sreg_out_data[4 * j + i] = m_sreg_out_data[4 * j + i + 1];
						m_sreg_out_data[4 * j + 3] = in[j];
					}
				}
		}
		else if (is_cache_mode) 
		{
				if (in_valid[0]) m_sreg_out_data[dest0] = in0;
				if (in_valid[1]) m_sreg_out_data[dest1] = in1;
				if (in_valid[2]) m_sreg_out_data[dest2] = in2;
				if (in_valid[3]) m_sreg_out_data[dest3] = in3;
		}



		cout << "[OPERATOR RCU " << m_rcu_id << " SREG " << m_sreg_id << "]  (";
		m_bcu_mod->m_cout_out_log << "[OPERATOR RCU " << m_rcu_id << " SREG " << m_sreg_id << "]  (";

		for (int i = 0; i < 16; i++)
		{
			cout << hex << m_sreg_out_data[i] << ",";
			m_bcu_mod->m_cout_out_log << hex << m_sreg_out_data[i] << ",";
		}
		cout << ")" << dec << endl;
		m_bcu_mod->m_cout_out_log << ")" << dec << endl;
	}

}
