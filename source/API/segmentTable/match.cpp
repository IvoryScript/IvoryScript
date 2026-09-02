/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    match.cpp
 *
 * Module:  Segment Table Class
 *
 * Author:  Alasdair Scott
 *
 * Original date: 2 February 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Default method to compare two segments for a match
 *
 * Call format:
 *
 *    res = match(segment1, len1, segment2, len2);
 *
 * Method:
 *
 *    Returns 1 if len1 = len2 and bytewise match between segment1 and segment2
 *    otherwise 0
 *     
 * Errors:
 *
 *    Errors are signalled with a return value of NULL_SEGMENT_ID or NULL.
 *
 * Notes:
 *
 *    This method may be overridden for different match semantics
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

#include "segmentTable.h"
#include <string.h>

int SegmentTable::match(const Byte* segment1, size_t len1,
                        const Byte* segment2, size_t len2) const {
   return len1 == len2 && memcmp(segment1, segment2, len1) == 0
      ? 1
      : 0; 
}
