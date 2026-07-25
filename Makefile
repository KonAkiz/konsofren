CC=c99

SRC=examples/main.c

OBJ=$(SRC:.c=.o)

TARGET=program

CFLAGS=-Wall -Wextra -Werror -pedantic -std=c99

INC=-I.

LIBS=-L. -lm -lX11 -lXrandr

.PHONY: test

${TARGET}: ${OBJ}
	${CC} ${OBJ} -o ${TARGET} ${CFLAGS} ${INC} ${LIBS}

%.o: %.c
	${CC} -c ${CFLAGS} $< -o $@ ${INC}

test: ${TARGET}
	@./${TARGET}

clean:
	rm -vf ${TARGET} ${OBJ}
