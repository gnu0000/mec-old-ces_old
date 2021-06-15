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
 * avedll.c
 */

#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itimbase.h"
#include "..\include\itiutil.h"
#include "..\include\itifmt.h"
#include "..\include\itiwin.h"
#include "..\include\itiest.h"
#include "..\include\winid.h"
#include "..\include\itiglob.h"
#include "..\include\itiimp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "average.h"
#include "initdll.h"
#include "avewin.h"


static CHAR szDllVersion[] = "1.1a0 Avedll.dll";

/* global variables */
PGLOBALS pglobals = NULL;


USHORT EXPORT ItiDllQueryVersion (void)
   {
   return VERSION;
   }



BOOL EXPORT ItiDllInitDll (void)
   {
   static BOOL bInited = FALSE;

   if (bInited)
      return TRUE;

   bInited = TRUE;

   pglobals = ItiGlobQueryGlobalsPointer ();

   if (ItiMbRegisterStaticWnd (JobPemethAvgS, JobPemethAvgSProc, hmod))
      return FALSE;

   if (ItiMbRegisterStaticWnd (PemethAvgS, PemethAvgSProc, hmod))
      return FALSE;

   return TRUE;
   }



/*
 * ItiDllQueryCompatibility returns TRUE if this DLL is compatable
 * with the caller's version numer, otherwise FALSE is returned.
 */
BOOL EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion)

   {
   if ( ItiVerCmp(szDllVersion) )
      return TRUE;
   return FALSE;
   }



VOID EXPORT ItiDllQueryMenuName (PSZ      pszBuffer, 
                                 USHORT   usMaxSize,
                                 PUSHORT  pusWindowID)
   {
   ItiStrCpy (pszBuffer, "~PEMETH Averages", usMaxSize);
   *pusWindowID = PemethAvgS;
   }



VOID EXPORT ItiEstQueryMenuName (PSZ      pszBuffer, 
                                 USHORT   usMaxSize,
                                 PUSHORT  pusWindowID)
   {
   ItiStrCpy (pszBuffer, "~PEMETH Average", usMaxSize);
   *pusWindowID = JobPemethAvgS;
   }



