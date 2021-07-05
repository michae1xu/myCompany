
#include "benes_mod.h"

namespace rpu_esl_model
{
    //************************************
    // Method:    cmd_ctrl
    // FullName:  cmd_ctrl::cmd_ctrl
    // Access:    public 
    // Returns:   
    // Qualifier: :sc_module(name), m_top_cfg(total_top_cfg)
    // Parameter: sc_module_name name
    // Parameter: top_cfg total_top_cfg
    //************************************
    benes_mod::benes_mod(sc_module_name name, top_cfg total_top_cfg, int rcu_id, int benes_id, bcu_mod *mod_t) :sc_module(name), m_top_cfg(total_top_cfg)
    {
        //SC_METHOD(data_process_mth);
        //sensitive << sci_chip_clk.pos();
        //dont_initialize();

        m_bcu_mod = mod_t;
		m_rcu_id = rcu_id;
		m_benes_id = benes_id;
        mod_init(rcu_id, benes_id);
    }

    //************************************
    // Method:    mod_init
    // FullName:  cmd_ctrl::mod_init
    // Access:    public 


    // Returns:   void
    // Qualifier:
    //************************************
    void benes_mod::mod_init(int rcu_id, int benes_id)
    {
        convert.resize(832);

        convert = {
            0, 64, 96, 112, 120, 124, 126, 128, 136, 160, 224, 384, 768,
            1, 65, 97, 113, 121, 125, 127, 129, 137, 161, 225, 385, 769,
            2, 66, 98, 114, 122, 130, 132, 134, 138, 162, 226, 386, 770,
            3, 67, 99, 115, 123, 131, 133, 135, 139, 163, 227, 387, 771,
            4, 68, 100, 116, 140, 144, 146, 148, 156, 164, 228, 388, 772,
            5, 69, 101, 117, 141, 145, 147, 149, 157, 165, 229, 389, 773,
            6, 70, 102, 118, 142, 150, 152, 154, 158, 166, 230, 390, 774,
            7, 71, 103, 119, 143, 151, 153, 155, 159, 167, 231, 391, 775,
            8, 72, 104, 168, 176, 180, 182, 184, 192, 216, 232, 392, 776,
            9, 73, 105, 169, 177, 181, 183, 185, 193, 217, 233, 393, 777,
            10, 74, 106, 170, 178, 186, 188, 190, 194, 218, 234, 394, 778,
            11, 75, 107, 171, 179, 187, 189, 191, 195, 219, 235, 395, 779,
            12, 76, 108, 172, 196, 200, 202, 204, 212, 220, 236, 396, 780,
            13, 77, 109, 173, 197, 201, 203, 205, 213, 221, 237, 397, 781,
            14, 78, 110, 174, 198, 206, 208, 210, 214, 222, 238, 398, 782,
            15, 79, 111, 175, 199, 207, 209, 211, 215, 223, 239, 399, 783,
            16, 80, 240, 256, 264, 268, 270, 272, 280, 304, 368, 400, 784,
            17, 81, 241, 257, 265, 269, 271, 273, 281, 305, 369, 401, 785,
            18, 82, 242, 258, 266, 274, 276, 278, 282, 306, 370, 402, 786,
            19, 83, 243, 259, 267, 275, 277, 279, 283, 307, 371, 403, 787,
            20, 84, 244, 260, 284, 288, 290, 292, 300, 308, 372, 404, 788,
            21, 85, 245, 261, 285, 289, 291, 293, 301, 309, 373, 405, 789,
            22, 86, 246, 262, 286, 294, 296, 298, 302, 310, 374, 406, 790,
            23, 87, 247, 263, 287, 295, 297, 299, 303, 311, 375, 407, 791,
            24, 88, 248, 312, 320, 324, 326, 328, 336, 360, 376, 408, 792,
            25, 89, 249, 313, 321, 325, 327, 329, 337, 361, 377, 409, 793,
            26, 90, 250, 314, 322, 330, 332, 334, 338, 362, 378, 410, 794,
            27, 91, 251, 315, 323, 331, 333, 335, 339, 363, 379, 411, 795,
            28, 92, 252, 316, 340, 344, 346, 348, 356, 364, 380, 412, 796,
            29, 93, 253, 317, 341, 345, 347, 349, 357, 365, 381, 413, 797,
            30, 94, 254, 318, 342, 350, 352, 354, 358, 366, 382, 414, 798,
            31, 95, 255, 319, 343, 351, 353, 355, 359, 367, 383, 415, 799,
            32, 416, 448, 464, 472, 476, 478, 480, 488, 512, 576, 736, 800,
            33, 417, 449, 465, 473, 477, 479, 481, 489, 513, 577, 737, 801,
            34, 418, 450, 466, 474, 482, 484, 486, 490, 514, 578, 738, 802,
            35, 419, 451, 467, 475, 483, 485, 487, 491, 515, 579, 739, 803,
            36, 420, 452, 468, 492, 496, 498, 500, 508, 516, 580, 740, 804,
            37, 421, 453, 469, 493, 497, 499, 501, 509, 517, 581, 741, 805,
            38, 422, 454, 470, 494, 502, 504, 506, 510, 518, 582, 742, 806,
            39, 423, 455, 471, 495, 503, 505, 507, 511, 519, 583, 743, 807,
            40, 424, 456, 520, 528, 532, 534, 536, 544, 568, 584, 744, 808,
            41, 425, 457, 521, 529, 533, 535, 537, 545, 569, 585, 745, 809,
            42, 426, 458, 522, 530, 538, 540, 542, 546, 570, 586, 746, 810,
            43, 427, 459, 523, 531, 539, 541, 543, 547, 571, 587, 747, 811,
            44, 428, 460, 524, 548, 552, 554, 556, 564, 572, 588, 748, 812,
            45, 429, 461, 525, 549, 553, 555, 557, 565, 573, 589, 749, 813,
            46, 430, 462, 526, 550, 558, 560, 562, 566, 574, 590, 750, 814,
            47, 431, 463, 527, 551, 559, 561, 563, 567, 575, 591, 751, 815,
            48, 432, 592, 608, 616, 620, 622, 624, 632, 656, 720, 752, 816,
            49, 433, 593, 609, 617, 621, 623, 625, 633, 657, 721, 753, 817,
            50, 434, 594, 610, 618, 626, 628, 630, 634, 658, 722, 754, 818,
            51, 435, 595, 611, 619, 627, 629, 631, 635, 659, 723, 755, 819,
            52, 436, 596, 612, 636, 640, 642, 644, 652, 660, 724, 756, 820,
            53, 437, 597, 613, 637, 641, 643, 645, 653, 661, 725, 757, 821,
            54, 438, 598, 614, 638, 646, 648, 650, 654, 662, 726, 758, 822,
            55, 439, 599, 615, 639, 647, 649, 651, 655, 663, 727, 759, 823,
            56, 440, 600, 664, 672, 676, 678, 680, 688, 712, 728, 760, 824,
            57, 441, 601, 665, 673, 677, 679, 681, 689, 713, 729, 761, 825,
            58, 442, 602, 666, 674, 682, 684, 686, 690, 714, 730, 762, 826,
            59, 443, 603, 667, 675, 683, 685, 687, 691, 715, 731, 763, 827,
            60, 444, 604, 668, 692, 696, 698, 700, 708, 716, 732, 764, 828,
            61, 445, 605, 669, 693, 697, 699, 701, 709, 717, 733, 765, 829,
            62, 446, 606, 670, 694, 702, 704, 706, 710, 718, 734, 766, 830,
            63, 447, 607, 671, 695, 703, 705, 707, 711, 719, 735, 767, 831
        };

        m_benes_out.resize(BENES_OUT_PORT_NUM, -1);
        m_benes_last_out.resize(BENES_OUT_PORT_NUM, -1);
        m_input_benes.resize(BENES_OUT_PORT_NUM, 0);

		m_last_in_que.resize(BENES_OUT_PORT_NUM); 
	    //m_last_in_que.clear();
        m_ififo = m_bcu_mod->m_ififo;
        m_ofifo = m_bcu_mod->m_ofifo;

		string rca_name = "rca";
		string rcu_name = "rcu";
		string bfu_name = "_benes";
		string a = "_debug.log";

		ostringstream oss;
		oss << rcu_name << rcu_id << bfu_name << benes_id << a;
		//m_benes_debug_log.open(oss.str());

    }

    //************************************
    // Method:    data_process_mth
    // FullName:  cmd_ctrl::data_process_mth
    // Access:    public 
    // Returns:   void
    // Qualifier:
    //************************************
    void benes_mod::data_process_mth()
    {
        //if (sci_tb2chip_cfg_data->num_available() > 0)
        //{
        //    bcu_cfg_data tmp_data = sci_tb2chip_cfg_data->read();
        //}
    }

    //************************************
    // Method:    rr_process
    // FullName:  cmd_ctrl::rr_process
    // Access:    public 
    // Returns:   void
    // Qualifier:
    //************************************
    void benes_mod::rr_process()
    {

    }


    //************************************
    // Method:    stat_pro
    // FullName:  cmd_ctrl::stat_pro
    // Access:    public 
    // Returns:   void
    // Qualifier:
    //************************************
    void benes_mod::stat_pro()
    {

    }
    //************************************
    // Method:    print_info
    // FullName:  cmd_ctrl::print_info
    // Access:    public 
    // Returns:   void
    // Qualifier:
    //************************************
    void benes_mod::print_info()
    {

    }

    //************************************
    // Method:    get_xml_cfg
    // FullName:  cmd_ctrl::get_xml_cfg
    // Access:    public 
    // Returns:   void
    // Qualifier:
    // Parameter: XMLElement * zorov3_root
    //************************************
    void benes_mod::get_xml_cfg(XMLElement *ele)
    {

    }


    benes_mod::~benes_mod()
    {
    }

    void benes_mod::run_process(int bcu_id, int rcu_id, int benes_id, XMLElement * benes_ele,int print_num)
    {
        bool is_infifo_used = false;

        ElementType element = ElementType::ELEMENT_BENES;
        int element_id = benes_id;
        vector<int > benes_input_type;
        vector<int > benes_input_index;
        benes_input_type.resize(BENES_OUT_PORT_NUM, -1);
        benes_input_index.resize(BENES_OUT_PORT_NUM, -1);

        benes_input_type[0] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::BenesInput0_Type, 0);
        benes_input_index[0] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::BenesInput0_Index, 0);
        benes_input_type[1] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::BenesInput1_Type, 0);
        benes_input_index[1] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::BenesInput1_Index, 0);
        benes_input_type[2] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::BenesInput2_Type, 0);
        benes_input_index[2] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::BenesInput2_Index, 0);
        benes_input_type[3] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::BenesInput3_Type, 0);
        benes_input_index[3] = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, element_id, AttributeID::BenesInput3_Index, 0);

        const char * input_src = m_rca_xml_slv->getElementAttributeStr(bcu_id, rcu_id, element_id, AttributeID::BenesSrc);;
        vector<int> input_a;
        input_a.resize(BENES_OUT_PORT_NUM, -1);

		//如何确定last_rcu_id ?index 和实际bfu编号的转换,map
		int rcu_order_id = m_bcu_mod->m_rcuid2orderid_map[rcu_id];
		int last_order_id = m_bcu_mod->m_rcuid2orderid_map[rcu_id] -1 ;
		int last_rcu_id = 0;
		if (last_order_id > 0)
		{
			last_rcu_id = m_bcu_mod->m_orderid2rcuid_map[last_order_id];
		}
		int sbox_id_a = -1;
		int sbox_out_port_id_a = -1;
		int benes_id_a = -1;

        for (int i = 0; i < BENES_OUT_PORT_NUM; ++i)
        {
			sbox_id_a = benes_input_index[i] / SBOX_OUT_PORT_NUM;
			sbox_out_port_id_a = benes_input_index[i] % SBOX_OUT_PORT_NUM;
			//benes_id 必为0 
			benes_id_a = benes_input_index[i] / BENES_OUT_PORT_NUM;

            switch (benes_input_type[i])
            {
            case InputPort_NULL:
                input_a[i] = 0;
				m_last_in_que[i].push_back(input_a[i]);
                break;
            case InputPort_CurrentBfuX:
				//if (m_bcu_mod->m_rcu_mod[rcu_id]->m_bfu_mod[benes_input_index[i]]->m_bfu_out_valid)
				//{
					input_a[i] = m_bcu_mod->m_rcu_mod[rcu_order_id]->m_bfu_mod[benes_input_index[i]]->m_bfu_out[0];
					m_last_in_que[i].push_back(input_a[i]);
					break;
				//}
  
            case InputPort_CurrentBfuY:
                input_a[i] = m_bcu_mod->m_rcu_mod[rcu_order_id]->m_bfu_mod[benes_input_index[i]]->m_bfu_out[1];
				m_last_in_que[i].push_back(input_a[i]);
                break;
            case InputPort_CurrentSbox:
                input_a[i] = m_bcu_mod->m_rcu_mod[rcu_order_id]->m_sbox_mod[sbox_id_a]->m_sbox_out[sbox_out_port_id_a];
				m_last_in_que[i].push_back(input_a[i]);
				break;
            case InputPort_CurrentBenes:
                input_a[i] = m_bcu_mod->m_rcu_mod[rcu_order_id]->m_benes_mod[benes_id_a]->m_benes_out[benes_input_index[i]];
				m_last_in_que[i].push_back(input_a[i]);
				break;
            case InputPort_LastBfuX:
                input_a[i] = m_bcu_mod->m_rcu_mod[last_order_id]->m_bfu_mod[benes_input_index[i]]->m_bfu_out[0];
				m_last_in_que[i].push_back(input_a[i]);
				break;
            case InputPort_LastBfuY:
                input_a[i] = m_bcu_mod->m_rcu_mod[last_order_id]->m_bfu_mod[benes_input_index[i]]->m_bfu_out[1];
				m_last_in_que[i].push_back(input_a[i]);
				break;
            case InputPort_LastBenes:
                input_a[i] = m_bcu_mod->m_rcu_mod[last_order_id]->m_benes_mod[benes_id_a]->m_benes_out[benes_input_index[i]];
				m_last_in_que[i].push_back(input_a[i]);
				break;
            case InputPort_LastSbox:
				input_a[i] = m_bcu_mod->m_rcu_mod[last_order_id]->m_sbox_mod[sbox_id_a]->m_sbox_out[sbox_out_port_id_a];
				m_last_in_que[i].push_back(input_a[i]);
				break;
            case InputPort_InFifo:
                //从input fifo中获取值，移步bfu里做还是rcu里做，需要考虑一下
				//if (m_ififo->size() > 0)
				//{
				//	input_a[i] = m_ififo->front().input_data;
				//	m_last_in_que[i].push_back(input_a[i]);
				//	m_bcu_mod->m_is_used_infifo = true;
				//	if (print_num >= 3)
				//	{
				//		m_bcu_mod->m_ififo->pop_front();
				//	}
				//}
				input_a[i] = m_bcu_mod->m_infifo_pre_read.at(benes_input_index[i]);
				m_last_in_que[i].push_back(input_a[i]);
				m_bcu_mod->m_is_used_infifo = true;

                break;
            case InputPort_Mem:
                //element = ELEMENT_OUTMEM;
                //int mem_offset = m_rca_xml_slv->getElementAttributeValue(bcu_id, rcu_id, element, 0, AttributeID::RCUMEMRDOFFSET, 0);
                input_a[i] = m_bcu_mod->m_mem_mod->m_mem_ram[m_bcu_mod->m_mem_mod->m_mem_read_addr[benes_input_index[i]]];
				m_last_in_que[i].push_back(input_a[i]);
                break;
			case InputPort_Imd:
				input_a[i] = m_bcu_mod->m_imd[benes_input_index[i]];
				m_last_in_que[i].push_back(input_a[i]);
				m_bcu_mod->m_is_used_imd = true;
			case InputPort_Rch:
				input_a[i] = m_bcu_mod->m_sreg_mod->m_sreg_out_data[benes_input_index[i]];
				m_last_in_que[i].push_back(input_a[i]);
				break;
            case InputPort_Lfsr:
                if (m_bcu_mod->m_rcv_lfsr_rst_que.size() > 0)
                {
                    input_a[i] = m_bcu_mod->m_rcv_lfsr_rst_que.at(benes_input_index[i]);
                    m_last_in_que[i].push_back(input_a[i]);
                    m_bcu_mod->m_is_used_lfsr = true;
                }
                break;
            }

        }
//        char * input_src_char = new char[strlen(input_src) + 1];
//		  char * input_src_tmp = new char[strlen(input_src) - 1];
//        strcpy(input_src_char, input_src);

//        input_src_char = del_char(input_src_char, ':');
//        input_src_char = del_char(input_src_char, '/');
		string input_src_t;
		if (strcmp(input_src, "") != 0 && (strcmp(input_src,"unset")!=0))
		{
			input_src_t = m_bcu_mod[bcu_id].get_source_map()->at(string(input_src));
		}

		//if (m_last_in_que[0].size() > 0 )
		if (m_last_in_que[0].size()>0 && m_last_in_que[1].size()>0 && m_last_in_que[2].size() > 0 && m_last_in_que[3].size() > 0)
		{
            int *tmp_bene_out = benes(input_a[0], input_a[1], input_a[2], input_a[3], input_src_t.c_str(),print_num);
			m_benes_out_valid = true;

			for (int i = 0; i < BENES_OUT_PORT_NUM; ++i)
			{
                m_input_benes[i] = input_a[i];

				m_last_in_que[i].pop_front();
				//保存之前输出为last
				m_benes_last_out[i] = m_benes_out[i];
				//更新最新的输出结果
				m_benes_out[i] = tmp_bene_out[i];
				//m_benes_debug_log << "benes out " << i << " = " << hex << m_benes_out[i] << endl;
			}
            delete tmp_bene_out;
		}
		else
		{
			m_benes_out_valid = false;
		}
       
    }


    int* benes_mod::benes(int a, int b, int c, int d, const char *src, int print_num)
    {
        bool benes_cfg[832];
        int data[128];
        unsigned int aa, bb, cc, dd;
        aa = a; bb = b; cc = c; dd = d;
        ifstream fin(src);
        if (!fin.is_open()){
            cout << "FILE open failed" << endl;
            //exit(0);
        }
        string line;
        char temp[20];
        int i = 0;
        while (i < 26){
            getline(fin, line);
            if (line.find("#") == string::npos){
                line.erase(0, line.find(' ') + 3);
                char c = line[0];
                int j = 0;
                while (j < line.length()){
                    switch (line[7 - j]){
                    case '0': benes_cfg[i * 32 + j * 4 + 3] = 0; benes_cfg[i * 32 + j * 4 + 2] = 0; benes_cfg[i * 32 + j * 4 + 1] = 0; benes_cfg[i * 32 + j * 4 + 0] = 0; break;
                    case '1': benes_cfg[i * 32 + j * 4 + 3] = 0; benes_cfg[i * 32 + j * 4 + 2] = 0; benes_cfg[i * 32 + j * 4 + 1] = 0; benes_cfg[i * 32 + j * 4 + 0] = 1; break;
                    case '2': benes_cfg[i * 32 + j * 4 + 3] = 0; benes_cfg[i * 32 + j * 4 + 2] = 0; benes_cfg[i * 32 + j * 4 + 1] = 1; benes_cfg[i * 32 + j * 4 + 0] = 0; break;
                    case '3': benes_cfg[i * 32 + j * 4 + 3] = 0; benes_cfg[i * 32 + j * 4 + 2] = 0; benes_cfg[i * 32 + j * 4 + 1] = 1; benes_cfg[i * 32 + j * 4 + 0] = 1; break;
                    case '4': benes_cfg[i * 32 + j * 4 + 3] = 0; benes_cfg[i * 32 + j * 4 + 2] = 1; benes_cfg[i * 32 + j * 4 + 1] = 0; benes_cfg[i * 32 + j * 4 + 0] = 0; break;
                    case '5': benes_cfg[i * 32 + j * 4 + 3] = 0; benes_cfg[i * 32 + j * 4 + 2] = 1; benes_cfg[i * 32 + j * 4 + 1] = 0; benes_cfg[i * 32 + j * 4 + 0] = 1; break;
                    case '6': benes_cfg[i * 32 + j * 4 + 3] = 0; benes_cfg[i * 32 + j * 4 + 2] = 1; benes_cfg[i * 32 + j * 4 + 1] = 1; benes_cfg[i * 32 + j * 4 + 0] = 0; break;
                    case '7': benes_cfg[i * 32 + j * 4 + 3] = 0; benes_cfg[i * 32 + j * 4 + 2] = 1; benes_cfg[i * 32 + j * 4 + 1] = 1; benes_cfg[i * 32 + j * 4 + 0] = 1; break;
                    case '8': benes_cfg[i * 32 + j * 4 + 3] = 1; benes_cfg[i * 32 + j * 4 + 2] = 0; benes_cfg[i * 32 + j * 4 + 1] = 0; benes_cfg[i * 32 + j * 4 + 0] = 0; break;
                    case '9': benes_cfg[i * 32 + j * 4 + 3] = 1; benes_cfg[i * 32 + j * 4 + 2] = 0; benes_cfg[i * 32 + j * 4 + 1] = 0; benes_cfg[i * 32 + j * 4 + 0] = 1; break;
                    case 'A': benes_cfg[i * 32 + j * 4 + 3] = 1; benes_cfg[i * 32 + j * 4 + 2] = 0; benes_cfg[i * 32 + j * 4 + 1] = 1; benes_cfg[i * 32 + j * 4 + 0] = 0; break;
                    case 'B': benes_cfg[i * 32 + j * 4 + 3] = 1; benes_cfg[i * 32 + j * 4 + 2] = 0; benes_cfg[i * 32 + j * 4 + 1] = 1; benes_cfg[i * 32 + j * 4 + 0] = 1; break;
                    case 'C': benes_cfg[i * 32 + j * 4 + 3] = 1; benes_cfg[i * 32 + j * 4 + 2] = 1; benes_cfg[i * 32 + j * 4 + 1] = 0; benes_cfg[i * 32 + j * 4 + 0] = 0; break;
                    case 'D': benes_cfg[i * 32 + j * 4 + 3] = 1; benes_cfg[i * 32 + j * 4 + 2] = 1; benes_cfg[i * 32 + j * 4 + 1] = 0; benes_cfg[i * 32 + j * 4 + 0] = 1; break;
                    case 'E': benes_cfg[i * 32 + j * 4 + 3] = 1; benes_cfg[i * 32 + j * 4 + 2] = 1; benes_cfg[i * 32 + j * 4 + 1] = 1; benes_cfg[i * 32 + j * 4 + 0] = 0; break;
                    case 'F': benes_cfg[i * 32 + j * 4 + 3] = 1; benes_cfg[i * 32 + j * 4 + 2] = 1; benes_cfg[i * 32 + j * 4 + 1] = 1; benes_cfg[i * 32 + j * 4 + 0] = 1; break;
                    }
                    j++;
                }
                i++;
            }
        }
        fin.close();

        for (int i = 0; i < 32; i++){
            data[i] = aa % 2; data[32 + i] = bb % 2; data[64 + i] = cc % 2; data[96 + i] = dd % 2;
            aa /= 2; bb /= 2; cc /= 2; dd /= 2;
        }

        bool benes_cfg_new[832];
        for (int i = 0; i < 832; i++) {
            int m, n;
            m = i / 13; n = i % 13;
            int src, dest;
            dest = n * 64 + m;
            src = convert[i];
            benes_cfg_new[src] = benes_cfg[dest];
        }

        int* core_out = benes_n(data, benes_cfg_new, 128);

        aa = core_out[31]; bb = core_out[63]; cc = core_out[95]; dd = core_out[127];
        for (int i = 1; i < 32; i++){
            aa *= 2; bb *= 2; cc *= 2; dd *= 2;
            aa += core_out[31 - i];
            bb += core_out[63 - i];
            cc += core_out[95 - i];
            dd += core_out[127 - i];
        }
        delete[]core_out;

        int* out = new int[4];
        out[0] = (int)aa;
        out[1] = (int)bb;
        out[2] = (int)cc;
        out[3] = (int)dd;
		if (print_num >= 3)
		{
			cout << "[OPERATOR RCU " << m_rcu_id << " BENES " << m_benes_id << "]";
			cout << hex << "a:" << a << ",b:" << b << ",c:" << c << ",d:" << d << ",";
			cout << "out[0]:(" << out[0] << "," << out[1] << "," << out[2] << "," << out[3] << ")" << dec << endl;


			m_bcu_mod->m_cout_out_log << "[OPERATOR RCU " << m_rcu_id << " BENES " << m_benes_id << "]";
			m_bcu_mod->m_cout_out_log << hex << "a:" << a << ",b:" << b << ",c:" << c << ",d:" << d << ",";
			m_bcu_mod->m_cout_out_log << "out[0]:(" << out[0] << "," << out[1] << "," << out[2] << "," << out[3] << ")" << dec << endl;
			
		}
        return out;
    }


    int * benes_mod::benes_2(int *in, bool *cfg)
    {
        int * out = new int[2];
        out[0] = cfg[0] ? in[1] : in[0];
        out[1] = cfg[0] ? in[0] : in[1];
        return out;
    }

    int * benes_mod::benes_n(int *in, bool* cfg, int width)
    {
        int SIZE = width;
        int HALF = SIZE / 2;
        double temp = (log10(HALF) / log10(2));
        int OFST = (2 * temp - 1)*HALF / 2;
        int * out = new int[SIZE];
        int * in_temp = new int[SIZE];
        int * out_temp = new int[SIZE];

        for (int i = 0; i < SIZE; i++) in_temp[i] = in[i];

        for (int i = 0; i < SIZE; i++) out_temp[i] = cfg[i / 2] ? in_temp[i / 2 * 2 + (i % 2 == 0)] : in_temp[i];
        for (int i = 0; i < SIZE; i++) in_temp[i] = i < HALF ? out_temp[i * 2] : out_temp[i%HALF * 2 + 1];

        int *benes_0;
        int *benes_1;
        if (SIZE == 4){
            benes_0 = benes_2(in_temp, cfg + HALF);
            benes_1 = benes_2(in_temp + HALF, cfg + HALF + OFST);
        }
        else{
            benes_0 = benes_n(in_temp, cfg + HALF, HALF);
            benes_1 = benes_n(in_temp + HALF, cfg + HALF + OFST, HALF);
        }

        for (int i = 0; i < HALF; i++) {
            out_temp[i] = benes_0[i];
            out_temp[i + HALF] = benes_1[i];
        }

        delete[] benes_0;
        delete[] benes_1;

        for (int i = 0; i < SIZE; i++) in_temp[i] = out_temp[i % 2 ? i / 2 + HALF : i / 2];
        for (int i = 0; i < SIZE; i++) out_temp[i] = cfg[i / 2 + HALF + OFST + OFST] ? in_temp[i / 2 * 2 + (i % 2 == 0)] : in_temp[i];

        for (int i = 0; i < SIZE; i++) out[i] = out_temp[i];

        delete[] in_temp;
        delete[] out_temp;

        return out;
    }


    void benes_mod::get_xml_resolver(XmlResolver *xml_resolver)
    {
        m_rca_xml_slv = xml_resolver;
    }

	char * benes_mod::del_char(char* str, char ch)
	{
		unsigned char i = 0, j = 0;
		while (str[i] != '\0')
		{
			if (str[i] != ch)//只有在bai不是空格的情况下目du标才会移动赋值
			{
				str[j++] = str[i];
			}
			i++;//源一zhi直移动

		}
		str[j] = '\0';
		return str;

	}

}
