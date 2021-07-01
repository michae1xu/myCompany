
#include <windows.h>
#include <setupapi.h>
#include <guiddef.h> 
#include <initguid.h>
#include <winioctl.h>
#include <strsafe.h>

#include <stdbool.h>

#include "rcp.h"
#include "api_typedef.h"
#include "version.h"
#include "common.h"
#include "token.h"

#define RCP_PCIE_CMDQ_INT_ENABLE   0x4020
#define RCP_PCIE_CMDQ_INT_STATUS   0x4024
#define RCP_PCIE_CMDQ_INT_TRIGGER  0x4040

struct rcp
{
	BOOL opened;
	HANDLE handle;
	RCP_DDR_ADDR ddr;
	RCP_DDR_ADDR bios_ddr;
	RCP_MSI_INFO msi;
	HANDLE hMsiEvent;
	volatile PUCHAR ddrUserAddr;
	volatile PUCHAR barUserAddr;
	sys_info_t sys_info;
};

static struct rcp rcp;

static struct ddr_info ddr_info;

static inline bool rcp_ioctl(uint32_t code, void *in, uint32_t inlen, void *out, uint32_t outlen)
{
	BOOL bRet = TRUE;
	DWORD nBytesReturned = 0;

	bRet = DeviceIoControl(rcp.handle, code, in, inlen, out, outlen, &nBytesReturned, NULL);

	return bRet;
}

static inline BOOL rcp_bar_read(UINT offset, LPVOID data, DWORD size)
{
	if (!rcp_ioctl(IOCTL_RCP_BAR_SEEK, (LPVOID)&offset, sizeof(offset), NULL, 0))
		return FALSE;

	if (!rcp_ioctl(IOCTL_RCP_BAR_READ, NULL, 0, data, size))
		return FALSE;

	return TRUE;
}

static inline BOOL rcp_bar_write(UINT offset, LPVOID data, DWORD size)
{
	if (!rcp_ioctl(IOCTL_RCP_BAR_SEEK, (LPVOID)&offset, sizeof(offset), NULL, 0))
		return FALSE;

	if (!rcp_ioctl(IOCTL_RCP_BAR_WRITE, data, size, NULL, 0))
		return FALSE;

	return TRUE;
}

int rcp_ddr_read(uint32_t offset, void* data, uint32_t size)
{
	if (!rcp_ioctl(IOCTL_RCP_DDR_SEEK, &offset, sizeof(offset), NULL, 0))
		return FALSE;

	if (!rcp_ioctl(IOCTL_RCP_DDR_READ, NULL, 0, data, size))
		return FALSE;

	return TRUE;
}

int rcp_ddr_write(uint32_t offset, void* data, uint32_t size)
{
	if (!rcp_ioctl(IOCTL_RCP_DDR_SEEK, &offset, sizeof(offset), NULL, 0))
		return FALSE;

	if (!rcp_ioctl(IOCTL_RCP_DDR_WRITE, data, size, NULL, 0))
		return FALSE;

	return TRUE;
}

int rcp_ddr_copy(void *dst, void *src, size_t n, uint32_t flag)
{
	memcpy(dst, src, n);
	return 0;

	uint32_t offset = 0;

	if (flag)
		offset = (uint32_t)((uint8_t*)src - (uint8_t*)ddr_info.ddr_addr);
	else
		offset = (uint32_t)((uint8_t*)dst - (uint8_t*)ddr_info.ddr_addr);

	if (flag)
		return rcp_ddr_read(offset, dst, n);
	else
		return rcp_ddr_write(offset, src, n);
}

static inline BOOL rcp_get_ddr_phy_addr(PRCP_DDR_ADDR ddr)
{
	RCP_PHYSICAL_ADDRESS PhysicalAddress;

	if (!rcp_ioctl(IOCTL_RCP_DDR_GET_PHY_ADDR, NULL, 0, ddr, sizeof(RCP_DDR_ADDR)))
		return false;

	PhysicalAddress.Address = ddr->pa_xeon;
	PhysicalAddress.Length = ddr->sz;
	if (!rcp_ioctl(IOCTL_RCP_MAP_PHYSICAL_ADDRESS, &PhysicalAddress, sizeof(RCP_PHYSICAL_ADDRESS), &rcp.ddrUserAddr, sizeof(rcp.ddrUserAddr)))
		return false;	
	
	ddr_info.ddr_phy = ddr->pa;
	ddr_info.ddr_addr = (void*)rcp.ddrUserAddr; // ddr->va;
	ddr_info.ddr_real_phy = ddr->pa_xeon;
	ddr_info.ddr_len = ddr->sz;

	return true;
}

static inline BOOL rcp_get_msi_info(PRCP_MSI_INFO msi)
{
	return rcp_ioctl(IOCTL_RCP_MSI_GET_INFO, NULL, 0, msi, sizeof(RCP_MSI_INFO));
}

static inline BOOL rcp_register_event(HANDLE *handle)
{
	return rcp_ioctl(IOCTL_RCP_REGISTER_EVENT, handle, sizeof(handle), NULL, 0);
}

static inline BOOL rcp_interrupt_enable(UINT value)
{
	UINT clear = 0xff;
	rcp_bar_write(RCP_PCIE_CMDQ_INT_STATUS, &clear, sizeof(clear));

	return rcp_bar_write(RCP_PCIE_CMDQ_INT_ENABLE, &value, sizeof(value));
}

static inline BOOL rcp_set_trigger(UINT value)
{
	if (!rcp_ioctl(IOCTL_RCP_SET_TRIGGER, NULL, 0, NULL, 0))
		return FALSE;

	return TRUE;
}

static inline BOOL rcp_pool_status(UINT timeout)
{
	UINT status;
	DWORD ticks = GetTickCount();

	do
	{
		if (!rcp_bar_read(RCP_PCIE_CMDQ_INT_STATUS, &status, sizeof(status)))
		{
			Sleep(5);
		}

		if ((GetTickCount() - ticks) > timeout)
			return FALSE;
	} while ((status & 0x1) == 0);

	// Clear int status
	status = 0xff;
	if (!rcp_bar_write(RCP_PCIE_CMDQ_INT_STATUS, &status, sizeof(status)))
		return FALSE;

	return TRUE;
}

static int rcp_open_device(void)
{
	rcp.handle = CreateFile(RCP_DEVICE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == rcp.handle)
	{
		return -1;
	}

	rcp.opened = TRUE;

	return 0;
}

static int rcp_close_device(void)
{
	if (rcp.opened)
	{
		RCP_PHYSICAL_ADDRESS PhysicalAddress;

		PhysicalAddress.Address = rcp.ddr.pa_xeon;
		PhysicalAddress.Length = rcp.ddr.sz;
		if (!rcp_ioctl(IOCTL_RCP_UNMAP_PHYSICAL_ADDRESS, &PhysicalAddress, sizeof(RCP_PHYSICAL_ADDRESS), NULL, 0))
			return false;

		CloseHandle(rcp.hMsiEvent);
		CloseHandle(rcp.handle);
	}

	rcp.opened = FALSE;
	return 0;
}

int open_dev()
{
	int i;
	uint8_t data[1024*16];
	memset(data, 0, sizeof(data));
	memset(&rcp, 0, sizeof(rcp));
	memset(&ddr_info, 0, sizeof(ddr_info));

	rcp.opened = FALSE;
	rcp.handle = INVALID_HANDLE_VALUE;

	rcp.hMsiEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (rcp.hMsiEvent == NULL)
		return -1;
	
	if (rcp_open_device() < 0)
		return -1;

	if (rcp_register_event(&rcp.hMsiEvent) < 0)
		return -1;

	if (!rcp_get_ddr_phy_addr(&rcp.ddr))
		return -1;

	if (!rcp_get_msi_info(&rcp.msi))
		return -1;

	if (!rcp_interrupt_enable(1))
		return -1;

	if (rcp_get_sys_info(&rcp.sys_info) != API_SUCCESS)
		return -1;

#if 0
	if (rcp_bar_write(0, data, sizeof(data)) < 0)
		return -1;

	for (i = 0; i < rcp.ddr.sz / sizeof(data); i++)
	{
		if (rcp_ddr_write(i * sizeof(data), data, sizeof(data)) < 0)
			return -1;
	}
	FlushFileBuffers(rcp.handle);
#endif

	return 0;
}

int close_dev(int fd)
{
	return rcp_close_device();
}

struct ddr_info *get_ddr_info()
{
	return &ddr_info;
}

int exchange_token(int fd, void *token, int len, void *reply, int replylen)
{
#if 1

	DWORD ret;
	uint16_t token_id = ((uint32_t*)token)[0] & 0xffff;
	
    ResetEvent(rcp.hMsiEvent);

	// Write command token to PCIE bar space
	if (!rcp_bar_write(0, token, len))
		return -1;

	// Set trigger bit in PCIE bar register
	if (!rcp_set_trigger(1))
		return -1;

	// Wait status bit in PCIE bar register
	ret = WaitForSingleObject(rcp.hMsiEvent, 5000);
	switch (ret)
	{
	case WAIT_OBJECT_0:
		break;
	default:
		return -1;
	}

	// Read ack form PCIE bar memory
	if (!rcp_bar_read(0x2000, reply, replylen))
		return -1;

#else

	if (!rcp_ioctl(IOCTL_RCP_TOKEN_EXCHANGE, token, len, reply, replylen))
		return -1;

#endif

	if ((((uint32_t*)reply)[0] & 0xffff) != token_id)
		return -1;

	return 0;
}
