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
#include "priorityQueue.h"

size_t numberOfPipes = 0;
local_id currentId = 0;
process_t processes[16];
bool mutexl = false;

int8_t messagesStartLeft, messagesDoneLeft, csRepliesLeft, csReplies;
int8_t currentPhase = FISRT_PHASE;
Node* requestQueue;

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
    csReplies = numberOfPipes - 1;

    sendStart();
    Message msg;
    receive_any(&processes[currentId], &msg);
    printReceiveAllStarted(get_lamport_time(), currentId);

    currentPhase = SECOND_PHASE;
    if (currentId != PARENT_ID) {
        loopProcessing();
    }

    currentPhase = THIRD_PHASE;
    sendDone();
    receive_any(&processes[currentId], &msg);
    printReceiveAllDone(get_lamport_time(), currentId);


    if (currentId == PARENT_ID) {
        for (size_t i = 1; i <= numberOfPipes; i++) {
            waitpid(processes[i].pid, NULL, 0);
        }
    }

    fclose(eventsLog);
    fclose(pipesLog);
}

void addElementToQueue(Request req) {
    if (isEmpty(&requestQueue)) {
        requestQueue = newNode(req);
    } else {
        push(&requestQueue, req);
    }
}

int request_cs(const void * self) {
    Message msg;
    processes[currentId].lamport_time++;
    addElementToQueue((Request) {.time =  get_lamport_time(), .pid =  ((process_t *) self)->id});
    sendRequestAll();
    csRepliesLeft = csReplies;
    receive_any(&processes[currentId], &msg);
    return 0;
}

int release_cs(const void * self) {
    if (peek(&requestQueue).pid == ((process_t *) self)->id) {
        pop(&requestQueue);
        sendReleaseAll();
        return 0;
    } else {
        printf("Oops, something wrong\n");
        return 1;
    }
}

void loopProcessing() {
    char log_str[64];
    int iterationsAmount = 5 * currentId;
    for (int i = 1; i <= iterationsAmount; i++) {
        memset(log_str, 0, sizeof(log_str));
        if (mutexl) {
            request_cs(&processes[currentId]);
            sprintf(log_str, log_loop_operation_fmt, currentId, i, iterationsAmount);
            print(log_str);
            release_cs(&processes[currentId]);
        } else {
            sprintf(log_str, log_loop_operation_fmt, currentId, i, iterationsAmount);
            print(log_str);
        }
    }
}

void sendRequestAll() {
    MessageHeader messageHeader;
    messageHeader.s_magic = MESSAGE_MAGIC;
    messageHeader.s_type = CS_REQUEST;
    messageHeader.s_local_time = get_lamport_time();
    messageHeader.s_payload_len = 0;
    Message message;
    message.s_header = messageHeader;

    send_multicast(&processes[currentId], &message);

}

void sendReleaseAll() {
    MessageHeader messageHeader;
    messageHeader.s_magic = MESSAGE_MAGIC;
    messageHeader.s_type = CS_RELEASE;
    processes[currentId].lamport_time++;
    messageHeader.s_local_time = get_lamport_time();
    messageHeader.s_payload_len = 0;
    Message message;
    message.s_header = messageHeader;

    send_multicast(&processes[currentId], &message);

}

void sendReply(local_id dst) {
    MessageHeader messageHeader;
    messageHeader.s_magic = MESSAGE_MAGIC;
    messageHeader.s_type = CS_REPLY;
    processes[currentId].lamport_time++;
    messageHeader.s_local_time = get_lamport_time();
    messageHeader.s_payload_len = 0;
    Message message;
    message.s_header = messageHeader;

    send(&processes[currentId], dst, &message);

}

//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------

timestamp_t get_lamport_time() {
    return processes[currentId].lamport_time;
}

void chooseLamportTime(process_t *process, timestamp_t msg_time) {
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
    void *buffer = &msg->s_header;
    int readBytes = (int) read(processes[from].readPipes[((process_t *) self)->id], buffer, sizeof(MessageHeader));
    if (readBytes <= 0) {
        return -1;
    }

    if (msg->s_header.s_magic != MESSAGE_MAGIC) {
        return -2;
    }

    buffer = &msg->s_payload;

    readBytes = (int) read(processes[from].readPipes[((process_t *) self)->id], buffer, msg->s_header.s_payload_len);
    if (readBytes <= 0 && msg->s_header.s_payload_len != 0) {
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
        case CS_REQUEST:
            if (((process_t *) self)->id != PARENT_ID) {
                addElementToQueue((Request) {.pid =  from, .time =  msg->s_header.s_local_time});
                sendReply(from);
            }
            break;
        case CS_REPLY:
            csRepliesLeft--;
            break;
        case CS_RELEASE:
            if (((process_t *) self)->id != PARENT_ID) {
                if (peek(&requestQueue).pid == from) {
                    pop(&requestQueue);
                } else {
                    printf("Oops, something wrong\n");
                }
            }
            break;
    }


    return 0;
}


int receive_any(void *self, Message *msg) {
    while ((currentPhase == SECOND_PHASE && (csRepliesLeft || peek(&requestQueue).pid != ((process_t *) self)->id)) ||
           (currentPhase == FISRT_PHASE && messagesStartLeft) ||
           (currentPhase == THIRD_PHASE && messagesDoneLeft)) {
        for (size_t i = 0; i <= numberOfPipes; i++) {
            if (i != ((process_t *) self)->id) {
                receive(self, (local_id) i, msg);
            }
        }
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------



int readParams(int argc, char *argv[]) {
    if (argc == 3 && !strncmp(argv[1], "-p", 2)) {
        numberOfPipes = (size_t) strtol(argv[2], NULL, 10);
    } else {
        if (argc == 4) {
            if (!strncmp(argv[1], "-p", 2) && !strncmp(argv[3], "--mutexl", 8)) {
                numberOfPipes = (size_t) strtol(argv[2], NULL, 10);
                mutexl = true;
            } else {
                if (!strncmp(argv[1], "--mutexl", 8) && !strncmp(argv[2], "-p", 2)) {
                    numberOfPipes = (size_t) strtol(argv[3], NULL, 10);
                    mutexl = true;
                } else {
                    printf("Incorrect input format!");
                    return -1;
                }
            }
        }
    }

    if (numberOfPipes > MAX_PROCESS_ID) {
        printf("Too many processes requested!");
        return -1;
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
            processes[i].lamport_time = 0;
        } else {
            if (child == 0) {
                //in child
                currentId = (local_id) i;
                processes[i].id = i;
                processes[i].pid = getpid();
                processes[i].parentPid = processes[0].pid;
                processes[i].lamport_time = 0;
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

        sprintf(message.s_payload, log_started_fmt, get_lamport_time(), currentId, getpid(), getppid(), 0);
        message.s_header.s_payload_len = (uint16_t) strlen(message.s_payload);
        send_multicast(&processes[currentId], &message);

        //printProcessStarted(get_lamport_time(), processes[currentId], 0);
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

        sprintf(message.s_payload, log_done_fmt, get_lamport_time(), currentId, 0);
        message.s_header.s_payload_len = (uint16_t) strlen(message.s_payload);
        send_multicast(&processes[currentId], &message);

        //printProcessDone(get_lamport_time(), currentId, 0);
        //log done
    }
}
