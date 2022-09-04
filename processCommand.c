/*********************************************************************
** Function: processCommand
** Description: Checks user input to see if any of the commands are
** built-in commands. If so, built-in commands are processed. Built-
** in commands include: exit, cd, or status
** Parameters: userInput (struct), and pointers to ints that act as
** flags (exitFlag, backProcess, foreProcess, inRedirect, outRedirect,
** builtcmd, exitStat)
** Pre-Conditions: user input must be parsed and stored into a struct
** Post-Conditions: function returns 1 if any of the built-in
** commands are processed. The flag "builtcmd" is also set to 1 to 
** to indicate that a built-in command was run. Otherwise returns 0. 
*********************************************************************/

#include <stdio.h>
#include "processInput.h"
#include "processCommand.h"



int processCommand(struct input* userInput, int* process, int* exitFlag, int* backProcess, int* foreProcess, int* inRedirect, int* outRedirect, int* builtcmd, int exitStat)
{
	// Exit program if command is "exit"
	if (strncmp(userInput->command, "exit", 4) == 0)
	{
		// Set exit flag to 1 (true)
		*exitFlag = 1;
		
		// Kill background processes
		for (int x = 0; x < 512; x++)
		{
			if (process[x] != -5)
			{
				kill(process[x], SIGTERM);
			}
		}

		builtcmd = 1;
		return 1;
		exit(0);
	}

	// Change directory if command starts with "cd"
	if (strncmp(userInput->command, "cd", 2) == 0)
	{
		// Array to store current working directory name
		char cwd[256];
		int flag;

		// If no arguments passed with "cd", change to
		// the directory specified in the HOME environment variable
		if (userInput->arguments[0] == '\0')
		{
			chdir(getenv("HOME"));
			getcwd(cwd, sizeof(cwd));
		}

		// If "cd" has arguments, change to that directory
		else
		{
			// Copy directory name to cwd up to the \n character
			strncpy(cwd, userInput->arguments[0], strlen(userInput->arguments[0]));

			// Change directory
			flag = chdir(cwd);

			// Display error message if directory doesn't exist
			if (flag == -1)
			{
				perror("Error");
				fflush(stdout);
			}
		}
		
		builtcmd = 1;
		return 1;
	}

	// Show status of last foreground process
	// Followed examples as shown in "3.1 Processes" video by Benjamin Brewster https://youtu.be/1R9h-H2UnLs
	if (strncmp(userInput->command, "status", 6) == 0)
	{
		int status;

		// Check if no other foreground process started
		// Exit value will be 0
		if (exitStat == -5)
		{
			status = 0;
			printf("Exit value %d\n", status);
			fflush(stdout);
		}

		// Display status of last foreground process
		else
		{	
			//Wait for child to terminate
			wait(&status);

			if (WIFEXITED(exitStat))
			{
				int exitStatus = WEXITSTATUS(exitStat);
				printf("Exit value %d\n", exitStatus);
				fflush(stdout);
			}

			else
			{
				if (WIFSIGNALED(exitStat))
				{
					int exitStatus = WTERMSIG(exitStat);
					printf("Exit value %d\n", exitStatus);
					fflush(stdout);

				}
			}
		}
	
		builtcmd = 1;
		return 1;
	}

	return 0;
}