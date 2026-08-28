/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    serialContext.cpp
 *
 * Module:  Ivory common (streams)
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Serialisation context and address table support.
 *
 *-----------------------------------------------------------------------------
 */

#include "ivory/int.h"
#include "ivory/streams.h"
#include "ivory/void.h"
#include <string.h>

AddressTable::AddressTable(UInt nSlots, MSA& msa)
   : _msa(msa), _nSlots(nSlots), _nextId(0) {
   _addrSlots = (Entry**)_msa.alloc(sizeof(Entry*) * _nSlots);
   _idSlots = (Entry**)_msa.alloc(sizeof(Entry*) * _nSlots);

   for (UInt i = 0; i < _nSlots; ++i) {
      _addrSlots[i] = NULL;
      _idSlots[i] = NULL;
   }
}

Bool AddressTable::insert(const Void* address, Id& id) {
   Entry* entry = lookUpAddress(address);
   if (entry != NULL) {
      id = entry->_id;
      return TRUE;
   }

   id = _nextId++;
   add(address, id, NULL);
   return FALSE;
}

Bool AddressTable::extract(Id id, Void*& address) const {
   Entry* entry = lookUpId(id);
   if (entry != NULL) {
      address = entry->_address;
      return TRUE;
   }

   address = NULL;
   return FALSE;
}

Void AddressTable::bind(Id id, Void* address) {
   Entry* entry = lookUpId(id);
   if (entry != NULL) {
      entry->_address = address;
      return;
   }

   add(address, id, address);
}

AddressTable::Entry* AddressTable::lookUpAddress(const Void* address) const {
   Entry* entry = _addrSlots[hashAddress(address) % _nSlots];
   while (entry != NULL) {
      if (entry->_keyAddress == address)
         return entry;
      entry = entry->_addrNext;
   }
   return NULL;
}

AddressTable::Entry* AddressTable::lookUpId(Id id) const {
   Entry* entry = _idSlots[hashId(id) % _nSlots];
   while (entry != NULL) {
      if (entry->_id == id)
         return entry;
      entry = entry->_idNext;
   }
   return NULL;
}

Void AddressTable::add(const Void* keyAddress, Id id, Void* address) {
   Entry* entry = new(_msa) Entry(keyAddress, id, address);

   UInt addrIndex = hashAddress(keyAddress) % _nSlots;
   entry->_addrNext = _addrSlots[addrIndex];
   _addrSlots[addrIndex] = entry;

   UInt idIndex = hashId(id) % _nSlots;
   entry->_idNext = _idSlots[idIndex];
   _idSlots[idIndex] = entry;
}

UInt AddressTable::hashAddress(const Void* address) const {
   return UInt(((size_t)address) >> 3);
}

UInt AddressTable::hashId(Id id) const {
   return id;
}

Bool insertBinSerialString(InsertArchive& archive,
                        SerialContext& serialContext,
                        const char* str) {
   NameTable& nameTable = serialContext.nameTable();
   UInt hashVal = NameTable::hash(str);
   NameTable::Entry* entry = nameTable.lookUp(str, hashVal);
   Bool isFirst = FALSE;

   if (entry == NULL) {
      entry = nameTable.addName(str, hashVal);
      isFirst = TRUE;
   }

   archive << entry->name();
   if (isFirst) {
      size_t len = strlen(str);
      archive.insertVLU(len);
      for (UInt i = 0; i < len; ++i)
         archive << str[i];
   }
   return isFirst;
}

const char* extractBinSerialString(ExtractArchive& archive,
                                SerialContext& serialContext,
                                Bool& isFirst) {
   NameTable& nameTable = serialContext.nameTable();
   Name name;
   archive >> name;
   NameTable::Entry* entry = name < (Name)nameTable.count()
      ? nameTable.lockName(name)
      : NULL;

   if (entry == NULL) {
      isFirst = TRUE;
      UInt len = archive.extractVLU();
      char* nameBuf = (char*)serialContext.msa().alloc(len + 1);
      for (UInt i = 0; i < len; ++i)
         archive >> nameBuf[i];
      nameBuf[len] = 0;

      entry = nameTable.addName(nameBuf, NameTable::hash(nameBuf));
      if (entry->name() != name)
         error("extractBinSerialString: serial string id mismatch");
      serialContext.msa().free(nameBuf);
   }

   return nameTable.string(name);
}

defineTypeCon(SerialContext);

static Void init$SerialContext(TypeDescr* typeDescr);
defineType_n(1, SerialContext, typeCon(SerialContext), init$SerialContext);

Void evalFn$SerialContext(TypeDescr* typeDescr, Ptr ptr, Env& env argN_VM) {
   rPtr = *static_cast<SerialContext**>(ptr);
}

Void init$SerialContext(TypeDescr* typeDescr) {
   typeDescr->_size = sizeof(SerialContext*);
   typeDescr->setRepr(Repr::REPR_SERIAL_CONTEXT);
   typeDescr->_evalFn = evalFn$SerialContext;
}

defineBuiltInFn(getSerialContext,
   builtInAp2(ARROW,
      typeCon(Int),
      typeCon(SerialContext)))
#define stackDepth 0
{
   checkArgs("getSerialContext");
   jump(altEntry(getSerialContext));
}

defineAltEntry(getSerialContext) {
   MSA* msa = new MSA(rInt);
   NameTable* nameTable = new(*msa) NameTable(997, 256, *msa);
   AddressTable* addressTable = new(*msa) AddressTable(DEFAULT_ADDRESS_TABLE_SLOTS, *msa);
   SerialContext* serialContext = new(*msa) SerialContext(*msa, *nameTable, *addressTable);

   rPtr = serialContext;
   rType = builtInType(SerialContext);
   jump(popLabel());
}
#undef stackDepth

defineBuiltInFn_1_arg(destroySerialContext,
   typeCon(SerialContext), typeCon(Void),
   serialContext, SerialContext)

   delete &(serialContext->msa());

   returnVoid();
endBuiltInFn
