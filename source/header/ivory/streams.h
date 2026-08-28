/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    streams.h
 *
 * Module:  
 *
 * Author:  Alasdair Scott
 *
 * Original date: 24 May 2001
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with streams.
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
 */

#ifndef IVORY_STREAMS_H_DEFINED
#define IVORY_STREAMS_H_DEFINED

#include "archive.h"
#include "common.h"
#include "machine.h"
#include "type.h"

class VirtualMachine;

#define DEFAULT_ADDRESS_TABLE_SLOTS 997

class AddressTable {
public:
   typedef UInt Id;

   AddressTable(UInt nSlots, MSA& msa);

public:
   inline Void* operator new(size_t size, MSA& msa) {
      return msa.alloc(size);
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) {}
#else
   inline Void operator delete(Void* ptr) {}
#endif

   Bool insert(const Void* address, Id& id);
   Bool extract(Id id, Void*& address) const;
   Void bind(Id id, Void* address);

protected:
   struct Entry {
      Entry(const Void* keyAddress, Id id, Void* address)
         : _addrNext(NULL), _idNext(NULL),
           _keyAddress(keyAddress), _id(id), _address(address) {}

      inline Void* operator new(size_t size, MSA& msa) {
         return msa.alloc(size);
      }
#ifdef DELETE_NEEDS_NEW_ARG
      inline Void operator delete(Void* ptr, MSA& msa) { msa.free(ptr); }
#else
      inline Void operator delete(Void* ptr) {}
#endif

      Entry*      _addrNext;
      Entry*      _idNext;
      const Void* _keyAddress;
      Id          _id;
      Void*       _address;
   };

   Entry* lookUpAddress(const Void* address) const;
   Entry* lookUpId(Id id) const;
   Void add(const Void* keyAddress, Id id, Void* address);

   UInt hashAddress(const Void* address) const;
   UInt hashId(Id id) const;

protected:
   MSA&    _msa;
   UInt    _nSlots;
   Id      _nextId;
   Entry** _addrSlots;
   Entry** _idSlots;
};

class SerialContext {
public:
   SerialContext(MSA& msa,
                 NameTable& nameTable,
                 AddressTable& addressTable)
      : _msa(msa), _nameTable(nameTable), _addressTable(addressTable) {
   }

   inline Void* operator new(size_t size, MSA& msa) {
      return msa.alloc(size);
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr, MSA& msa) {}
#else
   inline Void operator delete(Void* ptr) {}
#endif

   inline MSA& msa(Void) const { return _msa; }
   inline NameTable& nameTable(Void) const { return _nameTable; }
   inline AddressTable& addressTable(Void) const { return _addressTable; }

protected:
   MSA&           _msa;
   NameTable&     _nameTable;
   AddressTable&  _addressTable;
};

class InsertArchive : public Archive {
public:
	InsertArchive(Cell* outFn, Env* outFnEnv);

	virtual Byte get(void);
	virtual void put(Byte b);
	virtual void get(Byte& b);

protected:

#if (GLOBAL_VM == 0)
	VirtualMachine _vm;
#endif

	Cell* _outFn;
	Env*  _outFnEnv;
};

class ExtractArchive : public Archive {
public:
	ExtractArchive(Expr inAct, Env* inActEnv);

	virtual Byte get(void);
	virtual void put(Byte b);
	virtual void get(Byte& b);

protected:

#if (GLOBAL_VM == 0)
	VirtualMachine _vm;
#endif

	Expr _inAct;
	Env* _inActEnv;
};

declareBuiltInTypeCon(InputStream);

declareBuiltInTypeCon(OutputStream);

declareBuiltInTypeCon(SerialContext);

declareBuiltInType(SerialContext);

declareBuiltInFn(getSerialContext);

declareBuiltInFn(destroySerialContext);

declareBuiltInFn(extractBinSerialString);

declareBuiltInFn(extractBinTag);

declareBuiltInFn(insertBinSerialString);

declareBuiltInFn(insertBinTag);

declareBuiltInFn(insertTxtTag);

extern Bool insertBinSerialString(InsertArchive& archive,
                               SerialContext& serialContext,
                               const char* str);

extern const char* extractBinSerialString(ExtractArchive& archive,
                                       SerialContext& serialContext,
                                       Bool& isFirst);

struct InputStream_Byte {
	Expr	_inAct;
	Expr	_closeAction;
   SerialContext* _serialContext;
};

struct OutputStream_Byte {
	Cell* _outputFn;
	Expr	_closeAction;
   SerialContext* _serialContext;
};

struct OutputStream_Char {
	Cell* _outputFn;
	Expr	_closeAction;
   SerialContext* _serialContext;
};

extern Void putChar(OutputStream_Char& os, const Env& osEnv,
                    Char c argN_VM);

extern Void putString(OutputStream_Char& os, const Env& osEnv,
                      const char* str argN_VM);

#define typeSpecInputStream_Byte InputStream_Byte*

#define argRegInputStream_Byte rPtr

#define typeSpecOutputStream_Byte OutputStream_Byte*

#define argRegOutputStream_Byte rPtr

#define typeSpecOutputStream_Char OutputStream_Char*

#define argRegOutputStream_Char rPtr

#define loadArgRegInputStream_Byte(name)\
InputStream_Byte* name=static_cast<InputStream_Byte*>(rPtr);\
Env* name##Env=rEnv

#define loadArgRegOutputStream_Byte(name)\
OutputStream_Byte* name=static_cast<OutputStream_Byte*>(rPtr);\
Env* name##Env=rEnv

#define loadArgRegOutputStream_Char(name)\
OutputStream_Char* name=static_cast<OutputStream_Char*>(rPtr);\
Env* name##Env=rEnv

#define argRegFromCellInputStream_Byte(s,c)\
rPtr=cellBody(*cell,s).c;\
rEnv=cellEnv

#define argRegFromCellOutputStream_Byte(s,c)\
rPtr=cellBody(*cell,s).c;\
rEnv=cellEnv

#define argRegFromCellOutputStream_Char(s,c)\
rPtr=cellBody(*cell,s).c;\
rEnv=cellEnv

//T.B.D
#define map_InputStream_Byte(name,srcEnv,dstEnv) name

//T.B.D.
#define map_OutputStream_Byte(name,srcEnv,dstEnv) name // T.B.D

//T.B.D.
#define map_OutputStream_Char(name,srcEnv,dstEnv) name // T.B.D

#define typeSpecSerialContext SerialContext*

#define argRegSerialContext rPtr

#define loadArgRegSerialContext(name)\
SerialContext* name=static_cast<SerialContext*>(rPtr)

#define map_SerialContext(name,srcEnv,dstEnv) name

#endif // IVORY_STREAMS_H_DEFINED
