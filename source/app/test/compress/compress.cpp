#include <math.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>



struct Node
{
   struct Node*   _next;
   size_t         _len;
   unsigned char* _ptr;
   double         _w;
   size_t         _count;
   bool           _flag;
};

struct Node** ssArray;
struct Node** ssTable;
struct Node** ssTrial;
struct Node** ssBest;

struct Node* matchSS(struct Node* list, size_t len, unsigned char* ss)
{
   struct Node* node;
   for (node = list; node != NULL; node = node->_next)
      if (memcmp(ss, node->_ptr, len) == 0)
         return node;
   return NULL;
}

void printSS(size_t len, unsigned char* ss)
{
   for (size_t i = 0; i < len; i++)
      printf("%c", ss[i]);
}

void compress(size_t len, unsigned char* data)
{

// build array

   ssArray = (struct Node**)malloc(len * sizeof(struct Node*));

   ssTrial = (struct Node**)malloc(len * sizeof(struct Node*));

   ssBest = (struct Node**)malloc(len * sizeof(struct Node*));

   ssTable = (struct Node**)malloc(len * len * sizeof(struct Node*));

   size_t i, j;

   for (i = 0; i < len; i++)
   {
      ssArray[i] = NULL;
      for (j = 0; j < len - i; j++)
      {
         if ((ssTable[i * len + j] = matchSS(ssArray[i], i + 1, data + j)) == NULL)
         {
            struct Node* node = new Node;
            node->_len = i + 1;
            node->_next = ssArray[i];
            node->_ptr = data + j;
            node->_w = 1.0;
            node->_count = 0;
            node->_flag = false;
            ssTable[i * len + j] = ssArray[i] = node;
            printf("%d %d ", i, j);
            printSS(node->_len, node->_ptr);
            printf(";");
         }
      }
   }
}

size_t nodeTotal(size_t len)
{
   size_t pos = 0;
   size_t count = 0;

   while (pos < len)
   {
      struct Node* node = ssTrial[pos];
      if (!node->_flag)
      {
         count += node->_count;
         node->_flag = true;
      }
      pos += node->_len;
   }

   printf("Node total = %d\n", count);
   return count;
}

double entropy(size_t len, size_t count)
{
   size_t pos = 0;
   double res = 0.0;

   while (pos < len)
   {
      struct Node* node = ssTrial[pos];

      if (node->_flag)
      {
         res += -log((double)node->_count / (double)count) / log (2.0);
         res += (node->_len * 8);
         node->_flag = false;
      }
      pos += node->_len;
   }

   printf("Entropy = %g\n", res);
   return res;
}

void adjustWeights(size_t len, double factor)
{
   size_t pos = 0;

   while (pos < len)
   {
      struct Node* node = ssTrial[pos];

      if (node->_count > 0)
      {
         if (factor != 1.0)
         {
            node->_w *= factor;
            printf("%s w to %g for ", factor > 1.0 ? "increasing" : "decreasing", node->_w);
            printSS(node->_len, node->_ptr);
            printf("\n");
          }
      }
      node->_count = 0;
      pos += node->_len;
   }
}

size_t estimate(size_t len)
{
   size_t pos = 0;
   size_t i;

   for (i = 0; i < len; i++)
      ssTrial[i] = NULL;

// compute total weight


   while (pos < len)
   {
      double w = 0.0;
      for (i = 0; i < len - pos; i++)
         w += ssTable[i * len + pos]->_w;
      double x = (w * rand()) / RAND_MAX;

      i = 0;
      while (x > ssTable[i * len + pos]->_w)
      {
         x -= ssTable[i * len + pos]->_w;
         i++;
      }
      struct Node* node = ssTable[i * len + pos];
      printf("selected len %d for pos %d ", node->_len, pos);
      printSS(node->_len, node->_ptr);
      ssTrial[pos] = node;
      node->_count++;
      pos += node->_len;
      printf("\n");
   }

   return nodeTotal(len);
}

size_t estimateBest(size_t len)
{
   size_t pos = 0;
   size_t i;

   for (i = 0; i < len; i++)
      ssTrial[i] = NULL;

   while (pos < len)
   {
      double wMax;
      size_t iMax;

      for (i = 0; i < len - pos; i++)
         if (i == 0 || ssTable[i * len + pos]->_w > wMax)
         {
            iMax = i;
            wMax =  ssTable[i * len + pos]->_w;
         }
 
      struct Node* node = ssTable[iMax * len + pos];
      printf("selected len %d for pos %d ", node->_len, pos);
      printSS(node->_len, node->_ptr);
      ssTrial[pos] = node;
      node->_count++;
      pos += node->_len;
      printf("\n");
   }

   return nodeTotal(len);
}

int n = 0;
double minEntropy;

void best(size_t pos, size_t len)
{
   size_t i;

   for (i = 0; i < len - pos; i++)
   {
      struct Node* node = ssTable[i * len + pos];
      ssTrial[pos] = node;
      node->_count++;
      if (pos == len - 1)
      {
         double x = entropy(len, nodeTotal(len));
         if (n++ == 0 || x < minEntropy)
         {
            for (i = 0; i < len; i++)
               ssBest[i] = ssTrial[i];
            minEntropy = x;
         }
      }
      else
         best(pos + 1, len);
      node->_count--;
   }
}

#define DATA_LEN 8
int main(void)
{
//   compress(DATA_LEN, (unsigned char*)"accbcaaabbccbbca");


//   best(0, DATA_LEN);

   compress(DATA_LEN, (unsigned char*)"abababab");

   printf("\n");
/*
   best(0, DATA_LEN);

   size_t pos = 0;

   while (pos < DATA_LEN)
   {
      struct Node* node = ssBest[pos];
      printSS(node->_len, node->_ptr);
      printf(" ");
      pos += node->_len;
   }
   printf("\n entropy %g\n", minEntropy);
*/


   double best = DATA_LEN < 8;
   for (int i = 0; i < 1000; i++)
   {
      double x = entropy(DATA_LEN, estimate(DATA_LEN));

      adjustWeights(DATA_LEN, x >= best
                    ? 1.0
                    : (x < best
                       ? (best / x > 1.11
                          ? best / x
                          : 1.11)
                       : 0.9));
      if (x < best)
         best = x;
   }

      double x = entropy(DATA_LEN, estimateBest(DATA_LEN));


   return 0;
}