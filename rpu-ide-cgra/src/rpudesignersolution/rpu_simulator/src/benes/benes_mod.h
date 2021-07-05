#ifndef __BENES_MOD_H__
#define __BENES_MOD_H__

#include "../struct_def.h"
#include "../bcu/bcu.h"
namespace rpu_esl_model
{

    class bcu_mod;

    class benes_mod : public sc_module
    {
    public:
        benes_mod(sc_module_name name, top_cfg total_top_cfg, int rcu_id,int benes_id, bcu_mod *mod_t);
        SC_HAS_PROCESS(benes_mod);
        ~benes_mod();

        void data_process_mth();
        void mod_init(int rcu_id, int benes_id);
        void rr_process();

        void stat_pro();
        void print_info();
        void get_xml_cfg(XMLElement *ele);

        void run_process(int bcu_id, int rcu_id, int benens_id, XMLElement * benes_ele,int print_num);

        int* benes(int a, int b, int c, int d, const char *src, int print_num);

        int * benes_2(int *in, bool *cfg);

        int * benes_n(int *in, bool* cfg, int width);

        void get_xml_resolver(XmlResolver *xml_resolver);

		char * del_char(char* str, char ch);

        sc_in_clk  sci_chip_clk;

        XmlResolver *m_rca_xml_slv;

        vector<int> m_benes_out;

        vector<int> m_benes_last_out;
        vector<int> convert;
        vector<uint32_t> m_input_benes;

    private:

        top_cfg  m_top_cfg;

        bcu_mod * m_bcu_mod;
        deque<rpu_data_struct>   *m_ififo;
        deque<int>   *m_ofifo;

		int m_rcu_id;
		int m_benes_id;
        //int m_cycle_cnt; 

		//ofstream    m_benes_debug_log;

		bool  m_benes_out_valid;

		vector<deque<int> > m_last_in_que;

      
    };
}
#endif
