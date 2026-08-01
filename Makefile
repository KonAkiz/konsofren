.POSIX:

#
# COMPILER OPTIONS
#

CC=cc

CFLAGS=-Wall -Wextra -Werror -pedantic -std=c99

INC=-I. -Iexamples

LIBS=-L. -lm -lX11 -lXrandr

# SRCS

SRC=examples/main.c

# OBJS

OBJ=examples/main.o

# TARGETS

TARGET=program

FONT_TARGET=font-test

.PHONY: test clean all

all: ${TARGET} ${FONT_TARGET}

${TARGET}: ${OBJ} examples/RGFW.h konsofren.h
	${CC} ${OBJ} -o ${TARGET} ${CFLAGS} ${INC} ${LIBS}

${FONT_TARGET}: examples/test_konfont.c examples/konwinlib.h konfont.h konsofren.h
	${CC} examples/test_konfont.c -o ${FONT_TARGET} ${CFLAGS} ${INC} ${LIBS}

.SUFFIXES: .c .o

.c.o:
	${CC} -c ${CFLAGS} $< -o $@ ${INC}

test: all
	-./${TARGET}
	-./${FONT_TARGET}

.IGNORE: clean

clean:
	rm -vf ${TARGET} ${OBJ} ${FONT_TARGET}
