#include <stdio.h>
#include <fcntl.h>
#include "log.h"
#include "pa2345.h"
#include "pa1.h"

void printReceiveAllStareted(timestamp_t timestamp, local_id id) {
    fprintf(eventsLog, log_received_all_started_fmt, timestamp, id);
    printf(log_received_all_started_fmt, timestamp, id);
}

void printReceiveAllDone(timestamp_t timestamp, local_id id) {
    fprintf(eventsLog, log_received_all_done_fmt, timestamp, id);
    printf(log_received_all_done_fmt, timestamp, id);
}


void printProcessStarted(timestamp_t timestamp, process_t process, balance_t balance) {
    fprintf(eventsLog, log_started_fmt, timestamp, process.id, process.pid, process.parentPid, balance);
    printf(log_started_fmt, timestamp, process.id, process.pid, process.parentPid, balance);
}

void printProcessDone(timestamp_t timestamp, local_id id, int balance) {
    fprintf(eventsLog, log_done_fmt, timestamp, id, balance);
    printf(log_done_fmt, timestamp, id, balance);
}

void logPipeReadStarted(int source, int dest, int id) {
    fprintf(pipesLog, log_pipe_read_opened_fmt, source, dest, id);
    printf(log_pipe_write_opened_fmt, source, dest, id);
}

void logPipeWriteStarted(int source, int dest, int id) {
    fprintf(pipesLog, log_pipe_write_opened_fmt, source, dest, id);
    printf(log_pipe_write_opened_fmt, source, dest, id);
}

void logTransferOut(timestamp_t timestamp, local_id src, local_id dst, balance_t amount){
    fprintf(pipesLog, log_transfer_out_fmt, timestamp, src, amount, dst);
    printf(log_transfer_out_fmt, timestamp, src, amount, dst);
}
void logTransferIn(timestamp_t timestamp, local_id src, local_id dst, balance_t amount){
    fprintf(pipesLog, log_transfer_in_fmt, timestamp, dst, amount, src);
    printf(log_transfer_in_fmt, timestamp, dst, amount, src);
}

