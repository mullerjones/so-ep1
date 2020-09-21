#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <readline/readline.h>
#include <readline/history.h>

void read_command(char *argv[], char* temp);

int main(int argc, char **argv)
{
   char temp[100];
   using_history();
   HISTORY_STATE *state;
   read_command(argv, temp);
   state = history_get_history_state();
   if(strcmp(argv[0], "mkdir") == 0)
   {
      //mkdir
      //argv[1] -> directory to create
   }
   else if(strcmp(argv[0], "kill") == 0)
   {
      //kill
      //argv[1] -> -9
      //argv[2] -> PID
   }
   else if (strcmp(argv[0], "ln") == 0)
   {
      //ln
      //argv[1] -> -s
      //argv[2] -> arquivo
      //argv[3] -> link
   }
   
   exit(0);
}

void read_command(char *argv[], char* temp)
{	
   int n = 0;
   char user[32]; //max username length in linux
   char path[4096]; //max path length in linux
   if(getlogin_r(user, 32) != 0)
      printf("couldn't retrieve username\n");
   if(getcwd(path, 4096) == NULL)
      printf("couldn't get path\n");
   printf("{%s@%s}", user, path);
   temp = readline(" ");
   add_history(temp);
   argv[n++] = strtok (temp," ");
   while (argv[n-1] != NULL)
   argv[n++] = strtok (NULL, " ");
}