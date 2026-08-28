/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    segmentTable.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 22 August 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with the ISegmentTable class.
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

#ifndef I_SEGMENT_TABLE_H_DEFINED
#define I_SEGMENT_TABLE_H_DEFINED

#include "nameTable.h"
#include "../segmentTable.h"
#include "ivory/typeTable.h"

class ISegmentTable : public SegmentTable {

public:
   ISegmentTable(unsigned int nSlots, unsigned int nPerSeg, MSA& msa);

   SegmentId lookUp(const Byte * segment, size_t len, unsigned int hashVal);

   Void load(Archive& archive,
             const NameTable& nameTable,
             const TypeTable& typeTable,
             MSA& msa);
   Void postLoad(Env& env);
   Void store(Archive& archive,
              const NameTable& nameTable,
              const TypeTable& typeTable) const;

protected:
   virtual int match(const Byte* segment1, size_t len1,
                     const Byte* segment2, size_t len2) const;
};

#endif /* I_SEGMENT_TABLE_H_DEFINED */
