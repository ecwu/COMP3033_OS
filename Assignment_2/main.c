#include <stdio.h>
#include <stdlib.h>

// function declaration
int gcd(int a, int b);
int lcm(int a[], int num);

int main() {

    int NumOfProcesses = 0; // Number of processes
    int MaxTime = 0; // Maximum execution time for all process (LCM of all process period time)
    printf("Enter the number of processes to schedule: ");
    scanf("%d", &NumOfProcesses);

    // Create two arrays with the size of processes number using dynamic memory allocation
    int *ProcessBurstTimeArray = (int *) malloc(sizeof(int) * NumOfProcesses);
    int *ProcessPeriodTimeArray = (int *) malloc(sizeof(int) * NumOfProcesses);

    for (int i = 0; i < NumOfProcesses; i++) {
        printf("Enter the burst time of process %d: ", i + 1);
        scanf("%d", &ProcessBurstTimeArray[i]);
        printf("Enter the period of process %d: ", i + 1);
        scanf("%d", &ProcessPeriodTimeArray[i]);
    }

    MaxTime = lcm(ProcessPeriodTimeArray, NumOfProcesses);

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