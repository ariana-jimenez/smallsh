#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>	
#include "processInput.h"

/*********************************************************************
** Function: expandVariable
** Description: Performs variable expansion whenever there is any
** instance of "$$" in a command line. It replaces it with the 
** process ID of the smallsh program.
** Parameters: char* buffer (containing user input) and int x for
** indexing the location of the first "$"
** Pre-Conditions: buffer must have user input, and must have an
** occurrence of "$$"
** Post-Conditions: Gets process ID, replaces the ""$$" in buffer
** with the processID of smallsh, returns updated buffer length
*********************************************************************/
int expandVariable(char* buffer, int x)
{
    char* tempBuffer = calloc(strlen(buffer), sizeof(char));
    char* pidStr = calloc(6, sizeof(char));
    int bufferLength = strlen(buffer);
    
    // Get process id
    pid_t pid = getppid();

    // store process id into a string 
    sprintf(pidStr, "%d", pid);

    // Make the '$' characters null
    buffer[x] = '\0';
    buffer[x + 1] = '\0';

    // Store rest of string after the '$$' occurence into a temp buffer
    int j = 0;
    for (int i = 2; i < bufferLength + 1; i++)
    {
        tempBuffer[j] = buffer[x + i];
        j++;
    }

    // Replace the two '$$' with the process id, concatenate with rest of buffer
    strcat(buffer, pidStr);
    strcat(buffer, tempBuffer);
    printf("%s\n", buffer);

    // return updated buffer length
    return strlen(buffer);
}


/*********************************************************************
** Function: processInput
** Description: Takes in raw user input and parses the data into a
** struct with the following members: command, arguments, inputFile,
** and outputFile. Also takes in int* parameters that act as flags 
** (backProcess, foreProcess, inRedirect, outRedirect) to be used
** back in main.
** Parameters: char* buffer, int* backProcess, int* foreProcess, 
** int* inRedirect, int* outRedirect
** Pre-Conditions: Buffer must contain user input, must not be blank
** Post-Conditions: Returns a struct that contains parsed user input
*********************************************************************/

struct input*processInput(char* buffer, int* backProcess, int* foreProcess, int* inRedirect, int* outRedirect, int* count)
{
    size_t length = 0;
    length = strlen(buffer);

    // Create struct to store parsed data
    struct input* userInput = malloc(sizeof(struct input));

    // Check if command is for background (check last character)
    if (buffer[strlen(buffer)-2] == '&')
    {
        // Set flag to indicate background process, to be used in main
        *backProcess = 1;
    }
    
    // Otherwise command is a foreground process
    else
    {
        // Set flag to indicate foreground process, to be used in main
        *foreProcess = 1;
    }
    
    // If any instances of "$$" in input, perform variable expansion 
    for (int x = 0; x < length; x++)
    {
        if (buffer[x] == '$' && buffer[x + 1] == '$')
        {
            length = expandVariable(buffer, x);
        }
        
    }

    // For use with strtok_r
    char* saveptr;

    // The first token is the command
    char* token = strtok_r(buffer, " ", &saveptr);
    userInput->command = calloc(strlen(token) + 1, sizeof(char));
    strcpy(userInput->command, token);


    // The next token is the argument(s) if any
    token = strtok_r(NULL, " ", &saveptr);
    

    // Allocate memory for arguments array. 513 rows allows for
    // max 512 arguments. For every row, allocate space for 256 char
    int col = 256;
    for (int row = 0; row < 513; row++)
    {
        userInput->arguments[row] = (char*)calloc(col, sizeof(char));
    }

    int x = 0;      // index for token character
    int i = 0;      // index for argument row
    int j = 0;      // index for argument col

    // Store arguments (if any) in arguments array
    while (token != NULL)
    {
        switch(token[x])
        {
        case '<':
            // Store arrow
            strcpy(userInput->arguments[j], token);
            j++;

            // Store the name after the arrow to the inFile member
            token = strtok_r(NULL, " ", &saveptr);
            userInput->inFile = calloc(strlen(token) + 1, sizeof(char));
            strncpy(userInput->inFile, token, strlen(token) - 1);

            // Move to next argument
            token = strtok_r(NULL, " ", &saveptr);
            *inRedirect = 1;
            break;

        case '>':
            
            // Store arrow
            strcpy(userInput->arguments[j], token);
            j++;

            // Store the name after the arrow to the outFile member
            token = strtok_r(NULL, " ", &saveptr);
            userInput->outFile = calloc(strlen(token) + 1, sizeof(char));
            strncpy(userInput->outFile, token, strlen(token) - 1);

            // Move to next argument
            token = strtok_r(NULL, " ", &saveptr);
            *outRedirect = 1;
            break;
        
        
        default: 
            // Store argument in arguments array
            //strcpy(userInput->arguments[j], token);
            strncpy(userInput->arguments[j], token, strlen(token) - 1);
            j++;

            // Move to next argument
            token = strtok_r(NULL, " ", &saveptr);
            i++;
        }
        
    }
   
    // Check for empty arguments in arguments array, make them null
    // This is to ensure that non-null arguments don't get passed to the exec functions
    for (int r = 0; r < 513; r++)
    {
        if (strlen(userInput->arguments[r]) == 0)
        {
            userInput->arguments[r] = '\0';  
        } 

        // Keep count of non-null arguments
        else
        {
            *count += 1;
        }
    }
    
    return userInput;
}
