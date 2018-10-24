#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#include "PCB.h"

int TOTAL_PROCS =0 ;
int CurrentProcess = 0;
struct ProcessControlBlock *Processes = NULL;
int Run =0;
int Exit =0;
sigset_t signal_set; 

char *prgm = "./gambler";
char *args[] = { "1","2","3","4","5","6","7","8","9"};

void InitilizePCBs()
{
	printf("Enter: %s\n",__FUNCTION__);
	Processes = (struct ProcessControlBlock *)malloc(sizeof(struct ProcessControlBlock) *TOTAL_PROCS);
	int i =0; 
	for( i =0; i< TOTAL_PROCS; i++)
	{
		processes[i].PID = NOTRUN;
		processes[i].status = -1;
	// TODO:  Initilize PCB Processes[i]
		char *dup = strdup(prgm);	
		processes[i].command = dup;
		processes[i].args = (char **)malloc(sizeof(char*)*3);
		processes[i].args[0] = strdup(prgm);
		processes[i].args[1] = strdup(args[i]);	
	}
	printf("Exit: %s\n",__FUNCTION__);
}

void FreePCBs()
{
	printf("Enter: %s\n",__FUNCTION__);
	int i =0; 
	for( i =0; i< TOTAL_PROCS; i++)
	{	
		free(processes[i].command);
		// todo: Cleanup Processes[i]
		int itr =0;
		for (itr =0; Processes[i].args[itr] != NULL; itr++)
		{			
			free(processes[i].args[itr]);
		
		}
	}
	free(Processes);
	Processes=NULL;
	printf("Exit: %s\n",__FUNCTION__);
}

void PrintPCB(struct ProcessControlBlock *print)
{
	printf("Enter: %s\n",__FUNCTION__);
	printf("\tCMD: %s\n", print->command);
	int i=0;
	while(print->args[i] != NULL)
	{
		i++;
	}
	printf("\tArgs: %d\n", i);
	i =0;
	while(print->args[i] != NULL)
	{
		printf("\t\tArg: %s\n", print->args[i]);
		i++;
	}
	printf("\tPID: %d\n",print->PID);
	switch(print->status)
	{
		case NOTRUN:
			printf("\tStatus: NOTRUN\n");
			break;
		case RUNING:
			printf("\tStatus: RUNING\n");
			break;
		case PAUSED:
			printf("\tStatus: PAUSED\n");
			break;
		case EXITED:
			printf("\tStatus: EXITED\n");
			break;
		default:
			printf("\tStatus: ERROR, invalid status: %d\n",print->status);
			break;
	}
	printf("Exit: %s\n",__FUNCTION__);
}

void PrintAllPCB()
{
	printf("Enter: %s\n",__FUNCTION__);
	int i=0;
	for(i=0; i< TOTAL_PROCS; i++)
	{
		PrintPCB(&Processes[i]);
	}
	printf("Exit: %s\n",__FUNCTION__);
}

void sigusr1_handler(int signum)
{
	printf("Enter: %s\n",__FUNCTION__);
	Run =1;
	printf("Exit: %s\n",__FUNCTION__);
}

void sigusr2_handler(int signum)
{
	printf("Enter: %s\n",__FUNCTION__);
	// todo : Block other signals.
	sigprocmask(SIG_BLOCK, signal_set, NULL)	
	Exit =1;
	// todo : unblock other signals

	printf("Exit: %s\n",__FUNCTION__);
}

void WaitAndExecOrCleanup(struct ProcessControlBlock *prgm)
{
	printf("Enter: %s\n",__FUNCTION__);

	while(Run ==0)
	{
		usleep(1);
	}
	// TODO: Run Execvp
	
	printf("FAILED EXECVP Command :\"%s\" with result %d\n",prgm->command,result);
	// Cleanup PCBS
	// Exit
	
	printf("Exit: %s\n",__FUNCTION__);

}

void LaunchProcess(struct ProcessControlBlock *launch)
{
	printf("Enter: %s\n",__FUNCTION__);

	//todo:  Fork: Either run exec as child or save pid as parent.








	printf("Exit: %s\n",__FUNCTION__);	
}

void LaunchAllProcesses()
{
	printf("Enter: %s\n",__FUNCTION__);
	
	// Subscribe to SIGUSR1 so child has it. 

	int i=0;
	for(i =0; i<TOTAL_PROCS;i++)
	{
		LaunchProcess(&Processes[i]);	
	}
	// unsubscribe to sigusr 1 

	printf("Exit: %s\n",__FUNCTION__);
}


void settimer(int sec, int microsec)
{
	printf("Enter: %s\n",__FUNCTION__);
	
	//todo : set timer for seconds & useconds

	printf("Exit: %s\n",__FUNCTION__);
}

void stoptimer()
{
	printf("Enter: %s\n",__FUNCTION__);

	//todo: stop timer. 

	printf("Exit: %s\n",__FUNCTION__);
}

void sigalarm_handler(int signum)
{
	printf("Enter: %s\n",__FUNCTION__);
	// todo: Block signals
	if(CurrentProcess == TOTAL_PROCS)
	{
		//todo: Send a sigusr2 to oursevles to exit.
	}
	else
	{
		switch(Processes[CurrentProcess].status)
		{
			case NOTRUN:
				//todo: send correct signal
				//todo: update process state
				break;
			case RUNING:
				//todo: send correct signal
				//todo: update process state
				break;
			case PAUSED:
				//todo: send correct signal
				//todo: update process state
				break;
			case EXITED:
				CurrentProcess++;
				break;
	
		}
	}
	// todo: unblock signals
	printf("Exit: %s\n",__FUNCTION__);
}

void sigchild_handler(int signum)
{
	printf("Enter: %s\n",__FUNCTION__);
	//todo: block signals

	pid_t pid;
	int status;

	// todo: Loop over all children pids that had a status change. (WNOHANG)
	{
		printf("PID Status Changed:%d to %d\n",pid,status);
		printf("Exited?: %d\n",WIFEXITED(status));
		//todo: check if child process has exited.
		{
			int i;
			for(i =0; i< TOTAL_PROCS;i++)
			{
				if(Processes[i].PID==pid)
				{
					printf("Set State Exited: %d\n",pid);
					//todo: Update PID status.
					//todo: send a sigalrm to ourself to trigger scheduling next process.
					break;
				}
			}
		}
	}
	//todo: unblock signals
	printf("Exit: %s\n",__FUNCTION__);
}

int main(int argc, char *argv[])
{
	printf("Enter: %s\n",__FUNCTION__);
	if(argc !=2)
		exit(0);
	TOTAL_PROCS = atoi(argv[1]);
	if((TOTAL_PROCS < 1) || (TOTAL_PROCS > 9))
		exit(0);
	
	InitilizePCBs();

	PrintAllPCB();

	LaunchAllProcesses();

	//todo: setup signal blocking .



	//todo: subscribe to sigusr2/sigalrm/sigchld




	settimer(0,500000);

	while(Exit == 0)
	{
		pause();
	}
	//todo: unsubscribe to signals.

	FreePCBs();
		

	printf("Exit: %s\n",__FUNCTION__);
	return 0;	
}
