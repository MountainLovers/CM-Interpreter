#The makefile for interpreter based on Flex and Bison
CC = gcc
FLAG = -w
interpreter: main.o util.o cm.tab.o cm.lex.o symtab.o analyze.o
	$(CC) $(FLAG) -o ../bin\&test/interpreter main.o util.o cm.tab.o cm.lex.o symtab.o analyze.o
main.o: main.c parse.h analyze.h
	$(CC) $(FLAG) -c main.c
cm.tab.o:cm.tab.c parse.h
	$(CC) $(FLAG) -c cm.tab.c
cm.tab.c:cm.y
	bison -d cm.y
cm.lex.o:cm.lex.c cm.tab.h
	$(CC) $(FLAG) -c cm.lex.c
cm.lex.c:cm.l
	flex -o cm.lex.c cm.l
symtab.o:symtab.c symtab.h globals.h
	$(CC) $(FLAG) -c symtab.c
analyze.o:analyze.c globals.h symtab.h analyze.h
	$(CC) $(FLAG) -c analyze.c
util.o:util.c
	$(CC) $(FLAG) -c util.c

clean:
	-rm *.o