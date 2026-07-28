.POSIX:

#
# COMPILER OPTIONS
#

CC=cc

CFLAGS=-Wall -Wextra -Werror -pedantic -std=c99

INC=-I.

LIBS=-L. -lm -lX11 -lXrandr

# SRCS

SRC=examples/main.c

# OBJS

OBJ=examples/main.o

# TARGETS

TARGET=program

.PHONY: test clean

${TARGET}: ${OBJ}
	${CC} ${OBJ} -o ${TARGET} ${CFLAGS} ${INC} ${LIBS}

.SUFFIXES: .c .o

.c.o:
	${CC} -c ${CFLAGS} $< -o $@ ${INC}

test: ${TARGET}
	@./${TARGET}

.IGNORE: clean

clean:
	rm -vf ${TARGET} ${OBJ}
