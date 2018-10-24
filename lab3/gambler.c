#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	if(argc !=2)
		exit(0);
	int count = atoi(argv[1]);

	while(count >0)	
	{
		int dice = rand() % 11 + 2;  // from 2- 12.
		printf("PID: %d Dice Roll is : %d \n",getpid(),dice);
		if(dice == 7)
		{
			count--;
			kill(getpid(),SIGSTOP);
		}
		else
		{
			usleep(100000);
		}
	}
	return 0;
}
