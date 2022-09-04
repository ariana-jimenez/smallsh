#ifndef	PROCESSCOMMAND_H
#define PROCESSCOMMAND_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>

int processCommand(struct input* userInput, int* process, int* exitFlag, int* backProcess, int* foreProcess, int* inRedirect, int* outRedirect, int* builtcmd, int exitStat);


#endif