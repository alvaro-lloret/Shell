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
	job* fgJob;
	job* bgJob;
	enum status status_res_in_fg;
	
	// My handler to work with signals
	void signal_handler(int signal){			
		int position; 		//Current position in the jobs_list
		job *currentJob;	//Current job in the jobs_list
		int pidHandler;		//pid I will use to wait for currentJob

		//Here I should delete a job(because here I would receive the SIGCHLD)
		for(position=1;position<=list_size(jobs_list);position++){
			
			currentJob=get_item_bypos(jobs_list,position);
			
			//currentJob is stopped or background 
			if(currentJob->state != FOREGROUND){
				pidHandler = waitpid(currentJob->pgid,&status,WNOHANG|WUNTRACED);
				
				if(pidHandler == currentJob->pgid){
					//pidHandler has the same Process Group Id than currentJob so we analyze it
					status_res=analyze_status(status,&info);
					if(status_res==SUSPENDED){
						printf("Process %s\n",status_strings[0]); //O represents "Suspended"
						currentJob->state=STOPPED;
						printf("State of the currentJob is %s \n",state_strings[currentJob->state]);
					}else{
						printf("Process was finished by a termination signal\n");
						delete_job(jobs_list,currentJob);
						position--;
					}

				}//end if pidHandler was different of the Process Group Id

			}//end if currenJob was in foreground

		} //end for-loop of jobs_list
		 
	}//end of signal_handler

	
	jobs_list = new_list("my list of tasks"); 	//
	
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
		printf("\n");		
		system("pwd");	
		printf("#Current dir above# COMMAND->");
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
		
		//--------------------------------------- CODE BELOW --------------------------------------------//


		//Internal commands//
		//----------cd----------//
		if(strcmp(args[0],"cd")==0){
			if(args[1]==NULL){
				printf("You have to specify an argument for cd. Example: cd /home\n");			
			}else{
				if(chdir(args[1])==0){
					printf("Change directory to %s has been succesful\n",args[1]);	
				}else{
					printf("Change directory to %s hasn't been possible\n",args[1]);
				}
			}
			
			continue;
		}
		
		//----------jobs----------//
		if(strcmp(args[0],"jobs")==0){
			if(empty_list(jobs_list)){
				printf("There are no jobs on the list\n");			
			}else{
				print_job_list(jobs_list);
			}
			
			continue;
		}

		//----------fg----------//
		if(strcmp(args[0],"fg")==0){
			if(empty_list(jobs_list)){
				printf("There are no jobs on the list\n");			
			}else{
				if(args[1]==NULL){
					fgJob = get_item_bypos(jobs_list,1);	
				}else{
					fgJob = get_item_bypos(jobs_list,atoi(args[1]));
				}

				if(fgJob!=NULL){
					set_terminal(fgJob->pgid);
					fgJob->state=FOREGROUND;
					killpg(fgJob->pgid, SIGCONT);
					pid_wait=waitpid(pid_fork,&status,WUNTRACED);
					
					//We have to do the same than in the father
					set_terminal(getpid());
					status_res_in_fg = analyze_status(status,&info);
				
				printf("Foreground pid: %d, command: %s , %s, info: %d \n",pid_wait,fgJob->command,status_strings[status_res_in_fg],info);
					if(status_res_in_fg==SUSPENDED){
						printf("Process %s\n",status_strings[0]); //O represents "Suspended"
						fgJob->state=STOPPED;
						printf("State of the fgJob is %s \n",state_strings[fgJob->state]);
					}else{
						block_SIGCHLD();
						delete_job(jobs_list,fgJob);
						unblock_SIGCHLD();
					}
					
					
				}else {
					printf("No proccess has been found in that position\n");
				}
		

			}
			
			continue;
		}

		//----------bg----------//
		if(strcmp(args[0],"bg")==0){
			if(empty_list(jobs_list)){
				printf("There are no jobs on the list\n");
			}else{
				if(args[1]==NULL){
					bgJob = get_item_bypos(jobs_list,1);	
				}else{
					bgJob = get_item_bypos(jobs_list,atoi(args[1]));
				}

				if(bgJob!=NULL){
					if(bgJob->state==STOPPED){
						bgJob->state=BACKGROUND;
					}
					killpg(bgJob->pgid,SIGCONT);
					printf("Background jod running... pid: %d, command: %s \n",bgJob->pgid,bgJob->command);
				      	
				}else{
					printf("No proccess has been found in that position\n");
				}
				
			}
			
			continue;
		}

		// (1)FORK A CHILD PROCESS USING fork()
   		pid_fork = fork();

    		/* Errors in fork mean that there is no child process. Parent is alone. */
   		if (pid_fork == -1)
    		{
        		fprintf(stderr, "parent: error in fork\n");
        		exit(1);
    		}
    		else if (pid_fork == 0)
    		{
       			// <POST-FORK CHILD ONLY CODE HERE>
		
			/**To execute an extenal commnand in the Shell, the command 
			   should belong to an independent process group so that the 
 			   the terminal can be assigned to one unique foreground task 
			   at a time. Therefore, the child processes of the shell are
			   assigned a group id that differs from the parent id so I
			   need to use new_process_group()
			**/
			new_process_group(getpid());
		
			if(background==0){
				set_terminal(getpid());
			}
			
			//I restore default behaviour
        		restore_terminal_signals(); 

			//(2) THE CHILD PROCESS WILL INVOKE execvp()
        		execvp(args[0],args);
		
        		printf("Error, command not found: %s\n",args[0]);
        		exit(EXIT_FAILURE);
	    	}

		
    		/* Only parent process should reach here */

		new_process_group(pid_fork);

   		//I use block_SIGCHLD() and unblock_SIGCHLD() every time my code modify jobs_list 
		// in order to avoid consistency problems between the parent and the child when
		// when they try to modify jobs_list

		//(3) IF background == 0, THE PARENT WILL WAIT, OTHERWISE CONTINUE 
		if(background==0){
			//Here we have the foreground processes

			set_terminal(pid_fork);			
			int waitpid_value = waitpid(pid_fork,&status,WUNTRACED);
			set_terminal(getpid());
			status_res = analyze_status(status,&info);

			//(4) SHELL SHOWS A STATUS MESSAGE FOR PROCESSED COMMAND 
			printf("Foreground pid: %d, command: %s , %s, info: %d \n",pid_fork,args[0],status_strings[status_res],info);
			
			if(status_res==SUSPENDED){
				block_SIGCHLD();
				add_job(jobs_list, new_job(pid_fork, args[0], STOPPED));
				unblock_SIGCHLD();
			}	
			  
		}else{
			//Here we have the background processes

			block_SIGCHLD();
			add_job(jobs_list, new_job(pid_fork, args[0],BACKGROUND));
			unblock_SIGCHLD();
			
		 	printf("Background jod running... pid: %d, command: %s \n",pid_fork,args[0]);
			fflush(NULL);
		}
       
		//(5) LOOP RETURNS TO get_commnad() FUNCTION
					
	} // end while
}
