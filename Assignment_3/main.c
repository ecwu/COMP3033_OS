#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define N 26  // Total number of threads (in addition to main thread).
#define M 10  // Number of loop iterations per thread.
int sum = 0;  // Data shared by all the threads.
int flag[N];

// The function executed by each thread.
void *runner(void *param) {
    int i = *(int *) param; // This thread’s ID number.
    int m;
    for (m = 0; m < M; m++) {
        flag[i] = 1;
        int door_status = 0;
        while (door_status != N) { // Atomic test.
            door_status = 0; // door
            for(int j = 0; j < N; j++){
                if (flag[j] < 3){
                    door_status++;
                }
            }
        }
        // Pi goes through the entrance door:
        flag[i] = 3;
        for (int j = 0; j < N; j++) {
            if (flag[j] == 1) {
                flag[i] = 2;
                while (flag[j] != 4);
                // then Pi starts waiting inside the waiting room:
            }
        }
//        if (wait_status != 0){
//            flag[i] = 2;
//            // until Pj comes inside the waiting room too and the entrance
//            // door closes (then flag[j] becomes 4 and Pi then stops waiting):
//            while (flag[wait_status] != 4);
//        }

        flag[i] = 4;

//        if (i - 1 >= 0 ? 1 : 0) {
//            while (flag[i - 1] >= 2) // Atomic test.
//                ;
//        }

        int smaller_status = 0;
        while (smaller_status != i){
            smaller_status = 0;
            for (int j = 0; j < i; j++) {
                if (flag[j] < 2){
                    smaller_status++;
                }
            }
        }

        // The Critical Section starts right below.
        int s = sum;
        // Even threads increase s, odd threads decrease s.
        if (i % 2 == 0) {
            s++;
        } else {
            s--;
        }
        // Sleep a small random amount of time.  Do not remove this code.
        struct timespec delay;
        delay.tv_sec = 0;
        delay.tv_nsec = 100000000ULL * rand() / RAND_MAX;
        nanosleep(&delay, NULL);
        sum = s;
        // The Critical Section ends right above.

//        if (i + 1 < N ? 1 : 0) {
//            while (flag[i+1] == 3 || flag[i+1] == 2) // Order matters!
//                ;
//        }

        int wait_open = 0;
        while (wait_open != N - i - 1){
            wait_open = 0;
            for (int j = i + 1; j < N; j++) {
                if (flag[j] < 2 || flag[j] == 4){
                    wait_open++;
                }
            }
        }

        flag[i] = 0;
        printf("%c", 'A' + i); // Print this thread’s ID number as a letter.
        fflush(stdout);
    }
    return 0; // Thread dies.
}

int main(void) {
    pthread_t tid[N]; // Thread ID numbers.
    int param[N];     // One parameter for each thread.
    int i;
    // Create N threads.  Each thread executes the runner function with
    // i as argument.
    for (int i = 0; i < N; i++) {
        flag[i] = 0;
    }
    for (i = 0; i < N; i++) {
        param[i] = i;
        pthread_create(&tid[i], NULL, runner, &param[i]);
    }
    // Wait for N threads to finish.
    for (i = 0; i < N; i++) {
        pthread_join(tid[i], NULL);
    }
    printf("\nResult is %d\n", sum);
    return 0;
}