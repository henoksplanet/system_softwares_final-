#define _POSIX_SOURCE
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#include<pthread.h>
#include<stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "sbuffer.h"
#include "connmgr.h"
#include "sensor_db.h"
#include  "datamgr.h"
#include <sys/poll.h>
#include <string.h>
#include<wait.h>
#include<time.h>



FILE *fptr; 

pthread_mutex_t lock;
pthread_mutex_t lock_log;

int logFIFO[2];

dplist_t * room_sensor_map;


logEvent log1;


int READ_BY_STORAGE=0;
int READ_BY_DATA=0;

int logProcess=0;   //contains the child processes
  
int KILL_ALL=0;

dplist_t *shared_data;
DBCONN * conn;


int PORT_NUMBER=0;



void *conn_mgr(void *arg)
{



      connmgr_listen(PORT_NUMBER,&shared_data,&lock,room_sensor_map,&lock_log,logFIFO);  

      connmgr_free();     //clears out all sockets 

     
      dpl_free(&room_sensor_map,true);  //free memory from the dataManager's internal dynamic

      sbuffer_free(&shared_data);   //clear the memory held by the internal buffer 

      disconnect(conn);   //disconnect the Database

      KILL_ALL=1;     //terminate all threads 
     

      printf("Goodbye!\n");
      kill(logProcess,SIGTERM);
      return NULL;
}


void *data_mgr(void *arg)
{


                              pthread_mutex_lock(&lock_log);
                                 //  time_t now=time(NULL);
                                  

                                   //char *msg="hello this is from the data manager";
             
                                  // writer(logFIFO,now,msg);
                 // free(msg);
                               pthread_mutex_unlock(&lock_log); 



  FILE *fptr=fopen("room_sensor.map","r");

  room_sensor_map=datamgr_parse_room_sensor(fptr);

// something to debug that all data's are added to the internal dynamic library 

  while(1){  //better would be to check if there is data in the buffer 
 
      if(KILL_ALL==1)    //kills the thread on the command of the connection thread when the server timeout is triggered 
        {
          break;
        }

        sleep(1);


       pthread_mutex_lock(&lock);

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
                              pthread_mutex_lock(&lock_log);
                                 
                                 time_t now=time(NULL);
                                 writer(logFIFO,now,3,sensor_node_t->id,sensor_node_t->running_avg_t);

                                 pthread_mutex_unlock(&lock_log);

                            }
                        if((float)averageCalculator(sensor_node_t->lastFive,RUN_AVG_LENGTH)>=SET_MAX_TEMP)   //value provided from command-line 
                            {
                              fprintf(stderr, "room-id %i is too hot  at %lu\n", sensor_node_t->room_id,sensor_reading->ts);
                              
                               pthread_mutex_lock(&lock_log);
                                 
                                 time_t now=time(NULL);
                                 writer(logFIFO,now,4,sensor_node_t->id,sensor_node_t->running_avg_t);

                                 pthread_mutex_unlock(&lock_log);
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
    
          pthread_mutex_unlock(&lock);   //unlocks once done with reading from the shared_data buffer
    
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

    pthread_mutex_lock(&lock);

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
    pthread_mutex_unlock(&lock);
    
}
   
return NULL;
}






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
 
  
    
    
    pipe(logFIFO);

    logProcess=fork();


  

  if(logProcess==0){  //child process 
        FILE *fptr;
         fptr = fopen("gateway.log","w");
            if(fptr == NULL)
              {
                printf("Error!");               
              }
        fclose(fptr);
        int seqCounter=0;
        logEvent log1;
        int result=-9;
         while(1){
       //  for(int j=0;j<6;j++){
         result=read(logFIFO[0],&log1,sizeof(logEvent));
         if(result>0){
        seqCounter++;
        
        switch (log1.logCode)
        {
          case 1: 
            // printf("%i\t%lu\tA sensor node with %i has opened a new connection\n",seqCounter,log1.timestamp,log1.logData );
            fptr = fopen("gateway.log","a");
            if(fptr == NULL)
              {
                printf("Error!");               
              }
            fprintf(fptr,"%i\t%lu\tA sensor node with %i has opened a new connection\n",seqCounter,log1.timestamp,log1.logData );
            fclose(fptr);
            break;
          case 2: 
            //printf("%i\t%lu\tA sensor node with %i has closed the connection\n",seqCounter,log1.timestamp,log1.logData );
             
            fptr = fopen("gateway.log","a");
            if(fptr == NULL)
              {
                printf("Error!");               
              }
            fprintf(fptr,"%i\t%lu\tA sensor node with %i has  closed the connection\n",seqCounter,log1.timestamp,log1.logData );
            fclose(fptr);
            break;
          case 3: 
            //printf("%i\t%lu\tThe sensor node with %i reports it's too cold (running avg temperature=%f\n",seqCounter,log1.timestamp,log1.logData,log1.extras);
             
            fptr = fopen("gateway.log","a");
            if(fptr == NULL)
              {
                printf("Error!");               
              }
            fprintf(fptr,"%i\t%lu\tThe sensor node with %i reports it's too cold (running avg temperature=%f\n",seqCounter,log1.timestamp,log1.logData,log1.extras);
            fclose(fptr);
            break;
          case 4: 
            //printf("%i\t%lu\tThe sensor node with %i reports it's too hot (running avg temperature=%f\n",seqCounter,log1.timestamp,log1.logData,log1.extras);
             
            fptr = fopen("gateway.log","a");
            if(fptr == NULL)
              {
                printf("Error!");               
              }
            fprintf(fptr,"%i\t%lu\tThe sensor node with %i reports it's too hot (running avg temperature=%f\n",seqCounter,log1.timestamp,log1.logData,log1.extras);
            fclose(fptr);
            break;
          case 5: 
          //  printf("%i\t%lu\tReceived sensor data with invalid sensor node ID %i\n",seqCounter,log1.timestamp,log1.logData);
             
            fptr = fopen("gateway.log","a");
            if(fptr == NULL)
              {
                printf("Error!");               
              }
            fprintf(fptr,"%i\t%lu\tReceived sensor data with invalid sensor node ID %i\n",seqCounter,log1.timestamp,log1.logData);
            fclose(fptr);
            break;
          case 6: 
           // printf("%i\t%lu\tConnection to SQL server established\n",seqCounter,log1.timestamp);
             
            fptr = fopen("gateway.log","a");
            if(fptr == NULL)
              {
                printf("Error!");               
              }
            fprintf(fptr,"%i\t%lu\tConnection to SQL server established\n",seqCounter,log1.timestamp);
            fclose(fptr);
            break;
          case 7: 
            // printf("%i\t%lu\tNew table sensorData created\n",seqCounter,log1.timestamp);
             
            fptr = fopen("gateway.log","a");
            if(fptr == NULL)
              {
                printf("Error!");               
              }
            fprintf(fptr,"%i\t%lu\tNew table sensorData created\n",seqCounter,log1.timestamp);
            fclose(fptr);
            break;
          case 8: 
            
            break;
          case 9: 
            
            //printf("%i\t%lu\tUnable to connect to SQL server\n",seqCounter,log1.timestamp);
             
            fptr = fopen("gateway.log","a");
            if(fptr == NULL)
              {
                printf("Error!");               
              }
            fprintf(fptr,"%i\t%lu\tUnable to connect to SQL server\n",seqCounter,log1.timestamp);
            fclose(fptr);
            break;
          default: 
           break;
        }
        

         }
         }

    }

        // End of code for the child process
      
      else{     // the Parent process containing with three threads running 


        /* time_t now=time(NULL);
    */
       /*  char* msg= "Database connection failed"; 
        writer(logFIFO,now,msg);
        writer(logFIFO,now,msg);
        writer(logFIFO,now,msg);
         */


      
            pthread_t conn_mgr_thread=0;
            pthread_t data_mgr_thread=0;
            pthread_t storage_mgr_thread=0;

            sbuffer_init(&shared_data);
    
            conn=init_connection('1',&lock_log,logFIFO);

            pthread_mutex_init(&lock,NULL);
            pthread_mutex_init(&lock_log,NULL);
  
            pthread_create(&conn_mgr_thread,NULL,conn_mgr,NULL);
            pthread_create(&storage_mgr_thread,NULL,storage_mgr,NULL);
            pthread_create(&data_mgr_thread,NULL,data_mgr,NULL);
    
  
             pthread_join(conn_mgr_thread,NULL);
            pthread_join(storage_mgr_thread,NULL);
            pthread_join(data_mgr_thread,NULL);
   
            pthread_mutex_destroy(&lock);
            pthread_mutex_destroy(&lock_log);

              

            wait(NULL);
          }
      return 0;
}






 