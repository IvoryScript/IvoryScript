/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    canonicalTypeSig.cpp
 *
 * Module:  Ivory common (type)
 *
 * Author:  Alasdair Scott
 *
 * Original date: 20 September 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Function to provide a canonical type signature.
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

#include "ivory/type.h"

class CanonicalTypeVarMap {
public:
   CanonicalTypeVarMap(Void)
      : _elements(NULL) {}

   ~CanonicalTypeVarMap(Void) {
      while (_elements != NULL) {
         Element* element = _elements;
         _elements = _elements->_next;
         delete element;
      }
   }

   TypeSig lookUp(Name name) const {
      Element* element = _elements;
      while (element != NULL) {
         if (element->_name == name)
            return element->_typeSig;
         element = element->_next;
      }
      return TypeSig(VOID);
   }

   Void add(Name name, TypeSig typeSig) {
      _elements = new Element(_elements, name, typeSig);
   }

protected:
   struct Element {
      Element(Element* next, Name name, TypeSig typeSig)
         : _next(next), _name(name), _typeSig(typeSig) {}

      Element* _next;
      Name     _name;
      TypeSig  _typeSig;
   };

   Element* _elements;
};

class CanonicalTypeVarSet {
public:
   CanonicalTypeVarSet(Void)
      : _elements(NULL) {}

   ~CanonicalTypeVarSet(Void) {
      while (_elements != NULL) {
         Element* element = _elements;
         _elements = _elements->_next;
         delete element;
      }
   }

   Bool hasElement(Name name) const {
      Element* element = _elements;
      while (element != NULL) {
         if (element->_name == name)
            return TRUE;
         element = element->_next;
      }
      return FALSE;
   }

   Void addElement(Name name) {
      if (!hasElement(name))
         _elements = new Element(_elements, name);
   }

protected:
   struct Element {
      Element(Element* next, Name name)
         : _next(next), _name(name) {}

      Element* _next;
      Name     _name;
   };

   Element* _elements;
};

static Void canonicalTypeVarName(UInt index, char* s) {
   *s++ = char('a' + index % 26);
   if (index >= 26) {
      UInt suffix = index / 26;
      char buf[16];
      UInt n = 0;
      do {
         buf[n++] = char('0' + suffix % 10);
         suffix /= 10;
      } while (suffix > 0);
      while (n > 0)
         *s++ = buf[--n];
   }
   *s = '\0';
}

static Bool hasTypeVars(TypeSig typeSig, CanonicalTypeVarSet& typeVars) {
   if (typeSig == TypeSig(UNKNOWN))
      return TRUE;
   else if (isAp(typeSig)) {
      Bool funNeedsCanonical = hasTypeVars(fun(typeSig), typeVars);
      Bool argNeedsCanonical = hasTypeVars(arg(typeSig), typeVars);
      return funNeedsCanonical || argNeedsCanonical;
   } else if (isName(typeSig)) {
      Name name = toName(typeSig);
      if (name >= 0)
         typeVars.addElement(name);
      return name < 0;
   }
   return FALSE;
}

static TypeSig nextCanonicalTypeVar(UInt& nTypeVars,
                                    CanonicalTypeVarSet& usedTypeVars,
                                    NameTable& nameTable) {
   char name[16];
   Name typeVar;
   do {
      canonicalTypeVarName(nTypeVars++, name);
      typeVar = useName(name, nameTable);
   } while (usedTypeVars.hasElement(typeVar));
   usedTypeVars.addElement(typeVar);
   return fromName(typeVar);
}

static TypeSig canonicalTypeSig(TypeSig typeSig, CanonicalTypeVarMap& typeVarMap,
                                CanonicalTypeVarSet& usedTypeVars,
                                UInt& nTypeVars,
                                NameTable& nameTable, MSA& msa) {
   if (typeSig == TypeSig(UNKNOWN))
      return nextCanonicalTypeVar(nTypeVars, usedTypeVars, nameTable);
   else if (!isAp(typeSig)) {
      if (isName(typeSig)) {
         Name typeVar = toName(typeSig);
         if (typeVar >= 0)
            return typeSig;

         TypeSig canonical = typeVarMap.lookUp(typeVar);
         if (canonical != TypeSig(VOID))
            return canonical;

         canonical = nextCanonicalTypeVar(nTypeVars, usedTypeVars, nameTable);
         typeVarMap.add(typeVar, canonical);
         return canonical;
      }
      return typeSig;
   } else {
      TypeSig funTypeSig = canonicalTypeSig(fun(typeSig), typeVarMap,
                                            usedTypeVars, nTypeVars,
                                            nameTable, msa);
      TypeSig argTypeSig = canonicalTypeSig(arg(typeSig), typeVarMap,
                                            usedTypeVars, nTypeVars,
                                            nameTable, msa);
      return TypeSig(ap(funTypeSig, argTypeSig, msa));
   }
}

TypeSig canonicalTypeSig(TypeSig typeSig, NameTable& nameTable, MSA& msa) {
   CanonicalTypeVarSet usedTypeVars;
   if (!hasTypeVars(typeSig, usedTypeVars))
      return typeSig;

   CanonicalTypeVarMap typeVarMap;
   UInt nTypeVars = 0;
   return canonicalTypeSig(typeSig, typeVarMap, usedTypeVars, nTypeVars,
                           nameTable, msa);
}
