#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "dplist.h"

//#define DEBUG

/*
 * definition of error codes
 * */
#define DPLIST_NO_ERROR 0
#define DPLIST_MEMORY_ERROR 1 // error due to mem alloc failure
#define DPLIST_INVALID_ERROR 2 //error due to a list operation applied on a NULL list 

#ifdef DEBUG
	#define DEBUG_PRINTF(...) 									         \
		do {											         \
			fprintf(stderr,"\nIn %s - function %s at line %d: ", __FILE__, __func__, __LINE__);	 \
			fprintf(stderr,__VA_ARGS__);								 \
			fflush(stderr);                                                                          \
                } while(0)
#else
	#define DEBUG_PRINTF(...) (void)0
#endif


#define DPLIST_ERR_HANDLER(condition,err_code)\
	do {						            \
            if ((condition)) DEBUG_PRINTF(#condition " failed\n");    \
            assert(!(condition));                                    \
        } while(0)


/*
 * The real definition of struct list / struct node
 */

struct dplist_node {
  dplist_node_t * prev, * next;
  element_t element;
};

struct dplist {
  dplist_node_t * head;
  // more fields will be added later
};

dplist_t * dpl_create ()
{

  dplist_t * list;
  dplist_node_t * dummynode;
  list = malloc(sizeof(struct dplist));
  DPLIST_ERR_HANDLER(list==NULL,DPLIST_MEMORY_ERROR);
  list->head = NULL;  
  
  // pointer drawing breakpoint
  return list;
}

void dpl_free( dplist_t ** list )
{


  assert(*list!=NULL);

/* if(dpl_size(*list)==0){

free((*list)->head);
free(*list);
*list=NULL;

return;
} */


if(dpl_size(*list)==0){

//(*list)->head->element=5;

}

int initial=dpl_size(*list);

 for(int i=initial+1;i>1;i--){
dpl_remove_at_index((*list),i);
};

//dpl_remove_at_index((*list),45);

free((*list)->head);
free(*list);
*list=NULL;

//*()=NULL;
//dpl_remove_at_index((*list),5);

//(*(**list).head))=NULL;
//printf("%d",(**list).head->element);
}

  /* Important note: to implement any list manipulation operator (insert, append, delete, sort, ...), always be aware of the following cases:
   * 1. empty list ==> avoid errors
   * 2. do operation at the start of the list ==> typically requires some special pointer manipulation
   * 3. do operation at the end of the list ==> typically requires some special pointer manipulation
   * 4. do operation in the middle of the list ==> default case with default pointer manipulation
   * ALWAYS check that you implementation works correctly in all these cases (check this on paper with list representation drawings!)
   **/ 

dplist_t * dpl_insert_at_index( dplist_t * list, element_t element, int index)
{


//  assert(dpl_get_reference_at_index(list,index)==NULL);

/*  /*  if(dpl_size(list)==0/*  && list->head->element==-56000 ){

  list->head->element=element;
  return list;
  }
/* 
  if(index<0 && list->head->element==-56000){
    
  list->head->element=element;
  return list;
  }

 */



  dplist_node_t * ref_at_index, * list_node;
  DPLIST_ERR_HANDLER(list==NULL,DPLIST_INVALID_ERROR);
  list_node = malloc(sizeof(dplist_node_t));
  DPLIST_ERR_HANDLER(list_node==NULL,DPLIST_MEMORY_ERROR);
  list_node->element = element;
  
  // pointer drawing breakpoint
  if (list->head == NULL || list->head->element==-56000 )  //added case for pre-occupied list
  { // covers case 1 
    list_node->prev = NULL;
    list_node->next = NULL;
    list->head = list_node;
    // pointer drawing breakpoint
  } else if (index <= 0)  
  { // covers case 2 
    list_node->prev = NULL;
    list_node->next = list->head;
    list->head->prev = list_node;
    list->head = list_node;
    // pointer drawing breakpoint
  } else 
  {
    ref_at_index = dpl_get_reference_at_index(list, index);  
    assert( ref_at_index != NULL);
    // pointer drawing breakpoint
    if (index < dpl_size(list))
    { // covers case 4
      list_node->prev = ref_at_index->prev;
      list_node->next = ref_at_index;
      ref_at_index->prev->next = list_node;
      ref_at_index->prev = list_node;
      // pointer drawing breakpoint
    } else
    { // covers case 3 
      assert(ref_at_index->next == NULL);
      list_node->next = NULL;
      list_node->prev = ref_at_index;
      ref_at_index->next = list_node;    
      // pointer drawing breakpoint
    }
  }
  return list;
}


dplist_t * dpl_remove_at_index( dplist_t * list, int index)
{








int counter=0;

dplist_t* currentList=list;

dplist_node_t* current=list->head;


 /*  if(dpl_size(list)==0){
    currentList->head=NULL;
    free(currentList->head);
    return currentList;
  
  } */

if( list->head->element==-56000){

list->head->element=-56000;

return currentList;
}




 if (dpl_size(list)!=1 && (index==0 || index<0 ))
{
  current->next->prev=NULL;
  currentList->head=current->next;
  
  free(current);

  return currentList; 
}

else if(dpl_size(list)!=1 && index>dpl_size(list)-1){
/* if(dpl_size(list)==1){

dplist_node_t* current= list->head;
free(current);
return currentList;
} */
/* else
{ */
  dplist_node_t* current= dpl_get_reference_at_index(list,dpl_size(list)-1);
  current->prev->next=NULL;
  current->prev=NULL;
  current->next=NULL;
  current->element=65000;
  free(current);

  return currentList;
/* } */
}
 
/* else if(dpl_size(list)!=1 && dpl_size(list)==2){

  printf("only head is left");
 headholder=list->head; 
  return currentList;
} */


else if(dpl_size(list)==1  || ((dpl_size(list)==0 & list->head->element==-56000))){
   
//   printf("hello");

   list->head->element=-56000; 
   // printf("--%i--",currentList->head->element);
    //currentList->head=NULL;
   //free(currentList->head);
    //list->head=NULL;


    return currentList;
}







else{
while (counter!=index-1)
{
  current=current->next;
  counter++;
}

current->prev->next=current->next;
current->next->prev=current->prev;

  current->prev=NULL;
  current->next=NULL;
  current->element=65000;
free(current);

return currentList;
}



}



int dpl_size( dplist_t * list )
{
//  int size(struct node *list){

/*  if(list==NULL){
   printf("none\n");
 }  */

if(list==NULL ){
  return 0;
}




int counter=1;
dplist_node_t* current=list->head;

if(list->head==NULL || current->element==-56000){
  return 0;
}

//assert(list->head==NULL);

else{
while(current->next!=NULL){
   current=current->next;
  counter++;
} 
return counter;
}


}


dplist_node_t * dpl_get_reference_at_index( dplist_t * list, int index )
{

  int count;
  dplist_node_t * dummy;
  DPLIST_ERR_HANDLER(list==NULL,DPLIST_INVALID_ERROR);
  if (list->head == NULL ) return NULL;
  for ( dummy = list->head, count = 0; dummy->next != NULL ; dummy = dummy->next, count++) 
  { 
    if (count >= index) return dummy;
  }  
  return dummy;  
}


element_t dpl_get_element_at_index( dplist_t * list, int index )
{


  if(dpl_size(list)==0 || list->head->element==-56000){
    return 0;
  }

int counter=0;

dplist_node_t* current=list->head;


if((index+1)<=(dpl_size(list)) && index>0){

while (counter!=index)
{
  current=current->next;
  counter++;
}
return current->element;

}
if(index==0 || index<0){

return current->element;

}

if((index+1)>dpl_size(list)){

while (counter!=(dpl_size(list)-1))
{
  current=current->next;
  counter++;
} 

return current->element;


}




}
/* 

void dpl_print( dplist_t * list ){
int counter=0;
element_t element;

if(list==NULL || list->head->element==-56000){
  printf("List is Null\n");
  return;
}
dplist_node_t* current=list->head;
 
  while(current!=NULL){

    
    element =current->element;
    printf("index %d has element = %d\n\n",counter,element);
    counter++;
    current=current->next;
  } */





int dpl_get_index_of_element( dplist_t * list, element_t element )
{

if(dpl_size(list)==0){
  return -1;
}

int counter=0;

dplist_node_t* current=list->head;

while (current->element!=element)
{

  if(counter+1==dpl_size(list)){
    return -1;
  }

  current=current->next;
  counter++;
}

return counter;

}

