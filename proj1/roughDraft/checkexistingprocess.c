#include <stdio.h> 
#include <stdlib.h> 
#include <sys/types.h>
#include <string.h>

char* checkExistingPIDCommand(pid_t PID);
void main()
{
	char* ret = checkExistingPIDCommand(4747);
	printf("string %s \n",ret);
}
char* checkExistingPIDCommand(pid_t PID)
{
	int length = snprintf( NULL, 0, "%d", PID );
	char* str = malloc( length + 1 );
	snprintf( str, length + 1, "%d", PID );
	char* psCommand = "ps -p ";
	char* ret = " > /dev/null";
	char* s = malloc(strlen(psCommand) + strlen(str) + strlen(ret) + 1);
	strcat(s,psCommand);
	strcat(s,str);
	strcat(s,ret);
	if(0 == system(s)) {
		return "process is running";
	   }
	   else {
	      return "process is notrunning";
	   }
	return s;
}
