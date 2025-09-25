#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void *thread_func1(void *arg) {
    printf("Thread với ID %lu đang chạy!\n", pthread_self());
    return NULL;
}

void bai1() {
    printf("\n Bai1 \n");
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_func1, NULL);
    pthread_create(&t2, NULL, thread_func1, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("Hai luồng đã kết thúc.\n");
}

long long counter = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *thread_func2(void *arg) {
    for (int i = 0; i < 1000000; i++) {
        pthread_mutex_lock(&mutex);
        counter++;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void bai2() {
    printf("\n bai2 \n");
    pthread_t t[3];
    counter = 0;

    for (int i = 0; i < 3; i++) {
        pthread_create(&t[i], NULL, thread_func2, NULL);
    }
    for (int i = 0; i < 3; i++) {
        pthread_join(t[i], NULL);
    }
    printf("Giá trị cuối cùng của counter = %lld\n", counter);
}

int data;
int data_ready = 0;
pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *producer(void *arg) {
    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&mutex3);
        data = rand() % 100;
        data_ready = 1;
        printf("Producer tạo data = %d\n", data);
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex3);
        sleep(1);
    }
    return NULL;
}

void *consumer(void *arg) {
    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&mutex3);
        while (!data_ready) {
            pthread_cond_wait(&cond, &mutex3);
        }
        printf("Consumer nhận data = %d\n", data);
        data_ready = 0;
        pthread_mutex_unlock(&mutex3);
    }
    return NULL;
}

void bai3() {
    printf("\n bai3 \n");
    pthread_t p, c;
    pthread_create(&p, NULL, producer, NULL);
    pthread_create(&c, NULL, consumer, NULL);

    pthread_join(p, NULL);
    pthread_join(c, NULL);
}

int shared_data = 0;
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

void *reader(void *arg) {
    int id = *((int*)arg);
    pthread_rwlock_rdlock(&rwlock);
    printf("Reader %d đọc shared_data = %d\n", id, shared_data);
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}

void *writer(void *arg) {
    int id = *((int*)arg);
    pthread_rwlock_wrlock(&rwlock);
    shared_data++;
    printf("Writer %d ghi shared_data = %d\n", id, shared_data);
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}

void bai4() {
    printf("\n bai4 \n");
    pthread_t r[5], w[2];
    int id_r[5], id_w[2];

    for (int i = 0; i < 5; i++) {
        id_r[i] = i+1;
        pthread_create(&r[i], NULL, reader, &id_r[i]);
    }
    for (int i = 0; i < 2; i++) {
        id_w[i] = i+1;
        pthread_create(&w[i], NULL, writer, &id_w[i]);
    }

    for (int i = 0; i < 5; i++) pthread_join(r[i], NULL);
    for (int i = 0; i < 2; i++) pthread_join(w[i], NULL);
}

int main() {
    srand(time(NULL));
    bai1();
    bai2();
    bai3();
    bai4();
    return 0;
}
