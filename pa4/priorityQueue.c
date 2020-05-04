#include <stdio.h>
#include <stdlib.h>
#include "priorityQueue.h"

Node* newNode(Request r) {
    Node* temp = (Node*)malloc(sizeof(Node));
    temp->req = r;
    temp->next = NULL;

    return temp;
}

Request peek(Node** head) {
    return (*head)->req;
}

void pop(Node** head) {
    Node* temp = *head;
    (*head) = (*head)->next;
    free(temp);
}

void push(Node** head, Request req) {
    Node* start = (*head);

    Node* temp = newNode(req);

    if ((*head)->req.time > req.time || (((*head)->req.time == req.time) && ((*head)->req.pid > req.pid))) {
        temp->next = *head;
        (*head) = temp;
    }
    else {
        while (start->next != NULL && (start->next->req.time < req.time
                || ((start->next->req.time == req.time) && (start->next->req.pid < req.pid)))) {
            start = start->next;
        }

        temp->next = start->next;
        start->next = temp;
    }
}

int isEmpty(Node** head) {
    return (*head) == NULL;
}
