#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>

#define NUMTOPICS 1
#define MAXENTRIES 8
#define QUACKSIZE 140

int circular_buffer;

pthread_t publisher;
pthread_t subscriber;

struct topicentry{
    int entrynum;
    struct timeval timestamp;
    int pubID;
    int message;
};


struct queue{
    pthread_mutex_t topic_lock;
    struct topicentry circular_buffer[MAXENTRIES];
    int in;
    int out;
    int topic_counter;
};

int enqueue(struct queue *Q_ptr, struct topicentry *new_post)
{
    pthread_mutex_lock(&Q_ptr -> topic_lock);
    if( (Q_ptr -> in + 1) % MAXENTRIES == Q_ptr -> out ) 
    {
        printf("\nQ_ptr -> in                      = %d", Q_ptr -> in);
        printf("\nMAXENTRIES                       = %d", MAXENTRIES);
        printf("\n(Q_ptr -> in + 1) mod MAXENTRIES = %d\n", ((Q_ptr -> in + 1) % MAXENTRIES));
        printf("\nQ_ptr -> out                     = %d\n", Q_ptr -> out);
        /* Buffer is full. 
        Return -1 and allow caller to try again. */
        pthread_mutex_unlock(&Q_ptr -> topic_lock);
        return -1;
    }

    Q_ptr -> topic_counter += 1;
    new_post -> entrynum = Q_ptr -> topic_counter;

    struct timeval time;
    gettimeofday(&time, NULL);
    new_post -> timestamp = time;

    new_post -> pubID = 0;

    Q_ptr -> circular_buffer[Q_ptr -> in] = *new_post;
    Q_ptr -> in = (Q_ptr -> in + 1) % MAXENTRIES;
    pthread_mutex_unlock(&Q_ptr -> topic_lock);
    return 0;
}

int dequeue(struct queue * Q_ptr)
{
    pthread_mutex_lock(&Q_ptr -> topic_lock);
     {
        /* Check if buffer is empty.
        Return -1 and allow caller to try again. */
        pthread_mutex_unlock(&Q_ptr -> topic_lock);
        return -1;
    }
    /*
    Move out to the next spot in the queue.
    */
}

int read_post(struct queue * Q_ptr, int lastentry)
{
    
}

void printtopicQ(struct queue *Q_ptr)
{
    printf("in  = %d\n", Q_ptr->in);
    printf("out = %d\n", Q_ptr->out);
}

void* pub()
{
    printf("\n Pub thread running.\n");
    
    pthread_t tid = pthread_self();
    printf("\n thread id = %d.\n", (int)tid);
    
    struct queue Topic_Q_ptr;
    printf("\n Created topic queue.\n");
    
    struct topicentry this_topic;
    printf("\n Created topic entry.\n");
    
    Topic_Q_ptr.in =0;
    Topic_Q_ptr.out = 0;

    printtopicQ(&Topic_Q_ptr);

    printf("\n Creating the topic lock.\n");

    if(pthread_mutex_init(&(Topic_Q_ptr.topic_lock), NULL) != 0)
    {
        printf("\n mutex init has failed\n");
    }
    
    for(int i = 0; i < ; i++)
    {
        printf("\n Creating a topic: %d\n", i);
        this_topic.message = (int)(tid) * 1000 + i;
        int indicator = enqueue(&Topic_Q_ptr, &this_topic);
        printtopicQ(&Topic_Q_ptr);
        while (indicator == -1)
        {
            pthread_yield();
            indicator = enqueue(&Topic_Q_ptr, &this_topic);
            printtopicQ(&Topic_Q_ptr);
        }
        printf("successfully added to the queue: %d\n", indicator);
    }
    pthread_mutex_destroy(&(Topic_Q_ptr.topic_lock));
    return NULL;

}

void* sub()
{
    /* Activate Read Topic Function

    Read Topic should receive the last topic that was read by sub 
    
    It will return the intdex of the next topic number that is readable */
}

void* del()
{
    /* Activate the deque function. */
}
int main(int argc, char *argsv[])
{
    int num_pubs = 1;
    int num_subs = 1;
    int error;

    error = pthread_create(&publisher, NULL, &pub, NULL);
    if (error != 0)
        printf("\n Thread can't be created : [%s]\n", strerror(error));

    // error = pthread_create(&subscriber, NULL, sub, NULL);
    // if (error != 0)
    //     printf("\n Thread can't be created : [%s]", strerror(error));
    
    pthread_join(publisher, NULL);
    return 0;
}
