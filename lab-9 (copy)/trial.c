#include<pthread.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include "sbuffer.h"
#include "connmgr.h"

#include "sensor_db.h"



pthread_rwlock_t lock;


void *data_mgr(void *arg)
{


//  while(1){


   sleep(1);
     pthread_rwlock_rdlock(&lock);

    pthread_rwlock_unlock(&lock);
    
  // }


return NULL;
}

void *conn_mgr(void *arg)
{

return NULL;
}


void *storage_mgr(void *arg)
{
//while(1){
 

   sleep(1);


     pthread_rwlock_rdlock(&lock);


    pthread_rwlock_unlock(&lock);
    
 //  }
return NULL;
}




int main()
{

  
   


   pthread_rwlock_init(&lock,NULL);

    pthread_t conn_mgr_thread;
  pthread_t data_mgr_thread;
  pthread_t storage_mgr_thread;
    
 
    pthread_create(&conn_mgr_thread,NULL,conn_mgr,NULL);
   

   pthread_create(&data_mgr_thread,NULL,data_mgr,NULL);
    
   pthread_create(&storage_mgr_thread,NULL,storage_mgr,NULL);
    
  
    pthread_join(conn_mgr_thread,NULL);
    pthread_join(data_mgr_thread,NULL);
    pthread_join(storage_mgr_thread,NULL);

    conn_mgr_thread=0;

    pthread_rwlock_destroy(&lock);
    
    //sbuffer_free(&shared_data);
    //disconnect(conn);
 
    return 0;
}