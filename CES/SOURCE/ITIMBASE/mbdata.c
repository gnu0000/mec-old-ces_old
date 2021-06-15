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
 * mbdata.c                                                             *
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

#define  INCL_WIN
#include "..\include\iti.h"
#include "..\include\itimbase.h"
#include "mbdata.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct
   {
   USHORT   uId;
   PFNWP    pfn;
   HMODULE  hmod;
   }  STATICDEF;

typedef struct
   {
   USHORT   uId;
   PFNTXT   pfn;
   HMODULE  hmod;
   }  LISTDEF;

typedef struct
   {
   USHORT   uDlgId;
   USHORT   uId;
   PFNWP    pfn;
   HMODULE  hmod;
   }  DIALOGDEF;


STATICDEF   asd [MAXSTATICWINDOWS];
USHORT      uStaticWindows = 0;

LISTDEF     ald [MAXLISTWINDOWS];
USHORT      uListWindows = 0;

DIALOGDEF   adg [MAXDIALOGWINDOWS];
USHORT      uDialogWindows = 0;







/* --- used by bsearch for statics --- */
int _FAR_ cdecl sdCmp (void _FAR_ *puId, void _FAR_ *psd)
   {
   return *((int *)puId) - (int) (((STATICDEF *)psd)->uId);
   }

/* --- used by bsearch for lists --- */
int _FAR_ cdecl ldCmp (void _FAR_ *puId, void _FAR_ *pld)
   {
   return *((int *)puId) - (int) (((LISTDEF *)pld)->uId);
   }

/* --- used by bsearch for dialogs --- */
int _FAR_ cdecl dgCmp (void _FAR_ *puId, void _FAR_ *pdg)
   {
   return *((int *)puId) - (int) (((DIALOGDEF *)pdg)->uId);
   }





USHORT ErrDat (PSZ psz, USHORT u)
   {
   char  szTmp [255];

   sprintf (szTmp, "windata.c : %s%d", psz, u);
   WinMessageBox (HWND_DESKTOP, HWND_DESKTOP, szTmp,
                  "Window Data Error", MB_OK, MB_APPLMODAL);
   return 1;
   }





USHORT EXPORT ItiMbRegisterStaticWnd (USHORT  uId,
                                      PFNWP   pfn,
                                      HMODULE hmod)
   {
   USHORT    i;

   /*--- Add To Array In Order ---*/
   for (i = 0; i < uStaticWindows && uId > asd[i].uId; i++)
      ;

   if (uId != asd[i].uId || !uStaticWindows) /*check ifwe arenot overwriting old entry*/
      {
      if (uStaticWindows > i)
         memmove (asd+ i+ 1, asd+ i,
                   (uStaticWindows - i) * sizeof (STATICDEF));
      uStaticWindows++;
      }
   else
      { /* --- warn them that they are re-registering an id --- */
      ErrDat ("Re-Registering Static Window Id id:", uId);
      }

   asd[i].uId    = uId;
   asd[i].pfn    = pfn;
   asd[i].hmod   = hmod;

   if (uStaticWindows > MAXSTATICWINDOWS)
      return ErrDat ("More Static Windows Registered than array Space Id:", uId);
   return 0;
   }




                        
USHORT EXPORT ItiMbRegisterListWnd (USHORT      uId,
                                    PFNTXT      pfn,
                                    HMODULE     hmod)
   {
   USHORT    i;

   /*--- Add To Array In Order ---*/
   for (i = 0; i < uListWindows && uId > ald[i].uId; i++)
      ;

   if (uId != ald[i].uId || !uListWindows) /* check if we are not overwriting old entry */
      {
      if (uListWindows > i)
         memmove (ald+ i+ 1, ald+ i,
                   (uListWindows - i) * sizeof (LISTDEF));
      uListWindows++;
      }
   else
      { /* --- warn them that they are re-registering an id --- */
      ErrDat ("Re-Registering List Window Id id:", uId);
      }

   ald[i].uId     = uId;
   ald[i].pfn     = pfn;
   ald[i].hmod    = hmod;

   if (uListWindows > MAXLISTWINDOWS)
      return ErrDat ("More List Windows Registered than array Space Id: ", uId);
   return 0;
   }



USHORT EXPORT ItiMbRegisterDialogWnd (USHORT  uId,
                                      USHORT  uDlgId,
                                      PFNWP   pfn,
                                      HMODULE hmod)
   {
   USHORT    i;

   /*--- Add To Array In Order ---*/
   for (i = 0; i < uDialogWindows && uId > adg[i].uId; i++)
      ;

   if (uId != adg[i].uId || !uDialogWindows) /*check ifwe arenot overwriting old entry*/
      {
      if (uDialogWindows > i)
         memmove (adg+ i+ 1, adg+ i,
                   (uDialogWindows - i) * sizeof (DIALOGDEF));
      uDialogWindows++;
      }
   else
      { /* --- warn them that they are re-registering an id --- */
      ErrDat ("Re-Registering Dialog Window Id id:", uId);
      }

   adg[i].uId    = uId;
   adg[i].uDlgId = uDlgId;
   adg[i].pfn    = pfn;
   adg[i].hmod   = hmod;

   if (uStaticWindows > MAXDIALOGWINDOWS)
      return ErrDat ("More Dialog Windows Registered than array Space Id:", uId);
   return 0;
   }










/*
 * This procedure returns a pfn for the 
 * static window uId.  If uId does not have a matching proc,
 * the default is returned.
 */
PFNWP EXPORT ItiMbStaticProc (USHORT uId, HMODULE *phmod)
   {
   STATICDEF   *psd;

   if (!uStaticWindows)
      {
      ErrDat ("No Static Windows Registered uId:", uId);
      *phmod = 0;
      return NULL;
      }

   if ((psd = bsearch (&uId, asd, uStaticWindows, sizeof (STATICDEF), sdCmp)) == NULL)
      {
      *phmod = asd[0].hmod;
      return asd[0].pfn;
      }
   *phmod = psd->hmod;
   return  psd->pfn;
   }








/*
 * This procedure returns a pfn for the 
 * list window edit proc.  If uId does not have a matching proc,
 * the default is returned.
 */
PFNTXT EXPORT ItiMbListEditProc (USHORT uId, HMODULE *phmod)
   {
   LISTDEF   *pld;

   if (!uListWindows)
      {
      ErrDat ("No List Windows Registered. uId:", uId);
      *phmod = 0;
      return NULL;
      }

   if ((pld = bsearch (&uId, ald, uListWindows, sizeof (LISTDEF), ldCmp)) == NULL)
      {
      *phmod =ald[0].hmod;
      return ald[0].pfn;
      }
   *phmod = pld->hmod;
   return  pld->pfn;
   }





/*
 * This procedure returns a pfn for the 
 * static window uId.  If uId does not have a matching proc,
 * the default is returned.
 */
PFNWP EXPORT ItiMbDialogProc (USHORT uId, USHORT *uDlgId, HMODULE *phmod)
   {
   DIALOGDEF   *pdg;

   if (!uDialogWindows)
      {
      ErrDat ("No Dialog Windows Registered uId:", uId);
      *phmod = 0;
      return NULL;
      }

   if ((pdg = bsearch (&uId, adg, uDialogWindows, sizeof (DIALOGDEF), dgCmp)) == NULL)
      {
      *uDlgId = adg[0].uDlgId;
      *phmod  = adg[0].hmod;
      return adg[0].pfn;
      }
   *uDlgId = pdg->uDlgId;
   *phmod  = pdg->hmod;
   return  pdg->pfn;
   }






/* finds the hmod for the window list or static or dialog
 * returns 0 if it finds the appropriate entry
 * returns 1 if it has to return the default.
 * the def hmod is the one for static windows.
 */
USHORT EXPORT ItiMbQueryHMOD (USHORT uId, HMODULE *phmod)
   {
   LISTDEF     *pld;
   STATICDEF   *psd;
   DIALOGDEF   *pdg;

   if (uListWindows   && (pld = bsearch (&uId, ald, uListWindows,   sizeof (LISTDEF),   ldCmp)) != NULL)
      {
      *phmod = pld->hmod;
      return 0;
      }
   if (uStaticWindows && (psd = bsearch (&uId, asd, uStaticWindows, sizeof (STATICDEF), sdCmp)) != NULL)
      {
      *phmod = psd->hmod;
      return 0;
      }
   if (uDialogWindows && (pdg = bsearch (&uId, adg, uDialogWindows, sizeof (DIALOGDEF), dgCmp)) != NULL)
      {
      *phmod = pdg->hmod;
      return 0;
      }
   *phmod = asd[0].hmod;
   return 1;
   }


/* return values:  0 - window id not found
 *                 1 - static window
 *                 2 - list window
 *                 3 - dialog window
 */
USHORT EXPORT ItiMbQueryWindowType (USHORT uId)
   {
   LISTDEF     *pld;
   STATICDEF   *psd;
   DIALOGDEF   *pdg;

   if (uStaticWindows && (psd = bsearch (&uId, asd, uStaticWindows, sizeof (STATICDEF), sdCmp)) != NULL)
      return ITI_STATIC;
   if (uListWindows   && (pld = bsearch (&uId, ald, uListWindows,   sizeof (LISTDEF),   ldCmp)) != NULL)
      return ITI_LIST;
   if (uDialogWindows && (pdg = bsearch (&uId, adg, uDialogWindows, sizeof (DIALOGDEF), dgCmp)) != NULL)
      return ITI_DIALOG;
   return 0;
   }
