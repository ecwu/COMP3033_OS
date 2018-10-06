#include <string.h>
#define SIZE 1024
int main(void) {
    char prompt[] = "Type a command: ";
    char buf[SIZE];
    // Ask the user to type a command (write to standard output stream):
    write(1, prompt, sizeof(prompt)-1);
    // Read from the standard input the command typed by the user (note
    // that fgets also puts into the array buf the ’\n’ character typed
    // by the user when the user presses the Enter key on the keyboard):
    // Read from standard input stream (fd == 0)
    read(0, buf, SIZE);
    // Replace the Enter key typed by the user with ’\0’:
    for(int i = 0; i < SIZE; i++) {
        if (buf[i] == '\n' || buf[i] == '\r') {
            buf[i] = '\0';
            break;
        }
    }
    // prepare the variable to store the output content
    char output_content[SIZE*2] = "Executing command: ";
    // connect the input buffer with output_content string
    strcat(output_content, buf);
    // write to standard output stream (fd == 1)
    write(1, output_content, strlen(output_content));
    // add a line feed
    write(1, "\n", 1);
    return 0;
}