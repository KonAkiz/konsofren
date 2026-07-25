CC=c99

SRC=main.c

OBJ=$(SRC:.c=.o)

TARGET=program

CFLAGS=-Wall -Wextra -Werror -std=c99

INC=-I.

LIBS=-L.

.PHONY: test

${TARGET}: ${OBJ}
	${CC} ${SRC} -o ${TARGET} ${CFLAGS}

%.o: %.c
	${CC} -c ${CFLAGS} $< -o $@

test: ${TARGET}
	@./${TARGET}

clean:
	rm -vf ${TARGET} ${OBJ}
