/**
 * @file    rcp.h
 * @author  
 * @version v0.1
 * @date    2012-06-22
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * Device driver user space header.
 *
 * <h2><center>&copy; COPYRIGHT 2016</center></h2>
 */

#ifndef __RCP_H__
#define __RCP_H__

#if defined(__GNUC__) && !defined(__MINGW32__)

#include <linux/types.h>
#include <asm/ioctl.h>

struct rcp_ddr_addr{
    unsigned long ddr_vir;
    int size;
    unsigned long ddr_phy;
    uint64_t ddr_real_phy;
};

struct ddr_info{
    void *mmap_base;
    int mmap_size;

    uint64_t ddr_phy;
	uint64_t ddr_real_phy;
    void* ddr_addr;
    uint64_t ddr_len;
};

#define MAX_SEG_CNT 3
struct rcp_ddr_info2{
    int seg_cnt;
    struct rcp_ddr_addr info[MAX_SEG_CNT];
};

struct token {
    int token_id;
    unsigned char* token;
    int todev_tokenlen;
    unsigned char* reply;
    int fromdev_tokenlen;
};


#define IOCTL_PCIE_GET_BAR_ADDR    _IOWR(0xF3, 1, unsigned char *)
#define IOCTL_PCIE_GET_DDR_ADDR    _IOWR(0xF3, 2, struct rcp_ddr_addr *)
#define IOCTL_PCIE_BAR_WRITE       _IOWR(0xF3, 3, unsigned char *)
#define IOCTL_PCIE_BAR_READ        _IOWR(0xF3, 4, unsigned char *)
#define IOCTL_PCIE_DDR_WRITE       _IOWR(0xF3, 5, unsigned char *)
#define IOCTL_PCIE_DDR_READ        _IOWR(0xF3, 6, unsigned char *)
#define IOCTL_PCIE_GET_RCP_STATUS  _IOWR(0xF3, 7, unsigned char *)
#define IOCTL_PCIE_CLR_RCP_STATUS  _IOWR(0xF3, 8, unsigned char *)


#define IOCTL_PCIE_DDR_REMAP        _IOWR(0xF3, 9, int)

#define IOCTL_PCIE_GET_DDR_ADDR2   _IOWR(0xF3, 10, struct rcp_ddr_info2*)
#define IOCTL_PCIE_TOKEN_EXCHANGE  _IOWR(0xF3, 11, struct token*)

#else

#define RCP_DEVICE_NAME  L"\\\\.\\RCPOffloading"

#define IOCTL_RCP_TOKEN_EXCHANGE   \
            CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_RCP_BAR_SEEK   \
            CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_IN_DIRECT, FILE_ANY_ACCESS)
#define IOCTL_RCP_BAR_READ   \
            CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
#define IOCTL_RCP_BAR_WRITE   \
            CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, METHOD_IN_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_RCP_DDR_SEEK   \
            CTL_CODE(FILE_DEVICE_UNKNOWN, 0x804, METHOD_IN_DIRECT, FILE_ANY_ACCESS)
#define IOCTL_RCP_DDR_READ   \
            CTL_CODE(FILE_DEVICE_UNKNOWN, 0x805, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
#define IOCTL_RCP_DDR_WRITE   \
            CTL_CODE(FILE_DEVICE_UNKNOWN, 0x806, METHOD_IN_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_RCP_DDR_GET_PHY_ADDR  \
            CTL_CODE(FILE_DEVICE_UNKNOWN, 0x807, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
#define IOCTL_RCP_DDR_GET_MAP_ADDR  \
            CTL_CODE(FILE_DEVICE_UNKNOWN, 0x808, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_RCP_MSI_GET_INFO  \
            CTL_CODE(FILE_DEVICE_UNKNOWN, 0x809, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_RCP_REGISTER_EVENT   \
            CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80a, METHOD_IN_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_RCP_SET_TRIGGER   \
            CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80b, METHOD_IN_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_RCP_MAP_PHYSICAL_ADDRESS \
            CTL_CODE(FILE_DEVICE_UNKNOWN, 0x810, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
#define IOCTL_RCP_UNMAP_PHYSICAL_ADDRESS \
            CTL_CODE(FILE_DEVICE_UNKNOWN, 0x811, METHOD_IN_DIRECT, FILE_ANY_ACCESS)

#pragma pack(push, 1)

struct ddr_info {
	void *mmap_base;
	int mmap_size;

	unsigned __int64 ddr_phy;
	unsigned __int64 ddr_real_phy;
	void* ddr_addr;
	unsigned __int64 ddr_len;
};

typedef struct _RCP_SYS_INFO
{
	unsigned __int32 magic;
	unsigned __int32 features;
	unsigned __int32 version;
	unsigned __int32 commit[5];
	unsigned __int32 checksum;
} RCP_SYS_INFO, *PRCP_SYS_INFO;

typedef struct _RCP_DDR_ADDR
{
	unsigned __int64 pa;
	unsigned __int64 pa_xeon;
	unsigned __int64 va;
	unsigned __int32 sz;
	unsigned __int64 bar_pa;
	unsigned __int64 bar_sz;
} RCP_DDR_ADDR, *PRCP_DDR_ADDR;

typedef struct _RCP_MSI_INFO
{
	unsigned int addr;
	unsigned int upper_addr;
	unsigned int data;
} RCP_MSI_INFO, *PRCP_MSI_INFO;

typedef struct _RCP_PHYSICAL_ADDRESS
{
	unsigned __int64 Address;
	unsigned __int32 Length;
} RCP_PHYSICAL_ADDRESS, *PRCP_PHYSICAL_ADDRESS;

#pragma pack(pop)

#endif

#endif /* __RCP_H__ */
