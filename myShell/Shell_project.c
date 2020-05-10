/**
UNIX Shell Project

Sistemas Operativos
Grados I. Informatica, Computadores & Software
Dept. Arquitectura de Computadores - UMA

Some code adapted from "Fundamentos de Sistemas Operativos", Silberschatz et al.

To compile and run the program:
   $ gcc Shell_project.c job_control.c -o Shell
   $ ./Shell          
	(then type ^D to exit program)

**/

#include "job_control.h"   // remember to compile with module job_control.c 
#include <string.h>
#define MAX_LINE 256 /* 256 chars per line, per command, should be enough. */

// -----------------------------------------------------------------------
//                            MAIN          
// -----------------------------------------------------------------------




int main(void)
{
	char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
	int background;             /* equals 1 if a command is followed by '&' */
	char *args[MAX_LINE/2];     /* command line (of 256) has max of 128 arguments */
	// probably useful variables:
	int pid_fork, pid_wait; /* pid for created and waited process */
	int status;             /* status returned by wait */
	enum status status_res; /* status processed by analyze_status() */
	int info;				/* info processed by analyze_status() */

	job* jobs_list; 				//
	
	// My handler to work with signals
	void signal_handler(int signal){		//
		
	}

	
	jobs_list = new_list("My list of tasks"); 	//
	
	signal(SIGCHLD,signal_handler); 		//
	/* Signal that will be used when a user wants to suspend
	*  the background task by pressing Ctrl+Z on the keyboard
	*/
	signal(SIGTSTP,signal_handler);			//
	/* The shell should ignore these signals:
	*  SIGINT, SIGQUIT, SIGTSTP, SIGTTIN, SIGTTOUT
	*  but the commnad created with fork() should
	*  restore its default behaviour
	*/
	ignore_terminal_signals();			//	


	while (1)   /* Program terminates normally inside get_command() after ^D is typed*/
	{   		
		printf("COMMAND->");
		fflush(stdout);
		get_command(inputBuffer, MAX_LINE, args, &background);  /* get next command */
		
		if(args[0]==NULL) continue;   // if empty command
		
		/* the steps are:
			 (1) fork a child process using fork()
			 (2) the child process will invoke execvp()
			 (3) if background == 0, the parent will wait, otherwise continue 
			 (4) Shell shows a status message for processed command 
			 (5) loop returns to get_commnad() function
		*/
		
		//--------------------------------------- MY OWN CODE BELOW --------------------------------------------
		// (1)FORK A CHILD PROCESS USING fork()
   		pid_fork = fork();

    		/* Errors in fork mean that there is no child process. Parent is alone. */
   		if (pid_fork == -1)
    		{
        		fprintf(stderr, "parent: error in fork\n");
        		exit(1);
    		}

    		if (pid_fork == 0)
    		{
       		
        	// <POST-FORK CHILD ONLY CODE HERE>
		
		//I restore default behaviour
        	restore_terminal_signals(); 

       		//SEE HERE BRO TASK 1  a)
        	executable_args[0] = "cat"; // A custom name for the process
        	executable_args[1] = "mypipecat";
        	executable_args[2] = NULL; // Last argument

        	fprintf(stderr, "child : %d %s\n", getpid(), executable_file);
		
		//(2) THE CHILD PROCESS WILL INVOKE execvp()
        	execvp(executable_args[0], executable_args);
		
        	fprintf(stderr, "child: error in exec\n");
        	exit(EXIT_FAILURE);
	    	}
   		else
   		{
        	// <POST-FORK PARENT ONLY CODE HERE>
       
  		}

    		/* Only parent process should reach here */
   		

		
	} // end while
}
