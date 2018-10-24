#include <unistd.h>
#include <sys/types.h>    //NOTE: Added

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
