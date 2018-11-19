/*
 * This program is to use Szymanski’s synchronization algorithm to
 * prevent race conditions in a multi-threaded program. Written and
 * Tested in macOS 10.14.1 (Kernel Version: 18.2.0)
 */
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define N 26  // Total number of threads (in addition to main thread).
#define M 10  // Number of loop iterations per thread.
int sum = 0;  // Data shared by all the threads.
int flag[N];  // Flag data for each thread

// The function executed by each thread.
void *runner(void *param) {
    int i = *(int *) param; // This thread’s ID number.
    int m;
    for (m = 0; m < M; m++) {
        // Pi wants to enter the waiting room:
        flag[i] = 1;

        int door_status = 0;
        while (door_status != N) { // Wait for open door
            // The door is open only when all the thread have a flag less than 3
            door_status = 0; // Door, N: open, less than N: close
            for(int j = 0; j < N; j++){
                if (flag[j] < 3){
                    door_status++;
                }
            }
        }

        // Pi goes through the entrance door:
        flag[i] = 3;

        // Check whether Pj wants to enter the waiting room too:
        int wait_enter = 0;
        while (wait_enter == 0){
            int want_enter = 0; // indicate whether there is an thread want to enter
            for (int j = 0; j < N; j++) {
                if (flag[j] == 1){ // There is a thread want to enter
                    flag[i] = 2; // Pi waiting for the other threads to enter
                    want_enter = 1;
                    break;
                }
            }
            if (want_enter == 1) { // a thread want to enter the room
                int entered = 0; // indicate the thread entered the waiting room or not
                while (entered == 0) { // wait for threads that want to enter
                    for (int j = 0; j < N; j++) {
                        if (flag[j] == 4) { // one of the thread is ready to enter the critical section
                            entered = 1; // the thread is entered the critical section
                            break;
                        }
                    }
                }
            }
            wait_enter = 1; // end waiting
        }

        // Pi is ready to enter the critical section
        flag[i] = 4;

        int smaller_status = 0;
        /* smaller_status indicate how many threads with a smaller pid than Pi
         * that already entered the critical section (flag is 0 or 1)
         * The Pi will keep waiting until all Pj(j >= 0 && j < i)
         * is finished running*/
        while (smaller_status != i){
            smaller_status = 0;
            for (int j = 0; j < i; j++) { // for all thread pid smaller than i
                if (flag[j] < 2){ // if the thread is finished running
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
        

        /*
         * Pi have to wait for Pj(j > i) which already inside the waiting room
         * to a ready to enter the critical section status
         * (flag[j] == 4 or flag < 2)
         */
        int wait_open = 0;
        while (wait_open != N - i - 1){
            wait_open = 0;
            for (int j = i + 1; j < N; j++) { // for all thread j pid larger than i
                if (flag[j] < 2 || flag[j] == 4){ // if pj is ready to enter critical section or not in the room
                    wait_open++;
                }
            }
        }

        flag[i] = 0; // finish the running and don't want to enter the room.
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