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

        int pipe_index=-1;
        for(int index=0;args[index]!=NULL;index++){
            if(strcmp(args[index], "|") == 0){
                pipe_index=index;
                break;
            }
        }

        if(pipe_index != -1) {
            args[pipe_index]=NULL;
            char** cmd1 = args;
            char** cmd2 = &args[pipe_index+1];

            int pipefd[2];
            if(pipe(pipefd)<0){
                perror("pipe failed");
                continue;
            }

            pid_t pid1 = fork();
            if(pid1<0){
                perror("fork failed during pipe exec 1");
                continue;
            }

            if(pid1==0){
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[0]);
                close(pipefd[1]);

                execvp(cmd1[0], cmd1);

                perror("execvp() during pipe exec 1 failed");
                exit(1);
            }

            pid_t pid2 = fork();
            if(pid2<0){
                perror("fork() failed during pipe exec 2");
                continue;
            }

            if(pid2==0){
                dup2(pipefd[0], STDIN_FILENO);
                close(pipefd[0]);
                close(pipefd[1]);

                execvp(cmd2[0], cmd2);

                perror("execvp() failed during pipe exec 2");
                exit(1);
            }

            close(pipefd[0]);
            close(pipefd[1]);


            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
            continue;
        }

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