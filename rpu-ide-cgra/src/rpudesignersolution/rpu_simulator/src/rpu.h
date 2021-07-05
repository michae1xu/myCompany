#ifndef __RPU_H__
#define __RPU_H__

#include "systemc.h"
#include "struct_def.h"
#include "sysc/communication/sc_fifo.h"
#include "./rca/rca.h"
#include "generator.h"
#include "rpu_wrapper.h"

namespace rpu_esl_model
{
    class rpu : public sc_module
    {
    public:
        rpu(sc_module_name name,int bcu_num, int rcu_num = 256);
        SC_HAS_PROCESS(rpu);
        ~rpu();
        void rcv_pkt();
        void send_pkt();
        void module_init(int bcu_num,int rcu_num);
        void module_connect();

        vector<sc_fifo_in<rpu_data_struct > *>  sci_gen2chip_pkt;
        sc_fifo_out<rpu_struct >   sco_chip2gen_pkt;
        sc_fifo_in<rpu_struct >    sci_recycle_ring_addr;


		ESIM_RET get_xml_name(vector<string > xml_name, const string &pro_name);
		ESIM_RET get_mem_file_name(const string& xml_name);
		void get_rch_file_name(const string& xml_name);
		ESIM_RET get_imd_file_name(const string& xml_name);


        rca_mod *   m_rca_mod = nullptr;

        top_cfg             *m_top_cfg = nullptr;

        tinyxml2::XMLDocument *m_document = nullptr;
        XMLElement *m_rca_ele;

        sc_in_clk  chip_in_clk;

        sc_clock  *chip_clk;

        sc_fifo<rpu_struct>                    m_ring_ctrl2cmd_crtl_fifo;
        vector<sc_fifo<parser2cmd_ctrl_struct> *>   m_parser2cmdctrl_req_fifo;
        sc_fifo<int >                               m_ring_mst_ready_fifo;
        sc_fifo<int >                               m_cmd_mst_ready_fifo;
        sc_fifo<int >                               m_data_mst_ready_fifo;
        sc_fifo<rpu_struct >                   m_cmd_ctrl2ring_master_fifo;
        sc_fifo<rpu_struct >                   m_cmd_ctrl2cmd_master_fifo;
        sc_fifo<rpu_struct >                   m_cmd_ctrl2data_master_fifo;

        sc_fifo<rpu_struct >                   m_cmd_mst2pcie_response_fifo;
        sc_fifo<rpu_struct >                   m_data_mst2pcie_response_fifo;
        sc_fifo<rpu_struct >                   m_ring_mst2pcie_response_fifo;

        sc_fifo<rpu_struct>                    m_ring_mst2dis_req_fifo;
        vector<sc_fifo<rpu_struct>   *>        m_dis2parser_req_fifo;

        vector<sc_fifo<rpu_struct >  *>        m_pcie2ring_ctrl_req_fifo;
        sc_fifo<rpu_struct >                   m_pcie2ring_addr_req_fifo;
        sc_fifo<rpu_struct >                   m_pcie2ring_addr_recycle_fifo;

        int m_input_channel_num;
        int m_total_parser_num;
        int m_print_period;

        vector<XmlResolver *> m_rca_xml_slv;

		string   m_xml_name;
		string   m_mem_name;
		string   m_imd_name;
		string   m_rch_name;

        rca_mod *getRca_mod() const;

        void set_source_map(const map<string, string> *source_map);
    };
}
#endif
