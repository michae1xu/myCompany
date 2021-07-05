#ifndef __SBOX_H__
#define __SBOX_H__

#include "../struct_def.h"
#include "../bcu/bcu.h"
namespace rpu_esl_model
{
    class bcu_mod;

    class sbox_mod : public sc_module
    {
    public:
        sbox_mod(sc_module_name name, top_cfg total_top_cfg, int rcu_id, int sbox_id, bcu_mod* mod_t);
        SC_HAS_PROCESS(sbox_mod);
        ~sbox_mod();

        void data_process_mth();
        void mod_init(int rcu_id, int sbox_id);

        void stat_pro();
        void print_info();

        void get_xml_cfg(XMLElement *zorov3_root);

        void crossbar_crtl_mth();
        void men_crtl_mth();
        void ahb_set_cfg();

		char * del_char(char* str, char ch);


        void run_process(int bcu_id, int rcu_id, int sbox_id, XMLElement * sbox_ele, int print_num);

        int  *run_sbox(int a, int mode, const char *src, int id0, int id1, int id2, int id3, int print_num);

        void get_xml_resolver(XmlResolver *xml_resolver);

		//int ch16toi(char ch);
  //      long hex2dec(char* str);

        sc_in_clk    chip_clk;

            XmlResolver *m_rca_xml_slv;

        vector<int> m_sbox_out;
        vector<int> m_sbox_last_out;

		bool m_sbox_out_valid;

        uint32_t   m_sbox_in_date;

    private:
        top_cfg  m_top_cfg;

        deque<rpu_data_struct >			        m_infifo;
        deque<rpu_data_struct >			        m_outfifo;


        vector<deque <rpu_data_struct> *>       m_inbuffer;
        vector<deque <rpu_data_struct> *>       m_outbuffer;

        int                                     m_cycle_cnt;
        int                                     m_mode;
        int                                     m_outbuff_rr_ptr;
        vector<int >                            m_core2crsbar_legth;

        int  m_t_latency_que_sum_size;
        int  m_t_latency_que_max;
        int  m_t_latency_que_min;

        int  m_p_latency_que_sum_size;
        int  m_p_latency_que_max;
        int  m_p_latency_que_min;

        int   print_period;


        //ofstream    m_sbox_debug_log;

        bcu_mod * m_bcu_mod;
        deque<rpu_data_struct>    *m_ififo;
        deque<int>    *m_ofifo;

		deque<int>                m_last_in_que;

		vector<U32> in_date;


		int m_rcu_id ;
		int m_sbox_id ;
    };
}
#endif
