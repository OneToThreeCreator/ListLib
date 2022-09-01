#include <stdlib.h>
#include <stdint.h>

#include "list.h"

/*
struct node:
   LL_PREV (if defined)
   LL_NEXT
-> data
*/

#define MAX(x,y) (((x)>(y))?(x):(y))
#define MIN(x,y) (((x)<(y))?(x):(y))

void *llnodealloc (size_t size, lltype_t type)
{
   void **node = ((void**) malloc(size + (1 + (type == LL_DOUBLELINKED)) * sizeof(void*))) + (1 + (type == LL_DOUBLELINKED));
   LL_NEXT(node) = NULL;
   if (type == LL_DOUBLELINKED)
      LL_PREV(node) = NULL;
   return (void*) node;
}

void *llnoderealloc (void *node, size_t size, lltype_t type)
{
   return ((void**) realloc(((void**) node) - (1 + (type == LL_DOUBLELINKED)),
                            size + (1 + (type == LL_DOUBLELINKED)) * sizeof(void*))) + (1 + (type == LL_DOUBLELINKED));
}

void llnodefree (void *node, lltype_t type)
{
   free(((void**) node) - (1 + (type == LL_DOUBLELINKED)));
}

void *llgetnode (struct list *list, size_t position)
{
   if (position > (list->size - 1))
      return NULL;
   if (position == list->size - 1)
      return list->tail;
   
   void **node;
   size_t i;
   if ((list->type == LL_DOUBLELINKED) && (position > (list->size / 2)))
   {
      node = list->tail;
      i = list->size - 1;
      while (i > position)
      {
         node = LL_PREV(node);
         --i;
      }
   }
   else
   {
      node = list->chain;
      i = 0;
      while (i < position)
      {
         node = LL_NEXT(node);
         ++i;
      }
   }
   
   return node;
}

void *llpullnodep (struct list *list, void *prevnode)
{
   void *node, *nextnode;
   if (prevnode == NULL)
   {
      node = list->chain;
      nextnode = LL_NEXT(node);
   }
   else
   {
      node = LL_NEXT(prevnode);
      nextnode = LL_NEXT(node);
      LL_NEXT(prevnode) = nextnode;
   }
   LL_NEXT(node) = NULL;
   if (list->type == LL_DOUBLELINKED)
   {
      if (nextnode != NULL)
         LL_PREV(nextnode) = prevnode;
      LL_PREV(node) = NULL;
   }
   if (nextnode == NULL)
      list->tail = prevnode;
   return node;
}

int llinsertchainp (struct list *list, void *prevnode, void *first_node)
{
   if (first_node == NULL)
      return LL_POINTLESS;

   void **node, **nextnode;
   if (prevnode == NULL)
   {
      nextnode = list->chain;
      list->chain = first_node;
      node = list->chain;
   }
   else
   {
      node = prevnode;
      if (list->type == LL_DOUBLELINKED)
         LL_PREV((void**) first_node) = node;
         
      nextnode = LL_NEXT(node);
      LL_NEXT(node) = first_node;
      node = LL_NEXT(node);
   }
   ++(list->size);
   while (LL_NEXT(node) != NULL)
   {
      node = LL_NEXT(node);
      ++(list->size);
   }
   LL_NEXT(node) = nextnode;
   if (nextnode == NULL)
   {
      list->tail = node;
   }
   else
   {
      if (list->type == LL_DOUBLELINKED)
         LL_PREV(nextnode) = node;
   }
   
   return EXIT_SUCCESS;
}

int llinsertchaini (struct list *list, size_t position, void *first_node)
{
   if (position > list->size)
      return LL_OUTOFBOUNDS;
      
   if (first_node == NULL)
      return LL_POINTLESS;
   
   void *node = ((position == 0) ? NULL : llgetnode(list, position - 1));
   return llinsertchainp(list, node, first_node);
}

int llmergelist (struct list *dest, struct list *src, size_t dest_offset)
{
   if (dest_offset > dest->size)
      return LL_OUTOFBOUNDS;
   if ((dest->type != src->type) || (src->size == 0))
      return LL_POINTLESS;
   
   void **node;
   if (dest_offset == 0)
   {
      node = dest->chain;
      dest->chain = src->chain;
      LL_NEXT(src->tail) = node;
      if (dest->type == LL_DOUBLELINKED)
         LL_PREV(node) = src->tail;
   }
   else
   {
      node = llgetnode(dest, dest_offset - 1);
      void **nextnode = LL_NEXT(node);
      LL_NEXT(node) = src->chain;
      LL_NEXT(src->tail) = nextnode;
      if (dest->type == LL_DOUBLELINKED)
      {
         LL_PREV(nextnode) = src->tail;
         LL_PREV(src->chain) = node;
      }
   }
   
   if (dest->size == dest_offset)
      dest->tail = src->tail;
   dest->size += src->size;
   src->chain = NULL;
   src->tail  = NULL;
   src->size  = 0;
   return EXIT_SUCCESS;
}

struct list llsublistp (struct list *list, void *prevnode, size_t quantity)
{
   if (quantity == 0)
      return (struct list) {NULL, NULL, 0, list->type};
      
   void **node;
   if (prevnode == NULL)
      node = list->chain;
   else
      node = LL_NEXT(prevnode);
   
   void *chain = node;
   size_t i = 0;
   while (i < (quantity - 1))
   {
      node = LL_NEXT(node);
      ++i;
   }
   void **nextnode = LL_NEXT(node);
   if (prevnode == NULL)
      list->chain = nextnode;
   else
      LL_NEXT(prevnode) = nextnode;
      
   LL_NEXT(node) = NULL;
   if (list->type == LL_DOUBLELINKED)
   {
      if (nextnode != NULL)
         LL_PREV(nextnode) = prevnode;
      LL_PREV(chain) = NULL;
   }
   if (nextnode == NULL)
      list->tail = prevnode;
   list->size -= quantity;
   return (struct list) {chain, node, quantity, list->type};
}

struct list llsublisti (struct list *list, size_t position, size_t quantity)
{
   if ((position >= list->size) || (quantity == 0))
      return (struct list) {NULL, NULL, 0, list->type};
   
   if ((position == 0) && (quantity == list->size))
   {
      void *chain = list->chain, *tail = list->tail;
      size_t size = list->size;
      *list = LL_LIST_INIT(list->type);
      return (struct list) {chain, tail, size, list->type};
   }
   
   void **node;
   void **prevnode;
   void **chain;
   void **last_chain_node;
   if ((list->type == LL_DOUBLELINKED) && (list->size - position <= position))
   {
      node = list->tail;
      size_t i = list->size - 1, end = position + quantity;
      while (i > end)
      {
         node = LL_PREV(node);
         --i;
      }
      if (position + quantity < list->size)
      {
         prevnode = node;
         node = LL_PREV(node);
         --i;
         LL_NEXT(node) = NULL;
      }
      last_chain_node = node;
      end = position;
      while (i > end)
      {
         node = LL_PREV(node);
         --i;
      }
      chain = node;
      node = LL_PREV(node);
      LL_PREV(chain) = NULL;
      if (position + quantity < list->size)
      {
         LL_NEXT(node) = prevnode;
         LL_PREV(prevnode) = node;
      }
      else
      {
         LL_NEXT(node) = NULL;
         list->tail = node;
      }
   }
   else
   {
      if (position == 0)
      {
         prevnode = NULL;
         node = list->chain;
      }
      else
      {
         node = llgetnode(list, position - 1);
         prevnode = node;
         node = LL_NEXT(node);
      }
      
      if (list->type == LL_DOUBLELINKED)
         LL_PREV(node) = NULL;
         
      if (position + quantity + 1 == list->size)
      {
         if (position == 0)
            list->chain = NULL;
         else
            LL_NEXT(prevnode) = NULL;
         
         void *last_chain_node = list->tail;
         list->tail = prevnode;
         size_t prevsize = list->size;
         list->size -= MIN(quantity, list->size - position);
         return (struct list) {node, last_chain_node, MIN(quantity, prevsize - position), list->type};
      }
      chain = node;
      if ((list->type == LL_DOUBLELINKED) && (list->size - position - quantity) < quantity)
      {
         node = llgetnode(list, MIN(position + quantity - 1, list->size - 1));
      }
      else
      {
         size_t i = 0;
         while (i < (quantity - 1))
         {
            node = LL_NEXT(node);
            ++i;
         }
      }
      last_chain_node = node;
      node = LL_NEXT(node);
      LL_NEXT(last_chain_node) = NULL;
      if (position + quantity < list->size)
      {
         if (list->type == LL_DOUBLELINKED)
            LL_PREV(node) = prevnode;
      }
      else
      {
         list->tail = prevnode;
      }
      
      if (position == 0)
         list->chain = node;
      else
         LL_NEXT(prevnode) = node;
   }
   size_t prevsize = list->size;
   list->size -= MIN(quantity, list->size - position);
   return (struct list) {chain, last_chain_node, MIN(quantity, prevsize - position), list->type};
}

int llrmsublistp (struct list *list, void *prevnode, size_t quantity)
{
   if (quantity == 0)
      return LL_POINTLESS;

   void **node;
   if (prevnode == NULL)
      node = list->chain;
   else
      node = LL_NEXT(prevnode);

   size_t i = 0;
   void **lastnode;
   while (i < quantity)
   {
      lastnode = node;
      node = LL_NEXT(node);
      llnodefree(lastnode, list->type);
      ++i;
   }
   if (prevnode == NULL)
      list->chain = node;
   else
      LL_NEXT(prevnode) = node;
   
   if (node == NULL)
   {
      list->tail = prevnode;
   }
   else
   {
      if (list->type == LL_DOUBLELINKED)
      {
         LL_PREV(node) = prevnode;
      }
   }
   list->size -= quantity;
   return EXIT_SUCCESS;
}

int llrmsublisti (struct list *list, size_t position, size_t quantity)
{
   if (quantity == 0)
      return LL_POINTLESS;
   
   if (position >= list->size)
      return LL_OUTOFBOUNDS;
   
   void **node;
   void **prevnode;
   if ((list->type == LL_DOUBLELINKED) && (position >= list->size - position - quantity))
   {
      node = list->tail;
      size_t i = list->size - 1, end = position + quantity;
      while (i > end)
      {
         node = LL_PREV(node);
         --i;
      }
      if (position + quantity < list->size)
      {
         prevnode = node;
         node = LL_PREV(node);
      }
      void **lastnode;
      while (i >= position)
      {
         lastnode = node;
         node = LL_PREV(node);
         llnodefree(lastnode, list->type);
         --i;
      }
      if (position + quantity < list->size)
      {
         LL_NEXT(node) = prevnode;
         LL_PREV(prevnode) = node;
      }
      else
      {
         LL_NEXT(node) = NULL;
         list->tail = node;
      }
      list->size -= MIN(quantity, list->size - position);
      return EXIT_SUCCESS;
   }
   return llrmsublistp(list, ((position == 0) ? NULL : llgetnode(list, position - 1)), quantity);
}

void* llreverse (void *node, lltype_t type)
{
   void **prevnode = NULL, **nextnode;
   while (node != NULL)
   {
      nextnode = LL_NEXT(node);
      LL_NEXT(node) = prevnode;
      if (type == LL_DOUBLELINKED)
         LL_PREV(node) = nextnode;
         
      prevnode = node;
      node = nextnode;
   }
   return prevnode;
}

struct list llchaintolist (void *node, lltype_t type)
{
   if (node == NULL)
      return (struct list) {NULL, NULL, 0, type};
   size_t size = 1;
   void **first_node = node, **last_node = node;
   if (type == LL_DOUBLELINKED)
   {
      while (LL_PREV(first_node) != NULL)
      {
         first_node = LL_PREV(first_node);
         ++size;
      }
   }
   while (LL_NEXT(last_node) != NULL)
   {
      last_node = LL_NEXT(last_node);
      ++size;
   }
   return (struct list) {first_node, last_node, size, type};
}

void* llsubchain (void *prevnode, size_t size, lltype_t type)
{
   if ((size == 0) || (prevnode == NULL) || (LL_NEXT(prevnode) == NULL))
      return NULL;
   void **chain;
   void **node = LL_NEXT(prevnode);
   chain = node;
   size_t i = 1;
   while (i < size)
   {
      node = LL_NEXT(node);
      ++i;
   }
   void **nextnode = LL_NEXT(node);
   LL_NEXT(prevnode) = nextnode;
   LL_NEXT(node) = NULL;
   if (type == LL_DOUBLELINKED)
   {
      LL_PREV(chain) = NULL;
      LL_PREV(nextnode) = prevnode;
   }
   return chain;
}

void* llbreakchain (void *lastnode, lltype_t type)
{
   void *node = LL_NEXT(lastnode);
   LL_NEXT(lastnode) = NULL;
   if (type == LL_DOUBLELINKED)
      LL_PREV(node) = NULL;
   
   return node;
}

void llmergechains (void *dest, void *src, lltype_t type)
{
   void **node = dest, **nextnode = LL_NEXT(dest);
   LL_NEXT(node) = src;
   while (LL_NEXT(node) != NULL)
      node = LL_NEXT(node);
   
   LL_NEXT(node) = nextnode;
}

void llrmchain (void *chain, lltype_t type)
{
   void **node = chain, **prevnode;
   if (type == LL_DOUBLELINKED)
   {
      node = LL_PREV(node);
      while (node != NULL)
      {
         prevnode = node;
         node = LL_PREV(node);
         llnodefree(prevnode, type);
      }
   }
   node = chain;
   while (node != NULL)
   {
      prevnode = node;
      node = LL_NEXT(node);
      llnodefree(prevnode, type);
   }
}
