#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<string.h>
#include<fcntl.h>
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

        int redirect = -1;
        for(int index=0;args[index]!=NULL;index++){
            if(strcmp(args[index], ">") == 0){
                redirect=index;
                break;
            }
        }

        pid_t pid = fork(); // pid_t -> data type to store process id

        if(pid<0){
            perror("fork() failed!");
            continue;
        }

        if(pid==0){
            //child process
            if(redirect!=-1){
                char* fileName = args[redirect+1];
                int fd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0644); // 0 means im octal 6 means owner can r/w 4 means anyone in the group can read 4 means anyone outside the group can read

                if(fd<0){
                    perror("Cannot open file");
                    exit(1);
                }

                dup2(fd, STDOUT_FILENO);
                close(fd);

                args[redirect]=NULL;
            }

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