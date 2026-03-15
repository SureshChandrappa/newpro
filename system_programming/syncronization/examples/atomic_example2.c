// atomic_userspace.c
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <unistd.h>

atomic_int counter = ATOMIC_VAR_INIT(0);

void* thread_func(void* arg) {
    int thread_id = *(int*)arg;
    
    for (int i = 0; i < 5; i++) {
        atomic_fetch_add(&counter, 1);
        printf("Thread %d: counter = %d\n", thread_id, atomic_load(&counter));
        
        int old = atomic_exchange(&counter, atomic_load(&counter) + 2);
        printf("Thread %d: exchanged, old = %d, new = %d\n", 
               thread_id, old, atomic_load(&counter));
        
        usleep(100000);
    }
    return NULL;
}

int main() {
    pthread_t threads[3];
    int ids[3] = {1, 2, 3};
    
    for (int i = 0; i < 3; i++) {
        pthread_create(&threads[i], NULL, thread_func, &ids[i]);
    }
    
    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("Final counter: %d\n", atomic_load(&counter));
    return 0;
}
