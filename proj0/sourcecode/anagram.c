// Authorship statement:
	// Author: Vu Vo
	// ID: 951437454
	// CIS 415 Project 0
/* This is my own work except that 
	my qsort() is from Stackoverflow.com and 
	GTF helped me with AddWordAList(), MallocAList()
*/
#include "anagram.h"

char *LowerCase(char *word);
char *letterSort(char *word);
int compare(const void *a, const void *b);

char *LowerCase(char *word)
{
	int diff = 'A' - 'a';
	char ch;
	int size = strlen(word);
	int i;
	for (i = 0; i < size; i++)
	{
		strncpy(&ch,word+i,1);
		if (ch >= 'A' && ch <= 'Z')
		{
			ch = ch - diff;
			memcpy(word+i,&ch,1);
		}
	}
	return word;
}
// my original sorting function
/*char *letterSort(char *word)
{
	int size = strlen(word);
	char temp;
	for (int i = 0; i < size-1; i++)
	{
		for (int j = i+1; j < size; j++)
		{
			int s = tolower(word[i]) - tolower(word[j]);
			if (s == 0)
				s = word[i] - word[j];
			if (s > 0)
			{
				temp = word[i];
				word[i] = word[j];
				word[j] = temp;
			}
		}
	}
	return word;
}*/

struct StringList *MallocSList(char *word)
{
	//create a new string list node
	struct StringList *list = (struct StringList *)malloc(sizeof(struct StringList));
	if (list != NULL)
	{
		list->Next = NULL;
		list->Word = (char *)malloc(strlen(word)+1);
		strcpy(list->Word,word);
	}
	return list;
}

void FreeSList(struct StringList **node)
{
	//free a string list, including all children
	struct StringList *current = *node;
	struct StringList *next;
	while (current != NULL)
	{
		next = current->Next;
		free(current->Word);
		free(current);
		current = next;
	}
}

void PrintSList(FILE *file,struct StringList *node)
{
	//format output to a file according to specification
	while (node != NULL)
	{
		fprintf(file, "\t%s\n", node->Word);
		node = node->Next;
	}
}

int SListCount(struct StringList *node)
{
	//return the number of strings in the string list
	int count = 0;
	while (node != NULL)
	{
		count++;
		node = node->Next;
	}
	return count;
}

struct AnagramList* MallocAList(char *word)
{
	//create a new anagram node, store the word as sorted char array
	// add S list node with the word
	struct AnagramList *AListnode = (struct AnagramList *)malloc(sizeof(*AListnode));
	if (AListnode != NULL)
	{
		AListnode->Words = MallocSList(word);
		AListnode->Next = NULL;
		AListnode->Anagram = (char *)malloc(strlen(word)+1);
		strcpy(AListnode->Anagram,word);
		AListnode->Anagram = LowerCase(AListnode->Anagram);
		//AListnode->Anagram = letterSort(AListnode->Anagram);
		qsort(AListnode->Anagram,strlen(AListnode->Anagram),1,compare);
	}
	return AListnode;
}

void FreeAList(struct AnagramList **node)
{
	//free an anagram list, including anagram children and string list words
	struct AnagramList *current = *node;
	struct AnagramList *next;
	while (current != NULL)
	{
		next = current->Next;
		free(current->Anagram);
		FreeSList(&current->Words);
		free(current);
		current = next;
	} 
}

void PrintAList(FILE *file,struct AnagramList *node)
{
	//format output to a file, print anagram list with words, according to specs
	while (node != NULL)
	{	
		int count = SListCount(node->Words);
		// do not print out anagram families whose size is 1
		if (count > 1) {
			fprintf(file, "%s:%d\n", node->Anagram, SListCount(node->Words));
			PrintSList(file,node->Words);
		}
		node = node->Next;
	}

}

void AddWordAList(struct AnagramList **node, char *word)
{
	//add a new word to the anagram list: search the list and add the word
	// search with a sorted lower case version of the word
	char strdup[1024];
	memset(strdup, '\0', sizeof(strdup));
	strcpy(strdup,word);
	//char *buffer = letterSort(LowerCase(strdup));
	char *buffer = LowerCase(strdup);
	qsort(buffer,strlen(buffer),1,compare);

	if (*node == NULL) {
		*node = MallocAList(word);
		return;
	}
	while (*node != NULL )
	{
		// if the anagram family already existed
		if (strcmp((**node).Anagram,buffer) == 0) {	
			struct StringList *thisNode = MallocSList(word);
			AppendSList(&(*node)->Words, thisNode);
			break;
		} else {
			if ((*node)->Next == NULL) {
				(*node)->Next = MallocAList(word);
				return;
			} else {
				node = &(*node)->Next;
			}
		}

	}
}
void AppendSList(struct StringList **head, struct StringList *node)
{
	// append a string node to the end/tail of a string list
	while (*head != NULL) {
		head = &(*head)->Next;
	}
	*head = &(*node);
}

int compare(const void *a, const void *b)
{
    return *(const char *)a - *(const char *)b;
}