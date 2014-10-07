#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "uInterp.h"
#include "Main.h"

#if defined(WINDOWS)
#pragma warning( disable: 4100 ) /* unreferenced arguments in x86 call */
#pragma warning( disable: 4189 ) /* unreferenced local variable */
#endif

#define MAXLINE 512

int Help(void);
int Quit(void);


// Declare a command table
static const uInterpCmd_T gExtCmds [] = 
{
    {Help,	        "help",         "show command help",                "help <cmd>"},
    {Quit,	        "quit",         "quit the interpreter",             "quit"},
};

static int gQuitFlag;

// Help command arguments
int Help(void)
{
    int Count;
    int i;

    Count = sizeof(gExtCmds)/sizeof(*gExtCmds);

    printf("%-10s %-30s %-40s\n", "Name", "Description", "Usage"); 
    printf("---------------------------------------------------------------------\n");
   
    for (i = 0; i < Count; i = i + 1)
    {
        printf("%-10s %-30s %-40s\n", 
                gExtCmds[i].pName, 
                gExtCmds[i].pDescription, 
                gExtCmds[i].pUsage);
    }
 
    return 0;
}   

// Quit command arguments
int Quit(void)
{
    gQuitFlag = 1;
    return 0;
}   

// Display the prompt 
void ShowPrompt(void)
{
    printf(">>");
}

// Remove leading and trailing charactes 
void TrimLine(char *pLine, const char *pChars)
{
	char *p;

	for (p = pLine; *p; p = p + 1)
	{
		if (!strchr(pChars, *p))
		{
			break;
		}
	}

	memmove(pLine, p, strlen(p)+1);

	for (p = pLine + strlen(pLine); p >= pLine; p = p - 1)
	{
		if (!strchr(pChars, *p))
		{
			break;
		}
		*p = '\0';
	}
}

// Display error message when bad command is entered 
void ShowBadCommand(char *pLine)
{
    char *p = pLine + strlen(pLine) - 1;

    for ( ; p >= pLine; p = p - 1)
    {
        if (*p == '\r' || *p == '\n')
        {
            *p = 0;
        }
        else
        {
            break;
        }
    }

    printf("\"%s\" is not a recognized internal command\n", pLine);
}
 
// Start running the interpreter loop
int StartInterpreter(void)
{
    char Line[MAXLINE];
    int Retval;

    // Construct the interpreter 	
    uInterpCtor(gExtCmds, sizeof(gExtCmds)/sizeof(*gExtCmds));

    // Display the prompt 
    ShowPrompt();
 
    // Start accepting lines of text 
    for ( ; ; )
    {
        // Read a line of text from the device / console 
 	    if (fgets(Line, sizeof(Line), stdin))
 	    {	
            // Attempt to execute the line of text 
            Retval = uInterpExecute(Line);

            // Command not recognized 
            if (Retval == UINTERP_ECOMMAND)
            {
                ShowBadCommand(Line);
            }
            else if (gQuitFlag)
            {
                break;
            }

            // Display the prompt 
            ShowPrompt();
        }

        // If end of file found 
        else if (feof(stdin))
        {
            break;
        }
    }	
 
    // Destroy the interpreter 
    uInterpDtor();

    return 0;
}	
 
   
// This is the start 
int main(int ac, char **av)
{
    return StartInterpreter();
}
