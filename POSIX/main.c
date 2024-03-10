#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


int v = 0;
pthread_mutex_t m;

void *f(void *){
    // int i;
    pthread_t t;
    t = pthread_self();
    for (int i=0; i<10000; i++){
        pthread_mutex_lock(&m);
        v = v + 1;
        pthread_mutex_unlock(&m);
    }
    printf("%d: v = %d\n ", t, v);
}


void main (void){
pthread_t t1, t2;
    pthread_mutex_init(&m, NULL);
    pthread_create(&t1, NULL, f, NULL);
    pthread_create(&t2, NULL, f, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf ("v = %d\n ", v);
}