#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include"parser.h"

int main () {
    char input[1024];
    char* args[100];
    while(1){
        printf("bashless> ");
        fflush(stdout);
        if(fgets(input, sizeof(input), stdin)==NULL){
            break;
        }

        // parsing
        parse_input(input, args);

        pid_t pid = fork(); // pid_t -> data type to store process id

        if(pid<0){
            perror("fork() failed!");
            continue;
        }

        if(pid==0){
            //child process
            execvp(args[0], args);

            perror("execvp() failed");
            exit(1);
        }

        else {
            // parent process
            waitpid(pid, NULL, 0); // takes process id, exit status of child process (NULL means idc), flags which tells how the waitpid will behave until child is not terminated
        }
    }
    return 0;
}