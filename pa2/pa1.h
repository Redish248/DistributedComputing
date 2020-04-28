#ifndef __IFMO_DISTRIBUTED_CLASS_PA1
#define __IFMO_DISTRIBUTED_CLASS_PA1

#include "banking.h"

typedef enum {
    FISRT_PHASE = 0,     ///< start
    SECOND_PHASE,            ///< transfer
    THIRD_PHASE,             ///< done
    BALANCE_HISTORY_PHASE   /// balance history
} Phase;

typedef struct process_t {
    pid_t pid;
    local_id id;
    pid_t parentPid;
    int writePipes[16];
    int readPipes[16];
} process_t;

int readParams(int argc, char *argv[]);
void openPipes();
int forkProcesses();
void closeExtraPipes();
void sendStart();
void sendDone();
void sendStopAll();
void receiveTransfer(Message* msg);
void setBalance(timestamp_t time, balance_t balance);
void changeBalance(timestamp_t time, balance_t amount);
void sendBalanceHistory();
void receiveHistory(Message* msg);
void prepareHistory();

#endif // __IFMO_DISTRIBUTED_CLASS_PA1
