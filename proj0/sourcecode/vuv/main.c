// Authorship statement:
	// Author: Vu Vo
	// ID: 951437454
	// CIS 415 Project 0
/* This is my own work except that 
	my qsort() is from Stackoverflow.com and 
	GTF helped me with AddWordAList(), MallocAList()
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "anagram.h"

int main(int argc, char *argv[])
{	
	// open input file 
	FILE *infile, *outfile;
	if (argc > 3)
	{
		fprintf(stderr, "Invalid command\n");
		exit(0);
	}
	if (argc < 3)
	{
		if (argc == 2)	{
			infile = fopen(argv[1], "r");
		} else {
			infile = stdin;
		}
	}
	// exit if either file is invalid
	if (infile == NULL)
	{
		fprintf(stderr, "Input file is invalid\n");
		exit(0);
	}
	// open output file
	if (argc == 3)
	{
		outfile = fopen(argv[2], "w");
	} else {
		outfile = stdout;
	}
	if (outfile == NULL) 
	{
		fprintf(stderr, "Output file is invalid\n");
		exit(0);
	}

	/* while get line from input file
			add word to a list */
	char aWord[1024];
	//struct StringList *strList;
	struct AnagramList **anafam = (struct AnagramList **)malloc(sizeof(struct AnagramList*));
	*anafam = NULL;
	
	while (fgets(aWord, sizeof(aWord), infile) != NULL)
	{
		//printf("going into loop in main");
		// remove '\n' character
		size_t len = strlen(aWord);
  		if (len > 0 && aWord[len-1] == '\n') {
    		aWord[--len] = '\0';
 		}

		AddWordAList(anafam, aWord);
	}
	// print a list
	PrintAList(outfile,(*anafam));
	// free a list
	FreeAList(anafam);
	free(&(*anafam));
	// close input and output file
	fclose(infile);
	fclose(outfile);
}