#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 100

// Struct to store data about the simulated processes
typedef struct {
    char name[31];
    int t0;
    int dt;
    int deadline;
} Process;

// Linked list of 'Process' data
typedef struct proc_node {
    Process *process;
    pthread_t thread;
    struct proc_node *next;
} ProcessNode;

// Reads trace file, returns linked list with process data
ProcessNode* get_processes(FILE *trace) {
    char line[100];
    ProcessNode *process_list, *new_node, *last_node;
    Process *new_process;

    // Create head of list
    process_list = malloc(sizeof(ProcessNode));
    process_list->process = NULL;
    process_list->next = NULL;
    last_node = process_list;

    // read line from file, add data to the end of the list
    while (fgets(line, 100, trace) != NULL) {
        new_process = (Process *) malloc(sizeof(Process));

        sscanf(line,
               "%s %d %d %d ",
               new_process->name,
               &(new_process->t0),
               &(new_process->dt),
               &(new_process->deadline)
               );

        new_node = malloc(sizeof(ProcessNode));
        new_node->process =  new_process;
        new_node->next = NULL;
        last_node->next = new_node;
        last_node = new_node;
    }

    return process_list;
}

int main(int argc, char **argv) {
    int scheduler;
    FILE *trace;
    char *output_file;
    ProcessNode *process_list;

    if (argc < 4) {
        printf("Usage: %s <scheduler_id> <trace_file> <output_file>", argv[0]);
        return -1;
    }

    scheduler = atoi(argv[1]);
    if ((trace = fopen(argv[2], "r")) == 0) {
        printf("Error: Failed to open trace file.\nExiting ...\n");
        return -1;
    }
    output_file = argv[3];

    process_list = get_processes(trace);
    fclose(trace);

    while(1) {
        printf("estou esperando terminar\n");
        sleep(2);
    }
    return 0;
}
