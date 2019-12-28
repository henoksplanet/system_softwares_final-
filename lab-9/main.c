#include "sbuffer.h"
#include "config.h"
#include "lib/dplist.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



#define TRUE 1
#define FALSE 0


int main()
{


dplist_t * buffer;


sbuffer_init(&buffer);


for(int i=0;i<2;i++){
   sensor_data_t * data1=(sensor_data_t *)(malloc(sizeof(sensor_data_t)));
   data1->id=i*20;
   data1->ts=i*123;
   data1->value=i*2;
     
 sbuffer_insert(buffer,data1);
 
}


sensor_data_t * data1=(sensor_data_t *)(malloc(sizeof(sensor_data_t)));
   data1->id=0;



modify_buffer(buffer,data1,READ_BY_DATA_MGR,TRUE);
modify_buffer(buffer,data1,READ_BY_STORAGE_MGR,TRUE);




free(data1);

printAll(buffer);

sbuffer_free(&buffer);
 
    
return 0;

}