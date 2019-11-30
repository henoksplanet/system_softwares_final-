#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "dplist.h"



int *copy_int(int *integer) {
    int *new_int = (int*)malloc(sizeof(int));
    *new_int = *integer;
    return new_int;
}

void **free_int(int **integer) {
    free(*integer);
    *integer = NULL;
   return NULL;
}

int compare_int(int *integer1, int *integer2) {
    
    int one =*((int *) integer1);
    int two =*((int *) integer2);
    
    
    
    
    if(one > two) return 1;
    if(one < two) return -1;
    return 0;    
}

void display_int(int *integer) {
    if( integer!=NULL){
    printf("int is: %d\n", *integer);
    }
}

int main() {
    dplist_t *list;

    int a = 1;
    int b = 2;
    int c = 3;
   

 
    list = dpl_create(  (void *(*)(void *))copy_int,    // create works perfect 
                        (void (*)(void **))free_int,
                        (int (*)(void *, void *))compare_int
                    );




//printf("before addition---> size of list: %i\n",dpl_size(list));


dpl_insert_at_index(list, &a, 0, true);

dpl_insert_at_index(list, &b, 0, true);

dpl_insert_at_index(list, &c, 3, true);

dpl_insert_at_index(list, &a, 0, true);

dpl_insert_at_index(list, &b, 0, true);

dpl_insert_at_index(list, &c, 3, true);

dpl_insert_at_index(list, &a, 0, true);

dpl_insert_at_index(list, &b, 0, true);

dpl_insert_at_index(list, &c, 3, true);







for(int i=0;i<dpl_size(list);i++){

    printf("index %i has element %i \n",i,*(int *)dpl_get_element_at_index(list,i));
}

printf("list has %i elements\n ",dpl_size(list));


dpl_free(&list, true);
    
    
    

}