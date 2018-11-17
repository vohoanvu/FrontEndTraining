#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h> 

struct pub_process{
    pid_t pid;
    // pthread_t pub_tid;
    int pub_to_quacker[2];
    int quacker_to_pub[2];
};

struct sub_process{
    pid_t pid;
    // pthread_t sub_tid;
    int sub_to_quacker[2];
    int quacker_to_sub[2];
};

pid_t quacker_pid;

void print_pubs(struct pub_process pubs[], int num_pubs);
void print_subs(struct sub_process subs[], int num_subs);

int main(int argc, char * argv[]){

    printf("total args %d\n", argc);
    int i;
    for (i = 0; i < argc; i++)
    {
        printf("argv[%d] = %s\n", i, argv[i]);
    }

    if(argc != 3)
    {
        fprintf(stderr, "Error, bad args.\n USAGE: startQuacker [num_publisheds] [num_subscribers]\n");
    }

    int TOTAL_PUBS = atoi(argv[1]);
    printf("total pubs : %d\n", TOTAL_PUBS);

    int TOTAL_SUBS = atoi(argv[2]);
    printf("total subs : %d\n", TOTAL_SUBS);

    struct pub_process pubs[TOTAL_PUBS];
    struct sub_process subs[TOTAL_SUBS];

    // fork off all of the pub processes
    // keep a file desscriptor for each publisher
    pid_t pid;
    for (i  = 0; i < TOTAL_PUBS; i++)
    {
        pipe(pubs[i].pub_to_quacker);
        pipe(pubs[i].quacker_to_pub);
        pid = fork();
        
        // Safety check 
        if (pid < 0)
        {
            fprintf(stderr, "Fork Failed.\n");
            exit(0);
        }
        // Parent updates pubs data structure
        else if (pid > 0)
        {
            printf("Forked child with ID : %d\n", pid);
            pubs[i].pid = pid;
        }
        // child escepes the creation loop
        else if (pid == 0 )
        {
            break;
        }
    }

    // Now parent will spawn off subscribers
    if (pid > 0)
    {
        for (i  = 0; i < TOTAL_SUBS; i++)
        {
            pipe(subs[i].sub_to_quacker);
            pipe(subs[i].quacker_to_sub);
            pid = fork();
            
            // Safety check 
            if (pid < 0)
            {
                fprintf(stderr, "Fork Failed.\n");
                exit(0);
            }
            // Parent updates pubs data structure
            else if (pid > 0)
            {
                printf("Forked child with ID : %d\n", pid);
                subs[i].pid = pid;
            }
            // child escepes the creation loop
            else if (pid == 0 )
            {
                break;
            }
        }
    }

    if (pid > 0)
    {
        print_pubs(pubs, TOTAL_PUBS);
        print_subs(subs, TOTAL_SUBS);
        
        // Fork to create sever request handler.
        quacker_pid = fork();
        if (quacker_pid == 0)
        {
            // Quacker created. Excepvp the Quacker Server.
            
            int result = execvp("circular_buffer", "circular_buffer");
            fprintf(stderr, "Error creating server instance!");
            exit(0);
        }
        
        else if (quacker_pid > 0)
        {
            // Parent process waits for all child procs to complete
        }

        else if (quacker_pid < 0)
        {
            fprintf(stderr, "Error creating Quacker Server.");
            exit(0);
        }
    }

    // Wait for all child procs to complete to complete
    if (pid > 0){
        int status = 0;
        pid_t child_pid;
        while( (child_pid = wait(&status)) > 0 );
    }


    return 0;
}

void print_pubs(struct pub_process pubs[], int num_pubs)
{  
    int i;
    for(i = 0; i < num_pubs; i++)
    {
        fprintf(stdout, "pubs[%d].pid = %d\n", i, (int)pubs[i].pid);
    }
    printf("\n");
}
void print_subs(struct sub_process subs[], int num_subs)
{  
    int i;
    for(i = 0; i < num_subs; i++)
    {
        fprintf(stdout, "subs[%d].pid = %d\n", i, (int)subs[i].pid);
    }
    printf("\n");
}