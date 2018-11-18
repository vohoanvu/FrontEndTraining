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
    char message[QUACKSIZE];
};


struct queue{
    pthread_mutex_t topic_lock;
    struct topicentry circular_buffer[MAXENTRIES];
    int tail =0; //in
    int head =0; //out
    int topic_counter=0;
};

int enqueue(struct queue *Q_ptr, struct topicentry *new_post)
{
    pthread_mutex_lock(&Q_ptr->topic_lock);
    if( (Q_ptr->tail + 1) % MAXENTRIES == Q_ptr->head ) 
    {
        printf("\nQ_ptr->in                      = %d", Q_ptr->tail);
        printf("\nMAXENTRIES                       = %d", MAXENTRIES);
        printf("\n(Q_ptr->in + 1) mod MAXENTRIES = %d\n", ((Q_ptr->tail + 1) % MAXENTRIES));
        printf("\nQ_ptr->out                     = %d\n", Q_ptr->head);
        /* Buffer is full. 
        Return -1 and allow caller to try again. */
        pthread_mutex_unlock(&Q_ptr->topic_lock);
        return -1;
    }

    Q_ptr->topic_counter += 1;
    new_post->entrynum = Q_ptr->topic_counter;

    struct timeval time;
    gettimeofday(&time, NULL);
    new_post->timestamp = time;

    new_post->pubID = 0;

    Q_ptr->circular_buffer[Q_ptr->tail] = *new_post;
    Q_ptr->tail = (Q_ptr->tail + 1) % MAXENTRIES;
    pthread_mutex_unlock(&Q_ptr->topic_lock);
    return 0;
}

int dequeue(struct queue *Q_ptr)
{
    pthread_mutex_lock(&Q_ptr->topic_lock);
    if (Q_ptr->topic_counter <= 0) {
        /* Check if buffer is empty.
        Return -1 and allow caller to try again. */
        pthread_mutex_unlock(&Q_ptr -> topic_lock);
        return -1;
    }
    /*  
    Move out to the next spot in the queue.
    */
    int i = Q_ptr->head;
    int data = Q_ptr->circular_buffer[i].entrynum;
    Q_ptr->head = (i +1) % MAXENTRIES;
    Q_ptr->topic_counter--;
    pthread_mutex_unlock(&Q_ptr->topic_lock);
    return data;

}

int getentry(struct queue * Q_ptr, int lastentry,struct topicentry *t)
{
    pthread_mutex_lock(&Q_ptr -> topic_lock);
    if (Q_ptr->topic_counter <= 0) {
        /* Check if buffer is empty.
        Return -1 and allow caller to try again. */
        pthread_mutex_unlock(&Q_ptr -> topic_lock);
        return -1;
    }
    int i = Q_ptr->head;
    int j = Q_ptr->tail;
    // check if [lastentry+1] is in the queue
    if (Q_ptr->circular_buffer[i].entrynum == lastentry+1) {
        // it's there
        t = Q_ptr->circular_buffer[i]; // Is this even legal???
        pthread_mutex_unlock(&Q_ptr->topic_lock);
        return 1;
    } else { 
        // lastentry+1 was dequeued
        i++; // advancing along the queue
        //Q_ptr->head = (i+1) % MAXENTRIES;
        //Q_ptr->topic_counter--;
        // found something newer and return it
        if (Q_ptr->circular_buffer[i] != Q_ptr->circular_buffer[j]) {
            t = Q_ptr->circular_buffer[i];
            pthread_mutex_unlock(&Q_ptr->topic_lock);
            return Q_ptr->circular_buffer[i].entrynum;
        } else { 
            // at the end of the queue
            int negative = Q_ptr->circular_buffer[j].entrynum;
            pthread_mutex_unlock(&Q_ptr->topic_lock);
            return (-negative); // not sure if this is legal
        }
    }

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
    
    Topic_Q_ptr.tail =0;
    Topic_Q_ptr.head = 0;

    printtopicQ(&Topic_Q_ptr);

    printf("\n Creating the topic lock.\n");

    if(pthread_mutex_init(&(Topic_Q_ptr.topic_lock), NULL) != 0)
    {
        printf("\n mutex init has failed\n");
    }
    
    for(int i = 0; i < MAXENTRIES; i++)
    {
        printf("\n Creating a topic: %d\n", i);
        this_topic.entrynum = (int)(tid) * 1000 + i;
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
    printf("\n Sub thread running.\n");
    
    subscriber = pthread_self();
    printf("\n thread id = %d.\n", (int)tid);


    int getentry(struct queue * Q_ptr, int lastentry,struct topicentry *t);

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
