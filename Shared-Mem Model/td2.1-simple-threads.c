// C11 code that creates two threads, each printing a simple message.

#include <stdio.h>
#include <threads.h>
typedef unsigned long int pthread_t;
extern pthread_t pthread_self(void);

int print_message() {
    printf("Hello from thread with identifier : %d\n", (unsigned int)pthread_self());
    return 0;
}

int main() {
    thrd_t thread1, thread2;
    thrd_create(&thread1, print_message, NULL);
    thrd_create(&thread2, print_message, NULL);
    thrd_join(thread1, NULL);
    thrd_join(thread2, NULL);
    return 0;
    }