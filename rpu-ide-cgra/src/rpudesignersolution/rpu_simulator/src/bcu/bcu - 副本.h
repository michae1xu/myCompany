#ifndef __BCU_H__
#define __BCU_H__

#include "../struct_def.h"
#include "../rcu/rcu.h"
#include "../sreg/sreg_mod.h"
#include "../mem/mem_mod.h"


namespace rpu_esl_model
{

    class rcu_mod;
	class mem_mod;

    class bcu_mod : public sc_module
    {
    public:
        bcu_mod(sc_module_name name, top_cfg total_top_cfg, int bcu_id);
        SC_HAS_PROCESS(bcu_mod);
        ~bcu_mod();
        void data_process_mth();

		void notify_process_mth();


        void mod_init();
        void rcv_data_pro();
        void stat_pro();
        void print_info();
        void run_process(int bcu_id, XMLElement * bcu_ele);
        void get_xml_resolver(XmlResolver *xml_resolver, XMLElement *rca_ele);

		void loop_process(XMLElement *m_rcu_ele, int rcu_id);

        void get_xml_cfg(XMLElement *rpu_root);
        void set_bcu_cfg(bcu_cfg_data);
        void fetch_cfg(int base_addr);
        void decode_cfg();
		
		ESIM_RET get_mem_file_name(string mem_name);
		ESIM_RET get_imd_file_name(string imd_file_name);

		void notify_debug_by_step_test();
        void set_debug_by_step_mode(bool mode = false);

public:
		int* get_lastorder_id();
		vector<int> *get_mem_data();

		//sc_event m_continue_envent;

        sc_in_clk    chip_clk;
        //sc_fifo_in <rpu_data_struct >		sci_crsbar2core_data;
        //sc_fifo_out<rpu_data_struct >		sco_core2crsbar_req_data;
        //sc_fifo_out<rpu_data_struct >		sco_core2crsbar_res_data;

        sc_fifo_in<rpu_data_struct>         sci_ahb2bcu_cfg_data;


    public:
        top_cfg  m_top_cfg;
        vector<rcu_mod * > m_rcu_mod;

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
		int                                     m_cycle_cnt_notify;


		int  m_bcu_id;

        deque<rpu_data_struct>       *m_ififo;
        deque<int>       *m_ofifo;

        vector<int >            m_mem;

		deque<U32 >            m_imd;

		sreg_mod               *m_sreg_mod;
		mem_mod                *m_mem_mod;

		bool                    m_is_used_imd;
		bool                    m_is_used_infifo;

		int					    m_imd_mode;


		vector<int>             m_rcuxmlid2need_map;
		vector<int>             m_needid2rcuxml_map;

		int                     m_need_id;

		vector<int>             m_rcuid2orderid_map;
		vector<int>             m_orderid2rcuid_map;


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

        XmlResolver *m_rca_xml_slv;

		XMLElement *m_rca_ele;

        XMLElement * m_rcu_ele;
		XMLElement * m_rcu_ele_record;

		XMLElement * m_sreg_ele;
		XMLElement * m_mem_ele;

		XMLElement * m_loop_ele;

		//һ��ѭ��
		vector<int > m_loop_mode;
		vector<int > m_loop_start;
		vector<int > m_loop_end;
		vector<int > m_loop_times;
		vector<int > m_loop_enable;

		int   m_loop_start_rcu_id;
		int   m_loop_end_rcu_id;
		int   m_loop_real_rcu_id;

		//����ѭ��
		vector<int > m_loop_mode_2;
		vector<int > m_loop_times_2;
		vector<int > m_loop_start_2;
		vector<int > m_loop_end_2;

		int   m_loop_start_rcu_id_2;
		int   m_loop_end_rcu_id_2;
		int   m_loop_real_rcu_id_2;


		sc_event m_one_step_test;
		bool m_on_step_mode;

		int m_rcu_order_id ;

		bool m_bcu_simu_end;

		bool m_first_point_flg;
		vector<int> m_first_loop_flg;

		vector<int> m_infifo_pre_read;

        ofstream    core_bcu_log;

		string m_mem_name;
		string m_imd_file_name;

		vector<U32> in_date;

		vector<int >  m_in_loop_1;
        vector<int >  m_in_loop_2;

		vector<rcu_mod *> *getRcu_mod();

        const map<string, string> *m_source_map;

        void set_source_map(const map<string, string> *source_map);
        const map<string, string> *get_source_map() const;

		ofstream m_cout_out_log;
    };
}
#endif
