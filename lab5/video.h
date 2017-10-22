#ifndef	__VIDEO_H
#define __VIDEO_H


//VBE
	//VBE Return Status
	#define FUNC_SUPPORTED 				0x4F //AL
	#define FUNC_CALL_SUCCESS 			0x00 //AH
	#define FUNC_CALL_FAILED 			0x01 //AH
	#define FUNC_NOT_SUPPORTED_HW_CONF 	0x02 //AH
	#define FUNC_CALL_INVALID_VBE_MODE 	0x03 //AH
	//Note: if AL != 4FH -> Function is not supported
	#define INTERRUPT_VECTOR 0x10


int draw_square(char* video_mem, unsigned short x, unsigned short y, unsigned short size, unsigned long color);

int draw_line(char* video_mem, unsigned short xi, unsigned short yi, unsigned short xf, unsigned short yf, unsigned long color);

int draw_line_v2(char* video_mem, unsigned short xi, unsigned short yi, unsigned short xf, unsigned short yf, unsigned long color);

int draw_point(char* video_mem, unsigned short x, unsigned short y, unsigned long color);

int print_pixmap(char* video_mem, char * pixmap, unsigned short x, unsigned short y, unsigned short x_size,unsigned short y_size);

int delete_pixmap(char* video_mem, char * pixmap, unsigned short x, unsigned short y, unsigned short x_size,unsigned short y_size);

int clear_video_screen(char* video_mem);



#endif
