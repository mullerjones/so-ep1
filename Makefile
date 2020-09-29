default: bccsh ep1


bccsh:
	gcc -g bccsh.c -o bccsh -lreadline -lhistory

ep1: ep1.c
	gcc -g ep1.c -o ep1 -pthread

clean:
	rm -f ep1 ep1.o bccsh bccsh.o

