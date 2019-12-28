#include<pthread.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include "sbuffer.h"
#include "connmgr.h"

#include "sensor_db.h"
#include  "datamgr.h"
#include <sys/poll.h>
#include <string.h>
#include<wait.h>
#include<time.h>



struct logEvent{
int seqNum;
time_t timestamp;
char * Logevent;

}typedef logEvent;




FILE *fptr;
   
int sharedData=0;
pthread_rwlock_t lock;

pthread_rwlock_t lock_log;


void writer(int seqNum,long int timestamp,char* LogEvent);

dplist_t * room_sensor_map;


int READ_BY_STORAGE=0;
int READ_BY_DATA=0;




int KILL_ALL=0;

dplist_t *shared_data;
 pthread_t conn_mgr_thread=0;
pthread_t data_mgr_thread=0;
pthread_t storage_mgr_thread=0;
    

DBCONN * conn;

void *conn_mgr(void *arg)
{
/* 
   pthread_rwlock_wrlock(&lock_log);

 char* msg= "Connection to SQL established"; 
     writer(0,34,msg);
pthread_rwlock_unlock(&lock_log);
 */



printf("this is the conn manager, running a server\n");

connmgr_listen(1234,&shared_data,&lock);
connmgr_free();


// to debug 
for(int i=0;i<dpl_size(room_sensor_map);i++){

sensor_node *sensorNode=dpl_get_element_at_index(room_sensor_map,i);

printf("index %i with sensor-ID %i has running average of %f and last time stamp was %lu\n",i,sensorNode->id,sensorNode->running_avg_t,sensorNode->last_modified_t);
displayArray(sensorNode->lastFive,RUN_AVG_LENGTH);

}






dpl_free(&room_sensor_map,true);
sbuffer_free(&shared_data);
disconnect(conn);
KILL_ALL=1;

//fclose(fptr);
printf("\n");

return NULL;
}


void *data_mgr(void *arg)
{

/* pthread_rwlock_wrlock(&lock_log);

 char* msg= "Connection to SQL established"; 
     writer(0,34,msg);

pthread_rwlock_unlock(&lock_log);

 */


FILE *fptr=fopen("room_sensor.map","r");

room_sensor_map=datamgr_parse_room_sensor(fptr);



for(int i=0;i<dpl_size(room_sensor_map);i++){

sensor_node *sensorNode=dpl_get_element_at_index(room_sensor_map,i);

printf("index %i with sensor-ID %i has running average of %f and last time stamp was %lu\n",i,sensorNode->id,sensorNode->running_avg_t,sensorNode->last_modified_t);
displayArray(sensorNode->lastFive,RUN_AVG_LENGTH);

}


printf("just opened the file \n");


//fclose(fptr);

while(1){
 
 if(KILL_ALL==1)
 {
   break;
 }

   sleep(1);


     pthread_rwlock_rdlock(&lock);

       if(dpl_size(shared_data)>0 && READ_BY_DATA!=1)
      {
         
       sensor_data_t * sensor_reading=(sensor_data_t * )dpl_get_element_at_index(shared_data,0);
       
       if(dpl_get_index_of_element(room_sensor_map,sensor_reading)!=-1)
       {
         printf("vuala! adding the bullshit now \n");
           
     int  index=dpl_get_index_of_element(room_sensor_map,sensor_reading);

     sensor_node * sensor_node_t=dpl_get_element_at_index(room_sensor_map,index);

    
shiftArray(sensor_node_t->lastFive,RUN_AVG_LENGTH,sensor_reading->value);

sensor_node_t->running_avg_t=averageCalculator(sensor_node_t->lastFive,RUN_AVG_LENGTH);

if((float)averageCalculator(sensor_node_t->lastFive,RUN_AVG_LENGTH)!=0){

//printf("run average is: %f\n",sensor_node_t->running_avg_t);

if((float)averageCalculator(sensor_node_t->lastFive,RUN_AVG_LENGTH)<=SET_MIN_TEMP)
{
  fprintf(stderr, "room-id %i is too cold at %lu\n", sensor_node_t->room_id,sensor_reading->ts);
}
if((float)averageCalculator(sensor_node_t->lastFive,RUN_AVG_LENGTH)>=SET_MAX_TEMP)
{
  fprintf(stderr, "room-id %i is too hot  at %lu\n", sensor_node_t->room_id,sensor_reading->ts);
}


}


sensor_node_t->last_modified_t=sensor_reading->ts; //updates time stamp



     READ_BY_DATA=1;
         
         if(READ_BY_STORAGE==1 && READ_BY_DATA==1)
         {
             dpl_remove_element(shared_data,dpl_get_element_at_index(shared_data,0),true);
         //    printf("Read by both  ---storage\n");

          //((sensor_node* )(dpl_get_element_at_index(room_sensor_map,dpl_get_index_of_element(room_sensor_map,reading))))->last_modified_t=ts;
             READ_BY_DATA=0;
             READ_BY_STORAGE=0;

         }
       }
       else
       {
         printf("Received sensor data with invalid sensor Node ID %i \n",sensor_reading->id);
       }
       
      }
    
 
     
    //printAll(shared_data);
    pthread_rwlock_unlock(&lock);
    
   }
return NULL;
}




void *storage_mgr(void *arg)
{
 
pthread_rwlock_wrlock(&lock_log);

 char* msg;
 msg="Connection to SQL established"; 
 writer(0,34,msg);
pthread_rwlock_unlock(&lock_log);
 


while(1){
 
 if(KILL_ALL==1)
 {
   break;
 }

   sleep(1);


     pthread_rwlock_rdlock(&lock);

       if(dpl_size(shared_data)>0 & READ_BY_STORAGE!=1 )
      {
          
       sensor_data_t * sensor_reading=(sensor_data_t * )dpl_get_element_at_index(shared_data,0);
       
       if(dpl_get_index_of_element(room_sensor_map,sensor_reading)!=-1)
       {
         printf("vuala! adding to database\n");

       sensor_data_t * reading =dpl_get_element_at_index(shared_data,0);
       int id=reading->id;
       double value=reading->value;
       time_t ts=reading->ts;  
       int read_by_storage=reading->read_by_storage_mgr; 
        
      
         insert_sensor(conn, id,value,ts);

         READ_BY_STORAGE=1;

         if(READ_BY_STORAGE==1 && READ_BY_DATA==1)
         {
             dpl_remove_element(shared_data,dpl_get_element_at_index(shared_data,0),true);
          //   printf("Read by both  ---data\n");
             READ_BY_DATA=0;
             READ_BY_STORAGE=0;
         }
       }
       else
       {
       //printf("Received sensor data with invalid sensor Node ID %i \n",sensor_reading->id);
       }
       
       

//       dataToModify->read_by_storage_mgr=1;

  //    ((sensor_data_t* )(dpl_get_element_at_index(shared_data,0)))->read_by_storage_mgr=1;  


        //modify_buffer(shared_data,reading,READ_BY_STORAGE_MGR,1);
    
     //if(reading->read_by_data_mgr==1 && reading->read_by_storage_mgr==1)
   //{
     //  dpl_remove_element(shared_data,reading,true);
        
      // printf("both have got the data -- sm\n");
   //} 
     
      }
    
 
     
    //printAll(shared_data);
    pthread_rwlock_unlock(&lock);
    
}
   
return NULL;
}




struct pollfd logFIFO[2];
int fd[2];

int main()
{

   

    
    int p=0;
    pipe(fd);

    p=fork();

  
  int    status;

   


logFIFO[0].fd = fd[0];
logFIFO[0].events = POLLIN;


logFIFO[1].fd = fd[1];
logFIFO[1].events =POLLOUT;

if(p==0)
{




do
{
printf("hello from child process\n");

 status = poll(logFIFO, 1, TIMEOUT*1000*10);

    if (status < 0)
    {
      perror("  poll() failed");
      break;
    }
    if (status == 0)
    {
      printf("server timeout...  End program.\n");
      break;
    }

      
  int i=0;

  logEvent log1;

  int result=-9;
  printf("I am a child\n");
  printf("Hello world. %d \n",getpid());
  while(1){
  //  for(int j=0;j<6;j++){
  result=read(logFIFO[0].fd,&log1,sizeof(logEvent));
    if(result>0){

             if(log1.timestamp==99999){
                 exit(0);
             }
         printf("%i\t%lu\t%s\n",log1.seqNum,log1.timestamp,log1.Logevent );
         
         fptr = fopen("gateway.log","a");
        if(fptr == NULL)
        {
         printf("Error!");               
         }
         fprintf(fptr,"%i\t%lu\t%s\n",log1.seqNum,log1.timestamp,log1.Logevent );
         fclose(fptr);
         //sleep(1);
         }
         }

}while(1);



}
else{

   
   

   sbuffer_init(&shared_data);
    
    conn=init_connection('1');

   pthread_rwlock_init(&lock,NULL);
   pthread_rwlock_init(&lock_log,NULL);
  
   
 
    pthread_create(&conn_mgr_thread,NULL,conn_mgr,NULL);
    pthread_create(&storage_mgr_thread,NULL,storage_mgr,NULL);
    pthread_create(&data_mgr_thread,NULL,data_mgr,NULL);
    
  
    pthread_join(conn_mgr_thread,NULL);
    pthread_join(storage_mgr_thread,NULL);
    pthread_join(data_mgr_thread,NULL);
   
    pthread_rwlock_destroy(&lock);
    pthread_rwlock_destroy(&lock_log);
    
    
}
    return 0;
}




void writer(int seqNum,long int timestamp,char* LogEvent){
         logEvent log1;
        log1.seqNum=seqNum*1000;
        log1.timestamp=timestamp;
        log1.Logevent=LogEvent; 
        logFIFO[1].fd=fd[1];
        int seqNums=seqNum;
        long int timestamps=timestamp;
        char * logEvent=LogEvent;

        write(fd[1],&seqNums,sizeof(int));
        write(fd[1],&timestamps,sizeof(long int));
        write(fd[1],&logEvent,sizeof(logEvent));
        
        


}












/* 

sensor_node * sensor_reading=(sensor_node *)malloc(sizeof(sensor_node));
sensor_reading->id=sensor_id;


if(dpl_get_index_of_element(list,sensor_reading)==-1){ //unknown sensor 

//int sensor_id=sensorData_t->id;

 fprintf(stderr, "new sensor ID found with id %i", sensor_reading->sensor_id);

}


  if(dpl_get_index_of_element(room_sensor_map,sensor_reading)!=-1){
    
    
index=dpl_get_index_of_element(room_sensor_map,sensor_reading);

sensor_node * sensor_node_t=dpl_get_element_at_index(list,index);

sensor_data_t * sensor_data=dpl_get_element_at_index(list_data,i);

shiftArray(sensor_node_t->lastFive,RUN_AVG_LENGTH,sensor_data->value);

sensor_node_t->running_avg_t=averageCalculator(sensor_node_t->lastFive,RUN_AVG_LENGTH);

if((float)averageCalculator(sensor_node_t->lastFive,RUN_AVG_LENGTH)!=0){

//printf("run average is: %f\n",sensor_node_t->running_avg_t);

if((float)averageCalculator(sensor_node_t->lastFive,RUN_AVG_LENGTH)<=SET_MIN_TEMP)
{
  fprintf(stderr, "room-id %i is too cold at %lu\n", sensor_node_t->room_id,sensor_data->ts);
}
if((float)averageCalculator(sensor_node_t->lastFive,RUN_AVG_LENGTH)>=SET_MAX_TEMP)
{
  fprintf(stderr, "room-id %i is too hot  at %lu\n", sensor_node_t->room_id,sensor_data->ts);
}


}


sensor_node_t->last_modified_t=sensor_data->ts; //updates time stamp

  } */