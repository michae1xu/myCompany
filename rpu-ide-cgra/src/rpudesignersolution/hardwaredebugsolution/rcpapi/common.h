
#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#pragma pack(push, 1)

struct ddr_info
{
	void *mmap_base;
	int mmap_size;
	uint64_t ddr_phy;
	uint64_t ddr_real_phy;
	void *ddr_addr;
	uint64_t ddr_len;
};

#pragma pack(pop)

int rcp_ddr_read(uint32_t offset, void* data, uint32_t size);
int rcp_ddr_write(uint32_t offset, void* data, uint32_t size);

int rcp_ddr_copy(void *dst, void *src, size_t n, uint32_t flag);
int rcp_bios_ddr_copy(void *dst, void *src, size_t n, uint32_t flag);

int open_dev();
int close_dev(int fd);
struct ddr_info *get_ddr_info();
struct ddr_info *get_bios_ddr_info();
int exchange_token(int fd, void *token, int len, void *reply, int replylen);

#endif /* __COMMON_H__ */
