/**
 * @brief       : 
 *
 * @file        : api_debug.c
 * @version     : v0.0.1
 * @date        : 2017.9.6

 */

#define API_ASYM_ENABLE 1

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

#include <windows.h>

#include "api_debug.h"
#include "api_comm.h"
#include "token_helper.h"

#define ASYM_DATA_DDR			1
#define ASYM_DATA_REPLAY		2
#define CMD_TYPE_COMMON			3

/*  异步方式用户数据返回分两种方式：
	1：replay payload -> user_p 
	2: dst_ddr -> user_p */
typedef struct rcp_cmd_ 
{
    long int type;			// 类型
    uint8_t *user_p;		// 读取数据存放地址 
    uint32_t user_size;		// 读取数据长度
    uint8_t *dst_ddr;		// 读取数据ddr地址
    uint8_t finish;			// 指令完成
    uint8_t error;			// 错误码
    uint32_t size;			// 帧长度
    uint8_t data[128];		// 组帧数据
} rcp_cmd_t;

int32_t rcp_fd;
static int32_t msgid;
static rcp_cmd_t rcp_cmd;

#define WM_RCP_DEBUG (WM_USER+100)
static DWORD rcp_thread_id;
HANDLE rcp_thread;
HANDLE rcp_thread_start_event;

static int rcp_rpu_send_frame(rcp_cmd_t *cmd)
{
	struct rpu_asym_read_reply reply;
	memset(&reply, 0, sizeof(reply));
	int ret = exchange_token(rcp_fd, cmd->data, cmd->size, &reply, sizeof(reply));
    if(ret != 0)
    {
        cmd->error = 0xff;
        printf(" token return faild = %d\n", ret);
        return cmd->error;
    }
    else
    {
    	cmd->error = token_g_result(&reply);
    	memcpy(cmd->user_p, ((cmd->type - 1) ? reply.payload : cmd->dst_ddr), cmd->user_size);
    }
    cmd->finish = 1;
    return API_SUCCESS;
}

static int rcp_send_cmd(long int type, uint8_t *user_p, uint32_t user_size, 
								uint8_t *dst_ddr, uint8_t *data, uint32_t size)
{
	rcp_cmd.type = type;
	rcp_cmd.user_p = user_p;
	rcp_cmd.user_size = user_size;
	rcp_cmd.dst_ddr = dst_ddr;
	rcp_cmd.finish = 0;
	rcp_cmd.error = 0;
	rcp_cmd.size = size;
	memcpy(rcp_cmd.data, data, size);

#if API_ASYM_ENABLE

#ifdef __GNUC__111
    if (msgsnd(msgid, (void *)&rcp_cmd, 128, 0) == -1)
    {
        RCP_DEBUG_LOG("msgsnd failed\n");
        return API_FAILED;
    }
#else
	SetEvent(rcp_thread_start_event);
#endif
    return API_SUCCESS;
#else
    return rcp_rpu_send_frame(&rcp_cmd);
#endif
}

/**
*
* @brief rpu debug 模式下复位操作
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_rpu_dbg_reset(void)
{
	RCP_DEBUG_LOG("%s\n", __func__);

	// 组帧
	struct reset_rpu_req req;

	set_token_hdr(&req, TOKEN_PRIORITY_MID_2, RPU_DEBUG, RESET_RPU, sizeof(req));

	return rcp_send_cmd(CMD_TYPE_COMMON, NULL, 0, NULL, &req, req.command_len);
}

/**
*
* @brief rpu 进入调试模式
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_rpu_dbg_enter(void)
{
	RCP_DEBUG_LOG("%s\n", __func__);

	// 组帧
	struct enter_dbg_req req;

	set_token_hdr(&req, TOKEN_PRIORITY_MID_2, RPU_DEBUG, ENTER_DBG, sizeof(req));

	return rcp_send_cmd(CMD_TYPE_COMMON, NULL, 0, NULL, &req, req.command_len);
}

/**
*
* @brief rpu 退出调试模式
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_rpu_dbg_exit(void)
{
	RCP_DEBUG_LOG("%s\n", __func__);

	// 组帧
	struct exit_dbg_req req;

	set_token_hdr(&req, TOKEN_PRIORITY_MID_2, RPU_DEBUG, EXIT_DBG, sizeof(req));

	return rcp_send_cmd(CMD_TYPE_COMMON, NULL, 0, NULL, &req, req.command_len);
}

/**
*
* @brief 初始化 rpu 配置信息
*
* @param[in] filename 配置信息文件名.
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_rpu_dbg_init_config( uint8_t cmd_bits_len, const char *filename)
{
	RCP_DEBUG_LOG("%s\n", __func__);

	uint32_t size;
	struct ddr_info *ddrinfo = get_ddr_info();

	uint8_t *src = ddrinfo->ddr_addr;

	rcp_read_file_size(filename, &size);
	rcp_read_file_data(filename, size, src);

	// 组帧
	struct init_cfg_req req;
  
	rpu_dbg_init_config_req_token(&req, ddrinfo->ddr_phy, size, cmd_bits_len);

	return rcp_send_cmd(CMD_TYPE_COMMON, NULL, 0, NULL, &req, req.command_len);
}

/**
*
* @brief 初始化 rpu sbox
*
* @param[in] file_bin：sbox 配置信息文件名.
* @param[in] file_group：offset data, index data.
*
* @return：见 RCP_STATUS 枚举
*
* e.g.:
* int sboxGroupOffset[] = {-1, -1,};
* unsigned int xxx_sboxDataIndex[] = {1, 1,};
* unsigned int xxx_sboxDataIndexSize = 2;
*/
RCP_STATUS EXPORT rcp_rpu_dbg_init_sbox(const char *file_sbox, const char *file_group)
{
	uint32_t sbox_size = 0;		/** sbox 数据长度 */
	uint8_t *ptr_vir_sbox;		/** sbox 数据存放虚拟地址 */
	uint32_t ptr_phy_sbox;  	/** sbox 数据存放物理地址 */
	uint32_t offset_size = 0;   /** offset 数据长度 */
	int32_t *ptr_vir_offset; 	/** offset 数据存放虚拟地址 */
	uint32_t ptr_phy_offset; 	/** offset 数据存放物理地址 */
	uint32_t index_size = 0;    /** index 数据长度 */
	uint32_t *ptr_vir_index; 	/** index 数据存放虚拟地址 */
	uint32_t ptr_phy_index; 	/** index 数据存放物理地址 */

	/** 获取 ddr 虚拟地址和物理地址，ddr 空间为 4M */
	struct ddr_info *ddrinfo = get_ddr_info();
	ptr_vir_sbox = ddrinfo->ddr_addr;
	ptr_phy_sbox = ddrinfo->ddr_phy;

	/** 将 sbox 数据读入到 ptr_vir_sbox 中 */
	rcp_read_file_size(file_sbox, &sbox_size);
	rcp_read_file_data(file_sbox, sbox_size, ptr_vir_sbox);

	/** 读取 group 数据到临时缓存 */
	uint32_t group_size = 0;
	rcp_read_file_size(file_group, &group_size);
	char *ptr_malloc_group = malloc(group_size + 1);
	char *ptr_group = ptr_malloc_group;
	rcp_read_file_data(file_group, group_size, (uint8_t *)ptr_group);

	// 去除 空格和回车换行
	str_remove(ptr_group, ' ');
	str_remove(ptr_group, '\n');
	str_remove(ptr_group, '\t');
	uint32_t str_group_size = strlen(ptr_group);

	/** offset 解析 */
	ptr_vir_offset = (int32_t *)(ptr_vir_sbox + sbox_size);
	ptr_phy_offset = ptr_phy_sbox + sbox_size;
	char *ptr_malloc_offset = malloc(str_group_size);
	char *ptr_offset = ptr_malloc_offset;
	memcpy(ptr_offset, ptr_group, str_group_size);
	ptr_offset = strtok(ptr_offset, "{"); // 定位到第一个数组元素
	ptr_offset = strtok(NULL, "}");

	ptr_offset = strtok(ptr_offset, ",");
	ptr_vir_offset[offset_size] = atoi(ptr_offset);
	offset_size++;
	for(;;)
	{
		ptr_offset = strtok(NULL, ",");
		if(ptr_offset)
		{
			ptr_vir_offset[offset_size] = atoi(ptr_offset);
			offset_size++;
		}
		else
		{
			break;
		}
	}

	offset_size = offset_size*4;

	/** index 解析 */
	ptr_vir_index = (uint32_t *)(ptr_vir_offset + offset_size/4);
	ptr_phy_index = ptr_phy_offset + offset_size;
	char *ptr_malloc_index = malloc(str_group_size);
	char *ptr_index = ptr_malloc_index;
	memcpy(ptr_index, ptr_group, str_group_size);
	ptr_index = strtok(ptr_index, "{"); // 定位到第一个数组元素
	ptr_index = strtok(NULL, "{"); // 定位到第二个数组元素
	ptr_index = strtok(NULL, "}");

	ptr_index = strtok(ptr_index, ",");
	*(ptr_vir_index + index_size) = strtoul(ptr_index, 0, 0);
	index_size++;
	for(;;)
	{
		ptr_index = strtok(NULL, ",");
		if(ptr_index)
		{
			*(ptr_vir_index + index_size) = strtoul(ptr_index, 0, 0);
			index_size++;
		}
		else
		{
			break;
		}
	}

	index_size = index_size*4;

	free(ptr_malloc_group);
	free(ptr_malloc_offset);
	free(ptr_malloc_index);

	// 组帧
	struct init_sbox_req req;
 
	rpu_dbg_init_sbox_req_token(&req, ptr_phy_sbox, sbox_size, ptr_phy_offset, offset_size, ptr_phy_index, index_size);

	return rcp_send_cmd(CMD_TYPE_COMMON, NULL, 0, NULL, &req, req.command_len);
}

/**
*
* @brief rpu fifo 写入数据
*
* @param[in] id: fifo id.
* @param[in] data: 待写入数据存放地址.
* @param[in] size: 待写入数据长度.
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_rpu_dbg_write_fifo(uint8_t id, uint8_t *data, uint32_t size)
{
	RCP_DEBUG_LOG("%s\n", __func__);

	struct ddr_info *ddrinfo = get_ddr_info();
	uint8_t *src = ddrinfo->ddr_addr;
	memcpy(src, data, size);

	// 组帧
	struct r_w_fifo_req req;

	rpu_dbg_rw_fifo_req_token(&req, ddrinfo->ddr_phy, size, id, RPU_DBG_WRITE);

	return rcp_send_cmd(CMD_TYPE_COMMON, NULL, 0, NULL, &req, req.command_len);
}


/**
*
* @brief rpu gprf 写入数据
*
* @param[in] num: 写入gprf的内存块编号.
* @param[in] offset: 写入gprf的偏移地址.
* @param[in] data: 待写入数据存放地址.
* @param[in] size: 待写入数据长度.
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_rpu_dbg_write_gprf(uint8_t num, uint16_t offset, uint8_t *data, uint32_t size)
{
	RCP_DEBUG_LOG("%s\n", __func__);

	struct ddr_info *ddrinfo = get_ddr_info();
	uint8_t *src = ddrinfo->ddr_addr;
	memcpy(src, data, size);

	// 组帧
	struct w_gprf_req req;

	rpu_dbg_write_gprf_req_token(&req, ddrinfo->ddr_phy, size, num, offset);

	return rcp_send_cmd(CMD_TYPE_COMMON, NULL, 0, NULL, &req, req.command_len);
}

/**
*
* @brief 启动 rpu 
*
* @param[in] execute_times: RPU计算执行的总次数（一次是算法的一个burst）.
* @param[in] start_num: 算法在配置信息中的起始位置编号.
* @param[in] end_num: 算法在配置信息中的结束位置编号.
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_rpu_dbg_start(uint32_t execute_times, uint32_t start_num, uint32_t end_num)
{
	RCP_DEBUG_LOG("%s\n", __func__);

	// 组帧
	struct start_rpu_req req;

	rpu_dbg_start_req_token(&req, execute_times, start_num, end_num);

	return rcp_send_cmd(CMD_TYPE_COMMON, NULL, 0, NULL, &req, req.command_len);
}

/**
*
* @brief rpu 运行 N 周期
*
* @param[in] cycle: 执行周期
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_rpu_dbg_run(uint32_t cycle)
{
	RCP_DEBUG_LOG("%s\n", __func__);

	// 组帧
	struct run_n_cycles_req req;

	rpu_dbg_run_req_token(&req, cycle);

	return rcp_send_cmd(CMD_TYPE_COMMON, NULL, 0, NULL, &req, req.command_len);
}

/**
*
* @brief rpu 设置 pe(计算节点) 数据
*
* @param[in] pe: 计算节点数据地址
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_rpu_dbg_write_pe(pe_t *pe)
{
	RCP_DEBUG_LOG("%s\n", __func__);

	// 组帧
	struct r_w_pe_req req;

	rpu_dbg_write_pe_req_token(&req, pe->bcu_id, pe->rcu_id, pe->pe_id, pe->data, RPU_DBG_WRITE);

	return rcp_send_cmd(CMD_TYPE_COMMON, NULL, 0, NULL, &req, req.command_len);
}

/**
*
* @brief rpu 获取 pe(计算节点) 数据
*
* @param[out] pe: 计算节点数据地址
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_rpu_dbg_read_pe(pe_t *pe)
{
	RCP_DEBUG_LOG("%s\n", __func__);

	// 组帧
	struct r_w_pe_req req;

	rpu_dbg_write_pe_req_token(&req, pe->bcu_id, pe->rcu_id, pe->pe_id, pe->data, RPU_DBG_READ);

	return rcp_send_cmd(ASYM_DATA_REPLAY, (uint8_t *)(pe->data), 16, NULL, &req, req.command_len);
}

/**
*
* @brief rpu 获取所有 pe(计算节点) 数据
*
* @param[out] data: 数据
* @param[out] size: 长度(default:4*4*13)
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_rpu_dbg_read_bcu_pe(uint32_t id, uint8_t *data, uint32_t size)
{
	RCP_DEBUG_LOG("%s\n", __func__);

	struct ddr_info *ddrinfo = get_ddr_info();
	uint8_t *dst = ddrinfo->ddr_addr;

	// 组帧
	struct read_bcu_pe_req req;

	rpu_dbg_read_bcu_pe_req_token(&req, ddrinfo->ddr_phy, size, id);

	return rcp_send_cmd(ASYM_DATA_DDR, data, size, dst, &req, req.command_len);
}

/**
*
* @brief rpu 读取 fifo 数据
*
* @param[in] id: fifo id
* @param[out] data: 数据
* @param[out] size: 长度
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_rpu_dbg_read_fifo(uint8_t id, uint8_t *data, uint32_t size)
{
	RCP_DEBUG_LOG("%s\n", __func__);

	struct ddr_info *ddrinfo = get_ddr_info();
	uint8_t *dst = ddrinfo->ddr_addr;

	// 组帧
	struct r_w_fifo_req req;

	rpu_dbg_rw_fifo_req_token(&req, ddrinfo->ddr_phy, size, id, RPU_DBG_READ);

	return rcp_send_cmd(ASYM_DATA_DDR, data, size, dst, &req, req.command_len);
}


RCP_STATUS EXPORT rcp_rpu_dbg_read_cmd_status(uint32_t *status)
{
	*status = rcp_cmd.finish;
	if(rcp_cmd.error)
	{
		return rcp_cmd.error;
	}
	return API_SUCCESS;
}

/**
*
* @brief rpu 读取 fifo 状态
*
* @param[out] status: fifo 状态
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_rpu_dbg_read_fifo_status(uint32_t *status)
{
	RCP_DEBUG_LOG("%s\n", __func__);

	// 组帧
	struct read_fifo_status_req req;

	set_token_hdr(&req, TOKEN_PRIORITY_MID_2, RPU_DEBUG, READ_FIFO_STATUS, sizeof(req));

	return rcp_send_cmd(ASYM_DATA_REPLAY, (uint8_t *)status, 4, NULL, &req, req.command_len);
}

/**
*
* @brief rpu 读取 rca 状态
*
* @param[out] status: rca 状态
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_rpu_dbg_read_rca_status(uint32_t *status)
{
	RCP_DEBUG_LOG("%s\n", __func__);

	// 组帧
	struct read_rca_status_req req;

	set_token_hdr(&req, TOKEN_PRIORITY_MID_2, RPU_DEBUG, READ_RCA_STATUS, sizeof(req));

	return rcp_send_cmd(ASYM_DATA_REPLAY, (uint8_t *)status, 4, NULL, &req, req.command_len);
}

#if API_ASYM_ENABLE
/**
* @brief rpu debug 线程
*/
#ifdef __GNUC__111
static int32_t *thread_debug(void *arg)
#else
static unsigned __stdcall thread_debug(void *arg)
#endif
{
    for(;;)
    {
#ifdef __GNUC__111
        msgrcv(msgid, (void *)&rcp_cmd, 128, 0, 0);
#else
		WaitForSingleObject(rcp_thread_start_event, INFINITE);
#endif
        rcp_rpu_send_frame(&rcp_cmd);
    }
    return API_SUCCESS;
}
#endif

/**
*
* @brief rpu debug 线程创建
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_rpu_dbg_init(void)
{
	RCP_DEBUG_LOG("%s\n", __func__);

#if API_ASYM_ENABLE

#ifdef __GNUC__111
    pthread_t thread_debug_id;
    int32_t res = pthread_create(&thread_debug_id, NULL, (void *)thread_debug, NULL);
    if(res == -1)
    {
        RCP_DEBUG_LOG("%s [%d] failed.\n", __func__, __LINE__);
        return API_FAILED;
    }
    msgid = msgget((key_t)1234, 0666 | IPC_CREAT);
    if(msgid == -1)  
    {  
        RCP_DEBUG_LOG("%s [%d] failed.\n", __func__, __LINE__);
        return API_FAILED;
    }

#else
	rcp_thread_start_event = CreateEvent(NULL, FALSE, FALSE, NULL);
	rcp_thread = CreateThread(NULL, 0, &thread_debug, NULL, 0, &rcp_thread_id);
	if (rcp_thread == 0)
	{
		RCP_DEBUG_LOG("start thread failed,errno:%d\n", GetLastError());
		CloseHandle(rcp_thread_start_event);
		return 1;
	}
#endif

#endif

    return API_SUCCESS;
}
