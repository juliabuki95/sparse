FLAGS = -ansi -Wall -Wextra -Werror -pedantic-errors
LIBS = -lm

all: ex2

clean:
	rm -rf *.o ex2

ex2: spmat.o main.o
	gcc spmat.o main.o -o ex2 $(LIBS)

spmat.o: spmat.c spmat.h
	gcc $(FLAGS) -c spmat.c

main.o: main.c spmat.h
	gcc $(FLAGS) -c main.c
