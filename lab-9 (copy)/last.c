#include<pthread.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include "sbuffer.h"
#include "connmgr.h"

#include "sensor_db.h"


int sharedData=0;
pthread_rwlock_t lock;

dplist_t *shared_data;
 pthread_t conn_mgr_thread=0;
pthread_t data_mgr_thread=0;
pthread_t storage_mgr_thread=0;
    

DBCONN * conn;

void *data_mgr(void *arg)
{


  while(1){


   sleep(1);
     pthread_rwlock_rdlock(&lock);
/* 
     
     if((sensor_data_t *)dpl_get_element_at_index(shared_data,dpl_size(shared_data)+1)!=NULL){
         sensor_data_t * reading=(sensor_data_t *)malloc(sizeof(sensor_data_t));
        reading=dpl_get_element_at_index(shared_data,dpl_size(shared_data)+1);
       modify_buffer(shared_data,reading,READ_BY_DATA_MGR,1);
    
     if(reading->read_by_data_mgr==1 && reading->read_by_storage_mgr==1)
   {
       dpl_remove_element(shared_data,reading,true);
     //  printf("both have got the data  --dm\n");
   }
   free(reading);
     } */
    
    
     
    pthread_rwlock_unlock(&lock);
    
   }


return NULL;
}

void *conn_mgr(void *arg)
{

printf("this is the conn manager, running a server\n");



connmgr_listen(1234,&shared_data,&lock);



connmgr_free(&shared_data,&conn);

printf("\n");

exit(0);
return NULL;
}


void *storage_mgr(void *arg)
{
while(1){
 

   sleep(1);


     pthread_rwlock_rdlock(&lock);

      if(dpl_size(shared_data)>0)
      {

    sensor_data_t * reading=(sensor_data_t *)malloc(sizeof(sensor_data_t));
        reading=dpl_get_element_at_index(shared_data,dpl_size(shared_data)+1);

         insert_sensor(conn, reading->id, reading->value, reading->ts);
          modify_buffer(shared_data,reading,READ_BY_STORAGE_MGR,1);
    
     if(reading->read_by_data_mgr==1 && reading->read_by_storage_mgr==1)
   {
       dpl_remove_element(shared_data,reading,true);
        
      // printf("both have got the data -- sm\n");
   }
      free(reading);
      }
    

     
    //printAll(shared_data);
    pthread_rwlock_unlock(&lock);
    
   }
return NULL;
}



dplist_t *shared_data;

int main()
{


   sbuffer_init(&shared_data);
  
   conn=init_connection('1'); // connect to the database and override if clear flag is set to y or n results in appending
   


   pthread_rwlock_init(&lock,NULL);

   
 
    //pthread_create(&conn_mgr_thread,NULL,conn_mgr,NULL);
   

   //pthread_create(&data_mgr_thread,NULL,data_mgr,NULL);
    
   //pthread_create(&storage_mgr_thread,NULL,storage_mgr,NULL);
    
  
   // pthread_join(conn_mgr_thread,NULL);
    //pthread_join(data_mgr_thread,NULL);
    //pthread_join(storage_mgr_thread,NULL);

   
    pthread_rwlock_destroy(&lock);
    
    sbuffer_free(&shared_data);
    disconnect(conn);
 
    return 0;
}