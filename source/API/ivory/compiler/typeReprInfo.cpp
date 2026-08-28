/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    typeReprInfo.cpp
 *
 * Module:  IvoryScript compiler
 *
 * Author:  Alasdair Scott
 *
 * Original date: 20 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Function to return the representation inforrmation for a type.
 *
 * Call format:
 * 
 *    res = typeReprInfo(typeSig, typeTable, nameTable)
 *       res         (ReprInfo)        - Returned representation information
 *       typeSig     (TypeSig)         - Type signature
 *       typeTable   (TypeTable&)      - Type table
 *       nameTable   (NameTable&)      - Name table
 *
 * Method:
 *
 * Errors:
 *
 *    None
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

#include "ivory/fileIO.h"
#include "ivory/repr.h"
#include "ivory/streams.h"
#include "ivory/compiler/type.h"

declareTypeCon(ADS_Instance); 

declareTypeCon(Array);

declareTypeCon(Bits);

declareTypeCon(Bool);

declareTypeCon(Byte);

declareTypeCon(ByteString);

declareTypeCon(Char);

declareTypeCon(Dir);

declareTypeCon(DirEntry);

declareTypeCon(Double);

declareTypeCon(Env);

declareTypeCon(Int);

declareTypeCon(Float);

declareTypeCon(List);

declareTypeCon(Name);

declareTypeCon(Plain);

declareTypeCon(Ptr);

declareTypeCon(Ref);

declareTypeCon(String);

declareTypeCon(SerialContext);

declareTypeCon(UTC);

declareTypeCon(Void);

extern size_t sizeOfRepr(Repr repr);

static ReprInfo tupleTypeReprInfo(TypeSig typeSig, TypeSig hfTypeSig,
                                  TypeTable& typeTable, NameTable& nameTable) {
   MSA& msa = typeTable.msa();
   UInt nCompReprs = tupleCard(hfTypeSig);
   Repr* compReprV = (Repr*)msa.alloc(nCompReprs * sizeof(Repr));
   size_t* compSizeV = (size_t*)msa.alloc(nCompReprs * sizeof(size_t));
   size_t* compOffsetV = (size_t*)msa.alloc(nCompReprs * sizeof(size_t));

   Bool needsEnv = FALSE;
   UInt i = nCompReprs;
   while (isAp(typeSig)) {
      ReprInfo compReprInfo = typeReprInfo(arg(typeSig), typeTable, nameTable);
      compReprV[--i] = compReprInfo._repr;
      if (compReprInfo._needsEnv)
         needsEnv = TRUE;
      typeSig = fun(typeSig);
   }
   StructRepr* structRepr = new(msa) StructRepr(nCompReprs, compReprV, compSizeV, compOffsetV);
   return { Repr(structRepr), needsEnv };
}

// Sum types are represented by a structure: (TAG, <union of constructor representations>)

static ReprInfo unionTypeReprInfo(TypeSig typeSig, TypeCon& typeCon,
                                  TypeTable& typeTable, NameTable& nameTable) {
   Repr* reprV = (Repr*)typeTable.msa().alloc(typeCon.n() * sizeof(Repr));

   Bool needsEnv = FALSE;
   UInt nNonVoid = 0;
   for (Tag tag = 0; tag < typeCon.n(); tag++) {
      TypeSig dataConTypeSig = reprTypeSig(typeSig, tag, typeTable.msa());
      ReprInfo dataConReprInfo = typeReprInfo(dataConTypeSig, typeTable, nameTable);
      reprV[tag] = dataConReprInfo._repr;
      if (dataConReprInfo._repr != Repr::REPR_VOID)
         nNonVoid++;
      if (dataConReprInfo._needsEnv)
         needsEnv = TRUE;
   }
 
   if (nNonVoid > 0) {
      MSA& msa = typeTable.msa();
      Repr sndRepr = Repr(new(typeTable.msa()) UnionRepr(typeCon.n(), reprV));
      Repr* compReprV = (Repr*)msa.alloc(2 * sizeof(Repr));
      size_t* compSizeV = (size_t*)msa.alloc(2 * sizeof(size_t));
      size_t* compOffsetV = (size_t*)msa.alloc(2 * sizeof(size_t));
      compReprV[0] = Repr::REPR_TAG;
      compReprV[1] = sndRepr;
      StructRepr* structRepr = new(msa) StructRepr(2, compReprV, compSizeV, compOffsetV);
      return { Repr(structRepr), needsEnv };
   }
   else
      return { Repr::REPR_TAG, FALSE };
}

ReprInfo typeReprInfo(TypeSig typeSig, TypeTable& typeTable,
                                       NameTable& nameTable) {
   TypeDescr& typeDescr = typeDescriptor(useType(typeSig, typeTable, nameTable),
                                         typeTable);
   Repr repr = typeDescr.repr();
   if (!repr.isUnknown())
      return { repr, typeDescr.needsEnv() };
   Bool needsEnv = FALSE;

   if (isArrowTypeSig(typeSig)) {
      repr = Repr::REPR_CELL_PTR;
      needsEnv = TRUE;
   } else {
      TypeSig hfTypeSig = headForm(typeSig);
      switch (formOf(hfTypeSig)) {
         case NAME:
            repr = Repr::REPR_UNKNOWN;
            break;

         case VOID:
            repr = Repr::REPR_VOID;
            break;

         case ENV_PTR:
            repr = Repr::REPR_ENV_PTR;
            break;

         case LABEL:
            repr = Repr::REPR_LABEL;
            break;

         case CELL_INFO:
            repr = Repr::REPR_CELL_INFO_PTR;
            break;

         case CELL_PTR:
            repr = Repr::REPR_CELL_PTR;
            needsEnv = TRUE;
            break;

         case TAG:
            repr = Repr::REPR_TAG;
            break;

         case TYPE_CON:
            if (typeSig == typeCon(Void))
               repr = Repr::REPR_VOID;
            else if (typeSig == typeCon(Expr) || hfTypeSig == typeCon(Exp)) {
               repr = Repr::REPR_EXPR;
               needsEnv = TRUE;
            } 
            else if (typeSig == typeCon(Env))
               repr = Repr::REPR_ENV_PTR;
            else if (typeSig == typeCon(Name)) {
               repr = Repr::REPR_NAME;
               needsEnv = TRUE;
            }
            else if (typeSig == typeCon(Type)) {
               repr = Repr::REPR_TYPE;
               needsEnv = TRUE;
            } else if (typeSig == typeCon(Byte))
               repr = Repr::REPR_BYTE;
            else if (typeSig == typeCon(Bits))
               repr = Repr::REPR_BITS;
            else if (typeSig == typeCon(Int))
               repr = Repr::REPR_INT;
            else if (typeSig == typeCon(Float))
               repr = Repr::REPR_FLOAT;
            else if (typeSig == typeCon(Double))
               repr = Repr::REPR_DOUBLE;
            else if (typeSig == typeCon(Char))
               repr = Repr::REPR_CHAR;
            else if (typeSig == typeCon(String)) {
               repr = Repr::REPR_STRING;
               needsEnv = TRUE;
            } else if (typeSig == typeCon(UTC))
               repr = Repr::REPR_UTC;
            else if (typeSig == typeCon(Dir))
               repr = Repr::REPR_DIR;
            else if (typeSig == typeCon(DirEntry))
               repr = Repr::REPR_DIR_ENTRY;
            else if (hfTypeSig == typeCon(Plain)) {
               repr = typeReprInfo(arg(typeSig), typeTable, nameTable)._repr;
               needsEnv = FALSE;
            } 
            else if (hfTypeSig == typeCon(Ptr)) {

//------------ Pointer type representations may be recursive
//------------ So populate the type table slot first 

               PtrRepr* ptrRepr = new(typeTable.msa()) PtrRepr(Repr::REPR_UNKNOWN);
               typeDescr.setRepr(ptrRepr);
               typeDescr.setNeedsEnv(TRUE);
               ReprInfo reprInfo = typeReprInfo(arg(typeSig), typeTable, nameTable);

               ptrRepr->repr() = reprInfo._repr;
               return { typeDescr.repr(), TRUE };
            }
            else if (typeSig == typeCon(Bool) ||
                     typeSig == typeCon(IO_Mode))
               return { Repr::REPR_TAG, FALSE };
            else if (typeSig == typeCon(FileHandle))
               return { Repr::REPR_FILE_HANDLE, FALSE };
            else if (typeSig == typeCon(SerialContext))
               return { Repr::REPR_SERIAL_CONTEXT, FALSE };
            else if (hfTypeSig == typeCon(Array)) {
               ReprInfo elementReprInfo = typeReprInfo(arg(typeSig), typeTable, nameTable);
               repr = Repr(new(typeTable.msa()) ArrayRepr(1, elementReprInfo._repr));
               needsEnv = elementReprInfo._needsEnv;
               break;
            }
            else {
               TypeCon& tc = toBody(hfTypeSig, TypeCon);
               Bool isList = &toBody(typeCon(List), TypeCon) == &tc;
               ReprInfo reprInfo = tc.n() == 1 || isList
                  ? typeReprInfo(reprTypeSig(typeSig, !isList ? 0 : 1, typeTable.msa()),
                                 typeTable, nameTable)
                  : unionTypeReprInfo(typeSig, tc, typeTable, nameTable);
               repr = reprInfo._repr;
               needsEnv = reprInfo._needsEnv;
            }
            break;

         case TUPLE: {
            ReprInfo reprInfo = tupleTypeReprInfo(typeSig, hfTypeSig,
                                                  typeTable, nameTable);
            repr = reprInfo._repr;
            needsEnv = reprInfo._needsEnv;
            break;
         }

         default:
            error("typeRepr: unexpected");
            repr = Repr::REPR_UNKNOWN;
            break;
      }
   }

   typeDescr.setRepr(repr);
   typeDescr.setNeedsEnv(needsEnv);
   return { repr, needsEnv };
}
