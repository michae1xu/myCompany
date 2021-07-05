#ifndef __RCA_H__
#define __RCA_H__

#include "../struct_def.h"
#include "../bcu/bcu.h"
#include "../lfsr/lfsr.h"

namespace rpu_esl_model
{
    class bcu_mod;
    class lfsr_mod;
    class rca_mod : public sc_module
    {
    public:
        rca_mod(sc_module_name name, int rca_id, top_cfg total_top_cfg);
        SC_HAS_PROCESS(rca_mod);
        ~rca_mod();
        void data_process_mth();
        void mod_init();

        void get_xml_cfg(vector<XmlResolver *> rca_xml_slv, const string &pro_name);

        void get_xml_name(const string &xml_name);
        ESIM_RET get_mem_file_name(const string &xml_name);
        void get_rch_file_name(const string &xml_name);
        ESIM_RET get_imd_file_name(const string &xml_name);

        bool get_simu_end_status();

        sc_in_clk    chip_clk;
        vector<sc_fifo_in<rpu_data_struct>  *>  sci_ahb2rca_cfg_data;

        bool                               m_bcu_end_status;


    private:
        top_cfg  m_top_cfg;

        vector<bcu_mod * >                  m_bcu_mod;
        lfsr_mod *                          m_lfsr_mod = nullptr;

        deque<rpu_struct >			        m_data_mst_latency_que;

        deque<rpu_struct >			        m_parser_latency_que;

        vector<sc_fifo<uint32_t>   *>           m_bcu2lfsr_req_fifo;
        vector<sc_fifo< lfsr_send_data_t>   *>  m_lfsr2bcu_rst_fifo;

        vector<sc_fifo<uint32_t>   *>           m_bcu2lfsr_hand_flg_fifo;

        vector<sc_fifo<uint32_t>   *>           m_bcu_last_ofifo_fifo;


        deque<lfsr_send_data_t>            m_stop_latency_que;

        int						            m_total_parser_num;
        int                                 m_cycle_cnt;
        int                                 m_rca_id;

        vector<XmlResolver *> m_rca_xml_slv;

        XMLElement   *m_rca_ele;


		string   m_xml_name;
		string   m_mem_name;
		string   m_imd_name;
		string   m_rch_name;

	public:
		vector<bcu_mod *> *getBcu_mod();

        void set_source_map(const map<string, string> *source_map);
    };
}

#endif
