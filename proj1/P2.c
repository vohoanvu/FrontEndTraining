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

//signal declaration
sigset_t sigset;

// this function counts how many lines are in the file
int line_count(char *filechar);
int line_count(char *filechar)
{
	FILE *fptr = fopen(filechar, "r");
	int count = 1;
	char chr;
	chr = getc(fptr);
	while ( chr != EOF ) {
		if ( chr == '\n') {
			count++;
		}
		chr = getc(fptr);
	}
	printf("\nNumber of line: %d \n",count);
	return count;
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
	
	sigaddset(&sigset, SIGUSR1);
	sigaddset(&sigset, SIGSTOP);
	sigaddset(&sigset, SIGCONT);
	//Block signals
	sigprocmask(SIG_BLOCK,&sigset,NULL);
	for(i =0; i< numProg;i++)
	{	
		LaunchProcess(&processes[i]);
	}
	
	puts("\nPart 2\n");
	for(i =0; i< numProg;i++)
	{	
		//Send signal to start a process 
		printf("\nSend SIGUSR1 from %d to %d \n",getpid() ,processes[i].PID);
		kill(processes[i].PID, SIGUSR1);
		processes[i].status = RUNING;
	}
	for(i =0; i< numProg;i++)
	{	
		printf("\nSend SIGSTOP from %d to %d \n",getpid() ,processes[i].PID);
		kill(processes[i].PID, SIGSTOP);
		processes[i].status = PAUSED;	
	}

	for(i =0; i< numProg;i++)
	{
		printf("\nSend SIGCONT from %d to %d \n",getpid() ,processes[i].PID);
		kill(processes[numProg - 1 - i].PID, SIGCONT);		
		processes[i].status = RUNING;	
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
		
		printf("\nChild process %s pid %d start \n",launch->command, getpid());
		int sig;
		//Raise a signal to parent to update state

		sigwait(&sigset, &sig);
		if(sig == SIGUSR1)
		{
			printf("\nChild process %s pid %d receive the signal SIGUSR1 %d \n",launch->command, getpid() , sig);
			execvp(launch->command, launch->args);
			
		}
		sigwait(&sigset, &sig);
		if(sig == SIGCONT)
		{
			
			printf("\nChild process %s pid %d receive the signal continue \n",launch->command, getpid());
		}
		exit(0);
	}
	else
	{
		launch->PID = pid;
		launch->status = RUNING;
	}
	printf("Exit: %s\n",__FUNCTION__);	
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
