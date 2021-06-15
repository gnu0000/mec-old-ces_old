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
 * Kbd.c
 *
 * (C) 1992 Info Tech Inc.
 * This file is part if the EBS module
 */


#define INCL_VIO
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <ctype.h>
#include "ebs.h"
#include "scr.h"
#include "util.h"



CHAR   SZTMP [256];



/********************************************************************/
/*                                                                  */
/*  Low Level Keyboard I/O Routines                                 */
/*                                                                  */
/********************************************************************/

void ClearKeyBuff ()
   {
   while (kbhit ())
      getch ();
   return;
   }



int GetKey (PSZ pszKeyList)
   {
   int   c;

   ClearKeyBuff ();

   while (TRUE)
      {
      c = getch ();
      c = toupper (c);
      if (c == 0x00 || c == 0xE0)
         {
         getch ();
         Beep ();
         continue;
         }
      if (strchr (pszKeyList, c))
         return c;
      Beep ();
      }
   }



/*
 * reads string from kbd
 *
 * returns 0 on escape
 *         1 on enter
 * string from GLOBUFF
 */
BOOL GetString (PSZ *ppsz, PSZ pszOld,
                USHORT y, USHORT x, USHORT uMax)
   {
   USHORT i = 0;
   int    c;

   ShowCursor (TRUE);
   VioSetCurPos (y, x, (HVIO)NULL);

   *ppsz = SZTMP;

   if (*pszOld)
      {
      strcpy (SZTMP, pszOld);
      i = strlen (SZTMP);
      VioWrtCharStr (SZTMP, i, y, x, (HVIO)NULL);
      }
   else
      **ppsz = '\0';

   VioSetCurPos (y, x + i, (HVIO)NULL);
   while (TRUE)
      {
      c = getch ();
      if (c == 0x00 || c == 0xE0)
         {
         getch ();
         continue;
         }

      if (c == '\x1B')
         {
         ShowCursor (FALSE);
         return 0;
         }

      if (c == '\x0D')
         {
         (*ppsz)[i] = '\0';
         ShowCursor (FALSE);
         return 1;
         }
      if (c == '\x08')
         {
         if (!i) 
            continue;

         i--;
         VioWrtNChar (bcSPC, 1, y, x + i, (HVIO)NULL);
         VioSetCurPos (y, x + i, (HVIO)NULL);
         continue;
         }
      if (i >= uMax -1)
         {
         Beep ();
         continue;
         }
      (*ppsz)[i] = (char) c;
      VioWrtNChar (&(char)c, 1, y, x + i, (HVIO)NULL);
      i++;
      VioSetCurPos (y, x + i, (HVIO)NULL);
      }
   }


/* uCmd Options:
 *   0 - clear first
 *   1 - cursor at rt end
 *   2 - cursor at left end
 *
 *   other val assumes 0 with val as first key
 *
 * returns  
 *   the key that was pressed that caused the exit
 *   if an extended key, 0x100 is added
 */

int EditCell (PSZ psz, USHORT x, USHORT y, USHORT uMax, USHORT uCmd,
               PSZ pszNormalReturns, PSZ pszExtendedReturns)
   {
   int      c;
   USHORT   uLen, uPos, i;
   BYTE     cCell[2];

   /*--- clear out the screen space first ---*/
   uLen = 2;
   VioReadCellStr (cCell, &uLen, y, x, (HVIO)NULL);
   cCell[0] = ' ';
   VioWrtNCell (cCell, uMax, y, x, (HVIO)NULL);

   VioSetCurPos (y, x, (HVIO)NULL);
   strcpy (SZTMP, psz);
   *SZTMP = (uCmd==1 || uCmd==2 ? *SZTMP : (CHAR)'\0');
   uLen = strlen (SZTMP);
   uPos = (uCmd==1 ? 0 : uLen);

   VioWrtCharStr (SZTMP, uLen, y, x, (HVIO)NULL);

   while (TRUE)
      {
      VioSetCurPos (y, x + uPos, (HVIO)NULL);
      if (uCmd > 2)
         {
         c = uCmd;
         uCmd = 0;
         }
      else
         c = getch ();

      if (c == 0x00 || c == 0xE0)
         {
         c = getch ();
         switch (c)
            {
            case 75:                           /*--- lt arrow ---*/
               if (uPos)
                  uPos--;
               else
                  Beep();
               break;

            case 77:                           /*--- rt arrow ---*/
               if (uPos < uLen)
                  uPos++;
               else
                  Beep();
               break;
            case 83:                           /*--- del      ---*/
               if (uPos >= uLen)
                  Beep ();
               else
                  {
                  for (i=uPos; i<uLen; i++)
                     SZTMP[i] = SZTMP[i+1];
                  uLen -= !!uLen;
                  VioWrtNCell (cCell, uMax, y, x, (HVIO)NULL);
                  VioWrtCharStr (SZTMP, uLen, y, x, (HVIO)NULL);
                  }
               break;
            case 82:                           /*--- ins      ---*/
               if (uPos >= uMax)
                  Beep ();
               else
                  {
                  for (i=uLen+1; i>uPos; i--)
                     SZTMP[i] = SZTMP[i-1];
                  SZTMP[uPos] = ' ';
                  SZTMP[uMax] = '\0';
                  uLen = min (uMax , uLen+1);
                  VioWrtCharStr (SZTMP, uLen, y, x, (HVIO)NULL);
                  }
               break;

            case 71:                           /*------ home  ------*/
               uPos = 0;
               break;

            case 79:                           /*------ end   ------*/
               uPos = uLen;
               break;

            default :                        /*---          ---*/
               if (strchr (pszExtendedReturns, c))
                  {
                  strcpy (psz, SZTMP);
                  return (0x100 | c);
                  }
               else
                  Beep ();
            }
         }
      else
         {
         switch (c)
            {
            case 8:                           /*--- bksp     ---*/
               if (!uPos)
                  Beep ();
               else
                  {
                  for (i=uPos; i<=uLen; i++)
                     SZTMP[i-1] = SZTMP[i];
                  uLen -= !!uLen;
                  VioWrtNCell (cCell, uMax, y, x, (HVIO)NULL);
                  VioWrtCharStr (SZTMP, uLen, y, x, (HVIO)NULL);
                  uPos--;
                  }
               break;

            case 27:                           /*--- esc      ---*/
               return FALSE;

            default :                        /*---          ---*/
               if (strchr (pszNormalReturns, c))
                  {
                  strcpy (psz, SZTMP);
                  return c;
                  }
               else if (uPos >= uMax)
                     Beep ();
               else
                  {
                  for (i=uLen+1; i>uPos; i--)
                     SZTMP[i] = SZTMP[i-1];
                  SZTMP[uPos] = (CHAR) c;
                  SZTMP[uMax] = '\0';
                  uPos++;
                  uLen = min (uMax , uLen+1);
                  VioWrtCharStr (SZTMP, uLen, y, x, (HVIO)NULL);
                  }
            }
         }
      }
   }


