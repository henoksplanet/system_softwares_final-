#include"sensor_db.h"
#include <stdio.h>




int callbackfunc(void *NotUsed, int argc, char **argv, char **azColName) {
    NotUsed = 0;
    
    for (int i = 0; i < argc; i++) {

        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    
    printf("\n");
    
    return 0;
}

int main(){

DBCONN * conn=init_connection('1'); // connect to the database and override if clear flag is set to y or n results in appending


FILE *fPointer=fopen("sensor_data","rb");



//printf("file size is:%i\n",filesizeNum);

insert_sensor_from_file(conn,fPointer);

find_sensor_all(conn,callbackfunc);

printf("--------data filtered based on value--------\n");
find_sensor_by_value(conn, 18,callbackfunc);

printf("--------data filtered based on exceeding value--------\n");

find_sensor_exceed_value(conn,25,callbackfunc);

printf("--------data filtered based on timestamp-------\n");

find_sensor_by_timestamp(conn,1575646128 , callbackfunc);


printf("--------data filtered based exceeding timestamp-------\n");


find_sensor_after_timestamp(conn, 1575649067,callbackfunc);

disconnect(conn);

}


