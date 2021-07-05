#ifndef __STRUCT_DEF_H__
#define __STRUCT_DEF_H__

#include "systemc.h"
#include "sc_vector.h"
#include "deque"
#include <iostream>
//#include <iosfwd>
#include <sstream>
#include <stdio.h>
//#include <tchar.h>
#include "../xml/tinyxml2.h"
#include "../xml/xmltype.h"
#include "../xml/xmlresolver.h"
#include "export_typedef.h"
//#include "./bcu/bcu.h"
// lib
#include "./lib/bfuau.h"
#include "./lib/bfulu.h"
#include "./lib/bfusu.h"
#include "./lib/benes.h"
#include "./lib/type.h"


#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
//#include <crtdbg.h>
//#include "stdafx.h"
//#include "./bcu/bcu.h"
//#include "./rcu/rcu.h"
//#include "./bcu/bcu.h"

using namespace std;
using namespace tinyxml2;

namespace rpu_esl_model
{
    const int PKE_RSA_PARSER_NUM = 1;
    const int PKE_SM2_PARSER_NUM = 1;
    const int BUK_PARSER_NUM = 10;
    const int CMD_FIFO_DEPTH = 64;
    const int FIFO_DEPTH = 16;
    const int PARSER_PRE_LATENCY = 10;
    const int PARSER_PRO_LATENCY = 1000;
    const int RING_MST_LATENCY = 10;
    const int RPU_DATA_LATENCY = 10;
    const int RING_MST_FIFO_DEPTH = 16;
    const int CMD_MST_FIFO_DEPTH = 16;
    const int DATA_MST_FIFO_DEPTH = 16;
    const int CFG_RAM_NUM = 4;
    const int CFG_RAM_TOTAL_ADDR = 256;
    const int PE_NUM = 4;
    const int BCU_NUM = 4;
    const int OUTBUFFER_SIZE = 256;
    const int HALF_OUTBUFFER_SIZE = 128;
    const int TRAM_TOTAL_ADDR = 256;
    const int SBOX_OUT_PORT_NUM = 4;
    const int BENES_OUT_PORT_NUM = 4;
    const int BFU_OUT_PORT_NUM = 2;
	const int SREG_IN_PORT_NUM = 4;
	const int SREG_OUT_PORT_NUM = 16;

	const int MEM_IN_PORT_NUM = 1;
	const int MEM_OUT_PORT_NUM = 4;

	const int IMD_OUT_PORT_NUM = 4;

    const int BCU_TOTAL_NUM = 16;

    enum input_port_type
    {
		InputPort_NULL = 0,
		InputPort_CurrentBfuX,
		InputPort_CurrentBfuY,
		InputPort_CurrentSbox,
		InputPort_CurrentBenes,
		InputPort_LastBfuX,
		InputPort_LastBfuY,
		InputPort_LastSbox,
		InputPort_LastBenes,
		InputPort_InFifo,
		InputPort_Mem,
		InputPort_Imd,
		InputPort_Rch,
        InputPort_Lfsr
    };

    enum benes_onput_port_no
    {
        PORT_0 = 0,
        PORT_1,
        PORT_2,
        PORT_3,
        BENES_PORT_NUM
    };

	enum rch_mode
	{
		CACHE_MODE = 0,
		SREG_MODE_1X16,
		SREG_MODE_4X4,
		RCH_MODE_NUM
	};

    enum lfsr_mode
    {
        LFSR_BIT_0 = 0,
        LFSR_BIT_1,
        LFSR_BIT_8,
        LFSR_BIT_16,
        LFSR_BIT_32,
        LFSR_MODE_NUM
    };

    typedef class bfu_cfg
    {
    public:
        int input_a;
        int input_b;
        int input_t;
        int output_x;
        int output_y;
        int func_mod;
        bool bypass;
        int  clc_mod;

        bfu_cfg()
        {
            input_a = -1;
            input_b = -1;
            input_t = -1;
            output_x = -1;
            output_y = -1;
            func_mod = 0;
            bypass = false;
            clc_mod = 0;
        }

        friend ostream& operator<<(ostream& os, const bfu_cfg& t)
        {
            return  os;
        }
    }bfu_cfg_t;


    typedef class bcu_cfg_data
    {
    public:
        int packet_no;
        int input_data;
        int latency;
        int ram_addr;
        vector<uint32_t> real_data;

        //uint new_count = 0;
        //uint delete_count = 0;

        bcu_cfg_data()
        {
            //new_count++;
            //cout << "bcu_cfg_data() constructor: " << new_count << endl;

            packet_no = -1;
            input_data = 0x0;
            latency = 0;
            ram_addr = 0;
            real_data.resize(16, 0);
        }

        friend ostream& operator<<(ostream& os, const bcu_cfg_data& t)
        {
            return  os;
        }

        ~bcu_cfg_data()
        {
            //delete_count++;
            //cout << "~~bcu_cfg_data(): " << delete_count << endl;


        }
    } lfsr_send_data_t;

    class  ring2cmd_struct
    {
    public:
        int  addr;
        bool valid;

        ring2cmd_struct()
        {
            addr = -1;
            valid = false;
        }

        friend ostream& operator<<(ostream& os, const ring2cmd_struct& t)
        {
            return  os;
        }
    };

    enum bfu_calc_mode
    {
        LU = 0,
        SU,
        AU,
        MU
    };

    enum parser_cmd_type
    {
        CMD_TYPE = 0,
        DATA_TYPE,
        CMD_TYPE_MAX
    };

    enum parser_req_type
    {
        PKE_RSA_TYPE = 0,
        PKE_SM2_TYPE,
        BUK_TYPE,
        REQ_TYPE_MAX
    };

    enum fetch_cfg_jump_type
    {
        NOMARL = 0,
        BRANCH_JUMP,
        DIRECT_JUMP,
        ADVANCED_JUMP
    };

    enum operation
    {
        SBOX = 0,
        LOAD,
        STORE,
    };

    enum rpu_mode
    {
        SINGLE_MOD = 0,
        DOUBLE_MOD,
        FOURTH_MOD,
    };

    class  top_cfg
    {
    public:
        int  print_period;
        int  bcu_num;
        int  rcu_num;
        int  bfu_num;
        int  sbox_num;
        int  benes_num;
        int  mem_size;

        //static int new_count;
        //static int delete_count;
        top_cfg()
        {

            //new_count++;
            //cout << "ttop_cfg() constructor: " << new_count << endl;

            print_period = 50000;
            bcu_num = 1;
            rcu_num = 32;
            bfu_num = 8;
            sbox_num = 4;
            benes_num = 1;
            mem_size = 64;

        }
        top_cfg(const top_cfg& right)
        {
            //new_count++;
            //cout << "ttop_cfg() constructor copy: " << new_count << endl;

            this->print_period = right.print_period;
            this->bcu_num      = right.bcu_num     ;
            this->rcu_num      = right.rcu_num     ;
            this->bfu_num      = right.bfu_num     ;
            this->sbox_num     = right.sbox_num    ;
            this->benes_num    = right.benes_num   ;
            this->mem_size     = right.mem_size    ;
        }

        ~top_cfg()
        {
            //delete_count++;
            //cout << "~top_cfg(): " << delete_count << endl;
        }

        friend ostream& operator<<(ostream& os, const top_cfg& t)
        {
            return  os;
        }
    };

}
#endif
