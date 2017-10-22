#Makefile for Final Project

COMPILER_TYPE= gnu

CC= gcc

PROG= proj
SRCS= proj.c game.c boat.c timer.c kbd.c mouse.c vbe.c video.c video_gr.c bitmap.c obstacle.c enemy.c cannonball.c assembly.S

CFLAGS= -Wall

DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -ldriver -lsys -llm

LDFLAGS+= -L .

MAN=

clean:
	rm -rf *.d
	rm -rf *.o
	rm -rf proj
	rm -rf .depend

.include <bsd.prog.mk>
.include <bsd.gcc.mk>
