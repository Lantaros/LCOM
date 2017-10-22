#Makefile for lab3

COMPILER_TYPE= gnu

CC= gcc

PROG= lab3
SRCS= lab3.c kbd.c test3.c timer.c assembly.S

CFLAGS= -Wall

DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -ldriver -lsys

LDFLAGS+= -L .

MAN=

clean:
	rm -rf *.d
	rm -rf *.o
	rm -rf lab3
	rm -rf .depend

.include <bsd.prog.mk>
.include <bsd.gcc.mk>
