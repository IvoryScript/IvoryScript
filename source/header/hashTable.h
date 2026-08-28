/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Name:    hashTable.h 
 *
 * Author:  Alasdair Scott
 *
 * Original date: 08 September 2000
 *
 *-----------------------------------------------------------------------------
 *
 * Description:
 *
 *    Abstract classes to represent hash tables.
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

#ifndef HASH_TABLE_H_DEFINED
#define HASH_TABLE_H_DEFINED

#include "general.h"
#include "MSA.h"

// Basic chained hash table: externally provided nodes

// Node should have public member Node_T& next(Void);

template<class Link_T, class Node_T, class Key_T>
class BasicChainedHashTable {

protected:
   BasicChainedHashTable(UInt nSlots, Link_T nullLink)
      : _nSlots(nSlots), _nullLink(nullLink), _slots(NULL) {
   }

public:

// Default is an assertion error on duplicate value

   Void add(Link_T link, UInt hashVal, MSA& msa) {
      if (_slots == NULL)
         allocSlots(msa);
      else
         assert(lookUp(keyOf(linkToNode(link)), hashVal) == NULL, "BasicHashTable::add: key not unique");

      UInt index = hashVal % _nSlots;
      linkToNode(link).next() = _slots[index];
      _slots[index] = link;
   }

   Node_T* lookUp(Key_T key, UInt hashVal) const {
      if (_slots != NULL) {
         UInt index = hashVal % _nSlots;
         Link_T link = _slots[hashVal % _nSlots];
         while (link != _nullLink) {
            Node_T& node = linkToNode(link);
            if (key == keyOf(node))
               return &node;
            link = node.next();
         }
      }
      return NULL;
   }

   inline UInt nSlots(Void) const { return _nSlots; }

protected:
   Void allocSlots(MSA& msa) {
      _slots = (Link_T*)msa.alloc(_nSlots * sizeof(Link_T));
      for (UInt i = 0; i < _nSlots; i++)
         _slots[i] = _nullLink;
   }
   virtual Key_T keyOf(const Node_T& node) const = 0;
   virtual Node_T& linkToNode(Link_T link) const = 0;

   UInt     _nSlots;
   Link_T   _nullLink;
   Link_T*  _slots;
};

// Similar but with direct slot array and permitting duplicate entries

template<class Node_T, class Key_T, UInt N_SLOTS>
class BasicChainedHashTable_ {

protected:
   BasicChainedHashTable_(Void) {
      memset(_slots, 0, sizeof(Node_T*) * N_SLOTS);
   }

public:
   Void add(Node_T& node) {  
      UInt index = hashVal(keyOf(node)) % N_SLOTS;
      node.next() = _slots[index];
      _slots[index] = &node;
   }

   Node_T* lookUp(Key_T key) const {
      Node_T* link = _slots[hashVal(key) % N_SLOTS];
      while (link != NULL) {
         if (key == keyOf(*link))
            return link;
         link = link->next();
      }
      return NULL;
   }

protected:
   virtual UInt hashVal(Key_T key) const = 0;
   virtual Key_T keyOf(const Node_T& node) const = 0;
   inline UInt nSlots(Void) const { return N_SLOTS; }
   inline Node_T* slot(UInt index) const { return _slots[index]; }

protected:
   Node_T*   _slots[N_SLOTS];
};

// Chained hash table: internal nodes

template<class Link_T, class Entry_T, class Key_T> class ChainedHashTable;
template<class Link_T, class Entry_T, class Key_T>
class ChainedHashTableNode {

// public due to the difficult of declaring ChainedHashTable_ as a friend

public:
   ChainedHashTableNode(Link_T next, const Entry_T& entry)
      : _next(next), _entry(entry) {
   }
   inline
      Void* operator new(size_t size, Void* ptr) {
      return ptr;
   }
#ifdef DELETE_NEEDS_NEW_ARG
   inline Void operator delete(Void* ptr1, Void* ptr2) {
#else
   inline Void operator delete(Void* ptr) {
#endif
   }

   Link_T& next(Void) { return _next; }
   const Entry_T& entry_(Void) const { return _entry; }

protected:

   Link_T   _next;
   Entry_T  _entry;

   friend class ChainedHashTable<Link_T, Entry_T, Key_T>;
};

template<class Link_T, class Entry_T, class Key_T>
class ChainedHashTable : public BasicChainedHashTable<Link_T, ChainedHashTableNode<Link_T, Entry_T, Key_T>, Key_T> {

protected:
   using BasicChainedHashTable<Link_T, ChainedHashTableNode<Link_T, Entry_T, Key_T>, Key_T>::_nSlots;
   using BasicChainedHashTable<Link_T, ChainedHashTableNode<Link_T, Entry_T, Key_T>, Key_T>::_nullLink;
   using BasicChainedHashTable<Link_T, ChainedHashTableNode<Link_T, Entry_T, Key_T>, Key_T>::_slots;

   ChainedHashTable(UInt nSlots, Link_T nullLink)
      : BasicChainedHashTable<Link_T, ChainedHashTableNode<Link_T, Entry_T, Key_T>, Key_T>(nSlots, nullLink) {
   }
public:
   Void add(Entry_T& entry, UInt hashVal, MSA& msa) {
      if (_slots == NULL)
         BasicChainedHashTable<Link_T, ChainedHashTableNode<Link_T, Entry_T, Key_T>, Key_T>::allocSlots(msa);
      else
         if (lookUp(keyOf(entry), hashVal) != NULL) {
            error("HashTable::add: key not unique");
            return;
         }

      UInt index = hashVal % _nSlots;
      _slots[index] = allocNode(entry, _slots[index], msa);
   }

   Entry_T* lookUp(Key_T key, UInt hashVal) const {
      if (_slots != NULL) {
         Link_T link = _slots[hashVal % _nSlots];
         while (link != _nullLink) {
            ChainedHashTableNode<Link_T, Entry_T, Key_T>& node = linkToNode(link);
            if (key == keyOf(node._entry))
               return &node._entry;
            link = node._next;
         }
      }
      return NULL;
   }
   inline UInt nSlots(Void) const { return _nSlots; }
   inline ChainedHashTableNode<Link_T, Entry_T, Key_T>* slot(UInt index) const {
      return (ChainedHashTableNode<Link_T, Entry_T, Key_T>*)_slots[index];
   }

protected:
   virtual Void* allocNodePtr(size_t size, MSA& msa, Link_T& link) = 0;
   virtual ChainedHashTableNode<Link_T, Entry_T, Key_T>& linkToNode(Link_T link) const = 0;

   virtual Key_T keyOf(const Entry_T& entry) const = 0;
   virtual Key_T keyOf(const ChainedHashTableNode<Link_T, Entry_T, Key_T>& node) const {
      return keyOf(node._entry);
   }
   virtual Link_T allocNode(Entry_T& entry, Link_T next, MSA& msa) {
      Link_T link;
      new(allocNodePtr(sizeof(ChainedHashTableNode<Link_T, Entry_T, Key_T>), msa, link)) ChainedHashTableNode<Link_T, Entry_T, Key_T>(next, entry);
      return link;
   }

   inline
      Link_T& nodeNext(ChainedHashTableNode<Link_T, Entry_T, Key_T>& node) const { return node._next; }
   inline
      const Entry_T& nodeEntry(const ChainedHashTableNode<Link_T, Entry_T, Key_T>& node) const { return node._entry; }
};

// Chained hash table with direct slots: internal nodes

template<class Entry_T, class Key_T, UInt N_SLOTS>
class ChainedHashTable_ : public BasicChainedHashTable_<ChainedHashTableNode<Void*, Entry_T, Key_T>, Key_T, N_SLOTS> {

protected:
   using BasicChainedHashTable_<ChainedHashTableNode<Void*, Entry_T, Key_T>, Key_T, N_SLOTS>::_slots;

   ChainedHashTable_(Void)
      : BasicChainedHashTable_<ChainedHashTableNode<Void*, Entry_T, Key_T>, Key_T, N_SLOTS>() {
   }

public:

   Void add(Entry_T& entry, UInt hashVal, MSA& msa) {
      UInt index = hashVal % N_SLOTS;
      _slots[index] = allocNode(entry, _slots[index], msa);
   }

   const Entry_T* lookUp(Key_T key) const {
      ChainedHashTableNode<Void*, Entry_T, Key_T>* node = _slots[hashVal(key) % N_SLOTS];
      while (node != NULL) {
         if (key == keyOf(node->entry_()))
            return &node->entry_();
         node = (ChainedHashTableNode<Void*, Entry_T, Key_T>*)node->next();
      }
      return NULL;
   }

   inline ChainedHashTableNode<Void*, Entry_T, Key_T>*& slot(UInt index) { return _slots[index]; }

protected:
   virtual Void* allocNode(size_t size, MSA& msa, Void*& link) = 0;
   virtual ChainedHashTableNode<Void*, Entry_T, Key_T>* allocNode(Entry_T& entry, Void* next, MSA& msa) {
      Void* link;
      new(allocNode(sizeof(ChainedHashTableNode<Void*, Entry_T, Key_T>), msa, link)) ChainedHashTableNode<Void*, Entry_T, Key_T>(next, entry);
      return (ChainedHashTableNode<Void*, Entry_T, Key_T>*)link;
   }
   virtual UInt hashVal(Key_T key) const = 0;
   virtual Key_T keyOf(const Entry_T& entry) const = 0;
   virtual Key_T keyOf(const ChainedHashTableNode<Void*, Entry_T, Key_T>& node) const {
      return keyOf(node.entry_());
   }
};

// Hash table 

#define HashTableNode ChainedHashTableNode

#define HashTable ChainedHashTable

#endif /* HASH_TABLE_H_DEFINED */
