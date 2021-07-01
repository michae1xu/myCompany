#define TWOFISH_ECB_DC_TWOFISH_MC_STR_ADDR 0
#define TWOFISH_ECB_DC_TWOFISH_MC_END_ADDR 0
#define TWOFISH_ECB_DC_TWOFISH_MC_CACHE_NUM 4

rcm_mem_rlm_array='{
0xc0200400 0x00000000 0x00000000 0xffff4000
};

rcm_mem_rcu_array='{
32'h00000000,32'h03000000,32'h00000000,32'h08000000,32'h02008020,32'h02a0b02e,32'h0000fe0a,32'h00000000,
32'h00000041,32'h02000000,32'h04000000,32'h26020001,32'h08022090,32'h01a0701e,32'h0000fe06,32'h00000000,
32'h00000042,32'h01000000,32'h00000000,32'h08000000,32'h04814058,32'h80c03810,32'h0000fe02,32'h00000000,
32'h00100000,32'h00000000,32'h00000000,32'h08000000,32'h02008020,32'h00201006,32'h0000fe00,32'h00000000
};

rcm_mem_bfu_array='{
32'h00000028,32'h0003fc45,32'h00000000,32'h00000000,
32'h20000028,32'h0003fc65,32'h00000000,32'h00000000,
32'h40000028,32'h0003fc05,32'h00000000,32'h00000000,
32'h60000028,32'h0003fc25,32'h00000000,32'h00000000,
32'he0000010,32'h0003ffff,32'h00000000,32'h00000000,
32'h50c20004,32'h0003fc51,32'h00000000,32'h00000000,
32'h9101fc24,32'h00023ff1,32'h00000000,32'h00000000,
32'h20000010,32'h0003fff8,32'h00000000,32'h00000000,
32'h60000010,32'h0003fff8,32'h00000000,32'h00000000,
32'hd1010404,32'h0003fff9,32'h00000000,32'h00000000,
32'h40000812,32'h0003f899,32'h00000000,32'h00000000,
32'h80000812,32'h0003f889,32'h00000000,32'h00000000,
32'h0000002a,32'h00020112,32'h00000000,32'h00000000,
32'h4000002a,32'h00021132,32'h00000000,32'h00000000,
32'h8000002a,32'h00022152,32'h00000000,32'h00000000,
32'hc000002a,32'h00023172,32'h00000000,32'h00000000,
32'h00000028,32'h0003f820,32'h00000000,32'h00000000,
32'h00000812,32'h0003fa81,32'h00000000,32'h00000000,
32'h00000812,32'h0003fa91,32'h00000000,32'h00000000,
32'h80000029,32'h0003f860,32'h00000000,32'h00000000,
32'h80000028,32'h0003fa87,32'h00000000,32'h00000000,
32'ha0000028,32'h0003fa97,32'h00000000,32'h00000000,
32'hc0000028,32'h0003faa7,32'h00000000,32'h00000000,
32'he0000028,32'h0003fab7,32'h00000000,32'h00000000
};

rcm_mem_route_array='{
32'h0fffffff,32'h00000000,32'h00000000,32'h00000000,
32'h00c10100,32'h00000000,32'h00000000,32'h00000000,
32'h0fffe140,32'h00000000,32'h00000000,32'h00000000
};

rcm_mem_mem_ctrl_array='{
32'h00000007,32'h18000000,32'h1e0001e0,32'h00000000,
32'h00000005,32'h00000000,32'h1e0001e0,32'h00000000,
32'h00000007,32'h00000000,32'hde0001e0,32'h00000001,
32'h00000001,32'h18000000,32'h1e0001e0,32'h00000000
};

