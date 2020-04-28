#include <string.h>
#include <malloc.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "ipc.h"
#include "common.h"
#include "pa1.h"
#include "log.h"

size_t numberOfPipes = 0;
local_id currentId = 0;
process_t processes[16];

int main(int argc, char *argv[]) {

    if (argc < 2 || !strncmp(argv[0], "-p", 2)) {
        printf("Incorrect input format! Type '-p X'");
        return -1;
    }

    numberOfPipes = (size_t)strtol(argv[2], NULL, 10);

    if (numberOfPipes > MAX_PROCESS_ID) {
        printf("Too many processes requested!");
        return -1;
    }

    pipesLog = fopen(pipes_log, "w");
    eventsLog = fopen(events_log, "w");



        for (size_t src = 0; src <= numberOfPipes; src++) {
            for (size_t dst = 0; dst <= numberOfPipes; dst++) {
                if (src != dst) {
                    int p[2];
                    pipe(p);
                    processes[src].readPipes[dst] = p[0];
                    processes[src].writePipes[dst] = p[1];

                    logPipeReadStarted((int)src, (int)dst, p[0]);
                    logPipeWriteStarted((int)src, (int)dst, p[1]);
                    //log pipe opened
                }
            }
        }

        processes[0].id = PARENT_ID;
        processes[0].pid = getpid();


    for (local_id i = 1; i <= numberOfPipes; i++) {
        int child = fork();
        if (child > 0) {
            //parent
            currentId= PARENT_ID;
            processes[i].id = i;
            processes[i].pid = child;
            processes[i].parentPid = processes[0].pid;
         //   printf(log_started_fmt, i, child, processes[0].pid);
        } else {
            if (child == 0) {
                //in child
                currentId = (local_id) i;
                processes[i].id = i;
                processes[i].pid = getpid();
                processes[i].parentPid = processes[0].pid;
                break;
            } else {
                perror("Error during fork processes!");
                return 1;
            }
        }
    }

    for (size_t src = 0; src <= numberOfPipes; src++) {
        for (size_t dst = 0; dst <= numberOfPipes; dst++) {
            if (src != currentId && dst != currentId && src != dst) {
                close(processes[src].readPipes[dst]);
                close(processes[src].writePipes[dst]);
            }
            if (src == currentId && dst != currentId) {
                close(processes[src].readPipes[dst]);
            }
            if ( dst == currentId && src != currentId) {
                close(processes[src].writePipes[dst]);
            }

        }
    }

    if (currentId != PARENT_ID) {
        MessageHeader messageHeader;
        messageHeader.s_magic = MESSAGE_MAGIC;
        messageHeader.s_type = STARTED;
        Message message;
        message.s_header = messageHeader;

        sprintf(message.s_payload, log_started_fmt, currentId, getpid(), getppid());
        message.s_header.s_payload_len = (uint16_t) strlen(message.s_payload);
        send_multicast(&processes[currentId], &message);

        printProcessStarted(processes[currentId]);
        //log started
    }

    Message msg;
    receive_any(&processes[currentId], &msg);

    printReceiveAllStareted(currentId);

    if (currentId != PARENT_ID) {
        MessageHeader messageHeader;
        messageHeader.s_magic = MESSAGE_MAGIC;
        messageHeader.s_type = DONE;
        Message message;
        message.s_header = messageHeader;

        sprintf(message.s_payload, log_done_fmt, currentId);
        message.s_header.s_payload_len = (uint16_t) strlen(message.s_payload);
        send_multicast(&processes[currentId], &message);

        printProcessDone(currentId);
        //printf(log_done_fmt, currentId);
        //log done
    }

    Message msg2;
    receive_any(&processes[currentId], &msg2);

    printReceiveAllDone(currentId);

    if (currentId == PARENT_ID) {
        for (size_t i = 1; i <= numberOfPipes; i++) {
            waitpid(processes[i].pid, NULL, 0);
        }
    }

    fclose(eventsLog);
    fclose(pipesLog);

}


int send(void * self, local_id dst, const Message * msg) {
    if (msg->s_header.s_magic != MESSAGE_MAGIC) {
       // printf("Incorrect header");
        return -2;
    }

    write(((process_t*)self)->writePipes[dst], &msg->s_header, sizeof(MessageHeader));
    write(((process_t*)self)->writePipes[dst], &msg->s_payload, msg->s_header.s_payload_len);


    return 0;
}


int send_multicast(void * self, const Message * msg) {

    for (local_id i = 0; i <= numberOfPipes; i++) {
        if (((process_t*)self)->id != i) {
            send(self, i, msg);
        }
    }

    return 0;
}


int receive(void * self, local_id from, Message * msg) {
    size_t left = sizeof(MessageHeader);
    void* buffer = &msg->s_header;

    while (left > 0) {
        int readBytes = (int) read(processes[from].readPipes[((process_t*)self)->id], buffer, left);
        if (readBytes > 0) {
            buffer = (char*)buffer + readBytes;
            left -= readBytes;
        } else {
            return -1;
        }
    }

    if (msg->s_header.s_magic != MESSAGE_MAGIC) {
        //log invalid message magic or return smth
        return -2;
    }

    buffer = &msg->s_payload;
    left = msg->s_header.s_payload_len;

    while (left > 0) {
        int readBytes = (int) read(processes[from].readPipes[((process_t*)self)->id], buffer, left);
        if (readBytes > 0) {
            left -= readBytes;
            buffer = (char*)buffer + readBytes;
        } else {
            return -1;
        }
    }

    return 0;
}


int receive_any(void * self, Message * msg) {

    for (size_t i = 1; i <= numberOfPipes; i++) {
        if (i != ((process_t*)self)->id) {
            receive(self, (local_id) i, msg);
        }
    }

    return 0;
}
