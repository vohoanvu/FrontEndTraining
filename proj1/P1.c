#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <proc/readproc.h>
#include <errno.h>
#include "p1fxns.h"

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

// this function counts how many lines are in the file
int line_count(FILE *fptr);
int line_count(FILE *fptr)
{
	int count = 0;
	char chr;
	chr = getc(fptr);
	while ( chr != EOF ) {
		if ( chr == '\n') {
			count++;
		}
		chr = getc(fptr);
	}
	return count;
}

// this function return the number of words in a line
int get_word_count(char line[], char hold[]);
int get_word_count(char line[], char hold[])
{
	int index, word_count = 1;
	index = p1getword(line, 0, hold);
	while (index != -1) {
		word_count++;
		index = p1getword(line, index, hold); //update index
	}
	return word_count;
}

void PrintPCB(struct ProcessControlBlock *print);
void PrintPCB(struct ProcessControlBlock *print)
{
	//printf("Enter: %s\n",__FUNCTION__);
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
	//printf("Exit: %s\n",__FUNCTION__);
}

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
	int i,j,len,numProg,numWord;
	struct ProcessControlBlock *processes = NULL;

	numProg = line_count(in_f);
	processes = (struct ProcessControlBlock *)malloc(sizeof(struct ProcessControlBlock) *numProg);
	// initialize
	for (i = 0; i < numProg; i++) {
		processes[i].PID = NOTRUN;
		processes[i].status = -1;
		//read in each line from input file, be careful with infinite loop tho!
		if (fgets(line, sizeof(line), in_f) != NULL) {
			//remove '/n' character
			len = strlen(line);
			if (line[len-1] == '\n') {
				line[len-1] = '\0';
			}
			numWord = get_word_count(line,NextWord);
			// allocating memory for each args in a line
			processes[i].args = (char **)malloc(sizeof(char*) * numWord+1);
			// save each word into args
			int index = 0;
			for (i = 0; i < numWord; i++) {
				index = p1getword(line,index,NextWord);
				p1strcpy(processes[i].command, NextWord);
				p1strcpy(processes[i].args[i], NextWord);
			}
		}
	}
	// launching all processes, going thru each processes and fork them
	for (j = 0; j < numProg; j++) {
		pid_t temp = fork();
		processes[j].PID = temp;

		if (temp < 0) {
			printf("Forking failed!");
			exit(1);
		}
		// child processes
		if (temp == 0) {
			// Im not sure if these 2 args below are of proper form?
			execvp(processes[j].command, processes[j].args);
		}

	}
	// not sure what this loop does
	for (j = 0; j < numProg; j++) {
		wait(processes[j].PID);
	}

	//print all PCBs
	int p=0;
	for(p=0; p< numProg; p++)
	{
		PrintPCB(&processes[p]);
	}

	//free PCBS
	for (j = 0; j < numProg; j++) {
		free(processes[j].command);
		int itr =0;
		for (itr =0; processes[j].args[itr] != NULL; itr++) {
			free(processes[j].args[itr]);
		}
	}
	free(processes);
	processes = NULL;

	fclose(in_f);
	return 0;
}