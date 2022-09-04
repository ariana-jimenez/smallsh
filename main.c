/*********************************************************************
** Program Filename: Assignment 3: smallsh
** Author: Ariana Jimenez
** Date: 2/8/2021
** Description: A small shell that implements features such as:
** providing a prompt for running commands, handling blank lines
** and comments, variable expansion, executing 3 built-in commands
** (exit, cd, and status), executing other commands, input and 
** output redirection, and foreground and background processes.
** Input: smallsh does not  take any arguments. Once started, it
** prompts user to enter a command until user decides to exit
** Output: Output will vary based on the commands entered by the user.
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>			
#include <sys/types.h>
#include <fcntl.h>
#include "processInput.h"
#include "processCommand.h"
#include "redirect.h"



int main(int argc, char* argv[])
{
	// bool type flags 
	int exitFlag;
	int backProcess = 0;
	int foreProcess = 0;
	int inRedirect = 0;
	int outRedirect = 0;
	int builtcmd = 0;
	int exitStat = -5;
	int count = 0;

	// array to hold raw user input
	char input[2049];
	struct input* userInput = malloc(sizeof(struct input));

	// array that holds list of process ID's
	int* process;
	process = malloc(512 * sizeof(int));
	int n = 0;

	// Initialize process array with a garbage value of -5
	for (int x = 0; x < 512; x++)
	{
		process[x] = -5;
	}

	// Prompt for command until user enters the "exit" command
	do
	{
		exitFlag = 0;

		// Prompt command input from user
		printf(": ");
		fflush(stdout);

		// Get input
		fgets(input, sizeof(input), stdin);
		
		// If command is a blank line or a comment line, 
		// reprompt for user input
		if(input[0] == '#' || input[0] == '\n' || input[0] == ' ')
		{
			continue;
		}

		// Otherwise parse input and process
		else
		{
			// Parse user input
			userInput = processInput(input, &backProcess, &foreProcess, &inRedirect, &outRedirect, &count);

			// Check for built-in commands, process if any
			builtcmd = processCommand(userInput, process, &exitFlag, &backProcess, &foreProcess, &inRedirect, &outRedirect, &builtcmd, exitStat);
			
			
			// If not built-in command, execute command
			if (builtcmd != 1)
			{
				pid_t spawnpid = -5;
				int childPid;
				int childStatus;

				// Create array to pass to exec function containing command and arguments
				char* arg[count + 2];

				// If there is at least 1 argument
				if (count != 0)
				{

					// Initialize / allocate array to pass to exec function
					int c = 256;
					for (int r = 0; r < count + 2; r++)
					{
						arg[r] = (char*)calloc(c, sizeof(char));
					}

					// Fill array with command
					strcpy(arg[0], userInput->command);

					// Fill array with arguments 
					int i = 0;
					for (int j = 1; j < count + 1; j++)
					{
						strcpy(arg[j], userInput->arguments[i]);
						i++;
					}

					// Set last argument of array to null
					int index = count + 2;
					arg[index -1] = '\0';
				}
				
				
				// Create new process
				spawnpid = fork();

				switch (spawnpid)
				{
				case -1:
					// fork fails
					perror("fork() failed!");
					fflush(stdout);
					exit(1);
					break;

				case 0:
					// Child will execute
					// Used portions of code from "Exploration: Process API - Executing a New Program"

					// Check if redirection needs to occur
					if (redirect(userInput, inRedirect, outRedirect) == 1)
					{
						// Call exec function
						execvp(userInput->command, userInput->arguments);

						// Print error msg if execvp fails, set exit status to 1
						perror("execvp");
						fflush(stdout);
						exit(1);
					}
					
					else
					{
						// Execute with command w/ arguments
						if (userInput->arguments[0] != NULL)
						{
							// Call exec function
							execvp(arg[0], arg);

							// Print error msg if execvp fails, set exit status to 1
							perror("execvp");
							fflush(stdout);
							exit(1);
						}

						// Execute with command with no arguments
						else
						{
							// Copy command into cmd to remove extra new line character
							char* cmd = (char*)calloc(256, sizeof(char));
							strncpy(cmd, userInput->command, strlen(userInput->command) - 1);


							// Create new array to pass to execvp
							char* arg[] = { cmd, NULL };

							// Call exec function
							execvp(arg[0], arg);

							// Print error msg if execvp fails, set exit status to 1
							perror("execvp");
							fflush(stdout);
							exit(1);
						}
					}
			
					// Terminate child process
					return 0;
					break;

				default:
					// Parent will execute

					// If command is background process, store spawnid (child id) in array of background processes
					if (backProcess == 1)
					{
						// Print background process id when it begins
						printf("Background pid is %d.\n", spawnpid);
						fflush(stdout);

						// Store background process ID in array
						// Array will be used by the built-in command "exit"
						process[n] = spawnpid;
						n++;
						
					}

					// Otherwise foreground process, call waitpid on child
					// Code borrowed from "Exploration: Process API - Monitoring Child Processes" module
					else
					{
						// Store process id in array
						process[n] = spawnpid;
						n++;


						// Wait for child process to terminate before prompting for next command
						childPid = waitpid(spawnpid, &childStatus, 0);


						if (WIFSIGNALED(childStatus))
						{
							printf("Terminated by signal %d\n", WTERMSIG(childStatus));
							fflush(stdout);
						}

						// Store exit status of foreground process.
						// This will be used by the built-in status command to check 
						// the exit status of the last foreground process
						exitStat = childStatus;
					}
					break;
				}
			}

			// Reset flags
			backProcess = 0;
			foreProcess = 0;
			inRedirect = 0;
			outRedirect = 0;
			builtcmd = 0;
			count = 0;
		}

	} while(exitFlag != 1);

	return 0;
}


