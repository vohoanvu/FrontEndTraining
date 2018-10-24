#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "p1fxns.h"
#include <wordexp.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <proc/readproc.h>
#include <errno.h>

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

	struct ProcesseBlock
	{
		char *cmd;
		char **args;
		int status;
		pid_t proc_id;
	};

	char *buffer = (char *)malloc(sizeof(char)*1024);
	char line[256];
	int space_index,i,j,len,numProg = 0;
	char *ProcArray[256];
	pid_t pid[numProg];
	struct ProcessBlock *processes = NULL;
	
	//read in each line from input file
	while (fgets(line, sizeof(line), in_f) != NULL) {
		len = strlen(line);
		p1getline()
		//remove '/n' character 
		if (line[len-1] == '\n') {
			line[len-1] = '\0';
		}
		p1strcpy(buffer[numProg], line);
		for (i = 0; buffer[numProg][i] != '\0'; i++) {
			space_index = p1getword(buffer[numProg][i], 0, ProcArray[i]);
			space_index = p1getword(buffer[numProg][i], space_index, ProcArray[i]);
			space_index = p1getword(buffer[numProg][i], space_index, ProcArray[i]);
		}
		numProg++;
	}

	// going thru each processes and fork them
	for (j = 0; j < numProg; j++) {
		pid_t temp = fork();
		pid[i] = temp;

		if (temp < 0) {
			perror("Forking failed!");
			exit(1);
		}
		// child processes
		if (temp == 0) {
			// Im not sure if these 2 args below are of proper form?
			execvp(buffer[j][0], ProcArray[j]);
		}

	}

	// not sure what the fuck this loop does
	for (i = 0; i < numProg; i++) {
		wait(pid[i]);
	}
}