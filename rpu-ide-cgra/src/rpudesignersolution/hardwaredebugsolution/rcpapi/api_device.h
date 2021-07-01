/**
 * @brief       : 
 *
 * @file        : api_device.h
 * @version     : v0.0.1
 * @date        : 2017.09.06

 */
#ifndef __API_DEVICE_H__
#define __API_DEVICE_H__

#include "api_typedef.h"

/**
*
* @brief 打开设备
*
* @param[in] fd: 设备描述符
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_device_open(int32_t *fd);

/**
*
* @brief 关闭设备
*
* @param[in] fd: 设备描述符
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_device_close(int32_t fd);

/**
*
* @brief 复位设备
*
* @param[in] fd: 设备描述符
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_device_reset(int32_t fd);

/**
*
* @brief 设备信息查询
*
* @param[out] data: 设备信息待存放地址
* @param[out] size: 设备信息长度存放地址
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_device_poll(uint8_t *data, uint32_t *size);

/**
*
* @brief 设备信息查询
*
* @param[out] data: 设备固件信息待存放地址
* @param[out] size: 设备固件信息长度存放地址
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_device_firmware_version(uint8_t *data, uint32_t *size);

/**
*
* @brief 更新设备固件
*
* @param[in] filename: 固件文件名
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_device_update_firmware(const char *filename);

/**
*
* @brief 设备固件更新进度查询
*
* @param[out] status: 固件更新状态
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_device_update_progress_query(uint32_t *status);

#endif