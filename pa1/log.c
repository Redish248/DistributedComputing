#include <stdio.h>
#include <fcntl.h>
#include "log.h"
#include "pa1.h"

void printReceiveAllStareted(local_id id) {
    fprintf(eventsLog, log_received_all_started_fmt, id);
    printf(log_received_all_started_fmt, id);
}

void printReceiveAllDone(local_id id) {
    fprintf(eventsLog, log_received_all_done_fmt, id);
    printf(log_received_all_done_fmt, id);
}


void printProcessStarted(process_t process) {
    fprintf(eventsLog, log_started_fmt, process.id, process.pid, process.parentPid);
    printf(log_started_fmt, process.id, process.pid, process.parentPid);
}

void printProcessDone(local_id id) {
    fprintf(eventsLog, log_done_fmt, id);
    printf(log_done_fmt, id);
}

void logPipeReadStarted(int source, int dest, int id) {
    fprintf(pipesLog, log_pipe_read_opened_fmt, source, dest, id);
    printf(log_pipe_write_opened_fmt, source, dest, id);
}

void logPipeWriteStarted(int source, int dest, int id) {
    fprintf(pipesLog, log_pipe_write_opened_fmt, source, dest, id);
    printf(log_pipe_write_opened_fmt, source, dest, id);
}

