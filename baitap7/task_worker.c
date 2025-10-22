#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>      // O_CREAT, O_RDONLY
#include <unistd.h>     // sleep()
#include <signal.h>

#define QUEUE_NAME "/my_task_queue"
#define MAX_MSG_SIZE 256
#define MAX_MESSAGES 10

mqd_t mq; // message queue descriptor

void cleanup(int signum) {
    printf("\nCleaning up and exiting...\n");
    mq_close(mq);
    mq_unlink(QUEUE_NAME);
    exit(0);
}

int main() {
    struct mq_attr attr;
    char buffer[MAX_MSG_SIZE];
    unsigned int prio;

    // Cấu hình thuộc tính hàng đợi
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    // Tạo hoặc mở hàng đợi (chế độ đọc)
    mq = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0644, &attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }

    // Bắt tín hiệu Ctrl+C để dọn dẹp
    signal(SIGINT, cleanup);

    printf("Worker started. Waiting for tasks...\n");

    // Vòng lặp nhận và xử lý tác vụ
    while (1) {
        ssize_t bytes_read = mq_receive(mq, buffer, MAX_MSG_SIZE, &prio);
        if (bytes_read >= 0) {
            buffer[bytes_read] = '\0';
            printf("Processing task (Priority: %u): %s\n", prio, buffer);
            sleep(1);
        } else {
            perror("mq_receive");
        }
    }

    // Không bao giờ đến đây, nhưng thêm cho chắc
    mq_close(mq);
    mq_unlink(QUEUE_NAME);
    return 0;
}
