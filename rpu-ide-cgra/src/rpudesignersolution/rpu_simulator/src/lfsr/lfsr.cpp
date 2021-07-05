
#include "lfsr.h"

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
	lfsr_mod::lfsr_mod(sc_module_name name, top_cfg total_top_cfg, int lfsr_id) :sc_module(name), m_top_cfg(total_top_cfg)
    {
        SC_METHOD(data_process_mth);
        sensitive << sci_chip_clk.pos();
        dont_initialize();

		//m_rcu_id = rcu_id;
		m_lfsr_id = lfsr_id;
		m_rcu_id = 0;
        m_lfsr_mode = 0;
        mod_init(lfsr_id);
    }

    //************************************
    // Method:    mod_init
    // FullName:  cmd_ctrl::mod_init
    // Access:    public 


    // Returns:   void
    // Qualifier:
    //************************************
    void lfsr_mod::mod_init(int lfsr_id)
    {       
        sci_rcu2lfsr_req.resize(m_top_cfg.bcu_num);
        sco_lfsr2lrcu_rst.resize(m_top_cfg.bcu_num);
        m_rcv_rcu_req_que.resize(m_top_cfg.bcu_num);
        sci_rcu2lfsr_hand_flg.resize(m_top_cfg.bcu_num);
        m_rcv_bcu_hand_flg.resize(m_top_cfg.bcu_num);

        for (int i = 0 ; i< m_top_cfg.bcu_num;++i)
        {
            sci_rcu2lfsr_req[i] = new sc_fifo_in<uint32_t>();
            sco_lfsr2lrcu_rst[i] = new sc_fifo_out<lfsr_send_data_t >();
            m_rcv_rcu_req_que[i] = new deque<uint32_t>();
            sci_rcu2lfsr_hand_flg[i] = new sc_fifo_in<uint32_t>();
            m_rcv_bcu_hand_flg[i] = new deque<uint32_t>();

        }

        m_bcu_syn_flg.resize(m_top_cfg.bcu_num, 0);
        m_need_syn_bcu.resize(m_top_cfg.bcu_num, 0);
        m_syn_ready_flg.resize(m_top_cfg.bcu_num, 0);
        m_all_date_reay = false;

        m_lfsr_link_date.resize(32,0);
        m_lfsr_out_date.resize(16, 0);


        string lfsr_name = "lfsr_";
        string a = "_debug.log";
        ostringstream oss;
        oss << lfsr_name << lfsr_id << a;
        m_cout_out_log.open(oss.str());
        oss.clear();

    }

    //************************************
    // Method:    data_process_mth
    // FullName:  cmd_ctrl::data_process_mth
    // Access:    public 
    // Returns:   void
    // Qualifier:
    //************************************
    void lfsr_mod::data_process_mth()
    {

        //if (m_send_rcu_rst_que.size() > 0)
        //{
        //    for (int i = 0; i < m_top_cfg.bcu_num; ++i)
        //    {
        //        //发送请求
        //        lfsr_send_data_t send_date;
        //        send_date.real_data = m_send_rcu_rst_que.front();
        //        sco_lfsr2lrcu_rst[i]->nb_write(send_date);
        //    }
        //    m_send_rcu_rst_que.pop_front();
        //}

         //接收请求
        for (int i = 0; i < m_top_cfg.bcu_num; ++i)
        {
            if (sci_rcu2lfsr_req[i]->num_available() > 0)
            {
                uint32_t rec_req = sci_rcu2lfsr_req[i]->read();
                m_rcv_rcu_req_que[i]->push_back(rec_req);
            }
        }
        //接收握手信号
        for (int i = 0; i < m_top_cfg.bcu_num; ++i)
        {
            if (sci_rcu2lfsr_hand_flg[i]->num_available() > 0)
            {
                uint32_t hand_flg = sci_rcu2lfsr_hand_flg[i]->read();
                m_rcv_bcu_hand_flg[i]->push_back(hand_flg);
            }
        }

        lfsr_run_process();

        m_cycle_cnt++;
    }

    //************************************
    // Method:    rr_process
    // FullName:  cmd_ctrl::rr_process
    // Access:    public 
    // Returns:   void
    // Qualifier:
    //************************************
    void lfsr_mod::lfsr_run_process()
    {
        m_need_syn_bcu.clear();
        for (int i = 0; i < m_top_cfg.bcu_num; ++i)
        {
            if (1 == m_bcu_syn_flg[i])
            {
                m_need_syn_bcu.push_back(i);
            }
            
        }
        //同步
        for (int i = 0; i < m_need_syn_bcu.size(); ++i)
        {
            int bcu_id = m_need_syn_bcu[i];
            //if (m_rcv_rcu_req_que[bcu_id]->size()>0 && m_rcv_bcu_hand_flg[bcu_id]->size()>0)
            if (m_rcv_bcu_hand_flg[bcu_id]->size() > 0)
            {
                m_syn_ready_flg[bcu_id] = 1;
            }
            else
            {
                m_syn_ready_flg[bcu_id] = 0;
            }
        }
        m_all_date_reay = false;
        for (int i = 0; i < m_need_syn_bcu.size(); ++i)
        {
            int bcu_id = m_need_syn_bcu[i];
            if (0 == m_syn_ready_flg[bcu_id])
            {
                m_all_date_reay = false;
                break;
            }
            m_all_date_reay = true;
            //同步需要修改

            //if (1 == m_syn_ready_flg[bcu_id])
            //{
            //    m_all_date_reay = true;
            //    break;
            //}


        }
        //m_all_date_reay = true;

        if (m_all_date_reay)
        {
            update_lfsr(m_lfsr_mode);
            for (int i = 0 ;i< m_need_syn_bcu.size();++i)
            {
                int bcu_id = m_need_syn_bcu[i];
                m_rcv_bcu_hand_flg[bcu_id]->pop_front();
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
    void lfsr_mod::stat_pro()
    {

    }
    //************************************
    // Method:    print_info
    // FullName:  cmd_ctrl::print_info
    // Access:    public 
    // Returns:   void
    // Qualifier:
    //************************************
    void lfsr_mod::print_info()
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
    void lfsr_mod::get_xml_cfg(int bcu_id, int rcu_id,  XMLElement * sreg_ele)
    {
		//ElementType element = ElementType::ELEMENT_RCH;
		////<RCH addr3 = "2" addr4 = "3" addr1 = "0" addr2 = "1" mode = "0" / >
		//m_rch_mode = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, 0, AttributeID::RCURCH_Mode, 0);
		////sreg_input_index = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::RCURCH_Index, 0);
		//m_dest[0] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, 0, AttributeID::RCURCH_DEST0, 0);
		//m_dest[1] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, 0, AttributeID::RCURCH_DEST1, 1);
		//m_dest[2] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, 0, AttributeID::RCURCH_DEST2, 2);
		//m_dest[3] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, 0, AttributeID::RCURCH_DEST3, 3);
    }


    lfsr_mod::~lfsr_mod()
    {
        m_cout_out_log.close();

        while (!sci_rcu2lfsr_req.empty()) {
            delete sci_rcu2lfsr_req.back(),sci_rcu2lfsr_req.pop_back();
        }
        while (!sco_lfsr2lrcu_rst.empty()) {
            delete sco_lfsr2lrcu_rst.back(),sco_lfsr2lrcu_rst.pop_back();
        }
        while (!m_rcv_rcu_req_que.empty()) {
            delete m_rcv_rcu_req_que.back(),m_rcv_rcu_req_que.pop_back();
        }
        while (!sci_rcu2lfsr_hand_flg.empty()) {
            delete sci_rcu2lfsr_hand_flg.back(),sci_rcu2lfsr_hand_flg.pop_back();
        }
        while (!m_rcv_bcu_hand_flg.empty()) {
            delete m_rcv_bcu_hand_flg.back(),m_rcv_bcu_hand_flg.pop_back();
        }


    }

    void lfsr_mod::run_process(int bcu_id, int rcu_id, int lfsr_id, XMLElement * sreg_ele)
    {
	     
    }

    void lfsr_mod::get_xml_resolver(vector<XmlResolver* > xml_resolver, const string &pro_name)
    {
        m_rca_xml_slv = xml_resolver;

        for (int i = 0 ; i < m_rca_xml_slv.size();++i)
        {
            if (NULL != m_rca_xml_slv[i]->getRcaNode())
            {
                XMLElement *bcu_ele = m_rca_xml_slv[i]->getRcaNode()->FirstChildElement("BCU");
                if (bcu_ele)
                {
                    if (NULL != bcu_ele->Attribute("lfsrgroup"))
                    {
                        m_bcu_syn_flg[i] = atoi(bcu_ele->Attribute("lfsrgroup"));
                    }
                }
            }
        }
        if (NULL != m_rca_xml_slv[0]->getRcaNode())
        {
            XMLElement *bcu_ele = m_rca_xml_slv[0]->getRcaNode()->FirstChildElement("BCU");
            m_lfsr_mode = atoi(bcu_ele->Attribute("LFSR_WIDTH"));
      
        }


        tinyxml2::XMLDocument pro_document;
        pro_document.LoadFile(pro_name.c_str());
        XMLElement *root = pro_document.RootElement();
        if (root)
        {
            if (NULL != root->Attribute("lfsrMode"))
            {
                m_lfsr_out_mode = atoi(root->Attribute("lfsrMode"));
            }
        }
    }

	char * lfsr_mod::del_char(char* str, char ch)
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



	void lfsr_mod::update_lfsr(int mode)
	{
        for (int i= 0;i<m_top_cfg.bcu_num;++i)
        {
           if (1 == mode)
           {

           }
           else if (8 == mode)
           {
           }
           else if (16 == mode)
           {
           }
           else if (32 == mode)
           {
               //uint32_t link_32 = m_lfsr_link_date[31];

               if (m_rcv_rcu_req_que[i]->size() > 0)
               {
                   for (int j = 31; j > 0; --j)
                   {
                       m_lfsr_link_date[j] = m_lfsr_link_date[j - 1];
                   }
                   //暂时都往0地址写
                   m_lfsr_link_date[0] = m_rcv_rcu_req_que[i]->front();
                   m_rcv_rcu_req_que[i]->pop_front();
               }
           }
           else
           {
               //不移位
           }

            
        }


        if (0 == m_lfsr_out_mode)
        {
            for (int i = 0; i < 16; ++i)
            {
                m_lfsr_out_date[i] = m_lfsr_link_date[i];
            }
        }
        else if (1 == m_lfsr_out_mode)
        {
            for (int i = 0; i < 16; ++i)
            {
                m_lfsr_out_date[i] = m_lfsr_link_date[i + 8];
            }
        }
        else
        {
            for (int i = 0; i < 16; ++i)
            {
                m_lfsr_out_date[i] = m_lfsr_link_date[i + 16];
            }
        }

        //m_send_rcu_rst_que.push_back(m_lfsr_out_date);

     for (int i = 0; i < m_top_cfg.bcu_num; ++i)
    {
        //发送请求
        lfsr_send_data_t send_date;
        send_date.real_data = m_lfsr_out_date;
        //send_date.real_data = m_send_rcu_rst_que.front();
        sco_lfsr2lrcu_rst[i]->write(send_date);
    }
    //m_send_rcu_rst_que.pop_front();

        cout << "[OPERATOR LFSR " << m_lfsr_id << "]  (";
        m_cout_out_log << "[OPERATOR LFSR " << m_lfsr_id << "]  (";
        for (int i = 0 ; i < 16;++i)
        {
            cout << hex << m_lfsr_out_date[i] << ",";
            m_cout_out_log << hex << m_lfsr_out_date[i] << ",";
        }
        cout << ")" << dec << endl;
        m_cout_out_log << ")" << dec << endl;

	}

}
