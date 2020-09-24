#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    printf("Entrou no ep1\n");
    int i;
    for(i=0;i<argc;i++)
    {
        printf("%s\n", argv[i]);
    }
    while(1)
    {
        printf("estou esperando terminar\n");
        sleep(2);
    }
    return 0;
}