
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
* @brief ���豸
*
* @param[in] fd: �豸������
*
* @return���� RCP_STATUS ö��
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
* @brief �ر��豸
*
* @param[in] fd: �豸������
*
* @return���� RCP_STATUS ö��
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
* @brief ��λ�豸
*
* @param[in] fd: �豸������
*
* @return���� RCP_STATUS ö��
*/
RCP_STATUS EXPORT rcp_device_reset(int32_t fd)
{
	return API_SUCCESS;
}

/**
*
* @brief �豸��Ϣ��ѯ
*
* @param[out] data: �豸��Ϣ����ŵ�ַ
* @param[out] size: �豸��Ϣ���ȴ�ŵ�ַ
*
* @return���� RCP_STATUS ö��
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
* @brief �豸��Ϣ��ѯ
*
* @param[out] data: �豸�̼���Ϣ����ŵ�ַ
* @param[out] size: �豸�̼���Ϣ���ȴ�ŵ�ַ
*
* @return���� RCP_STATUS ö��
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
* @brief �����豸�̼�
*
* @param[in] filename: �̼��ļ���
*
* @return���� RCP_STATUS ö��
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
* @brief �豸�̼����½��Ȳ�ѯ
*
* @param[out] status: �̼�����״̬
*
* @return���� RCP_STATUS ö��
*/
RCP_STATUS EXPORT rcp_device_update_progress_query(uint32_t *status)
{
#if SIMULATION
	*status = 1;
#else

#endif

	return API_SUCCESS;
}
