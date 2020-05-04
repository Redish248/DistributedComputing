#ifndef PA3_PRIORITYQUEUE_H
#define PA3_PRIORITYQUEUE_H

#include "ipc.h"

typedef struct {
    timestamp_t time;
    local_id pid;
} Request;

typedef struct node {
    Request req;
    struct node* next;
} Node;

Node* newNode(Request r);
Request peek(Node** head);
void pop(Node** head);
void push(Node** head, Request req);
int isEmpty(Node** head);


#endif //PA3_PRIORITYQUEUE_H
