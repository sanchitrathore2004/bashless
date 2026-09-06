#include<string.h>
#include<stdio.h>

void parse_input(char* input, char** args){
    int index = 0;
    args[index] = strtok(input, " \n");
    while(args[index] != NULL){
        index++;
        args[index] = strtok(NULL, " \n");
    }
    return ;
}