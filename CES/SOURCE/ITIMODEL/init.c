/*--------------------------------------------------------------------------+
|                                                                           |
|       Copyright (c) 1992 by AASHTO.  All Rights Reserved.                 |
|                                                                           |
|       This program module is part of BAMS/CES, a module of BAMS,          |
|       The American Association of State Highway and Transportation        |
|       Officials' (AASHTO) Information System for Managing Transportation  |
|       Programs, a proprietary product of AASHTO, no part of which may be  |
|       reproduced or transmitted in any form or by any means, electronic,  |
|       mechanical, or otherwise, including photocopying and recording      |
|       or in connection with any information storage or retrieval          |
|       system, without permission in writing from AASHTO.                  |
|                                                                           |
+--------------------------------------------------------------------------*/


/*
 * init.c
 * Mark Hampton
 *
 * This module defines global variables for the itimenu module.
 */

#include "..\include\iti.h"
#include "..\include\itierror.h"
#include "..\include\itibase.h"
#include "..\include\itiglob.h"
#include "..\include\winid.h"
#include "..\include\itimbase.h"
#include "..\include\itiwin.h"
#include <stdio.h>
#include "init.h"
#include "profile.h"
#include "quantity.h"
#include "price.h"

HMODULE hmodMe;

PGLOBALS pglobals;


static BOOL ItiModelInitialize (void)
   {
   static BOOL bInited = FALSE;

   if (bInited)
      return TRUE;

   bInited = TRUE;

   ItiMbRegisterDialogWnd (ModelProfileD, ModelProfileD, ProfileDlgProc, hmodMe);
   ItiMbRegisterDialogWnd (ModelQuantityD, ModelQuantityD, QuantityDlgProc, hmodMe);
   ItiMbRegisterDialogWnd (ModelPriceD, ModelPriceD, PriceDlgProc, hmodMe);

   ItiMbRegisterListWnd (ModelWorkTypeL,  ItiWndDefListWndTxtProc, hmodMe);
   ItiMbRegisterListWnd (ModelRoadTypeL,  ItiWndDefListWndTxtProc, hmodMe);
   ItiMbRegisterListWnd (ModelDistrictL,  ItiWndDefListWndTxtProc, hmodMe);
   ItiMbRegisterListWnd (ModelMajorItemL, ItiWndDefListWndTxtProc, hmodMe);
   ItiMbRegisterListWnd (ModelStatusL,    ItiWndDefListWndTxtProc, hmodMe);

   return TRUE;
   }

/*
 * DllInit is called by either ITIDLLI.OBJ or ITIDLLC.OBJ.  This function
 * must return non-zero if the initialization succeded, or FALSE
 * if initialization failed.
 */

BOOL DLLINITPROC DllInit (HMODULE hmodDll)
   {
   hmodMe = hmodDll;
   pglobals = ItiGlobQueryGlobalsPointer ();

   /* call your module initialization functions here. */
   return ItiModelInitialize ();
   }


