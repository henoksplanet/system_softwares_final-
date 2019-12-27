#define _GNU_SOURCE

#include "connmgr.h"

dplist_t * connecties;         //lijst met sockets bijhouden, de server komt op index 0

void* callback_copy(void * src_element){
        pollinfo* copy = malloc(sizeof(pollinfo));
        copy->file_descriptors = ((pollinfo*)src_element)->file_descriptors;
        copy->last_record = ((pollinfo*)src_element)->last_record;
        copy->sensor_id= ((pollinfo*)src_element)->sensor_id;
        copy->socket_id= ((pollinfo*)src_element)->socket_id;
        return (void*)copy;
}

void callback_free(void ** element){
        free(*element);
}

int callback_compare(void * x, void * y){
        return 0;
}                                                    //kunnen we niet gebruiken hier

//http://man7.org/linux/man-pages/man2/poll.2.html voor poll()
//BOVENSTAANDE LINK GOED LEZEN!!!!!
/*
   POLLIN There is data to read.
   POLLRDHUP (since Linux 2.6.17)
              Stream socket peer closed connection, or shut down writing
              half of connection.  The _GNU_SOURCE feature test macro must
              be defined (before including any header files) in order to
              obtain this definition.

              //bij POLLHUP is connectie gesloten in beide richtingen

              fd is file file descriptor
              events is requested event
              revents is returned events
 */

void connmgr_listen(int port_number){
        FILE * fp = fopen("sensor_data_recv.txt","w");

        connecties = dpl_create(callback_copy,callback_free,callback_compare);    //dplist maken, callback functies hierboven
        //nodige variabelen
        tcpsock_t *server, *sensor;       //verchillende namen voor makkelijk onderscheid, niet persé nodig
        pollinfo pollserver;
        polldescr pollserver_fd;  //file descriptor*/
        bool terminate = false;

        if(tcp_passive_open(&server, port_number) != TCP_NO_ERROR) { //socket maken voor server
                printf("server geraakt niet gemaakt\n");
        }
        if(tcp_get_sd(server,&(pollserver_fd.fd)) != TCP_NO_ERROR) {    //socket/file descriptor vinden en opslaan in pollserver_fd
                printf("socket not yet bound\n");
        }

        //start server en zet deze op index 0
        pollserver.last_record = time(NULL);  //last event in server
        pollserver.socket_id = server;
        pollserver.file_descriptors = pollserver_fd;                   //file discriptor van server opslaan in pollserver
        pollserver.file_descriptors.events = POLLIN;             //zo gaat het luisteren naar POLLIN, wat betekent dat er data beschikbaar is
        connecties = dpl_insert_at_index(connecties, (void*)(&pollserver),0,true); //de server staat op index 0

        //blijven pollen met loop, "luisteren"
        //  int dummy = 0;
        printf("loop start\n");
        while(!terminate)
        {

                int amount_connections = dpl_size(connecties);
                //printf("%d\n", amount_connections );
                for(int i=0; i<amount_connections; i++)  //elk element in onze lijst afgaan, beginnen bij server op index 0
                {
                        //  printf("in for-loop\n");
                        pollinfo* current_poll = ((pollinfo*)dpl_get_element_at_index(connecties,i));
                        if(poll(&(current_poll->file_descriptors),1,0)>0) {  //zie site, bij succes >0 en int poll(struct pollfd fds[](welke array van fds'en), nfds_t nfds(aantal fds'en), int timeout(in miliseconden)-> 0 dus geen blocking);
                                //    printf("in if\n");
                                if(current_poll->file_descriptors.revents == POLLIN) {  //POLLIN = INKOMENDE DATA
                                        //  printf("in if2\n");
                                        if(i==0) { //server want i=0

                                                tcp_wait_for_connection(current_poll->socket_id, &sensor);
                                                pollinfo new_sensor;
                                                new_sensor.socket_id = sensor;         //sensor vs server
                                                if(tcp_get_sd(sensor,&(pollserver_fd.fd)) != TCP_NO_ERROR) {    //socket/file descriptor vinden en opslaan in pollserver_fd
                                                        printf("socket not yet bound\n");
                                                }
                                                new_sensor.file_descriptors = pollserver_fd;
                                                new_sensor.file_descriptors.events = POLLIN | POLLRDHUP; //luisteren naar pollin en pollrdhup->socket verbroken aan de andere kant
                                                new_sensor.last_record = time(NULL);
                                                connecties = dpl_insert_at_index(connecties,(void*)(&new_sensor),dpl_size(connecties)+1,true);         //nieuwe sensor op positie 1, vorige sensors verschuiven naar achter
                                                printf("Nieuwe sensor is verbonden\n");
                                        }
                                        else //binnenkomende data van een sensor en https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.3.0/com.ibm.zos.v2r3.bpxbd00/rcv.htm, return van 0 of -1 bij verbroken connectie/probleem
                                        {
                                                //    printf("aantal connecties bij data input %d van %d\n", amount_connections,i);
                                                sensor_data_t data;
                                                int grootte = sizeof(data.id);
                                                tcp_receive((current_poll->socket_id),(void*)&(data.id), &grootte); //sensor ID LEZEN
                                                current_poll->sensor_id = data.id;                                  //sensor ID invullen bij socket
                                                grootte = sizeof(data.value);
                                                tcp_receive((current_poll->socket_id),(void*)&(data.value), &grootte); //sensor temperatuur LEZEN
                                                grootte = sizeof(data.ts);
                                                tcp_receive((current_poll->socket_id),(void*)&(data.ts), &grootte); //sensor timestamp LEZEN
                                                current_poll->last_record= time(NULL);
                                                printf("Data van %d met temperatuur %f op tijdstip %ld\n\n", data.id, data.value, data.ts);
                                                fprintf(fp, "%d %f %ld\n", data.id, data.value, data.ts);
                                        }
                                }

                        }
                        if(current_poll->file_descriptors.revents == POLLRDHUP) {         //socket is closed on the other side
                                //TODO connmgr geraakt niet in pollrdhup, vreemd
                                printf("verbreken door pollhup\n");
                                tcp_close(&(current_poll->socket_id));         //socket sluiten
                                connecties = dpl_remove_at_index(connecties,i,true);         //uit lijst verwijderen
                                pollserver.last_record = time(NULL);
                                amount_connections = dpl_size(connecties);
                                printf("aantal sockets over(incl server):%d\n", amount_connections );
                                break;         //dit beeindigt de for loop, dit is nodig omdat de indexen van connecties niet meer kloppen
                        }
                        if((current_poll->last_record + TIMEOUT)<time(NULL) && i!= 0)         //socket sluiten indien TIMEOUT
                        {
                                printf("verbreken door timeout\n");
                                tcp_close(&(current_poll->socket_id));         //socket sluiten
                                connecties = dpl_remove_at_index(connecties,i,true);         //uit lijst verwijderen
                                pollserver.last_record = time(NULL);
                                amount_connections = dpl_size(connecties);
                                printf("aantal sockets over(incl server):%d\n", amount_connections );
                                break;         //dit beeindigt de for loop, dit is nodig omdat de indexen van connecties niet meer kloppen
                        }

                        if(amount_connections == 1 && (pollserver.last_record + TIMEOUT)<time(NULL)) {
                                printf("No sensors active anymore, terminate connmgr\n");
                                tcp_close(&(current_poll->socket_id));         //socket van server sluiten
                                terminate = true;
                                connmgr_free();

                        }
                }

        }
        fclose(fp);
}

void connmgr_free(){
        dpl_free(&connecties,true);
}