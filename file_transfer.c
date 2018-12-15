#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include<math.h>
#include<string.h>
#include "rsa-sample.h"
#include <pthread.h>
#include <sys/time.h>
#define DEFAULT_BUFFER_SIZE 1000
#define CHUNK_SIZE 50
typedef struct producer_variables{
	int tid ; 
	int job_count ;
	int start_index ;
}producer_variable ;


// global variables 
char * file_array;
char * decrypted_file_array;
int file_size;
rsa_variable ** buffer ;
int buffer_pointer = 0 ;
int buffer_max_count ;
int producer_count ; 
int job_ended = 0 ;

pthread_mutex_t buffer_lock ; 
pthread_mutex_t producer_lock;

pthread_cond_t can_produce = PTHREAD_COND_INITIALIZER; // signaled when items are removed
pthread_cond_t can_consume = PTHREAD_COND_INITIALIZER; // signaled when items are added
pthread_barrier_t producer_barrier ; 
pthread_mutex_t mutex ; 

void rw_file (char* file_name , char mode ){
	FILE* fp = fopen(file_name, "rb");
	fseek(fp, 0L, SEEK_END);
	int sz = ftell(fp);
	file_size = sz;
	fseek(fp, 0L, SEEK_SET);
	file_array = (char *) malloc(sizeof(char) * sz);
	decrypted_file_array = (char *) malloc(sizeof(char) * sz);
	if(mode == 'r'){
		for (int i = 0 ; i < sz; i ++){
			fread(&file_array[i] , 1, 1 ,fp);
		}
		fclose(fp);
	} else {

	}
}
void* producer(void *arg) {
	producer_variable * p_variable = (producer_variable *) arg ;
	for(int i = 0 ; i<p_variable->job_count ; i ++){
		rsa_variable* variable = (rsa_variable*) malloc(sizeof(rsa_variable)); 
		variable->start_index = p_variable->start_index+ i*CHUNK_SIZE;
		for(int j = 0 ; j < CHUNK_SIZE ; j++){
			variable->message[j] = file_array[p_variable->start_index + j + i*CHUNK_SIZE];
		}
		variable->message[CHUNK_SIZE] = 0;
		encrypt(variable);
		pthread_mutex_lock(&buffer_lock );
		if(buffer_pointer == (buffer_max_count-1) ){
			pthread_cond_wait(&can_produce, &buffer_lock);
			pthread_mutex_unlock(&buffer_lock);
			continue;
		}
		buffer[buffer_pointer++] = variable;
		
		pthread_cond_signal(&can_consume);
		pthread_mutex_unlock(&buffer_lock);

	}
	pthread_mutex_lock(&mutex);	
	job_ended++ ;
	pthread_mutex_unlock(&mutex);	

	pthread_barrier_wait(&producer_barrier);
	pthread_mutex_lock(&buffer_lock);
	if(p_variable->tid== 0){
		pthread_cond_broadcast (&can_consume);
	}
	pthread_mutex_unlock(&buffer_lock);
}

void* consumer(void *arg) {

	while((job_ended!=producer_count) || (buffer_pointer != 0) ){
		pthread_mutex_lock(&buffer_lock);
		if((buffer_pointer == 0) && (job_ended!=producer_count)){
			pthread_cond_wait(&can_consume , &buffer_lock);
			pthread_mutex_unlock(&buffer_lock);
			continue;
		} else if((buffer_pointer == 0) && (job_ended==producer_count)){

            pthread_mutex_unlock(&buffer_lock);
			break;
		}

		rsa_variable * variable = buffer[--buffer_pointer];
		pthread_cond_signal(&can_produce);
	
		pthread_mutex_unlock(&buffer_lock);
		decrypt(variable);
		for(int i = 0 ; variable->decrypted[i]!=-1; i++){
			// printf("%c" , variable->decrypted[i]);
			decrypted_file_array[variable->start_index + i] = variable->decrypted[i];
		}
	}	
}

int are_they_the_same(){
	for(int i = 0 ; i < file_size;i++){
		if(file_array[i]!=decrypted_file_array[i]){
			return 0;
		}
	}
	return 1;
}

int main (int argc, char *argv[]) {
	struct timeval  tv1, tv2;
/* stuff to do! */
	int thread_count = atoi(argv[1]);
	rw_file(argv[2] , 'r');

	gettimeofday(&tv1, NULL);
	int job_count = file_size / CHUNK_SIZE;
	producer_count = thread_count/2;
	int job_per_thread = job_count/producer_count ; 
	int remaind_job_count = job_count ; 

	pthread_barrier_init(&producer_barrier , NULL , producer_count);

	pthread_mutex_init(&producer_lock , NULL);
	pthread_mutex_init(&mutex , NULL);

	int buffer_len = DEFAULT_BUFFER_SIZE / sizeof(rsa_variable*);
	buffer = (rsa_variable**) malloc(sizeof(rsa_variable) * buffer_len);
	printf("buffer len = %d\n" , buffer_len);
	buffer_max_count = buffer_len;

	rsa_init();	
	pthread_t threads[thread_count] ; 
	for(int i = 0 ; i < producer_count; i ++){
		producer_variable *variable = (producer_variable *)malloc(sizeof(producer_variable));
		if((remaind_job_count < job_per_thread) ||(job_per_thread == 0)){
			variable->job_count = remaind_job_count;

		} else {
			variable->job_count = job_per_thread; 
		}
		variable->tid = i;
		variable->start_index = job_per_thread * i * CHUNK_SIZE ; 
		pthread_create(&threads[i] , NULL ,producer ,  variable);

		remaind_job_count = remaind_job_count - job_per_thread ; 
		if(remaind_job_count < 0){
			remaind_job_count = 0 ;
		}
	}

	for(int i = (thread_count/2) ; i < thread_count; i ++){
		pthread_create(&threads[i] , NULL ,consumer,  NULL);
	}

	for(int i = 0 ; i < thread_count; i++){
		pthread_join(threads[i] , NULL);
	}
	gettimeofday(&tv2, NULL);


	printf ("Total time = %f seconds\n",
         (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
         (double) (tv2.tv_sec - tv1.tv_sec));
	// for (int i = 0 ; i < file_size ; i++){
	// 	printf("%c" , decrypted_file_array[i]);
	// }

	printf("\n");
	printf("are they the same ? %d\n" ,are_they_the_same());
	
	
	return 0;
}
