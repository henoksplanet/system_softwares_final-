#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "dplist.h"

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
  void * element;
  int status;
 }; 







struct dplist {
  dplist_node_t * head;
  void * (*element_copy)(void * src_element);			  
  void (*element_free)(void ** element);
  int (*element_compare)(void * x, void * y);
};


dplist_t * dpl_create (// callback functions
			  void * (*element_copy)(void * src_element),
			  void (*element_free)(void ** element),
			  int (*element_compare)(void * x, void * y)
			  )
{
  dplist_t * list;
  list = malloc(sizeof(struct dplist));
  DPLIST_ERR_HANDLER(list==NULL,DPLIST_MEMORY_ERROR);

  list->head=NULL;
  list->element_copy = element_copy;
  list->element_free = element_free;
  list->element_compare = element_compare; 
  return list;
}

void dpl_free(dplist_t ** list, bool free_element)
{
 
if(dpl_size(*list)==0){   // do nothing if no element is in the list 

}

if(free_element==true){

    for(int i=dpl_size(*list);i>=0;i--){    //remove each elements individually 
    dpl_remove_at_index((*list),i,true);
};
}
else{
    for(int i=dpl_size(*list);i>=0;i--){    //remove each elements individually 
    dpl_remove_at_index((*list),i,false);
}
}


free((*list)->head); // free the content of the header ,mostly contained as satus=-1 
free(*list);        // free the heap memory reserved
*list=NULL;         // double check

}



dplist_t * dpl_insert_at_index(dplist_t * list, void * element, int index, bool insert_copy)
{

void *elementcurrent;

if(insert_copy==true){              //depending on the insert_copy the element gets copied or not
    elementcurrent=list->element_copy(element);
}
else
{
    elementcurrent=element;
}



if(list->head!=NULL){       // if the head isn't empty 
    if(list->head->status==-1){     //check the status if it is a placeholder
       
  list->head->status=0;         //sets the status to normal element 
  list->head->element=elementcurrent;  //sets the first element 
  return list;
    }
}




  dplist_node_t * ref_at_index, * list_node;
  DPLIST_ERR_HANDLER(list==NULL,DPLIST_INVALID_ERROR);
  list_node = malloc(sizeof(dplist_node_t));  
  DPLIST_ERR_HANDLER(list_node==NULL,DPLIST_MEMORY_ERROR);
  list_node->element = elementcurrent;
  list_node->status=0;
  
  // pointer drawing breakpoint
  if (list->head == NULL )  //added case for pre-occupied list
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



dplist_t * dpl_remove_at_index( dplist_t * list, int index, bool free_element)
{
  
dplist_node_t *current=list->head;
if(dpl_size(list)==1){      // to set the last element to placeholder 

list->head->status=-1;

if(free_element==true)
{
    list->element_free(&list->head->element);
}


return list;

}

if(dpl_size(list)==0){       //gets executed when the list is free 
    return list;

}



int counter=0;

if(index<=0){               // when the index provided is less than or equal to zero remove the head/first element 
list->head->next->prev=NULL;
list->head=list->head->next;


if(free_element==true)
{
    list->element_free(&current->element);
}
free(current);
return list;
}

                            //if the index provided is more than the size of the list remove the last element
if(index>=dpl_size(list)-1){
    
while(current->next!=NULL){
current=current->next;
}

current->prev->next=NULL;


if(free_element==true)
{
    list->element_free(&current->element);
}
free(current);

return list;

}
                            //if not and if it is in the middle 


  while(counter!=index){
   
   current=current->next;
   counter++;
  }


 current->prev->next=current->next;
 current->next->prev=current->prev;
 

 current->next=NULL;
 current->prev=NULL;

if(free_element==true)
{
    list->element_free(&current->element);
}

free(current);// just to free the remove the list node





 return list;
}









int dpl_size( dplist_t * list )
{


if(list==NULL ){   // empty list or not initialized list 
return 0;
}

if(list->head!=NULL){
    if(list->head->status==-1)
    {
        return 0;
    }
}

if(list->head==NULL){
    return 0;
}



int counter=1;
dplist_node_t* current=list->head; 


 /* 
if(list->head->prev!=NULL){    //if there is only a place holder dummy node

return 0;
} */
 


//assert(list->head==NULL);


while(current->next!=NULL){
   current=current->next;
  counter++;
} 
return counter; 
}

void * dpl_get_element_at_index( dplist_t * list, int index )
{

if(dpl_size(list)==0)               // if the list is empty NULL pointer is returned
{
    return NULL;
};

    int counter=0;
    dplist_node_t *current=list->head;

   if(index>=dpl_size(list)-1){     // when the index is biggers than the number of elements 

    while(current->next!=NULL){
      current=current->next;
    }
    return current->element;
   }
   
   if(index<=0){            //when the index is 0 or negative

    return current->element;
   }




    while(counter!=index){          // all good things happen here
      
      current=current->next;
      counter++;
    }

    return current->element;
}






int dpl_get_index_of_element( dplist_t * list, void * element )
{
    int counter=0;
    dplist_node_t *current=list->head;
  

   if(dpl_size(list)==1){ // if there is only single element in the list 

      if(list->element_compare(element,list->head->element)==0){
        return 0;
      }
      else{
          return -1;
      }

   }
    
 if(dpl_size(list)==0){    // if no element is found in the list 
     return -1;
 }
   


    while(current->next!=NULL){             //all good things happen here

        if(list->element_compare(current->element,element)==0){
            return counter;
        }
       current=current->next;
       counter++;
    }

    if(current->next==NULL){
        return (dpl_size(list)-1);
    }




    return -1;

   



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





 
void * dpl_get_element_at_reference( dplist_t * list, dplist_node_t * reference )
{

dplist_node_t *current= list->head;

while(current->next!=NULL){
    current=current->next;
    if(current==reference){
        return current->element;
    }
}

if(dpl_size(list)==0){ // when the list is empty 
    return NULL;
}
if(reference==NULL){  // when the reference pointer is null 
     
    while(current->next!=NULL){
    current=current->next;
    }
    return current->element;
}
return NULL;
    
}

