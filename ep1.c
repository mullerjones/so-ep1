#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define MAX_LINE_LENGTH 100

static int debug = 0;

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

void printProcessData(FILE *file, ProcessData *data) {
    fprintf(file,
            "%s %d %d %d",
            data->name,
            data->t0,
            data->dt,
            data->deadline);
}

// Reads trace file, returns linked list with process data
Process* get_processes(FILE *trace) {
    char line[MAX_LINE_LENGTH];
    Process *process_list, *new_node, *last_node;
    ProcessData *new_data;

    // Create head of list
    process_list = malloc(sizeof(Process));
    process_list->data = NULL;
    process_list->next = NULL;
    last_node = process_list;

    // read line from file, add data to the end of the list
    while (fgets(line, MAX_LINE_LENGTH, trace) != NULL) {
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

    sleep(pd->dt);

    return NULL;
}

void fcfs_scheduler(Process *process_list, FILE *output_file) {
    /*
     * inicial_t: time at beginning of simulation (t == 0);
     * t: current time
     * t0: time current process started, relative to beginning of simulation
     */
    struct timespec inicial_t, t, t0;
    int tf, tr;

    Process *current = process_list->next;

    clock_gettime(CLOCK_REALTIME, &inicial_t);

    while (current) {
        if (debug) {
            fprintf(stderr, "new process arrived: ");
            printProcessData(stderr, current->data);
            fprintf(stderr, "\n");
        }

        pthread_create(&(current->thread), NULL, work, current->data);
        clock_gettime(CLOCK_REALTIME, &t0);

        if (pthread_join(current->thread, NULL)) {
            fprintf(stderr, "failed to join thread %s\n", current->data->name);
        }
        clock_gettime(CLOCK_REALTIME, &t);

        tf = t.tv_sec - inicial_t.tv_sec;
        tr = t.tv_sec - t0.tv_sec;

        if (debug) {
            fprintf(stderr, "process ended: ");
            fprintf(stderr, "%s %d %d\n", current->data->name, tf, tr);
        }

        fprintf(output_file, "%s %d %d\n", current->data->name, tf, tr);

        process_list->next = current->next;
        free(current->data);
        free(current);
        current = process_list->next;
    }

    free(process_list);
}

int main(int argc, char **argv) {
    int scheduler;
    FILE *trace, *output;
    char *output_file;
    Process *process_list;

    if (argc < 4) {
        printf("Usage: %s <scheduler_id> <trace_file> <output_file>\n", argv[0]);
        return -1;
    }

    if (argc == 5 && argv[4][0] == 'd') {
        debug = 1;
        printf("debug mode active\n");
    }

    scheduler = atoi(argv[1]);

    if ((trace = fopen(argv[2], "r")) == 0) {
        printf("error: failed to open trace file.\nExiting ...\n");
        return -1;
    }
    process_list = get_processes(trace);
    fclose(trace);

    output_file = argv[3];

    output = fopen(output_file, "w");

    switch (scheduler) {
        case 1:
            fcfs_scheduler(process_list, output);
            break;
        default:
            fprintf(stderr, "error: invalid scheduler value\n");
    }
    process_list = NULL;

    fclose(output);

    return 0;
}
