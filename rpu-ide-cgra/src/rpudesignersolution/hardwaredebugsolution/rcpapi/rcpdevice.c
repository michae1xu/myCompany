
/**
* @brief       :
*
* @file        : api_device.c
* @version     : v0.0.1
* @date        : 2017.09.06

*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "api_device.h"
#include "api_comm.h"

/**
*
* @brief 打开设备
*
* @param[in] fd: 设备描述符
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_device_open(int32_t *fd)
{
#if SIMULATION
	*fd = 1;
#else
	extern int rcp_fd;
	rcp_fd = open_dev();
	*fd = rcp_fd;
	if (fd < 0)
	{
		printf("open dev faild!\n");
		return API_DEVICE_NOT_FOUND;
	}
#endif

	return API_SUCCESS;
}

/**
*
* @brief 关闭设备
*
* @param[in] fd: 设备描述符
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_device_close(int32_t fd)
{
#if SIMULATION

#else
	/* Close the original descriptor */
	if (close_dev(fd) < 0)
	{
		RCP_DEBUG_LOG("close device failed.\n");
		return API_DEVICE_NOT_FOUND;
	}
#endif

	return API_SUCCESS;
}

/**
*
* @brief 复位设备
*
* @param[in] fd: 设备描述符
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_device_reset(int32_t fd)
{
	return API_SUCCESS;
}

/**
*
* @brief 设备信息查询
*
* @param[out] data: 设备信息待存放地址
* @param[out] size: 设备信息长度存放地址
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_device_poll(uint8_t *data, uint32_t *size)
{
#if SIMULATION
	uint8_t info[] = "manufacturer: Tsinghua";
	*size = sizeof(info) - 1;
	memcpy(data, info, *size);
#else

#endif

	return API_SUCCESS;
}

/**
*
* @brief 设备信息查询
*
* @param[out] data: 设备固件信息待存放地址
* @param[out] size: 设备固件信息长度存放地址
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_device_firmware_version(uint8_t *data, uint32_t *size)
{
#if SIMULATION
	uint8_t ver[] = "0.0.1";
	*size = sizeof(ver) - 1;
	memcpy(data, ver, *size);
#else

#endif

	return API_SUCCESS;
}

/**
*
* @brief 更新设备固件
*
* @param[in] filename: 固件文件名
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_device_update_firmware(const char *filename)
{
	uint8_t *data;
	uint32_t size;
	rcp_read_file_size(filename, &size);
	data = malloc(size);
	rcp_read_file_data(filename, size, data);

#if SIMULATION
	RCP_DEBUG_LOG("firmware data[%d]: \n", size);
	for (int i = 0; i < size; i++)
	{
		RCP_DEBUG_LOG("%02x ", data[i]);
	}
	RCP_DEBUG_LOG("\n");
#else

#endif

	free(data);

	return API_SUCCESS;
}

/**
*
* @brief 设备固件更新进度查询
*
* @param[out] status: 固件更新状态
*
* @return：见 RCP_STATUS 枚举
*/
RCP_STATUS EXPORT rcp_device_update_progress_query(uint32_t *status)
{
#if SIMULATION
	*status = 1;
#else

#endif

	return API_SUCCESS;
}
