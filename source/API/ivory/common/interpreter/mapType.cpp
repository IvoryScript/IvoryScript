/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name: mapType.cpp
 *
 * Module: Ivory common  (interpreter)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 8 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Function to map a type from one environment to another.
 *
 * Call format:
 *
 *    res = mapType$(type, srcEnv, dstEnv)
 *                     
 *       res      (Type) -          Returned type mapped to destination
 *       type     (Type) -          Type to map
 *       srcEnv   (const Env&) -    Source environment
 *       dstEnv   (Env&) -          Destination environment
 *
 * Method:
 *
 *    Simply returns the given type if the type the type tables match.
 *    Otherwise looks up the source identifier in the destination type table
 *    and returns the result.
 * 
 * Note:
 * 
 *    For a built-in type, an entry is added to the destination table to
 *    ensure persistence consistency.
 *    (i.e. for a change to the built-in set)
 *
 * Errors:
 *
 *    Errors are signalled by a ERROR return value
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
 *
 * Steps:
 *
 *    1. Simple return if built in type or equal source and destination
 *    2. Look up type signature in destination type type
 *    3. If found, return the type result
 *    4. Otherwise
 */

#include "ivory/exec.h"
#include "ivory/iSegmentTable.h"
#include "ivory/segment.h"
#include "ivory/typeTable.h"

Type mapType$(Type type, const Env& srcEnv, Env& dstEnv) {
   assert(&srcEnv != &dstEnv, "mapType$: invalid precondition");
   if (type < builtInTypeCount) {
      (Void)useType(builtInTypeTable().typeSig(type),
                    dstEnv.typeTable(), dstEnv.nameTable());
      return type;
   } else {
      ITypeDescr& srcTypeDescr = *(ITypeDescr*)srcEnv.typeTable().typeDescr(type - builtInTypeCount);
      TypeSig typeSig = srcTypeDescr.typeSig();
      Type res = dstEnv.typeTable().lookUp(typeSig);
      if (res == NULL_TYPE) {
         ITypeDescr& dstTypeDescr = *new(dstEnv.msa()) ITypeDescr(copyTypeSig(typeSig, dstEnv.msa()));
         addTypeConNames(typeSig, dstEnv.nameTable());
         dstTypeDescr._size = srcTypeDescr._size;
         dstTypeDescr._hashVal = srcTypeDescr._hashVal;
         dstEnv.typeTable().enterDescr(&dstTypeDescr);
         res = dstEnv.typeTable().addDescr(&dstTypeDescr);
         dstTypeDescr._denotedType = mapType$(srcTypeDescr._denotedType,
                                             srcEnv, dstEnv);
         if (!(formOf(typeSig) == TYPE_CON &&
            toBody(typeSig, TypeCon)._objectFlag)) {

//--------- Copy segments and assign relocated labels

            if (srcTypeDescr._evalFnSeg != 0) {
               SegmentId segmentId = copySegment(srcTypeDescr._evalFnSeg, dstEnv);
               if (segmentId != NULL_SEGMENT_ID) {
                  dstTypeDescr._evalFnSeg = (IAddress)dstEnv.segmentTable()->segment(segmentId);
                  if (dstTypeDescr._evalFnSeg >= srcTypeDescr._evalFnSeg)
                     dstTypeDescr._evalFnLab = srcTypeDescr._evalFnLab + (dstTypeDescr._evalFnSeg - srcTypeDescr._evalFnSeg);
                  else
                     dstTypeDescr._evalFnLab = srcTypeDescr._evalFnLab - (srcTypeDescr._evalFnSeg - dstTypeDescr._evalFnSeg);
               } else {
                  dstTypeDescr._evalFnSeg = srcTypeDescr._evalFnSeg;
                  dstTypeDescr._evalFnLab = srcTypeDescr._evalFnLab;
               }
               dstTypeDescr._evalFnSegId = segmentId;
            }
            else
               dstTypeDescr._evalFnSeg = 0;

            if (srcTypeDescr._mapFnSeg != 0) {
               SegmentId segmentId = copySegment(srcTypeDescr._mapFnSeg, dstEnv);
               if (segmentId != NULL_SEGMENT_ID) {
                  dstTypeDescr._mapFnSeg = (IAddress)dstEnv.segmentTable()->segment(segmentId);
                  if (dstTypeDescr._mapFnSeg >= srcTypeDescr._mapFnSeg)
                     dstTypeDescr._mapFnLab = srcTypeDescr._mapFnLab + (dstTypeDescr._mapFnSeg - srcTypeDescr._mapFnSeg);
                  else
                     dstTypeDescr._mapFnLab = srcTypeDescr._mapFnLab - (srcTypeDescr._mapFnSeg - dstTypeDescr._mapFnSeg);
               } else {
                  dstTypeDescr._mapFnSeg = srcTypeDescr._mapFnSeg;
                  dstTypeDescr._mapFnLab = srcTypeDescr._mapFnLab;
               }
               dstTypeDescr._mapFnSegId = segmentId;
            }
            else
               dstTypeDescr._mapFnSeg = 0;
  
            if (srcTypeDescr._assignFnSeg != 0) {
               SegmentId segmentId = copySegment(srcTypeDescr._assignFnSeg, dstEnv);
               if (segmentId != NULL_SEGMENT_ID) {
                  dstTypeDescr._assignFnSeg = (IAddress)dstEnv.segmentTable()->segment(segmentId);
                  if (dstTypeDescr._assignFnSeg >= srcTypeDescr._assignFnSeg)
                     dstTypeDescr._assignFnLab = srcTypeDescr._assignFnLab + (dstTypeDescr._assignFnSeg - srcTypeDescr._assignFnSeg);
                  else
                     dstTypeDescr._assignFnLab = srcTypeDescr._assignFnLab - (srcTypeDescr._assignFnSeg - dstTypeDescr._assignFnSeg);
               } else {
                  dstTypeDescr._assignFnSeg = srcTypeDescr._assignFnSeg;
                  dstTypeDescr._assignFnLab = srcTypeDescr._assignFnLab;
               }
               dstTypeDescr._assignFnSegId = segmentId;
            }
            else
               dstTypeDescr._assignFnSeg = 0;

            if (srcTypeDescr._insertTxtFnSeg != 0) {
               SegmentId segmentId = copySegment(srcTypeDescr._insertTxtFnSeg, dstEnv);
               if (segmentId != NULL_SEGMENT_ID) {
                  dstTypeDescr._insertTxtFnSeg = (IAddress)dstEnv.segmentTable()->segment(segmentId);
                  if (dstTypeDescr._insertTxtFnSeg >= srcTypeDescr._insertTxtFnSeg)
                     dstTypeDescr._insertTxtFnLab = srcTypeDescr._insertTxtFnLab + (dstTypeDescr._insertTxtFnSeg - srcTypeDescr._insertTxtFnSeg);
                  else
                     dstTypeDescr._insertTxtFnLab = srcTypeDescr._insertTxtFnLab - (srcTypeDescr._insertTxtFnSeg - dstTypeDescr._insertTxtFnSeg);
               } else {
                  dstTypeDescr._insertTxtFnSeg = srcTypeDescr._insertTxtFnSeg;
                  dstTypeDescr._insertTxtFnLab = srcTypeDescr._insertTxtFnLab;
               }
               dstTypeDescr._insertTxtFnSegId = segmentId;
            }
            else
               dstTypeDescr._insertTxtFnSeg = 0;

#if (SERIALISATION==1)
            if (srcTypeDescr._extractBinFnSeg != 0) {
               SegmentId segmentId = copySegment(srcTypeDescr._extractBinFnSeg, dstEnv);
               if (segmentId != NULL_SEGMENT_ID) {
                  dstTypeDescr._extractBinFnSeg = (IAddress)dstEnv.segmentTable()->segment(segmentId);
                  if (dstTypeDescr._extractBinFnSeg >= srcTypeDescr._extractBinFnSeg)
                     dstTypeDescr._extractBinFnLab = srcTypeDescr._extractBinFnLab + (dstTypeDescr._extractBinFnSeg - srcTypeDescr._extractBinFnSeg);
                  else
                     dstTypeDescr._extractBinFnLab = srcTypeDescr._extractBinFnLab - (srcTypeDescr._extractBinFnSeg - dstTypeDescr._extractBinFnSeg);
               }
               else {
                  dstTypeDescr._extractBinFnSeg = srcTypeDescr._extractBinFnSeg;
                  dstTypeDescr._extractBinFnLab = srcTypeDescr._extractBinFnLab;
               }
               dstTypeDescr._extractBinFnSegId = segmentId;
            }
            else
               dstTypeDescr._extractBinFnSeg = 0;

            if (srcTypeDescr._insertBinFnSeg != 0) {
               SegmentId segmentId = copySegment(srcTypeDescr._insertBinFnSeg, dstEnv);
               if (segmentId != NULL_SEGMENT_ID) {
                  dstTypeDescr._insertBinFnSeg = (IAddress)dstEnv.segmentTable()->segment(segmentId);
                  if (dstTypeDescr._insertBinFnSeg >= srcTypeDescr._insertBinFnSeg)
                     dstTypeDescr._insertBinFnLab = srcTypeDescr._insertBinFnLab + (dstTypeDescr._insertBinFnSeg - srcTypeDescr._insertBinFnSeg);
                  else
                     dstTypeDescr._insertBinFnLab = srcTypeDescr._insertBinFnLab - (srcTypeDescr._insertBinFnSeg - dstTypeDescr._insertBinFnSeg);
               }
               else {
                  dstTypeDescr._insertBinFnSeg = srcTypeDescr._insertBinFnSeg;
                  dstTypeDescr._insertBinFnLab = srcTypeDescr._insertBinFnLab;
               }
               dstTypeDescr._insertBinFnSegId = segmentId;
            }
            else
               dstTypeDescr._insertBinFnSegId = 0;
#endif


#if (GARBAGE_COLLECTION==1)
            if (srcTypeDescr._markFnSeg != 0) {
               SegmentId segmentId = copySegment(srcTypeDescr._markFnSeg, dstEnv);
               if (segmentId != NULL_SEGMENT_ID) {
                  dstTypeDescr._markFnSeg = (IAddress)dstEnv.segmentTable()->segment(segmentId);
                  if (dstTypeDescr._markFnSeg >= srcTypeDescr._markFnSeg)
                     dstTypeDescr._markFnLab = srcTypeDescr._markFnLab + (dstTypeDescr._markFnSeg - srcTypeDescr._markFnSeg);
                  else
                     dstTypeDescr._markFnLab = srcTypeDescr._markFnLab - (srcTypeDescr._markFnSeg - dstTypeDescr._markFnSeg);
               }
               else {
                  dstTypeDescr._markFnSeg = srcTypeDescr._markFnSeg;
                  dstTypeDescr._markFnLab = srcTypeDescr._markFnLab;
               }
               dstTypeDescr._markFnSegId = segmentId;
            }
            else
               dstTypeDescr._markFnSeg = 0;
#endif

         }
      }
      return builtInTypeCount + res;
   }
}
