// main.cpp : Test program for persistent name table
//

#include <fcntl.h>
#include <io.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "general.h"
#include "PSA.h"
#include "persistentNameTable.h"

#define PAGE_SIZE 256

class FilePSA : public PSA
{
public:
   FilePSA(const char* filename);
   PageNum allocPage();
   int readPage(PageNum pageNum, size_t len, void* ptr);
   int writePage(PageNum pageNum, size_t len, void* ptr);
   inline bool existed(void)
   {
      return _existed;
   }

protected:
   char* _filename;
   int _fd;
   bool _existed;

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

FilePSA::FilePSA(const char* filename)
   : PSA(1024, 16)
{
   _filename = (char*)_msa.alloc(strlen(filename + 1));

   strcpy(_filename, filename);

   if (_access(filename, 0) < 0)
   {
      _pageNum = 0;
      create(PAGE_SIZE);
      _existed = false;
   }
   else
   {
      struct stat stat_buf;
      if (open() < 0 ||
          stat(filename, &stat_buf) < 0)
         exit(0);
      _pageNum = stat_buf.st_size / _pageSize;
      _existed = true;
   }
}

PageNum FilePSA::allocPage(void)
{
   return _pageNum++;
}

int FilePSA::readPage(PageNum pageNum, size_t len, void* ptr)
{
   if ((_fd = _open(_filename, _O_BINARY | _O_RDONLY)) == -1)
   {
      error("Open failed");
      return -1;
   }

   _lseek(_fd, pageNum * _pageSize, SEEK_SET);

   int res = _read(_fd, ptr, len);
   _close(_fd);
   return res;
}

static const char* dummy = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

int FilePSA::writePage(PageNum pageNum, size_t len, void* ptr)
{
   if ((_fd = _open(_filename, _O_BINARY | _O_CREAT | _O_RDWR | _O_RANDOM,
                               _S_IWRITE)) == -1)
   {
      int err = errno;
      error("writePage: open failed");
      return -1;
   }

   _lseek(_fd, pageNum * _pageSize, SEEK_SET);

   int res = _write(_fd, ptr, len);
   _close(_fd);
   return res;
}

int nAlloc = 0;
size_t bytes = 0;
int nFree  = 0;

#include <sstream>
#include <iosfwd>

int main(int argc, char* argv[])
{
   printf("Starting\n");

   MSA msa(256);
   FilePSA psa("test.ips");

   PersistentNameTable pnt(47, 32, msa, psa);

   if (psa.existed())
   {
      pnt.open(1);

//      const char* id = pnt.ident(0);



      Name name1 = pnt.lookUp("sidney");
      Name name2 = pnt.lookUp("kate");
      Name name3 = pnt.lookUp("bill");
      Name name4 = pnt.lookUp("jim");
      Name name5 = pnt.lookUp("fred");
      Name name6 = pnt.lookUp("joe");

      //pnt.add("fred", hashString("joe"));
   }
   else
   {
      pnt.create();

      pnt.add("joe", hashString("joe"));

      pnt.add("fred", hashString("joe"));

      pnt.add("jim", hashString("joe"));

      pnt.add("bill", hashString("joe"));
      pnt.add("kate", hashString("joe"));
            pnt.add("sidney", hashString("joe"));



      pnt.flush();

   }



   psa.close();

   printf("Done\n");

	return 0;
}

