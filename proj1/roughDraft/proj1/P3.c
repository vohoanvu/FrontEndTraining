#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include "p1fxns.h"
#include <stdbool.h>
#define NOTRUN 0
#define RUNING 1
#define PAUSED 2
#define EXITED 3

struct ProcessControlBlock
{
	char *	command;
	char **  args;
	pid_t	PID;
	int	status;
};

void LaunchProcess(struct ProcessControlBlock *launch);
void FreePCBs(struct ProcessControlBlock *Processes);
void PrintPCB(struct ProcessControlBlock *print);
//signal declaration
void sigalarm_handler(int signum);
void sigchild_handler(int signum);
sigset_t sigset_alarm;
sigset_t sigset;
int nextProcess = 0;
// this function counts how many lines are in the file
int line_count(char *filechar);
int line_count(char *filechar)
{
	FILE *fptr = fopen(filechar, "r");
	int count = 0;
	char chr;
	chr = getc(fptr);
	while ( chr != EOF ) {
		if ( chr == '\n') {
			count++;
		}
		chr = getc(fptr);
	}
	printf("\nNumber of line: %d \n",count);
	return count+1;
}

// this function return the number of words in a line
int get_word_count(char line[], char hold[]);
int get_word_count(char line[], char hold[])
{
	int index, word_count = 0;
	index = p1getword(line, 0, hold);
	while (index != -1) {
		word_count++;
		index = p1getword(line, index, hold); //update index
	}
	return word_count;
}



struct ProcessControlBlock *processes = NULL;
int numProg;

int main(int argc, char *argv[])
{
	FILE *in_f = fopen(argv[1], "r");

	if (argc < 2) {
		fprintf(stderr, "No input file!!!\n");
		return 1;
	} 

	if (in_f == NULL) {
		fprintf(stderr, "Error! Cannot open file!!\n");
		return 1;
	}

	char line[256];
	char NextWord[256];
	int i,j,len,numWord;

	numProg = line_count(argv[1]);
	processes = (struct ProcessControlBlock *)malloc(sizeof(struct ProcessControlBlock) *numProg);
	// initialize
	for (i = 0; i < numProg; i++) {
		processes[i].PID = -1;
		processes[i].status = NOTRUN;
		//read in each line from input file, be careful with infinite loop tho!
		if (fgets(line, sizeof(line), in_f) != NULL) {
			printf("%s\n",line);
			//remove '/n' character
			len = strlen(line);
			if (line[len-1] == '\n') {
				line[len-1] = '\0';
			}
			numWord = get_word_count(line,NextWord);
			// allocating memory for each args in a line
			processes[i].args = (char **)malloc(sizeof(char*) * (numWord+1));
			// save each word into args
			p1getword(line,0,NextWord);
			//p1strcpy(processes[i].command, NextWord);
			processes[i].command = p1strdup(NextWord);
			int index = 0;
			for (j = 0; j < numWord; j++) {
				index = p1getword(line,index,NextWord);
				processes[i].args[j] = p1strdup(NextWord);
			}
			processes[i].args[j] = NULL;
		}
	}
	sigaddset(&sigset, SIGSTOP);
	sigemptyset(&sigset_alarm);
	sigaddset(&sigset_alarm, SIGALRM);

	for(i =0; i< numProg;i++)
	{	
		LaunchProcess(&processes[i]);
	}
		
	
	puts("\nPart 3\n");
	signal(SIGALRM, &sigalarm_handler);
	alarm(2);
	bool isStopAll =  false;
	//Check if all processes are exited
	while(1)
	{
		for(i =0; i< numProg;i++)
		{
			if(processes[i].status == PAUSED || processes[i].status == RUNING)
			{
				isStopAll = false;
				break;
			}
			isStopAll = true;
		}
		if(isStopAll == true)
		{
			break;	
		}
		
	}
	for(i =0; i< numProg;i++)
	{
		wait(NULL);
	}
	
	exit(0);
	//free all processes
	FreePCBs(processes);

	fclose(in_f);
	return 0;
}
void PrintPCB(struct ProcessControlBlock *print)
{
	//printf("Enter: %s\n",__FUNCTION__);
	printf("\tCMD: %s", print->command);
	//int i=0;
	//while(print->args[i] != NULL)
	//{
	//	i++;
	//}
	//printf("\tArgs: %d\n", i);
	//i =0;
	//while(print->args[i] != NULL)
	//{
	//	printf("\t\tArg: %s\n", print->args[i]);
	//	i++;
	//}
	//printf("\tPID: %d\n",print->PID);
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
	//printf("Exit: %s\n",__FUNCTION__);
}
void LaunchProcess(struct ProcessControlBlock *launch)
{
	printf("Enter: %s\n",__FUNCTION__);
	//todo:  Fork: Either run exec as child or save pid as parent.
	// launching all processes, going thru each processes an fork them
	pid_t pid = fork();	
	if (pid < 0) {
		printf("Forking failed!");
		exit(1);
	}
	else if(pid == 0)
	{
		
		printf("\nChild process \"%s\" pid %d start \n",launch->command, getpid());
		execvp(launch->command, launch->args);
		printf("\nChild process \"%s\" pid %d exit \n",launch->command, getpid());
		exit(1);
	}
	else
	{
		launch->status = RUNING;
		launch->PID = pid;
	}
	printf("Exit: %s\n",__FUNCTION__);	
}

void sigalarm_handler(int signum)
{
	//sigprocmask(SIG_BLOCK, &sigset_alarm, NULL);
	int status;	
	printf("2 senconds passed - Enter: %s\n",__FUNCTION__);
	
	int i;
	for(i =0; i< numProg;i++)
	{
			//Send sigkill to determine which process is terminated at the next 
			kill(processes[i].PID, SIGSTOP);
	}	

	for(i =0; i< numProg;i++)
	{
		waitpid(processes[i].PID , &status, WUNTRACED);
		//Unexited process is stopped by a signal
		if(WIFSTOPPED(status))
		{
			if(processes[i].status == RUNING)
			{			
				printf("Process: %d \"%s\" is paused\n",processes[i].PID, processes[i].command);
			}
			processes[i].status = PAUSED;	
		}
		else //Process is exited
		{
			processes[i].status = EXITED;	
		}
		printf("Process:%d \n",i);

	}	
	printf("Process check:%d \n",i);
	for(i =0; i< numProg;i++)
	{
		if(processes[nextProcess].status == PAUSED)
		{
			printf("Process is available :%d \n",i);
			break;
		}

		nextProcess ++;
		nextProcess = nextProcess % numProg;
	}	
	printf("Process check:%d",i);
	if(i == numProg)
	{
		printf("All processes are exited \n");
		//return;
	}
	processes[nextProcess].status = RUNING;
	printf("Process: %d \"%s\" is waked up\n",processes[nextProcess].PID, processes[nextProcess].command);
	kill(processes[nextProcess].PID, SIGCONT);
	nextProcess ++;
	nextProcess = nextProcess % numProg;	
	for(i =0; i< numProg;i++)
	{

			PrintPCB(&processes[i]);
	}	
	alarm(2);
	//sigprocmask(SIG_UNBLOCK, &sigset_alarm, NULL);
	//printf("Exit: %s\n",__FUNCTION__);
}
void FreePCBs(struct ProcessControlBlock *Processes)
{
	printf("Enter: %s\n",__FUNCTION__);
	int i =0; 
	for( i =0; i< numProg; i++)
	{	
		free(Processes[i].command);
		// todo: Cleanup Processes[i]
		int itr =0;
		for (itr =0; Processes[i].args[itr] != NULL; itr++)
		{			
			free(Processes[i].args[itr]);
		
		}
	}
	free(Processes);
	Processes=NULL;
	printf("Exit: %s\n",__FUNCTION__);
}
