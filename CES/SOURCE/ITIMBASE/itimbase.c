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


/************************************************************************
 *                                                                      *
 * itimbase.c                                                           *
 *                                                                      *
 * Copyright (C) 1990-1992 Info Tech, Inc.                              *
 *                                                                      *
 * This file is part of the Shell system of DS/SHELL. This is a         *
 * proprietary product of Info Tech. and no part of which may be        *
 * reproduced or transmitted in any form or by any means, including     *
 * photocopying and recording or in connection with any information     *
 * storage or retrieval system, without permission in writing           *
 * from Info Tech, Inc.                                                 *
 *                                                                      *
 *                                                                      *
 ************************************************************************/

#define INCL_DOS
#include "..\include\iti.h"
#include "..\include\resource.h"
#include "..\include\itibase.h"
#include "..\include\itiwin.h"
#include "..\include\itimbase.h"
#include "..\include\itiutil.h"
#include "..\include\itiglob.h"
#include "..\include\itierror.h"
#include "initdll.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>



USHORT MbErr (PSZ psz, SHORT uVal, USHORT uRet)
   {
   char  szTemp [255];

   /* prepend the string "ITIMBASE: " to the error message. */
   strcpy (szTemp, "ITIMBASE: ");
   sprintf (szTemp + strlen (szTemp), psz, uVal);
   ItiErrWriteDebugMessage (szTemp);

   DosBeep (200, 50);
   DosBeep (800, 50);

   if (pglobals != NULL && 
       pglobals->hwndDesktop != NULL &&
       pglobals->hwndAppFrame != NULL)
      {
      WinMessageBox (pglobals->hwndDesktop, pglobals->hwndAppFrame, szTemp,
                     "ITIMBASE", 0, MB_OK | MB_SYSTEMMODAL | MB_MOVEABLE);
      }
   return uRet;
   }



/* converts short rct to long rcl */
RECTL RECTStoL (RECTS rcs)
   {
   RECTL rcl;

   rcl.xLeft   = rcs.xLeft  ;
   rcl.yBottom = rcs.yBottom;
   rcl.xRight  = rcs.xRight ;
   rcl.yTop    = rcs.yTop   ;
   return rcl;
   }



/*
 * Binary Search.
 * Works with duplicates
 * guarantees to return the 1st
 *  (lowest index) match.
 *
 * Guarantees not only not to work, by to confuse all who gaze at it.
 */

SHORT FindFirst ( PVOID pbase,
                  SHORT iCount,
                  SHORT iSize,
                  SHORT iMatch,
                  SHORT (* cmpfn)(SHORT iMatch, PVOID pElement))
   {
   SHORT iStart, iMid, iEnd;

   iStart = 0;
   iEnd   = iCount - 1;

   while (iStart < iEnd || iStart < iMid)
      {
      iMid   = (iEnd + iStart) / 2;
      switch (cmpfn ( iMatch, (char *)pbase + iMid * iSize))
         {
         case  1: iStart = iMid + 1;    break;
         case  0: iEnd   = iMid;        break;
         case -1: iEnd   = iMid - 1;    break;
         }
      }
   return (iStart >= iCount || cmpfn (iMatch, (char *)pbase+ iStart* iSize) ? -1 : iStart);
   }




USHORT GetCustomHMOD (USHORT uId, HMODULE *hmod)
   {
   return 1;
   }


USHORT GetWindowHMOD (USHORT uId, HMODULE *phmod)
   {
   ItiMbQueryHMOD (uId, phmod);
   return 0;
   }



/* This proc will eventually look in the custom res and
 * make sure the actual win id is present before returning it
 */
USHORT ItiMbGetResource (HMODULE hmod,
                         USHORT  uResType,
                         USHORT  uResId,
                         USHORT  uElement,
                         PVOID  *pVoid)
   {
   HMODULE  hmod2;

   if (GetCustomHMOD (uElement, &hmod2) ||
       DosGetResource2 (hmod, uResType, uResId, pVoid))
      {
      if (DosGetResource2 (hmod, uResType, uResId, pVoid))
         return 1;
      }
   return 0;
   }





/*************************************************************************/

SHORT CDCmp (SHORT iMatch, PVOID pElement)
   {
   CD  *pcd;

   pcd = (CD *)pElement;
   return max (-1, min (1, iMatch - ((SHORT)pcd->uId)));
   }


/*
 * Metabase Error Returns
 * 0 = no error
 * 1 = unable to find resource
 * 2 = no match found
 * 3 = Other Error
 */

USHORT EXPORT ItiMbGetChildInfo (HMODULE hmod,
                                 USHORT  uResId,
                                 USHORT  uId,
                                 HHEAP   hheap,
                                 PCHD    *ppCHD,
                                 USHORT  *uNum)
   {
   PVOID    pBuff;
   CD       *pcd;
   SHORT    i;
   USHORT   uCount, uEntrys;

   *ppCHD = NULL;
   /*--- load resource ---*/
   if (ItiMbGetResource (hmod, ITIRT_CHILD, uResId, uId, &pBuff))
      return MbErr ("Unable to load Child Info Resource", 0, 1);

   *uNum  = 0;

   if (!(uCount = *((USHORT *) pBuff)))
      {
      DosFreeResource (pBuff);
      return 2;
      }

   pcd    = (CD *)((char *)pBuff + 2);
   /*--- find first entry in resource ---*/
   if ((i = FindFirst (pcd, uCount, sizeof (CD), uId, CDCmp)) != -1)
      {
      /*--- get number of entrys in resource ---*/
      for (uEntrys=0; (USHORT)i+uEntrys < uCount && pcd[i+uEntrys].uId==uId; uEntrys++)
         ;
      *uNum = uEntrys;

      if ((*ppCHD = ItiMemAlloc (hheap, uEntrys * sizeof (CHD), 0)) == NULL)
         return MbErr ("Unable to alloc memory for Child Data", 0, 3);
      while (uEntrys--)
         {
         (*ppCHD)[uEntrys].uId        = pcd[i+uEntrys].uChildId ;
         (*ppCHD)[uEntrys].ulKind     = pcd[i+uEntrys].ulKind ;
         (*ppCHD)[uEntrys].uAddId     = pcd[i+uEntrys].uAddId;
         (*ppCHD)[uEntrys].uChangeId  = pcd[i+uEntrys].uChangeId;
         (*ppCHD)[uEntrys].rcl        = RECTStoL (pcd[i+uEntrys].rcs);
         (*ppCHD)[uEntrys].uCoordType = pcd[i+uEntrys].uCoordType;
         }
      }

   DosFreeResource (pBuff);
   return 0;
   }


/*************************************************************************/
SHORT LDCmp (SHORT iMatch, PVOID pElement)
   {
   LD *pld;

   pld = (LD *)pElement;
   return max (-1, min (1, iMatch - ((SHORT)pld->uId)));
   }


USHORT EXPORT ItiMbGetLabelInfo ( HMODULE   hmod,
                                  USHORT uResId,
                                  USHORT uId,
                                  HHEAP  hheap,
                                  PEDT   *ppEDT,
                                  PSZ    **pppsz,
                                  USHORT *uNum)
   {
   PVOID    pBuff;
   PSZ      pStrBuff, pStr;
   LD       *pld;
   SHORT    i;
   USHORT   uCount, uEntrys;

   *ppEDT = NULL;
   *pppsz = NULL;
   /*--- load resources ---*/
   if (ItiMbGetResource (hmod, ITIRT_LABEL, uResId, uId, &pBuff))
      return MbErr ("Unable to load Label Resource",0, 1);
   if (ItiMbGetResource (hmod, ITIRT_LABELTXT, uResId, uId, &pStrBuff))
      return MbErr ("Unable to load Label Txt Resource",0, 1);

   *uNum  = 0;
   if (!(uCount = *((USHORT *) pBuff)))
      {
      DosFreeResource (pBuff);
      DosFreeResource (pStrBuff);
      return 2;
      }
   pld    = (LD *)((char *)pBuff + 2);

   /*--- find first entry in resource ---*/
   if ((i = FindFirst (pld, uCount, sizeof (LD), uId, LDCmp)) != -1)
      {
      /*--- get number of entrys in resource ---*/
      for (uEntrys=0; (USHORT)i + uEntrys < uCount && pld[i+uEntrys].uId==uId; uEntrys++)
         ;
      *uNum = uEntrys;

      if ((*ppEDT = ItiMemAlloc (hheap, uEntrys * sizeof (EDT), 0)) == NULL)
         return MbErr ("Unable to alloc memory for Label Data", 0, 3);
      if ((*pppsz = ItiMemAlloc (hheap, uEntrys * sizeof (PSZ), 0)) == NULL)
         return MbErr ("Unable to alloc memory for Label Strings", 0, 3);

      while (uEntrys--)
         {
         /* --- this field is currently redundant but could
                be needed at a later time.                    --- */

         (*ppEDT)[uEntrys].uIndex      = uEntrys;
         (*ppEDT)[uEntrys].rcl         = RECTStoL (pld[i+uEntrys].rcs);
         (*ppEDT)[uEntrys].uCoordType  = pld[i+uEntrys].uCoordType;
         (*ppEDT)[uEntrys].uNextId     = pld[i+uEntrys].uNextId;
         (*ppEDT)[uEntrys].lFColor     = pld[i+uEntrys].lFColor;
         (*ppEDT)[uEntrys].lFlags      = pld[i+uEntrys].lFlags;

         pStr = &(pStrBuff[pld[i+uEntrys].uOffset]);
         (*pppsz)[uEntrys] = ItiStrDup (hheap, pStr);
         }
      }
   DosFreeResource (pBuff);
   DosFreeResource (pStrBuff);
   return 0;
   }

                       
/*************************************************************************/

SHORT DDCmp (SHORT iMatch, PVOID pElement)
   {
   DD *pdd;

   pdd =(DD *)pElement;
   return max (-1, min (1, iMatch - ((SHORT)pdd->uId)));
   }


USHORT EXPORT ItiMbGetDataInfo  ( HMODULE   hmod,
                                  USHORT uResId,
                                  USHORT uId,
                                  HHEAP  hheap,
                                  PEDT   *ppEDT,
                                  PSZ    **pppsz,
                                  USHORT *uNum)
   {
   PVOID    pBuff;
   PSZ      pStrBuff, pStr;
   DD       *pdd;
   SHORT    i;
   USHORT   uCount, uEntrys;

   *ppEDT = NULL;
   *pppsz = NULL;
   /*--- load resources ---*/
   if (ItiMbGetResource (hmod, ITIRT_DATA, uResId, uId, &pBuff))
      return MbErr ("Unable to load Data Resource",0, 1);
   if (ItiMbGetResource (hmod, ITIRT_FORMAT, uResId, uId, &pStrBuff))
      return MbErr ("Unable to load Data Txt Resource",0, 1);

   *uNum  = 0;
   if (!(uCount = *((USHORT *) pBuff)))
      {
      DosFreeResource (pBuff);
      DosFreeResource (pStrBuff);
      return 2;
      }
   pdd    = (DD *)((char *)pBuff + 2);

   /*--- find first entry in resource ---*/
   if ((i = FindFirst (pdd, uCount, sizeof (DD), uId, DDCmp)) != -1)
      {
      /*--- get number of entrys in resource ---*/
      for (uEntrys=0; (USHORT)i + uEntrys < uCount && pdd[i+uEntrys].uId==uId; uEntrys++)
         ;
      *uNum = uEntrys;

      if ((*ppEDT = ItiMemAlloc (hheap, uEntrys * sizeof (EDT), 0)) == NULL)
         return MbErr ("Unable to alloc memory for Data Info", 0, 3);
      if ((*pppsz = ItiMemAlloc (hheap, uEntrys * sizeof (PSZ), 0)) == NULL)
         return MbErr ("Unable to alloc memory for Label Strings", 0, 3);


      while (uEntrys--)
         {
         (*ppEDT)[uEntrys].uIndex      = pdd[i+uEntrys].uColumn;
         (*ppEDT)[uEntrys].uNextId     = pdd[i+uEntrys].uNextId;
         (*ppEDT)[uEntrys].rcl         = RECTStoL (pdd[i+uEntrys].rcs);
         (*ppEDT)[uEntrys].uCoordType  = pdd[i+uEntrys].uCoordType;
         (*ppEDT)[uEntrys].lFColor     = pdd[i+uEntrys].lFColor;
         (*ppEDT)[uEntrys].lFlags      = pdd[i+uEntrys].lFlags;

         pStr = &(pStrBuff[pdd[i+uEntrys].uOffset]);
         (*pppsz)[uEntrys] = ItiStrDup (hheap, pStr);
         }
      }
   DosFreeResource (pBuff);
   DosFreeResource (pStrBuff);
   return 0;
   }



/*************************************************************************/

SHORT IDXCmp (SHORT iMatch, PVOID pElement)
   {
   IDX   *pidx;

   pidx = (IDX *)pElement;
   return max (-1, min (1, iMatch - ((SHORT)pidx->uId)));
   }


USHORT EXPORT ItiMbQueryQueryText (HMODULE hmod,
                                   USHORT  uResId,
                                   USHORT  uId,
                                   PSZ     psz,
                                   USHORT  uSize)
   {
   PVOID    pBuff;
   IDX      *pidx;
   PSZ      pszBase;
   SHORT    i;
   USHORT   uCount;

   *psz = '\0';
   /*--- load resource ---*/
   if (ItiMbGetResource (hmod, ITIRT_QUERY, uResId, uId, &pBuff))
      return MbErr ("Unable to load Query Resource",0, 1);

   if (!(uCount = *((USHORT *) pBuff)))
      {
      DosFreeResource (pBuff);
      return 2;
      }
   pidx    = (IDX *)((char *)pBuff + 2);
   pszBase = (char *)pBuff;

   /*--- find first entry in resource ---*/
   if ((i = FindFirst (pidx, uCount, sizeof (IDX), uId, IDXCmp)) == -1)
      {
      DosFreeResource (pBuff);
      return 2;
      }
   ItiStrCpy (psz, pszBase + pidx[i].uOffset, uSize);
   DosFreeResource (pBuff);
   return 0;
   }

/*************************************************************************/



USHORT EXPORT ItiMbQueryTitleText ( HMODULE hmod,
                                    USHORT  uResId,
                                    USHORT  uId,
                                    PSZ     psz,
                                    USHORT  uSize)
   {
   PVOID    pBuff;
   IDX      *pidx;
   PSZ      pszBase;
   SHORT    i;
   USHORT   uCount;

   /*--- load resource ---*/
   if (ItiMbGetResource (hmod, ITIRT_TITLE, uResId, uId, &pBuff))
      return MbErr ("Unable to load Title Resource",0, 1);

   *psz   = '\0';
   if (!(uCount = *((USHORT *) pBuff)))
      {
      DosFreeResource (pBuff);
      return 2;
      }
   pidx    = (IDX *)((char *)pBuff + 2);
   pszBase = (PSZ)((char *)pBuff);

   /*--- find first entry in resource ---*/
   if ((i = FindFirst (pidx, uCount, sizeof (IDX), uId, IDXCmp)) == -1)
      {
      DosFreeResource (pBuff);
      return 2;
      }
   ItiStrCpy (psz, pszBase + pidx[i].uOffset, uSize);
   DosFreeResource (pBuff);
   return 0;
   }


/*************************************************************************/


SHORT SMCmp (SHORT iMatch, PVOID pElement)
   {
   SM *psm;

   psm = (SM *)pElement;
   return max (-1, min (1, iMatch - ((SHORT)psm->uId)));
   }


USHORT EXPORT ItiMbQueryStaticMetrics ( HMODULE   hmod,
                                        USHORT uResId,
                                        USHORT uId,
                                        PSWMET pswm)
   {
   PVOID    pBuff;
   SM       *psm;
   SHORT    i;
   USHORT   uCount;

   /*--- load resource ---*/
   if (ItiMbGetResource (hmod, ITIRT_SWMET, uResId, uId, &pBuff))
      return MbErr ("Unable to load Static Met Resource",0, 1);

   if (!(uCount = *((USHORT *) pBuff)))
      {
      DosFreeResource (pBuff);
      return 2;
      }
   psm     = (SM *)((char *)pBuff + 2);

   /*--- find first entry in resource ---*/
   if ((i = FindFirst (psm, uCount, sizeof (SM), uId, SMCmp)) == -1)
      {
      DosFreeResource (pBuff);
      return MbErr ("Unable to locate uId= %u in Static Met Resource.", uId, 2);
      }

   pswm->rcl         = RECTStoL (psm[i].rcs);
   pswm->uAddId      = psm[i].uAddId;
   pswm->uChangeId   = psm[i].uChangeId;
   pswm->uCoordType  = psm[i].uCoordType;
   pswm->uOptions    = psm[i].uOptions;
   pswm->lBColor     = psm[i].lBColor;

   DosFreeResource (pBuff);
   return 0;
   }


/*************************************************************************/


SHORT LMCmp (SHORT iMatch, PVOID pElement)
   {
   LM  *plm;

   plm = (LM *)pElement;
   return max (-1, min (1, iMatch - ((SHORT)plm->uId)));
   }


USHORT EXPORT ItiMbQueryListMetrics (HMODULE    hmod,
                                     USHORT  uResId,
                                     USHORT  uId,
                                     PLWMET  plwm)
   {
   PVOID    pBuff;
   LM       *plm;
   SHORT    i;
   USHORT   uCount;

   /*--- load resource ---*/
   if (ItiMbGetResource (hmod, ITIRT_LWMET, uResId, uId, &pBuff))
      return MbErr ("Unable to load List Met Resource",0, 1);

   if (!(uCount = *((USHORT *) pBuff)))
      {
      DosFreeResource (pBuff);
      return 2;
      }
   plm    = (LM *)((char *)pBuff + 2);

   /*--- find first entry in resource ---*/
   if ((i = FindFirst (plm, uCount, sizeof (LM), uId, LMCmp)) == -1)
      {
      DosFreeResource (pBuff);
      return MbErr ("Unable to locate uId= %u in List Met Resource.", uId, 2);
      }

   plwm->uOptions    = plm[i].uOptions;
   plwm->lBColor     = plm[i].lBColor;
   plwm->lLBColor    = plm[i].lLBColor;
   plwm->uLabelYSize = plm[i].uLabelYSize;
   plwm->uDataYSize  = plm[i].uDataYSize;
   plwm->uButtonId   = plm[i].uButtonId;

   DosFreeResource (pBuff);
   return 0;
   }


/*************************************************************************/

SHORT DLGICmp (SHORT iMatch, PVOID pElement)
   {
   DLGI *pdlgi;

   pdlgi =(DLGI *)pElement;
   return max (-1, min (1, iMatch - ((SHORT)pdlgi->uId)));
   }


USHORT EXPORT ItiMbGetDialogInfo  ( HMODULE hmod,
                                    USHORT  uResId,
                                    USHORT  uId,
                                    HHEAP   hheap,
                                    PDGI    *ppDGI,
                                    PSZ     **pppszOutput,
                                    USHORT  *uNum)
   {
   PVOID    pBuff;
   PSZ      pStrBuff, pStr;
   DLGI     *pdlgi;
   SHORT    i;
   USHORT   uCount, uEntrys;

   *ppDGI = NULL;
   *pppszOutput = NULL;
   /*--- load resources ---*/
   if (ItiMbGetResource (hmod, ITIRT_DIALOG, uResId, uId, &pBuff))
      return MbErr ("Unable to load Dialog Resource",0, 1);
   if (ItiMbGetResource (hmod, ITIRT_DLGFMAT, uResId, uId, &pStrBuff))
      return MbErr ("Unable to load Dialog Format Resource",0, 1);

   *uNum  = 0;
   if (!(uCount = *((USHORT *) pBuff)))
      {
      DosFreeResource (pBuff);
      return 2;
      }
   pdlgi = (DLGI *)((char *)pBuff + 2);

   /*--- find first entry in resource ---*/
   if ((i = FindFirst (pdlgi, uCount, sizeof (DLGI), uId, DLGICmp)) != -1)
      {
      /*--- get number of entrys in resource ---*/
      for (uEntrys=0; (USHORT)i + uEntrys < uCount && pdlgi[i+uEntrys].uId==uId; uEntrys++)
         ;
      *uNum = uEntrys;

      if ((*ppDGI = ItiMemAlloc (hheap, uEntrys * sizeof (DGI), 0)) == NULL)
         return MbErr ("Unable to alloc memory for Data Info", 0, 3);
      if ((*pppszOutput = ItiMemAlloc (hheap, uEntrys * sizeof (PSZ), 0)) == NULL)
         return MbErr ("Unable to alloc memory for Output Format Strings", 0, 3);

      while (uEntrys--)
         {
         (*ppDGI)[uEntrys].uAddQuery     = pdlgi[i+uEntrys].uAddQuery;
         (*ppDGI)[uEntrys].uChangeQuery  = pdlgi[i+uEntrys].uChangeQuery;
         (*ppDGI)[uEntrys].uColId  = pdlgi[i+uEntrys].uColId ;
         (*ppDGI)[uEntrys].uCtlId  = pdlgi[i+uEntrys].uCtlId ;
         (*ppDGI)[uEntrys].uOffset = pdlgi[i+uEntrys].uOffset;

         pStr = &(pStrBuff[pdlgi[i+uEntrys].uOffset]);
         (*pppszOutput)[uEntrys] = ItiStrDup (hheap, pStr);

         pStr += ItiStrLen (pStr) + 1;
         (*ppDGI)[uEntrys].pszInputFmt = ItiStrDup (hheap, pStr);
         }
      }

   DosFreeResource (pBuff);
   DosFreeResource (pStrBuff);
   return 0;
   }



/*************************************************************************/


SHORT IDX2Cmp (SHORT iMatch, PVOID pElement)
   {
   IDX2   *pidx2;

   pidx2 = (IDX2 *)pElement;
   return max (-1, min (1, iMatch - ((SHORT)pidx2->uId)));
   }



USHORT EXPORT ItiMbGetFormatText ( HMODULE hmod,
                                   USHORT  uResId,
                                   USHORT  uId,
                                   HHEAP   hheap,
                                   PSZ     **pppsz,
                                   USHORT  *puNum)
   {
   PVOID    pBuff;
   IDX2     *pidx2;
   PSZ      pszBase;
   PSZ      pszStrPtr;
   SHORT    i;
   USHORT   uCount;

   *pppsz = NULL;
   *puNum = 0;
   /*--- load resource ---*/
   if (ItiMbGetResource (hmod, ITIRT_FORMAT, uResId, uId, &pBuff))
      return MbErr ("Unable to load Format Resource",0, 1);

   if (!(uCount = *((USHORT *) pBuff)))
      {
      DosFreeResource (pBuff);
      return 0;
      }

   pidx2   = (IDX2 *)((char *)pBuff + 2);
   pszBase = (PSZ)((char *)pBuff);

   /*--- find first entry in resource ---*/
   if ((i = FindFirst (pidx2, uCount, sizeof (IDX2), uId, IDX2Cmp)) == -1)
      {
      DosFreeResource (pBuff);
      return 0;
      }

   *puNum = pidx2[i].uCount;
   pszStrPtr = pszBase + pidx2[i].uOffset;

   if ((*pppsz = ItiMemAlloc (hheap, *puNum * sizeof (PSZ), 0)) == NULL)
      return MbErr ("Unable to alloc memory for Format Data", 0, 3);
   for (i = 0; (USHORT)i < *puNum; i++)
      {
      (*pppsz)[i] = ItiStrDup (hheap, pszStrPtr);
      pszStrPtr += strlen (pszStrPtr) + 1;
      }
   
   DosFreeResource (pBuff);
   return 0;
   }




/*************************************************************************/

void EXPORT ItiMbFreeCHD (HHEAP hheap, PCHD pCHD)
   {
   if (pCHD != NULL)
      ItiMemFree (hheap, pCHD);
   }


void EXPORT ItiMbFreeEDT (HHEAP hheap, PEDT pEDT)
   {
   if (pEDT != NULL)
      ItiMemFree (hheap, pEDT);
   }








