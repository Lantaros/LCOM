#Makefile for lab4

COMPILER_TYPE= gnu

CC= gcc

PROG= lab4
SRCS= lab4.c test4.c mouse.c kbd.c timer.c

CFLAGS= -Wall

DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -ldriver -lsys

LDFLAGS+= -L .

MAN=

clean:
	rm -rf *.d
	rm -rf *.o
	rm -rf lab4
	rm -rf .depend

.include <bsd.prog.mk>
.include <bsd.gcc.mk>
