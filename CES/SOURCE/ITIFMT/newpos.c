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
 * Position.c
 * Mark Hampton
 * Copyright (C) 1991 Info Tech, Inc.
 *
 * This file provides the Position format method.
 */

#include "..\include\iti.h"
#include "..\include\itifmt.h"
#include "..\include\itiutil.h"
#include "..\include\itierror.h"
#include "Position.h"

#define MAX_POSITION_SIZE  16

#define GetDegrees(ulDeg)  ((USHORT) ((ulDeg) >> 16))
#define GetMinutes(ulDeg) (((USHORT) ((ulDeg) >>  8)) & 0xffU)
#define GetSeconds(ulDeg) (((USHORT) ((ulDeg)      )) & 0xffU)

static USHORT CreateOutput (PSZ     pszDest, 
                            PSZ     pszFormatInfo, 
                            ULONG   ulLong, 
                            ULONG   ulLat, 
                            USHORT  usDestMax);

/*
 * pszFormatInfo = String LongLat String .
 *
 * LongLat = Longitude | Latitude .
 *
 * Longitude = '<long>' Format .
 *
 * Latitude = '<lat>' Format .
 *
 * Format = String '<deg>' String '<min>' String ['<sec>' String]
 */


USHORT EXPORT FmtPosition (PSZ     pszSource,
                           PSZ     pszDest,
                           USHORT  usDestMax,
                           PSZ     pszFormatInfo,
                           PUSHORT pusDestLength)
   {
   char szLong [MAX_POSITION_SIZE];
   char szLat [MAX_POSITION_SIZE];
   char szTemp [MAX_POSITION_SIZE];
   ULONG ulLong, ulLat;
   USHORT usLen;

   /* split up number from SQL Server into longitude and latitude */
   ItiStrNCpy (szLong, pszSource, 9);
   ItiStrNCpy (szLat,  pszSource + 8, 9);

   if (!ItiStrToULONG (szLong, &ulLong))
      return ITIFMT_BADDATA;

   if (!ItiStrToULONG (szLat, &ulLat))
      return ITIFMT_BADDATA;


   /* create output */
   usLen = CreateOutput (pszDest, pszFormatInfo, ulLong, ulLat, usDestMax);

   if (pusDestLength)
      *pusDestLength = usLen;

   return 0;
   }




static PSZ apszTokens [] = 
   {
   "<long>", "<lat>", "<deg>", "<min>", "<sec>"
   }


enum eTokens 
   {
   eLong = 1, eLat, eDeg, eMin, eSec
   };



static USHORT GetToken (PSZ pszString, PUSHORT pusLen)
   {
   USHORT i;

   for (i=0; i < (sizeof apszTokens / sizeof (PSZ)); i++)
      {
      *pusLen = ItiStrLen (apszTokens [i]);
      if (ItiStrNICmp (pszString, apszTokens [i], *pusLen) == 0)
         return i + 1;
      }
   *pusLen = 0;
   return 0;
   }


static USHORT CreateOutput (PSZ     pszDest, 
                            PSZ     pszFormatInfo, 
                            ULONG   ulLong, 
                            ULONG   ulLat, 
                            USHORT  usDestMax)
   {
   USHORT usLen;
   USHORT usTemp;
   BOOL   bLongitude;

   bLongitude = FALSE;

   usLen = 0;
   while (*pszFormatInfo && usDestMax > 1)
      {
      switch (*pszFormatInfo)
         {
         case '<':
            {
            if (*(pszFormat + 1) == '<')
               {
               /* translate "<<" into "<" in the output stream */
               *pszDest++ = *pszFormatInfo++;
               usDestMax--;
               usLen++;
               break;
               break;
               }

            switch (GetToken (pszFormatInfo, &usTemp))
               {
               case eLong:
                  pszFormatInfo += usTemp;
                  bLongitude = TRUE;
                  break;

               case eLat:
                  pszFormatInfo += usTemp;
                  bLongitude = FALSE;
                  break;

               case eDeg:
                  pszFormatInfo += usTemp;
                  break;

               case eMin:
                  pszFormatInfo += usTemp;
                  break;

               case eSec:
                  pszFormatInfo += usTemp;
                  break;

               default:
                  ItiStrCat (pszDest, "Bad format string", usDestMax);
                  return ItiStrLen (pszDest);
                  break;
               }
            }
            break;

         default:
            *pszDest++ = *pszFormatInfo++;
            usDestMax--;
            usLen++;
            break;
         }
      }
   *pszDest = '\0';
   return usLen;
   }




USHORT EXPORT CheckPosition (PSZ     pszSource,
                             PSZ     pszDest,
                             USHORT  usDestMax,
                             PSZ     pszFormatInfo,
                             PUSHORT pusDestLength)
   {
   ItiStrCpy (pszDest, "0x", usDestMax);
   ItiStrCat (pszDest, pszSource, usDestMax);
   if (pusDestLength)
      *pusDestLength = ItiStrLen (pszDest);
   return 0;
   }

