OPTIONS=-g -Wall
LIBS=

pacnewMngr : pacnewMngr.c
	gcc -o pacnewMngr pacnewMngr.c $(OPTIONS) $(LIBS)

clean :
	rm -f pacnewMngr
