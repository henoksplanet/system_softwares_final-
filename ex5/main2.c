
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "dplist.h"
#include <time.h>
#include "config.h"

int fd_container[200];


int now=120;

typedef int fd_number;

typedef struct sensor{
    sensor_id_t sensor_id;
    fd_number   fd_num;
    time_t  last_update;
}sensor;

sensor *copy_employee(sensor *old_sensor) {
    sensor *new_sensor = (sensor*)malloc(sizeof(sensor));
    new_sensor->sensor_id=old_sensor->sensor_id;
    new_sensor->fd_num = old_sensor->fd_num;
    new_sensor->last_update = old_sensor->last_update;
    
    return new_sensor;
}

void free_employee(sensor **sensor_to_free) {
    free(*sensor_to_free);
    *sensor_to_free = NULL;
    return;
}

int compare_employee(sensor *sensor1, sensor *sensor2) {
    if(sensor1->sensor_id > sensor2->sensor_id) return 1;
    if(sensor1->sensor_id < sensor2->sensor_id) return -1;
    return 0;
}


int* fd_set_return(dplist_t * sensor_list,int timeout);
dplist_t* modifiedDp(dplist_t** sensor_list,int timeout);
int modify_fd_set(dplist_t ** sensor_list);


int main() {
    

    fd_container[0]=3;


    dplist_t * list;
    //Employee *e;
    //dplist_node_t *n;
    
    sensor *sensor1 = (sensor*) malloc(sizeof(sensor));
    sensor1->sensor_id=192;
    sensor1->fd_num=1;
    sensor1->last_update=100;
    
     sensor *sensor2 = (sensor*) malloc(sizeof(sensor));
    sensor2->sensor_id=120;
    sensor2->fd_num=2;
    sensor2->last_update=90;
    
     sensor *sensor3 = (sensor*) malloc(sizeof(sensor));
    sensor3->sensor_id=123;
    sensor3->fd_num=3;
    sensor3->last_update=80;
    
     sensor *sensor4 = (sensor*) malloc(sizeof(sensor));
    sensor4->sensor_id=102;
    sensor4->fd_num=4;
    sensor4->last_update=70;
    
     sensor *sensor5 = (sensor*) malloc(sizeof(sensor));
    sensor5->sensor_id=213;
    sensor5->fd_num=5;
    sensor5->last_update=60;
    

    

    list = dpl_create(  (void *(*)(void *))copy_employee,
                        (void (*)(void **))free_employee,
                        (int (*)(void *, void *))compare_employee
                    );
    
    list=dpl_insert_at_index(list, sensor1, 0, false);
    list=dpl_insert_at_index(list, sensor2, 1, false);
    list=dpl_insert_at_index(list, sensor3, 2, false);
    list=dpl_insert_at_index(list, sensor4, 3, false);
    list=dpl_insert_at_index(list, sensor5, 4, false);



    
int index=0;
sensor *sensorvh;
long int last_updated;
int sensor_id;
int fd_numbers;
   for(int i=0;i<dpl_size(list);i++)
   {

       //index=dpl_get_element_at_index(list,i);

        sensorvh=(sensor *)dpl_get_element_at_index(list,i);
       last_updated=sensorvh->last_update;
       sensor_id=sensorvh->sensor_id;
       fd_numbers=sensorvh->fd_num;
     
       printf("last updated for index %i is %lu and has sensor_id %i of  and fd_num %i.\n",i,last_updated,sensor_id,fd_numbers);

   }


//fd_set_return(list,50);

modifiedDp(&list,70);
modify_fd_set(&list);



printf("------");

for(int i=0;i<dpl_size(list)+1;i++){
printf(" %i ",fd_container[i]);
}

printf("------\n"); 






printf("-------------------after removal------------------------ \n");

 for(int i=0;i<dpl_size(list);i++)
   {

       //index=dpl_get_element_at_index(list,i);

        sensorvh=(sensor *)dpl_get_element_at_index(list,i);
       last_updated=sensorvh->last_update;
       sensor_id=sensorvh->sensor_id;
       fd_numbers=sensorvh->fd_num;
     
       printf("last updated for index %i is %lu and has sensor_id %i of  and fd_num %i.\n",i,last_updated,sensor_id,fd_numbers);

   }


    dpl_free(&list, true);
    
    
    return 0;
}



dplist_t* modifiedDp(dplist_t** sensor_list,int timeout)
{
int index=0;
sensor *sensorvh;
long int last_updated;
int sensor_id;
int fd_numbers;
   for(int i=dpl_size(*sensor_list)-1;i>-1;i--)
   {
       sensorvh=(sensor *)dpl_get_element_at_index(*sensor_list,i);
       last_updated=sensorvh->last_update;
       sensor_id=sensorvh->sensor_id;
       fd_numbers=sensorvh->fd_num;
       if(last_updated<now-timeout)
      {
       dpl_remove_at_index(*sensor_list,i,false);   
       printf("sensor_id %i is timeed out\n",sensor_id);
      }     
   }

return *sensor_list;
}





int modify_fd_set(dplist_t ** sensor_list){

int index=0;
sensor *sensorvh;
long int last_updated;
int sensor_id;
int fd_numbers;

for(int i=0;i<dpl_size(*sensor_list);i++)
   {
       sensorvh=(sensor *)dpl_get_element_at_index(*sensor_list,i);
       fd_numbers=sensorvh->fd_num; 
      fd_container[i+1]=fd_numbers;
   }
}