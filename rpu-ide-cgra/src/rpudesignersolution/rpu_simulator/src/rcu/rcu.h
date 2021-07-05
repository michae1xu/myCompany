#ifndef __RCU_H__
#define __RCU_H__

#include "../struct_def.h"
#include "../bfu/bfu.h"
#include "../sbox/sbox.h"
#include "../benes/benes_mod.h"
namespace rpu_esl_model
{
    class bfu_mod;
    class sbox_mod;
    class benes_mod;
	class sreg_mod;

    class rcu_mod : public sc_module
    {
    public:
        rcu_mod(sc_module_name name, top_cfg total_top_cfg, int rcu_id,  bcu_mod * mod_t);
        SC_HAS_PROCESS(rcu_mod);
        ~rcu_mod();
        void data_process_mth();
        void mod_init(bcu_mod * mod_t);
        void run_process(int bcu_id, int rcu_id, XMLElement * rcu_ele);

        void stat_pro();
        void print_info();
        void get_xml_cfg(XMLElement *rpu_ele);
        void get_xml_resolver(XmlResolver *xml_resolver);

		void run_ofifo_process(int bcu_id, int rcu_id, int out_fifo_id,XMLElement * rcu_ele,int print_num);



        sc_in_clk    chip_clk;

        //sc_fifo_in<rpu_struct >  			   sci_ringmst2dis_req;

        //   vector<sc_fifo_out<rpu_struct > * >    sco_dis2parser_req;


    public:

        top_cfg  m_top_cfg;

        vector  <bfu_mod *>     m_bfu_mod;
        vector  <sbox_mod *>    m_sbox_mod;
        vector<benes_mod * >    m_benes_mod;


        deque<rpu_struct >      m_cmd_fifo;

        int						m_rcu_id;


        XMLElement * m_rcu_ele;
        XMLElement * m_bfu_ele;
        XMLElement * m_benes_ele;
        XMLElement * m_sbox_ele;
		XMLElement * m_ofifo_ele;
		XMLElement * m_sreg_ele;



        int  m_t_cmd_fifo_sum_size;
        int  m_t_cmd_fifo_max;
        int  m_t_cmd_fifo_min;

        int  m_p_cmd_fifo_sum_size;
        int  m_p_cmd_fifo_max;
        int  m_p_cmd_fifo_min;

        int   print_period;


        //ofstream    rcu_log;
		//ofstream    rcu_deug_log;

		//int m_mem_offset;
		//int m_mem_mod;
		//int m_mem_step;
		//int m_mem_128or32;


        XmlResolver *m_rca_xml_slv;

		bcu_mod * m_bcu_mod;

        int m_cycle_cnt;

		// out fifo Êä³ö
		vector<int > m_out_fifo_type;
		vector<int > m_out_fifo_index;
		vector<int > m_out_fifo_data;

        std::shared_ptr<vector<int>> getOut_fifo_data();

		int m_print_ctrl_num;


    };
}
#endif
