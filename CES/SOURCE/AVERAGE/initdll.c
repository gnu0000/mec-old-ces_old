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
 * InitDll.c
 * Mark Hampton
 *
 * This is an almost generic DLL initialization module.  The function 
 * DllInit is called by either the ITIDLLI.OBJ or ITIDLLC.OBJ module.
 * 
 * Use the ITIDLLI.OBJ module when your DLL does not use the C run time
 * library.  Use the ITIDLLC.OBJ module when you need to use the C run
 * time library, and you need to initialize your DLL.  Note that when
 * you do this, you will have to link with CRTDLL_I.OBJ, a standard 
 * Microsoft module.
 *
 * Note: Do not rename this file to DLLINIT, since this file name
 * is already used by Microsoft for C run time library initialization.
 */

#include "..\include\iti.h"
#include "..\include\itiglob.h"
#include "..\include\itidbase.h"
#include "..\include\itibase.h"
#include "initdll.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>



HMODULE hmod;

static CHAR szInit1[] = "/* Average.dll init */ "
          " DELETE FROM DSShell..StandardItemPEMETHAverage "
          " WHERE StandardItemKey NOT IN "
          " (select distinct StandardItemKey from DSShell..PEMETHAverage) " ;

static CHAR szInit2[] = "/* Average.dll init */ "
          " DELETE FROM DSShell..WeightedAverageUnitPrice "
          " WHERE StandardItemKey NOT IN "
          " (select distinct StandardItemKey from DSShell..PEMETHAverage) ";

/*
 * DllInit is called by either ITIDLLI.OBJ or ITIDLLC.OBJ.  This functino
 * must return non-zero if the initialization succeded, or FALSE
 * if initialization failed.
 */

BOOL DLLINITPROC DllInit (HMODULE hmodDll)
   {
   HHEAP hhp;
   /* call your module initialization functions here. */
   hmod = hmodDll;

   hhp = ItiMemCreateHeap(1000);

   ItiDbExecSQLStatement(hhp, szInit1);
   ItiDbExecSQLStatement(hhp, szInit2);

   ItiMemDestroyHeap(hhp);

   return TRUE;
   }


