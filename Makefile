OBJS = possibles.o evaluation.o backgammon.o
CC = clang
CFLAGS = -Wall -pedantic -fPIC -O2 -std=c99


bertrand.so : $(OBJS)
	$(CC) -shared -O2 -o libiabackgammon.so $(OBJS)

%.o : %.c
	$(CC) $(CFLAGS) -c $<

clean :
	rm *.o *.so
