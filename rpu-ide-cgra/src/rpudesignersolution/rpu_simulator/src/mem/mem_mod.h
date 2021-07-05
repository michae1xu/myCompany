#ifndef __MEM_MOD_H__
#define __MEM_MOD_H__

#include "../struct_def.h"
#include "../bcu/bcu.h"
namespace rpu_esl_model
{

    class bcu_mod;

    class mem_mod : public sc_module
    {
    public:
		mem_mod(sc_module_name name, top_cfg total_top_cfg, int mem_id, bcu_mod *mod_t);
        SC_HAS_PROCESS(mem_mod);
        ~mem_mod();

        void data_process_mth();
        void mod_init(int benes_id);
        void mem_run_process(int rcu_id);

        void stat_pro();
        void print_info();

        void get_xml_cfg(int bcu_id, int rcu_id);

        void run_process(int bcu_id, int rcu_id, int mem_id, XMLElement * mem_ele);

        void get_xml_resolver(XmlResolver *xml_resolver);

		char * del_char(char* str, char ch);

		ESIM_RET get_mem_file_name(string mem_name);


		void mem_read_addr_update();
		void mem_write_addr_update();

		void mem_read_addr_reset();

        sc_process_handle t;

		//void update_sreg( int in0,  int in1,  int in2,  int in3, int word_enable, int dest0, int dest1, int dest2, int dest3,int mode);

        sc_in_clk  sci_chip_clk;

        XmlResolver *m_rca_xml_slv;

  //      vector<int> m_mem_out;

		//vector<int> m_mem_last_out;

        //vector<int> convert;

		vector<int > m_mem_read_addr;


		vector<int > m_mem_read_addr_oring;

		vector<int >            m_mem_ram;

		bool m_need_update_read_addr;
		bool m_need_update_write_addr;

		bool m_intel_num_add;

		//每次循环自动+offset，循环结束清零。
		int  m_intel_read_offset;
		int  m_intel_write_offset;


		int*  dec2hex(int input_data);



    private:

        top_cfg  m_top_cfg;

        bcu_mod * m_bcu_mod;
        deque<rpu_data_struct>   *m_ififo;
        deque<rpu_data_struct>   *m_ofifo;

		int m_rcu_id;
		int m_mem_id;
        //int m_cycle_cnt; 


		//ofstream    m_mem_debug_log;

		bool  m_mem_out_valid;

		vector<deque<int> > m_last_in_que;

		vector<deque<int> > m_last_in_que_latency;

		vector<deque<int> > m_write_mem_que;

		vector<int> m_mem_out_data;
		vector<int> m_last_mem_out_date;

		//read
	public:
		int m_mem_read_mode;
		int m_mem_read_offset;
		int m_mem_read_thd;
		bool m_mem_read_128or32;

		int m_mem_write_offset;
		int m_mem_write_mode;
		int m_mem_write_mask;
		int m_mem_write_thd ;
		int m_mem_write_addr;
		vector<deque<int > >m_mem_write_addr_org_deque;



		vector<int >  m_dest;

		vector<int > m_in_date_valid;

		int  m_word_enable;

		bool m_is_infifo_used;


		char  *str0;
		char  *str1;
		char  *str2;
		char  *str3;

    };
}
#endif
