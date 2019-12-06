#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "lib/dplist.h"
#include "config.h"
#include<time.h>
#include "datamgr.h"


int filesize=0;

int extraSensors[100]={0};

int extraSensorCounter=0;

int fileSize(FILE *fPointer);
double averageCalculator(double array[],int size);
int checkArray(double array[],int size);
void displayArray(double array[],int size);
void shiftArray(double array[],int size,double newdata);
void displayAll();

sensor_node *copy_sensor_node(sensor_node *sensor_node_t) {
sensor_node *new_sensor_node = (sensor_node*)malloc(sizeof(sensor_node));
new_sensor_node->id =sensor_node_t->id;
new_sensor_node->last_modified_t=sensor_node_t->last_modified_t;
new_sensor_node->room_id = sensor_node_t->room_id;
return new_sensor_node;
}

void free_sensor_node(sensor_node **sensor_node_t) {
free(*sensor_node_t);
*sensor_node_t = NULL;
return;
}

int compare_sensor_node(sensor_node *sensor_value_t_1, sensor_node *sensor_value_t_2) {
   if((int)sensor_value_t_1->id > (int)sensor_value_t_2->id) return 1;
    if((int)sensor_value_t_1->id < (int)sensor_value_t_2->id) return -1;
    return 0;
}



dplist_t * datamgr_parse_room_sensor(FILE * fp_sensor_map){
 char room_id_t[3];
 char sensor_id_t[3];
 dplist_t * list;
    //sensor_node *e;
    //dplist_node_t *n;
 

list = dpl_create(  (void *(*)(void *))copy_sensor_node,
                    (void (*)(void **))free_sensor_node,
                    (int (*)(void *, void *))compare_sensor_node
                    );

int counter=-1;
  
FILE *fp = fp_sensor_map;
      if(fp == NULL) {
          perror("Unable to open file!");
         exit(1);
      }
     char * pch;
     
     char chunk[128];

     int i=0;

     while(fgets(chunk, sizeof(chunk), fp) != NULL) {
     
      pch = strtok (chunk, " ");           

  while (pch != NULL)
  {
    if (i%2==0){
    //printf("first chunck: ");
    strcpy(room_id_t, pch); 
    }  ;
    if (i%2!=0){
    strcpy(sensor_id_t, pch);
    counter++;
   // printf ("room-id:%s -- sensor-id:%s\n",room_id_t,sensor_id_t);
    //printf ("added :%i nodes\n",counter);  
     sensor_node *room = (sensor_node*) malloc(sizeof(sensor_node));
    room->room_id=atoi(room_id_t);
    room->id =atoi(sensor_id_t);
    room->running_avg_t=0;
    room->last_modified_t=time(NULL);


for(int i=0;i<RUN_AVG_LENGTH;i++)
{
    room->lastFive[i]=0;
    }


   list=dpl_insert_at_index(list, room, counter, false); 
    }  ;
    //printf ("%s\n",pch);
    pch = strtok (NULL, " "); 
    i++;
  }
         //fputs(chunk, stdout);    
    }
     fclose(fp);
return list;
}




dplist_t * datamgr_parse_sensor_data(FILE * fPointer){

 dplist_t * list2;
    //sensor_node *e;
    //dplist_node_t *n;
 
list2 = dpl_create(  (void *(*)(void *))copy_sensor_node,
                    (void (*)(void **))free_sensor_node,
                    (int (*)(void *, void *))compare_sensor_node
                    );

 
FILE *file =fPointer;

   
   // sensor_data_t sensorNode;

   // fread(&id, sizeof(uint16_t), 1, file);
    
   int counter=0;
   for(int i=0;i<filesize;i=i+18){
     sensor_data_t *sensorData= (sensor_data_t*) malloc(sizeof(sensor_data_t)); 

     fseek ( file , i, SEEK_SET );
    fread(&(sensorData->id), sizeof(uint16_t), 1, file);
     fseek ( file , i+2, SEEK_SET );
    fread(&(sensorData->value), sizeof(double), 1, file);
    fseek ( file , i+10, SEEK_SET );
    fread(&(sensorData->ts), sizeof(time_t), 1, file);

  // printf("id: %i  value: %f    time:%d\n",id,value,ts); 
  /* 
     sensorData->id=id;
     sensorData->value=value;
     sensorData->ts=ts; */

     dpl_insert_at_index(list2, sensorData, counter, false); 
    //printf("id: %i  value: %f    time:%d   index:%i\n",sensorData->id,sensorData->value,sensorData->ts,counter); 
     
     counter++;

    }  ;
 
 printf ("counter :%i\n",counter);
     fclose(file);

return list2;
}

int fileSize(FILE * fPointer){
  
    FILE *fp=fPointer;
    
    int size = 0;
   
    if (fp == NULL)
        printf("\nFile unable to open ");
    else 
        //printf("\nFile opened ");
    fseek(fp, 0, 2);    /* file pointer at the end of file */
    size = ftell(fp);   /* take a position of file pointer un size variable */
    //printf("The size of given file is : %d\n", size);    
    fclose(fp);

    //printf("size-8 :%i\n",size);
    return size;
}


dplist_t * list;

dplist_t * list_data;


void datamgr_parse_sensor_files(FILE * fp_sensor_map, FILE * fp_sensor_data)

{

 FILE * fPointerSize=fopen("sensor_data","rb");
filesize=fileSize(fPointerSize);

list=datamgr_parse_room_sensor(fp_sensor_map);
list_data=datamgr_parse_sensor_data(fp_sensor_data);

int index=0;
for(int i=0;i<dpl_size(list_data);i++){

if(dpl_get_index_of_element(list,dpl_get_element_at_index(list_data,i))==-1){ //unknown sensor 

sensor_data_t *sensorData_t=dpl_get_element_at_index(list_data,i);

//int sensor_id=sensorData_t->id;

 fprintf(stderr, "new sensor ID found with id %i", sensorData_t->id);

}


  if(dpl_get_index_of_element(list,dpl_get_element_at_index(list_data,i))!=-1){
index=dpl_get_index_of_element(list,dpl_get_element_at_index(list_data,i));

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
  fprintf(stderr, "room-id %i is too hot\n", sensor_node_t->room_id);
}


}


sensor_node_t->last_modified_t=sensor_data->ts; //updates time stamp

  }
}

}



/* 
int main() {

FILE * fPointer=fopen("room_sensor.map","r");
FILE * fPointerData=fopen("sensor_data","rb");


datamgr_parse_sensor_files(fPointer,fPointerData);
 */
/* int size=5;    
int array[5]={2,3,4,5,5};

printf("average of array :%f",averageCalculator(array,size));
 */







//printf("size of the second list is: %i\n",dpl_size(list_data));
   

//double newTemp=0;
//for(int init=0;init<dpl_size(list_data);init++){

//if(dpl_get_index_of_element(list,dpl_get_element_at_index(list_data,init))!=-1){
//int index=dpl_get_index_of_element(list,dpl_get_element_at_index(list_data,init));

//sensor_node * sensor_node_t=dpl_get_element_at_index(list,index);

//sensor_data_t * sensor_data=dpl_get_element_at_index(list,init);

//newTemp=sensor_data->value;
//shiftArray(sensor_node_t->lastFive,5,newTemp);
//sensor_node_t->running_avg_t=averageCalculator(sensor_node_t->lastFive,5);
//}

//}

//for(int i=0;i<dpl_size(list);i++){

//sensor_node *sensorNode=dpl_get_element_at_index(list,i);

//printf("index %i has running average of %f\n",i,sensorNode->running_avg_t);


//}



//sensor_data_t * sensor_data=dpl_get_element_at_index(list_data,1);

//shiftArray(sensor_node_t->lastFive,5,sensor_data->value);
//shiftArray(sensor_node_t->lastFive,5,3);


/* printf("the last value (0)is: %f\n",sensor_node_t->lastFive[0]);

printf("the last value (1)is: %f\n",sensor_node_t->lastFive[1]);

printf("the last value (2): %f\n",sensor_node_t->lastFive[2]);

printf("the last value (3): %f\n",sensor_node_t->lastFive[3]);

printf("the last value (4): %f\n",sensor_node_t->lastFive[4]);

 *///sensor_node *sensorNode=dpl_get_element_at_index(list,0);
//sensor_node_t->running_avg_t=averageCalculator(sensor_node_t->lastFive,5);

//printf("\nindex 0 has running average of %f\n",sensor_node_t->running_avg_t);


/* 




// start of test
int index=dpl_get_index_of_element(list,dpl_get_element_at_index(list_data,1));
printf("index of element with that id is:%i\n",index);

sensor_node * sensor_node_t_2=dpl_get_element_at_index(list,index);


sensor_node * sensor_node_t=dpl_get_element_at_index(list,-8);

printf("first data:%f\n",sensor_node_t->lastFive[1]);
sensor_node_t->lastFive[0]=1;
sensor_node_t->lastFive[1]=2;
sensor_node_t->lastFive[2]=3;
sensor_node_t->lastFive[3]=4;
sensor_node_t->lastFive[4]=5;


printf("status of the array:%i\n",checkArray(sensor_node_t->lastFive,5));

double arraylist[5]={0,1,2,3,4};
double newarray[5];



displayArray(sensor_node_t->lastFive,5);


shiftArray(sensor_node_t->lastFive,5,6);
displayArray(sensor_node_t->lastFive,5);



 */

// end of test



//printf("\nthe first id is : %i",sensor_node_t->lastFive[1]);
   
//sensor_data_t * sensor_node_t1=dpl_get_element_at_index(list_data,89);


//intf("\nthe first id is : %f",sensor_node_t1->value);

 //sensor_node_t * sensorData;

/*  int room_id_t; 
 int sensor_id_t;
 double value;
 for(int i=0;i<dpl_size(list_data);i++){
 
 sensorData=dpl_get_element_at_index(list_data,i);
 room_id_t=sensorData->id;
 value=sensorData->value;

 printf("sensor-id:%i room-value:%f \n",room_id_t,value);
 } */
 

  /* assert(dpl_size(list)==8);


   dpl_free(&list, false);
    return 0;
} */



double averageCalculator(double array[],int size){

if(checkArray(array,size)!=-1){
  return 0;
}

double sum=0;
for(int i=0;i<size;i++){
sum=sum+array[i];
}
double average=sum/size;
return average;
}



int checkArray(double array[],int size){

for(int i=0;i<size;i++){
  if(array[i]==0){   //if the shit is not full yet 
    return i;
  }
}
return -1;
}



void shiftArray(double array[],int size,double newdata){
//double b[size];



int n=0;
for(int i=0;i<size-1;i++){
array[n]=array[n+1];
n++;
}
array[size-1]=newdata;
//return b;

}

void displayAll(){

for(int i=0;i<dpl_size(list);i++){

sensor_node *sensorNode=dpl_get_element_at_index(list,i);

printf("index %i with sensor-ID %i has running average of %f and last time stamp was %lu\n",i,sensorNode->id,sensorNode->running_avg_t,sensorNode->last_modified_t);
displayArray(sensorNode->lastFive,RUN_AVG_LENGTH);

}

}




void displayArray(double array[],int size){
for(int i=0;i<size;i++){
printf("%f,",*(array + i));
}
printf("\n\n");
}


void datamgr_free(){

dpl_free(&list,true);
dpl_free(&list_data,true);

}



uint16_t datamgr_get_room_id(sensor_id_t sensor_id){
int index=0;
int room_id=0;
sensor_data_t *sensorData=(sensor_data_t *)malloc(sizeof(sensor_data_t));

sensorData->id=sensor_id;
sensorData->ts=0;
sensorData->value=0;


if(dpl_get_index_of_element(list,sensorData)==-1 || sensor_id<0){
free(sensorData);
return 0;
}

index=dpl_get_index_of_element(list,sensorData);
sensor_node *sensorNode=dpl_get_element_at_index(list,index);

room_id=sensorNode->room_id;
free(sensorData);
return room_id;

}

sensor_value_t datamgr_get_avg(sensor_id_t sensor_id){

int index=0;
double average=0;
sensor_data_t *sensorData=(sensor_data_t *)malloc(sizeof(sensor_data_t));

sensorData->id=sensor_id;
sensorData->ts=0;
sensorData->value=0;


if(dpl_get_index_of_element(list,sensorData)==-1 || sensor_id<0){
free(sensorData);
return 0;
}

index=dpl_get_index_of_element(list,sensorData);
sensor_node *sensorNode=dpl_get_element_at_index(list,index);

average=sensorNode->running_avg_t;
free(sensorData);
return average;



}


time_t datamgr_get_last_modified(sensor_id_t sensor_id){

int index=0;
time_t last_modified_t=0;
sensor_data_t *sensorData=(sensor_data_t *)malloc(sizeof(sensor_data_t));

sensorData->id=sensor_id;
sensorData->ts=0;
sensorData->value=0;


if(dpl_get_index_of_element(list,sensorData)==-1 || sensor_id<0){
free(sensorData);
return 0;
}

index=dpl_get_index_of_element(list,sensorData);
sensor_node *sensorNode=dpl_get_element_at_index(list,index);

last_modified_t=sensorNode->last_modified_t;
free(sensorData);
return last_modified_t;

}



int datamgr_get_total_sensors(){


return dpl_size(list);

}
/* 

int arraySize()
{

for(int i=0;i<10;i++)
{

if(extraSensors[i]==0)
{
  return i;
}

} */
