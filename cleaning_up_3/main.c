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
#include <signal.h>




FILE *fptr; 

pthread_rwlock_t lock;
pthread_rwlock_t lock_log;


void writer(int seqNum,long int timestamp,char* LogEvent);

dplist_t * room_sensor_map;

int READ_BY_STORAGE=0;
int READ_BY_DATA=0;

int logProcess=0;   //contains the child processes
  
int KILL_ALL=0;

dplist_t *shared_data;
DBCONN * conn;


int PORT_NUMBER=0;



void *conn_mgr(void *arg)
{



      connmgr_listen(PORT_NUMBER,&shared_data,&lock,room_sensor_map);  

      connmgr_free();     //clears out all sockets 

      kill(logProcess, SIGKILL);  //kills the logProcess 

      dpl_free(&room_sensor_map,true);  //free memory from the dataManager's internal dynamic

      sbuffer_free(&shared_data);   //clear the memory held by the internal buffer 

      disconnect(conn);   //disconnect the Database

      KILL_ALL=1;     //terminate all threads 


      printf("Goodbye!\n");

      return NULL;
}


void *data_mgr(void *arg)
{



  FILE *fptr=fopen("room_sensor.map","r");

  room_sensor_map=datamgr_parse_room_sensor(fptr);

// something to debug that all data's are added to the internal dynamic library 

  while(1){  //better would be to check if there is data in the buffer 
 
      if(KILL_ALL==1)    //kills the thread on the command of the connection thread when the server timeout is triggered 
        {
          break;
        }

        sleep(1);


       pthread_rwlock_rdlock(&lock);

       if(dpl_size(shared_data)>0 && READ_BY_DATA!=1)         //only reads if there is data available in the buffer and isn't already read by the data manager thread
          {
         
             sensor_data_t * sensor_reading=(sensor_data_t * )dpl_get_element_at_index(shared_data,0);
       
             if(dpl_get_index_of_element(room_sensor_map,sensor_reading)!=-1)
                  {    
                   // printf("adding to the data-manager internal dynamic storage \n");
           
                    int  index=dpl_get_index_of_element(room_sensor_map,sensor_reading);

                    sensor_node * sensor_node_t=dpl_get_element_at_index(room_sensor_map,index);


                    shiftArray(sensor_node_t->lastFive,RUN_AVG_LENGTH,sensor_reading->value);

                    sensor_node_t->running_avg_t=averageCalculator(sensor_node_t->lastFive,RUN_AVG_LENGTH);

                    if((float)averageCalculator(sensor_node_t->lastFive,RUN_AVG_LENGTH)!=0){

                        if((float)averageCalculator(sensor_node_t->lastFive,RUN_AVG_LENGTH)<=SET_MIN_TEMP)    //value provided from command-line 
                            
                            {
                              
                              fprintf(stderr, "room-id %i is too cold at %lu\n", sensor_node_t->room_id,sensor_reading->ts);
                            }
                        if((float)averageCalculator(sensor_node_t->lastFive,RUN_AVG_LENGTH)>=SET_MAX_TEMP)   //value provided from command-line 
                            {
                              fprintf(stderr, "room-id %i is too hot  at %lu\n", sensor_node_t->room_id,sensor_reading->ts);
                            }


                                                                                            }
                      sensor_node_t->last_modified_t=sensor_reading->ts; //updates time-stamp

                      READ_BY_DATA=1;     //sets the flag read_by_data manager on

                      if(READ_BY_STORAGE==1 && READ_BY_DATA==1)   //removes the data from the buffer once it is read by both the storage manager and the data manager 
                          {    
             
                            dpl_remove_element(shared_data,dpl_get_element_at_index(shared_data,0),true);
                            
                            //Resets the read/write Flags
                            READ_BY_DATA=0;
                            READ_BY_STORAGE=0;

                          }
                   }
                  else
                      {

                         printf("Received sensor data with invalid sensor Node ID %i \n",sensor_reading->id);
       
                      }   
            }
    
          pthread_rwlock_unlock(&lock);   //unlocks once done with reading from the shared_data buffer
    
          }
        return NULL;
      }




void *storage_mgr(void *arg)
{



  while(1){
 
    if(KILL_ALL==1)
      {
          break;
      }

    sleep(1);

    pthread_rwlock_rdlock(&lock);

       if(((dpl_size(shared_data))>0) & (READ_BY_STORAGE!=1))        // checks if the data isn't already read by this thread and also if there is something to read on the buffer
      {
          
       sensor_data_t * sensor_reading=(sensor_data_t * )dpl_get_element_at_index(shared_data,0); // fetching the data from buffer
       
       if(dpl_get_index_of_element(room_sensor_map,sensor_reading)!=-1)     // if the data is from a room/sensor combination from the room_sensor.map file 
       {
       //  printf("vuala! adding to database\n");

       sensor_data_t * reading =dpl_get_element_at_index(shared_data,0);
       int id=reading->id;
       double value=reading->value;
       time_t ts=reading->ts;  
       
      
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
       //printf("Received sensor data with invalid sensor Node ID %i \n",sensor_reading->id);   //something that has to do with the threads 
       }
       
       

      }
    
 
     
    //printAll(shared_data);
    pthread_rwlock_unlock(&lock);
    
}
   
return NULL;
}




struct pollfd logFIFO[2];
int fd[2];

int main(int argc, char *argv[])
{

    if(argc<2){
        printf("please provide port number\n");
        printf("\nExample: './sensor_gateway 1234' to start a server on port 1234 \n\n");
        exit(-1);
      }
    else
      {
        PORT_NUMBER=atoi(argv[1]);
      }
 
  
  pipe(fd);

  logProcess=fork();
  
  int    status;

  logFIFO[0].fd = fd[0];
  logFIFO[0].events = POLLIN;

  logFIFO[1].fd = fd[1];
  logFIFO[1].events =POLLOUT;

  if(logProcess==0)            // code for the child process
    {

      do
        {

        logEvent log1;

        status = poll(logFIFO, 1, TIMEOUT*1000*10);

        if (status < 0)
            {
              perror("  poll() failed");
              break;
            }
        if (status == 0)
            {
              printf("No log... timeout\n");
              break;
            }

      

        int result=-2;
        while(1){
  
              result=read(logFIFO[0].fd,&log1,sizeof(logEvent));   //reads the fds if there is anything to read 
    
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
                  //fclose(fptr);
                        }
                }

        }while(1);



      }

      // End of code for the child process
      
      else{     // the Parent process containing with three threads running 

            pthread_t conn_mgr_thread=0;
            pthread_t data_mgr_thread=0;
            pthread_t storage_mgr_thread=0;

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

            
            
            wait(NULL);
          }
      return 0;
}




void writer(int seqNum,long int timestamp,char* LogEvent){
         /* logEvent log1;
        log1.seqNum=seqNum*1000;
        log1.timestamp=timestamp;
        log1.Logevent=LogEvent; */ 
        logFIFO[1].fd=fd[1];
        int seqNums=seqNum;
        time_t timestamps=timestamp;
        char * logEvent=LogEvent;

        write(fd[1],&seqNums,sizeof(int));
        write(fd[1],&timestamps,sizeof(long int));
        write(fd[1],&logEvent,sizeof(logEvent));
        
}
