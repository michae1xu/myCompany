#ifndef __BFU_H__
#define __BFU_H__

#include "../struct_def.h"
#include "../bcu/bcu.h"
#include "../rca/rca.h"

namespace rpu_esl_model
{
    class bcu_mod;


    class bfu_mod : public sc_module
    {
    public:
        bfu_mod(sc_module_name name, top_cfg total_top_cfg, int rcu_id,int bfu_id, bcu_mod *mod_t);
        SC_HAS_PROCESS(bfu_mod);
        ~bfu_mod();

        bfu_mod(const bfu_mod& r) = delete;
        bfu_mod& operator= (const bfu_mod& r) = delete;

        void data_process_mth();
        void mod_init(int rcu_id, int bfu_id);
        void rcv_data_pro();
        void stat_pro();
        void print_info();
        void run_process(int bcu_id, int rcu_id, int bfu_id, XMLElement * bfu_ele,int print_num);

        void get_xml_cfg(XMLElement *rpu_root);
        void get_xml_resolver(XmlResolver *xml_resolver);
        void set_bcu_cfg(bcu_cfg_data);

        int* bfu(int a, int b, int t, const char *func, char *exp, int bypass, char *bypassExp, int mode , int print_num);

        sc_in_clk    chip_clk;


        vector<int >  m_bfu_out;
        vector<int >  m_bfu_last_out;


        XmlResolver *m_rca_xml_slv;

		bool m_bfu_out_valid;


        int input_a_in;
        int input_b_in;
        int input_t_in;


    private:
        top_cfg  m_top_cfg;
        deque    <rpu_data_struct >			      m_latency_que;
        vector   <vector  <bcu_cfg_data >   >     m_cfg_ram;
        vector   <bcu_cfg_data >                  m_tcm;
        vector   <bcu_cfg_data >                  m_cfg_reg;
        vector   <bcu_cfg_data >                  m_sbox;
        vector   <bcu_cfg_data >                  m_benes_ram;
        vector   <bcu_cfg_data >                  m_stream_reg;

        vector   <bcu_cfg_data>                 pe_cfg_data;
        vector   <int >                         core_result;
        int                                     m_cycle_cnt;

        int  m_t_latency_que_sum_size;
        int  m_t_latency_que_max;
        int  m_t_latency_que_min;


        int  m_p_latency_que_sum_size;
        int  m_p_latency_que_max;
        int  m_p_latency_que_min;

        int   print_period;
        int   m_pc_addr;
        int   m_fetch_addr;
        int   m_jump_type;
        int   m_branch_jump_addr;
        int   m_direct_jump_addr;
        int   m_advanced_jump_addr;

        //ofstream    m_bfu_debug_log;


        bcu_mod * m_bcu_mod = nullptr;

		//ºƒ¥Ê ‰»Î
		deque <int >  m_last_in_queue_a;
		deque <int >  m_last_in_queue_b;
		deque <int >  m_last_in_queue_t;

		int m_bfu_id;
		int m_rcu_id;

        //static int s_newCount;


    };
}
#endif
