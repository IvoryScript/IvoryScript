/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    ref.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 17 April 2002
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with teh Ivory 'Ref' data type
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

#ifndef IVORY_REF_H_DEFINED
#define IVORY_REF_H_DEFINED

#include "ivory/builtInDataCon.h"
#include "ivory/builtInFn.h"
#include "ivory/type.h"

declareBuiltInDataCon(NullRef);

#define NULL_REF     ((Ref)NULL_OID)
#define ADS_REF_MASK 2

declareTypeCon(Ref);

declareType(Ref);

#define sizeOfRef sizeof(Ref)

#define typeSpecRef Ref

#define argRegRef rRef

#define stackFPSizeRef (stackSlotSize(Ref)+stackSlotSize(Env))

#define loadArgRegRef(name)\
Ref name=rRef;\
Env* name##Env=rEnv

#define loadArgRef(name,off)\
Env* name##Env=stack(off+stackSlotSize(Ref),Env*);\
Ref name=stack((off),Ref)

#define storeArgRef(name,off)\
stack(off+stackSlotSize(Ref),Env*)=name##Env;\
stack(off,Ref)=name

#define map_Ref(name,srcEnv,dstEnv) name

class ADS_Instance;

class ADSRef {
public:
   inline ADSRef()
      : _adsi(NULL) {
   }
   inline ADSRef(Ref ref, ADS_Instance* adsi)
      :  _ref(ref), _adsi(adsi) {
   }
   void* operator new(size_t size, MSA& msa) {
      return msa.alloc(size);
   }
   inline void* operator new(size_t size, void* ptr) {
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline void operator delete(void* ptr, MSA& msa)
   {
   }
   inline void operator delete(void* ptr1, void* ptr2)
#else
   inline void operator delete(void* ptr)
#endif
   {
   }
   inline bool isNull() const {
      return _adsi == NULL && _ref == NULL_REF;
   }
   Ref               _ref;
   ADS_Instance*     _adsi;

   friend inline bool operator == (const ADSRef& l, const ADSRef& r) {
      return l._adsi == r._adsi && l._ref == r._ref;
   }
   friend inline bool operator != (const ADSRef& l, const ADSRef& r) {
      return l._adsi != r._adsi || l._ref != r._ref;
   }
};

//class UnionRef;
class ExtADSRef : public ADSRef
{
   ExtADSRef(Ref ref, ADS_Instance* adsi)
      : ADSRef(ref, adsi), _refCount(1)
   {
   }
   ExtADSRef(Ref ref, ADS_Instance* adsi, UInt refCount)
      : ADSRef(ref, adsi), _refCount(refCount)
   {
   }
protected:
   UInt  _refCount;

//   friend class UnionRef;
};

// UnionRef (either Ref or ExtADSRef)
// Used to represent Ref within a structured type
// Shares primary expression tag
// Assumes sizeof(UInt32) >= max(sizeof(ExtADSRef*, Ref))
/*
class UnionRef
{
public:
   inline
   UnionRef(Void)
      : _repr(NULL_OID)
   {
   }
   inline
   UnionRef(UInt32 repr)
      : _repr(repr) 
   {
   }
   inline
   UnionRef(ExtADSRef* adsr)
      : _repr((UInt32)adsr | ADS_REF_MASK) 
   {
   }
   inline
   UnionRef(Ref ref)
      : _repr(ref)
   {
   }
   UnionRef(const Ref      src, const Env& srcEnv, Env& env, MSA& msa);
   UnionRef(const UnionRef src, const Env& srcEnv, Env& env, MSA& msa);
   inline Void* operator new(size_t size, Void* ptr)
   {
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2)
#else
   inline Void operator delete(Void* ptr)
#endif
   {
   }
   Void destroy(Env& env, MSA& msa);
   inline
   operator Ref(Void)
   {
      return (Ref)_repr;
   }
   operator Ref(Void) const
   {
      return (const Ref)_repr;
   }
   inline
   operator ExtADSRef&(Void)
   {
      return *(ExtADSRef*)(_repr & ~ADS_REF_MASK);
   }
   inline
   operator const ExtADSRef&(Void) const
   {
      return *(const ExtADSRef*)(_repr & ~ADS_REF_MASK);
   }
   inline
   UInt32 repr(Void)
   {
      return _repr;
   }
   UInt32 repr(Void) const
   {
      return _repr;
   }
   Void incRefCount(ADS_Instance* const adsi, MSA& msa);
   UInt decRefCount(ADS_Instance* const adsi, MSA& msa);

   Void print(ostream& os, const Env& env) const;

//protected:
   UInt32 _repr;
protected:

   friend bool operator == (UnionRef l, UnionRef r);
   friend bool operator != (UnionRef l, UnionRef r);
};
*/


extern Bool isNullRef(Expr r);
extern Expr loadADSRef(Archive& archive, MSA& msa);
extern void storeADSRef(Archive& archive, const ADSRef& adsr);
extern void fixUpRefs(struct ADSRefNode* node, ADS_Instance* adsi);

// Structure to support forward ADS references

struct ADSRefNode
{
   struct ADSRefNode*   _next;
   Ref                  _ref;
   ADSRef*              _adsRefList;
   struct ADSRefNode*   _childList;
};

#define isExtUnionRef(unionRef) (((unionRef).repr()&ADS_REF_MASK)!=0)

#define returnRef(ref,env){\
rRef=(ref);\
rEnv=(env);\
jump(popLabel());}

extern Ref checkRef$(argVM);
#define checkRef() checkRef$(vm)

extern Void update_Ref(argVM);
#define updateRef()\
if(updatePtr!=NULL){update_Ref(vm);updatePtr=NULL;}

declareBuiltInFn(eqRef);

declareBuiltInFn(nEqRef);

declareBuiltInFn(matchRef);

declareBuiltInFn(selectRef);

declareBuiltInFn(showRef);

declareBuiltInFn(insertTxtRef);

extern Expr mkADSRef(ADS_Instance* adsi, Ref ref, MSA& msa);

//extern Void returnCopy(UnionRef* ptr, Env& env, MSA& msa argN_VM);

#endif /* IVORY_REF_H_DEFINED */
