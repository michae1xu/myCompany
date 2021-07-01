#ifndef TOKEN_HELPER_H_MNRT9ZMA
#define TOKEN_HELPER_H_MNRT9ZMA

#include "token.h"

#define _offsetof_(base, type, member) ((volatile unsigned int*)((char*)(base) + offsetof(type, member)))


#define _HI32(value64bit) (( ((value64bit) >> 16)>>16)&(0xFFFFFFFF))
#define _LO32(value64bit) ( (value64bit) &(0xFFFFFFFF))


#define set_token_hdr2(token, seqno,prio, main_op,sub_op,len) \
do { \
    ((volatile u32*)(token))[0] = ((sub_op) << 28) | ((main_op) <<19) |  ((prio) <<16) | ((seqno)&0xFFFF); \
    ((volatile u32*)(token))[1] = (len); \
}while(0)

#define set_token_seqno(token, seq) \
do { \
    ((volatile unsigned int*)(token))[0] = (((volatile unsigned int*)(token))[0] &(0xFFFF0000)) | ((seq) & 0xFFFF); \
}while(0)

#define set_reply_token_hdr2(token, seqno, glb_resultsrc, glb_result, error,len) \
do { \
    ((volatile u8*)(token))[0] = ( ((error) << 31) |((glb_result) << 19) | ((glb_resultsrc) <<16) | ((seqno)&0xFFFF); \
    ((volatile u8*)(token))[1] = (len)&(0b1111111111111); \
}while(0)

#define _memcpy32(src, dst, len) \
do{ \
    int i =0; \
    for(i = 0; i < (len)/4; i++){ \
        ((volatile u32*)(src))[i] = ((volatile u32*)(dst))[i]; \
    } \
}while(0)

/*----------------------------------------------------------------------------
* rcp_get_token_id()
*
* This function return a unique token id since startup.
*
* Return Value:
*     A unique token id.
*/
static u16 g_rcp_token_id = 0;        // TokenID reference counter
static inline u16 rcp_get_token_id(void)
{
	// Generate a TokenID
	g_rcp_token_id++;
	if (g_rcp_token_id == 0)
	{
		g_rcp_token_id++;
	}
	return g_rcp_token_id;
}

static inline void set_token_hdr(void *token, u32 prio, u32 gop, u32 sop, u32 len)
{
	((u32*)(token))[0] = ((sop) << 28) | ((gop) << 19) | ((prio) << 16) | rcp_get_token_id();
	((u32*)(token))[1] = (len);
}

static inline void rpu_mac_req_token(volatile struct rpu_mac_req* req, u64 data, u32 data_len, 
                                    u8 algo, u8 mode, u32 keylen)
{
    set_token_hdr(req, TOKEN_PRIORITY_MID_2, RPU_MAC, 0, sizeof(*req));

    req->dma_in_addr_low    = _LO32(data);
    req->dma_in_addr_high  = _HI32(data);
    req->dma_in_data_len    = data_len;

    //req->param = ((algo) <<0) |  ((mode)<<5) | ((keylen) << 7);
    req->algo = algo;
    req->mode = mode;
    req->key_len = keylen;
}

static inline void dma_req_token(volatile struct dma2dev_req*req, u64 dst, u64 src, u32 srclen)
{
    set_token_hdr(req, TOKEN_PRIORITY_MID_2, RPU_DMA, 0, sizeof(*req));

    req->dma_in_addr_low = _LO32(src);
    req->dma_in_addr_high = _HI32(src);
    req->dma_in_data_len = srclen;

    req->dma_out_addr_low = _LO32(dst);
    req->dma_out_addr_high = _HI32(dst);
    req->dma_out_data_len = srclen;
}

static inline void hash_req_token(volatile struct hash_req* req, u64 data, u32 datalen, u8 algo, u8 mode)
{
    set_token_hdr(req, TOKEN_PRIORITY_MID_2, RPU_HASH, 0, sizeof(*req));

    req->dma_in_addr_low    = _LO32(data);
    req->dma_in_addr_high  = _HI32(data);
    req->dma_in_data_len    = datalen;

    //req->param =  ((algo) <<0) |  ((mode)<<5) ;
    req->algo = algo;
    req->mode = mode;
}

static inline void crypto_req_token(volatile struct crypto_req* req, u64 src, u32 srclen, u64 dst, u32 dstlen, u8 algo, u8 algo_mode, u8 keylen, u8 mode, u8 do_encrypt)
{
    set_token_hdr(req, TOKEN_PRIORITY_MID_2, RPU_CRYPTO, 0, sizeof(*req));

    req->dma_in_addr_low    = _LO32(src);
    req->dma_in_addr_high  = _HI32(src);
    req->dma_in_data_len    = srclen;

    req->dma_out_addr_low = _LO32(dst);
    req->dma_out_addr_high = _HI32(dst);
    req->dma_out_data_len = dstlen;

    //req->param =  ((algo) << 0) |  (algo_mode<<5) | (keylen << 9) | (mode <<16) | (do_encrypt << 31);
    req->algo =  algo;
    req->algo_mode = algo_mode; 
    req->key_len = keylen;
    req->mode = mode;
    req->encrypt = do_encrypt?1:0 ;
}

static inline void dcs_crypto_req_token(volatile struct dcs_crypto_req* req, u64 src, u32 srclen, u64 dst, u32 dstlen, u8 algo, u8 algo_mode, u8 keylen, u8 mode, u8 do_encrypt)
{
    set_token_hdr(req, TOKEN_PRIORITY_MID_2, DCS_ALGO, DCS_ALGO_SM4, sizeof(*req));

    req->dma_in_addr_low    = _LO32(src);
    req->dma_in_addr_high  = _HI32(src);
    req->dma_in_data_len    = srclen;

    req->dma_out_addr_low = _LO32(dst);
    req->dma_out_addr_high = _HI32(dst);
    req->dma_out_data_len = dstlen;

    //req->param =  ((algo) << 0) |  ((algo_mode)<<5) | (keylen << 9) | (mode <<16) | (do_encrypt << 31);
    req->algo = algo;
    req->algo_mode = algo_mode;
    req->key_len = keylen;
    req->mode = mode;
    req->encrypt = do_encrypt?1:0 ;
}

static inline void dcs_hash_req_token(volatile struct dcs_hash_req* req, u64 data, u32 datalen, u8 algo, u8 mode)
{
    set_token_hdr(req, TOKEN_PRIORITY_MID_2, DCS_ALGO, DCS_ALGO_SM3, sizeof(*req));

    req->dma_in_addr_low    = _LO32(data);
    req->dma_in_addr_high  = _HI32(data);
    req->dma_in_data_len    = datalen;

    //req->param =  ((algo) <<0) |  ((mode)<<5) ;
    req->algo = algo;
    req->mode = mode;
}


static inline void dcs_sign_verify_req_token(volatile struct dcs_sign_verify_req* req, 
        u64 msg, u32 msglen, u64 signdata, u32 signlen, u64 cert, u32 certlen,
        u8 algo, u8 hash_type, u8 do_hash, u8 cerloc, u8 cert_type)
{
    set_token_hdr(req, TOKEN_PRIORITY_MID_2, DCS_ALGO, DCS_ALGO_SIGN_VERIFY, sizeof(*req));

    req->data_in_addr_low   = _LO32(msg);
    req->data_in_addr_high  = _HI32(msg);
    req->data_in_len        = msglen;

    req->sign_in_addr_low   = _LO32(signdata);
    req->sign_in_addr_high  = _HI32(signdata);
    req->sign_in_len        = signlen;

    req->cert_in_addr_low   = _LO32(cert);
    req->cert_in_addr_high  = _HI32(cert);
    req->cert_in_len        = certlen;

    //req->param =  (algo << 0) |  (hash_type<<5) | (do_hash << 11) | (cerloc <<12) | (cert_type << 16);
    req->algo = algo;
    req->hash_type = hash_type;
    req->do_hash =  do_hash;
    req->cert_loc = cerloc;
    req->cert_type = cert_type;
}

static inline void dcs_sign_req_token(volatile struct dcs_sign_req* req, 
        u64 msg, u32 msglen, u64 signresult, u32 signlen, u64 cert, u32 certlen,
        u8 algo, u8 hash_type, u8 do_hash, u8 cerloc, u8 cert_type)
{
    set_token_hdr(req, TOKEN_PRIORITY_MID_2, DCS_ALGO, DCS_ALGO_SIGN, sizeof(*req));

    req->data_in_addr_low   = _LO32(msg);
    req->data_in_addr_high  = _HI32(msg);
    req->data_in_len        = msglen;

    req->sign_out_addr_low   = _LO32(signresult);
    req->sign_out_addr_high  = _HI32(signresult);
    req->sign_out_len        = signlen;

    req->cert_in_addr_low   = _LO32(cert);
    req->cert_in_addr_high  = _HI32(cert);
    req->cert_in_len        = certlen;

    //req->param =  (algo << 0) |  (hash_type<<5)
    //               | (do_hash << 11) | (cerloc <<12)
    //               | (cert_type << 16);

    req->algo = algo;
    req->hash_type = hash_type;
    req->do_hash =  do_hash;
    req->cert_loc = cerloc;
    req->cert_type = cert_type;
}


static inline void nop_req_token(volatile struct nop_req*req,  u64 src, u32 srclen, u64 dst, u32 dstlen)
{
    ((volatile u32*)(req))[0] = ((0) << 28) | ((0)<<24) | ((0) <<19) | ((0) <<16) | 0; 
    req->identy = 0xa5a5a5a;
    req->datalen = srclen;
    req->inputdata_addr_lo = _LO32(src);
    req->inputdata_addr_hi = _HI32(src);
    req->inputdatalen = srclen;
    req->outputdata_addr_lo = _LO32(dst);
    req->outputdata_addr_hi = _LO32(dst);
    req->outputdatalen = dstlen;
}

static inline void mgr_msi_req_token(volatile struct mgr_int_set_req* req,  u32 msiaddr_lo, u32 msiaddr_hi, u32 value)
{
    set_token_hdr(req, TOKEN_PRIORITY_HI, MGR, MGR_PCIE_INT_SET, sizeof(*req));
    req->msi_int_addr_low = msiaddr_lo;
    req->msi_int_addr_high = msiaddr_hi;
    req->msi_int_value = value;
}

static inline int token_valid_error_check(void* token, int from, u32 seq)
{
    if( seq != token_seqno(token) || 
            token_error(token) != 0 || 
            ((struct token2xeon_hdr*)token)->command_len < sizeof(struct token2xeon_hdr) || 
            token_g_result_src(token) != from){
        return -1;
    }
    return 0;
}

static inline void mgr_bios_remap_req_token(volatile struct ddr_bios_remap_req* req, u8 do_map, u32 ddr_lo, u32 ddr_hi, u32 size)
{
    set_token_hdr(req, TOKEN_PRIORITY_HI, MGR, MGR_BIOS_DDR_REMAP,sizeof(*req));

    req->unremap = !do_map ? 1:0;
    req->remap_addr_low = ddr_lo;
    req->remap_addr_high = ddr_hi;
    req->remap_size = size;
}

static inline void mgr_rcp_ddr_remap_req_token(volatile struct ddr_rcp_remap_req* req, u8 do_map, u8 cnt, struct remap_addr_info* addrs)
{
    int i = 0;

    set_token_hdr(req, TOKEN_PRIORITY_HI, MGR, MGR_RCP_DDR_REMAP, 
            sizeof(*req) + cnt*sizeof(struct remap_addr_info));

    req->unremap = !do_map ? 1:0;
    req->segcnt = cnt;
    for (i = 0; i < cnt; ++i) {
        req->seg[i].remap_addr_low =addrs[i].remap_addr_low;
        req->seg[i].remap_addr_high =addrs[i].remap_addr_high;
        req->seg[i].remap_size =addrs[i].remap_size;
    }
}

static inline void mgr_fwupd_req_token(struct mgr_fwupd_req*req, u64 data, u32 datalen, u8 mode, u32 datacrc)
{
    set_token_hdr(req, TOKEN_PRIORITY_HI, MGR, MGR_FWUPD, sizeof(*req));

    req->dma_in_addr_low = _LO32(data);
    req->dma_in_addr_high = _HI32(data);
    req->dma_in_data_len = _HI32(datalen);
    req->mode = mode;
    req->in_data_crc = datacrc;
}

static inline void mgr_sysctrl_req_token(struct mgr_sysctrl_req *req, u32 code)
{
	set_token_hdr(req, TOKEN_PRIORITY_HI, MGR, MGR_SYS_CTRL, sizeof(*req));
	req->ctrlcode = code;
}

/** rpu debug token helper */
static inline void rpu_dbg_init_config_req_token(volatile struct init_cfg_req *req, 
                                                    u64 src, u32 len, u8 cmd_bits_len)
{
    set_token_hdr(req, TOKEN_PRIORITY_MID_2, RPU_DEBUG, INIT_CFG, sizeof(*req));

    req->dma_in_addr_low   = _LO32(src);
    req->dma_in_addr_high  = _HI32(src);
    req->dma_in_data_len   = len;
    req->cmd_bits_len = cmd_bits_len;
}

static inline void rpu_dbg_init_sbox_req_token(volatile struct init_sbox_req *req, 
                    u64 sbox, u32 sboxlen, u64 offset, u32 offsetlen, u64 index, u32 indexlen)
{
    set_token_hdr(req, TOKEN_PRIORITY_MID_2, RPU_DEBUG, INIT_SBOX, sizeof(*req));

    req->sbox_data_addr_low = _LO32(sbox);
    req->sbox_data_addr_high = _HI32(sbox);
    req->sbox_data_len = sboxlen;

    req->sbox_grp_ofs_addr_low = _LO32(offset);
    req->sbox_grp_ofs_addr_high = _HI32(offset);
    req->sbox_grp_ofs_len = offsetlen;

    req->sbox_index_addr_low = _LO32(index);
    req->sbox_index_addr_high = _HI32(index);
    req->sbox_index_len = indexlen;
}

static inline void rpu_dbg_rw_fifo_req_token(volatile struct r_w_fifo_req *req, 
                                                    u64 data, u32 len, u8 id, u8 type)
{
    set_token_hdr(req, TOKEN_PRIORITY_MID_2, RPU_DEBUG, RW_FIFO, sizeof(*req));

    req->dma_in_out_addr_low = _LO32(data);
    req->dma_in_out_addr_high = _HI32(data);
    req->dma_in_out_data_len = len;
    req->fifo_id = id;
    req->r_w = type;
}

static inline void rpu_dbg_write_gprf_req_token(volatile struct w_gprf_req *req, 
                                                    u64 data, u32 len, u8 num, u16 offset)
{
    set_token_hdr(req, TOKEN_PRIORITY_MID_2, RPU_DEBUG, W_GPRF, sizeof(*req));

    req->dma_in_addr_low = _LO32(data);
    req->dma_in_addr_high = _HI32(data);
    req->dma_in_data_len = len;
    req->gprf_mem_ofs = offset;
    req->gprf_mem_num = num;
}

static inline void rpu_dbg_start_req_token(volatile struct start_rpu_req *req, 
                                            u32 execute_times, u32 start_num, u32 end_num)
{
    set_token_hdr(req, TOKEN_PRIORITY_MID_2, RPU_DEBUG, START_RPU, sizeof(*req));

    req->execute_times = execute_times;
    req->cmd_start_num = start_num;
    req->cmd_end_num = end_num;
}

static inline void rpu_dbg_run_req_token(volatile struct run_n_cycles_req *req, u32 cycle)
{
    set_token_hdr(req, TOKEN_PRIORITY_MID_2, RPU_DEBUG, RUN_N_CYCLE, sizeof(*req));

    req->cycle_num = cycle;
}

static inline void rpu_dbg_write_pe_req_token(volatile struct r_w_pe_req *req, 
                                u8 bcu_id, u8 rcu_id, u8 pe_id, u32 data[4], u8 type)
{
    set_token_hdr(req, TOKEN_PRIORITY_MID_2, RPU_DEBUG, RW_PE, sizeof(*req));

    req->bcu_id = bcu_id;
    req->rcu_id = rcu_id;
    req->pe_id = pe_id;
    if(type == RPU_DBG_WRITE)
        memcpy((void*)req->data_in, (void*)data, 16);
    req->r_w = type;
}

static inline void rpu_dbg_read_bcu_pe_req_token(volatile struct read_bcu_pe_req *req, u64 data, u32 len, u32 id)
{
    set_token_hdr(req, TOKEN_PRIORITY_MID_2, RPU_DEBUG, READ_BCU_PE, sizeof(*req));

    req->dma_out_addr_low = _LO32(data);
    req->dma_out_addr_high = _HI32(data);
    req->dma_out_data_len = len;
    req->bcu_id = id;
}

static inline void mgr_bios_otp_req_token(volatile struct mgr_bios_otp_req* req,  u32 r_w, u32 *data)
{
    u32 i = 0;
    
    set_token_hdr(req, TOKEN_PRIORITY_HI, MGR, MGR_BIOS_OTP, sizeof(*req));
    req->r_w = r_w;
    
    if (r_w == 0)
    {
        for (i=0; i<4; i++)
        {
            req->otp_data_in[i] = *(data + i);
        }
    }
}

static inline void mgr_bios_ddr_remap_req_token(volatile struct ddr_bios_remap_req *req,
	u8 do_map,
	u64 ddr_addr,
	u32 size)
{
	set_token_hdr(req, TOKEN_PRIORITY_HI, MGR, MGR_BIOS_DDR_REMAP, sizeof(*req));

	req->unremap = !do_map ? 1 : 0;
	req->remap_addr_low = _LO32(ddr_addr);
	req->remap_addr_high = _HI32(ddr_addr);
	req->remap_size = size;
}

static inline void bios_crypto_req_token(volatile struct fixed_algo_crypto_req *req,
	u64 src, u32 srclen, u64 dst, u32 dstlen, u8 algo, u8 algo_mode, u8 keylen, u8 mode, u8 enc)
{
	set_token_hdr(req, TOKEN_PRIORITY_MID_2, FIXED_ALGO, FIXED_ALGO_CRYPTO, sizeof(*req));

	req->dma_in_addr_low = _LO32(src);
	req->dma_in_addr_high = _HI32(src);
	req->dma_in_data_len = srclen;

	req->dma_out_addr_low = _LO32(dst);
	req->dma_out_addr_high = _HI32(dst);
	req->dma_out_data_len = dstlen;

	req->algo = algo;
	req->algo_mode = algo_mode;
	req->key_len = keylen;
	req->encrypt = enc;
}

static inline void bios_hash_req_token(volatile struct fixalgo_hash_req *req,
	u64 data,
	u32 datalen,
	u8 algo,
	u8 mode)
{
	set_token_hdr(req, TOKEN_PRIORITY_MID_2, FIXED_ALGO, FIXED_ALGO_HASH, sizeof(*req));

	req->dma_in_addr_low = _LO32(data);
	req->dma_in_addr_high = _HI32(data);
	req->dma_in_data_len = datalen;
	req->algo = algo;
	req->mode = mode;
}

static inline void bios_hmac_req_token(volatile struct fixalgo_mac_req *req,
	u64 data,
	u32 datalen,
	u64 key,
	u32 keylen,
	u32 algo,
	u32 mode)
{
	set_token_hdr(req, TOKEN_PRIORITY_MID_2, FIXED_ALGO, FIXED_ALGO_MAC, sizeof(*req));

	req->dma_in_addr_low = _LO32(data);
	req->dma_in_addr_high = _HI32(data);
	req->dma_in_data_len = datalen;
	req->key_in_addr_low = _LO32(key);
	req->key_in_addr_high = _HI32(key);
	req->key_in_data_len = keylen;
	req->algo = algo;
	req->mode = mode;
}

static inline void bios_set_key_req_token(volatile struct rsa_set_key_req *req,
	u64 src,
	u32 srclen,
	u8 mode,
	u8 type,
	u8 hash)
{
	set_token_hdr(req, TOKEN_PRIORITY_MID_2, FIXED_ALGO, FIXED_ALGO_RSA_SET_KEY, sizeof(*req));

	req->key_in_addr_low = _LO32(src);
	req->key_in_addr_high = _HI32(src);
	req->key_in_len = srclen;
	req->key_mode = mode;
	req->key_type = type;
	req->hash_type = hash;
}

static inline void bios_sign_verify_req_token(volatile struct fixed_algo_sign_verify_req *req,
	u8 algo,
	u8 cert_loc,
	u8 cert_type,
	u64 sigaddr,
	u32 siglen,
	u64 dataddr,
	u32 datlen,
	u64 cert,
	u32 certlen)
{
	set_token_hdr(req, TOKEN_PRIORITY_MID_2, FIXED_ALGO, FIXED_ALGO_SIGN_VERIFY, sizeof(*req));

	req->sign_in_out_addr_low = _LO32(sigaddr);
	req->sign_in_out_addr_high = _HI32(sigaddr);
	req->sign_in_out_len = siglen;

	req->data_in_addr_low = _LO32(dataddr);
	req->data_in_addr_high = _HI32(dataddr);
	req->data_in_len = datlen;

	req->cert_in_addr_low = _LO32(cert);
	req->cert_in_addr_high = _HI32(cert);
	req->cert_in_len = certlen;

	req->algo = algo;
	req->digest_len = 0;

	req->cert_loc = cert_loc;
	req->cert_type = cert_type;
}

#endif /* end of include guard: TOKEN_HELPER_H_MNRT9ZMA */
