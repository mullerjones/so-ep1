#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>

void get_prompt(char *prompt);
void read_command(char *argv[], char *prompt, char* temp);

int main(int argc, char **argv)
{
   char temp[100];
   char prompt[4200];
   pid_t childpid;
   using_history();
   int running = 1;
   while (running)
   {
      get_prompt(prompt);
      read_command(argv, prompt, temp);
      if (strcmp(argv[0], "mkdir") == 0)
      {
         char *dir_name = argv[1];

         if (mkdir(dir_name, S_IFDIR | S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) == 0)
         {
            printf("directory %s created\n", dir_name);
         }
      }
      else if (strcmp(argv[0], "kill") == 0)
      {
         pid_t target_pid;

         if (strcmp(argv[1], "-9") == 0)
         {
            target_pid = atoi(argv[2]);

            if (kill(target_pid, SIGKILL) == 0)
            {
               printf("process %d killed\n", target_pid);
            }
         }
      }
      else if (strcmp(argv[0], "ln") == 0)
      {
         char *target, *link_path;

         if (strcmp(argv[1], "-s") == 0)
         {
            target = argv[2];
            link_path = argv[3];

            if (symlink(target, link_path) == 0)
            {
               printf("symlink created\n");
            }
         }
      }
      else if (strcmp(argv[0], "exit") == 0)
      {
         running = 0;
      }
      else// if (strcmp(argv[0], "./ep1") == 0)
      {
         //TODO: fork do ep1
         //argv[1]: nome do arquivo .txt
         if((childpid = fork()) == 0)
         {
            //filho
            printf("vou executar %s com argumento %s\n", argv[0], argv[1]);
            execve(argv[0],&argv[0],0);
         }
         else
         {
            //pai
            printf("childpid == %d\n", childpid);
            waitpid(-1, NULL, 0);
         }
         
      }
   }
   clear_history();
   exit(0);
}

void get_prompt(char *prompt)
{
   char user[32];   //max username length in linux
   char path[4096]; //max path length in linux
   if (getlogin_r(user, 32) != 0)
      printf("couldn't retrieve username\n");
   if (getcwd(path, 4096) == NULL)
      printf("couldn't get path\n");
   sprintf(prompt, "{%s@%s} ", user, path);
}

void read_command(char *argv[], char *prompt, char* temp)
{
   int n = 0;
   temp = readline(prompt);
   add_history(temp);
   argv[n++] = strtok(temp, " ");
   while (argv[n - 1] != NULL)
      argv[n++] = strtok(NULL, " ");
}