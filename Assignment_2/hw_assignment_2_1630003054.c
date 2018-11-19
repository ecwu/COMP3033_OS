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

    float cpuBurst = 0, totalWaiting = 0;

    // Create a pcb array using dynamic memory allocation
    pcb *ProcessInfoArray = (pcb *) malloc(sizeof(pcb) * NumOfProcesses);

    // Ask user to input the burst time and period for each process
    for (int i = 0; i < NumOfProcesses; i++) {
        printf("Enter the burst time of process %d: ", i + 1);
        scanf("%d", &ProcessInfoArray[i].burst_time);
        printf("Enter the period of process %d: ", i + 1);
        scanf("%d", &ProcessInfoArray[i].period);
        ProcessInfoArray[i].remaining_time = 0; // Init the remaining time
    }

    // MaxTime is the Least Common Multiple of all the periods
    MaxTime = lcm(ProcessInfoArray, NumOfProcesses);
    int nextProcess = -1; // Indicate which process currently are running

    for (int i = 0; i < MaxTime;) {
        /* Switch to different operation according to the current status
         *
         * Operate list:
         * 0 - CPU Idle
         * 1 - Process Choosing
         * 2 - Process Running
         * 3 - Process Finish
         * 4 - New Period
         * 5 - Overdue
         */

        int operate = 0;

        for (int k = 0; k < NumOfProcesses; k++) {
            if (ProcessInfoArray[k].remaining_time == 0 &&
                i % ProcessInfoArray[k].period == 0) {
                operate = 4; // If meet a period's start
                break;
            }
            if (i > 0) { // When a process is overdue
                if (ProcessInfoArray[k].remaining_time > 0 &&
                    ProcessInfoArray[k].remaining_time <
                    ProcessInfoArray[k].burst_time &&
                    i % ProcessInfoArray[k].period == 0
                        ) {
                    operate = 5; // If meet a period's start and also some process's remaining time is greater than 0
                    break;
                }
            }
        }


        if (operate < 4) { // If the status is non of them above
            if (nextProcess == -1) { // If currently don't have process
                operate = 1; // Pick next process
            } else if (ProcessInfoArray[nextProcess].remaining_time > 0) {
                // If a process is running and remaining_time greater than 0 (still running)
                operate = 2; // Running the Process
            } else if (ProcessInfoArray[nextProcess].remaining_time == 0) {
                // If the remaining_time go to 0 (finished running)
                operate = 3; // Finish a Process
            }

            int FinishedInPeriod = 0; // Number Of the Process finished within the period
            for (int j = 0; j < NumOfProcesses; j++) {
                if (ProcessInfoArray[j].remaining_time == 0) {
                    FinishedInPeriod++;
                }
            }
            if (FinishedInPeriod == NumOfProcesses) {
                operate = 0; // idle the cpu
                if (nextProcess != -1) { // no process before idle
                    operate = 3;
                }
            }
        }

        switch (operate) {
            case 0:
                i++; // CPU IDLE
                break;
            case 1:
                nextProcess = 0; // Pick One
                for (int l = 0; l < NumOfProcesses; l++) {
                    // The a process which its remaining_time is not equal to 0 (non finished)
                    if (ProcessInfoArray[l].remaining_time != 0) {
                        nextProcess = l;
                    }
                }
                for (int j = 0; j <
                                NumOfProcesses; j++) { // Find the Earliest Deadline Process
                    // Pick the next running process
                    if (ProcessInfoArray[j].remaining_time > 0) {
                        if (ProcessInfoArray[j].period -
                            i % ProcessInfoArray[j].period <
                            ProcessInfoArray[nextProcess].period -
                            i % ProcessInfoArray[nextProcess].period) {
                            nextProcess = j;
                        }
                    }
                }
                printf("%d: process %d starts\n", i,
                       nextProcess + 1); // Print the start message
                break;
            case 2:
                i++; // increase total running time
                ProcessInfoArray[nextProcess].remaining_time -= 1; // minus the remaining_time
                for (int k = 0; k < NumOfProcesses; k++) {
                    if (k != nextProcess &&
                        ProcessInfoArray[k].remaining_time > 0) {
                        // count waiting time for waiting process
                        totalWaiting++;
                    }
                }
                break;
            case 3:
                printf("%d: process %d end\n", i,
                       nextProcess + 1); // Print the end message
                nextProcess = -1; // Reset the next process
                break;
            case 4: // Meet Period
                // Update remaining time
                if (nextProcess != -1) {
                    if (i % ProcessInfoArray[nextProcess].period == 0 &&
                        ProcessInfoArray[nextProcess].remaining_time == 0) {
                        // the process finished but not end
                        printf("%d: process %d end\n", i,
                               nextProcess + 1); // Print the end message
                        nextProcess = -1; // Reset the next process
                        break;
                    }
                }

                for (int j = 0; j < NumOfProcesses; j++) {
                    if (i % ProcessInfoArray[j].period == 0) {
                        ProcessInfoArray[j].remaining_time += ProcessInfoArray[j].burst_time;
                        // add burst time to every process that meet it's period
                    }
                }
                if (nextProcess != -1) {
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
                    // When the current process have another process that have the same deadline but different remaining_time
                    for (int j = 0; j < nextProcess; j++) {
                        if (ProcessInfoArray[nextProcess].remaining_time <
                            ProcessInfoArray[j].remaining_time) {
                            if (ProcessInfoArray[j].period -
                                i % ProcessInfoArray[j].period ==
                                ProcessInfoArray[nextProcess].period -
                                i % ProcessInfoArray[nextProcess].period)
                                maxRemainingTimeProcess = j;
                        }
                    }

                    // if one of the process which are going to runing in the next status is overdue
                    // we need to switch to another process if needed

                    int minRemainingTimeProcess = -1;
                    if (maxRemainingTimeProcess == -1 &&
                        urgentProcess == nextProcess) {
                        for (int j = 0; j < NumOfProcesses; j++) {
                            if (ProcessInfoArray[nextProcess].period -
                                i % ProcessInfoArray[nextProcess].period ==
                                ProcessInfoArray[j].period -
                                i % ProcessInfoArray[j].period &&
                                ProcessInfoArray[nextProcess].remaining_time >
                                ProcessInfoArray[j].remaining_time) {
                                if (minRemainingTimeProcess == -1) {
                                    minRemainingTimeProcess = j;
                                } else {
                                    if (ProcessInfoArray[minRemainingTimeProcess].remaining_time >
                                        ProcessInfoArray[j].remaining_time) {
                                        minRemainingTimeProcess = j;
                                    }
                                }
                            }
                        }
                    }

                    if (maxRemainingTimeProcess != -1) {
                        urgentProcess = maxRemainingTimeProcess;
                    }

                    if (minRemainingTimeProcess != -1) {
                        urgentProcess = minRemainingTimeProcess;
                    }

                    // Do the preempt and start the next process

                    if (urgentProcess != nextProcess &&
                        ProcessInfoArray[urgentProcess].remaining_time != 0) {
                        printf("%d: process %d preempted!\n", i,
                               nextProcess + 1);  // Preempted Message
                        nextProcess = urgentProcess;
                        printf("%d: process %d starts\n", i,
                               nextProcess + 1); // Print the start message
                    }
                }
                break; // End of case 4
            case 5:
                // When coming to this case, the time is equal to some process's period
                printf("%d: process %d missed deadline (%d ms left)\n", i,
                       nextProcess + 1,
                       ProcessInfoArray[nextProcess].remaining_time);
                for (int j = 0; j < NumOfProcesses; j++) {
                    if (i % ProcessInfoArray[j].period == 0) {
                        ProcessInfoArray[j].remaining_time += ProcessInfoArray[j].burst_time;
                    }
                }

                // Choice a another process to run
                if (nextProcess != -1) {
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
                    // Switch Process
                    if (urgentProcess != nextProcess &&
                        ProcessInfoArray[urgentProcess].remaining_time != 0) {
                        printf("%d: process %d preempted!\n", i,
                               nextProcess + 1);  // Preempted Message
                        nextProcess = urgentProcess;
                        printf("%d: process %d starts\n", i,
                               nextProcess + 1); // Print the start message
                    }
                }
                break; // End of case 5
        }
    }
    printf("%d: MaxTime reached\n", MaxTime);
    for (int m = 0; m < NumOfProcesses; m++) {
        cpuBurst += MaxTime / ProcessInfoArray[m].period;
        // CPU burst time equal to the sum of all process's period divided by burst time
    }
    free(ProcessInfoArray); // Release the memory
    printf("Sum of all waiting times: %.0f\n", totalWaiting);
    printf("Number of CPU bursts: %.0f\n", cpuBurst);
    float averageWaitingTime =
            totalWaiting / cpuBurst; // Calculate the average waiting time
    printf("Average Waiting Time: %f\n", averageWaitingTime);
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
