/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    main_unix.cpp
 *
 * Module:  Ivory common (unix specific)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 18 July 2001
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    'main' function for handling asynchronous input and elapsed time
 *    simultaneously for the unix (Linux) platform.
 *
 * Modification history:
 *
 *------------------------------------------------------------------------------
 *
 * License: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *
 *-----------------------------------------------------------------------------                   
 */

#ifdef unix

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "ivory/unix.h"
#include "ivory/unix/timer.h"

// Global file descriptor list for common 'select' call

FileDescriptorElement* fileDescriptorList = NULL;

void FileDescriptorElement::toList(int fd)
{
   _fd = fd;
   if ((_next = fileDescriptorList) != NULL)
      _next->_prev = this;
   fileDescriptorList = this;
   _prev = NULL;
}

void FileDescriptorElement::fromList(void)
{
   if (_next != NULL)
      _next->_prev = _prev;
   if (_prev != NULL)
      _prev->_next = _next;
   else
      fileDescriptorList = _next;
}

void main_unix(void)
{
   unsigned int usecDelta = 0;
   struct timeval prevTime;
   gettimeofday(&prevTime, 0);

   while (true)
   {
      fd_set readFDS;
      fd_set writeFDS;
      fd_set exceptFDS;

      FD_ZERO(&readFDS);
      FD_ZERO(&writeFDS);
      FD_ZERO(&exceptFDS);
      FileDescriptorElement* fde;
      int maxFD = -1;
      for (fde = fileDescriptorList; fde != NULL; fde = fde->_next)
      {
         if (fde->_fd > maxFD)
            maxFD = fde->_fd;
         if (fde->_read)
            FD_SET(fde->_fd, &readFDS);
         if (fde->_write)
            FD_SET(fde->_fd, &writeFDS);
         if (fde->_except)
            FD_SET(fde->_fd, &exceptFDS);
      }

      unsigned int delay;
      struct timeval tv;
      struct timeval* timeout;
 
      if (mainTimeout_unix(delay))
      {
         tv.tv_sec  = delay / 1000;
         tv.tv_usec = (delay % 1000) * 1000;
         timeout = &tv;
      }
      else
         timeout = NULL;

      if (select(maxFD + 1, &readFDS, &writeFDS, &exceptFDS, timeout) > 0)
         for (fde = fileDescriptorList; fde != NULL; fde = fde->_next)
         {
            int state = 0;
            if (FD_ISSET(fde->_fd, &readFDS))
               state |= FileDescriptorElement::READ;
            if (FD_ISSET(fde->_fd, &writeFDS))
               state |= FileDescriptorElement::WRITE;
            if (FD_ISSET(fde->_fd, &exceptFDS))
               state |= FileDescriptorElement::EXCEPT;

            if (state != 0)
            {
               fde->onSelect(state);

//------------ Break out at this point in case the list has changed.
//------------ Might consider looping here (restarting at the list head)

               break;
            }
         }

      struct timeval currTime;
      gettimeofday(&currTime, 0);

      struct timeval timeDiff;
      timeDiff.tv_sec = currTime.tv_sec - prevTime.tv_sec;
      if (currTime.tv_usec >= prevTime.tv_usec)
         timeDiff.tv_usec = currTime.tv_usec - prevTime.tv_usec;
      else
      {
         timeDiff.tv_usec = (1000000 + currTime.tv_usec) - prevTime.tv_usec;
         timeDiff.tv_sec -= 1;
      }

      if ((timeDiff.tv_usec += usecDelta) >= 1000000)
      {
         timeDiff.tv_usec -=1000000;
         timeDiff.tv_sec += 1;
      }
      checkTimers_unix(timeDiff.tv_sec * 1000 + timeDiff.tv_usec / 1000);
      usecDelta = timeDiff.tv_usec % 1000;

      prevTime.tv_sec  = currTime.tv_sec;
      prevTime.tv_usec = currTime.tv_usec;
   }

}

#endif /* unix */
