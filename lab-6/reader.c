#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

typedef unsigned int sensor_id_t;
typedef long int sensor_value_t;    
typedef time_t sensor_ts_t;         // UTC timestamp as returned by time() - notice that the size of time_t is different on 32/64 bit machine

typedef struct {		//blueprint for the sensor data
  uint16_t id;
  double value;
  time_t ts;
} sensor_data_t;


int fileSize(char filename[]);

int main(void)
{
    FILE *file = fopen("sensor_data", "r");
    sensor_data_t * sensorNode;
    uint16_t id=0;
    double value=0;
    time_t ts=0;
   // sensor_data_t sensorNode;

   // fread(&id, sizeof(uint16_t), 1, file);
    
int counter=0;
   for(int i=0;i<fileSize("sensor_data");i=i+18){
     fseek ( file , i, SEEK_SET );
    fread(&id, sizeof(uint16_t), 1, file);
     fseek ( file , i+2, SEEK_SET );
    fread(&value, sizeof(double), 1, file);
    fseek ( file , i+10, SEEK_SET );
    fread(&ts, sizeof(time_t), 1, file);
     printf("id: %i  value: %f    time:%d\n",id,value,ts); 
     sensorNode-id=id;
     sensorNode.value=value;
     sensorNode.ts=ts;
    counter++;
   }



//printf("file size is :%i\n",fileSize("sensor_data"));
printf("counter value is:%i\n",counter);
   
   
   

    fclose(file);

 
    

}





int fileSize(char filename[]){
    FILE *fp;
    char ch;
    int size = 0;
    fp = fopen(filename, "r");
    if (fp == NULL)
        printf("\nFile unable to open ");
    else 
        //printf("\nFile opened ");
    fseek(fp, 0, 2);    /* file pointer at the end of file */
    size = ftell(fp);   /* take a position of file pointer un size variable */
    //printf("The size of given file is : %d\n", size);    
    fclose(fp);
    return size;
}