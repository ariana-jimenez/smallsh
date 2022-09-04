#ifndef	PROCESSINPUT_H
#define PROCESSINPUT_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct input
{
    char *command;           // stores command
    char *arguments[513];    // stores arguments
    char* inFile;            // stores input file name
    char* outFile;           // stores output file name
    
    struct input* next;
};

int expandVariable(char* buffer, int x);
struct input* processInput(char* input, int* backProcess, int* foreProcess, int* inRedirect, int* outRedirect, int* count);
 
#endif