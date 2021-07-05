#ifndef __GENERATOR_H__
#define __GENERATOR_H__

#include "systemc.h"
#include "struct_def.h"
#include "sysc/kernel/sc_time.h"
#include "rca/rca.h"

namespace rpu_esl_model
{
    class generator : public sc_module
    {
    public:
        generator(sc_module_name name, sc_time simu_time,int bcu_num);
        SC_HAS_PROCESS(generator);
        ~generator();
        void gen_pkt();
        void rcv_pkt();
        void print_simu_info();
        void run_process_mth();
        void set_debug_by_step_mode(bool mode);

		ESIM_RET get_source_xml(vector<string > source_name);

        void notify_debug_by_step_test();

        void set_rca_ptr(rca_mod *rca_mod_ptr);

        sc_in_clk  chip_in_clk;
        vector<sc_fifo_out<rpu_data_struct > *>  sco_gen2chip_pkt;
        sc_fifo_in <rpu_struct >   sci_chip2gen_pkt;
        sc_fifo_out<rpu_struct >   sco_recycle_ring_addr;

        int                    m_pkt_id;
        sc_time                m_simu_time;
        int m_cycle_cnt;

        int m_bcu_num;

        sc_process_handle t;

        //ofstream    output_debug_log;
        //ofstream    output_stat_log;

		vector<deque<U32 >  >m_source_date;
 

        rca_mod         *m_rca_mod;

        int  m_p2p_max_laetency;
        int  m_p2p_min_laetency;
        double  m_p2p_latecny_sum;
        int  rcv_pkt_cnt;


		vector<U32> in_date;
		string      m_input_date_file;

		vector<U32 > m_input_date ;

        sc_event m_one_step_event;

        bool m_on_step_mode;

    };
}
#endif
