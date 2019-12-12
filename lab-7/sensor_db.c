#include "sensor_db.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>


int filesizeNum=0;

int fileSize(FILE *fPointer);


DBCONN * init_connection(char clear_up_flag){


	sqlite3* DB; 
   char *sql_flag_on =      "DROP TABLE IF EXISTS sensorData;"
                    "CREATE TABLE 	sensorData("
					"id INTEGER, "
					"sensor_id	INTEGER, "
					"sensor_value	REAL, "
					"timestamp	INTEGER,"
                    "PRIMARY KEY('id' AUTOINCREMENT)); ";
    char *sql_flag_off = "CREATE TABLE 	sensorData("
					"id INTEGER, "
					"sensor_id	INTEGER, "
					"sensor_value	REAL, "
					"timestamp	INTEGER,"
                    "PRIMARY KEY('id' AUTOINCREMENT)); ";
	int exit = 0; 
	exit = sqlite3_open("Sensor.db", &DB); 

	char* messaggeError="couldn't execute the sql command\n"; 

if(clear_up_flag=='1'){

	exit = sqlite3_exec(DB, sql_flag_on, NULL, 0, &messaggeError); 
}
else{
exit = sqlite3_exec(DB, sql_flag_off, NULL, 0, &messaggeError);

}
	if (exit != SQLITE_OK) { 
		printf("Error:%s \n",messaggeError); 
		sqlite3_free(messaggeError); 
	} 
	else
		printf("Table created Successfully\n") ; 
	//sqlite3_close(DB); 
	return DB;
   // return (0); 
}


void disconnect(DBCONN *conn){

if(sqlite3_close(conn)==SQLITE_OK){
    printf("successfully disconnected from the database\n");
}
else
{
    printf("error occured when trying to disconnect from the database\ns");
}


}



/*
 * Write an INSERT query to insert a single sensor measurement
 * Return zero for success, and non-zero if an error occurs
 */
int insert_sensor(DBCONN * conn, sensor_id_t id, sensor_value_t value, sensor_ts_t ts){

int rc;
char *err_msg = 0;
sensor_id_t id_t=id;
sensor_value_t value_t=value;
sensor_ts_t ts_t=ts;

char sensor_id[20];

char sensor_value[20];

char sensor_ts[20];


char sqlfinal[150];
char *sql_part_1 = "INSERT INTO sensorData (sensor_id,sensor_value,timestamp) VALUES(";

strcpy(sqlfinal,sql_part_1);

snprintf(sensor_id, 20, "%i", id_t);
strcat(sqlfinal,sensor_id);
strcat(sqlfinal,",");


snprintf(sensor_value, 20, "%f", value_t);
strcat(sqlfinal,sensor_value);
strcat(sqlfinal,",");


snprintf(sensor_ts, 20, "%lu", ts_t);
strcat(sqlfinal,sensor_ts);
strcat(sqlfinal,");");




    rc = sqlite3_exec(conn, sqlfinal, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "\nSQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);        
        return 1;
    } 
    return 0;

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
   

    //printf("size-8 :%i\n",size);
    return size;
}



int insert_sensor_from_file(DBCONN *conn, FILE * fPointer){
 

filesizeNum=fileSize(fPointer);



FILE *file =fPointer;
sensor_id_t id_t=0;
sensor_value_t value_t=0;
sensor_ts_t ts_t=0;


    
   int counter=0;
   for(int i=0;i<filesizeNum;i=i+18){
 
     fseek ( file , i, SEEK_SET );
    fread(&(id_t), sizeof(uint16_t), 1, file);
     fseek ( file , i+2, SEEK_SET );
    fread(&(value_t), sizeof(double), 1, file);
    fseek ( file , i+10, SEEK_SET );
    fread(&(ts_t), sizeof(time_t), 1, file);


    if(insert_sensor(conn,id_t, value_t, ts_t)!=0){
    printf("error occurred while inserting data\n");
  //printf("id: %i  value: %f    time:%d   index:%i  ----\n",id_t,value_t,ts_t,counter); 
    } 


     counter++;

    }  ;
 
 printf ("counter :%i\n",counter);
     fclose(file);

return 0;
}



/*
  * Write a SELECT query to select all sensor measurements in the table 
  * The callback function is applied to every row in the result
  * Return zero for success, and non-zero if an error occurs
  */
int find_sensor_all(DBCONN * conn, callback_t f)
{

   int rc;
    char *err_msg=0;

    char *sql = "SELECT * FROM sensorData";
        
    rc = sqlite3_exec(conn, sql, f, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        return 1;
    } 
    
    return 0;
}


int find_sensor_by_value(DBCONN * conn, sensor_value_t value, callback_t f){

 int rc;
    char *err_msg=0;
    char sqlfinal[80];
    char sensor_value[20];
     
    snprintf(sensor_value, 20, "%f", value);


    char *sql = "SELECT * FROM sensorData WHERE sensor_value=";

    strcpy(sqlfinal,sql);
    strcat(sqlfinal,sensor_value);


    rc = sqlite3_exec(conn, sqlfinal, f, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        return 1;
    } 
    
    return 0;


}



int find_sensor_exceed_value(DBCONN * conn, sensor_value_t value, callback_t f){

 int rc;
    char *err_msg=0;
    char sqlfinal[80];
    char sensor_value[20];
     
    snprintf(sensor_value, 20, "%f", value);


    char *sql = "SELECT * FROM sensorData WHERE sensor_value>";

    strcpy(sqlfinal,sql);
    strcat(sqlfinal,sensor_value);


    rc = sqlite3_exec(conn, sqlfinal, f, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        return 1;
    } 
    
    return 0;





}

/*
 * Write a SELECT query to return all sensor measurements having a timestamp 'ts'
 * The callback function is applied to every row in the result
 * Return zero for success, and non-zero if an error occurs
 */
int find_sensor_by_timestamp(DBCONN * conn, sensor_ts_t ts, callback_t f){

 int rc;
    char *err_msg=0;
    char sqlfinal[80];
    char ts_s[20];
     
    snprintf(ts_s, 20, "%lu", ts);


    char *sql = "SELECT * FROM sensorData WHERE timestamp=";

    strcpy(sqlfinal,sql);
    strcat(sqlfinal,ts_s);


    rc = sqlite3_exec(conn, sqlfinal, f, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        return 1;
    } 
    
    return 0;





}


int find_sensor_after_timestamp(DBCONN * conn, sensor_ts_t ts, callback_t f){


 int rc;
    char *err_msg=0;
    char sqlfinal[80];
    char ts_s[20];
     
    snprintf(ts_s, 20, "%lu", ts);


    char *sql = "SELECT * FROM sensorData WHERE timestamp>";

    strcpy(sqlfinal,sql);
    strcat(sqlfinal,ts_s);


    rc = sqlite3_exec(conn, sqlfinal, f, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        return 1;
    } 
    
    return 0;
}



