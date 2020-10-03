CFLAGS = -g -Wall

default: bccsh ep1 ep1teste


bccsh:
	gcc $(CFLAGS) bccsh.c -o bccsh -lreadline -lhistory

ep1: ep1.c
	gcc $(CFLAGS) ep1.c -o ep1 -pthread

ep1teste: ep1teste.c
	gcc $(CFLAGS) ep1teste.c -o ep1teste -pthread

clean:
	rm -f ep1 ep1.o bccsh bccsh.o ep1teste ep1teste.o

