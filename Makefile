bertrand.so : possibles.o evaluation.o backgammon.o
	gcc -shared -O2 -o bertrand.so backgammon.o possibles.o evaluation.o
	
backgammon.o : backgammon.c
	gcc -Wall -pedantic -fPIC -O2 -c backgammon.c

evaluation.o : evaluation.c
	gcc -Wall -pedantic -fPIC -O2 -c evaluation.c
	
possibles.o : possibles.c
	gcc -Wall -pedantic -fPIC -O2 -c possibles.c