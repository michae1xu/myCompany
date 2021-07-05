#ifndef __LFSR_H__
#define __LFSR_H__

#include "../struct_def.h"
#include "../bcu/bcu.h"
namespace rpu_esl_model
{

    class bcu_mod;

    class lfsr_mod : public sc_module
    {
    public:
        lfsr_mod(sc_module_name name, top_cfg total_top_cfg, int lfsr_id);
        SC_HAS_PROCESS(lfsr_mod);
        ~lfsr_mod();

        void data_process_mth();
        void mod_init(int lfsr_id);
        void lfsr_run_process();

        void stat_pro();
        void print_info();

        void get_xml_cfg(int bcu_id, int rcu_id, XMLElement * lfsr_ele);

        void run_process(int bcu_id, int rcu_id, int sreg_id, XMLElement * lfsr_ele);

        void get_xml_resolver(vector<XmlResolver* > xml_resolve,const string &pro_name);

		char * del_char(char* str, char ch);

		void update_lfsr(int mode);

        sc_in_clk  sci_chip_clk;

        vector<sc_fifo_in<uint32_t>  *> sci_rcu2lfsr_req;
        vector<sc_fifo_out<lfsr_send_data_t> *> sco_lfsr2lrcu_rst;

        vector<deque<uint32_t> *>  m_rcv_rcu_req_que;
        deque<vector<uint32_t> >  m_send_rcu_rst_que;

        //Í¬²½ÐÅºÅ
        vector<sc_fifo_in<uint32_t>  *> sci_rcu2lfsr_hand_flg;
        vector<deque<uint32_t> *>  m_rcv_bcu_hand_flg;

        vector<XmlResolver * >m_rca_xml_slv;

		vector<int> m_sreg_out_data;
		vector<int> m_last_sreg_out_date;

        vector<int>  m_bcu_syn_flg;
        vector<int >  m_need_syn_bcu;
        vector<int >  m_syn_ready_flg;
        bool          m_all_date_reay;

    private:

        top_cfg  m_top_cfg;

        //bcu_mod * m_bcu_mod;
        ofstream m_cout_out_log;


		int m_rcu_id;
		int m_lfsr_id;

		//ofstream    m_lfsr_debug_log;

        int  m_lfsr_mode;
        bool  m_lfsr_out_mode;
		bool  m_lfsr_out_valid;

        vector<uint32_t >  m_lfsr_out_date;
        vector<uint32_t >  m_lfsr_link_date;


		int  m_word_enable;

        int   m_cycle_cnt;

    };
}
#endif
