/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    lock.cpp
 *
 * Module:  Object store class
 *
 * Author:  Alasdair Scott
 *
 * Original date: 7 December 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Lock an object and return a pointer to the object data.
 *
 * Call format:
 *
 *    ptr = lock(oid);
 *
 *       ptr -    (void*)     - Returned pointer to object data
 *       oid -    (OID)       - Object identifier
 *
 * Method:
 *     
 * Errors:
 *
 *    None.
 *
 * Notes:
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

#include "objStore.h"
/*
void* ObjStore::lock(OID oid)
{
   return oid != OID_NULL ? _clustTbl[oid.ci()]._tblSeg[oid.tsi()]
                          : NULL;
}
*/
