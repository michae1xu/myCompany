/**
 * @brief       : 
 *
 * @file        : api_debug.h
 * @version     : v0.0.1
 * @date        : 2017.09.06

 */
#ifndef __API_DEBUG_H__
#define __API_DEBUG_H__

#include "api_typedef.h"

#ifdef _MSC_VER
#pragma pack(1)
#endif

/**
* @brief 计算节点数据结构
*/
typedef struct pe_
{
    uint8_t bcu_id; 
    uint8_t rcu_id; 
    uint8_t pe_id; 
    uint32_t data[4];
#ifdef _MSC_VER
} pe_t;
#else
} __attribute__((packed)) pe_t;
#endif

#ifdef _MSC_VER
#pragma pack()
#endif

/**
*
* @brief rpu debug 线程创建
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_rpu_dbg_init(void);

/**
*
* @brief rpu debug 模式下复位操作
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_rpu_dbg_reset(void);

/**
*
* @brief rpu 进入调试模式
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_rpu_dbg_enter(void);

/**
*
* @brief rpu 退出调试模式
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_rpu_dbg_exit(void);

/**
*
* @brief 初始化 rpu 配置信息
*
* @param[in] filename 配置信息文件名.
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_rpu_dbg_init_config( uint8_t cmd_bits_len, const char *filename);


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

RCP_STATUS EXPORT rcp_rpu_dbg_init_sbox(const char *file_sbox, const char *file_group);

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
RCP_STATUS EXPORT rcp_rpu_dbg_write_fifo(uint8_t id, uint8_t *data, uint32_t size);


/**
*
* @brief rpu gprf 写入数据
*
* @param[in] id: gprf id.
* @param[in] data: 待写入数据存放地址.
* @param[in] size: 待写入数据长度.
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_rpu_dbg_write_gprf(uint8_t num, uint16_t offset, uint8_t *data, uint32_t size);

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
RCP_STATUS EXPORT rcp_rpu_dbg_start(uint32_t execute_times, uint32_t start_num, uint32_t end_num);

/**
*
* @brief rpu 运行 N 周期
*
* @param[in] cycle: 执行周期
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_rpu_dbg_run(uint32_t cycle);

/**
*
* @brief rpu 设置 pe(计算节点) 数据
*
* @param[in] pe: 计算节点数据地址
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_rpu_dbg_write_pe(pe_t *pe);

/**
*
* @brief rpu 获取 pe(计算节点) 数据
*
* @param[out] pe: 计算节点数据地址
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_rpu_dbg_read_pe(pe_t *pe);

/**
*
* @brief rpu 获取所有 pe(计算节点) 数据
*
* @param[out] data: 数据
* @param[out] size: 长度(default:4*4*13)
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_rpu_dbg_read_bcu_pe(uint32_t id, uint8_t *data, uint32_t size);

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
RCP_STATUS EXPORT rcp_rpu_dbg_read_fifo(uint8_t id, uint8_t *data, uint32_t size);

/**
*
* @brief rpu 读取 fifo 状态
*
* @param[out] status: fifo 状态
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_rpu_dbg_read_fifo_status(uint32_t *status);

/**
*
* @brief rpu 读取 rca 状态
*
* @param[out] status: rca 状态
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_rpu_dbg_read_rca_status(uint32_t *status);

/**
*
* @brief rpu 读取上一条指令完成情况（上层需要维护逻辑）
*
* @param[out] status: 指令完成情况
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_rpu_dbg_read_cmd_status(uint32_t *status);

#endif // __API_DEBUG_H__