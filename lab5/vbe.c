#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>

#include "vbe.h"
#include "video.h"
#include "lmlib.h"

#define LINEAR_MODEL_BIT 14

#define PB2BASE(x) (((x) >> 4) & 0x0F000)
#define PB2OFF(x) ((x) & 0x0FFFF)

int vbe_get_mode_info(unsigned short mode, vbe_mode_info_t *vmi_p)
{

	struct reg86u r;
	mmap_t map;
	if(lm_alloc(sizeof(vbe_mode_info_t), &map) == NULL)
		return 1;

	//phys_bytes buf;

	r.u.w.ax = 0x4F01; //VBE return status
	r.u.w.es = PB2BASE(map.phys);
	r.u.w.di = PB2OFF(map.phys);
	r.u.w.cx = mode;
	r.u.b.intno = 0x10;
	if (sys_int86(&r) != OK)
	{
		printf("\nvbe_get_mode_info: sys_int86() failed\n");
		return 1;
	}

	*vmi_p = *(vbe_mode_info_t*) map.virtual;
	lm_free(&map);

	if (r.u.w.ax != FUNC_SUPPORTED)
	{
		printf("\nvbe_get_mode_info: Function is not supported\n");
	}

	switch (r.u.w.ax)
	{
	case FUNC_CALL_SUCCESS:
		printf(
				"\nvbe_get_mode_info: function is supported; function call success\n");
		break;
	case FUNC_CALL_FAILED:
		printf(" - function call failed\n");
		break;
	case FUNC_NOT_SUPPORTED_HW_CONF:
		printf(
				" - function is not supported in the current hardware configuration\n");
		break;
	case FUNC_CALL_INVALID_VBE_MODE:
		printf(" - function call invalid in current video mode\n");
		break;

	default:
		printf(" - invalid VBE return status\n");
		break;
	}

	return 0;
}

