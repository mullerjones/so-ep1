#include "builtins.h"

int run_builtin(char *command, char *command_args[]) {
	if (strcmp(command, "mkdir") == 0) {
		char *dir_name = command_args[0];

		if (mkdir(dir_name, S_IFDIR|S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)
				== 0) {
			printf("directory %s created\n", dir_name);
		}
	}
	else if (strcmp(command, "kill") == 0) {
		pid_t target_pid;

		if (strcmp(command_args[0], "-9") == 0) {
			target_pid = atoi(command_args[1]);

			if (kill(target_pid, SIGKILL) == 0) {
				printf("process %d killed\n", target_pid);
			}
		}
	}
	else if (strcmp(command, "ln") == 0) {
		char *target, *link_path;

		if (strcmp(command_args[0], "-s") == 0) {
			target = command_args[1];
			link_path = command_args[2];

			if (symlink(target, link_path) == 0) {
				printf("symlink created\n");
			}
		}
	}
}
