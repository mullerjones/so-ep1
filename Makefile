CFLAGS = -g -Wall

default: bccsh ep1


bccsh:
	gcc $(CFLAGS) bccsh.c -o bccsh -lreadline -lhistory

ep1: ep1.c
	gcc $(CFLAGS) ep1.c -o ep1 -pthread

clean:
	rm -f ep1 ep1.o bccsh bccsh.o

