#define _GNU_SOURCE // this is needed for pthread_tryjoin_np to work

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define MAX_LINE_LENGTH 100
#define SECOND 1000000000

static int debug = 0;
pthread_mutex_t lock;
int contextChanges = 0;

// Struct to store data about the simulated processes
typedef struct
{
    char name[31];
    int t0;
    int dt;
    int deadline;
} ProcessData;

// Linked list of 'Process' data
typedef struct proc_node
{
    ProcessData *data;
    pthread_t thread;
    struct timespec t0;
    struct proc_node *next;
} Process;

void printProcessData(FILE *file, ProcessData *data)
{
    fprintf(file,
            "%s %d %d %d",
            data->name,
            data->t0,
            data->dt,
            data->deadline);
}

// Reads trace file, returns linked list with process data
Process *get_processes(FILE *trace)
{
    char line[MAX_LINE_LENGTH];
    Process *process_list, *new_node, *last_node;
    ProcessData *new_data;

    // Create head of list
    process_list = malloc(sizeof(Process));
    process_list->data = NULL;
    process_list->next = NULL;
    last_node = process_list;

    // read line from file, add data to the end of the list
    while (fgets(line, MAX_LINE_LENGTH, trace) != NULL)
    {
        new_data = (ProcessData *)malloc(sizeof(ProcessData));

        sscanf(line,
               "%s %d %d %d ",
               new_data->name,
               &(new_data->t0),
               &(new_data->dt),
               &(new_data->deadline));

        new_node = malloc(sizeof(Process));
        new_node->data = new_data;
        new_node->next = NULL;
        last_node->next = new_node;
        last_node = new_node;
    }

    return process_list;
}

void *work(void *data)
{
    ProcessData *pd = (ProcessData *)data;
    struct timespec t0, t;
    long x = 1;
    pthread_mutex_lock(&lock);

    clock_gettime(CLOCK_MONOTONIC, &t0);
    clock_gettime(CLOCK_MONOTONIC, &t);

    //    sleep(pd->dt);
    while (compareTimes(pd->dt, t, t0) == 1)
    {
        for (int i = 0; i < 1000; i++)
        {
            x = (x * x) % 1000000000;
        }
        clock_gettime(CLOCK_MONOTONIC, &t);
    }
    pthread_mutex_unlock(&lock);

    return NULL;
}

int compareTimes(int value, struct timespec t, struct timespec init)
{
    int milisecs = (t.tv_nsec - init.tv_nsec) / 1000000;
    milisecs += (t.tv_sec - init.tv_sec) * 1000;
    if (value < milisecs)
        return -1;
    else if (value > milisecs)
        return 1;
    else
        return 0;
}

int calcTime(struct timespec t, struct timespec t0)
{
    int milisecs = (t.tv_nsec - t0.tv_nsec) / 1000000;
    milisecs += (t.tv_sec - t0.tv_sec) * 1000;
    return milisecs;
}

void mudouContexto()
{
    contextChanges++;
    fprintf(stderr, "Mudou de contexto, total = %d\n", contextChanges);
}

/*
 * TODO: Os processos ainda estão executando em paralelo. Precisamos
 *       adicionar mecanismo de controle de execução das threads
 * */
void fcfs_scheduler(Process *process_list, FILE *output_file)
{
    /*
     * initial_t: time at beginning of simulation (t == 0);
     * t: current time
     */
    struct timespec initial_t, t, sleep_t, rem;
    int tf, tr, total_t, total_tn;

    Process *current = process_list->next, *previous;
    Process *ready_list = malloc(sizeof(Process)), *last_ready;
    ready_list->data = NULL;
    ready_list->next = NULL;
    last_ready = ready_list;

    sleep_t.tv_sec = 0;
    sleep_t.tv_nsec = SECOND; // 1.0 seconds

    clock_gettime(CLOCK_MONOTONIC, &initial_t);
    clock_gettime(CLOCK_MONOTONIC, &t);

    while (current || ready_list->next)
    {
        total_t = t.tv_sec - initial_t.tv_sec;
        total_tn = t.tv_nsec - initial_t.tv_nsec;

        // add processes to queue of ready processes
        while (current && compareTimes(current->data->t0, t, initial_t) != 1)
        {
            if (debug)
            {
                fprintf(stderr, "new process arrived: ");
                printProcessData(stderr, current->data);
                fprintf(stderr, "\n");
            }

            process_list->next = current->next;
            current->next = NULL;

            last_ready->next = current;
            last_ready = current;

            current = process_list->next;
        }

        // start all ready processes and store the time
        current = ready_list->next;
        while (current)
        {
            if (current->t0.tv_sec == 0)
            {
                pthread_create(&(current->thread), NULL, work, current->data);
                clock_gettime(CLOCK_MONOTONIC, &(current->t0));
            }
            current = current->next;
        }

        nanosleep(&sleep_t, &rem);

        // remove finished processes from ready list and write output to file
        previous = ready_list;
        current = ready_list->next;
        while (current)
        {
            if (pthread_tryjoin_np(current->thread, NULL) == 0)
            {
                clock_gettime(CLOCK_MONOTONIC, &t);
                tr = calcTime(t, current->t0);
                //tr = t.tv_sec - current->t0.tv_sec;
                tf = calcTime(t, initial_t);
                //tf = t.tv_sec - initial_t.tv_sec;

                if (debug)
                {
                    fprintf(stderr, "process ended: ");
                    fprintf(stderr, "%s %d %d\n", current->data->name, tf, tr);
                }

                fprintf(output_file, "%s %d %d\n", current->data->name, tf, tr);

                previous->next = current->next;
                free(current->data);
                free(current);
                if(ready_list->next != NULL) mudouContexto();
                current = previous;
            }
            current = current->next;
        }

        current = process_list->next;
        clock_gettime(CLOCK_MONOTONIC, &t);
    }

    free(process_list);
}

void srtn_scheduler(Process *process_list, FILE *output_file)
{
    /*
     * initial_t: time at beginning of simulation (t == 0);
     * t: current time
     */
    struct timespec initial_t, t, sleep_t, rem;
    int tf, tr, total_t;

    Process *current = process_list->next, *previous;
    Process *ready_list = malloc(sizeof(Process)), *last_ready;
    ready_list->data = NULL;
    ready_list->next = NULL;
    last_ready = ready_list;

    sleep_t.tv_sec = 0;
    sleep_t.tv_nsec = SECOND; // 1.0 seconds

    clock_gettime(CLOCK_MONOTONIC, &initial_t);
    clock_gettime(CLOCK_MONOTONIC, &t);

    while (current || ready_list->next)
    {
        total_t = t.tv_sec - initial_t.tv_sec;

        // add processes to queue of ready processes
        while (current && current->data->t0 <= total_t)
        {
            if (debug)
            {
                fprintf(stderr, "new process arrived: ");
                printProcessData(stderr, current->data);
                fprintf(stderr, "\n");
            }

            process_list->next = current->next;
            current->next = NULL;

            last_ready->next = current;
            last_ready = current;

            current = process_list->next;
        }

        // start all ready processes and store the time
        current = ready_list->next;
        while (current)
        {
            if (current->t0.tv_sec == 0)
            {
                pthread_create(&(current->thread), NULL, work, current->data);
                clock_gettime(CLOCK_MONOTONIC, &(current->t0));
            }
            current = current->next;
        }

        nanosleep(&sleep_t, &rem);

        // remove finished processes from ready list and write output to file
        previous = ready_list;
        current = ready_list->next;
        while (current)
        {
            if (pthread_tryjoin_np(current->thread, NULL) == 0)
            {
                clock_gettime(CLOCK_MONOTONIC, &t);
                tr = t.tv_sec - current->t0.tv_sec;
                tf = t.tv_sec - initial_t.tv_sec;

                if (debug)
                {
                    fprintf(stderr, "process ended: ");
                    fprintf(stderr, "%s %d %d\n", current->data->name, tf, tr);
                }

                fprintf(output_file, "%s %d %d\n", current->data->name, tf, tr);

                previous->next = current->next;
                free(current->data);
                free(current);
                current = previous;
            }
            current = current->next;
        }

        current = process_list->next;
        clock_gettime(CLOCK_MONOTONIC, &t);
    }

    free(process_list);
}

int main(int argc, char **argv)
{
    int scheduler;
    FILE *trace, *output;
    char *output_file;
    Process *process_list;

    if (argc < 4)
    {
        printf("Usage: %s <scheduler_id> <trace_file> <output_file>\n",
               argv[0]);
        return -1;
    }

    if (argc == 5 && argv[4][0] == 'd')
    {
        debug = 1;
        printf("debug mode active\n");
    }

    scheduler = atoi(argv[1]);

    if ((trace = fopen(argv[2], "r")) == 0)
    {
        printf("error: failed to open trace file.\nExiting ...\n");
        return -1;
    }
    process_list = get_processes(trace);
    fclose(trace);

    output_file = argv[3];

    output = fopen(output_file, "w");

    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }
    else
    {
        printf("mutex init successful\n");
    }

    switch (scheduler)
    {
    case 1:
        fcfs_scheduler(process_list, output);
        break;

    case 2:
        srtn_scheduler(process_list, output);
        break;

    default:
        fprintf(stderr, "error: invalid scheduler value\n");
    }
    process_list = NULL;

    if (debug)
    {
        printf("Numero de mudancas de contexto = %d\n", contextChanges);
        fprintf(output, "Numero de mudancas de contexto = %d\n", contextChanges);
    }

    fclose(output);

    return 0;
}
