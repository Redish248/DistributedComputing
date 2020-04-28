#include "ipc.h"
#include "defs.h"

FILE* eventsLog;
FILE* pipesLog;

static const char * const log_pipe_read_opened_fmt =
        "Pipe from process %1d to process %1d opened for read with id %1d.\n";

static const char * const log_pipe_write_opened_fmt =
        "Pipe from process %1d to process %1d opened for write with id %1d.\n";

static const char * const log_pipe_read_closed_fmt =
        "Pipe from process %1d to process %1d closed for read.\n";

static const char * const log_pipe_write_closed_fmt =
        "Pipe from process %1d to process %1d closed for write.\n";

/**
 * Print log about receiving all started messages
 * @param id process id
 */
void printReceiveAllStareted(local_id id);

/**
 * Print log about receiving all done messages
 * @param id process id
 */
void printReceiveAllDone(local_id id);

/**
 * Pring log about starting process
 * @param process all process
 */
void printProcessStarted(process_t process);

/**
 * Print log when process finish it's work
 * @param id process id
 */
void printProcessDone(local_id id);

/**
 * Print log when pipe for read is opened
 * @param source source
 * @param dest destination
 * @param id pipe's id
 */
void logPipeReadStarted(int source, int dest, int id);


/**
 * Print log when pipe for write is opened
 * @param source source
 * @param dest destination
 * @param id pipe's id
 */
void logPipeWriteStarted(int source, int dest, int id);
