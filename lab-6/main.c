#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "datamgr.h"
#include "lib/dplist.h"

int main(void)
{
  
        FILE * fp= fopen("room_sensor.map","r");
        FILE * fp2=  fopen("sensor_data","rb");
        datamgr_parse_sensor_files(fp,fp2);
        displayAll();
        
       printf("room id of the sensor is : %i\n",datamgr_get_room_id(112));
       printf("the running average of the sensor is : %f\n",datamgr_get_avg(21));
       
       printf("total number of sensors is %i\n",datamgr_get_total_sensors());
      
        
        datamgr_free();
}