#include<stdio.h>
#include<string.h>
int main()
{

char str[80];
strcpy(str,"these ");
strcat(str,"strings ");
strcat(str,"are ");
strcat(str,"concatenated.");


printf("%s\n",str);
return 0;

}