/*------------------------------------------------------------------------------
 *
 * (c) Copyright (see Date)
 *
 * Name:    context.cpp
 *
 * Module:  IvoryScript parser context
 *
 * Author:  Alasdair Scott
 *
 * Date:    21 August 2025
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Functions relating to the supply of context sensitive tokens.
 *
 * Errors:
 *
 * Notes:
 *
 * Modification history:
 *
 *-----------------------------------------------------------------------------
 */

#include "ivory/compiler/parser.h"

Bool pendingArrowContext_ = FALSE;

Bool pendingArrowContext(Void) {
   return pendingArrowContext_;
}

Void enterPendingArrowContext(Void) {
   pendingArrowContext_ = TRUE;
}

Void exitPendingArrowContext(Void) {
   pendingArrowContext_ = FALSE;
}

Bool typeSigContext_ = FALSE;

Bool typeSigContext(Void) {
   return typeSigContext_;
}

Void enterTypeSigContext(Void) {
   typeSigContext_ = TRUE;
}

Void exitTypeSigContext(Void) {
   typeSigContext_ = FALSE;
}

Void clearParserContext(Void) {
   pendingArrowContext_ = FALSE;
   typeSigContext_ = FALSE;
}

