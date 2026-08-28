/*------------------------------------------------------------------------------
 *
 * (c) Copyright (see Date) by Alasdair Scott
 *
 * Name:    time.h
 *
 * Author:  A Scott
 *
 * Date:    15 January 2003
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    '#include' and function call re-writes for time functions
 *    under Windows CE.  For example, replace:
 *
 *			#include <time.h>
 *
 *    with:
 *
 *			#ifndef _WIN32_WCE
 *			#include <time.h>
 *			#else
 *			#include <WinCE/time.h>
 *			#endif
 *
 * Modification history:
 *
 *------------------------------------------------------------------------------
 */

#ifndef WCE_TIME_H_DEFINED
 #define WCE_TIME_H_DEFINED
//Don't use:  #include <afx.h>  Instead, copy relevant lines.

//mktime(), time(), gmtime(), strftime(), localtime()
//are not supported, so make substitutions:
  #define mktime(t)		wce_mktime(t)
  #define time(t)		wce_time(NULL)
  #define localtime(t)	wce_localtime(t)

  //gmtime() has no direct equivalent, so this will
  //need revising to remove localisation effect:
  #define gmtime(t)		wce_localtime(t)

  //strftime() has no direct equivalent either, so this will
  //need revising to control time output formatting:

    //This doesn't work:
    //#define strftime(dest, maxsize, format, timeptr) strlen(strcpy(dest, wce_ctime(wce_mktime(timeptr))))

    //Instead, using sprintf:
    #define strftime(dest, maxsize, format, timeptr) sprintf(dest, "%.2d/%.2d/%.2d %.2d:%.2d:%.2d", timeptr->tm_mday, timeptr->tm_mon+1, timeptr->tm_year, timeptr->tm_hour, timeptr->tm_min, timeptr->tm_sec)



//From stdlib.h:
  #ifndef _TIME_T_DEFINED
  typedef unsigned long  time_t;
  #define _TIME_T_DEFINED     /* avoid multiple def's of time_t */
  #endif



//From MS Pocket PC\mfc\include\afxver_.h:
// AFXAPI is used on global public functions
  #ifndef AFXAPI
	#define AFXAPI __stdcall
  #endif



//From MS Pocket PC\mfc\include\wcealt.h:
// WCETIME.CPP
  #ifndef _TM_DEFINED
    #define _TM_DEFINED
    struct tm {
		int tm_sec;     /* seconds after the minute - [0,59] */
		int tm_min;     /* minutes after the hour - [0,59] */
		int tm_hour;    /* hours since midnight - [0,23] */
        int tm_mday;    /* day of the month - [1,31] */
        int tm_mon;     /* months since January - [0,11] */
        int tm_year;    /* years since 1900 */
        int tm_wday;    /* days since Sunday - [0,6] */
        int tm_yday;    /* days since January 1 - [0,365] */
        int tm_isdst;   /* daylight savings time flag */
        };
    #endif // _TM_DEFINED


  //All known WCE equivalents to C time functions:
    long        AFXAPI wce_GetMessageTime();
    time_t      AFXAPI wce_mktime(struct tm* );
    struct tm * AFXAPI wce_localtime(const time_t *);
    char*       AFXAPI wce_ctime(const time_t* );
    time_t		AFXAPI wce_time(time_t *);

 static time_t _dummy_time_t = 1;
#endif
