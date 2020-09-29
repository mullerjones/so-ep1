#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_LINE_LENGTH 100

// Struct to store data about the simulated processes
typedef struct {
    char name[31];
    int t0;
    int dt;
    int deadline;
} ProcessData;

// Linked list of 'Process' data
typedef struct proc_node {
    ProcessData *data;
    pthread_t thread;
    struct proc_node *next;
} Process;

// Reads trace file, returns linked list with process data
Process* get_processes(FILE *trace) {
    char line[100];
    Process *process_list, *new_node, *last_node;
    ProcessData *new_data;

    // Create head of list
    process_list = malloc(sizeof(Process));
    process_list->data = NULL;
    process_list->next = NULL;
    last_node = process_list;

    // read line from file, add data to the end of the list
    while (fgets(line, 100, trace) != NULL) {
        new_data = (ProcessData *) malloc(sizeof(ProcessData));

        sscanf(line,
               "%s %d %d %d ",
               new_data->name,
               &(new_data->t0),
               &(new_data->dt),
               &(new_data->deadline)
               );

        new_node = malloc(sizeof(Process));
        new_node->data =  new_data;
        new_node->next = NULL;
        last_node->next = new_node;
        last_node = new_node;
    }

    return process_list;
}

void* work(void* data) {
    ProcessData *pd = (ProcessData*) data;

    return (void *) pd->name;
}

int main(int argc, char **argv) {
    int scheduler;
    FILE *trace, *output;
    char *output_file;
    Process *process_list, *current, *aux;

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

    // create all threads in process list
    current = process_list->next;
    while (current != NULL) {
        pthread_create(&(current->thread), NULL, work, current->data);
        current = current->next;
    }

    output = fopen(output_file, "w");

    current = process_list->next;
    while (current != NULL) {
        char *name;

        pthread_join(current->thread, (void*) &name);
        fprintf(output, "o processo '%s' terminou\n", name);
        name = NULL;
        
        free(current->data);
        aux = current;
        current = current->next;
        free(aux);
    }
    free(process_list);
    fclose(output);

    return 0;
}
