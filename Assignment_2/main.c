/*
 * This program is an implementation of the Earliest Deadline First real-time
 * CPU scheduling algorithm. Written and Tested in macOS 10.14 (Kernel
 * Version: 18.0.0)
 */

#include <stdio.h>
#include <stdlib.h>

// Function declaration
int gcd(int a, int b);
int lcm(int a[], int num);

int main() {

    int NumOfProcesses = 0; // Number of processes
    int MaxTime = 0; // Maximum execution time for all process
    printf("Enter the number of processes to schedule: ");
    scanf("%d", &NumOfProcesses);

    // Create two arrays using dynamic memory allocation
    int *ProcessBurstTimeArray = (int *) malloc(sizeof(int) * NumOfProcesses);
    int *ProcessPeriodTimeArray = (int *) malloc(sizeof(int) * NumOfProcesses);
    // Array use to store how many time the process run
    int *ProcessRunCounter = (int *) malloc(sizeof(int) * NumOfProcesses);

    // Ask user to input the burst time and period for each process
    for (int i = 0; i < NumOfProcesses; i++) {
        printf("Enter the burst time of process %d: ", i + 1);
        scanf("%d", &ProcessBurstTimeArray[i]);
        printf("Enter the period of process %d: ", i + 1);
        scanf("%d", &ProcessPeriodTimeArray[i]);
        ProcessRunCounter[i] = 0;
    }

    // MaxTime is the Least Common Multiple of all the periods
    MaxTime = lcm(ProcessPeriodTimeArray, NumOfProcesses);

    int currentTime = 0;
    int needToPreempt = 0, PreemptingProcess = -1, PreemptedProcess = -1, PreemptedProcessRestTime = 0;

    while (currentTime < MaxTime){
        int nextProcess = 0;
        for (int i = 0; i < NumOfProcesses; i++){
            if (i != nextProcess){
                if (ProcessPeriodTimeArray[i] * (ProcessRunCounter[i]+1) - currentTime <
                    ProcessPeriodTimeArray[nextProcess] * (ProcessRunCounter[nextProcess]+1) - currentTime){
                    nextProcess = i;
                }
            }
        }
        if (nextProcess == PreemptedProcess){
            if (PreemptedProcessRestTime + currentTime < ProcessPeriodTimeArray[PreemptedProcess] * (ProcessRunCounter[PreemptedProcess] + 1)){
                nextProcess = PreemptedProcess;
                printf("%d: process %d starts\n", currentTime, nextProcess+1);
                currentTime += PreemptedProcessRestTime;
                PreemptedProcessRestTime = 0;
                ProcessRunCounter[nextProcess]++;
                printf("%d: process %d end\n", currentTime, nextProcess+1);
                PreemptedProcess = -1;
                continue;
            }
        }
        printf("%d: process %d starts\n", currentTime, nextProcess+1);

        for (int i = 0; i < NumOfProcesses; i++){
            if (i != nextProcess){
                if (currentTime + ProcessBurstTimeArray[nextProcess] > ProcessPeriodTimeArray[i] * (ProcessRunCounter[i] + 1)){
                    needToPreempt = 1;
                    PreemptingProcess = i;
                    break;
                }
            }
        }
        if (needToPreempt == 1){
            PreemptedProcess = nextProcess;
            PreemptedProcessRestTime = ProcessPeriodTimeArray[PreemptingProcess] * (ProcessRunCounter[PreemptingProcess] + 1) - currentTime;
            currentTime += ProcessPeriodTimeArray[PreemptingProcess] * (ProcessRunCounter[PreemptingProcess] + 1) - PreemptedProcessRestTime;
            printf("%d: process %d preempted!\n", currentTime, PreemptedProcess);
            needToPreempt = 0;
            nextProcess = PreemptingProcess;
            PreemptingProcess = -1;
            printf("%d: process %d starts\n", currentTime, nextProcess+1);
        }
        currentTime += ProcessBurstTimeArray[nextProcess];
        ProcessRunCounter[nextProcess]++;
        printf("%d: process %d end\n", currentTime, nextProcess+1);
    }
    printf("%d: MaxTime reached", currentTime);

    free(ProcessBurstTimeArray);
    free(ProcessPeriodTimeArray);
    free(ProcessRunCounter);

    return 0;
}

// Greatest Common Divisor (recursive version)
int gcd(int a, int b) {
    return b ? gcd(b, a % b) : a;
}
// Least Common Multiple
int lcm(int a[], int num) {
    int result = 1;
    for (int i = 0; i < num; i++) {
        result *= a[i] / gcd(result, a[i]);
    }
    return result;
}

