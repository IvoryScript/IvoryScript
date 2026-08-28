/*-----------------------------------------------------------------------------
 *
 * (c) Copyright 2000-2026 by Alasdair Scott
 *
 * Part of the Ivory System/Script project
 *
 * https://ivorysystem.net | https://ivoryscript.net
 * https://github.com/IvorySystem | https://github.com/IvoryScript
 *
 * Name:    measurement.h
 *
 * Author:  Alasdair Scott
 *
 * Original date: 04 July 2005
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Definitions for use with Ivory measurement module
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

#ifndef IVORY_MEASUREMENT_H_DEFINED
#define IVORY_MEASUREMENT_H_DEFINED

#include "ivory/class.h"
#include "ivory/dataCon.h"


typedef Tag LTFState;

declareTypeCon(LTFState);

declareType(LTFState);

extern Tag checkLTFState$(argVM);
#define checkLTFState() checkLTFState$(vm)

extern CellInfo* cellInfoTable_LTFState[];

#define updateLTFState()\
if(updatePtr!=NULL){updatePtr->_tag=cellInfoTable_LTFState[rTag];updatePtr=NULL;}

#define LTFSTATE_LEVEL_TAG          0
#define LTFSTATE_INDETERMINATE_TAG  1

declareAtomicDataCon(LTFState_Level);
declareAtomicDataCon(LTFState_Indeterminate);

declareInstance(Show_LTFState);

#define returnLTFState(s){\
rTag = s;\
jump(popLabel());}

declareBuiltInFn(showLTFState);


declareTypeCon(DigitalInput);

declareType(DigitalInput);

declareDataCon(DigitalInput);

declareInstance(PropertySetBasedObject_DigitalInput);


declareTypeCon(DigitalOutput);

declareType(DigitalOutput);

declareDataCon(DigitalOutput);

declareInstance(PropertySetBasedObject_DigitalOutput);


declareTypeCon(Quantity);

declareType(Quantity);

declareDataCon(Quantity);

declareInstance(PropertySetBasedObject_Quantity);


declareTypeCon(LevelTimeFilter);

declareType(LevelTimeFilter);

declareDataCon(LevelTimeFilter);

declareInstance(PropertySetBasedObject_LevelTimeFilter);


declareTypeCon(Sensor);

declareType(Sensor);

declareDataCon(Sensor);

declareInstance(PropertySetBasedObject_Sensor);


declareTypeCon(TwoPointCal);

declareType(TwoPointCal);

declareDataCon(TwoPointCal);

declareInstance(PropertySetBasedObject_TwoPointCal);


declareBuiltInFn(applyTwoPointCal);

declareBuiltInFn(clockLevelTimeFilters);

declareBuiltInFn(monitorLevelTimeFilterInput);

declareBuiltInFn(monitorQuantityInput);

declareBuiltInFn(readSensor);

declareBuiltInFn(sampleSensor);

declareBuiltInFn(sampleSensors);

#endif /* IVORY_MEASUREMENT_H_DEFINED */

