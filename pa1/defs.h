
typedef struct process_t {
    pid_t pid;
    local_id id;
    pid_t parentPid;
    int writePipes[16];
    int readPipes[16];
} process_t;
