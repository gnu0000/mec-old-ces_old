/*--------------------------------------------------------------------------+
|                                                                           |
|       Copyright (c) 1992 by Info Tech, Inc.  All Rights Reserved.         |
|                                                                           |
|       This program module is part of DS/Shell (PC), Info Tech's           |
|       database interfaces for OS/2, a proprietary product of              |
|       Info Tech, Inc., no part of which may be reproduced or              |
|       transmitted in any form or by any means, electronic,                |
|       mechanical, or otherwise, including photocopying and recording      |
|       or in connection with any information storage or retrieval          |
|       system, without permission in writing from Info Tech, Inc.          |
|                                                                           |
+--------------------------------------------------------------------------*/


/*
 * dll.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * This file provides dll management functions.
 */

#define INCL_DOSMODULEMGR
#define INCL_DOSERRORS
#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "initdll.h"

#ifdef DEBUG
#include <stdio.h>
#include "..\include\itierror.h"
#endif





/*
 * ItiDllLoadDll loads the dynamic link library pszDllName, and verifies 
 * that it is compatible with usVersion.
 *
 * Parameters: pszDllName:    The name of the DLL to load.  Usually,
 *                            the caller should not include the ".DLL"
 *                            extension.
 *
 *             usVersion:     The version of the caller.
 *
 * Return Value:
 * A handle to the module, or 0 on error.   
 * 
 * Comments:
 * Errors could be caused by any of the following:  DLL not found,
 * DLL doesn't have version function, DLL is incompatible with your
 * version.
 */

HMODULE EXPORT ItiDllLoadDll (PSZ pszDllName, PSZ pszVersion)
   {
   char        szTemp [100];
   char        szMessage [1024];
   USHORT      usError;
   HMODULE     hmod;
   PFNCOMPAT   pfnCompat;
   PFNINIT     pfnInit;
   
   usError = DosLoadModule (szTemp, sizeof szTemp, pszDllName, &hmod);
   if (usError)
      {
#ifdef DEBUG
      sprintf (szMessage, "ItiDllLoadDll: could not load dll %s. usError = %u, file: %s", 
               pszDllName, usError, szTemp);
      ItiErrWriteDebugMessage (szMessage);
#endif
      return 0;
      }
   else
      {
      /* at this point, we need to check the version of the DLL.  Better do
         this at some time. */

      usError = DosGetProcAddr (hmod, "ITIDLLQUERYCOMPATIBILITY", &pfnCompat);
      if (usError)
         {
         if (usError != ERROR_PROC_NOT_FOUND)
            {
#ifdef DEBUG
            sprintf (szMessage, "ItiDllLoadDll: could not get query compat function in %s. usError = %u", 
                     pszDllName, usError);
            ItiErrWriteDebugMessage (szMessage);
#endif
            DosFreeModule (hmod);
            return 0;
            }
         }
      else
         {
     // The following removed for 1.1a integration test.
     //    if (!pfnCompat (pszVersion))
     //       {
     //       sprintf (szMessage,
     //          "ItiDllLoadDll: Possible version incompatiblity with %s.dll ", 
     //          pszDllName);
     //       ItiErrWriteDebugMessage (szMessage);
     //       DosFreeModule (hmod);
     //       return 0;
     //       }
         }

      /* call the DLL's initialization function. */
      usError = DosGetProcAddr (hmod, "ITIDLLINITDLL", &pfnInit);
      if (usError == 0)
         {
         if (!pfnInit ())
            {
#ifdef DEBUG
            sprintf (szMessage, "ItiDllLoadDll: Could not initialize DLL %s", 
                     pszDllName);
            ItiErrWriteDebugMessage (szMessage);
#endif
            DosFreeModule (hmod);
            return 0;
            }
         }
      }

#ifdef DEBUG
   if (bVerbose)
      {
      sprintf (szMessage, "ItiDllLoadDll: Loaded %s as module %u", 
               pszDllName, hmod);
      ItiErrWriteDebugMessage (szMessage);
      }
#endif

   return hmod;
   }





/*
 * ItiDllQueryProcAddress returns a pointer to the address of a 
 * function in the specified DLL.
 *
 * Parameters: hmodDll           The DLL that contains the function.
 *
 *             pszFunctionName   The name of the function to be found.
 *
 * Return Value:
 * A pointer to the function, or NULL on error.
 *
 */

PDLLFN EXPORT ItiDllQueryProcAddress (HMODULE hmodDll,
                                      PSZ     pszFunctionName)
   {
   PDLLFN   pdllfn;
   USHORT   usError;

   usError = DosGetProcAddr (hmodDll, pszFunctionName, &pdllfn);
   if (usError)
      {
#ifdef DEBUG
      char szTemp [1024];
      sprintf (szTemp, "ItiDllQueryProcAddress: could not find function %s in module %u.  Error = %u",
               pszFunctionName, hmodDll, usError);
      ItiErrWriteDebugMessage (szTemp);
#endif
      return NULL;
      }
   return pdllfn;
   }




/*
 * ItiDllQueryDllVersion returns the version number of the specified DLL.
 *
 * Parameters: hmodDll           The DLL to check.
 *
 * Return Value:
 * The version number of the DLL, or 0 on error.
 *
 */

USHORT EXPORT ItiDllQueryDllVersion (HMODULE hmodDll)
   {
   PFNVERSION pfnVersion;

return 1; /* skip the version check for now */

   pfnVersion = ItiDllQueryProcAddress (hmodDll, "ITIDLLQUERYVERSION");
   if (pfnVersion != NULL)
      return pfnVersion ();
   else
      return 0;
   }







/*
 * ItiDllFreeDll frees the given DLL.  The caller must no longer 
 * try to use the DLL, unless ItiDllLoadDll is called.
 */

void EXPORT ItiDllFreeDll (HMODULE hmodDll)
   {
   USHORT      usError;
   PFNDEINIT   pfnDeinit;

#ifdef DEBUG
   char        szMessage [1024];

   sprintf (szMessage, "ItiDllLoadDll: Freed module %u", 
            hmodDll);
   ItiErrWriteDebugMessage (szMessage);
#endif
   
   /* call the DLL's deinitialization function. */
   usError = DosGetProcAddr (hmodDll, "ITIDLLDEINITDLL", &pfnDeinit);
   if (usError == 0)
      pfnDeinit ();
   
   DosFreeModule (hmodDll);
   }




