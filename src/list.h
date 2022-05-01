#ifndef LIST_H
#define LIST_H
#include <stddef.h>

#define LL_SINGLELINKED 0
#define LL_DOUBLELINKED 1

#define LL_NEXT(x) (*(((void**) x) - 1))
#define LL_PREV(x) (*(((void**) x) - 2))
#define LL_LIST_INIT(type) ((struct list) {NULL, NULL, 0, type})
#define LL_OUTOFBOUNDS -1
#define LL_POINTLESS -2

typedef unsigned char lltype_t;

struct list
{
   void *chain;
   void *tail;
   size_t size;
   lltype_t type;
};

void* llnodealloc (size_t size, lltype_t type);
void* llnoderealloc (void *node, size_t size, lltype_t type);
void  llnodefree (void *node, lltype_t type);
void* llgetnode (struct list *list, size_t position);
void *llpullnodep (struct list *list, void *prevnode);
#define llpullnodei(list, position) ((position<(list)->size)?(llpullnodep((list), ((position) == 0) ? NULL : llgetnode((list), (position) - 1))):(NULL))
int llmergelist (struct list *dest, struct list *src, size_t dest_offset);
int llinsertchaini (struct list *list, size_t position, void *first_node);
int llinsertchainp (struct list *list, void *prevnode, void *first_node);
#define llprependchain(list, first_node) llinsertchain((list), 0, (first_node))
#define llappendchain(list, first_node) llinsertchain((list), (list)->size, (first_node))
struct list llsublistp (struct list *list, void *prevnode, size_t quantity);
struct list llsublisti (struct list *list, size_t position, size_t quantity);
int llrmsublisti (struct list *list, size_t position, size_t quantity);
int llrmsublistp (struct list *list, void *prevnode, size_t quantity);
void llrmchain (void *chain, lltype_t type);
#define llrmlist(list) llrmchain((list)->chain, (list)->type);*(list)=LL_LIST_INIT((list)->type)
void* llreverse (void *node, lltype_t type);
#define llreverselist(list) (list)->tail=(list)->chain;(list)->chain=llreverse((list)->chain,(list)->type)

/* In singly linked list, only the first node can be passed
 * In doubly linked list, any node can be passed */
struct list llchaintolist (void *node, lltype_t type);

/* extracts subchain from chain (destructive)
   prevnode - previous node relative to first one of the subchain
   size - the size of the resulting subchain
   type - the type of linked list (singly-linked or doubly-linked)
   return value - first node of the resulting subchain */
void* llsubchain (void *prevnode, size_t size, lltype_t type);
void* llbreakchain (void *lastnode, lltype_t type);

#define lllinknodes(first,second,type) \
LL_NEXT((first)) = (second); \
if ((type) == LL_DOUBLELINKED) \
   LL_PREV((second)) = (first)

/* inserts new chain after dest */
void llmergechains (void *dest, void *src, lltype_t type);


#endif // LIST_H