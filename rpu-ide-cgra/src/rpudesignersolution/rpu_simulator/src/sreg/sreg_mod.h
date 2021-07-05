#ifndef __SREG_MOD_H__
#define __SREG_MOD_H__

#include "../struct_def.h"
#include "../bcu/bcu.h"
namespace rpu_esl_model
{

    class bcu_mod;

    class sreg_mod : public sc_module
    {
    public:
		sreg_mod(sc_module_name name, top_cfg total_top_cfg, int sreg_id, bcu_mod *mod_t);
        SC_HAS_PROCESS(sreg_mod);
        ~sreg_mod();

        void data_process_mth();
        void mod_init(int benes_id);
        void sreg_run_process();

        void stat_pro();
        void print_info();

        void get_xml_cfg(int bcu_id, int rcu_id, XMLElement * sreg_ele);

        void run_process(int bcu_id, int rcu_id, int sreg_id, XMLElement * sreg_ele);

        void get_xml_resolver(XmlResolver *xml_resolver);

		char * del_char(char* str, char ch);

		void update_sreg( int in0,  int in1,  int in2,  int in3, int word_enable, int dest0, int dest1, int dest2, int dest3,int mode);

        sc_in_clk  sci_chip_clk;

        XmlResolver *m_rca_xml_slv;

        //vector<int> m_sreg_out;

		//vector<int> m_sreg_last_out;

		vector<int> m_sreg_out_data;
		vector<int> m_last_sreg_out_date;

        //vector<int> convert;
    private:

        top_cfg  m_top_cfg;

        bcu_mod * m_bcu_mod;
        deque<rpu_data_struct>   *m_ififo;
        deque<rpu_data_struct>   *m_ofifo;

		int m_rcu_id;
		int m_sreg_id;
        //int m_cycle_cnt; 

		//ofstream    m_sreg_debug_log;

		bool  m_sreg_out_valid;

		vector<deque<int> > m_last_in_que;



		int m_rch_mode;
		vector<int >  m_dest;

		vector<int > m_in_date_valid;

		int  m_word_enable;

		bool m_is_infifo_used;
    };
}
#endif
