#include<pthread.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>


int sharedData=0;
pthread_rwlock_t lock;


void *data_mgr(void *arg)
{


int *incoming=(int *)arg;
printf("data_mgr\n");

for(int j=0;j<10;j++){
   // printf("\ncounter from data_mgr:%i\n",i);
    pthread_rwlock_wrlock(&lock);
    sharedData++;
    pthread_rwlock_unlock(&lock);
    sleep(4);
}




return NULL;
}

void *conn_mgr(void *arg)
{
    int *incoming=(int *)arg;
printf("data_mgr\n");

 for(int i=0;i<10;i++){
        sleep(1);
        pthread_rwlock_wrlock(&lock);
        printf("shared Integer value is: %i \n",sharedData);
        pthread_rwlock_unlock(&lock);
    }
    printf("\n");





return NULL;
}




int main()
{

   pthread_rwlock_init(&lock,NULL);

    pthread_t conn_mgr_thread;
    pthread_t data_mgr_thread;
    
    void *thread_result;
    int value;

    value=42;
    pthread_create(&conn_mgr_thread,NULL,conn_mgr,&value);

    pthread_create(&data_mgr_thread,NULL,data_mgr,&value);
   



    pthread_join(conn_mgr_thread,&thread_result);
    pthread_join(data_mgr_thread,&thread_result);
    
    pthread_rwlock_destroy(&lock);


    return 0;
}