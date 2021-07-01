
#ifndef __RCP_H__
#define __RCP_H__

#include <initguid.h> // required for GUID definitions

#define RCP_DEVICE_NAME  L"\\\\.\\RCPOffloading"

DEFINE_GUID(GUID_DEVINTERFACE_RCP,
	0xb74cfec2, 0x9366, 0x454a, 0xba, 0x71, 0x7c, 0x27, 0xb5, 0x14, 0x70, 0xa4);

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

#endif // __RCP_H__
