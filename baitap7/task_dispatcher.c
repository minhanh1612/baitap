#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>  // O_WRONLY

#define QUEUE_NAME "/my_task_queue"
#define MAX_MSG_SIZE 256

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <priority> \"<task_description>\"\n", argv[0]);
        exit(1);
    }

    unsigned int priority = atoi(argv[1]);
    char *task = argv[2];

    mqd_t mq = mq_open(QUEUE_NAME, O_WRONLY);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }

    if (mq_send(mq, task, strlen(task), priority) == -1) {
        perror("mq_send");
        mq_close(mq);
        exit(1);
    }

    printf("Task sent successfully! (Priority: %u): %s\n", priority, task);

    mq_close(mq);
    return 0;
}
