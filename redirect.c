/*********************************************************************
** Function: redirect
** Description: Performs input and output redirection using the 
** dup2() function.
** Parameters: struct containing parsed user input, and two ints
** inRedirect and outRedirect that serve as flags
** Pre-Conditions: userInput must be parsed
** Post-Conditions: If redirection occurs, function returns
** 1 to indicate that redirection has occurred. Otherwise returns 0.
** Based code off from "Exploration: Processes and I/O" module
*********************************************************************/

#include "processInput.h"
#include "redirect.h"

int redirect(struct input* userInput, int inRedirect, int outRedirect)
{
	// Perform output redirection if outRedirect flag is 1 (true)
	if (outRedirect == 1)
	{
		// Open file
		int targetFD = open(userInput->outFile, O_WRONLY | O_CREAT | O_TRUNC, 0640);
		
		// If unable to open file, print error message and set exit status to 1
		if (targetFD == -1) 
		{
			printf("Cannot open %s\n", userInput->outFile);
			fflush(stdout);
			exit(1);
		}

		// Use dup2 to redirect standard output to targetFD
		int result = dup2(targetFD, 1);

		if (result == -1) 
		{
			perror("dup2");
			fflush(stdout);
			exit(2);
		}

		fcntl(targetFD, F_SETFD, FD_CLOEXEC);

		return 1;
	}


	// Perform input redirection if inRedirect flag is 1 (true)
	if (inRedirect == 1)
	{
		// Open file for reading only
		int sourceFD = open(userInput->inFile, O_RDONLY);
		
		// If unable to open file, print error message and set exit status to 1
		if (sourceFD == -1) 
		{
			printf("Cannot open %s\n", userInput->inFile);
			fflush(stdout);
			exit(1);
		}
		
		// Use dup2 to redirect standard input to targetFD
		int result = dup2(sourceFD, 0);

		if (result == -1) 
		{
			perror("dup2");
			fflush(stdout);
			exit(2);
		}

		fcntl(sourceFD, F_SETFD, FD_CLOEXEC);

		return 1;
	}

	return 0;
}