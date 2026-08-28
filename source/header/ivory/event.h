/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    event.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 08 September 2000
*
*-----------------------------------------------------------------------------
*
* Description:
*
*    Definitions for use with common event handling.
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

#ifndef IVORY_EVENT_H_DEFINED
#define IVORY_EVENT_H_DEFINED

#include "ivory/builtInFn.h"
#include "ivory/common.h"
#include "ivory/ref.h"

/*----------------------------------------------------------------------------*/

declareTypeCon(EventPhase);

declareType(EventPhase);

class EventPhase {
protected:
   EventPhase(Void);

   inline void* operator new(size_t size, void* ptr) {
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline void operator delete(void* ptr1, void* ptr2)
#else
   inline void operator delete(void* ptr)
#endif
   {
   }

   void evalActions(argVM);

   Expr  _actionList;

   friend Label entry_postAction$(argVM);
   friend Label entry_raiseEvent$(argVM);
   friend Label raiseEvent_l3(argVM);
};

/*----------------------------------------------------------------------------*/

declareTypeCon(StartEvent);

declareType(StartEvent);

declareDataCon(StartEvent);

class StartEvent {
public:
   StartEvent(Int time) : _time(time) {}
   StartEvent(const StartEvent& src, const Env& srcEnv, Env& dstEnv, MSA& msa);

   inline
      void* operator new(size_t size, MSA& msa) {
      return msa.alloc(size);
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa)
#else
   inline Void operator delete(Void* ptr)
#endif
   {
   }

   inline Int time()
   {
      return _time;
   }
   void print(ostream& os, const Env& env) const;

protected:
   Int _time;
};

extern StartEvent& checkStartEvent$(argVM);
#define checkStartEvent() checkStartEvent$(vm)

#define updateStartEvent()\
if(updatePtr!=NULL){update_StartEvent(vm);updatePtr=NULL;}


/*----------------------------------------------------------------------------*/

declareTypeCon(StopEvent);

declareType(StopEvent);

declareDataCon(StopEvent);

class StopEvent
{
public:
   StopEvent(Int time)
      : _time(time)
   {
   }
   StopEvent(const StopEvent& src, const Env& srcEnv, Env& dstEnv, MSA& msa);

   inline
      void* operator new(size_t size, MSA& msa)
   {
      return msa.alloc(size);
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa)
#else
   inline Void operator delete(Void* ptr)
#endif
   {
   }

   inline Int time()
   {
      return _time;
   }
   void print(ostream& os, const Env& env) const;

protected:
   Int _time;
};

extern StopEvent& checkStopEvent$(argVM);
#define checkStopEvent() checkStopEvent$(vm)

#define updateStopEvent()\
if(updatePtr!=NULL){update_StopEvent(vm);updatePtr=NULL;}

/*----------------------------------------------------------------------------*/

declareTypeCon(CreateObjectEvent);

declareType(CreateObjectEvent);

declareDataCon(CreateObjectEvent);

class CreateObjectEvent
{
public:
   CreateObjectEvent(const ADSRef& adsr, Type type, Env* type_env);
   CreateObjectEvent(const CreateObjectEvent& src, const Env& srcEnv, Env& dstEnv, MSA& msa);

   inline
      void* operator new(size_t size, MSA& msa)
   {
      return msa.alloc(size);
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa)
#else
   inline Void operator delete(Void* ptr)
#endif
   {
   }

   void print(ostream& os, const Env& env) const;

   inline ADSRef& adsr()
   {
      return _adsr;
   }
   inline Type type_()
   {
      return _type;
   }
   inline Env* type_env()
   {
      return _type_env;
   }
   // void print(ostream& os, Env& env) const;

protected:
   ADSRef   _adsr;
   Type     _type;
   Env*     _type_env;
};

extern CreateObjectEvent& checkCreateObjectEvent$(argVM);
#define checkCreateObjectEvent() checkCreateObjectEvent$(vm)

#define updateCreateObjectEvent()\
if(updatePtr!=NULL){update_CreateObjectEvent(vm);updatePtr=NULL;}

/*----------------------------------------------------------------------------*/

declareTypeCon(DestroyObjectEvent);

declareType(DestroyObjectEvent);

declareDataCon(DestroyObjectEvent);

class DestroyObjectEvent
{
public:
   DestroyObjectEvent(const ADSRef& adsr, Type type, Env* type_env);
   DestroyObjectEvent(const DestroyObjectEvent& src, const Env& srcEnv, Env& dstEnv, MSA& msa);

   inline
      void* operator new(size_t size, MSA& msa)
   {
      return msa.alloc(size);
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa)
#else
   inline Void operator delete(Void* ptr)
#endif
   {
   }

   void print(ostream& os, const Env& env) const;

   inline ADSRef& adsr()
   {
      return _adsr;
   }
   inline Type type_()
   {
      return _type;
   }
   inline Env* type_env()
   {
      return _type_env;
   }
   // void print(ostream& os, Env& env) const;

protected:
   ADSRef   _adsr;
   Type     _type;
   Env*     _type_env;
};

extern DestroyObjectEvent& checkDestroyObjectEvent$(argVM);
#define checkDestroyObjectEvent() checkDestroyObjectEvent$(vm)

#define updateDestroyObjectEvent()\
if(updatePtr!=NULL){update_DestroyObjectEvent(vm);updatePtr=NULL;}

/*----------------------------------------------------------------------------*/

typedef UInt When;

declareTypeCon(When);

declareType(When);

extern Tag checkWhen$(argVM);
#define checkWhen() checkWhen$(vm)

extern CellInfo* cellInfoTable_When[];

#define updateWhen()\
if(updatePtr!=NULL){updatePtr->tag()=cellInfoTable_When[rTag];updatePtr=NULL;}

#define BEFORE_TAG   0
#define AFTER_TAG    1

declareNullaryDataCon(Before);
declareNullaryDataCon(After);

#define returnWhen(w){\
rTag = w;\
jump(popLabel());}

/*----------------------------------------------------------------------------*/

declareTypeCon(UpdatePropertyEvent);

declareType(UpdatePropertyEvent);

declareDataCon(UpdatePropertyEvent);

class UpdatePropertyEvent
{
public:
   UpdatePropertyEvent(Tag when, const ADSRef& ref, Name name_, Env* name_env);
   inline
      void* operator new(size_t size, MSA& msa)
   {
      return msa.alloc(size);
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa)
#else
   inline Void operator delete(Void* ptr)
#endif
   {
   }

   void print(ostream& os, const Env& env) const;

   inline Tag when()
   {
      return _when;
   }
   inline ADSRef& ref()
   {
      return _ref;
   }
   inline Type type_()
   {
      return _type;
   }
   inline Name name_()
   {
      return _name;
   }
   inline Env* name_env()
   {
      return _name_env;
   }
protected:
   Tag      _when;
   ADSRef   _ref;
   Type     _type;
   Name     _name;
   Env*     _name_env;
};

extern UpdatePropertyEvent& checkUpdatePropertyEvent$(argVM);
#define checkUpdatePropertyEvent() checkUpdatePropertyEvent$(vm)

#define updateUpdatePropertyEvent()\
if(updatePtr!=NULL){update_UpdatePropertyEvent(vm);updatePtr=NULL;}

/*----------------------------------------------------------------------------*/

declareTypeCon(AddPropertyEvent);

declareType(AddPropertyEvent);

declareDataCon(AddPropertyEvent);

class AddPropertyEvent
{
public:
   AddPropertyEvent(const ADSRef& ref, Name name, Env* name_env);
   inline
      void* operator new(size_t size, MSA& msa)
   {
      return msa.alloc(size);
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa)
#else
   inline Void operator delete(Void* ptr)
#endif
   {
   }

   void print(ostream& os, const Env& env) const;

   inline ADSRef& ref()
   {
      return _ref;
   }
   inline Type type_()
   {
      return _type;
   }
   inline Name name_()
   {
      return _name;
   }
   inline Env* name_env()
   {
      return _name_env;
   }

protected:
   ADSRef   _ref;
   Type     _type;
   Name     _name;
   Env*     _name_env;
};

extern AddPropertyEvent& checkAddPropertyEvent$(argVM);
#define checkAddPropertyEvent() checkAddPropertyEvent$(vm)

#define updateAddPropertyEvent()\
if(updatePtr!=NULL){update_AddPropertyEvent(vm);updatePtr=NULL;}

/*----------------------------------------------------------------------------*/

declareTypeCon(RemovePropertyEvent);

declareType(RemovePropertyEvent);

declareDataCon(RemovePropertyEvent);

class RemovePropertyEvent
{
public:
   RemovePropertyEvent(const ADSRef& ref, Name name, Env* name_env);
   inline
      void* operator new(size_t size, MSA& msa)
   {
      return msa.alloc(size);
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa)
#else
   inline Void operator delete(Void* ptr)
#endif
   {
   }

   void print(ostream& os, const Env& env) const;

   inline ADSRef& ref()
   {
      return _ref;
   }
   inline Type type_()
   {
      return _type;
   }
   inline Name name_()
   {
      return _name;
   }
   inline Env* name_env()
   {
      return _name_env;
   }
protected:
   ADSRef   _ref;
   Type     _type;
   Name     _name;
   Env*     _name_env;
};

extern RemovePropertyEvent& checkRemovePropertyEvent$(argVM);
#define checkRemovePropertyEvent() checkRemovePropertyEvent$(vm)

#define updateRemovePropertyEvent()\
if(updatePtr!=NULL){update_RemovePropertyEvent(vm);updatePtr=NULL;}


/*----------------------------------------------------------------------------*/

declareTypeCon(AddRefEvent);

declareType(AddRefEvent);

declareDataCon(AddRefEvent);

class AddRefEvent
{
public:
   AddRefEvent(const ADSRef& r, const ADSRef& x, Name name, Env* name_env);
   inline
      void* operator new(size_t size, MSA& msa)
   {
      return msa.alloc(size);
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa)
#else
   inline Void operator delete(Void* ptr)
#endif
   {
   }

   void print(ostream& os, const Env& env) const;

   inline ADSRef& r()
   {
      return _r;
   }
   inline Type r_type()
   {
      return _r_type;
   }
   inline ADSRef& x()
   {
      return _x;
   }
   inline Type x_type()
   {
      return _x_type;
   }
   inline Name name_()
   {
      return _name;
   }
   inline Env* name_env()
   {
      return _name_env;
   }
protected:
   ADSRef  _r;
   Type    _r_type;
   ADSRef  _x;
   Type    _x_type;
   Name    _name;
   Env*    _name_env;
};

extern AddRefEvent& checkAddRefEvent$(argVM);
#define checkAddRefEvent() checkAddRefEvent$(vm)

#define updateAddRefEvent()\
if(updatePtr!=NULL){update_AddRefEvent(vm);updatePtr=NULL;}

/*----------------------------------------------------------------------------*/

declareTypeCon(RemoveRefEvent);

declareType(RemoveRefEvent);

declareDataCon(RemoveRefEvent);

class RemoveRefEvent
{
public:
public:
   RemoveRefEvent(const ADSRef& r, const ADSRef& x, Name name, Env* name_env);
   inline
      void* operator new(size_t size, MSA& msa)
   {
      return msa.alloc(size);
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa)
#else
   inline Void operator delete(Void* ptr)
#endif
   {
   }

   void print(ostream& os, const Env& env) const;

   inline ADSRef& r()
   {
      return _r;
   }
   inline Type r_type()
   {
      return _r_type;
   }
   inline ADSRef& x()
   {
      return _x;
   }
   inline Type x_type()
   {
      return _x_type;
   }
   inline Name name_()
   {
      return _name;
   }
   inline Env* name_env()
   {
      return _name_env;
   }
protected:
   ADSRef  _r;
   Type    _r_type;
   ADSRef  _x;
   Type    _x_type;
   Name    _name;
   Env*    _name_env;
};

extern RemoveRefEvent& checkRemoveRefEvent$(argVM);
#define checkRemoveRefEvent() checkRemoveRefEvent$(vm)

#define updateRemoveRefEvent()\
if(updatePtr!=NULL){update_RemoveRefEvent(vm);updatePtr=NULL;}

/*----------------------------------------------------------------------------*/

declareTypeCon(TimerEvent);

declareType(TimerEvent);

declareDataCon(TimerEvent);

class TimerEvent
{
public:
   TimerEvent(Expr timer)
      : _timer(timer)
   {
   }
   TimerEvent(const TimerEvent& src, const Env& srcEnv, Env& dstEnv, MSA& msa);

   inline void* operator new(size_t size, void* ptr)
   {
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline void operator delete(void* ptr1, void* ptr2)
#else
   inline void operator delete(void* ptr)
#endif
   {
   }

   void print(ostream& os, const Env& env) const;
   Expr timer();

protected:
   Expr _timer;
};


declareBuiltInDataCon(AddPropertyEvent);

declareBuiltInDataCon(CreateObjectEvent);

declareBuiltInDataCon(DestroyObjectEvent);

declareBuiltInDataCon(RemovePropertyEvent);

declareBuiltInDataCon(UpdatePropertyEvent);

declareBuiltInFn(raiseEvent);

declareBuiltInFn(monitorObjectEvent);

declareBuiltInFn(postAction);

declareBuiltInFn(showWhen);

declareBuiltInFn(insertTxtWhen);

#endif /* IVORY_EVENT_H_DEFINED */
