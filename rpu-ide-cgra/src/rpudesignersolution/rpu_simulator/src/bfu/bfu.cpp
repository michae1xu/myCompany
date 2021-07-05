
#include "bfu.h"

namespace rpu_esl_model
{

//int bfu_mod::s_newCount = 0;

    bfu_mod::bfu_mod(sc_module_name name, top_cfg total_top_cfg, int rcu_id,int bfu_id, bcu_mod *mod_t) :sc_module(name), m_top_cfg(total_top_cfg)
    {
        //SC_METHOD(data_process_mth);
        //sensitive << chip_clk.pos();
        //dont_initialize();

		//m_ififo = mod_t->m_ififo;	
		//m_ofifo = mod_t->m_ofifo;
        m_bcu_mod = mod_t;
        mod_init(rcu_id,bfu_id);

        //s_newCount++;
        //printf("buf_mod times: %d\n", s_newCount);
    }


    void bfu_mod::mod_init(int rcu_id , int bfu_id)
    {
        m_cycle_cnt = 0;
        m_pc_addr = 0;
        m_jump_type = NOMARL;
        m_branch_jump_addr = 0;
        m_direct_jump_addr = 0;
        m_advanced_jump_addr = 0;

        //顶层传入
        print_period = m_top_cfg.print_period;
        m_cfg_ram.resize(CFG_RAM_TOTAL_ADDR);
        m_tcm.resize(TRAM_TOTAL_ADDR);
        pe_cfg_data.resize(PE_NUM);
        core_result.resize(PE_NUM);

		string rca_name = "rca";
		string rcu_name = "rcu";
		string bfu_name = "_bfu";
		string a = "_debug.log";

		ostringstream oss;
		oss << rcu_name << rcu_id << bfu_name<< bfu_id <<a;
		//m_bfu_debug_log.open(oss.str());

        m_cfg_ram.resize(CFG_RAM_NUM);
        for (int i = 0; i < CFG_RAM_NUM; ++i)
        {
            m_cfg_ram[i].resize(CFG_RAM_TOTAL_ADDR);
        }

        m_bfu_last_out.resize(BFU_OUT_PORT_NUM, -1);
        m_bfu_out.resize(BFU_OUT_PORT_NUM, -1);

        input_a_in = 0;
        input_b_in = 0;
        input_t_in = 0;

		m_bfu_out_valid = false;
		m_rcu_id = rcu_id;
		m_bfu_id = bfu_id;
    }

    void bfu_mod::rcv_data_pro()
    {
    }

    void bfu_mod::data_process_mth()
    {
        rcv_data_pro();

        m_cycle_cnt++;
    }

    void bfu_mod::stat_pro()
    {

    }

    void bfu_mod::print_info()
    {
        //第一个周期不统计
        if (m_cycle_cnt >= print_period)
        {
            if (0 == (m_cycle_cnt) % print_period)
            {
            }

        }

    }


    bfu_mod::~bfu_mod()
    {
    }

    void bfu_mod::get_xml_cfg(XMLElement *rpu_root)
    {
        if (rpu_root)
        {
            XMLElement *data_mst_ele = rpu_root->FirstChildElement("data_mst");
        }
    }

    void bfu_mod::set_bcu_cfg(bcu_cfg_data)
    {


    }


    void bfu_mod::run_process(int bcu_id, int rcu_id, int bfu_id, XMLElement * bfu_ele,int print_num)
    {
        bool is_infifo_used = false;
        {//bfu

            ElementType element = ElementType::ELEMENT_BFU;
            int element_id = 0;
            if (ElementType::ELEMENT_BFU == element)
            {
                element_id = bfu_id;
            }

            int input_a_type = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::BfuInputA_Type, 0);
            int input_b_type = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::BfuInputB_Type, 0);
            int input_t_type = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::BfuInputT_Type, 0);
            int input_a_index = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::BfuInputA_Index, 0);
            int input_b_index = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::BfuInputB_Index, 0);
            int input_t_index = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::BfuInputT_Index, 0);


            //从对应的Iputtype对应的index端口取值
            int input_a = -1;
            int input_b = -1;

			//如何确定last_rcu_id ?index 和实际bfu编号的转换,map
			int rcu_order_id = m_bcu_mod->m_rcuid2orderid_map[rcu_id];
			int last_order_id = m_bcu_mod->m_rcuid2orderid_map[rcu_id] - 1;
			int last_rcu_id = 0;
			if (last_order_id > 0)
			{
				last_rcu_id = m_bcu_mod->m_orderid2rcuid_map[last_order_id];
			}		

			int sbox_id = input_a_index / SBOX_OUT_PORT_NUM;
			int sbox_out_port_id = input_a_index % SBOX_OUT_PORT_NUM;
			//benes_id 必为0 
			int benes_id = input_a_index / BENES_OUT_PORT_NUM;
            switch (input_a_type)
            {
            case InputPort_NULL:
                input_a = 0;
				m_last_in_queue_a.push_back(input_a);
                break;
            case InputPort_CurrentBfuX:
				//if (m_bcu_mod->m_rcu_mod[rcu_id]->m_bfu_mod[input_a_type]->m_bfu_out_valid)
				//{
					input_a = m_bcu_mod->m_rcu_mod[rcu_order_id]->m_bfu_mod[input_a_index]->m_bfu_out[0];
					m_last_in_queue_a.push_back(input_a);
				//}
				break;
            case InputPort_CurrentBfuY:
				//if (m_bcu_mod->m_rcu_mod[rcu_id]->m_bfu_mod[input_a_type]->m_bfu_out_valid)
				//{
					input_a = m_bcu_mod->m_rcu_mod[rcu_order_id]->m_bfu_mod[input_a_index]->m_bfu_out[1];
					m_last_in_queue_a.push_back(input_a);
					break;
				//}
            case InputPort_CurrentSbox:
                input_a = m_bcu_mod->m_rcu_mod[rcu_order_id]->m_sbox_mod[sbox_id]->m_sbox_out[sbox_out_port_id];
				m_last_in_queue_a.push_back(input_a);
				break;
            case InputPort_CurrentBenes:
				//benes_id 必为0 
				input_a = m_bcu_mod->m_rcu_mod[rcu_order_id]->m_benes_mod[benes_id]->m_benes_out[input_a_index];
				m_last_in_queue_a.push_back(input_a);
				break;
            case InputPort_LastBfuX:
				//if (m_bcu_mod->m_rcu_mod[rcu_id]->m_bfu_mod[input_a_type]->m_bfu_out_valid)
				//{
					input_a = m_bcu_mod->m_rcu_mod[last_order_id]->m_bfu_mod[input_a_index]->m_bfu_out[0];
					m_last_in_queue_a.push_back(input_a);
					break;
				//}
            case InputPort_LastBfuY:
				//if (m_bcu_mod->m_rcu_mod[rcu_id]->m_bfu_mod[input_a_type]->m_bfu_out_valid)
				//{
					input_a = m_bcu_mod->m_rcu_mod[last_order_id]->m_bfu_mod[input_a_index]->m_bfu_out[1];
					m_last_in_queue_a.push_back(input_a);
					break;
				//}
            case InputPort_LastBenes:
				input_a = m_bcu_mod->m_rcu_mod[last_order_id]->m_benes_mod[benes_id]->m_benes_out[input_a_index];
				m_last_in_queue_a.push_back(input_a);
				break;
            case InputPort_LastSbox:
				input_a = m_bcu_mod->m_rcu_mod[last_order_id]->m_sbox_mod[sbox_id]->m_sbox_out[sbox_out_port_id];
				m_last_in_queue_a.push_back(input_a);
				break;
            case InputPort_InFifo:
                //从input fifo中获取值，移步 bfu 里做还是rcu里做，需要考虑一下
				//if (m_bcu_mod->m_ififo->size() > 0)
				//{
				//	input_a = m_bcu_mod->m_ififo->front().input_data;
				//	m_last_in_queue_a.push_back(input_a);
				//	m_bcu_mod->m_is_used_infifo = true;
				//	if (print_num >= 3)
				//	{
				//		m_bcu_mod->m_ififo->pop_front();
				//	}
				//	break;
				//}
				input_a = m_bcu_mod->m_infifo_pre_read.at(input_a_index);
				m_last_in_queue_a.push_back(input_a);
				m_bcu_mod->m_is_used_infifo = true;
				break;
            case InputPort_Mem:
                //ElementType tmp_element = ELEMENT_OUTMEM;
                //int mem_offset = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, tmp_element, 0, AttributeID::RCUMEMRDOFFSET, 0);
                input_a = m_bcu_mod->m_mem_mod->m_mem_ram[m_bcu_mod->m_mem_mod->m_mem_read_addr[input_a_index]];
				m_last_in_queue_a.push_back(input_a);

                break;
			case InputPort_Imd:
				input_a = m_bcu_mod->m_imd[input_a_index];
				m_last_in_queue_a.push_back(input_a);
				m_bcu_mod->m_is_used_imd = true;

				break;
			case InputPort_Rch:
				input_a = m_bcu_mod->m_sreg_mod->m_sreg_out_data[input_a_index];
				m_last_in_queue_a.push_back(input_a);
				break;
            case InputPort_Lfsr:
                if (m_bcu_mod->m_rcv_lfsr_rst_que.size()>0)
                {
                    input_a = m_bcu_mod->m_rcv_lfsr_rst_que.at(input_a_index);
                    m_last_in_queue_a.push_back(input_a);
                    m_bcu_mod->m_is_used_lfsr = true;
                }
                break;
            }

			sbox_id = input_b_index / SBOX_OUT_PORT_NUM;
			sbox_out_port_id = input_b_index % SBOX_OUT_PORT_NUM;
			//benes_id 必为0 
			benes_id = input_b_index / BENES_OUT_PORT_NUM;

            switch (input_b_type)
            {
            case InputPort_NULL:
                input_b = 0;
				m_last_in_queue_b.push_back(input_b);
                break;
            case InputPort_CurrentBfuX:
                input_b = m_bcu_mod->m_rcu_mod[rcu_order_id]->m_bfu_mod[input_b_index]->m_bfu_out[0];
				m_last_in_queue_b.push_back(input_b);
                break;
            case InputPort_CurrentBfuY:
                input_b = m_bcu_mod->m_rcu_mod[rcu_order_id]->m_bfu_mod[input_b_index]->m_bfu_out[1];
				m_last_in_queue_b.push_back(input_b);
				break;
            case InputPort_CurrentSbox:
				input_b = m_bcu_mod->m_rcu_mod[rcu_order_id]->m_sbox_mod[sbox_id]->m_sbox_out[sbox_out_port_id];
				m_last_in_queue_b.push_back(input_b);
				break;
            case InputPort_CurrentBenes:
				//benes_id 必为0 
				input_b = m_bcu_mod->m_rcu_mod[rcu_order_id]->m_benes_mod[benes_id]->m_benes_out[input_b_index];
				m_last_in_queue_b.push_back(input_b);
				break;
            case InputPort_LastBfuX:
				input_b = m_bcu_mod->m_rcu_mod[last_order_id]->m_bfu_mod[input_b_index]->m_bfu_out[0];
				m_last_in_queue_b.push_back(input_b);
				break;
            case InputPort_LastBfuY:
				input_b = m_bcu_mod->m_rcu_mod[last_order_id]->m_bfu_mod[input_b_index]->m_bfu_out[1];
				m_last_in_queue_b.push_back(input_b);
				break;
            case InputPort_LastBenes:
				//benes_id 必为0 
				input_b = m_bcu_mod->m_rcu_mod[last_order_id]->m_benes_mod[benes_id]->m_benes_out[input_b_index];
				m_last_in_queue_b.push_back(input_b);
				break;
            case InputPort_LastSbox:
				input_b = m_bcu_mod->m_rcu_mod[last_order_id]->m_sbox_mod[sbox_id]->m_sbox_out[sbox_out_port_id];
				m_last_in_queue_b.push_back(input_b);
				break;
            case InputPort_InFifo:
                //从input fifo中获取值，移步 bfu 里做还是rcu里做，需要考虑一下
				//if (m_bcu_mod->m_ififo->size() > 0)
				//{
				//	input_b = m_bcu_mod->m_ififo->front().input_data;
				//	m_last_in_queue_b.push_back(input_b);

				//	m_bcu_mod->m_is_used_infifo = true;
				//	if (print_num >= 3)
				//	{
				//		m_bcu_mod->m_ififo->pop_front();
				//	}

				//}
				input_b = m_bcu_mod->m_infifo_pre_read.at(input_b_index);
				m_last_in_queue_b.push_back(input_b);
				m_bcu_mod->m_is_used_infifo = true;

                break;
            case InputPort_Mem:
                //ElementType tmp_element = ELEMENT_OUTMEM;
                //int mem_offset = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, tmp_element, 0, AttributeID::RCUMEMRDOFFSET, 0);
                input_b =  m_bcu_mod->m_mem_mod->m_mem_ram[m_bcu_mod->m_mem_mod->m_mem_read_addr[input_b_index]];
				m_last_in_queue_b.push_back(input_b);
				break;
			case InputPort_Imd:
				input_b = m_bcu_mod->m_imd[input_b_index];
				m_last_in_queue_b.push_back(input_b);
				m_bcu_mod->m_is_used_imd = true;
				break;
			case InputPort_Rch:
				input_b = m_bcu_mod->m_sreg_mod->m_sreg_out_data[input_b_index];
				m_last_in_queue_b.push_back(input_b);
				break;
            case InputPort_Lfsr:
                if (m_bcu_mod->m_rcv_lfsr_rst_que.size() > 0)
                {
                    input_b = m_bcu_mod->m_rcv_lfsr_rst_que.at(input_b_index);
                    m_last_in_queue_b.push_back(input_b);
                    m_bcu_mod->m_is_used_lfsr = true;
                }
                break;
            }

            int input_t = -1;

			sbox_id = input_t_index / SBOX_OUT_PORT_NUM;
			sbox_out_port_id = input_t_index % SBOX_OUT_PORT_NUM;
			//benes_id 必为0 
			benes_id = input_t_index / BENES_OUT_PORT_NUM;

            switch (input_t_type)
            {
            case InputPort_NULL:
                input_t = 0;
				m_last_in_queue_t.push_back(input_t);
                break;
            case InputPort_CurrentBfuX:
					input_t = m_bcu_mod->m_rcu_mod[rcu_order_id]->m_bfu_mod[input_t_index]->m_bfu_out[0];
					m_last_in_queue_t.push_back(input_t);
					break;
            case InputPort_CurrentBfuY:
                input_t = m_bcu_mod->m_rcu_mod[rcu_order_id]->m_bfu_mod[input_t_index]->m_bfu_out[1];
				m_last_in_queue_t.push_back(input_t);
				break;
            case InputPort_CurrentSbox:
				input_t = m_bcu_mod->m_rcu_mod[rcu_order_id]->m_sbox_mod[sbox_id]->m_sbox_out[sbox_out_port_id];
				m_last_in_queue_t.push_back(input_t);
				break;
            case InputPort_CurrentBenes:
				input_t = m_bcu_mod->m_rcu_mod[rcu_order_id]->m_benes_mod[benes_id]->m_benes_out[input_t_index];
				m_last_in_queue_t.push_back(input_t);
				break;
            case InputPort_LastBfuX:
                input_t = m_bcu_mod->m_rcu_mod[last_order_id]->m_bfu_mod[input_t_index]->m_bfu_out[0];
				m_last_in_queue_t.push_back(input_t);
				break;
            case InputPort_LastBfuY:
                input_t = m_bcu_mod->m_rcu_mod[last_order_id]->m_bfu_mod[input_t_index]->m_bfu_out[1];
				m_last_in_queue_t.push_back(input_t);
				break;
            case InputPort_LastBenes:
				//benes_id 必为0 
				input_t = m_bcu_mod->m_rcu_mod[last_order_id]->m_benes_mod[benes_id]->m_benes_out[input_t_index];
				m_last_in_queue_t.push_back(input_t);
				break;
            case InputPort_LastSbox:
				input_t = m_bcu_mod->m_rcu_mod[last_order_id]->m_sbox_mod[sbox_id]->m_sbox_out[sbox_out_port_id];
				m_last_in_queue_t.push_back(input_t);
				break;
            case InputPort_InFifo:
                //从input fifo中获取值，移步 bfu 里做还是rcu里做，需要考虑一下
				//if (m_bcu_mod->m_ififo->size() > 0)
				//{
				//	input_t = m_bcu_mod->m_ififo->front().input_data;
				//	m_last_in_queue_t.push_back(input_t);
				//	m_bcu_mod->m_is_used_infifo = true;
				//	if (print_num >= 3)
				//	{
				//		m_bcu_mod->m_ififo->pop_front();
				//	}

				//}
				input_t = m_bcu_mod->m_infifo_pre_read.at(input_t_index);
				m_last_in_queue_t.push_back(input_t);
				m_bcu_mod->m_is_used_infifo = true;

                break;
            case InputPort_Mem:
                //ElementType tmp_element = ELEMENT_OUTMEM;
                //int mem_offset = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, tmp_element, 0, AttributeID::RCUMEMRDOFFSET, 0);
                input_t = m_bcu_mod->m_mem_mod->m_mem_ram[m_bcu_mod->m_mem_mod->m_mem_read_addr[input_t_index]];
				m_last_in_queue_t.push_back(input_t);
				break;
			case InputPort_Imd:
				input_t = m_bcu_mod->m_imd[input_t_index];
				m_last_in_queue_t.push_back(input_t);
				m_bcu_mod->m_is_used_imd = true;
				break;
			case InputPort_Rch:
				input_t = m_bcu_mod->m_sreg_mod->m_sreg_out_data[input_t_index];
				m_last_in_queue_t.push_back(input_t);
				break;
            case InputPort_Lfsr:
                if (m_bcu_mod->m_rcv_lfsr_rst_que.size() > 0)
                {
                    input_t = m_bcu_mod->m_rcv_lfsr_rst_que.at(input_t_index);
                    m_last_in_queue_t.push_back(input_t);
                    m_bcu_mod->m_is_used_lfsr = true;
                }
                break;
            }


            int func_id = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::BfuFunc, 0);
            string tmp_func = m_rca_xml_slv->funcMap[func_id];
            const char *func = tmp_func.c_str();

            const char *exp = m_rca_xml_slv->getElementAttributeStr(bcu_id, rcu_id, element_id, AttributeID::BfuFuncExp);
            const char *bypass_exp = m_rca_xml_slv->getElementAttributeStr(bcu_id, rcu_id, element_id, AttributeID::BfuBypassExp);

            int bypass_t = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::BfuBypass, 0);
            int mode_t = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::BfuAUMod, 0);

            char * func_t = new char[strlen(func) + 1];
            strcpy(func_t, func);

            char * exp_t = new char[strlen(exp) + 1];
            strcpy(exp_t, exp);

            char * bypass_exp_t = new char[strlen(bypass_exp) + 1];
            strcpy(bypass_exp_t, bypass_exp);

			//如果三个输入没有同时valid,则寄存上一周期的输入
			//int *result = bfu(input_a, input_b, input_t, func_t, exp_t, bypass_t, bypass_exp_t, mode_t);
            int *result = nullptr;
            //result = new int[2];
			if((m_last_in_queue_a.size()>0) && (m_last_in_queue_b.size() > 0) && (m_last_in_queue_t.size() > 0))
			{
                input_a_in = m_last_in_queue_a.front();
				input_b_in = m_last_in_queue_b.front();
				input_t_in = m_last_in_queue_t.front();

                result = bfu(input_a_in, input_b_in, input_t_in, func_t, exp_t, bypass_t, bypass_exp_t, mode_t,print_num);
				m_bfu_out_valid = true;

				m_last_in_queue_a.pop_front();
				m_last_in_queue_b.pop_front();
			    m_last_in_queue_t.pop_front();
			
				for (int i = 0; i < BFU_OUT_PORT_NUM; ++i)
				{
					//保存之前输出为last
					m_bfu_last_out[i] = m_bfu_out[i];
					//更新最新的输出结果
					m_bfu_out[i] = result[i];
					//m_bfu_debug_log << "bfu out " << i << " = "<<hex<<m_bfu_out[i]<< endl;
				}
                delete[] result;

			}
			else
			{
				m_bfu_out_valid = false;
			}  

            delete[] func_t;
            delete[] exp_t;
            delete[] bypass_exp_t;

            //func = NULL;
        }

    }


    int* bfu_mod::bfu(int a, int b, int t, const char * func, char *exp, int bypass, char *bypassExp, int mode, int print_num)
    {
        U32 *out = new U32[2];
        char * EXP = strcmp(exp, "") == 0 ? (char *)"A" : exp;
        char * BYPASSEXP = strcmp(bypassExp, "") == 0 ? (char *)" " : bypassExp;
        //	cout<<EXP<<","<<BYPASSEXP<<endl;
        if (strcmp(func, "lu") == 0){
            BfuLu lu0;
            lu0.setFuncExp(EXP);
            out[0] = lu0.calculateByPostExp(a, b, t);
            out[1] = bypass == 0 ? a : bypass == 1 ? b : t;
        }
        else if (strcmp(func, "au") == 0){
            BfuAu lu0;
            lu0.setMod(mode);
            lu0.setFuncExp(EXP);
            out[0] = lu0.calculateByPostExp(a, b, t);
            //		out[0] = lu0.calculateByAttr(a,b,t);
            if (mode == 3) out[0] = lu0.mod31_1(out[0]);
            out[1] = bypass == 0 ? a : bypass == 1 ? b : t;

        }
        else if (strcmp(func, "su") == 0){
            BfuSu lu0;
            lu0.setFuncAndBypassExp(EXP, BYPASSEXP);
            out[1] = bypass == 0 ? a : bypass == 1 ? b : bypass == 2 ? t : lu0.calculateBypass(b, t);
            out[0] = lu0.calculate(a, b, t);
        }
        else{
            cout << "BFU type not supported. " << "lu" << endl;
        }
		if (print_num >=3)
		{
			cout << "[OPERATOR RCU " << m_rcu_id << " BFU " << m_bfu_id << "] Mode is " << func << ",";
			cout << "Exp is " << exp;
			cout << ",Bypass is " << bypass << ",";
			cout << "Bypass EXP is " << bypassExp << ",";
			cout << "Mode is " << mode << ',';
			cout << hex << " ,a:" << a << ",b:" << b << ",t:" << t << ",";
			cout << "x:" << out[0] << ",y:" << out[1] << dec << endl;


			m_bcu_mod->m_cout_out_log << "[OPERATOR RCU " << m_rcu_id << " BFU " << m_bfu_id << "] Mode is " << func << ",";
			m_bcu_mod->m_cout_out_log << "Exp is " << exp;
			m_bcu_mod->m_cout_out_log << ",Bypass is " << bypass << ",";
			m_bcu_mod->m_cout_out_log << "Bypass EXP is " << bypassExp << ",";
			m_bcu_mod->m_cout_out_log << "Mode is " << mode << ',';
			m_bcu_mod->m_cout_out_log << hex << " ,a:" << a << ",b:" << b << ",t:" << t << ",";
			m_bcu_mod->m_cout_out_log << "x:" << out[0] << ",y:" << out[1] << dec << endl;
			

		}        
        return (int*)out;
    }



    void bfu_mod::get_xml_resolver(XmlResolver *xml_resolver)
    {
        m_rca_xml_slv = xml_resolver;
    }
}
