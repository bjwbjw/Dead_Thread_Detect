#include "mutex_lock.h"
#include <iostream>
#include <unistd.h>

Mutex mutex0;
Mutex mutex1;

void *ThrdFunc0(void *) {
    while(1){
        mutex0.lock();
        printf("thread 0\n");
        sleep(1);
        mutex1.lock();

        mutex0.unlock();
        mutex1.unlock();
    }
    pthread_exit(nullptr);
}

void *ThrdFunc1(void *) {
    while(1){
        mutex1.lock();
        printf("thread 1\n");
        sleep(1);
        mutex0.lock();

        mutex0.unlock();
        mutex1.unlock();
    }
    pthread_exit(nullptr);
}

int main() {
    printf("%d\n", getpid());

    pthread_t tid[2];
    if (pthread_create(&tid[0], nullptr, ThrdFunc0, nullptr) == -1) {
        perror("pthread_create");
        exit(1);
    }

    if (pthread_create(&tid[1], nullptr, ThrdFunc1, nullptr) == -1) {
        perror("pthread_create");
        exit(1);
    }

    pthread_join(tid[0], nullptr);
    pthread_join(tid[1], nullptr);

    exit(0);
}
