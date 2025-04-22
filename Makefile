prog: main.o ennemie.o personne.o
	gcc main.o ennemie.o personne.o -o prog -lSDL -g -lSDL_image -lSDL_ttf -lm

main.o: main.c ennemie.h personne.h
	gcc -c main.c -g

ennemie.o: ennemie.c ennemie.h
	gcc -c ennemie.c -g

personnage.o: personne.c personne.h
	gcc -c personne.c -g

clean:
	rm -f prog *.o
