#include <string.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>

#include "ipc.h"
#include "common.h"
#include "pa1.h"
#include "log.h"
#include "pa2345.h"
#include "banking.h"

size_t numberOfPipes = 0;
local_id currentId = 0;
process_t processes[16];
int8_t startBalances[16];

int8_t messagesStartLeft, messagesDoneLeft, balanceHistoryLeft;
bool stopReceive = false;
int8_t currentPhase = FISRT_PHASE;

AllHistory allHistory;
BalanceHistory balanceHistory;
timestamp_t lastTimeMoment;


int main(int argc, char *argv[]) {

    if (readParams(argc, argv)) return -1;
    pipesLog = fopen(pipes_log, "w");
    eventsLog = fopen(events_log, "w");

    openPipes();
    processes[0].id = PARENT_ID;
    processes[0].pid = getpid();
    processes[0].lamport_time = 0;

    if (forkProcesses()) return 1;
    closeExtraPipes();

    messagesStartLeft = messagesDoneLeft = (currentId == PARENT_ID) ? numberOfPipes : numberOfPipes - 1;
    balanceHistoryLeft = (currentId == PARENT_ID) ? numberOfPipes : 0;

    sendStart();
    Message msg;
    receive_any(&processes[currentId], &msg);
    printReceiveAllStarted(get_lamport_time(), currentId);

    currentPhase = SECOND_PHASE;
    if (currentId == PARENT_ID) {
        //processes[currentId].lamport_time++;
        bank_robbery(&msg, numberOfPipes);
    } else {
        receive_any(&processes[currentId], &msg);
    }

    if (currentId == PARENT_ID) {
        sendStopAll();
    }

    currentPhase = THIRD_PHASE;
    sendDone();
    receive_any(&processes[currentId], &msg);
    printReceiveAllDone(get_lamport_time(), currentId);

    currentPhase = BALANCE_HISTORY_PHASE;
    sendBalanceHistory();
    if (currentId == PARENT_ID) {
        receive_any(&processes[currentId], &msg);
        allHistory.s_history_len = numberOfPipes;
    }

    if (currentId == PARENT_ID) {
        for (size_t i = 1; i <= numberOfPipes; i++) {
            waitpid(processes[i].pid, NULL, 0);
        }

        prepareHistory();
        print_history(&allHistory);
    }

    fclose(eventsLog);
    fclose(pipesLog);
}

//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------

timestamp_t get_lamport_time() {
    return processes[currentId].lamport_time;
}

void chooseLamportTime(process_t* process, timestamp_t msg_time) {
    if (msg_time > process->lamport_time) {
        process->lamport_time = msg_time;
    }
    process->lamport_time++;
}


//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------

int send(void *self, local_id dst, const Message *msg) {
    int flags = fcntl(((process_t *) self)->writePipes[dst], F_GETFL, 0);
    fcntl(((process_t *) self)->writePipes[dst], F_SETFL, flags | O_NONBLOCK);

    if (msg->s_header.s_magic != MESSAGE_MAGIC) {
        return -2;
    }

    write(((process_t *) self)->writePipes[dst], msg, sizeof(MessageHeader) + msg->s_header.s_payload_len);

    return 0;
}


int send_multicast(void *self, const Message *msg) {
    for (local_id i = 0; i <= numberOfPipes; i++) {
        if (((process_t *) self)->id != i) {
            send(self, i, msg);
        }
    }
    return 0;
}


int receive(void *self, local_id from, Message *msg) {
    void* buffer = &msg->s_header;
    int readBytes = (int) read(processes[from].readPipes[((process_t *) self)->id], buffer, sizeof(MessageHeader));
    if (readBytes <= 0) {
        return -1;
    }

    if (msg->s_header.s_magic != MESSAGE_MAGIC) {
        //log invalid message magic or return smth
        return -2;
    }

    buffer = &msg->s_payload;

    readBytes = (int) read(processes[from].readPipes[((process_t *) self)->id], buffer, msg->s_header.s_payload_len);
    if (readBytes <= 0 && msg->s_header.s_payload_len!=0) {
        return -1;
    }

    chooseLamportTime((process_t *) self, msg->s_header.s_local_time);

    switch (msg->s_header.s_type) {
        case STARTED:
            messagesStartLeft--;
            break;
        case DONE:
            messagesDoneLeft--;
            break;
        case TRANSFER:
            ((process_t *) self)->lamport_time++;
            receiveTransfer(msg);
            break;
        case STOP:
            stopReceive = true;
            break;
        case BALANCE_HISTORY:
            balanceHistoryLeft--;
            receiveHistory(msg);
            break;

    }


    return 0;
}


int receive_any(void *self, Message *msg) {
    while ((currentPhase == SECOND_PHASE && !stopReceive) ||
           (currentPhase == FISRT_PHASE && messagesStartLeft) ||
           (currentPhase == THIRD_PHASE && messagesDoneLeft) ||
           (currentPhase == BALANCE_HISTORY_PHASE && balanceHistoryLeft)) {
        for (size_t i = 0; i <= numberOfPipes; i++) {
            if (i != ((process_t *) self)->id) {
                receive(self, (local_id) i, msg);
            }
            if (stopReceive && currentPhase == SECOND_PHASE) break;
        }
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------



void transfer(void *parent_data, local_id src, local_id dst,
              balance_t amount) {

    processes[src].lamport_time++;

    bool gotResponse = false;
    MessageHeader messageHeader;
    messageHeader.s_magic = MESSAGE_MAGIC;
    messageHeader.s_type = TRANSFER;
    messageHeader.s_local_time = processes[src].lamport_time++;
    messageHeader.s_local_time = get_lamport_time();
    Message message;
    message.s_header = messageHeader;

    TransferOrder transferOrder;
    transferOrder.s_src = src;
    transferOrder.s_dst = dst;
    transferOrder.s_amount = amount;

    memcpy(message.s_payload, (char*) &transferOrder, sizeof(TransferOrder));
    message.s_header.s_payload_len = (uint16_t) strlen(message.s_payload);
    send(&processes[currentId], src, &message);

    Message msg;
    while (!gotResponse) {
        if (!receive(&processes[currentId], dst, &msg) && msg.s_header.s_type == ACK) {
            gotResponse = true;
        }
    }

}

void receiveTransfer(Message* msg) {
    TransferOrder *transferOrder = (TransferOrder *) &msg->s_payload;
    transferOrder->s_amount = (int8_t) transferOrder->s_amount;
    if (transferOrder->s_src == currentId) {
        //processes[currentId].balance -= transferOrder->s_amount;
        changeBalance(get_lamport_time(), -transferOrder->s_amount);
        processes[currentId].lamport_time++;
        msg->s_header.s_local_time = get_lamport_time();
        send(&processes[currentId], transferOrder->s_dst, msg);
        logTransferOut(lastTimeMoment, transferOrder->s_src, transferOrder->s_dst, (balance_t) transferOrder->s_amount);
    } else {
        if (transferOrder->s_dst == currentId) {

            //processes[currentId].balance += transferOrder->s_amount;
            changeBalance(get_lamport_time(), transferOrder->s_amount);

            MessageHeader messageHeader;
            messageHeader.s_magic = MESSAGE_MAGIC;
            messageHeader.s_type = ACK;
            messageHeader.s_payload_len = 0;
            processes[currentId].lamport_time++;
            messageHeader.s_local_time = get_lamport_time();
            Message message;
            message.s_header = messageHeader;

            send(&processes[currentId], PARENT_ID, &message);
            logTransferIn(lastTimeMoment, transferOrder->s_src, transferOrder->s_dst, transferOrder->s_amount);
        }
    }
}

int readParams(int argc, char *argv[]) {
    if (argc < 2 || !strncmp(argv[0], "-p", 2)) {
        printf("Incorrect input format!");
        return -1;
    }

    numberOfPipes = (size_t) strtol(argv[2], NULL, 10);
    if (argc != (numberOfPipes + 3)) {
        printf("Incorrect input format!");
        return -1;
    }

    if (numberOfPipes > MAX_PROCESS_ID) {
        printf("Too many processes requested!");
        return -1;
    }

    for (int8_t i = 3; i < argc; i++) {
        startBalances[i - 2] = strtol(argv[i], NULL, 10);
    }
    return 0;
}

void openPipes() {
    int flags;
    for (size_t src = 0; src <= numberOfPipes; src++) {
        for (size_t dst = 0; dst <= numberOfPipes; dst++) {
            if (src != dst) {
                int p[2];
                pipe(p);
                processes[src].readPipes[dst] = p[0];
                processes[src].writePipes[dst] = p[1];
                flags = fcntl(processes[src].readPipes[dst], F_GETFL, 0);
                fcntl(processes[src].readPipes[dst], F_SETFL, flags | O_NONBLOCK);
                flags = fcntl(processes[src].writePipes[dst], F_GETFL, 0);
                fcntl(processes[src].writePipes[dst], F_SETFL, flags | O_NONBLOCK);

                logPipeReadStarted((int) src, (int) dst, p[0]);
                logPipeWriteStarted((int) src, (int) dst, p[1]);
                //log pipe opened
            }
        }
    }
}

int forkProcesses() {
    for (local_id i = 1; i <= numberOfPipes; i++) {
        int child = fork();
        if (child > 0) {
            //parent
            currentId = PARENT_ID;
            processes[i].id = i;
            processes[i].pid = child;
            processes[i].parentPid = processes[0].pid;
          //  processes[i].lamport_time = 0;
            //   printf(log_started_fmt, i, child, processes[0].pid);
        } else {
            if (child == 0) {
                //in child
                currentId = (local_id) i;
                processes[i].id = i;
                processes[i].pid = getpid();
                processes[i].parentPid = processes[0].pid;
            //    processes[i].lamport_time = 0;
                balanceHistory.s_id = currentId;
                setBalance(get_lamport_time(), startBalances[i], 0);
                break;
            } else {
                perror("Error during fork processes!");
                return 1;
            }
        }
    }
    return 0;
}

void closeExtraPipes() {
    for (size_t src = 0; src <= numberOfPipes; src++) {
        for (size_t dst = 0; dst <= numberOfPipes; dst++) {
            if (src != currentId && dst != currentId && src != dst) {
                close(processes[src].readPipes[dst]);
                close(processes[src].writePipes[dst]);
            }
            if (src == currentId && dst != currentId) {
                close(processes[src].readPipes[dst]);
            }
            if (dst == currentId && src != currentId) {
                close(processes[src].writePipes[dst]);
            }

        }
    }
}

void sendStart() {
    if (currentId != PARENT_ID) {
        MessageHeader messageHeader;
        messageHeader.s_magic = MESSAGE_MAGIC;
        messageHeader.s_type = STARTED;
        processes[currentId].lamport_time++;
        messageHeader.s_local_time = get_lamport_time();
        Message message;
        message.s_header = messageHeader;

        sprintf(message.s_payload, log_started_fmt, lastTimeMoment, currentId, getpid(), getppid(), balanceHistory.s_history[lastTimeMoment].s_balance);
        message.s_header.s_payload_len = (uint16_t) strlen(message.s_payload);
        send_multicast(&processes[currentId], &message);

        printProcessStarted(lastTimeMoment, processes[currentId], balanceHistory.s_history[lastTimeMoment].s_balance);
        //log started
    }
}

void sendDone() {
    if (currentId != PARENT_ID) {
        MessageHeader messageHeader;
        messageHeader.s_magic = MESSAGE_MAGIC;
        messageHeader.s_type = DONE;
        processes[currentId].lamport_time++;
        messageHeader.s_local_time = get_lamport_time();
        Message message;
        message.s_header = messageHeader;

        sprintf(message.s_payload, log_done_fmt, lastTimeMoment, currentId, balanceHistory.s_history[lastTimeMoment].s_balance);
        message.s_header.s_payload_len = (uint16_t) strlen(message.s_payload);
        send_multicast(&processes[currentId], &message);

        printProcessDone(lastTimeMoment, currentId, balanceHistory.s_history[lastTimeMoment].s_balance);
        //printf(log_done_fmt, currentId);
        //log done
    }
}

void sendStopAll() {
    MessageHeader messageHeader;
    messageHeader.s_magic = MESSAGE_MAGIC;
    messageHeader.s_type = STOP;
    messageHeader.s_payload_len = 0;
    processes[currentId].lamport_time++;
    messageHeader.s_local_time = get_lamport_time();
    Message message;
    message.s_header = messageHeader;

    send_multicast(&processes[currentId], &message);
}

void setBalance(timestamp_t  time, balance_t balance, balance_t amount) {
    lastTimeMoment = time;
    balanceHistory.s_history_len = time + 1;
    balanceHistory.s_history[time] = (BalanceState) {
            .s_balance =  balance,
            .s_time =  time,
            .s_balance_pending_in =  amount < 0 ? -amount : 0
    };
}

void changeBalance(timestamp_t time, balance_t amount) {
    for (timestamp_t t = lastTimeMoment + 1; t < time; t++) {
        setBalance(t, balanceHistory.s_history[lastTimeMoment].s_balance, amount);
    }
    setBalance(time, balanceHistory.s_history[lastTimeMoment].s_balance + amount, amount);
}

void sendBalanceHistory() {
    if (currentId != PARENT_ID) {
        size_t payload_len = sizeof(local_id) + sizeof(uint8_t) + sizeof(BalanceState) * balanceHistory.s_history_len;
        MessageHeader messageHeader;
        messageHeader.s_magic = MESSAGE_MAGIC;
        messageHeader.s_type = BALANCE_HISTORY;
        processes[currentId].lamport_time++;
        messageHeader.s_local_time = get_lamport_time();
        Message message;
        memcpy(message.s_payload, (char *) &balanceHistory, payload_len);
        messageHeader.s_payload_len = payload_len;
        message.s_header = messageHeader;

        send(&processes[currentId], PARENT_ID, &message);
    }
}

void receiveHistory(Message* msg) {
    BalanceHistory* history = (BalanceHistory*) &msg->s_payload;
    allHistory.s_history[history->s_id - 1] = *history;
}

void prepareHistory() {
    timestamp_t t = 0;
    for (uint8_t p = 0; p < allHistory.s_history_len; p++) {
        if (allHistory.s_history[p].s_history_len > t)
            t = allHistory.s_history[p].s_history_len;
    }
    for (uint8_t p = 0; p < allHistory.s_history_len; p++) {
        if (allHistory.s_history[p].s_history_len < t) {
            for (uint8_t time = allHistory.s_history[p].s_history_len; time < t; time++) {
                allHistory.s_history[p].s_history[time] = (BalanceState) {
                        .s_balance =  allHistory.s_history[p].s_history[allHistory.s_history[p].s_history_len-1].s_balance,
                        .s_time =  time,
                        .s_balance_pending_in =  allHistory.s_history[p].s_history[allHistory.s_history[p].s_history_len-1].s_balance_pending_in
                };
            }
            allHistory.s_history[p].s_history_len = t;
        }
    }
}
