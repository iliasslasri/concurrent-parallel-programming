#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


void * f (void * arg) {
    int id = *(int*) arg;
    printf("id = %d\n ", id);
}


int main (void) {
    pthread_t t[2];
    int id;
    int * arg;
    printf("main\n");
    for (id = 0; id < 2; id++){
        arg = malloc(sizeof(int));
        *arg = id;
        pthread_create(&t[id], NULL,  f, arg);
    }
    for (id = 0; id < 2; id++){
        printf("main: waiting for thread %d\n", id);
        pthread_join(t[id], NULL);
    }
    printf("main: all threads terminated\n");
    return 0;
}