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
 * ItiEst.c
 * Mark Hampton
 */

#include "..\include\iti.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itiutil.h"
#include "..\include\itiest.h"
#include "..\include\itiwin.h"
#include "..\include\itimenu.h"
#include "..\include\itiglob.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "initdll.h"

static HHEAP hheapEstLoc;

typedef struct
   {
   PSZ         pszID;      /* estimation method ID */
   PSZ         pszAbbrev;  /* estimation method ID */
   PSZ         pszMenu;    /* text to place in job item view menu */
   HMODULE     hmod;       /* module handle */
   PFNEST      pfnest;     /* esitmation function */
   PFNJAVAIL   pfnjavail;  /* price available in job */
   PFNAVAIL    pfnavail;   /* price available in catalog */
   USHORT      usWindow;   /* window to open from job item window */
   } ESTINFO;

typedef ESTINFO *PESTINFO;

static USHORT usEstCount = 0;
static PESTINFO pest;


USHORT EXPORT ItiEstChangeViewMenu (HWND     hwndMenu,
                                    HWND     hwndList,
                                    USHORT   usCommandID,
                                    PSZ      pszJobKey,
                                    PSZ      pszJobItemKey,
                                    HHEAP    hhpCVM )
   {
   USHORT i;
   HHEAP  hhp;
   BOOL   bDisable;
   PSZ    pszJobItemEstMethodKey = NULL;
   CHAR   szTemp[256] = "";

   /* initialize our heap */
   hhp = ItiMemCreateHeap (1024);
   if (hhp != NULL)
      {
      ItiStrCpy (szTemp,
                "SELECT ItemEstimationMethodKey FROM JobItem WHERE JobKey = "
                , sizeof szTemp);
      ItiStrCat (szTemp, pszJobKey, sizeof szTemp);
      ItiStrCat (szTemp, " AND JobItemKey = ", sizeof szTemp);
      ItiStrCat (szTemp, pszJobItemKey, sizeof szTemp);

      pszJobItemEstMethodKey = ItiDbGetRow1Col1 (szTemp, hhp, 0, 0, 0);
      }

   for (i=0; i < usEstCount; i++)
      {
      if (pest [i].pszMenu && pest [i].usWindow)
         {
         ItiMenuInsertMenuItem (hwndMenu, pest [i].pszMenu, usCommandID + i);
         if (pest [i].pfnjavail == NULL)
            bDisable = FALSE;
         else if (pszJobKey == NULL || pszJobItemKey == NULL)
            bDisable = TRUE;
         else
            {                     
            bDisable = !(pest [i].pfnjavail (hhpCVM, pszJobKey, pszJobItemKey));
            }

         /* Disable the Cst view selection if JI method is NOT Cst. */
         if (0 != ItiStrCmp(pszJobItemEstMethodKey, "1000001") &&
             0 == ItiStrCmp(pest [i].pszID, "CBEst") )
            bDisable = TRUE;

         ItiMenuGrayMenuItem (hwndMenu, usCommandID + i, bDisable);
         }
      }

   if (hhp)
      ItiMemDestroyHeap (hhp);

   return usEstCount;
   }


USHORT EXPORT ItiEstQueryMethod (HHEAP hhpQM,
                                 PSZ   pszStandardItemKey,
                                 PSZ   pszBaseDate,
                                 PSZ   pszMethodID)
   {
   USHORT i;
   BOOL   bOK;

   if (pszMethodID == NULL || pszStandardItemKey == NULL)
      return FALSE;

   bOK = FALSE;
   for (i=0; i < usEstCount; i++)
      {
      if (pest [i].pszID && ItiStrICmp (pest [i].pszID, pszMethodID) == 0)
         {
         if (pest [i].pfnavail != NULL)
            bOK = pest [i].pfnavail (hhpQM, pszStandardItemKey, pszBaseDate);
         else
            bOK = TRUE;
         break;
         }
      }
   return bOK;
   }



HWND EXPORT ItiEstBuildMethodWnd (HWND  hwnd,
                                  PSZ   pszMethodAbbrev)
   {
   USHORT i;

   if (pszMethodAbbrev == NULL ||
       !WinIsWindow (pglobals->habMainThread, hwnd))
      return NULL;

   for (i=0; i < usEstCount; i++)
      {
      if (pest [i].pszAbbrev &&
          ItiStrICmp (pest [i].pszAbbrev, pszMethodAbbrev) == 0)
         {
         return ItiWndBuildWindow (hwnd, pest [i].usWindow);
         }
      }
   return NULL;
   }



HWND EXPORT ItiEstProcessViewCommand (HWND   hwndParent,
                                      USHORT usCommandStart,
                                      USHORT usCommand)
   {
   return ItiWndBuildWindow (hwndParent,
                             pest [usCommand - usCommandStart].usWindow);
   }



PSZ EXPORT ItiEstEstimateItem (HHEAP   hhpEI, 
                               PSZ     pszEstimationID,
                               PSZ     pszBaseDate, 
                               PSZ     pszJobKey, 
                               PSZ     pszJobBreakdownKey,
                               PSZ     pszStandardItemKey, 
                               PSZ     pszQuantity)
   {
   USHORT i;

   /* find the estimation method */
   if (pszEstimationID == NULL || pest == NULL)
      return NULL;

   for (i=0; i < usEstCount; i++)
      {
      if (pest [i].pszID && 
          ItiStrICmp (pszEstimationID, pest [i].pszID) == 0 &&
          pest [i].pfnest)
         {
         return pest [i].pfnest (hhpEI, pszBaseDate, pszJobKey, 
                                 pszJobBreakdownKey, pszStandardItemKey, 
                                 pszQuantity);
         }
      }
   return NULL;
   }




BOOL EXPORT ItiEstInitialize (void)
   {
   static BOOL bInited = FALSE;
   USHORT usNumRows, i, usError;
   HQRY hqry;
   PSZ *ppsz, pszTemp;
   char szTemp [256] = "";
   char szRes0 [16] = "";
   char szRes1 [16] = "";
   PFNMENUNAME pfnmenu;

   if (bInited)
      return TRUE;

   /* initialize our hheapEstLoc */
   hheapEstLoc = ItiMemCreateHeap (4000);
   if (hheapEstLoc == NULL)
      return FALSE;

   /* figure out how many structures to allocate */
   pszTemp = ItiDbGetRow1Col1 ("SELECT COUNT (*) FROM ItemEstimationMethod", 
                               hheapEstLoc, 0, 0, 0);
   if (pszTemp == NULL || *pszTemp == '\0' || 
       !ItiStrToUSHORT (pszTemp, &usEstCount))
      return FALSE;

   if (usEstCount == 0)
      return FALSE;

   pest = ItiMemAlloc (hheapEstLoc, sizeof *pest * usEstCount, MEM_ZERO_INIT);
   if (pest == NULL)
      return FALSE;

   hqry = ItiDbExecQuery ("SELECT ItemEstimationMethodID, "
                          "EstimationMethodAbbreviation "
                          "FROM ItemEstimationMethod "
                          "ORDER BY ItemEstimationMethodID ",
                          hheapEstLoc, 0, 0, 0, &usNumRows, &usError);
   if (hqry == NULL)
      return FALSE;

   i = 0;
   while (ItiDbGetQueryRow (hqry, &ppsz, &usError))
      {
      pest [i].pszID = ppsz[0];
      pest [i].pszAbbrev = ppsz[1];
      pest [i].hmod = ItiDllLoadDll (ppsz[0], "1");
      if (pest [i].hmod != 0)
         {
         pest [i].pfnest = (PFNEST) ItiDllQueryProcAddress (pest [i].hmod, 
                                                            "ITIESTITEM");
         pest [i].pfnjavail = (PFNJAVAIL) 
                               ItiDllQueryProcAddress (pest [i].hmod, 
                                                       "ITIESTQUERYJOBAVAIL");
         pest [i].pfnavail = (PFNAVAIL) 
                               ItiDllQueryProcAddress (pest [i].hmod, 
                                                       "ITIESTQUERYAVAIL");
         pfnmenu = ItiDllQueryProcAddress (pest [i].hmod, 
                                           "ITIESTQUERYMENUNAME");
         if (pfnmenu != NULL)
            {
            pfnmenu (szTemp, sizeof szTemp, &(pest [i].usWindow));
            pest [i].pszMenu = ItiStrDup (hheapEstLoc, szTemp);
            }
         }
      else
         {
         pest [i].pfnest = NULL;
         pest [i].pfnjavail = NULL;
         pest [i].pfnavail = NULL;
         pest [i].pszMenu = NULL;
         pest [i].usWindow = i;
         }
      i++;
      
      ItiMemFree (hheapEstLoc, ppsz);
      }

   usEstCount = i;

   bInited = TRUE;
   return TRUE;
   }


