/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    refList.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 24 March 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with the Ivory 'RefList' type
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

#ifndef IVORY_REF_LIST_H_DEFINED
#define IVORY_REF_LIST_H_DEFINED

#include "ivory/ref.h"
#include "ivory/vector.h"

class RefList : public Vector<Ref> {
public:
   RefList(Void)
      : Vector<Ref>() {
   }
   RefList(const RefList& src, const Env& srcEnv, Env& env, MSA& msa)
      : Vector<Ref>(src, srcEnv, env, msa) {
   }
   inline
   Bool memberOf(Ref ref, const ADS_Instance* adsi, const Env& env) const {
      return search(ref, adsi, env) >= 0;
   }
   Bool addUnique(Ref ref, const ADS_Instance* adsi, Env& env, MSA& msa);
   Void addAt(Ref ref, const ADS_Instance* adsi,
              UInt16 index, Env& env, MSA& msa);
   Void add(Ref ref, const ADS_Instance* adsi, Env& env, MSA& msa);
   Void rem(Ref ref, const ADS_Instance* adsi, Env& env, MSA& msa);

   Int search(Ref ref, const ADS_Instance* adsi, const Env& env) const;
};

declareTypeCon(RefList);

declareType(RefList);

declareDataCon(RefList);

declareBuiltInFn(showRefList);

declareBuiltInFn(lengthRefList);

declareBuiltInFn(getAtRefList);

extern RefList& checkRefList$(argVM);
#define checkRefList() checkRefList$(vm)

extern Void update_RefList(argVM);

#define updateRefList()\
if(updatePtr!=NULL){update_RefList(vm);updatePtr=NULL;}

declareBuiltInFn(firstRef);

declareBuiltInFn(foldRRefs);

declareBuiltInFn(hasRef);

declareBuiltInFn(mapProcRefs);

declareBuiltInFn(mapPredRefs);

declareBuiltInFn(referrer);

declareBuiltInFn(revRefs);

declareBuiltInFn(revRefsOfType);

extern Expr fromRefList(RefList& refList, Env& env, MSA& msa);

#endif /* IVORY_REF_LIST_H_DEFINED */
