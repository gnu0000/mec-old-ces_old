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
 * Paramet.c 
 * Mark Hampton
 *
 * This module provides Parametric Estimation.
 */


#define INCL_DOSMISC
#define INCL_WIN

#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itimbase.h"
#include "..\include\itidbase.h"
#include "..\include\winid.h"
#include "..\include\itibase.h"
#include "..\include\itiglob.h"
#include "..\include\itiutil.h"
#include "..\include\itimenu.h"
#include "..\include\dialog.h"
#include "..\include\colid.h"
#include "..\include\itierror.h"
#include "..\include\itifmt.h"
#include "..\include\paracalc.h"
#include "paramet.h"
#include "initcat.h"
#include "dialog.h"
#include <stdio.h>

static CHAR szDllVersion[20] = "1.1c_ Paramet.DLL";

static BOOL   bSet;
static DOUBLE dPctTotal = 0.0, dMaxPctAllowed = 0.0;

static CHAR  szQty[30] = "";

//static BOOL bDoneThat = FALSE;

/*
 * ItiDllQueryVersion returns the version number of this DLL.
 */

USHORT EXPORT ItiDllQueryVersion (void)
   {
   // return VERSION;
   ItiErrWriteDebugMessage (szDllVersion);
   return 3;  // 1.1c = 3
   }





/*
 * ItiDllQueryCompatibility returns TRUE if this DLL is compatable
 * with the caller's version numer, otherwise FALSE is returned.
 */

BOOL EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion)
   {
   CHAR szMsg[100] = "";

   if ( ItiVerCmp(szDllVersion) )
      {
      return TRUE;
      }

   return FALSE;
   }



VOID EXPORT ItiDllQueryMenuName (PSZ      pszBuffer, 
                                 USHORT   usMaxSize,
                                 PUSHORT  pusWindowID)
   {
   ItiStrCpy (pszBuffer, "Parametric Profile Catalo~g", usMaxSize);
   *pusWindowID = ParametricProfileCatS;
   }


static void SetTotal (HWND hwnd)
   {
   HHEAP    hheap;
   PSZ      *ppsz;
   PSZ      pszProfileKey = NULL;
   PSZ      pszPctOfCost;
   CHAR     szQuery [1024] = "", szPctStr [64];
   USHORT   usNumCols = 0, usReturn, usRow;
   DOUBLE   dPctOfCost;
   HWND     hwndList;

   hheap = WinSendMsg (hwnd, WM_ITIWNDQUERY, MPFROMSHORT (ITIWND_HEAP), 0);
   pszProfileKey = WinSendMsg (hwnd, WM_ITIWNDQUERY,
                           MPFROMSHORT (ITIWND_DATA),
                           MPFROM2SHORT (0, cProfileKey));

   if (   (pszProfileKey == NULL) || ((*pszProfileKey)== '0') )
      {
      sprintf (szQuery, "/* paramet.c.SetTotal (has null key) */  SELECT 0");
      }
   else      
      {
      ItiStrCpy (szQuery,
               "/* paramet.SetTotal */ "
               "SELECT sum(PercentofValue) "
               "FROM MajorItem, SignificantMajorItem "
               "WHERE SignificantMajorItem.MajorItemKey = MajorItem.MajorItemKey "
               "AND SignificantMajorItem.ProfileKey = ", sizeof szQuery);
      ItiStrCat (szQuery, pszProfileKey, sizeof szQuery);
      }

   ppsz = ItiDbGetRow1 (szQuery, hheap, hmodMe, ITIRID_CALC, SignifMajItemS,
                           &usNumCols);
   if (ppsz != NULL && ItiCharIsDigit (*ppsz [0]))
      {

      WinSendMsg (hwnd, WM_ITIWNDSET, MPFROM2SHORT (ITIWND_LABEL, 0),
                  MPFROMP (ppsz [0]));

      usReturn = ItiFmtCheckString (ppsz [0], szPctStr, sizeof szPctStr,
                                    "Number,....", NULL);
         
      if (!usReturn)
         dPctTotal = ItiStrToDouble (ppsz [0]);
      else
         dPctTotal = 100.0;

      dMaxPctAllowed = dPctOfCost = 0.0;
   
      if (ItiWndGetDlgMode () == ITI_ADD)
         dMaxPctAllowed = 100.0 - dPctTotal;
      else
         {
         hwndList = WinWindowFromID (hwnd, SignifMajItemL);
         usRow = (UM) QW (hwndList, ITIWND_ACTIVE, 0, 0, 0);

         pszPctOfCost = (PSZ) QW (hwndList, ITIWND_DATA,
                                  0, usRow, cPercentofValue);

         if (pszPctOfCost != NULL)
            {
            usReturn = ItiFmtCheckString (pszPctOfCost,
                                          szPctStr, sizeof szPctStr,
                                          "Number,....", NULL);
         
            if (!usReturn)
               dPctOfCost = ItiStrToDouble (pszPctOfCost);

            dMaxPctAllowed = 100.0 - dPctTotal + dPctOfCost;
            }
         else
            dMaxPctAllowed = 0.0;
         }

      ItiFreeStrArray (hheap, ppsz, usNumCols);
      }
   else
      {
      WinSendMsg (hwnd, WM_ITIWNDSET, MPFROM2SHORT (ITIWND_LABEL, 0),
                  MPFROMP ("0.00%"));
      dMaxPctAllowed = 100.0;
      }

   if (pszProfileKey)
      ItiMemFree (hheap, pszProfileKey);

   } /*  End of Function SetTotal */


/*  Function SetQualitativeListBox will handle the special case
 *  windows for Major item Price & Quantity Regression.
 *  The qualitative adjustment list box receives its key value
 *  from its sibling, the qualitative variable list, rather than
 *  its parent.
 */

MRESULT SetQualitativeListBox (HWND    hwnd,
                               USHORT  usMessage,
                               PUSHORT usWinId,
                               USHORT  usLabelId,
                               MPARAM  mp1,
                               MPARAM  mp2)
   {
   switch (usMessage)
      {
      case WM_INITQUERY:
         {
         HWND     hwndL1;

         /* --- manually init normal list window --- */
         hwndL1 = WinWindowFromID (hwnd, usWinId [0]);
         WinSendMsg (hwndL1, WM_INITQUERY, mp1, mp2);

         /* --- other list window isn't yet ready--- */

         /* --- turn off auto child init --- */
         mp2 = MPFROM2SHORT (SHORT1FROMMP (mp2), TRUE);

         /* --- allow msg to continue slightly modified --- */
         return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
         }

      case WM_ITILWMOUSE:
         {
         MPARAM   MP1, MP2;
         HBUF     hbufL1;
         HWND     hwndL1, hwndL2;
         char     szQualVar [64], szMajItemKey [64];
         HHEAP    hheap;

         if (!(SHORT1FROMMP (mp2) == usWinId [0]) ||
             !ItiWndIsSingleClick (mp2) ||
             !ItiWndIsLeftClick (mp2))
            return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);

         hheap  = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
         hwndL1 = WinWindowFromID (hwnd, usWinId [0]);
         hwndL2 = WinWindowFromID (hwnd, usWinId [1]);
         hbufL1 = (HBUF) QW (hwndL1, ITIWND_BUFFER, 0, 0, 0);

         /*  Get Key Values */
         ItiDbGetBufferRowCol (hbufL1, SHORT1FROMMP (mp1), cMajorItemKey, szMajItemKey);
         ItiDbGetBufferRowCol (hbufL1, SHORT1FROMMP (mp1), cQualitativeVariable, szQualVar);

         /* Set Qualitative Variable label on the parent window */
         SWL (hwnd, ITIWND_LABEL, usLabelId, szQualVar);

         MP1 = MPFROMP (hbufL1);  /* --- buffer to use  --- */
         MP2 = mp1;               /* --- row clicked on in low word--- */
         WinSendMsg (hwndL2, WM_CHANGEQUERY, MP1, MP2);
         return 0;
         }

      }

   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }/* End of Function SetQualitativeListBox */




MRESULT EXPORT JobBreakdownProfileSProc (HWND   hwnd,
                                        USHORT usMessage,
                                        MPARAM mp1,
                                        MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_QUERYDONE:
         {
         DosBeep(1200, 20);
         break;
         }

      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_EDIT_MENU:
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, TRUE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, TRUE);
               return 0;
               break;

            case IDM_VIEW_MENU:
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "~Job Breakdown Major Items", 
                                      IDM_VIEW_USER_FIRST + 1);
               return 0;
               break;
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_CHANGE:
               /* ? */
               ItiWndDoDialog (hwnd, 0, FALSE);
               ItiDbUpdateBuffers ("JobBreakdown");
               ItiDbUpdateBuffers ("Job");
               break;

            case (IDM_VIEW_USER_FIRST + 1):
               ItiWndBuildWindow (hwnd, JobBreakdownMajorItemS);
               ItiDbUpdateBuffers ("JobBreakdown");
               ItiDbUpdateBuffers ("Job");
               return 0;
               break;
            }
         break;
      }

   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }    





MRESULT EXPORT ParametricProfileCatSProc (HWND   hwnd,
                                         USHORT usMessage,
                                         MPARAM mp1,
                                         MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_EDIT_MENU:
               {
               BOOL bSelected;

               bSelected = ItiWndQueryActive (hwnd, ParametricProfileCatL, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, !bSelected);
               return 0;
               break;
               }

            case IDM_VIEW_MENU:
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_PARENT, TRUE);
               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2), 
                                           IDM_VIEW_USER_FIRST);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "~Significant Major Items", 
                                      IDM_VIEW_USER_FIRST + 1);
               return 0;

            }
            break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {

            HWND  hwndChild;

            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, ParametricProfileCatL, FALSE);
               ItiDbUpdateBuffers ("ParametricProfile");
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, ParametricProfileCatL, TRUE);
               ItiDbUpdateBuffers ("ParametricProfile");
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, ParametricProfileCatL), ITI_DELETE);
               break;

            case (IDM_VIEW_USER_FIRST + 1):
               hwndChild = WinWindowFromID (hwnd, ParametricProfileCatL);
               ItiWndBuildWindow (hwndChild, SignifMajItemS);
               return 0;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }





MRESULT EXPORT JobBreakdownMajorItemSProc (HWND   hwnd,
                                        USHORT usMessage,
                                        MPARAM mp1,
                                        MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_EDIT_MENU:
               {
               BOOL bSelected;

               bSelected = ItiWndQueryActive (hwnd, JobBreakdownMajorItemL, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, TRUE);
//csp 112594               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, TRUE);
               return 0;
               }
               break;

            case IDM_VIEW_MENU:
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_PARENT, TRUE);
               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2), 
                                           IDM_VIEW_USER_FIRST);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "Job Major Item ~Price Regression", 
                                      IDM_VIEW_USER_FIRST + 1);
               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "Job Major Item ~Quantity Regression", 
                                      IDM_VIEW_USER_FIRST + 2);
// ItiMenuGrayMenuItem (HWNDFROMMP (mp2),IDM_VIEW_USER_FIRST + 2, TRUE);
/* JUN 96 remove later after fixed. */  

               ItiMenuInsertMenuSeparator (HWNDFROMMP (mp2), 
                                           IDM_VIEW_USER_FIRST+3);

               ItiMenuInsertMenuItem (HWNDFROMMP (mp2), 
                                      "Reset Job Major Item to profile value", 
                                      IDM_VIEW_USER_FIRST + 4);
               return 0;
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            HWND  hwndChild;

            case (IDM_VIEW_USER_FIRST + 1):
               hwndChild = WinWindowFromID (hwnd, JobBreakdownMajorItemL);
               ItiWndBuildWindow (hwndChild, JobMajItemPriceRegrS);
               return 0;

            case (IDM_VIEW_USER_FIRST + 2):
               hwndChild = WinWindowFromID (hwnd, JobBreakdownMajorItemL);
               ItiWndBuildWindow (hwndChild, JobMajItemQuantityRegrS);
               return 0;

            case (IDM_VIEW_USER_FIRST + 4):
               //hwndChild = WinWindowFromID (hwnd, JobBreakdownMajorItemL);
               ResetMIValues(hwnd);
               ItiDbUpdateBuffers ("JobBreakdownMajorItem");
               ItiDbUpdateBuffers ("Job");
               ItiDbUpdateBuffers ("JobBreakdown");
               ItiDbUpdateBuffers ("JobBreakdownProfile");
               //return 0;
               break;

          // 25 JAN 94
            case IDM_CHANGE:
               ItiWndDoDialog (hwnd, JobBreakdownMajorItemL, FALSE);
               ItiDbUpdateBuffers ("JobBreakdown");
               ItiDbUpdateBuffers ("Job");
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, JobBreakdownMajorItemL, TRUE);
               ItiDbUpdateBuffers ("JobBreakdown");
               ItiDbUpdateBuffers ("Job");
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, JobBreakdownMajorItemL), ITI_DELETE);
               ItiDbUpdateBuffers ("JobBreakdown");
               ItiDbUpdateBuffers ("Job");
               break;
            }
         break;
      }

   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }




MRESULT EXPORT SignifMajItemSProc (HWND   hwnd,
                                  USHORT usMessage,
                                  MPARAM mp1,
                                  MPARAM mp2)
   {
   switch (usMessage)
      {
      case WM_ITIDBBUFFERDONE:
      case WM_QUERYDONE:
         SetTotal (hwnd);
         ItiDbUpdateBuffers ("SignificantMajorItem");
         break;

      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_EDIT_MENU:
               {
               BOOL bSelected;

               bSelected = ItiWndQueryActive (hwnd, SignifMajItemL, NULL);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_CHANGE, !bSelected);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_DELETE, !bSelected);
               return 0;
               break;
               }
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_CHANGE:
               SetTotal (hwnd);

               ItiWndDoDialog (hwnd, SignifMajItemL, FALSE);
               SetTotal (hwnd);
               ItiDbUpdateBuffers ("SignificantMajorItem");
               break;

            case IDM_ADD:
               ItiWndDoDialog (hwnd, SignifMajItemL, TRUE);
               SetTotal (hwnd);
               ItiDbUpdateBuffers ("SignificantMajorItem");
               break;

            case IDM_DELETE:
               ItiWndAutoModifyDb (WinWindowFromID (hwnd, SignifMajItemL), ITI_DELETE);
               SetTotal (hwnd);
               ItiDbUpdateBuffers ("SignificantMajorItem");
               break;
            }
         break;
      }

   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }




MRESULT EXPORT JobMajItemPriceRegrSProc (HWND   hwnd,
                                        USHORT usMessage,
                                        MPARAM mp1,
                                        MPARAM mp2)
   {

   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }



MRESULT EXPORT MajItemPriceRegrSProc (HWND   hwnd,
                                            USHORT usMessage,
                                            MPARAM mp1,
                                            MPARAM mp2)
   {
   USHORT usWinId [2] = { MajItemPriceRegrL1,
                          MajItemPriceRegrL2 };
   USHORT usLabelId   = 9;

   return SetQualitativeListBox (hwnd,
                                 usMessage,
                                 usWinId,
                                 usLabelId,
                                 mp1,
                                 mp2);
   }



MRESULT EXPORT MajItemQuantityRegrSProc
                       (HWND hwnd, USHORT usMessage, MPARAM mp1, MPARAM mp2)
   {
   MRESULT mrS;
   USHORT usWinId [2] = { MajItemQuantityRegrL1,
                          MajItemQuantityRegrL2 };
   USHORT usVAL,  usLabelId   = 21;

   switch (usMessage)
      {
      case WM_ITIDBBUFFERDONE:
         {
         HBUF hbufS;
         CHAR szMajItemKey[15] = "";

         hbufS = (HBUF) QW (hwnd, ITIWND_BUFFER, 0, 0, 0);

         /*  Get Key Values */
         ItiDbGetBufferRowCol (hbufS, SHORT1FROMMP (mp1), cMajorItemKey, szMajItemKey);

         usVAL = ParaCalcDetermineVorAorL (szMajItemKey);       
         switch (usVAL)
            {
            case PARACALC_VOLUME:
               SW (hwnd, ITIWND_LABELHIDDEN, PARACALC_VOLUME, FALSE, 0);
               SW (hwnd, ITIWND_LABELHIDDEN, PARACALC_AREA,    TRUE, 0);
               SW (hwnd, ITIWND_LABELHIDDEN, PARACALC_LENGTH,  TRUE, 0);
               break;

            case PARACALC_AREA:
               SW (hwnd, ITIWND_LABELHIDDEN, PARACALC_VOLUME,  TRUE, 0);
               SW (hwnd, ITIWND_LABELHIDDEN, PARACALC_AREA,   FALSE, 0);
               SW (hwnd, ITIWND_LABELHIDDEN, PARACALC_LENGTH,  TRUE, 0);
               // ------
               //SWL (hwnd, ITIWND_DATAHIDDEN, 1,  TRUE);  //BaseCoefficient,       
               //SWL (hwnd, ITIWND_DATAHIDDEN, 3,  FALSE); //WidthAdjustment,       
               //SWL (hwnd, ITIWND_DATAHIDDEN, 5,  TRUE);  //StructureBaseAdjustment
               //
               //SW (hwnd, ITIWND_DATAHIDDEN, 2, TRUE, 0);  //DepthAdjustment,         
               //SW (hwnd, ITIWND_DATAHIDDEN, 4, FALSE, 0); //CrossSectionAdjustment,  
               //SW (hwnd, ITIWND_DATAHIDDEN, 6, TRUE, 0);  //StructureLengthAdjustment
               // ------ FEB 97
               break;

            case PARACALC_LENGTH:
               SW (hwnd, ITIWND_LABELHIDDEN, PARACALC_VOLUME,  TRUE, 0);
               SW (hwnd, ITIWND_LABELHIDDEN, PARACALC_AREA,    TRUE, 0);
               SW (hwnd, ITIWND_LABELHIDDEN, PARACALC_LENGTH, FALSE, 0);
               break;
            } /* end switch */
         } /* end case WM_ITIDBBUFFERDONE */
         break;
      }
   mrS = SetQualitativeListBox (hwnd, usMessage, usWinId, usLabelId, mp1, mp2);

   return mrS;
   }




MRESULT EXPORT JobMajItemQuantityRegrSProc (HWND   hwnd,
                                           USHORT usMessage,
                                           MPARAM mp1,
                                           MPARAM mp2)
   {
   MRESULT  mr2;
   USHORT usVAL2;
   PSZ pszMIKey;

   mr2 = ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);

   switch (usMessage)
      {
      case WM_QUERYDONE:
      case WM_ITIDBBUFFERDONE:
         {
     //    //HBUF hbufS2;
     //    CHAR szMajItemKey2[15] = "";
     //
     //    //hbufS2 = (HBUF) QW (hwnd, ITIWND_BUFFER, 0, 0, 0);
     //
     //    /*  Get Key Values */
     //    //ItiDbGetBufferRowCol (hbufS2, SHORT1FROMMP (mp1), cMajorItemKey, szMajItemKey2);
     //    pszMIKey = WinSendMsg (hwnd, WM_ITIWNDQUERY,
     //                      MPFROMSHORT (ITIWND_DATA),
     //                      MPFROM2SHORT (0, cMajorItemKey));
     //
     //    if ((pszMIKey != NULL) && (*pszMIKey != '\0'))
     //       {
     //       ItiStrCat (szMajItemKey2, pszMIKey, sizeof szMajItemKey2);
     //       usVAL2 = ParaCalcDetermineVorAorL (szMajItemKey2);       
     //       }
     //    else
     //       usVAL2 = PARACALC_AREA;
     //
     //    switch (usVAL2)
     //       {
     //       case PARACALC_VOLUME:
     //          SW (hwnd, ITIWND_LABELHIDDEN, 8,  FALSE, 0);
     //          SW (hwnd, ITIWND_LABELHIDDEN, 7,   TRUE, 0);
     //          SW (hwnd, ITIWND_LABELHIDDEN, 6,   TRUE, 0);
     //          SW (hwnd, ITIWND_LABELHIDDEN, 11, FALSE, 0);
     //          SW (hwnd, ITIWND_LABELHIDDEN, 10,  TRUE, 0);
     //          SW (hwnd, ITIWND_LABELHIDDEN,  9,  TRUE, 0);
     //          break;
     //
     //       case PARACALC_AREA:
     //          SW (hwnd, ITIWND_DATAHIDDEN, 8,  TRUE, 0);
     //          SW (hwnd, ITIWND_DATAHIDDEN, 7, FALSE, 0);
     //          SW (hwnd, ITIWND_DATAHIDDEN, 6,  TRUE, 0);
     //          SW (hwnd, ITIWND_DATAHIDDEN, 11,  TRUE, 0);
     //          SW (hwnd, ITIWND_DATAHIDDEN, 10, FALSE, 0);
     //          SW (hwnd, ITIWND_DATAHIDDEN,  9,  TRUE, 0);
     //          break;
     //
     //       case PARACALC_LENGTH:
     //          SW (hwnd, ITIWND_DATAHIDDEN, 8,  TRUE, 0);
     //          SW (hwnd, ITIWND_DATAHIDDEN, 7,  TRUE, 0);
     //          SW (hwnd, ITIWND_DATAHIDDEN, 6, FALSE, 0);
     //          SW (hwnd, ITIWND_DATAHIDDEN, 11,  TRUE, 0);
     //          SW (hwnd, ITIWND_DATAHIDDEN, 10,  TRUE, 0);
     //          SW (hwnd, ITIWND_DATAHIDDEN,  9, FALSE, 0);
     //          break;
     //       } /* end inner switch */
         } /* end case WM_ITIDBBUFFERDONE */
         
      }/* end switch */

   //mr2 = ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);

   return mr2;
   }




MRESULT EXPORT ParametricProfileDProc (HWND    hwnd,
                                          USHORT   usMessage,
                                          MPARAM   mp1,
                                          MPARAM   mp2)
   {
   MRESULT mr;
   HHEAP hhp;
   PSZ psz;
   CHAR szTmp[256] = "";

   switch (usMessage)
      {
      case WM_INITDLG:
         bSet = FALSE;
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
               mr = ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
               bSet = TRUE;
               if (bSet)
                  {
                  hhp = ItiMemCreateHeap (1024);

                  sprintf (szTmp,
                     "/* paramet.c.ParametricProfileDProc */ "
                     " SELECT MAX(ProfileKey) FROM ParametricProfile ");
                  psz = ItiDbGetRow1Col1 (szTmp, hhp, 0, 0, 0);
                  if (NULL == psz)
                     {
                  	ItiErrWriteDebugMessage
                  		("paramet.c.ParametricProfileDProc, failed to get ProfileKey.");
                     ItiMemDestroyHeap (hhp);
                     bSet = FALSE;
                     return mr;
                     }

                  ItiStrCpy (szTmp,
                     "/* paramet.c.ParametricProfileDProc */ "
                     " INSERT INTO SignificantMajorItem"
                     " (ProfileKey, MajorItemKey, PercentofValue,"
                     " PercentofValueLock)"
                     " VALUES ( ", sizeof szTmp);
                  ItiStrCat (szTmp, psz, sizeof szTmp);
                  ItiStrCat (szTmp, " ,1,0,0) ", sizeof szTmp);
                  
                  ItiDbExecSQLStatement (hhp, szTmp);
                  
                  ItiMemDestroyHeap (hhp);
                  }
               return mr;
               break;

            case DID_CANCEL:   /* CANCEL button pushed */
               WinDismissDlg (hwnd, FALSE);
               return 0;
               break;

            default:
               break;
            }
         break;

      }// end of switch

   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }/* End of Function */




MRESULT EXPORT SignifMajItemDProc (HWND    hwnd,
                                      USHORT   usMessage,
                                      MPARAM   mp1,
                                      MPARAM   mp2)
   {
   HWND  hwndList;

   switch (usMessage)
      {
      case WM_ITIDBBUFFERDONE:
         hwndList = (HWND) QW (hwnd, ITIWND_OWNERWND, 1, 0, 0);
         SetTotal (hwndList);
         break;

      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, MajItemEditDL, cMajorItemKey, NULL, cMajorItemKey);
         hwndList = (HWND) QW (hwnd, ITIWND_OWNERWND, 1, 0, 0);
         SetTotal (hwndList);

//csp 112594
         if (ItiWndGetDlgMode () == ITI_CHANGE)
            {
            WinEnableWindow (WinWindowFromID (hwnd, DID_MAJORITEM), FALSE);
            WinEnableWindow (WinWindowFromID (hwnd, MajItemEditDL), FALSE);
            WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, DID_PCTOFVALUE));
            return 0;
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            char   szPctStr [64], szPctOfCost [64];
            DOUBLE dPctEntered = 0.0;
            USHORT usReturn;

            case DID_OK:
            case DID_ADDMORE:
               {
               hwndList = (HWND) QW (hwnd, ITIWND_OWNERWND, 1, 0, 0);
               WinQueryDlgItemText (hwnd, DID_PCTOFVALUE,
                                          sizeof szPctOfCost, szPctOfCost);

               usReturn = ItiFmtCheckString (szPctOfCost,
                                             szPctStr, sizeof szPctStr,
                                             "Number,....", NULL);

               if (!usReturn)
                  dPctEntered = ItiStrToDouble (szPctOfCost);

               if (dPctEntered > dMaxPctAllowed)
                  {
                  sprintf (szPctStr, "The percentage entered will exceed "
                                     " 100 percent for this profile");

                  WinMessageBox (HWND_DESKTOP, hwnd, szPctStr,  
                                 "Change Significant Major Item", 666,
                                 MB_MOVEABLE | MB_OK | 
                                 MB_APPLMODAL | MB_ICONHAND);
                  return 0;
                  }
               else
                  {
                  SetTotal (hwndList);
                  ItiDbUpdateBuffers ("SignificantMajorItem");
                  }
               }
               break;

            case DID_CANCEL:   /* CANCEL button pushed */
               WinDismissDlg (hwnd, FALSE);
               return 0;
               break;

            default:
               break;
            }
         break;

      case WM_ITILWMOUSE:
         if (SHORT1FROMMP (mp2) == MajItemEditDL &&
             ItiWndIsSingleClick (mp2) && ItiWndIsLeftClick (mp2))
            {
            PSZ  pszTemp;
            HWND hwndList;
            HHEAP hheap;
            USHORT usRow;

            hwndList = WinWindowFromID (hwnd, MajItemEditDL);
            if (hwndList == NULL)
               break;
//csp 112594
            if (ItiWndGetDlgMode () == ITI_CHANGE)
               break;
            
            usRow = (UM) QW (hwndList, ITIWND_ACTIVE, 0, 0, 0);

            hheap = (HHEAP) QW (hwndList, ITIWND_HEAP, 0, 0, 0);
            if (hheap == NULL)
               break;

            pszTemp = (PSZ) QW  (hwndList, ITIWND_DATA, 0, usRow, cMajorItemID);
            if (pszTemp != NULL)
               {
               WinSetDlgItemText (hwnd, DID_MAJORITEM, pszTemp);
               ItiMemFree (hheap, pszTemp);
               }
            }
         break;
      }


   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }



BOOL EXPORT IsButtonChecked (HWND hwnd, USHORT usControl)
   {
   return 1 == SHORT1FROMMR (WinSendDlgItemMsg (hwnd, usControl, 
                                                BM_QUERYCHECK, 0, 0));
   }



MRESULT EXPORT JobBreakdownMajorItemEProc (HWND    hwnd,
                                              USHORT   usMessage,
                                              MPARAM   mp1,
                                              MPARAM   mp2)
   {
   MRESULT mr;
   USHORT usDlgMode, uRowSel;
   PSZ    pszVol, pszArea, pszLen, pszTemp;
   CHAR  szQuantity[20] = "";
   CHAR szTmp[254] =
     " SELECT Quantity FROM JobBreakdownMajorItem WHERE JobKey = ";
   BOOL bDoneThat;

   switch (usMessage)
      {
      case WM_INITDLG:
         //ItiWndSetHourGlass (TRUE);
         bDoneThat = FALSE;
         break;

      case WM_ITILWMOUSE:
         {
         if (SHORT1FROMMP (mp2) == MajItemEditDL &&
             ItiWndIsSingleClick (mp2) && ItiWndIsLeftClick (mp2))
            {
            PSZ  pszTemp;
            HWND hwndList;
            HHEAP hheap;
            USHORT usRow;

            hwndList = WinWindowFromID (hwnd, MajItemEditDL);
            if (hwndList == NULL)
               break;
//csp 112594
            if (ItiWndGetDlgMode () == ITI_CHANGE)
               break;
            
            usRow = (UM) QW (hwndList, ITIWND_ACTIVE, 0, 0, 0);

            hheap = (HHEAP) QW (hwndList, ITIWND_HEAP, 0, 0, 0);
            if (hheap == NULL)
               break;

            pszTemp = (PSZ) QW (hwndList, ITIWND_DATA, 0, usRow, cMajorItemID);
            if (pszTemp != NULL)
               {
               WinSetDlgItemText (hwnd, DID_MAJORITEM, pszTemp);
               ItiMemFree (hheap, pszTemp);
               }
            }/* end of if (SHORT1FROMMP... */
         }
         break;

      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, MajItemEditDL, cMajorItemKey, NULL, cMajorItemKey);

         if (ItiWndGetDlgMode () == ITI_CHANGE)
            {
            HWND  hwndList3;
            HHEAP hheapList3, hhp;
            USHORT uRow;
            PSZ  psz3Vr, psz3Ar, psz3Lr, psz3MajItmKey;
            CHAR sz3MIKey[10] = "";

            ItiWndSetHourGlass (TRUE);

            pszLen = (PSZ) QW (hwnd, ITIWND_DATA, 0, 0, cUseLength);
            pszArea= (PSZ) QW (hwnd, ITIWND_DATA, 0, 0, cUseArea);
            pszVol = (PSZ) QW (hwnd, ITIWND_DATA, 0, 0, cUseVolume);
         
            /* -- Set the appropriate button. */
            if ((pszVol != NULL) && (*pszVol == '1'))
               {
               WinSendDlgItemMsg (hwnd, DID_VOL, BM_SETCHECK, MPFROMSHORT(1), (MPARAM) NULL );
               WinSendDlgItemMsg (hwnd, DID_AREA, BM_SETCHECK, MPFROMSHORT(0), (MPARAM) NULL );
               WinSendDlgItemMsg (hwnd, DID_LEN, BM_SETCHECK, MPFROMSHORT(0), (MPARAM) NULL );
               }

            if ((pszArea != NULL) && (*pszArea == '1'))
               {
               WinSendDlgItemMsg (hwnd, DID_VOL, BM_SETCHECK, MPFROMSHORT(0), (MPARAM) NULL );
               WinSendDlgItemMsg (hwnd, DID_AREA, BM_SETCHECK, MPFROMSHORT(1), (MPARAM) NULL );
               WinSendDlgItemMsg (hwnd, DID_LEN, BM_SETCHECK, MPFROMSHORT(0), (MPARAM) NULL );
               }

            if ((pszLen != NULL) && (*pszLen == '1'))
               {
               WinSendDlgItemMsg (hwnd, DID_VOL, BM_SETCHECK, MPFROMSHORT(0), (MPARAM) NULL );
               WinSendDlgItemMsg (hwnd, DID_AREA, BM_SETCHECK, MPFROMSHORT(0), (MPARAM) NULL );
               WinSendDlgItemMsg (hwnd, DID_LEN, BM_SETCHECK, MPFROMSHORT(1), (MPARAM) NULL );
               }


            WinEnableWindow (WinWindowFromID (hwnd, DID_MAJORITEM), FALSE);
            WinEnableWindow (WinWindowFromID (hwnd, DID_UNITPRICE), FALSE);
            WinSetFocus (HWND_DESKTOP, WinWindowFromID (hwnd, DID_QUANTITY));


            /* -- Compute the rý value for the V A L here. */
            if (bDoneThat)
               {
               hwndList3  = (HWND) QW(hwnd, ITIWND_OWNERWND, 0, 0, 0);
               hheapList3 = (HHEAP) QW (hwndList3, ITIWND_HEAP, 0, 0, 0);
               uRow = (UM) QW (hwndList3, ITIWND_ACTIVE, 0, 0, 0);
               psz3MajItmKey = (PSZ) QW (hwndList3, ITIWND_DATA, 0, uRow, cMajorItemKey);
               ItiStrCpy(sz3MIKey, psz3MajItmKey, sizeof sz3MIKey);
               hhp = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);

               psz3Vr = ParametMajorItemRSQ (hhp, sz3MIKey, " ", PARACALC_VOLUME);
               if (psz3Vr != NULL)
                 WinSetDlgItemText (hwnd, DID_VOL_R, psz3Vr);
               else
                 WinSetDlgItemText (hwnd, DID_VOL_R, " null ");
            
               psz3Ar = ParametMajorItemRSQ (hhp, sz3MIKey, " ", PARACALC_AREA);
               if (psz3Ar != NULL)
                 WinSetDlgItemText (hwnd, DID_AREA_R, psz3Ar);
               else
                 WinSetDlgItemText (hwnd, DID_AREA_R, " null ");
            
               psz3Lr = ParametMajorItemRSQ (hhp, sz3MIKey, " ", PARACALC_LENGTH);
               if (psz3Lr != NULL)
                 WinSetDlgItemText (hwnd, DID_LEN_R, psz3Lr);
               else
                 WinSetDlgItemText (hwnd, DID_LEN_R, " null ");
         
               bDoneThat = TRUE;
               } /* end if bDoneThat */

            ItiWndSetHourGlass (FALSE);
            return 0;
            }
         break;

      case WM_CONTROL:
         {
         HWND hwnd1, hwndList2;
         HHEAP hheapList2, hhp;
         USHORT   uRow;
         PSZ pszJobBreakdnKey, pszJobKey, pszMajItmKey;
         CHAR szJobKey[10], szJBKey[10], szMIKey[10], szCV[15] = "";
         CHAR szConvFac[15] = "";
         BOOL bButtonPushed = FALSE;

         hwnd1 = (HWND) QW (hwnd, ITIWND_OWNERWND, 1, 0, 0);
         pszJobKey = (PSZ) QW (hwnd1, ITIWND_DATA, 0, 0, cJobKey);
         ItiStrCpy(szJobKey, pszJobKey, sizeof szJobKey);

         pszJobBreakdnKey = (PSZ) QW (hwnd1, ITIWND_DATA, 0, 0, cJobBreakdownKey);
         ItiStrCpy(szJBKey, pszJobBreakdnKey, sizeof szJBKey);

         hwndList2 = (HWND) QW(hwnd, ITIWND_OWNERWND, 0, 0, 0);
         hheapList2 = (HHEAP) QW (hwndList2, ITIWND_HEAP, 0, 0, 0);
         uRow = (UM) QW (hwndList2, ITIWND_ACTIVE, 0, 0, 0);
         pszMajItmKey = (PSZ) QW (hwndList2, ITIWND_DATA, 0, uRow, cMajorItemKey);
         ItiStrCpy(szMIKey, pszMajItmKey, sizeof szMIKey);
         hhp = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);

         switch (SHORT1FROMMP (mp1))
            {
            case DID_VOL:
               DosBeep (1400,20);
               WinQueryDlgItemText (hwnd, DID_VOL_CONV, sizeof szCV, szCV);
               if (szCV[0] == '\0')
                  ItiStrCpy(szConvFac, " 1.0 ", sizeof szConvFac);
               else
                  ItiStrCpy(szConvFac, szCV, sizeof szConvFac);

               /* -- Recalc the Quantity to use V */
               pszTemp = ResetJBMIQty (hhp, szJobKey, szJBKey, szMIKey, PARACALC_VOLUME, szConvFac, FALSE);
               bButtonPushed = TRUE;
               WinSendDlgItemMsg (hwnd, DID_AREA, BM_SETCHECK, MPFROMSHORT(0), (MPARAM) NULL );
               WinSendDlgItemMsg (hwnd, DID_LEN, BM_SETCHECK, MPFROMSHORT(0), (MPARAM) NULL );
               break;

            case DID_AREA:
               DosBeep (1000,20);
               WinQueryDlgItemText (hwnd, DID_AREA_CONV, sizeof szCV, szCV);
               if (szCV[0] == '\0')
                  ItiStrCpy(szConvFac, " 1.0 ", sizeof szConvFac);
               else
                  ItiStrCpy(szConvFac, szCV, sizeof szConvFac);

               /* -- Recalc the Quantity to use A */
               pszTemp = ResetJBMIQty (hhp, szJobKey, szJBKey, szMIKey, PARACALC_AREA, szConvFac, FALSE);
               bButtonPushed = TRUE;
               WinSendDlgItemMsg (hwnd, DID_VOL, BM_SETCHECK, MPFROMSHORT(0), (MPARAM) NULL );
               WinSendDlgItemMsg (hwnd, DID_LEN, BM_SETCHECK, MPFROMSHORT(0), (MPARAM) NULL );
               break;

            case DID_LEN:
               DosBeep (1500,20);
               WinQueryDlgItemText (hwnd, DID_LEN_CONV, sizeof szCV, szCV);
               if (szCV[0] == '\0')
                  ItiStrCpy(szConvFac, " 1.0 ", sizeof szConvFac);
               else
                  ItiStrCpy(szConvFac, szCV, sizeof szConvFac);

               /* -- Recalc the Quantity to use L */
               pszTemp = ResetJBMIQty (hhp, szJobKey, szJBKey, szMIKey, PARACALC_LENGTH, szConvFac, FALSE);
               bButtonPushed = TRUE;
               WinSendDlgItemMsg (hwnd, DID_VOL, BM_SETCHECK, MPFROMSHORT(0), (MPARAM) NULL );
               WinSendDlgItemMsg (hwnd, DID_AREA, BM_SETCHECK, MPFROMSHORT(0), (MPARAM) NULL );
               break;

            default:
               bButtonPushed = FALSE;
               break;
            } /* end switch */


         /* -- Now reset the displayed Quantity to the new value. */
         if (bButtonPushed == TRUE)
            {
            if ((pszTemp != NULL) && ((*pszTemp) != '\0'))
               ItiStrCpy(szQuantity, pszTemp, sizeof szQuantity);
            else
               {
               ItiStrCpy(szQuantity, " 1.0 ", sizeof szQuantity);
               ItiErrWriteDebugMessage
                ("*=* WARNING MSG: JobBreakdownMajorItemEProc has a NULL quantity; using a value of 1.0 for the quantity.");
               }

            WinSetDlgItemText (hwnd, DID_QUANTITY, szQuantity);

            ItiStrCpy (szQty, szQuantity, sizeof szQty);
            ItiExtract (szQty, ",");
            mr = ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
            ParametMajorItemNewQtyPrice 
              (hheapList2, szMIKey, szJobKey, szJBKey, szQty, " ", FALSE);
            return mr;

            } /* end if bButtonPushed */

         }/* end case WM_CONTROL... */
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
               {
               HWND hwnd1, hwndList2;
               HHEAP hheapList2, hhp;
               USHORT   uRow;
               PSZ pszJobBreakdnKey, pszJobKey, pszMajItmKey;
               CHAR szJobKey[10], szJBKey[10], szMIKey[10], szCV[15] = "";
               CHAR szUpdate[510] = 
                     "/* JBMjrItmE */ "
                     "UPDATE JobBreakdownMajorItem SET "
                    " ConversionFactorLength = " ;

               hwnd1 = (HWND) QW (hwnd, ITIWND_OWNERWND, 1, 0, 0);
               pszJobKey = (PSZ) QW (hwnd1, ITIWND_DATA, 0, 0, cJobKey);
               ItiStrCpy(szJobKey, pszJobKey, sizeof szJobKey);

               pszJobBreakdnKey = (PSZ) QW (hwnd1, ITIWND_DATA, 0, 0, cJobBreakdownKey);
               ItiStrCpy(szJBKey, pszJobBreakdnKey, sizeof szJBKey);

               hwndList2 = (HWND) QW(hwnd, ITIWND_OWNERWND, 0, 0, 0);
               hheapList2 = (HHEAP) QW (hwndList2, ITIWND_HEAP, 0, 0, 0);
               uRow = (UM) QW (hwndList2, ITIWND_ACTIVE, 0, 0, 0);
               pszMajItmKey = (PSZ) QW (hwndList2, ITIWND_DATA, 0, uRow, cMajorItemKey);
               ItiStrCpy(szMIKey, pszMajItmKey, sizeof szMIKey);

            //  mr = ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);


               /* -- Set conversion factors. */
               if (IsButtonChecked (hwnd, DID_VOL))
                  pszVol = " 1 ";
               else
                  pszVol = " 0 ";

               if (IsButtonChecked (hwnd, DID_AREA))
                  pszArea = " 1 ";
               else
                  pszArea = " 0 ";

               if (IsButtonChecked (hwnd, DID_LEN))
                  pszLen = " 1 ";
               else
                  pszLen = " 0 ";

               WinQueryDlgItemText (hwnd, DID_LEN_CONV, sizeof szCV, szCV);
               if (szCV[0] == '\0')
                  ItiStrCat(szUpdate, " 1.0 ", sizeof szUpdate);
               else
                  ItiStrCat(szUpdate, szCV, sizeof szUpdate);

               ItiStrCat(szUpdate, ", ConversionFactorArea = ", sizeof szUpdate);
               WinQueryDlgItemText (hwnd, DID_AREA_CONV, sizeof szCV, szCV);
               if (szCV[0] == '\0')
                  ItiStrCat(szUpdate, " 1.0 ", sizeof szUpdate);
               else
                  ItiStrCat(szUpdate, szCV, sizeof szUpdate);

               ItiStrCat(szUpdate, ", ConversionFactorVolume = ", sizeof szUpdate);
               WinQueryDlgItemText (hwnd, DID_VOL_CONV, sizeof szCV, szCV);
               if (szCV[0] == '\0')
                  ItiStrCat(szUpdate, " 1.0 ", sizeof szUpdate);
               else
                  ItiStrCat(szUpdate, szCV, sizeof szUpdate);

               ItiStrCat(szUpdate, ", UseLength = ", sizeof szUpdate);
               ItiStrCat(szUpdate, pszLen, sizeof szUpdate);
               ItiStrCat(szUpdate, ", UseArea = ", sizeof szUpdate);
               ItiStrCat(szUpdate, pszArea, sizeof szUpdate);
               ItiStrCat(szUpdate, ", UseVolume = ", sizeof szUpdate);
               ItiStrCat(szUpdate, pszVol, sizeof szUpdate);
               ItiStrCat(szUpdate, " WHERE JobKey = ", sizeof szUpdate);
               ItiStrCat(szUpdate, szJobKey, sizeof szUpdate);
               ItiStrCat(szUpdate, " AND JobBreakdownKey = ", sizeof szUpdate);
               ItiStrCat(szUpdate, szJBKey, sizeof szUpdate);
               ItiStrCat(szUpdate, " AND MajorItemKey = ", sizeof szUpdate);
               ItiStrCat(szUpdate, szMIKey, sizeof szUpdate);

               hhp = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
               ItiDbExecSQLStatement (hhp, szUpdate);

               /* -- Recalc the Quantity to use V or A or L */
               //ResetJBMIQty (hhp, szJobKey, szJBKey, szMIKey, 0 , FALSE);

               /* -- Get the JobBreakdown Major Item's changed QUANTITY. */
               WinQueryDlgItemText (hwnd, DID_QUANTITY, sizeof szQty, szQty);
               ItiExtract (szQty, ",");

               mr = ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);

               ParametMajorItemNewQtyPrice 
                 (hheapList2, szMIKey, szJobKey, szJBKey, szQty, " ", TRUE);

              return mr;
               }
               break;

            case DID_CANCEL:   /* CANCEL button pushed */
               WinDismissDlg (hwnd, FALSE);
               return 0;
               break;

            default:
               break;
            }/* end of switch (SHORT1FROMMP... */

      default:
         break;
      }/* end of switch (usMessage... */

   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }/* End of Function JobBreakdownMajorItemEProc */





MRESULT EXPORT JobBreakdownProfileEProc (HWND     hwnd,
                                         USHORT   usMessage,
                                         MPARAM   mp1,
                                         MPARAM   mp2)
   {
   switch (usMessage)
      {
      case WM_QUERYDONE:
         ItiWndActivateRow (hwnd, ParametricProfileL, cProfileKey, NULL, cProfileKey);
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
               {
               HHEAP    hheap;
               MRESULT  mr;
               CHAR     szTemp[516] = "";
               PSZ      pszJobKey = NULL;
               PSZ      pszJobBreakdownKey = NULL;
               HWND   hwndParent, hwndStatic;

               hwndParent = QW(hwnd, ITIWND_OWNERWND, 1, 0, 0);
               hwndStatic = QW(hwndParent, ITIWND_OWNERWND, 1, 0, 0);               //
            
               hheap           = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);

               pszJobKey = (PSZ)   WinSendMsg (hwndParent, WM_ITIWNDQUERY, 
                                   MPFROMSHORT (ITIWND_DATA), 
                                   MPFROM2SHORT (0, cJobKey));

               pszJobBreakdownKey = (PSZ) QW (hwnd, ITIWND_DATA, 0, 0, cJobBreakdownKey);
               if ((pszJobKey == NULL) || (pszJobBreakdownKey == NULL))
                  {
                  sprintf (szTemp,
                     "ERROR: Missing Keys in paramet.c.JobBreakdownProfileEProc.  Given "
                      "JobKey: %s, JobbreakdownKey: %s ", pszJobKey, pszJobBreakdownKey);
                  ItiErrWriteDebugMessage
                     (szTemp);
                  break;
                  }

               mr = ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
            
               /* update the JobBreakdownMajorItem table */
               MakeJobBreakdownMajorItem (hheap, 
                                          pszJobKey, 
                                          pszJobBreakdownKey);
               if (pszJobBreakdownKey)
                  {
                  ItiMemFree (hheap, pszJobBreakdownKey);
                  }

               ItiDbUpdateBuffers ("Job");
               ItiDbUpdateBuffers ("JobBreakdown");

               return mr;
               }
               break;
            }
            break;
      }
   return ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2);
   }









BOOL EXPORT ItiDllInitDll (void)
   {
   HHEAP  hhp;
   USHORT usRet;
   PSZ    pszHPMI = NULL;
   PGLOBALS pglobLoc = NULL;
   CHAR   szChk[750] =
            " /* paramet.c.ItiDllInitDll */ "
            " IF not exists (select ProfileKey "
                           " from SignificantMajorItem "
                           " WHERE ProfileKey=1 AND MajorItemKey=1) "
            " INSERT INTO SignificantMajorItem"
            " (ProfileKey, MajorItemKey, PercentofValue, PercentofValueLock)"
            " VALUES (1,1,0,0) ";


   hhp = ItiMemCreateHeap (1024);
   ItiDbExecSQLStatement (hhp, szChk);  /* check defaults */

   ItiStrCpy (szChk,"/* paramet.c.ItiDllInitDll */ "
            " IF not exists (select ProfileKey"
                           " from ParametricProfile"
                           " WHERE ProfileKey = 1 )"
            " INSERT INTO ParametricProfile "
            " (ProfileKey, ProfileID, Description)"
            " VALUES (1,'Default','System Default') ", sizeof szChk);
   ItiDbExecSQLStatement (hhp, szChk);  /* check defaults */


   ItiStrCpy (szChk,"/* paramet.c.ItiDllInitDll */ "
      " UPDATE JobBreakdownMajorItem SET ConversionFactorLength = 1.0 "
                                 " WHERE ConversionFactorLength = NULL "
      , sizeof szChk);
   ItiDbExecSQLStatement (hhp, szChk);  /* check defaults */

   ItiStrCpy (szChk,"/* paramet.c.ItiDllInitDll */ "
      " UPDATE JobBreakdownMajorItem SET ConversionFactorArea = 1.0 "
                                 " WHERE ConversionFactorArea = NULL "
      , sizeof szChk);
   ItiDbExecSQLStatement (hhp, szChk);  /* check defaults */

   ItiStrCpy (szChk,"/* paramet.c.ItiDllInitDll */ "
      " UPDATE JobBreakdownMajorItem SET ConversionFactorVolume = 1.0 "
                                 " WHERE ConversionFactorVolume = NULL "
      , sizeof szChk);
   ItiDbExecSQLStatement (hhp, szChk);  /* check defaults */

   ItiStrCpy (szChk,"/* paramet.c.ItiDllInitDll */ "
      " UPDATE JobBreakdownMajorItem SET UseLength = 0 "
                                 " WHERE UseLength = NULL "
      , sizeof szChk);
   ItiDbExecSQLStatement (hhp, szChk);  /* check defaults */

   ItiStrCpy (szChk,"/* paramet.c.ItiDllInitDll */ "
      " UPDATE JobBreakdownMajorItem SET UseArea = 0 "
                                 " WHERE UseArea = NULL "
      , sizeof szChk);
   ItiDbExecSQLStatement (hhp, szChk);  /* check defaults */

   ItiStrCpy (szChk,"/* paramet.c.ItiDllInitDll */ "
      " UPDATE JobBreakdownMajorItem SET UseVolume = 0 "
                                 " WHERE UseVolume = NULL "
      , sizeof szChk);
   ItiDbExecSQLStatement (hhp, szChk);  /* check defaults */


   // correct old parametric profile catalog delete oversights.
   ItiStrCpy (szChk,"/* paramet.c.ItiDllInitDll */ "
      " UPDATE JobBreakdownProfile SET ProfileKey = 1 "
      " WHERE ProfileKey NOT IN (select ProfileKey from ParametricProfile) "
      , sizeof szChk);
   ItiDbExecSQLStatement (hhp, szChk);  /* check defaults */

   ItiMemDestroyHeap (hhp);


   pglobLoc = ItiGlobQueryGlobalsPointer();
   DosBeep (1000, 10); 
   ItiWndSetHourGlass (FALSE);
   if (MBID_YES == WinMessageBox (pglobLoc->hwndDesktop, pglobLoc->hwndAppFrame, 
                  "Recalculate Historical Proposal Major Items?"
                  , pglobLoc->pszAppName, 0, 
                  MB_YESNO | MB_MOVEABLE | MB_ICONEXCLAMATION) )
      {
      DosBeep (1000, 10); 
      if (MBID_YES == WinMessageBox (pglobLoc->hwndDesktop, pglobLoc->hwndAppFrame, 
                     "About to recalculate Historical Proposal Major Items.  "
                     "This can take from 15 minutes to an hour or more "
                     "depending on how much data you have, continue?"
                     , pglobLoc->pszAppName, 0, 
                     MB_YESNO | MB_MOVEABLE | MB_ICONEXCLAMATION) )
         {
         ItiWndSetHourGlass (TRUE);

         DosScanEnv ("RESET_HPMI", &pszHPMI);
         if ( (NULL != pszHPMI) && ((*pszHPMI) != '\0') )
            { 
            if (((*pszHPMI) == 'Y') || ((*pszHPMI) == 'y') )
               usRet = ParametResetHistPropMajItms(TRUE);  /* Do updates! */
            else
               usRet = ParametResetHistPropMajItms(FALSE); /* Test only   */
            }/* end of if ( (NULL != pszHPMI... */
         else /* no environment variable so do it. */
            usRet = ParametResetHistPropMajItms(TRUE);  /* Do updates! */

         if (usRet != 0 )
            ItiErrWriteDebugMessage ("*** NOTICE: Somekind of problem occured in ParametResetHistPropMajItms.");

         }/* end of if (MBID_YES..."About...  then clause */
      else
         ItiWndSetHourGlass (TRUE);
      }/* end of if (MBID_YES..."Recalculate... then clause */
   else
      ItiWndSetHourGlass (TRUE);

   if (ItiMbRegisterStaticWnd (JobBreakdownProfileS, JobBreakdownProfileSProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterStaticWnd (ParametricProfileCatS, ParametricProfileCatSProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterStaticWnd (JobBreakdownMajorItemS, JobBreakdownMajorItemSProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterStaticWnd (SignifMajItemS, SignifMajItemSProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterStaticWnd (JobMajItemPriceRegrS, JobMajItemPriceRegrSProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterStaticWnd (JobMajItemQuantityRegrS, JobMajItemQuantityRegrSProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterStaticWnd (MajItemPriceRegrS, MajItemPriceRegrSProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterStaticWnd (MajItemQuantityRegrS, MajItemQuantityRegrSProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (JobBreakdownMajorItemL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (SignifMajItemL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (ParametricProfileCatL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (JobMajItemPriceRegrL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (JobMajItemQuantityRegrL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (MajItemPriceRegrL1, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (MajItemPriceRegrL2, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (MajItemQuantityRegrL1, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (MajItemQuantityRegrL2, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (MajItemEditDL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterListWnd (ParametricProfileL, ItiWndDefListWndTxtProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterDialogWnd (ParametricProfileD, ParametricProfileD, ParametricProfileDProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterDialogWnd (SignifMajItemD, SignifMajItemD, SignifMajItemDProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterDialogWnd (JobBreakdownMajorItemE, JobBreakdownMajorItemE, JobBreakdownMajorItemEProc, hmodMe))
      return FALSE;

   if (ItiMbRegisterDialogWnd (JobBreakdownProfileE, JobBreakdownProfileE, JobBreakdownProfileEProc, hmodMe))
      return FALSE;

   return TRUE;
   }
