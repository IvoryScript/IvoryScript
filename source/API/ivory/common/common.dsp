# Microsoft Developer Studio Project File - Name="common" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=common - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "common.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "common.mak" CFG="common - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "common - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "common - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "common - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\..\build\win32\API\ivory\common\Release"
# PROP Intermediate_Dir "..\..\..\..\build\win32\API\ivory\common\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Ox /Oa /Og /Oi /Os /Ob2 /I "..\..\..\..\Source\Header" /D "NDEBUG" /D RUN_TIME_TYPE_CHECKS=0 /D GLOBAL_VM=1 /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "common - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\..\build\win32\API\ivory\common\Debug"
# PROP Intermediate_Dir "..\..\..\..\build\win32\API\ivory\common\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\Source\Header" /D "_DEBUG" /D "ENABLE_ASSERT" /D "CHECK_ARGS" /D RUN_TIME_TYPE_CHECKS=1 /D GLOBAL_VM=0 /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "common - Win32 Release"
# Name "common - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\double\acos.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\actionTrigger.cpp
# End Source File
# Begin Source File

SOURCE=.\event\addPropertyEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\propertySet\addPropertyPropertySetBasedObject.cpp
# End Source File
# Begin Source File

SOURCE=.\propertySet\addRefAtPropertySetBasedObject.cpp
# End Source File
# Begin Source File

SOURCE=.\event\addRefEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\propertySet\addRefPropertySetBasedObject.cpp
# End Source File
# Begin Source File

SOURCE=.\ADS\addVarADS.cpp
# End Source File
# Begin Source File

SOURCE=.\context\addVarContext.cpp
# End Source File
# Begin Source File

SOURCE=.\root\addVarRoot.cpp
# End Source File
# Begin Source File

SOURCE=.\ADS\ADS.cpp
# End Source File
# Begin Source File

SOURCE=.\repr\any.cpp
# End Source File
# Begin Source File

SOURCE=.\repr\ap.cpp
# End Source File
# Begin Source File

SOURCE=.\type\argType.cpp
# End Source File
# Begin Source File

SOURCE=.\repr\arity.cpp
# End Source File
# Begin Source File

SOURCE=.\type\array.cpp
# End Source File
# Begin Source File

SOURCE=.\double\asin.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\association.cpp
# End Source File
# Begin Source File

SOURCE=.\double\atan.cpp
# End Source File
# Begin Source File

SOURCE=.\double\atan2.cpp
# End Source File
# Begin Source File

SOURCE=.\binding\binding.cpp
# End Source File
# Begin Source File

SOURCE=.\class\bitsClass.cpp
# End Source File
# Begin Source File

SOURCE=.\int\bitsInt.cpp
# End Source File
# Begin Source File

SOURCE=.\bool\bool.cpp
# End Source File
# Begin Source File

SOURCE=.\propertySet\boolProperty.cpp
# End Source File
# Begin Source File

SOURCE=.\repr\builtInFn.cpp
# End Source File
# Begin Source File

SOURCE=.\byteString\bytesToString.cpp
# End Source File
# Begin Source File

SOURCE=.\byteString\byteString.cpp
# End Source File
# Begin Source File

SOURCE=.\repr\cellInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\char\char.cpp
# End Source File
# Begin Source File

SOURCE=.\repr\class.cpp
# End Source File
# Begin Source File

SOURCE=.\machine\closure.cpp
# End Source File
# Begin Source File

SOURCE=.\machine\cond.cpp
# End Source File
# Begin Source File

SOURCE=.\list\condCons.cpp
# End Source File
# Begin Source File

SOURCE=.\context\context.cpp
# End Source File
# Begin Source File

SOURCE=.\repr\copyName.cpp
# End Source File
# Begin Source File

SOURCE=.\double\cos.cpp
# End Source File
# Begin Source File

SOURCE=.\double\cosh.cpp
# End Source File
# Begin Source File

SOURCE=.\ADS\createObjectADS.cpp
# End Source File
# Begin Source File

SOURCE=.\event\createObjectEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\root\createObjectRoot.cpp
# End Source File
# Begin Source File

SOURCE=.\criteria\criteria.cpp
# End Source File
# Begin Source File

SOURCE=.\repr\dataCon.cpp
# End Source File
# Begin Source File

SOURCE=.\object\destroyObject.cpp
# End Source File
# Begin Source File

SOURCE=.\event\destroyObjectEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\type\destroyType.cpp
# End Source File
# Begin Source File

SOURCE=.\double\double.cpp
# End Source File
# Begin Source File

SOURCE=.\machine\enter.cpp
# End Source File
# Begin Source File

SOURCE=.\machine\enterReturn.cpp
# End Source File
# Begin Source File

SOURCE=.\propertySet\enumProperty.cpp
# End Source File
# Begin Source File

SOURCE=.\bool\eqBool.cpp
# End Source File
# Begin Source File

SOURCE=.\char\eqChar.cpp
# End Source File
# Begin Source File

SOURCE=.\class\eqClass.cpp
# End Source File
# Begin Source File

SOURCE=.\double\eqDouble.cpp
# End Source File
# Begin Source File

SOURCE=.\float\eqFloat.cpp
# End Source File
# Begin Source File

SOURCE=.\int\eqInt.cpp
# End Source File
# Begin Source File

SOURCE=.\name\eqName.cpp
# End Source File
# Begin Source File

SOURCE=.\ref\eqRef.cpp
# End Source File
# Begin Source File

SOURCE=.\string\eqString.cpp
# End Source File
# Begin Source File

SOURCE=.\type\eqType.cpp
# End Source File
# Begin Source File

SOURCE=.\UTC\eqUTC.cpp
# End Source File
# Begin Source File

SOURCE=.\machine\error.cpp
# End Source File
# Begin Source File

SOURCE=.\machine\eval.cpp
# End Source File
# Begin Source File

SOURCE=.\event\eventPhase.cpp
# End Source File
# Begin Source File

SOURCE=.\machine\exit.cpp
# End Source File
# Begin Source File

SOURCE=.\double\exp.cpp
# End Source File
# Begin Source File

SOURCE=.\repr\expr.cpp

!IF  "$(CFG)" == "common - Win32 Release"

# SUBTRACT CPP /Z<none>

!ELSEIF  "$(CFG)" == "common - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\int\fact.cpp

!IF  "$(CFG)" == "common - Win32 Release"

# SUBTRACT CPP /Z<none>

!ELSEIF  "$(CFG)" == "common - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\machine\fatBar.cpp
# End Source File
# Begin Source File

SOURCE=.\refList\firstRef.cpp
# End Source File
# Begin Source File

SOURCE=.\float\float.cpp
# End Source File
# Begin Source File

SOURCE=.\refList\foldRRefs.cpp
# End Source File
# Begin Source File

SOURCE=.\char\formatChar.cpp
# End Source File
# Begin Source File

SOURCE=.\int\formatInt.cpp
# End Source File
# Begin Source File

SOURCE=.\string\formatString.cpp
# End Source File
# Begin Source File

SOURCE=.\UTC\formatUTC.cpp
# End Source File
# Begin Source File

SOURCE=.\repr\formOf.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\formula.cpp
# End Source File
# Begin Source File

SOURCE=.\class\fractionalClass.cpp
# End Source File
# Begin Source File

SOURCE=.\double\fractionalDouble.cpp
# End Source File
# Begin Source File

SOURCE=.\float\fractionalFloat.cpp
# End Source File
# Begin Source File

SOURCE=.\propertySet\getProperty.cpp
# End Source File
# Begin Source File

SOURCE=.\machine\handleScriptInput.cpp
# End Source File
# Begin Source File

SOURCE=.\list\hasElement.cpp
# End Source File
# Begin Source File

SOURCE=.\type\hashType.cpp
# End Source File
# Begin Source File

SOURCE=.\propertySet\hasProperty.cpp
# End Source File
# Begin Source File

SOURCE=.\propertySet\hasPropertyPropertySet.cpp
# End Source File
# Begin Source File

SOURCE=.\propertySet\hasPropertyPropertySetBasedObject.cpp
# End Source File
# Begin Source File

SOURCE=.\refList\hasRef.cpp
# End Source File
# Begin Source File

SOURCE=.\ADS\hasVarADS.cpp
# End Source File
# Begin Source File

SOURCE=.\context\hasVarContext.cpp
# End Source File
# Begin Source File

SOURCE=.\root\hasVarRoot.cpp
# End Source File
# Begin Source File

SOURCE=.\list\head.cpp
# End Source File
# Begin Source File

SOURCE=.\repr\headForm.cpp
# End Source File
# Begin Source File

SOURCE=.\ADS\host.cpp
# End Source File
# Begin Source File

SOURCE=.\int\int.cpp
# End Source File
# Begin Source File

SOURCE=.\class\integralClass.cpp
# End Source File
# Begin Source File

SOURCE=.\int\integralInt.cpp
# End Source File
# Begin Source File

SOURCE=.\machine\interp.cpp

!IF  "$(CFG)" == "common - Win32 Release"

# SUBTRACT CPP /Z<none>

!ELSEIF  "$(CFG)" == "common - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\propertySet\intProperty.cpp
# End Source File
# Begin Source File

SOURCE=.\type\isSumType.cpp
# End Source File
# Begin Source File

SOURCE=.\parser\ivory.tab.cpp
# End Source File
# Begin Source File

SOURCE=.\repr\label.cpp
# End Source File
# Begin Source File

SOURCE=.\list\lengthList.cpp

!IF  "$(CFG)" == "common - Win32 Release"

# ADD CPP /Os
# SUBTRACT CPP /Z<none> /Ot

!ELSEIF  "$(CFG)" == "common - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\linkage\linkage.cpp
# End Source File
# Begin Source File

SOURCE=.\list\list.cpp
# End Source File
# Begin Source File

SOURCE=.\type\loadType.cpp
# End Source File
# Begin Source File

SOURCE=.\double\log.cpp
# End Source File
# Begin Source File

SOURCE=.\double\log10.cpp
# End Source File
# Begin Source File

SOURCE=.\machine\machine.cpp
# End Source File
# Begin Source File

SOURCE=.\list\map.cpp
# End Source File
# Begin Source File

SOURCE=.\machine\mapName.cpp
# End Source File
# Begin Source File

SOURCE=.\refList\mapPredRefs.cpp
# End Source File
# Begin Source File

SOURCE=.\ADS\mapProcObjectsADS.cpp
# End Source File
# Begin Source File

SOURCE=.\root\mapProcObjectsRoot.cpp
# End Source File
# Begin Source File

SOURCE=.\refList\mapProcRefs.cpp
# End Source File
# Begin Source File

SOURCE=.\binding\matchName.cpp
# End Source File
# Begin Source File

SOURCE=.\ref\matchRef.cpp
# End Source File
# Begin Source File

SOURCE=.\module\module.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\monitorActionTriggerInput.cpp
# End Source File
# Begin Source File

SOURCE=.\criteria\monitorCriteriaInput.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\monitorFormula.cpp
# End Source File
# Begin Source File

SOURCE=.\event\monitorObjectEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\sync\monitorShutdown.cpp
# End Source File
# Begin Source File

SOURCE=.\repr\name.cpp
# End Source File
# Begin Source File

SOURCE=.\repr\nameSpace.cpp
# End Source File
# Begin Source File

SOURCE=.\list\noDups.cpp
# End Source File
# Begin Source File

SOURCE=.\class\numClass.cpp
# End Source File
# Begin Source File

SOURCE=.\double\numDouble.cpp
# End Source File
# Begin Source File

SOURCE=.\float\numFloat.cpp
# End Source File
# Begin Source File

SOURCE=.\repr\object.cpp
# End Source File
# Begin Source File

SOURCE=.\class\objectClass.cpp
# End Source File
# Begin Source File

SOURCE=.\char\ordChar.cpp
# End Source File
# Begin Source File

SOURCE=.\class\ordClass.cpp
# End Source File
# Begin Source File

SOURCE=.\double\ordDouble.cpp
# End Source File
# Begin Source File

SOURCE=.\ord\ordering.cpp
# End Source File
# Begin Source File

SOURCE=.\float\ordFloat.cpp
# End Source File
# Begin Source File

SOURCE=.\int\ordInt.cpp
# End Source File
# Begin Source File

SOURCE=.\string\ordString.cpp
# End Source File
# Begin Source File

SOURCE=.\UTC\ordUTC.cpp
# End Source File
# Begin Source File

SOURCE=.\machine\postConstructorInit.cpp
# End Source File
# Begin Source File

SOURCE=.\machine\postEval.cpp
# End Source File
# Begin Source File

SOURCE=.\double\pow.cpp
# End Source File
# Begin Source File

SOURCE=.\machine\preEval.cpp
# End Source File
# Begin Source File

SOURCE=.\machine\printExpr.cpp
# End Source File
# Begin Source File

SOURCE=.\machine\printName.cpp
# End Source File
# Begin Source File

SOURCE=.\machine\printType.cpp
# End Source File
# Begin Source File

SOURCE=.\propertySet\property.cpp
# End Source File
# Begin Source File

SOURCE=.\propertySet\PropertySet.cpp
# End Source File
# Begin Source File

SOURCE=.\class\propertySetBasedObjectClass.cpp
# End Source File
# Begin Source File

SOURCE=.\event\raiseEvent.cpp

!IF  "$(CFG)" == "common - Win32 Release"

# SUBTRACT CPP /Z<none>

!ELSEIF  "$(CFG)" == "common - Win32 Debug"

# ADD CPP /ZI

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\machine\receiveScript.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\record.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\rectangle.cpp
# End Source File
# Begin Source File

SOURCE=.\ref\ref.cpp
# End Source File
# Begin Source File

SOURCE=.\refList\referrer.cpp
# End Source File
# Begin Source File

SOURCE=.\refList\refList.cpp
# End Source File
# Begin Source File

SOURCE=.\propertySet\refProperty.cpp
# End Source File
# Begin Source File

SOURCE=.\event\removePropertyEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\propertySet\removePropertyPropertySetBasedObject.cpp
# End Source File
# Begin Source File

SOURCE=.\event\removeRefEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\propertySet\removeRefPropertySetBasedObject.cpp
# End Source File
# Begin Source File

SOURCE=.\ADS\removeVarADS.cpp
# End Source File
# Begin Source File

SOURCE=.\context\removeVarContext.cpp
# End Source File
# Begin Source File

SOURCE=.\root\removeRegisterEntry_Root.cpp
# End Source File
# Begin Source File

SOURCE=.\repr\repr.cpp
# End Source File
# Begin Source File

SOURCE=.\type\resultType.cpp
# End Source File
# Begin Source File

SOURCE=.\refList\revRefs.cpp
# End Source File
# Begin Source File

SOURCE=.\refList\revRefsOfType.cpp
# End Source File
# Begin Source File

SOURCE=.\root\root.cpp
# End Source File
# Begin Source File

SOURCE=.\rule\rule.cpp
# End Source File
# Begin Source File

SOURCE=.\propertySet\searchPropertySet.cpp
# End Source File
# Begin Source File

SOURCE=.\ADS\selectADS.cpp
# End Source File
# Begin Source File

SOURCE=.\class\selectClass.cpp
# End Source File
# Begin Source File

SOURCE=.\context\selectRegister.cpp
# End Source File
# Begin Source File

SOURCE=.\propertySet\selectPropertySet.cpp

!IF  "$(CFG)" == "common - Win32 Release"

# SUBTRACT CPP /Z<none>

!ELSEIF  "$(CFG)" == "common - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\propertySet\selectPropertySetBasedObject.cpp

!IF  "$(CFG)" == "common - Win32 Release"

# SUBTRACT CPP /Z<none>

!ELSEIF  "$(CFG)" == "common - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\root\selectRoot.cpp
# End Source File
# Begin Source File

SOURCE=.\sync\semaphore.cpp
# End Source File
# Begin Source File

SOURCE=.\machine\seq.cpp
# End Source File
# Begin Source File

SOURCE=.\byteString\seqByteString.cpp
# End Source File
# Begin Source File

SOURCE=.\class\seqClass.cpp
# End Source File
# Begin Source File

SOURCE=.\propertySet\seqPropertySet.cpp
# End Source File
# Begin Source File

SOURCE=.\refList\seqRefList.cpp
# End Source File
# Begin Source File

SOURCE=.\string\seqString.cpp
# End Source File
# Begin Source File

SOURCE=.\type\setArgFlags.cpp
# End Source File
# Begin Source File

SOURCE=.\propertySet\setBoolProperty.cpp
# End Source File
# Begin Source File

SOURCE=.\propertySet\setIntProperty.cpp
# End Source File
# Begin Source File

SOURCE=.\propertySet\setPropertySetBasedObject.cpp
# End Source File
# Begin Source File

SOURCE=.\propertySet\setStringProperty.cpp
# End Source File
# Begin Source File

SOURCE=.\binding\showBinding.cpp
# End Source File
# Begin Source File

SOURCE=.\bool\showBool.cpp
# End Source File
# Begin Source File

SOURCE=.\byteString\showByteString.cpp
# End Source File
# Begin Source File

SOURCE=.\char\showChar.cpp
# End Source File
# Begin Source File

SOURCE=.\class\showClass.cpp
# End Source File
# Begin Source File

SOURCE=.\double\showDouble.cpp
# End Source File
# Begin Source File

SOURCE=.\float\showFloat.cpp
# End Source File
# Begin Source File

SOURCE=.\int\showInt.cpp
# End Source File
# Begin Source File

SOURCE=.\list\showList.cpp
# End Source File
# Begin Source File

SOURCE=.\name\showName.cpp
# End Source File
# Begin Source File

SOURCE=.\ord\showOrdering.cpp
# End Source File
# Begin Source File

SOURCE=.\tuple\showPair.cpp
# End Source File
# Begin Source File

SOURCE=.\propertySet\showPropertySet.cpp
# End Source File
# Begin Source File

SOURCE=.\ref\showRef.cpp
# End Source File
# Begin Source File

SOURCE=.\refList\showRefList.cpp
# End Source File
# Begin Source File

SOURCE=.\root\showRoot.cpp
# End Source File
# Begin Source File

SOURCE=.\string\showString.cpp
# End Source File
# Begin Source File

SOURCE=.\type\showType.cpp
# End Source File
# Begin Source File

SOURCE=.\UTC\showUTC.cpp
# End Source File
# Begin Source File

SOURCE=.\machine\showVal.cpp
# End Source File
# Begin Source File

SOURCE=.\event\showWhen.cpp
# End Source File
# Begin Source File

SOURCE=.\double\sin.cpp
# End Source File
# Begin Source File

SOURCE=.\double\sinh.cpp
# End Source File
# Begin Source File

SOURCE=.\type\sizeOfType.cpp
# End Source File
# Begin Source File

SOURCE=.\double\sqrt.cpp
# End Source File
# Begin Source File

SOURCE=.\machine\start.cpp
# End Source File
# Begin Source File

SOURCE=.\event\startEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\machine\stop.cpp
# End Source File
# Begin Source File

SOURCE=.\event\stopEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\type\storeType.cpp
# End Source File
# Begin Source File

SOURCE=.\machine\strict.cpp
# End Source File
# Begin Source File

SOURCE=.\string\string.cpp
# End Source File
# Begin Source File

SOURCE=.\propertySet\stringProperty.cpp
# End Source File
# Begin Source File

SOURCE=.\list\tail.cpp
# End Source File
# Begin Source File

SOURCE=.\double\tan.cpp
# End Source File
# Begin Source File

SOURCE=.\double\tanh.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\template.cpp
# End Source File
# Begin Source File

SOURCE=.\int\testPlus.cpp
# End Source File
# Begin Source File

SOURCE=.\UTC\time.cpp
# End Source File
# Begin Source File

SOURCE=.\UTC\timeDiff.cpp
# End Source File
# Begin Source File

SOURCE=.\machine\trace.cpp
# End Source File
# Begin Source File

SOURCE=.\tuple\tuple.cpp
# End Source File
# Begin Source File

SOURCE=.\repr\type.cpp
# End Source File
# Begin Source File

SOURCE=.\repr\typeCon.cpp
# End Source File
# Begin Source File

SOURCE=.\type\typeNeedsEnv.cpp
# End Source File
# Begin Source File

SOURCE=.\repr\typeOf.cpp
# End Source File
# Begin Source File

SOURCE=.\type\typeRepr.cpp
# End Source File
# Begin Source File

SOURCE=.\repr\typeTable.cpp
# End Source File
# Begin Source File

SOURCE=.\list\unique.cpp
# End Source File
# Begin Source File

SOURCE=.\type\unpackTypeSig.cpp
# End Source File
# Begin Source File

SOURCE=.\event\updatePropertyEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\UTC\UTC.cpp
# End Source File
# Begin Source File

SOURCE=.\context\var.cpp
# End Source File
# Begin Source File

SOURCE=.\class\varSetClass.cpp
# End Source File
# Begin Source File

SOURCE=.\type\vector.cpp
# End Source File
# Begin Source File

SOURCE=.\void\void.cpp
# End Source File
# Begin Source File

SOURCE=.\event\when.cpp
# End Source File
# Begin Source File

SOURCE=.\parser\yystack.cpp
# End Source File
# Begin Source File

SOURCE=.\parser\yywrap.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
