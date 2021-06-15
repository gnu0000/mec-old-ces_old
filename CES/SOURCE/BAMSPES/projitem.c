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


#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itimenu.h"
#include "..\include\itibase.h"
#include "..\include\itidbase.h"
#include "..\include\itimbase.h"
#include "..\include\itifmt.h"
#include "..\include\itiutil.h"
#include "..\include\winid.h"
#include "..\include\colid.h"
#include "..\include\metadecl.h"
#include "..\include\dialog.h"

#include "bamspes.h"
#include "jobproj.h"
#include "menu.h"
#include "dialog.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fred.h"

#include "..\itiwin\winmain.h"

PFNWP pfnVSBProc;

typedef struct
   {
   HWND   hwndL1;       /* hwnd on left                        */
   HWND   hwndL2;       /* hwnd on right                       */
   HWND   hwndL2V;      /* hwnd of vert scroll on right wnd    */
   HHEAP  hheap;        /* static hheap                        */
   USHORT uCurrRow;     /* current scroll pos                  */
   USHORT uColCount;    /* count of cols in L2                 */
   USHORT uDisplay;     /* displayed col (chosen in combobox)  */
   HBUF   *phbuf;       /* ptr to hbuf array for L2            */
   BOOL   *pbDone;      /* flag telling if buffers are done    */
// HBUF   hbufL1;       /* hbuf for L1, put here for speed     */
   PSZ    *ppszLabels;  /* L2 labels put here for convenience  */
                        /* even: PCN odd: category             */  
   } PROJDAT;
typedef PROJDAT *PPROJDAT;


char TxtStr [255];




/*******************************************************************/
/*             Text procedures for the list windows                */
/*                 and subclassed window procs                     */
/*******************************************************************/



int EXPORT ItiWndTestL2TxtProc (HWND   hwnd,
                                USHORT uRow,
                                USHORT uCol,
                                PSZ    *pszTxt)
   {
   PPROJDAT pproj;
   HWND     hwndListFrame, hwndStatic;
   USHORT   uBuff;

   hwndListFrame = WinQueryWindow (hwnd, QW_PARENT, 0);
   hwndStatic    = WinQueryWindow (hwndListFrame, QW_PARENT, 0);
   pproj         = ItiWndGetExtra (hwndStatic, ITI_STATIC);

   uBuff         = min (uCol, pproj->uColCount - 1);
   uCol          = 0x8001 + pproj->uDisplay;

   ItiDbGetBufferRowCol (pproj->phbuf[uBuff], uRow, uCol, TxtStr);
   *pszTxt = TxtStr;

   return 0;
   }



int EXPORT ItiWndTestL1TxtProc (HWND   hwnd,
                                USHORT uRow,
                                USHORT uCol,
                                PSZ    *pszTxt)
   {
   PPROJDAT pproj;
   HWND     hwndListFrame, hwndStatic;
   HBUF     hbuf;

   hwndListFrame = WinQueryWindow (hwnd, QW_PARENT, 0);
   hwndStatic    = WinQueryWindow (hwndListFrame, QW_PARENT, 0);
   pproj = ItiWndGetExtra (hwndStatic, ITI_STATIC);

   uRow += pproj->uCurrRow;

   if (uCol == 1 && pproj->uDisplay == 2)
      strcpy (TxtStr, "100.00%");
   else
      {
      uCol = 0x8001 + !!(uCol) * (1 + pproj->uDisplay);

      hbuf = (HBUF) QW (hwndListFrame, ITIWND_BUFFER, 0, 0, 0);

      ItiDbGetBufferRowCol (hbuf, uRow, uCol, TxtStr);
      }

   *pszTxt = TxtStr;
   return 0;
   }



WNDPROC EXPORT L2VSBProc (HWND hwnd, USHORT uMsg,
                          MPARAM mp1, MPARAM mp2)
   {
   switch (uMsg)
      {
      case SBM_SETPOS:
         {
         PPROJDAT pproj;
         HWND     hwndListFrame, hwndStatic;

         hwndListFrame = WinQueryWindow (hwnd, QW_PARENT, 0);
         hwndStatic    = WinQueryWindow (hwndListFrame, QW_PARENT, 0);
         pproj = ItiWndGetExtra (hwndStatic, ITI_STATIC);

         pproj->uCurrRow = SHORT1FROMMP (mp1);
         WinInvalidateRect (pproj->hwndL1, NULL, TRUE);
         }
      }
   return pfnVSBProc (hwnd, uMsg, mp1, mp2);
   }





/*******************************************************************/
/*                Window initialization functions                  */
/*******************************************************************/



USHORT KickItAgain (PPROJDAT pproj)
   {
   LWINIT     lwInit;
   PEDT       pedtData,
              pedtLabel;
   HHEAP      hheapL2;
   USHORT     uColWidth, i;
   PVOID      pTmpa, pTmpb, pTmpc;

   if (!pproj->uColCount)
      return 0;

   hheapL2 = (HHEAP) QW (pproj->hwndL2, ITIWND_HEAP, 0, 0, 0);

   /*--- Build Label MetaData ---*/
   /*
    * There are two labels per column
    * the first one is the project
    * the second is the catagory
    */
   pedtLabel  = (PEDT) ItiMemAlloc (hheapL2,
                                    sizeof (EDT) * pproj->uColCount * 2,
                                    0);

   /*--- Decide on a col width ---*/
   pTmpa = QW (pproj->hwndL2, ITIWND_DATAEDT, 0, 0, 0);
   uColWidth = (USHORT)((PEDT)pTmpa)->rcl.xRight;

   pTmpa = QW (pproj->hwndL2, ITIWND_LABELEDT, 0, 0, 0);
   pTmpb = QW (pproj->hwndL2, ITIWND_LABELEDT, 1, 0, 0);

   for (i=0; i<pproj->uColCount; i++)
      {
      pedtLabel[i*2] = *(PEDT)pTmpa;
      pedtLabel[i*2].uIndex = i*2;
      pedtLabel[i*2].rcl.xLeft += (i * uColWidth);

      pedtLabel[i*2+1] = *(PEDT)pTmpb;
      pedtLabel[i*2+1].uIndex = i*2+1;
      pedtLabel[i*2+1].rcl.xLeft += (i * uColWidth);
      }

   /*--- Build Data MetaData ---*/
   /* There is only one of these per column
    * plus 2 kludge columns at the end to provide formats
    */
   pedtData = (PEDT) ItiMemAlloc (hheapL2, sizeof (EDT) * (pproj->uColCount +2), 0);
   pTmpa = QW (pproj->hwndL2, ITIWND_DATAEDT, 0, 0, 0);
   pTmpb = QW (pproj->hwndL2, ITIWND_DATAEDT, 1, 0, 0);
   pTmpc = QW (pproj->hwndL2, ITIWND_DATAEDT, 2, 0, 0);
   for (i=0; i<pproj->uColCount; i++)
      {
      pedtData[i] = *(PEDT)pTmpa;
      pedtData[i].rcl.xLeft += (i * uColWidth);
      }
   pedtData[i] = *(PEDT)pTmpb;
   pedtData[i+1] = *(PEDT)pTmpc;

   lwInit.uDataRows   = 0;
   lwInit.uDataYSize  = 0;
   lwInit.uLabelYSize = 0;
   lwInit.uDataCols   = pproj->uColCount + 2;
   lwInit.uLabelCols  = pproj->uColCount * 2;
   lwInit.uXScrollInc = uColWidth * 2;
   lwInit.uOptions    = ITI_LISTOPTIONHVQ | LWS_BORDER;

   if (LWInit (pproj->hwndL2,        // handle to window to initialize
               0,                    // module containing metadata for lw
               &lwInit,              // lwinit struct
               pedtData,             // ptr to array to data element descriptors
               pedtLabel,            // ptr to array to label element descriptors
               pproj->ppszLabels,    // ptr to array of label strings
               ItiWndTestL2TxtProc)) // ptr to get text proc
      return 1; /* WndErr ("Unable To init List Window Id:", uId);*/

   /*--- free data because it is copied ---*/
   ItiMemFree (hheapL2, pedtLabel);
   ItiMemFree (hheapL2, pedtData);
// ItiFreeStrArray (hheapL2, pproj->ppszLabels, uCols);

   SWL (pproj->hwndL2, ITIWND_COLOR, LWC_CLIENT, CLR_PALEGRAY); 
   SWL (pproj->hwndL2, ITIWND_COLOR, LWC_LABEL,  ITI_LLABELBACKGROUND);
//done in setcolor WinInvalidateRect (pproj->hwndL2, NULL, TRUE);
   }






/*
 * This function performs the following tasks
 *
 * 1> exec a blocking query to obtain all the labels in L2 (and col count)
 * 3> exec n buffer querys to get data for L2
 *
 * (we'll see)
 *
 * if -1 is returned, the window should go away (no categories)
 */
USHORT InitBuffers (HWND hwnd, PPROJDAT pproj)
   {
   PSZ      pszJobKey, *ppsz;
   char     szTmpQ [1024];
   USHORT   i, uCols, uState, uNumRows;
   HQRY     hqry;
   HWND     hwndOwner;

   hwndOwner = (HWND) QW (hwnd, ITIWND_OWNERWND, 0, 0, 0);
   pszJobKey = (PSZ) QW (hwndOwner, ITIWND_DATA, 0, 0, cJobKey);
   pproj->uColCount = 0;


   /*
    * 1st Get the labels and column count
    * for L2
    *
    * NEED: pszJobKey
    * GET:  uColCount, ppszLabels
    */
//   sprintf (szTmpQ, " SELECT DISTINCT ProjectControlNumber, Category"
//                     " FROM JobItemProjectItem WHERE JobKey=%s", pszJobKey);

   sprintf (szTmpQ, " SELECT PC.ProjectControlNumber, PC.Category"
                    " FROM JobProject JP, ProjectCategory PC"
                    " WHERE JP.JobKey=%s"
                    " AND JP.ProjectControlNumber = PC.ProjectControlNumber", pszJobKey);

   if (!(hqry = ItiDbExecQuery (szTmpQ, pproj->hheap, 0, 0, 0, &uCols, &uState)))
      return 0xffff;

   while (ItiDbGetQueryRow (hqry, &ppsz, &uState))
      {
      pproj->uColCount++;
      pproj->ppszLabels = ItiMemRealloc (pproj->hheap, pproj->ppszLabels,
                                  pproj->uColCount * 2 * sizeof (PSZ), 0);
      (pproj->ppszLabels)[(pproj->uColCount-1) * 2]     = ppsz[0];
      (pproj->ppszLabels)[(pproj->uColCount-1) * 2 + 1] = ppsz[1];
      ItiMemFree (pproj->hheap, ppsz);
      }

   if (! pproj->uColCount)
      return 0xffff;

  /*
   *
   *
   * NEW !
   *
   * and improved!
   *
   * Now with more computing power!
   *
   * And a fresh pine scent!
   *
   */
   MapJobItemProjItem (pszJobKey);


   KickItAgain (pproj);

   /*
    * Startup the n buffers required for L2 data area
    */
    pproj->phbuf = (HBUF *) ItiMemAlloc (pproj->hheap,
                                         pproj->uColCount * sizeof (HBUF),
                                         0);

    pproj->pbDone = (BOOL *) ItiMemAlloc (pproj->hheap,
                                         (1 + pproj->uColCount) * sizeof (BOOL),
                                         MEM_ZERO_INIT);
    for (i=0; i< pproj->uColCount; i++)
      {
      sprintf (szTmpQ,
               " SELECT PI.Quantity, PI.ExtendedAmount,"
               " Percentage = PI.Quantity/JI.Quantity" 
               " FROM ProjectItem PI, JobItemProjectItem JIPI, JobItem JI,"
               " StandardItem SI" 
               " WHERE JIPI.ProjectControlNumber = PI.ProjectControlNumber"  
               " AND JIPI.ProjectControlNumber = '%s'" 
               " AND JIPI.Category = '%s'" 
               " AND JIPI.JobKey = %s"
               " AND JIPI.JobItemKey = JI.JobItemKey"  
               " AND SI.StandardItemKey = JI.StandardItemKey"  
               " AND JIPI.Category = ProjectItem.Category"  
               " AND JIPI.SequenceNumber = PI.SequenceNumber"
               " AND JIPI.JobKey = JI.JobKey" 
               " ORDER BY SI.StandardItemNumber, JI.JobItemKey",
               (pproj->ppszLabels)[i * 2],
               (pproj->ppszLabels)[i * 2 + 1],
               pszJobKey);
      /*
       * attach the buffers to the static so that it will receive
       * the messages, tally them and pass them on to L2
       */
      pproj->phbuf[i] = ItiDbCreateBuffer (hwnd, ProjectItemL2,
                                           ITI_LIST, szTmpQ, &uNumRows);

      /*--- make the window happy by letting it think it has a ---*/
      /*--- buffer for its very own                            ---*/
      if (i ==  pproj->uColCount-1)
         WinSendMsg (pproj->hwndL2, WM_ITIWNDSET,
                     (MPARAM)ITIWND_BUFFER, (MPARAM)pproj->phbuf[i]);


      pproj->pbDone[i] = FALSE;
      }
   return 0;
   }



/*******************************************************************/
/*                   Windows Static Proc                           */
/*******************************************************************/


BOOL AllSame (PPROJDAT pproj, BOOL bVal)
   {
   USHORT i;

   for (i=0; i<pproj->uColCount; i++)
      if (pproj->pbDone[i] != bVal)
         return FALSE;
   return TRUE;
   }



BOOL L2Message (HWND hwnd, USHORT uMsg, MPARAM mp1, MPARAM mp2)
   {
   PPROJDAT pproj;
   USHORT   i;

   pproj = ItiWndGetExtra (hwnd, ITI_STATIC);

   for (i=0; i<pproj->uColCount; i++)
      {
      if (PVOIDFROMMP (mp2) != pproj->phbuf[i])
         continue;

      switch (uMsg)
         {
         case WM_ITIDBAPPENDEDROWS:
         case WM_ITIDBDELETE:
         case WM_ITIDBGRAY:
            return TRUE;

         case WM_ITIDBBUFFERDONE:
            pproj->pbDone[i] = TRUE;
            if (AllSame (pproj, TRUE))
               {
               WinSendMsg (pproj->hwndL2, uMsg, mp1, mp2);
               return TRUE;
               }
            return TRUE;

         case WM_ITIDBCHANGED:     
            pproj->pbDone[i] = FALSE;
            if (AllSame (pproj, FALSE))
               {
               WinSendMsg (pproj->hwndL2, uMsg, mp1, mp2);
               return TRUE;
               }
            return TRUE;
         }
      }
   return FALSE;
   }








CHAR *ppszViews[] =     {"Quantity",
                         "Ext. Amount",
                         "Percentage"};


MRESULT EXPORT ProjectItemsSProc (HWND   hwnd,
                                  USHORT uMsg,
                                  MPARAM mp1,
                                  MPARAM mp2)
   {
   switch (uMsg)
      {
      case WM_INITQUERY:
         {
         PPROJDAT pproj;
         HHEAP    hheap;
         USHORT   i;

         hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
         pproj = (PPROJDAT) ItiMemAlloc (hheap, sizeof (PROJDAT), 0);

         ItiWndSetExtra (hwnd, ITI_STATIC, pproj);

         pproj->hwndL1   = WinWindowFromID (hwnd, ProjectItemL1);
         pproj->hwndL2   = WinWindowFromID (hwnd, ProjectItemL2);
         pproj->hheap    = hheap;
         pproj->uCurrRow = 0;
         pproj->uDisplay = 0;
         pproj->uColCount= 0;
         pproj->ppszLabels = NULL;
         pproj->phbuf = NULL;


         if (InitBuffers (hwnd, pproj) == 0xFFFF)
            {
            WinMessageBox (HWND_DESKTOP, hwnd,
                           "This window cannot be displayed until at "
                           "least 1 project has at least one category",
                           "Project Data Incomplete Warning",
                           666, MB_ICONHAND | MB_MOVEABLE | MB_OK);
            WinPostMsg (hwnd, WM_CLOSE, 0, 0);
            return 0;
            }

         pproj->hwndL2V = WinWindowFromID (pproj->hwndL2, 0x0008);

         WinSendMsg (pproj->hwndL1, WM_ITIWNDSET,
                     MPFROMSHORT (ITIWND_TXTPROC),
                     (MPARAM) (PVOID)ItiWndTestL1TxtProc);

         pfnVSBProc = (PFNWP) WinQueryWindowPtr (pproj->hwndL2V, QWP_PFNWP);
         WinSetWindowPtr (pproj->hwndL2V, QWP_PFNWP, (PVOID)L2VSBProc);


         /*--- Fill Combobox ---*/
         for (i=0; i < sizeof ppszViews / sizeof (PSZ); i++)
            WinSendDlgItemMsg (hwnd, ProjectItemC, LM_INSERTITEM,
                                  (MPARAM)LIT_END, MPFROMP (ppszViews[i]));
         WinSendDlgItemMsg (hwnd, ProjectItemC, LM_SELECTITEM, (MPARAM)0, MPFROMP (TRUE));

         break;
         }


      case WM_ITIDBAPPENDEDROWS:  // Traps hbuf messages from:
      case WM_ITIDBBUFFERDONE:    //    Static Query hbuf &
      case WM_ITIDBCHANGED:       //    L2 Buffers
      case WM_ITIDBDELETE:
      case WM_ITIDBGRAY:  
         {
         /*--- if a list message, take care of it now ---*/
         if (L2Message (hwnd, uMsg, mp1, mp2))
            return 0;

         /*--- else a static message, so pass it along ---*/
         break;
         }


      /*--- This message will come from the L1 window  ---*/
      case WM_CHILDQUERYDONE:
         {
         PPROJDAT pproj;

         pproj = ItiWndGetExtra (hwnd, ITI_STATIC);
         pproj->pbDone[pproj->uColCount] = TRUE;



         /*
          *  Please do not ask what this does
          *
          *
          *
          *
          *                    Code By SuperKludge (tm) 12/17/92
          */
            {
            HWND     hwndClient;
            SWP      swp;
            ULONG    ulStyle;

            hwndClient = WinWindowFromID (pproj->hwndL1, 11);

            ulStyle = WinQueryWindowULong (pproj->hwndL1, QWL_STYLE);
            WinSetWindowULong (pproj->hwndL1, QWL_STYLE, ulStyle & ~WS_CLIPSIBLINGS);

            ulStyle = WinQueryWindowULong (hwndClient, QWL_STYLE);
            WinSetWindowULong (hwndClient, QWL_STYLE, ulStyle & ~WS_CLIPSIBLINGS);

            WinQueryWindowPos (pproj->hwndL1, &swp);
            WinSetWindowPos (pproj->hwndL1, HWND_TOP, swp.x, swp.y, swp.cx, swp.cy,
                           SWP_ACTIVATE | SWP_FOCUSACTIVATE | SWP_MOVE | SWP_SIZE | SWP_ZORDER);

            WinQueryWindowPos (hwndClient, &swp);
            WinSetWindowPos (hwndClient, HWND_TOP, swp.x, swp.y, swp.cx, swp.cy,
                           SWP_ACTIVATE | SWP_FOCUSACTIVATE | SWP_MOVE | SWP_SIZE | SWP_ZORDER);

            WinInvalidateRect (pproj->hwndL1, NULL, TRUE);
            WinUpdateWindow (pproj->hwndL1);
            WinInvalidateRect (hwndClient, NULL, TRUE);
            WinUpdateWindow (hwndClient);
            }

         break;
         }


      case WM_ACTIVEROWCHANGED:
         {
         PPROJDAT pproj;
         HWND     hwndSet;
         USHORT   uRow;

         pproj = ItiWndGetExtra (hwnd, ITI_STATIC);

         hwndSet = (mp2 == pproj->hwndL1 ? pproj->hwndL2 : pproj->hwndL1);
         uRow    =  SHORT1FROMMP (mp1) + (mp2 == pproj->hwndL1 ? pproj->uCurrRow : - pproj->uCurrRow); 
         WinSendMsg (hwndSet, WM_ITIWNDSET, (MPARAM)ITIWND_ACTIVE, (MPARAM)uRow);
         return 0;
         }


      case WM_CONTROL:
         {
         PPROJDAT pproj;
         USHORT   i, uCmd;

         uCmd = SHORT2FROMMP (mp1);

         switch (SHORT1FROMMP (mp1))
            {
            case ProjectItemC:
               if (uCmd != CBN_LBSELECT)
                  break;

               pproj = ItiWndGetExtra (hwnd, ITI_STATIC);

               i = (UM) WinSendDlgItemMsg (hwnd, ProjectItemC, LM_QUERYSELECTION, 0, 0);
               if (i == LIT_NONE || i == pproj->uDisplay)
                  return 0;

               pproj->uDisplay = i;
               WinInvalidateRect (pproj->hwndL1, NULL, TRUE);
               WinInvalidateRect (pproj->hwndL2, NULL, TRUE);
               break;


            case ProjectItemL2:
               {
               USHORT uStartRow, uActiveRow;

               if (uCmd != LN_SCROLL || SHORT1FROMMP (mp2) != 0x0008)
                  break;

               pproj = ItiWndGetExtra (hwnd, ITI_STATIC);

               uStartRow = SHORT2FROMMP (mp2);
               uActiveRow= (UM) WinSendMsg (pproj->hwndL2, WM_ITIWNDQUERY,
                                       (MPARAM)ITIWND_ACTIVE, 0);
               WinSendMsg (pproj->hwndL1,
                           WM_ITIWNDSET,
                           (MPARAM)ITIWND_ACTIVE,
                           (MPARAM)(uActiveRow-uStartRow));
               break;
               }
            }
         break;
         }


      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_EDIT_MENU:
               {
               BOOL bSelected;

               bSelected = ItiWndQueryActive (hwnd, ProjectItemL2, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               return 0;
               }

            case IDM_VIEW_MENU:
               break;
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            HWND  hwndChild;

            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, ProjectItemL1, FALSE);
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, ProjectItemL1, TRUE);
               break;

            case (IDM_VIEW_USER_FIRST + 1):
               hwndChild = WinWindowFromID (hwnd, 0);
               ItiWndBuildWindow (hwndChild, 0);
               return 0;
            }
         break;

      case WM_DESTROY:
			{
         PPROJDAT pproj;
			USHORT   i;

         pproj = ItiWndGetExtra (hwnd, ITI_STATIC);

			/*--- We must manually delete the buffers we created ---*/
			/*--- and attached to the static window				  ---*/
		   for (i=1; i<pproj->uColCount; i++)
            ItiDbDeleteBuffer (pproj->phbuf[i-1], hwnd);
			break;
			}
      }
   return ItiWndDefStaticWndProc (hwnd, uMsg, mp1, mp2);
   }












/************************************************************************/
/*  Below lies the infamous JobItem/ProjItem Dialog Box Code... BEWARE  */
/************************************************************************/


typedef struct
   {
   BOOL   bInit;        // TRUE after it is initialized 
   HWND   hwndDL;       // scroll bar window
   HWND   hwndV;        // scroll bar window
   HBUF   hbuf;         // list window buffer
   USHORT uRows;        // rows in list window
   USHORT uDisplay;     // displayed col (chosen in combobox)
   DOUBLE dUnitPrice;   // unit price of job item
   DOUBLE dJobQuantity; // unit price of job item
   DOUBLE *pdQuantity;  // quantity of each proj item
   } ROWDAT;
typedef ROWDAT *PROWDAT;


/*
 * converts a string containing a uType  (0=Number/1=Money/2=Pct)
 * to a float
 */
static DOUBLE ToFloat (PSZ pszStr, USHORT uType)
   {
   DOUBLE dResult;

   ItiStrip (pszStr, "$,%");
   dResult = ItiStrToDouble (pszStr);
   if (uType == 2)
      dResult /= 100;

   return dResult;
   }

/*
 * converts a float to a string of type uType (0=Number/1=Money/2=Pct)
 *
 */
static void  FloatTo (PSZ pszStr, DOUBLE dnum, USHORT uType)
   {
   char  szTmp [40];
   PSZ   pszFormat;

   pszFormat = (uType? (uType==1? "Number,$,..": "Number,%.."): "Number,...");
   sprintf (szTmp, "%lf", dnum);
   ItiFmtFormatString (szTmp, pszStr, 40, pszFormat, NULL);
   }



/*
 * This inits the structure
 * of display data
 */
static USHORT InitEdits (HWND hwnd, PROWDAT prd)
   {
   HBUF     hbufD;
   HHEAP    hheap;
   char     pszTmp [40];
   USHORT   i;

   hbufD = (HBUF)WinSendMsg (hwnd, WM_ITIWNDQUERY, (MPARAM) ITIWND_BUFFER, 0);
   hheap = (HHEAP)WinSendMsg (hwnd, WM_ITIWNDQUERY, (MPARAM) ITIWND_HEAP, 0);

   prd->hwndDL = WinWindowFromID (hwnd,   ProjectItemDL);
   prd->hbuf = WinSendMsg (prd->hwndDL, WM_ITIWNDQUERY, (MPARAM) ITIWND_BUFFER, 0);
   prd->uRows = (UM)ItiDbQueryBuffer (prd->hbuf, ITIDB_NUMROWS, 0);
   prd->uDisplay = 0;

   ItiDbGetBufferRowCol (hbufD, 0, cUnitPrice, pszTmp);
   prd->dUnitPrice = ToFloat (pszTmp, 1);

   ItiDbGetBufferRowCol (hbufD, 0, cQuantity, pszTmp);
   prd->dJobQuantity = ToFloat (pszTmp, 0);

   prd->pdQuantity = ItiMemAlloc (hheap, sizeof (DOUBLE) * prd->uRows, 0);
   for (i=0; i<prd->uRows; i++)
      {
      ItiDbGetBufferRowCol (prd->hbuf, i, cQuantity, pszTmp);
      prd->pdQuantity[i] = ToFloat (pszTmp, 0);
      }

   prd->bInit = TRUE;
   return 0;
   }



/*
 * This proc converts a quantity float into a 
 * quantity string, ext amount string, or a percentage string
 * based on the setting of the combo box
 */
static void MakeDisplayStr (PSZ pszResult, DOUBLE dQuan, PROWDAT prd)
   {
   switch (prd->uDisplay)
      {
      case 0: //Quan
         FloatTo (pszResult, dQuan, 0);
         break;
 
      case 1: //Ext
         FloatTo (pszResult, prd->dUnitPrice * dQuan, 1);
         break;
     
      case 2: //Pct
         FloatTo (pszResult, dQuan / prd->dJobQuantity, 2);
         break;
     }
   return;
   }





/*  This proc fills in all the edit boxes
 *  based on the setting of the combo box
 */
static USHORT DisplayEdits (HWND hwnd, PROWDAT prd)
   {
   USHORT   uScrollPos, i, j;
   char     szTmp [40];

   if (!prd->bInit) return 0;

   /*--- Display in Edit Boxes ---*/
   uScrollPos = (UM) WinSendMsg (prd->hwndDL, LM_QUERYTOPINDEX, 0, 0);
   for (i= uScrollPos, j=0; i < prd->uRows && j <6; i++, j++)
      {
      MakeDisplayStr (szTmp, prd->pdQuantity[i], prd);
      WinSetDlgItemText (hwnd, DID_LISTEDIT + j, szTmp);
      }

   for (;j <6; j++)
      WinSetDlgItemText (hwnd, DID_LISTEDIT + j, "");

   return 0;
   }



/*  This proc fills in all the totals
 *  based on the setting of the combo box
 */
static USHORT DisplayTotals (HWND hwnd, PROWDAT prd)
   {
   USHORT   i;
   DOUBLE   dTot, dDiff;
   char     szTmp [40];

   if (!prd->bInit) return 0;

   /*--- Display In ProjItem Total ---*/
   dTot = 0;
   for (i=0; i < prd->uRows; i++)
      dTot += prd->pdQuantity[i];
   MakeDisplayStr (szTmp, dTot, prd);
   WinSetDlgItemText (hwnd, DID_PIT, szTmp);

   /*--- Display In JobItem Total ---*/
   MakeDisplayStr (szTmp, prd->dJobQuantity, prd);
   WinSetDlgItemText (hwnd, DID_JIT, szTmp);

   /*--- Display In Difference ---*/
   dDiff = prd->dJobQuantity - dTot;
   MakeDisplayStr (szTmp, dDiff, prd);
   WinSetDlgItemText (hwnd, DID_DIF, szTmp);
   return 0;
   }






static USHORT UpdateQuantities (HWND hwnd, USHORT uEditId, PROWDAT prd)
   {
   USHORT   uRow;
   char     szTmp [40];

   WinQueryDlgItemText (hwnd, uEditId, sizeof szTmp, szTmp);

//   uRow = uEditId - DID_LISTEDIT +
//           (UM)WinSendMsg (prd->hwndV, SBM_QUERYPOS, 0, 0);
   uRow = uEditId - DID_LISTEDIT +
           (UM)WinSendMsg (prd->hwndDL, LM_QUERYTOPINDEX, 0, 0);

   switch (prd->uDisplay)
      {
      case 0: //Quan
         prd->pdQuantity[uRow] = ToFloat (szTmp, 0);
         break;
 
      case 1: //Ext
         prd->pdQuantity[uRow] = ToFloat (szTmp, 1) / prd->dUnitPrice;
         break;
     
      case 2: //Pct
         prd->pdQuantity[uRow] = ToFloat (szTmp, 2) * prd->dJobQuantity;
         break;
      }

   DisplayTotals (hwnd, prd);
   return 0;
   }




USHORT DoProjItemUpdate (HWND hwnd, PROWDAT prd)
   {
   PSZ    pszPCN, pszCat, pszSN;
   char   szQuery[256];
   HHEAP  hheap;
   USHORT i, uRet;

   hheap   = QW (prd->hwndDL, ITIWND_HEAP, 0, 0, 0);

   for (i=0; i< prd->uRows; i++)
      {
      pszPCN  = QW (prd->hwndDL, ITIWND_DATA, 0, i, cProjectControlNumber);
      pszCat  = QW (prd->hwndDL, ITIWND_DATA, 0, i, cCategory);
      pszSN   = QW (prd->hwndDL, ITIWND_DATA, 0, i, cSequenceNumber);

      sprintf (szQuery, " UPDATE ProjectItem SET Quantity = %f"
                        " WHERE ProjectControlNumber = '%s'"
                        " AND Category = '%s'"
                        " AND SequenceNumber = '%s'",
                        prd->pdQuantity[i],
                        pszPCN, pszCat, pszSN);

      if (uRet = ItiDbExecSQLStatement (hheap, szQuery))
         return uRet;

      sprintf (szQuery, " UPDATE ProjectItem SET ExtendedAmount ="
                        " UnitPrice * Quantity"
                        " WHERE ProjectControlNumber = '%s'"
                        " AND Category = '%s'"
                        " AND SequenceNumber = '%s'",
                        pszPCN, pszCat, pszSN);
      if (uRet = ItiDbExecSQLStatement (hheap, szQuery))
         return uRet;

      ItiMemFree (hheap, pszPCN);
      ItiMemFree (hheap, pszCat);
      ItiMemFree (hheap, pszSN );

      }
   ItiDbUpdateBuffers ("ProjectItem");
   }






MRESULT EXPORT ProjectItemDProc (HWND   hwnd,
                                 USHORT uMsg,
                                 MPARAM mp1,
                                 MPARAM mp2)
   {
   static ROWDAT rd;

   switch (uMsg)
      {
      case WM_INITQUERY:
         {
         USHORT   i;
         
         rd.bInit = FALSE;
         /*--- Display Combobox ---*/
         for (i=0; i < sizeof ppszViews / sizeof (PSZ); i++)
            WinSendDlgItemMsg (hwnd, ProjectItemC, LM_INSERTITEM,
                                  (MPARAM)LIT_END, MPFROMP (ppszViews[i]));

         WinSendDlgItemMsg (hwnd, ProjectItemC, LM_SELECTITEM, (MPARAM)0, MPFROMP (TRUE));
         break;
         }

      case WM_QUERYDONE:
         {
         InitEdits (hwnd, &rd);
         DisplayEdits (hwnd, &rd);
         DisplayTotals (hwnd, &rd);

         WinSendDlgItemMsg (hwnd, DID_PIT, EM_SETREADONLY, (MPARAM)1L, 0);
         WinSendDlgItemMsg (hwnd, DID_JIT, EM_SETREADONLY, (MPARAM)1L, 0);
         WinSendDlgItemMsg (hwnd, DID_DIF, EM_SETREADONLY, (MPARAM)1L, 0);
         break;
         }

      case WM_CONTROL:
         {
         USHORT i, uCmd = SHORT2FROMMP (mp1);
         HWND   hwndFocus;

         switch (SHORT1FROMMP (mp1))
            {
            case ProjectItemDL:
               if (uCmd != LN_SCROLL)
                  break;

               hwndFocus = WinQueryFocus (HWND_DESKTOP, 0);
               for (i=0; i<6; i++)
                  if (WinWindowFromID (hwnd, DID_LISTEDIT + i) == hwndFocus)
                     UpdateQuantities (hwnd, DID_LISTEDIT + i, &rd);

               DisplayEdits (hwnd, &rd);
               break;

            case ProjectItemC:
               {
               USHORT   i;

               if (uCmd != CBN_LBSELECT)
                  break;

               i = (UM) WinSendDlgItemMsg (hwnd, ProjectItemC, LM_QUERYSELECTION, 0, 0);
               if (i == LIT_NONE || i == rd.uDisplay)
                  return 0;

               rd.uDisplay = i;
               DisplayEdits (hwnd, &rd);
               DisplayTotals (hwnd, &rd);
               break;
               }

            case DID_LISTEDIT + 0:
            case DID_LISTEDIT + 1:
            case DID_LISTEDIT + 2:
            case DID_LISTEDIT + 3:
            case DID_LISTEDIT + 4:
            case DID_LISTEDIT + 5:
               if (uCmd != EN_KILLFOCUS && uCmd != EN_CHANGE)
                  break;

               UpdateQuantities (hwnd, SHORT1FROMMP (mp1), &rd);
               break;
            }
         break;
         }

      case WM_COMMAND:
         {
         switch (SHORT1FROMMP (mp1))
            {
            case DID_DISTRIBUTE:
            case DID_OK:
               {
               USHORT   i;
               DOUBLE   dTot, dDelta;

               if (!rd.bInit)
                  return 0;

               dTot = 0;
               for (i=0; i < rd.uRows; i++)
                  dTot += rd.pdQuantity[i];


               if (SHORT1FROMMP (mp1) == DID_OK)
                  {
                  if ((rd.dJobQuantity - dTot) > 0.00001 || (rd.dJobQuantity - dTot) < -0.00001)
                     {
                     WinMessageBox (HWND_DESKTOP, hwnd, 
                                    "The Project Item Totals Must equal "
                                    "the Job Item Total",
                                    "Job/Project Dialog",
                                    666, MB_ICONHAND | MB_MOVEABLE | MB_OK);
                     return 0;
                     }
                  else
                     {
                     DoProjItemUpdate (hwnd, &rd);
                     break;
                     }
                  }

               if (rd.uDisplay == 2)  /*--- distribute weighted ---*/
                  {
                  dDelta = (rd.dJobQuantity - dTot) / dTot;

                  for (i=0; i < rd.uRows; i++)
                     rd.pdQuantity[i] += (dDelta * rd.pdQuantity[i]);

                  }
               else /*--- distribute evenly ---*/
                  {
                  dDelta = (rd.dJobQuantity - dTot) / rd.uRows;

                  for (i=0; i < rd.uRows; i++)
                     rd.pdQuantity[i] += dDelta;
                  }
               DisplayEdits (hwnd, &rd);
               DisplayTotals (hwnd, &rd);
               break;
               }
            }
         break;
         }
      }
   return ItiWndDefDlgProc (hwnd, uMsg, mp1, mp2);
   }

