#include<time.h>
#include<stdio.h>
#include<stdlib.h>

#include<unistd.h>


int main()
{

    while(1){
time_t now;
struct tm *ts;
char buf[80];
now=time(NULL);
ts=localtime(&now);
strftime(buf,sizeof(buf),"%a %Y-%m-%d %H:%M:%S %Z",ts);
puts(buf);
printf("the long signed integer is %lu\n",now);
sleep(1);
    }




return 0;

}