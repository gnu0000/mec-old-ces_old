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
 * JobCom.c
 * Mark Hampton
 * (C) 1991 AASHTO
 *
 * This module handles the Job window.
 */


#define INCL_WIN
#include "..\include\iti.h"
#include "..\include\itiwin.h"
#include "..\include\itimbase.h"
#include "..\include\itidbase.h"
#include "..\include\itimenu.h"
#include "..\include\winid.h"
#include "..\include\itibase.h"
#include "..\include\itiutil.h"
#include "..\include\colid.h"
#include <stdio.h>
#include <string.h>

// #include "..\include\itiglob.h"
// #include "..\include\dialog.h"
#include "bamsces.h"
#include "jobcom.h"
#include "menu.h"
#include "dialog.h"



/*
 * Window procedure for JobComment window.
 */



MRESULT EXPORT JobCommentSProc (HWND     hwnd,
                                  USHORT   usMessage,
                                  MPARAM   mp1,
                                  MPARAM   mp2)
   {
   switch (usMessage)
      {
      case WM_INITMENU:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_EDIT_MENU:
               ItiMenuGrayMenuItem (HWNDFROMMP (mp2), IDM_ADD, FALSE);
               return 0;
            }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case IDM_ADD:
               ItiWndDoDialog (hwnd, JobCommentL, TRUE);
               break;
            }
         break;
      }
   return ItiWndDefStaticWndProc (hwnd, usMessage, mp1, mp2);
   }





static BOOL CreateChildRelations (HHEAP   hheap, 
                                  PSZ     pszLongTextKey, 
                                  PSZ     pszLongText);




MRESULT EXPORT JobCommentDProc (HWND    hwnd,
                                   USHORT   usMessage,
                                   MPARAM   mp1,
                                   MPARAM   mp2)
   {
   static PSZ pszKey;

   switch(usMessage)
      {
      case WM_INITDLG:
         {
         HHEAP hhp;
         char szTemp[512] = "";

         hhp = ItiMemCreateHeap (2048);

         sprintf(szTemp, 
            "if ((select KeyValue from KeyMaster where TableName='JobComment') < "
               " (select KeyValue from KeyMaster where TableName='LongText'))"
            " UPDATE KeyMaster"
            " SET KeyValue= (select KeyValue from KeyMaster where TableName='LongText')"
            " WHERE TableName='JobComment' " );
         ItiDbExecSQLStatement (hhp, szTemp);

         ItiMemDestroyHeap (hhp);
         }
         break;

      case WM_NEWKEY:
         {
         HHEAP hheap;
         PSZ pszNotUsed;

         if (ItiStrICmp((PSZ) mp1, "JobComment") == 0)
            {
            hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
            pszKey = ItiStrDup(hheap, (PSZ) mp2);

            /* -- The Update of the JobComment table also  JAN 94
               -- needs to update the LongText table!     */
            pszNotUsed = ItiDbGetKey (hheap, "LongText", 1L);
            }

         return 0;
         }
         break;

      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
            {
            case DID_OK:
               {
               MRESULT mr;
               HHEAP hheap;
               char szTemp [500] = ""; // Text could be long...

               hheap = (HHEAP) QW (hwnd, ITIWND_HEAP, 0, 0, 0);
               pszKey = NULL;
               WinQueryDlgItemText (hwnd, DID_COMMENT, sizeof szTemp, szTemp);

               if (mr = ItiWndDefDlgProc (hwnd, usMessage, mp1, mp2))
                   return mr;

               if (pszKey != NULL)
                  {
                  CreateChildRelations (hheap, pszKey, szTemp);
                  ItiMemFree (hheap, pszKey);
                  }
               //
               ItiDbUpdateBuffers ("JobComment");
               return mr;
               }
            break;
            }
         break;
      }
   return ItiWndDefDlgProc(hwnd, usMessage, mp1, mp2);
   }







static BOOL CreateChildRelations (HHEAP   hheap,
                                  PSZ     pszLongTextKey,
                                  PSZ     pszLongText)
   {
   char szTemp   [500] = ""; // Text could be long...
   PSZ  pszTemp;                                       // csp 21sep94
   int  i, j;                                          // csp 21sep94
   BOOL bNumber;

/* csp 21sep94 BSR 930147 - Single quotes (0x27) in the Long Text column
   were causing an error on insert. Single quotes must be converted to
   double quotes.
*/
   ItiMemSet(szTemp, '\0', sizeof(szTemp));               
   pszTemp = pszLongText;                                 
   j = 0;                                                 
   bNumber = FALSE;
                                                          
   for (i = 0 ; i < (int)ItiStrLen(pszLongText) ; i++)    
      {                                                   
      ItiMemSet((szTemp + j), *pszTemp, 1);               
      if (*pszTemp == 0x27)                               
         {                                                
         if (bNumber)
            {
            if (*(pszTemp + 1) == 0x27)
               {
               ItiStrCpy((szTemp + j), "in", 3);
               ++j;
               ++pszTemp;
               }
            else
               {
               ItiStrCpy((szTemp + j), "ft", 3);
               ++j;
               }
            }
         else
            {                                      
            ++j;                                   
            ItiMemSet((szTemp + j), 0x27, 1);           
            }
         bNumber = FALSE;
         }                                           
      else
         if (!ItiCharIsSpace(*pszTemp))
            bNumber = ItiCharIsDigit(*pszTemp);
      ++j;                                            
      ++pszTemp;                                      
      }                                               
                                                      
   ItiStrCpy(pszLongText, szTemp, (ItiStrLen (szTemp) + 1));               

   sprintf(szTemp, "INSERT INTO LongText "
                   "(LongTextKey, ShortText) "
 //                    "(LongTextKey, LongText) "
               "VALUES "
               "(%s, '%s') ",pszLongTextKey, pszLongText);

 //    sprintf(szTemp, 
 //       " declare @val varbinary(16)"
 //       " select @val = textptr(LongText) from LongText where LongTextKey = %s "
 //       " WRITETEXT LongText.LongText @val '%s' "
 //       , pszLongTextKey, pszLongText);
   
   return 0 == ItiDbExecSQLStatement (hheap, szTemp);
   }

