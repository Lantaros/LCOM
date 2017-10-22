#Makefile for lab5

COMPILER_TYPE= gnu

CC= gcc

PROG= lab5
SRCS= lab5.c test5.c mouse.c kbd.c timer.c read_xpm.c vbe.c video_gr.c video.c

CFLAGS= -Wall

DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -ldriver -lsys -llm

LDFLAGS+= -L .

MAN=

clean:
	rm -rf *.d
	rm -rf *.o
	rm -rf lab5
	rm -rf .depend

.include <bsd.prog.mk>
.include <bsd.gcc.mk>
