/*
 * This program is an implementation of the Earliest Deadline First real-time
 * CPU scheduling algorithm. Written and Tested in macOS 10.14 (Kernel
 * Version: 18.0.0)
 */

#include <stdio.h>
#include <stdlib.h>

// Structure declaration
typedef struct {
    int burst_time;
    int period;
    int remaining_time;
} pcb;

// Function declaration
int gcd(int a, int b);

int lcm(pcb a[], int num);

int main() {

    int NumOfProcesses = 0; // Number of processes
    int MaxTime = 0; // Maximum execution time for all process
    printf("Enter the number of processes to schedule: ");
    scanf("%d", &NumOfProcesses);

    // Create a pcb array using dynamic memory allocation
    pcb *ProcessInfoArray = (pcb *) malloc(sizeof(pcb) * NumOfProcesses);

    // Ask user to input the burst time and period for each process
    for (int i = 0; i < NumOfProcesses; i++) {
        printf("Enter the burst time of process %d: ", i + 1);
        scanf("%d", &ProcessInfoArray[i].burst_time);
        printf("Enter the period of process %d: ", i + 1);
        scanf("%d", &ProcessInfoArray[i].period);
        ProcessInfoArray[i].remaining_time = 0;
    }

    // MaxTime is the Least Common Multiple of all the periods
    MaxTime = lcm(ProcessInfoArray, NumOfProcesses);
    int nextProcess = -1;
    for (int i = 0; i < MaxTime;) {
        /*
         * Operate list:
         * 0 - CPU Idle
         * 1 - Process Choosing
         * 2 - Process Running
         * 3 - Process Finish ?
         * 4 - New Period
         * 5 - Overdue
         */

        int operate = 0;

        for (int k = 0; k < NumOfProcesses ; k++) {
            if (ProcessInfoArray[k].remaining_time == 0 && i % ProcessInfoArray[k].period == 0){
                operate = 4;
                break;
            }
            if (i > 0){ // When a process is overdue
                if (ProcessInfoArray[k].remaining_time > 0 &&
                ProcessInfoArray[k].remaining_time < ProcessInfoArray[k].burst_time &&
                i % ProcessInfoArray[k].period == 0
                ){
                    operate = 5;
                    break;
                }
            }
        }


        if (operate < 4){
            if(nextProcess == -1){ // Pick next process
                operate = 1;
            } else if (ProcessInfoArray[nextProcess].remaining_time > 0){ // Running the Process
                operate = 2;
            } else if (ProcessInfoArray[nextProcess].remaining_time == 0){ // Finish a Process
                operate = 3;
            }

            int FinishedInPeriod = 0; // Number Of the Process finished within the period
            for(int j = 0; j < NumOfProcesses; j++){
                if (ProcessInfoArray[j].remaining_time == 0){
                    FinishedInPeriod++;
                }
            }
            if (FinishedInPeriod == NumOfProcesses) {
                operate = 0;
                if (nextProcess != -1){ // no process before idle
                    operate = 3;
                }
            }
        }

        switch (operate) {
            case 0:
                i++; // CPU IDLE
                // printf("IDLE\n"); // FIXME: DELETE LATER
                break;
            case 1:
                nextProcess = 0; // Pick One
                for (int l = 0; l < NumOfProcesses; l++) {
                    if (ProcessInfoArray[l].remaining_time != 0) {
                        nextProcess = l;
                    }
                }
                for (int j = 0; j < NumOfProcesses; j++) {
                    /*printf("process %d deadline remaining time: %d\n", j + 1,
                           ProcessInfoArray[j].period - i % ProcessInfoArray[j].period);*/

                    // Pick the next running process
                    if (ProcessInfoArray[j].remaining_time > 0) {
                        if (ProcessInfoArray[j].period -
                            i % ProcessInfoArray[j].period<
                            ProcessInfoArray[nextProcess].period -
                            i % ProcessInfoArray[nextProcess].period) {
                            nextProcess = j;
                        }
                    }
                }
                printf("%d: process %d starts\n", i, nextProcess+1); // Print the start message
                break;
            case 2:
                // printf("%d: Process%d running\n", i, nextProcess+1); // Print Runing status, FIXME: DELETE LATER
                i++;
                ProcessInfoArray[nextProcess].remaining_time -= 1;
                if(ProcessInfoArray[nextProcess].remaining_time == 0){
                    // TODO
                }
                break;
            case 3:
                printf("%d: process %d end\n", i, nextProcess+1); // Print the end message
                nextProcess = -1; // Reset the next process
                break;
            case 4: // Meet Period
                // Update remaining time
                // printf("UPDATE REMAINING TIME"); FIXME: DELETE LATER

                if (nextProcess != -1){
                    if (i % ProcessInfoArray[nextProcess].period == 0 && ProcessInfoArray[nextProcess].remaining_time == 0){
                        // the process finished but not end
                        printf("%d: process %d end\n", i, nextProcess+1); // Print the end message
                        nextProcess = -1; // Reset the next process
                        break;
                    }
                }

                for(int j = 0; j < NumOfProcesses; j++){
                    if (i % ProcessInfoArray[j].period == 0){
//                        printf("--  time(%d) add for process %d\n",ProcessInfoArray[j].burst_time, j+1); // FIXME: DELETE
                        ProcessInfoArray[j].remaining_time += ProcessInfoArray[j].burst_time;
                    }
                }
//                for (int k = 0; k < NumOfProcesses; ++k) { // FIXME: DELETE LATER
//                    printf("--  %d: P%d remaining Time: %d\n", i, k + 1,
//                           ProcessInfoArray[k].remaining_time);
//                    printf("       \\DDL: %d\n", ProcessInfoArray[k].period -
//                                                 i %
//                                                 ProcessInfoArray[k].period);
//                }
                if (nextProcess != -1){
                    int urgentProcess = nextProcess;
                    for (int j = 0; j < NumOfProcesses; j++) {
                        // Pick the a more urgent process
                        if (ProcessInfoArray[j].remaining_time > 0) {
                            if (ProcessInfoArray[j].period -
                                i % ProcessInfoArray[j].period <
                                ProcessInfoArray[nextProcess].period -
                                i % ProcessInfoArray[nextProcess].period) {
                                urgentProcess = j;
                            }
                        }
                    }


                    int maxRemainingTimeProcess = -1;
                    for (int j = 0; j < nextProcess; j++) {
                        if(ProcessInfoArray[nextProcess].remaining_time < ProcessInfoArray[j].remaining_time){
                            if (ProcessInfoArray[j].period -
                            i % ProcessInfoArray[j].period ==
                            ProcessInfoArray[nextProcess].period -
                            i % ProcessInfoArray[nextProcess].period)
                            maxRemainingTimeProcess = j;
                        }
                    }

                    int minRemainingTimeProcess = -1;
                    if (maxRemainingTimeProcess == -1 && urgentProcess == nextProcess){
                        for (int j = 0; j < NumOfProcesses; j++) {
                            if (ProcessInfoArray[nextProcess].period - i % ProcessInfoArray[nextProcess].period ==
                            ProcessInfoArray[j].period - i % ProcessInfoArray[j].period &&
                            ProcessInfoArray[nextProcess].remaining_time > ProcessInfoArray[j].remaining_time){
                                if (minRemainingTimeProcess == -1){
                                    minRemainingTimeProcess = j;
                                } else{
                                    if (ProcessInfoArray[minRemainingTimeProcess].remaining_time > ProcessInfoArray[j].remaining_time){
                                        minRemainingTimeProcess = j;
                                    }
                                }
                            }
                        }
                    }

                    if (maxRemainingTimeProcess != -1){
                        urgentProcess = maxRemainingTimeProcess;
                    }

                    if (minRemainingTimeProcess != -1){
                        urgentProcess = minRemainingTimeProcess;
                    }

                    if (urgentProcess != nextProcess && ProcessInfoArray[urgentProcess].remaining_time != 0){
                        printf("%d: process %d preempted!\n", i, nextProcess+1);  // Preempted Message
                        nextProcess = urgentProcess;
                        printf("%d: process %d starts\n", i, nextProcess+1); // Print the start message
                    }
                }
                break; // End of case 4
            case 5:
                printf("%d: process %d missed deadline (%d ms left)\n", i, nextProcess+1, ProcessInfoArray[nextProcess].remaining_time);
                for(int j = 0; j < NumOfProcesses; j++){
                    if (i % ProcessInfoArray[j].period == 0){
//                        printf("--  time(%d) add for process %d\n",ProcessInfoArray[j].burst_time, j+1); // FIXME: DELETE
                        ProcessInfoArray[j].remaining_time += ProcessInfoArray[j].burst_time;
                    }
                }

                if (nextProcess != -1){
                    int urgentProcess = nextProcess;
                    for (int j = 0; j < NumOfProcesses; j++) {
                        // Pick the a more urgent process
                        if (ProcessInfoArray[j].remaining_time > 0) {
                            if (ProcessInfoArray[j].period -
                                i % ProcessInfoArray[j].period <
                                ProcessInfoArray[nextProcess].period -
                                i % ProcessInfoArray[nextProcess].period) {
                                urgentProcess = j;
                            }
                        }
                    }
                    if (urgentProcess != nextProcess && ProcessInfoArray[urgentProcess].remaining_time != 0){
                        printf("%d: process %d preempted!\n", i, nextProcess+1);  // Preempted Message
                        nextProcess = urgentProcess;
                        printf("%d: process %d starts\n", i, nextProcess+1); // Print the start message
                    }
                }
                break;
        }
    }
    printf("%d: MaxTime reached\n", MaxTime);
    free(ProcessInfoArray); // Release the memory
    return 0;
}

// Greatest Common Divisor (recursive version)
int gcd(int a, int b) {
    return b ? gcd(b, a % b) : a;
}

// Least Common Multiple
int lcm(pcb a[], int num) {
    int result = 1;
    for (int i = 0; i < num; i++) {
        result *= a[i].period / gcd(result, a[i].period);
    }
    return result;
}
