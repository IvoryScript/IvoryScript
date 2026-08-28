// PSATest.cpp : Defines the entry point for the console application.
//

#if defined(WIN32) || defined (_WIN32_WCE)
#include <fcntl.h>
#include <io.h>
#else
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#endif

#if defined(WIN32) || defined (_WIN32_WCE)
#define access _access
#define open _open
#define close _close
#define read _read
#define write _write
#define lseek _lseek
#endif

#include "PSA.h"

#define PAGE_SIZE 16384

//#define FREE_OLDEST
//#define NO_RECORD

#define N_ITERATIONS 100000

class FilePSA : public PSA
{
public:
   FilePSA(MSA& msa);
   PageNum allocPage();
   int readPage(PageNum pageNum, size_t len, void* ptr);
   int writePage(PageNum pageNum, size_t len, void* ptr);

protected:
   char* _filename;
   int _fd;

   PageNum _pageNum;
};

void error(const char* s)
{
	printf("%s\n", s);
}

void trace(const char* s)
{
	printf("%s\n", s);
}

struct PIDRecord
{
   struct PIDRecord* _prev;
   struct PIDRecord* _next;
   PID               _pid;
};

struct PIDRecord* head = NULL;
struct PIDRecord* tail = NULL;


FilePSA::FilePSA(MSA& msa)
   : PSA(PSA::BufferManager(4), msa)
{
   _filename = (char*)_msa.alloc(strlen(filename + 1));

   strcpy(_filename, filename);

   if (access(filename, 0) < 0)
   {
      _pageNum = 0;
      create(PAGE_SIZE);
   }
   else
   {
      struct stat stat_buf;
      if (open() < 0 ||
          stat(filename, &stat_buf) < 0)
         exit(0);
      _pageNum = stat_buf.st_size / _pageSize;
   }
}

PageNum FilePSA::allocPage(void)
{
   return _pageNum++;
}

int FilePSA::readPage(PageNum pageNum, size_t len, void* ptr)
{
   if ((_fd = open(_filename, _O_BINARY | _O_RDONLY)) == -1)
   {
      error("Open failed");
      return -1;
   }

   lseek(_fd, pageNum * _pageSize, SEEK_SET);

   int res = read(_fd, ptr, len);
   close(_fd);
   return res;
}

static const char* dummy = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

int FilePSA::writePage(PageNum pageNum, size_t len, void* ptr)
{
   if ((_fd = open(_filename, _O_BINARY | _O_CREAT | _O_RDWR | _O_RANDOM,
                               _S_IWRITE)) == -1)
   {
      int err = errno;
      error("writePage: open failed");
      return -1;
   }

   lseek(_fd, pageNum * _pageSize, SEEK_SET);

   int res = write(_fd, ptr, len);
   close(_fd);
   return res;
}

int nAlloc = 0;
size_t bytes = 0;
int nFree  = 0;

int main(int argc, char* argv[])
{
   printf("Starting\n");
   FilePSA psa("test.ips");
/*

   PID pid1 = psa.alloc(5,  "fred");
   PID pid2 = psa.alloc(5,  "bill");
   PID pid3 = psa.alloc(21, "12345678901234567890");
   psa.free(pid2);
   PID pid4 = psa.alloc(64, "..........The quick brown fox jumps over the lazy dog..........");
   PID pid5 = psa.alloc(5,  "katy");

   char buf[44];

   psa.read(pid4, 30, 24, buf);

*/
   for (int i = 0; i < N_ITERATIONS; i++)
   {
      if (i % 1000 == 0)
         printf("%d, %d allocated, %d freed, bytes %d\n", i, nAlloc, nFree, bytes);
      int n = rand();
      PID pid;
#ifdef NO_RECORD
      if (true)
#else
      if (n % 5 != 0)
#endif
      {
         size_t len = (rand() * rand()) % 20000;

         bytes += len;

//         printf("alloc: len = %d\n", len);
         pid = psa.alloc(len/*, dummy*/);
//         printf("PID = %d\n", pid.repr());
         nAlloc++;


#ifndef NO_RECORD
         PIDRecord* rec = new PIDRecord;
         if ((rec->_prev = tail) == NULL)
            head = rec;
         else
            tail->_next = rec;
         rec->_next = NULL;
         tail = rec;
         rec->_pid = pid;
#endif
      }
      else
      {
         if (head != NULL)
         {
            PIDRecord* rec = head;

#ifdef FREE_OLDEST

//           printf("free: PID = %d\n", rec->_pid.repr());
            bytes -= psa.length(rec->_pid);
            psa.free(rec->_pid);
            if ((head = rec->_next) != NULL)
               head->_prev = NULL;
            else
               tail = NULL;
#else

            int n = (rand() * rand()) % (nAlloc - nFree);
            for (int j = 0; j < n; j++)
               rec = rec->_next;
//            printf("free: PID = %d\n", rec->_pid.repr());

            bytes -= psa.length(rec->_pid);
            psa.free(rec->_pid);
#endif
            nFree++;

            if (rec->_prev == NULL)
               head = rec->_next;
            else
               rec->_prev->_next = rec->_next;
            if (rec->_next == NULL)
               tail = rec->_prev;
            else
               rec->_next->_prev = rec->_prev;
            delete rec;
         }
      }

   }
   printf("%d, %d allocated, %d freed, bytes %d\n", i, nAlloc, nFree, bytes);

   psa.close();

   printf("Done\n");

	return 0;
}

