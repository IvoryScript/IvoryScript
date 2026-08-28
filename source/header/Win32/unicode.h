/*-----------------------------------------------------------------------------
 *
 * (c) Copyright (see Date) by Alasdair Scott
 *
 * Name:    unicode.h
 *
 * Author:  A Scott
 *
 * Date:    22 January 2003
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Win32 specific definitions for ANSI / UNICODE conversion.
 *
 * Modification history:
 *
 *-----------------------------------------------------------------------------
 */

#if defined( _WIN32_WCE )
 #define TEST_UNICODE _ansiToUnicode
 #include <afx.h>

 extern CString _ansiToUnicode(const char* ansiIn);

#else
 #define TEST_UNICODE
#endif
