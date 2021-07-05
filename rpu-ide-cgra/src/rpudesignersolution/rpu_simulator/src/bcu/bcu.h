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
		void lfsr_send_process();
        void lfsr_rcv_process();
        void mod_init();
        void rcv_data_pro();
        void stat_pro();
        void print_info();
        void run_process(int bcu_id, XMLElement * bcu_ele);
        void get_xml_resolver(XmlResolver *xml_resolver);
		void loop_process(XMLElement *m_rcu_ele, int rcu_id);
        void get_xml_cfg(XMLElement *rpu_root);
        void set_bcu_cfg(bcu_cfg_data);

        void send_date_to_next_bcu();

        bool get_simu_end_status();

		ESIM_RET get_mem_file_name(string mem_name);
		ESIM_RET get_imd_file_name(string imd_file_name);

		void notify_debug_by_step_test();
        void set_debug_by_step_mode(bool mode = false);
        void lfsr_process(int bcu_id, int rcu_id);

		int* get_lastorder_id();
        std::shared_ptr<vector<int> > get_mem_data();

        sc_process_handle t;

        sc_in_clk    chip_clk;
        sc_fifo_out<uint32_t >		            sco_bcu2lfsr_req_data;
        sc_fifo_in<lfsr_send_data_t >		    sci_lfsr2lbcu_rst_data;
        sc_fifo_in<rpu_data_struct>             sci_ahb2bcu_cfg_data;
        sc_fifo_out<uint32_t >		            sco_bcu2lfsr_hand_flag;

        //多核通道
        sc_fifo_in<uint32_t>                    sci_last_bcu_in;
        sc_fifo_out<uint32_t>                   sco_next_bcu_in;
        

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
        int  m_real_bcu_id;

        deque<rpu_data_struct>       *m_ififo = nullptr;
        deque<int>                   *m_ofifo = nullptr;
        vector<int>            m_mem;
        deque<U32>             m_imd;
        sreg_mod               *m_sreg_mod = nullptr;
        mem_mod                *m_mem_mod = nullptr;

		bool                    m_is_used_imd;
        bool                    m_is_used_lfsr;
		bool                    m_is_used_infifo;
		int					    m_imd_mode;
        int                     m_need_id;

		vector<int>             m_rcuxmlid2need_map;
		vector<int>             m_needid2rcuxml_map;
		vector<int>             m_rcuid2orderid_map;
		vector<int>             m_orderid2rcuid_map;

        //lfsr
        //deque<vector<uint32_t> >   m_rcv_lfsr_rst_que;
        vector<uint32_t>  m_rcv_lfsr_rst_que;
        deque<int>        m_send_lfsr_req_que;
        deque<uint32_t>   m_send_lfsr_hand_flg;


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
        tinyxml2::XMLDocument *m_document;

		//内层循环
		vector<int > m_loop_mode;
		vector<int > m_loop_start;
		vector<int > m_loop_end;
		vector<int > m_loop_times;
		vector<int > m_loop_enable;

		int   m_loop_start_rcu_id;
		int   m_loop_end_rcu_id;
		int   m_loop_real_rcu_id;

		//外层循环
		vector<int > m_loop_mode_2;
		vector<int > m_loop_times_2;
		vector<int > m_loop_start_2;
		vector<int > m_loop_end_2;

		int   m_loop_start_rcu_id_2;
		int   m_loop_end_rcu_id_2;
		int   m_loop_real_rcu_id_2;

        vector<int >  m_in_loop_1;
        vector<int >  m_in_loop_2;
        //单步调试控制
		sc_event m_one_step_event;
		bool m_on_step_mode;
        deque<int> m_step_control;

		int m_rcu_order_id ;
		bool m_bcu_simu_end;
		bool m_first_point_flg;
		vector<int> m_first_loop_flg;
		vector<U32> m_infifo_pre_read;
        vector<U32> m_infifo_last_bcu;


        ofstream    core_bcu_log;
		string m_mem_name;
		string m_imd_file_name;
		vector<U32> in_date;
		vector<rcu_mod *> *getRcu_mod();
        const map<string, string> *m_source_map;
        void set_source_map(const map<string, string> *source_map);
        const map<string, string> *get_source_map() const;
		ofstream m_cout_out_log;

        //多核
        int m_infifo_src;
        int m_outfifo_target;

        bool m_sim_end_status;
    };
}
#endif
