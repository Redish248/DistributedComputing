#ifndef __IFMO_DISTRIBUTED_CLASS_PA1
#define __IFMO_DISTRIBUTED_CLASS_PA1

#include "banking.h"

typedef enum {
    FISRT_PHASE = 0,     ///< start
    SECOND_PHASE,            ///< loop
    THIRD_PHASE,             ///< done
} Phase;

typedef struct process_t {
    pid_t pid;
    local_id id;
    pid_t parentPid;
    timestamp_t lamport_time;
    int writePipes[16];
    int readPipes[16];
} process_t;

int readParams(int argc, char *argv[]);
void openPipes();
int forkProcesses();
void closeExtraPipes();
void sendStart();
void sendDone();
void loopProcessing();
void sendRequestAll();
void sendReleaseAll();
void sendReply(local_id dst);

#endif // __IFMO_DISTRIBUTED_CLASS_PA1
