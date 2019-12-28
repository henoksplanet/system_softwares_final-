#include "sbuffer.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


void printAll(sbuffer_t * sbuffer)
{





}

int main()
{
sbuffer_t * buffer;

sensor_data_t * data1=(sensor_data_t *)(malloc(sizeof(sensor_data_t)));
data1->id=101;
data1->ts=21331;
data1->value=98;

sbuffer_init(&buffer);

sbuffer_insert(buffer,data1);






sbuffer_remove(buffer,data1);

sbuffer_free(&buffer);


}

