#include <stdio.h>
#include <string.h>
#include "quacker.h"

struct queue Topic_Q_ptr[NUM_TOPICS];

void pubt_print(int tid, char * str)
{
    printf("\n[Pub thread %d] %s\n", tid, str);
}

void subt_print(int tid, char * str)
{
    printf("\n[Subscriber thread %d] %s\n", tid, str);
}

int enqueue(struct queue *Q_ptr, struct topicentry *new_post)
{
    pthread_mutex_lock(&Q_ptr->topic_lock);
    if( (Q_ptr->tail + 1) % MAX_ENTRIES == Q_ptr->head ) 
    {
        printf("\nQ_ptr->in                      = %d", Q_ptr->tail);
        printf("\nMAX_ENTRIES                       = %d", MAX_ENTRIES);
        printf("\n(Q_ptr->in + 1) mod MAX_ENTRIES = %d\n", ((Q_ptr->tail + 1) % MAX_ENTRIES));
        printf("\nQ_ptr->out                     = %d\n", Q_ptr->head);
        //Buffer is full. 
        //Return -1 and allow caller to try again. 
        pthread_mutex_unlock(&Q_ptr->topic_lock);
        return -1;
    }
    Q_ptr->topic_counter += 1;
    new_post->entrynum = Q_ptr->topic_counter;
    struct timeval time;
    gettimeofday(&time, NULL);
    new_post->timestamp = time;
    //new_post->pubID = 0;
    Q_ptr->circular_buffer[Q_ptr->tail] = *new_post;
    Q_ptr->tail = (Q_ptr->tail + 1) % MAX_ENTRIES;
    pthread_mutex_unlock(&Q_ptr->topic_lock);
    return 0;
}
int getentry(struct queue *Q_ptr, int lastentry,struct topicentry *t)
{
    pthread_mutex_lock(&Q_ptr -> topic_lock);
    if (Q_ptr->topic_counter <= 0) {
        //Check if buffer is empty.
        //Return -1 and allow caller to try again. 
        pthread_mutex_unlock(&Q_ptr -> topic_lock);
        return -1;
    }
    int i = Q_ptr->head;
    int j = Q_ptr->tail;
    // check if [lastentry+1] is in the queue
    if (Q_ptr->circular_buffer[i].entrynum == lastentry+1) {
        // it's there
        *t = Q_ptr->circular_buffer[i]; // Is this even legal???
        pthread_mutex_unlock(&Q_ptr->topic_lock);
        return 1;
    } else { 
        // lastentry+1 was dequeued
        i = (i + 1) % MAX_ENTRIES; // advancing along the queue
        // found something newer and return it
        //if (Q_ptr->circular_buffer[i] != Q_ptr->circular_buffer[j]) {
		if(i!=j){	
            *t = Q_ptr->circular_buffer[i];
            pthread_mutex_unlock(&Q_ptr->topic_lock);
            return Q_ptr->circular_buffer[i].entrynum;
        } else { 
            // at the end of the queue
            int negative = Q_ptr->circular_buffer[j].entrynum;
            pthread_mutex_unlock(&Q_ptr->topic_lock);
            return (-negative); // not sure if this is legal
        }
    } 
    return 1;
}
int dequeue(struct queue * Q_ptr)
{
    // pthread_mutex_lock(&Q_ptr -> topic_lock);
    //  {
    //     /* Check if buffer is empty.
    //     Return -1 and allow caller to try again. */
    //     pthread_mutex_unlock(&Q_ptr -> topic_lock);
    //     return -1;
    // }
    /*
    Move out to the next spot in the queue.
    */
   return 0;
}

int read_post(struct queue * Q_ptr, int lastentry)
{
    return 1;
}

void printtopicQ(struct queue *Q_ptr)
{
    printf("in  = %d\n", Q_ptr->in);
    printf("out = %d\n", Q_ptr->out);
}

void* pub(void * pub_info)
{
    // pub_info identifies the pub process we are listening to
    struct pub_process *pub_proxy = pub_info;

    int tid = (int)pthread_self();
    char output[140]; // for printing thread info
    char message[QUACK_SIZE]; // for capturing messages from our pub process
    char response[10]; // accept or reject request from publisher

    // close reading end of quacker_to_pub, writing end of pub_to_quacker
    close(pub_proxy -> quacker_to_pub[0]);
    close(pub_proxy -> pub_to_quacker[1]);

    pubt_print(tid, "Pub thread running.");

    sprintf(output, "Listening for publisher %d", pub_proxy -> pid);
    pubt_print(tid, output);

    read(pub_proxy -> pub_to_quacker[0], message, QUACK_SIZE);

    sprintf(output, "pub proxy caught message : %s", message);
    pubt_print(tid, output);

    // Look for connection request in message
    if( strstr(message , "connect") != NULL )
    {
        pubt_print(tid, "sending 'accept'");
        write(pub_proxy -> quacker_to_pub[1], "accept", 7);
    }
    else // invalid message recieved, shut down thread
    {
        write(pub_proxy -> quacker_to_pub[1], "reject", 7);
        close(pub_proxy -> quacker_to_pub[1]);
        close(pub_proxy -> pub_to_quacker[0]);
        return NULL;
    }
   
    pubt_print(tid, "waiting for message");
    read(pub_proxy -> pub_to_quacker[0], message, QUACK_SIZE);
    sprintf(output, "got message: %s", message);
    pubt_print(tid, output);
    while( strstr(message, "terminate") == NULL )
    {
        if( strcmp(message, "end") == 0 )// end of connection protocol
        {
            sprintf(response, "accept");   
            sprintf(output, "Sending Response: %s", response);
            pubt_print(tid, output);
            write(pub_proxy -> quacker_to_pub[1], response, strlen(response)+1);
        }
        //else if(0){ //  message is valid style, pub <pubID> <topic> <message> end
        else{
		// post the message to the topic
			printf("\n Creating a topic: \n");
			 //this_topic.message = (int)(tid) * 1000 + i;
			struct topicentry this_topic; 
			int  i;
			int  j;
			for (i = 0; i < QUACK_SIZE; i++) {
				this_topic.message[i] = message[i];
			}
			this_topic.pubID =  (int)pub_proxy->pid;
			for (i = 0; i < NUM_TOPICS; i++) {
				//If there is a existing topic queue => add topicentry to this queue
				for (j = 0; j < QUACK_SIZE; j++) {
					if(this_topic.message[j] != Topic_Q_ptr[i].message[j]){
						continue;
					}
					else{
						break;
					}
				}
			}
			if(i == NUM_TOPICS) //No queue keep this topic
			{
				for (i = 0; i < NUM_TOPICS; i++){
					if(Topic_Q_ptr[i].topic_counter == 0)
					{
						break;
					}
				}
			}
			int indicator = enqueue(&Topic_Q_ptr[i], &this_topic);
			printtopicQ(&Topic_Q_ptr[i]);
			 /*while (indicator == -1)
			 {
				 pthread_yield_np();
				 indicator = enqueue(&Topic_Q_ptr, &this_topic);
				 printtopicQ(&Topic_Q_ptr);
			 }*/
			printf("successfully added to the queue: %d\n", indicator);
            sprintf(response, "accept");
            sprintf(output, "Sending Response: %s", response);
            pubt_print(tid, output);
            write(pub_proxy -> quacker_to_pub[1], response, strlen(response)+1);
        }
        /*else{
            // message is improperly formatted
            sprintf(response, "reject");
            sprintf(output, "Sending Response: %s", response);
            pubt_print(tid, output);
            write(pub_proxy -> quacker_to_pub[1], response, strlen(response)+1);
        }*/
        pubt_print(tid, "waiting for message");
        read(pub_proxy -> pub_to_quacker[0], message, QUACK_SIZE);
        sprintf(output, "got message: %s", message);
        pubt_print(tid, output);
    }
    sprintf(response, "terminate");   
    sprintf(output, "Sending Response: %s", response);
    pubt_print(tid, output);
    write(pub_proxy -> quacker_to_pub[1], response, strlen(response)+1);

    close(pub_proxy -> quacker_to_pub[1]);
    close(pub_proxy -> pub_to_quacker[0]);
    
    return NULL;
}

void* sub(void * sub_info)
{
    sleep(2);
    // sub_info identifies the sub process we are listening to
    struct sub_process *sub_proxy = sub_info;

    int tid = (int)pthread_self();
    char output[140]; // for printing thread info
    char message[QUACK_SIZE]; // for capturing messages from our sub process
    char response[10]; // accept or reject request from subscriber

    // close reading end of quacker_to_sub, writing end of sub_to_quacker
    close(sub_proxy -> quacker_to_sub[0]);
    close(sub_proxy -> sub_to_quacker[1]);

    subt_print(tid, "sub thread running.");

    sprintf(output, "Listening for subscriber %d", sub_proxy -> pid);
    subt_print(tid, output);

    read(sub_proxy -> sub_to_quacker[0], message, QUACK_SIZE);

    sprintf(output, "sub proxy caught message : %s", message);
    subt_print(tid, output);

    // Look for connection request in message
    if( strstr(message , "connect") != NULL )
    {
        subt_print(tid, "sending 'accept'");
        write(sub_proxy -> quacker_to_sub[1], "accept", 7);
    }
    else // invalid message recieved, shut down thread
    {
        write(sub_proxy -> quacker_to_sub[1], "reject", 7);
        close(sub_proxy -> quacker_to_sub[1]);
        close(sub_proxy -> sub_to_quacker[0]);
        return NULL;
    }
   
    subt_print(tid, "waiting for message");
    read(sub_proxy -> sub_to_quacker[0], message, QUACK_SIZE);
    sprintf(output, "got message: %s", message);
    subt_print(tid, output);
	int last_entry =0;
    while( strstr(message, "terminate") == NULL )
    {
		struct topicentry t;
        if( strcmp(message, "end") == 0 )// end of connection protocol
        {
            sprintf(response, "accept");   
            sprintf(output, "Sending Response: %s", response);
            subt_print(tid, output);
            write(sub_proxy -> quacker_to_sub[1], response, strlen(response)+1);
        }
		else{
			sprintf(response, "accept");
            sprintf(output, "Sending Response: %s", response);
			last_entry = getentry(&Topic_Q_ptr[0],last_entry,&t);
			
            subt_print(tid, output);
            write(sub_proxy -> quacker_to_sub[1], response, strlen(response)+1);
		}
        /*else if(0){ //  message is valid style, sub <subID> topic <topic> end

            sprintf(response, "accept");
            sprintf(output, "Sending Response: %s", response);
            subt_print(tid, output);
            write(sub_proxy -> quacker_to_sub[1], response, strlen(response)+1);
        }
        else{
            // message is improperly formatted
            sprintf(response, "reject");
            sprintf(output, "Sending Response: %s", response);
            subt_print(tid, output);
            write(sub_proxy -> quacker_to_sub[1], response, strlen(response)+1);
        }*/
        subt_print(tid, "waiting for message");
        read(sub_proxy -> sub_to_quacker[0], message, QUACK_SIZE);
        sprintf(output, "got message: %s", message);
        subt_print(tid, output);
    }
    sprintf(response, "terminate");   
    sprintf(output, "Sending Response: %s", response);
    subt_print(tid, output);
    write(sub_proxy -> quacker_to_sub[1], response, strlen(response)+1);

    close(sub_proxy -> quacker_to_sub[1]);
    close(sub_proxy -> sub_to_quacker[0]);
    
    return NULL;
}

void* del()
{
    /* Activate the deque function. */
    return NULL;
}

int circular_buffer(int num_pubs, struct pub_process pubs[], int num_subs, struct sub_process subs[])
{
    int error;

    int i;
    for(i = 0; i < NUM_TOPICS; i++)
    {
        Topic_Q_ptr[i].in = 0;
        Topic_Q_ptr[i].out = 0;
		Topic_Q_ptr[i].head = 0;
        Topic_Q_ptr[i].tail = 0;
        Topic_Q_ptr[i].topic_counter = 0;
    }

    printf("\n[circular_biffer] Creating the topic locks.\n");
    for(i = 0; i < NUM_TOPICS; i++)
    {
        if(pthread_mutex_init(&(Topic_Q_ptr[i].topic_lock), NULL) != 0)
        {
            printf("\n mutex init has failed\n");
        }
    }

    pthread_t publisher[num_pubs];
    for(i = 0; i < num_pubs; i++)
    {
        error = pthread_create(&publisher[i], NULL, &pub, &pubs[i]);
        if (error != 0)
            printf("\n Thread can't be created : [%s]\n", strerror(error));
    }

    pthread_t subscriber[num_subs];
    for(i = 0; i < num_subs; i++)
    {
        error = pthread_create(&subscriber[i], NULL, &sub, &subs[i]);
        if (error != 0)
            printf("\n Thread can't be created : [%s]\n", strerror(error));
    }
    
    // wait for publishers to finish
    for(i = 0; i < num_pubs; i++)
    {
        pthread_join(publisher[i], NULL);
    }

    // wait for subscribers to finish
    for(i = 0; i < num_subs; i++)
    {
        pthread_join(subscriber[i], NULL);
    }
    
    // destroy the locks
    for(i = 0; i < NUM_TOPICS; i++)
    {
        pthread_mutex_destroy(&(Topic_Q_ptr[i].topic_lock));
    }
    
    return 0;
}
