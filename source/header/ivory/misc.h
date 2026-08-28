/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    misc.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 24 March 2000
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with miscellaneous Ivory object types.
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

#ifndef IVORY_MISC_H_DEFINED
#define IVORY_MISC_H_DEFINED

#include "ivory/class.h"
#include "ivory/dataCon.h"

declareTypeCon(ActionTrigger);

declareType(ActionTrigger);

declareDataCon(ActionTrigger);


declareTypeCon(Association);

declareType(Association);

declareInstance(PropertySetBasedObject_Association);

declareDataCon(Association);


declareTypeCon(Formula);

declareType(Formula);

declareDataCon(Formula);


declareTypeCon(Record);

declareType(Record);

declareDataCon(Record);


declareTypeCon(Rectangle);

declareType(Rectangle);

declareDataCon(Rectangle);


declareTypeCon(Template);

declareType(Template);

declareInstance(PropertySetBasedObject_Template);

declareDataCon(Template);


declareTypeCon(TemplateGroup);

declareType(TemplateGroup);

declareInstance(PropertySetBasedObject_TemplateGroup);

declareDataCon(TemplateGroup);


declareBuiltInFn(monitorActionTriggerInput);

declareBuiltInFn(monitorFormula);

#endif /* IVORY_MISC_H_DEFINED */

