#include <stdio.h>
#include "list.h"

void fprintList (int *chain, FILE *f)
{
   fprintf(f, "{");
   for (int *node = chain; node != NULL; node = LL_NEXT(node))
   {
      fprintf(f, "%i, ", *node);
   }
   fprintf(f, "\b\b}\n");
}
int listCheck (int *chain, int *testArray)
{
   for (int *node = chain; node != NULL; node = LL_NEXT(node), ++testArray)
   {
      if (*node != *testArray)
         return 0;
   }
   return 1;
}

int listTest (lltype_t type)
{
   int *first  = llnodealloc(sizeof(int), type);
   {
      int *next = llnodealloc(sizeof(int), type);
      *first = 32;
      *next = -1;
      lllinknodes(first, next, type);
      {
         int check[2] = {32, -1};
         if(listCheck(first, check) != 1)
         {
            fprintf(stderr, "LINKEDCHAIN::LINKAGE::BROKEN:\nlllinknodes() cannot link nodes propertly\n");
            llrmchain(first, type);
            return 0;
         }
      }
      next = llnodealloc(sizeof(int), type);
      *next = 18;
      {
         int *nextnext = llnodealloc(sizeof(int), type);
         *nextnext = 9;
         lllinknodes(next, nextnext, type);
      }
      llmergechains(first, next, type);
      {
         int check[4] = {32, 18, 9, -1};
         if(listCheck(first, check) != 1)
         {
            fprintf(stderr, "LINKEDCHAIN::MERGE::BROKEN:\nExpected: {%d, %d, %d, %d}\nGot: ",
                    check[0], check[1], check[2], check[3]);
            fprintList(first, stderr);
            llrmchain(first, type);
            return 0;
         }
      }
   }
   struct list firstList = llchaintolist(first, type);
   if (firstList.size != 4)
   {
      fprintf(stderr, "LIST::CONVERSION::FAILED:\nllchaintolist() returned list with incorrect length %lu (length %u expected)\n", firstList.size, 4);
      llrmchain(first, type);
      return 0;
   }
   {
      int *second = llnodealloc(sizeof(int), type);
      *second = 78;
      int *next = llnodealloc(sizeof(int), type);
      lllinknodes(second, next, type);
      *next = 25;
      struct list secondList = llchaintolist(second, type);
      llmergelist(&firstList, &secondList, 3);
   }
   if (firstList.size != 6)
   {
      fprintf(stderr, "LIST::MERGE::FAILED:\nincorrect list size %lu (expected %u)\n", firstList.size, 6);
      llrmchain(first, type);
      return 0;
   }
   {
      int check[6] = {32, 18, 9, 78, 25, -1};
      if (listCheck(firstList.chain, check) != 1)
      {
         fprintf(stderr, "LIST::MERGE::FAILED:\nExpected: {%d, %d, %d, %d, %d, %d}\nGot: ",
         check[0], check[1], check[2], check[3], check[4], check[5]);
         fprintList(firstList.chain, stderr);
         llrmchain(first, type);
         return 0;
      }
   }
   llreverselist(&firstList);
   {
      int check[6] = {-1, 25, 78, 9, 18, 32};
      if (listCheck(firstList.chain, check) != 1)
      {
         fprintf(stderr, "LIST::REVERSE::FAILED:\nExpected: {%d, %d, %d, %d, %d, %d}\nGot: ",
         check[0], check[1], check[2], check[3], check[4], check[5]);
         fprintList(firstList.chain, stderr);
         llrmchain(first, type);
         return 0;
      }
   }
   struct list secondList = llsublisti(&firstList, 4, 2);
   if (firstList.size != 4)
   {
      fprintf(stderr, "LIST::SPLIT::FAILED:\nincorrect list size %lu (expected %u)\n", firstList.size, 4);
      llrmlist(&firstList);
      llrmlist(&secondList);
      return 0;
   }
   if (secondList.size != 2)
   {
      fprintf(stderr, "LIST::SPLIT::FAILED:\nincorrect list size %lu (expected %u)\n", secondList.size, 2);
      llrmlist(&firstList);
      llrmlist(&secondList);
      return 0;
   }
   {
      int check[2] = {18, 32};
      if (listCheck(secondList.chain, check) != 1)
      {
         fprintf(stderr, "LIST::SPLIT::FAILED:\nExpected: {%d, %d}\nGot: ",
         check[0], check[1]);
         fprintList(secondList.chain, stderr);
         llrmlist(&firstList);
         llrmlist(&secondList);
         return 0;
      }
   }
   {
      int check[4] = {-1, 25, 78, 9};
      if (listCheck(firstList.chain, check) != 1)
      {
         fprintf(stderr, "LIST::SPLIT::FAILED:\nExpected: {%d, %d, %d, %d}\nGot: ",
         check[0], check[1], check[2], check[3]);
         fprintList(firstList.chain, stderr);
         llrmlist(&firstList);
         llrmlist(&secondList);
         return 0;
      }
   }
   llmergelist(&firstList, &secondList, 1);
   if (firstList.size != 6)
   {
      fprintf(stderr, "LIST::MERGE::FAILED:\nincorrect list size %lu (expected %u)\n", firstList.size, 6);
      llrmlist(&firstList);
      return 0;
   }
   {
      int check[6] = {-1, 18, 32, 25, 78, 9};
      if (listCheck(firstList.chain, check) != 1)
      {
         fprintf(stderr, "LIST::MERGE::FAILED:\nExpected: {%d, %d, %d, %d}\nGot: ",
         check[0], check[1], check[2], check[3]);
         fprintList(firstList.chain, stderr);
         llrmlist(&firstList);
         llrmlist(&secondList);
         return 0;
      }
   }
   int *third = llsubchain(firstList.chain, 4, type);
   firstList.size -= 4;
   {
      int check[4] = {18, 32, 25, 78};
      if (listCheck(third, check) != 1)
      {
         fprintf(stderr, "LIST::SPLIT::FAILED:\nExpected: {%d, %d, %d, %d}\nGot: ",
         check[0], check[1], check[2], check[3]);
         fprintList(third, stderr);
         llrmlist(&firstList);
         llrmchain(&third, type);
         return 0;
      }
   }
   {
      int check[2] = {-1, 9};
      if (listCheck(firstList.chain, check) != 1)
      {
         fprintf(stderr, "LIST::SPLIT::FAILED:\nExpected: {%d, %d}\nGot: ",
         check[0], check[1]);
         fprintList(firstList.chain, stderr);
         llrmlist(&firstList);
         llrmchain(&third, type);
         return 0;
      }
   }
   llinsertchaini(&firstList, 2, third);
   {
      int check[6] = {-1, 9, 18, 32, 25, 78};
      if (listCheck(firstList.chain, check) != 1)
      {
         fprintf(stderr, "LIST::REVERSE::FAILED:\nExpected: {%d, %d, %d, %d, %d, %d}\nGot: ",
         check[0], check[1], check[2], check[3], check[4], check[5]);
         fprintList(firstList.chain, stderr);
         llrmlist(&firstList);
         return 0;
      }
   }
   llrmlist(&firstList);
   return 1;
}

int main(int argc, char **argv)
{
	int completed = 0;
   completed += listTest(LL_SINGLELINKED);
   completed += listTest(LL_DOUBLELINKED);
   printf("%d/2\n", completed);
	return completed/2 - 1;
}
