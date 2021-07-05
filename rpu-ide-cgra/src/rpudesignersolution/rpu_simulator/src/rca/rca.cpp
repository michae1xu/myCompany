#include "rca.h"
namespace rpu_esl_model
{
    rca_mod::rca_mod(sc_module_name name, int rca_id, top_cfg total_top_cfg) :sc_module(name), m_rca_id(rca_id), m_top_cfg(total_top_cfg)
    {
        SC_METHOD(data_process_mth);
        sensitive << chip_clk.pos();
        dont_initialize();

		m_bcu_mod.resize(m_top_cfg.bcu_num);
		for (int i = 0; i < m_top_cfg.bcu_num; ++i)
		{
            std::string str = "bcu_" + std::to_string(i);
			m_bcu_mod[i] = new bcu_mod(str.c_str(), m_top_cfg, i);
		}
        m_lfsr_mod = new lfsr_mod("lfsr_mod",m_top_cfg,0);
        mod_init();
    }

    void rca_mod::mod_init()
    {
        //bind
        m_bcu2lfsr_req_fifo.resize(m_top_cfg.bcu_num);
        m_lfsr2bcu_rst_fifo.resize(m_top_cfg.bcu_num);
        sci_ahb2rca_cfg_data.resize(m_top_cfg.bcu_num);
        m_bcu2lfsr_hand_flg_fifo.resize(m_top_cfg.bcu_num);
        m_bcu_last_ofifo_fifo.resize(m_top_cfg.bcu_num);
        for (int i = 0; i < m_top_cfg.bcu_num; ++i)
        {
            m_bcu2lfsr_req_fifo[i] = new sc_fifo<uint32_t>();
            m_lfsr2bcu_rst_fifo[i] = new sc_fifo<lfsr_send_data_t >();
            sci_ahb2rca_cfg_data[i] = new sc_fifo_in<rpu_data_struct >();
            m_bcu2lfsr_hand_flg_fifo[i] = new sc_fifo<uint32_t>();
            m_bcu_last_ofifo_fifo[i] = new sc_fifo<uint32_t>();

            m_bcu_mod[i]->chip_clk(chip_clk);
            m_bcu_mod[i]->sci_ahb2bcu_cfg_data(*sci_ahb2rca_cfg_data[i]);
            m_bcu_mod[i]->sci_lfsr2lbcu_rst_data(*m_lfsr2bcu_rst_fifo[i]);
            m_bcu_mod[i]->sco_bcu2lfsr_req_data(*m_bcu2lfsr_req_fifo[i]);
            m_bcu_mod[i]->sco_bcu2lfsr_hand_flag(*m_bcu2lfsr_hand_flg_fifo[i]);
        }

        for (int i = 0; i < m_top_cfg.bcu_num; ++i)
        {

            m_bcu_mod[i]->sco_next_bcu_in(*m_bcu_last_ofifo_fifo[i]);
            if (i > 0)
            {
                m_bcu_mod[i]->sci_last_bcu_in(*m_bcu_last_ofifo_fifo[i - 1]);
            }
            else
            {
                m_bcu_mod[i]->sci_last_bcu_in(*m_bcu_last_ofifo_fifo[m_top_cfg.bcu_num-1]);
            }
        }


        //lfsr

        for (int i = 0; i < m_top_cfg.bcu_num; ++i)
        {
            (*m_lfsr_mod->sci_rcu2lfsr_req[i])(*m_bcu2lfsr_req_fifo[i]); 
            (*m_lfsr_mod->sco_lfsr2lrcu_rst[i])(*m_lfsr2bcu_rst_fifo[i]);
            (*m_lfsr_mod->sci_rcu2lfsr_hand_flg[i])(*m_bcu2lfsr_hand_flg_fifo[i]);
        }
        m_lfsr_mod->sci_chip_clk(chip_clk);

        m_cycle_cnt = 0;

    }


    void rca_mod::get_xml_name(const string &xml_name)
	{
		m_xml_name = xml_name;
	}
    ESIM_RET rca_mod::get_mem_file_name(const string &xml_name)
	{
		ESIM_RET mem_load_sucess =RET_SUCCESS;
		m_mem_name = xml_name;
		for (int i = 0; i < m_top_cfg.bcu_num; ++i)
		{
			mem_load_sucess = m_bcu_mod[i]->get_mem_file_name(m_mem_name);
		}
		return mem_load_sucess;
	}
    void rca_mod::get_rch_file_name(const string &xml_name)
	{
		m_rch_name = xml_name;
	}
    ESIM_RET rca_mod::get_imd_file_name(const string &xml_name)
	{
		ESIM_RET load_sucess = RET_SUCCESS;
		m_imd_name = xml_name;
		for (int i = 0; i < m_top_cfg.bcu_num; ++i)
		{
			load_sucess = m_bcu_mod[i]->get_imd_file_name(m_imd_name);
		}

		return load_sucess;

	}


    void rca_mod::data_process_mth()
    {
        //bool simu_end_status = false;
        m_bcu_end_status = true;
        for (int i = 0; i < m_top_cfg.bcu_num; ++i)
        {
            m_bcu_end_status = m_bcu_end_status & m_bcu_mod[i]->get_simu_end_status();
        }

        if (m_bcu_end_status)
        {
            lfsr_send_data_t temp_date;
            temp_date.input_data = m_bcu_end_status;
            temp_date.latency =  m_cycle_cnt + 10;
            m_stop_latency_que.push_back(temp_date);
            //sc_stop();
        }


        if (m_stop_latency_que.size() > 0 && (m_stop_latency_que.front().latency < m_cycle_cnt))
        {
            m_stop_latency_que.pop_front();
            sc_stop();
            //sc_pause();
        }


        m_cycle_cnt++;
    }

    bool rca_mod::get_simu_end_status()
    {
        return m_bcu_end_status;
    }


    rca_mod::~rca_mod()
    {
        while (!m_bcu_mod.empty()) {
            delete m_bcu_mod.back(),m_bcu_mod.pop_back();
        }
        delete m_lfsr_mod;
        m_lfsr_mod = nullptr;

        while (!m_bcu2lfsr_req_fifo.empty()) {
            delete m_bcu2lfsr_req_fifo.back(),m_bcu2lfsr_req_fifo.pop_back();
        }
        while (!m_lfsr2bcu_rst_fifo.empty()) {
            delete m_lfsr2bcu_rst_fifo.back(),m_lfsr2bcu_rst_fifo.pop_back();
        }
        while (!sci_ahb2rca_cfg_data.empty()) {
            delete sci_ahb2rca_cfg_data.back(),sci_ahb2rca_cfg_data.pop_back();
        }
        while (!m_bcu2lfsr_hand_flg_fifo.empty()) {
            delete m_bcu2lfsr_hand_flg_fifo.back(),m_bcu2lfsr_hand_flg_fifo.pop_back();
        }
        while (!m_bcu_last_ofifo_fifo.empty()) {
            delete m_bcu_last_ofifo_fifo.back(),m_bcu_last_ofifo_fifo.pop_back();
        }

    }


    void rca_mod::get_xml_cfg(vector<XmlResolver *>  rca_xml_slv,const string &pro_name)
    {
        //m_rca_ele = rca_ele;
        m_rca_xml_slv = rca_xml_slv;
        for (int i = 0; i < m_top_cfg.bcu_num; ++i)
        {
            m_bcu_mod[i]->get_xml_resolver(m_rca_xml_slv[i]);
			//m_bcu_mod[i]->get_xml_cfg(m_rca_ele);
        }
        m_lfsr_mod->get_xml_resolver(m_rca_xml_slv, pro_name);

    }

	vector<bcu_mod *>* rca_mod::getBcu_mod()
	{
        return &m_bcu_mod;
    }

    void rca_mod::set_source_map(const map<string, string> *source_map)
    {
        for(auto* bcu_mod : m_bcu_mod)
        {
            bcu_mod->set_source_map(source_map);
        }
    }
}
