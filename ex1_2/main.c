#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "dplist.h"

dplist_t * list = NULL;

void dpl_print( dplist_t * list );

int main(void)
{
  list = dpl_create();  

dpl_insert_at_index(list,1,0); 
dpl_insert_at_index(list,2,1);
dpl_insert_at_index(list,3,2);
dpl_insert_at_index(list,4,3);
dpl_insert_at_index(list,5,4);
dpl_insert_at_index(list,6,5);




printf("ref at -8:%p\n",dpl_get_reference_at_index( list, 90));
//printf("ref at -8:%p\n",dpl_get_reference_at_index( list, 90));


printf("size of list is:%i\n",dpl_size(list));




dpl_print(list);

dpl_free(&list);

  return 0;
}





void dpl_print( dplist_t * list )
{
  element_t element;
  int i, length;

  length = dpl_size(list);
  for ( i = 0; i < length; i++)    
  {
    element = dpl_get_element_at_index( list, i );
    printf("index element %d = %d\n", i, element);
  }
}
