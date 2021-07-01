/**
 * @brief       : api 通用模块
 *
 * @file        : api_comm.h
 * @version     : v0.0.1
 * @date        : 2017.09.06

 */
#ifndef __API_COMM_H__
#define __API_COMM_H__

#include "api_typedef.h"

#include "common.h"


struct ddr_info* get_ddr_info();
uint32_t crc32(uint32_t crc, const void *buf, uint32_t size);
void str_remove(char *p_str, char select);
uint32_t str_charlen(char *p_str, char select);
int32_t rcp_read_file_size(const char *filename, uint32_t *size);
int32_t rcp_read_file_data(const char *filename, uint32_t size, uint8_t *data);

int rcp_get_sys_info(void *sys_info);

int open_dev(void);
int close_dev(int fd);
int exchange_token(int fd,void*token, int len, void*reply, int replylen);

#ifdef __GNUC__
#define rcp_ddr_copy(d,s,n,f) memcpy(d,s,n)
#endif

#endif // __API_COMM_H__
