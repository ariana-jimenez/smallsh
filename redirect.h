#ifndef	REDIRECT_H
#define REDIRECT_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>

int redirect(struct input* userInput, int inRedirect, int outRedirect);


#endif