
#include "systemc.h"
#include "generator.h"

namespace rpu_esl_model
{

    generator::generator(sc_module_name name, sc_time simu_time, int bcu_num) :sc_module(name), m_simu_time(simu_time)
    {
        m_pkt_id = 0;
        m_cycle_cnt = 0;
        SC_THREAD(run_process_mth);
        sensitive << chip_in_clk.pos();
        dont_initialize();

        //output_debug_log.open("debug.log");
        //output_stat_log.open("generater.log"); 

        m_p2p_max_laetency = 0;
        m_p2p_min_laetency = 65535;
        m_p2p_latecny_sum = 0;
        rcv_pkt_cnt = 0;
        m_on_step_mode = false;
        m_bcu_num = bcu_num;
        sco_gen2chip_pkt.resize(bcu_num);
        m_source_date.resize(bcu_num);
        for (int i = 0 ; i < bcu_num ;++i)
        {
            sco_gen2chip_pkt[i] = new sc_fifo_out<rpu_data_struct>();
        }
    }

    generator::~generator()
    {
        while (!sco_gen2chip_pkt.empty()) {
            delete sco_gen2chip_pkt.back(),sco_gen2chip_pkt.pop_back();
        }
    }

    void generator::run_process_mth()
    {
        t = sc_get_current_process_handle();
        while (true)
        {
            rcv_pkt();
            gen_pkt();
            print_simu_info();
            m_cycle_cnt++;


            if (m_rca_mod->m_bcu_end_status)
            {
                //sc_stop();
                //t.resume();
                //break;
            }

            if (m_on_step_mode)
            {
                //sc_pause();
                //wait(m_one_step_event);
                t.suspend();
                wait();
            }
            else
            {
                wait();
            }

        }
       

    }

    void generator::notify_debug_by_step_test()
    {
        t.resume();
        //m_one_step_event.notify();
    }

	ESIM_RET generator::get_source_xml(vector<string >source_name)
	{
        for (int i = 0 ; i < m_bcu_num;i++)
        {
            string line;
            int data_num = 0;
            ifstream fin(source_name[i]);
            if (!fin.is_open()) {
                cout << "FILE open failed" << " file name :" << source_name[i] << endl;
                g_retMsg = string(u8"File: ") + source_name[i] + u8" open failed";
                return RET_CRITICAL;
            }

            char str[1024];
            //UINT32 idate[4];
            in_date.resize(4, 0);
            int addr = 0;
            FILE *fp;
            fp = fopen((source_name[i]).c_str(), "r");
            bool valid_line = true;
            while (fgets(str, 1023, fp) != NULL)
            {
                valid_line = true;
                for (uint32_t i = 0; i < strlen(str); ++i)
                {
                    //if ("#" == str[i])
                    if (35 == str[i])
                    {
                        valid_line = false;
                        break;
                    }
                }
                if (valid_line)
                {
                    sscanf(str, "%x %x %x %x %x\n", &addr, &in_date[0], &in_date[1], &in_date[2], &in_date[3]);
                    if (in_date[0] >= 0)
                    {
                        m_source_date[i].push_back(in_date[0]);
                        m_source_date[i].push_back(in_date[1]);
                        m_source_date[i].push_back(in_date[2]);
                        m_source_date[i].push_back(in_date[3]);
                    }
                }

            }

            fclose(fp);
        }
		

		return RET_SUCCESS;
	}

    void generator::set_debug_by_step_mode(bool mode)
    {
        m_on_step_mode = mode;
    }


    void generator::set_rca_ptr(rca_mod *rca_mod_ptr)
    {
        m_rca_mod = rca_mod_ptr;
    }


    void generator::gen_pkt()
    {
        for (int i = 0; i<m_bcu_num;++i)
        {
            rpu_data_struct gen_pkt_info;
            if (m_source_date[i].size() > 0 && sco_gen2chip_pkt[i]->num_free() > 0)
            {
                gen_pkt_info.input_data = m_source_date[i].front();
                sco_gen2chip_pkt[i]->nb_write(gen_pkt_info);
                m_source_date[i].pop_front();
            }
        }

    }

    void generator::rcv_pkt()
    {
        while (sci_chip2gen_pkt->num_available() > 0)
        {
            rpu_struct tmp_data = sci_chip2gen_pkt->read();
            //统计延时等信息
            int rcv_cycle = sc_time_stamp().to_seconds()*1e9;
            int pkt_latency = rcv_cycle - tmp_data.time_stamp;
            m_p2p_latecny_sum += (double)pkt_latency;
            rcv_pkt_cnt++;
            //output_debug_log << "cycle = " << m_cycle_cnt <<"  ,gen rcv pkt info :pkt_id = " << tmp_data.req_no << " ,cmd_type = " << tmp_data.cmd_type << ",  latency = " << pkt_latency << endl;
            if (pkt_latency > m_p2p_max_laetency)
            {
                m_p2p_max_laetency = pkt_latency;
            }
            if (pkt_latency < m_p2p_min_laetency)
            {
                m_p2p_min_laetency = pkt_latency;
            }

            //回收ring_ctrl地址
            bool is_sucess = sco_recycle_ring_addr->nb_write(tmp_data);
        }
    }

    void generator::print_simu_info()
    {
        if (0 == m_cycle_cnt % 1000)
        {
            printf("Simu Percent : %0.2f%%           speed :  %d cycle/s \r", sc_time_stamp().to_double() / m_simu_time.to_double() * 100, 1000);
        }

        if (m_cycle_cnt >= 5000)
        {
            if (0 == (m_cycle_cnt) % 5000)
            {
                //output_stat_log << "**********************************************************************************************************************" << endl;
                //output_stat_log << "curr_time : " << m_cycle_cnt * 2 / 1e3 << "US" << endl;
                //output_stat_log << "generator stat info :     (t_max|t_aver|t_min)      " << endl;
                //output_stat_log << "p2p latecny(ns) : (" << m_p2p_max_laetency << "|" << (m_p2p_latecny_sum / rcv_pkt_cnt) << "|" << m_p2p_min_laetency << ")" << "       ，rcv_pkt_cnt =" << rcv_pkt_cnt << endl;
                //output_stat_log << endl;
                ////phase 初始化  total不变

            }
        }

    }
}
