/*
 * This is a simple version of command prompt by reading users command
 * and try to execute the command using child process. The program is
 * written and tested in macOS 10.14 (Kernel Version: Darwin 18.0.0)
 */
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define SIZE 1024

int main(void) {
    char prompt[] = "Type a command: ";
    char buf[SIZE];
    pid_t child_pid; // PID of child process
    int status; // Exit status of child process
    while (1) {
        // Ask the user to type a command (write to standard output stream):
        write(1, prompt, sizeof(prompt) - 1);
        // Read from standard input stream (fd == 0)
        read(0, buf, SIZE);
        // Replace the Enter key typed by the user with ’\0’:
        for (int i = 0; i < SIZE; i++) {
            if (buf[i] == '\n' || buf[i] == '\r') {
                buf[i] = '\0';
                break;
            }
        }

        if (strcmp(buf, "exit") == 0) {
            // if user input "exit", terminate the loop
            char exit_info[] = "Exiting..\n";
            write(1, exit_info, strlen(exit_info));
            return 0;
        }

        // create a child process same as parent
        child_pid = fork();
        if (child_pid < 0) {
            char error_info[] = "Parent: fork failed\n";
            write(1, error_info, strlen(error_info));
            return 1;
        } else if (child_pid == 0) {
            // Parent will never run this part
            char child_running_info[] = "Child: now running same program as parent, doing exec\n";
            write(1, child_running_info, strlen(child_running_info));
            // Child process run the user command
            // Execlp begin with the argument point the filename which to be execute,
            // and follow with many arguments. MUST be terminated by a NULL pointer.
            execlp(buf, buf, NULL);
            char error_info[] = "Child: exec failed, die\n";
            write(1, error_info, strlen(error_info));
            // Child process dies after the failed exec
            return 1;
        } else {
            char wait_info[] = "Parent: now sleeping waiting for child to end\n";
            write(1, wait_info, strlen(wait_info));
            // wait for child to end
            // wait will suspends until one of its children terminates
            // if wait not performed, then the child remains in a "zombie" state
            wait(&status);
            char finish_info[] = "Parent: Finish wait for child, child is dead\n";
            write(1, finish_info, strlen(finish_info));
        }
    }
}