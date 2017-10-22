#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "vbe.h"
#include "video_gr.h"

/* Private global variables */

static char *video_mem;		/* Process address to which VRAM is mapped */


static unsigned h_res = H_RES;		/* Horizontal screen resolution in pixels */
static unsigned v_res = V_RES;		/* Vertical screen resolution in pixels */
static unsigned bits_per_pixel = 8; /* Number of VRAM bits per pixel */

void *vg_init(unsigned short mode)
{
	struct reg86u reg;
	vbe_mode_info_t mode_info;


//		if(vbe_get_mode_info(mode, &mode_info) != OK)
//			return NULL;



		reg.u.w.ax = 0x4F02; // VBE call, function 02 -- set VBE mode
		reg.u.w.bx = 1 << 14 | 0x105; // set bit 14: linear framebuffer
		reg.u.b.intno = 0x10;
		if (sys_int86(&reg) != OK)
		{
			printf("set_vbe_mode: sys_int86() failed \n");
			return NULL;
		}

//------------------------------------------------------------------------------
		//Setting Gloabal Variables
//		h_res = mode_info.XResolution;
//		v_res = mode_info.YResolution;
//		bits_per_pixel = mode_info.BitsPerPixel;


		int r;
		struct mem_range mr;
		unsigned int vram_base = 0xE0000000;//(phys_bytes) mode_info.PhysBasePtr; /* VRAM's physical addresss *///0xE0000000
		unsigned int vram_size = h_res*v_res*(bits_per_pixel /8);  /* VRAM's size, but you can use
		                            the frame-buffer size, instead */
		//void *video_mem;         /* frame-buffer VM address */

		/* Allow memory mapping */

		mr.mr_base = (phys_bytes) vram_base;
		mr.mr_limit = mr.mr_base + vram_size;

		if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
		   panic("sys_privctl (ADD_MEM) failed: %d\n", r);

		/* Map memory */

		video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

		if(video_mem == MAP_FAILED)
		   panic("couldn't map video memory");

		return video_mem;
}

int vg_exit() {
  struct reg86u reg86;

  reg86.u.b.intno = 0x10; /* BIOS video services */

  reg86.u.b.ah = 0x00;    /* Set Video Mode function */
  reg86.u.b.al = 0x03;    /* 80x25 text mode*/



  if( sys_int86(&reg86) != OK ) {
      printf("\tvg_exit(): sys_int86() failed \n");
      return 1;
  } else
      return 0;
}
