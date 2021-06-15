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
#include <stdio.h>

#define MAX_POSITION_SIZE  16

#define GetDegrees(ulPos)  ((SHORT)  ((ulPos) >> 16))
#define GetMinutes(ulPos) (((USHORT) ((ulPos) >>  8)) & 0xffU)
#define GetSeconds(ulPos) (((USHORT) ((ulPos)      )) & 0xffU)

#define MakeDegrees(sDeg) ((ULONG) (sDeg) << 16)
#define MakeMinutes(usMin) ((ULONG) (usMin & 0xff) << 8)
#define MakeSeconds(usSec) ((ULONG) (usSec & 0xff))
#define MakePosition(sDeg, usMin, usSec)  (( (ULONG) (sDeg)   << 16) | \
                                           (((usMin) & 0xffU) <<  8) | \
                                           ( (usSec) & 0xffU       ))

static USHORT CreateOutput (PSZ     pszDest, 
                            PSZ     pszFormatInfo, 
                            ULONG   ulLong, 
                            ULONG   ulLat, 
                            USHORT  usDestMax);

/*
 * pszFormatInfo = String LongLat LongLat .
 *
 * LongLat = Longitude | Latitude | .
 *
 * Longitude = '<long>' Format .
 *
 * Latitude = '<lat>' Format .
 *
 * Format = Decimal | DMS 
 *
 * Decimal = String '<decimal>'
 *
 * DMS = String ['<deg>' String ['<min>' String ['<sec>' String] ] ]
 *
 * String = any set of characters, with the exception that any '<'s that
 * are to be part of the string are to be doubled.  If the '<' is part of
 * a token do not double them.  A string may also be empty.
 */

USHORT EXPORT FmtPosition (PSZ     pszSource,
                           PSZ     pszDest,
                           USHORT  usDestMax,
                           PSZ     pszFormatInfo,
                           PUSHORT pusDestLength)
   {
   char szLong [MAX_POSITION_SIZE];
   char szLat [MAX_POSITION_SIZE];
   ULONG ulLong, ulLat;
   USHORT usLen;

   if (*pszSource == '0' && ItiCharLower (*(pszSource + 1)) == 'x')
      pszSource += 2;

   /* split up number from SQL Server into longitude and latitude */
   ItiStrCpy (szLong, pszSource, 9);
   ItiStrCpy (szLat,  pszSource + 8, 9);

   if (!ItiStrHexToULONG (szLong, &ulLong))
      return ITIFMT_BAD_DATA;

   if (!ItiStrHexToULONG (szLat, &ulLat))
      return ITIFMT_BAD_DATA;

   /* create output */
   usLen = CreateOutput (pszDest, pszFormatInfo, ulLong, ulLat, usDestMax);

   if (pusDestLength)
      *pusDestLength = usLen;

   return 0;
   }




static PSZ apszTokens [] = 
   {
   "<long>", "<lat>", "<deg>", "<min>", "<sec>", "<decimal>",
   "<DDDMMSS>", "<DDMMSS>", "<lSeconds>"
   };


typedef enum 
   {
   eLong = 1,  eLat,    eDeg,    eMin,    eSec,    eDecimal,
   eDDDMMSS, eDDMMSS, eLongSeconds
   } TOKEN;



static TOKEN GetToken (PSZ pszString, PUSHORT pusLen)
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
   USHORT usTokenSize;
   BOOL   bLong;
   USHORT usTemp, usToken;
   DOUBLE dPosition;
   LONG   lPosition;
   char   szTemp [50];

   bLong = FALSE;

   usLen = 0;
   while (*pszFormatInfo && usDestMax > 1)
      {
      switch (*pszFormatInfo)
         {
         case '<':
            {
            if (*(pszFormatInfo + 1) == '<')
               {
               /* translate "<<" into "<" in the output stream */
               *pszDest++ = *pszFormatInfo++;
               usDestMax--;
               usLen++;
               break;
               }

            switch (usToken = GetToken (pszFormatInfo, &usTokenSize))
               {
               case eLong:
                  pszFormatInfo += usTokenSize;
                  bLong = TRUE;
                  break;

               case eLat:
                  pszFormatInfo += usTokenSize;
                  bLong = FALSE;
                  break;

               case eDeg:
                  pszFormatInfo += usTokenSize;
                  usTemp = GetDegrees (bLong ? ulLong : ulLat);
                  usTemp = ItiStrSHORTToString (usTemp, pszDest, usDestMax);
                  pszDest += usTemp;
                  usLen += usTemp;
                  usDestMax -= usTemp;
                  break;

               case eMin:
                  pszFormatInfo += usTokenSize;
                  usTemp = GetMinutes (bLong ? ulLong : ulLat);
                  usTemp = ItiStrSHORTToString (usTemp, pszDest, usDestMax);
                  pszDest += usTemp;
                  usLen += usTemp;
                  usDestMax -= usTemp;
                  break;

               case eSec:
                  pszFormatInfo += usTokenSize;
                  usTemp = GetSeconds (bLong ? ulLong : ulLat);
                  usTemp = ItiStrSHORTToString (usTemp, pszDest, usDestMax);
                  pszDest += usTemp;
                  usLen += usTemp;
                  usDestMax -= usTemp;
                  break;

               case eDecimal:
                  pszFormatInfo += usTokenSize;
                  dPosition = (DOUBLE) GetDegrees (bLong ? ulLong : ulLat) + 
                              (DOUBLE) GetMinutes (bLong ? ulLong : ulLat) / 60.0 + 
                              (DOUBLE) GetSeconds (bLong ? ulLong : ulLat) / 3600.0;
                  usTemp = sprintf (szTemp, "%lf", dPosition);
                  ItiStrCpy (pszDest, szTemp, usDestMax);
                  pszDest += usTemp;
                  usLen += usTemp;
                  usDestMax -= usTemp;
                  break;

               case eDDDMMSS:
               case eDDMMSS:
                  pszFormatInfo += usTokenSize;
                  usTemp = GetDegrees (bLong ? ulLong : ulLat);
                  if (usToken == eDDDMMSS)
                     usTemp = sprintf (szTemp, "%.3d", usTemp);
                  else
                     usTemp = sprintf (szTemp, "%.2d", usTemp);
                  ItiStrCpy (pszDest, szTemp, usDestMax);
                  pszDest += usTemp;
                  usLen += usTemp;
                  usDestMax -= usTemp;

                  usTemp = GetMinutes (bLong ? ulLong : ulLat);
                  usTemp = sprintf (szTemp, "%.2d", usTemp);
                  ItiStrCpy (pszDest, szTemp, usDestMax);
                  pszDest += usTemp;
                  usLen += usTemp;
                  usDestMax -= usTemp;

                  usTemp = GetSeconds (bLong ? ulLong : ulLat);
                  usTemp = sprintf (szTemp, "%.2d", usTemp);
                  ItiStrCpy (pszDest, szTemp, usDestMax);
                  pszDest += usTemp;
                  usLen += usTemp;
                  usDestMax -= usTemp;
                  break;

               case eLongSeconds:
                  pszFormatInfo += usTokenSize;
                  lPosition  = (LONG) GetDegrees (bLong ? ulLong : ulLat) * 3600L;
                  lPosition += (LONG) GetMinutes (bLong ? ulLong : ulLat) * 60L;
                  lPosition += (LONG) GetSeconds (bLong ? ulLong : ulLat);
                  usTemp = ItiStrLONGToString (lPosition, pszDest, usDestMax);
                  pszDest += usTemp;
                  usLen += usTemp;
                  usDestMax -= usTemp;
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


/*
 *
 * Input     = Position Position | SpecLong SpecLat | SpecLat SpecLong .
 * SpecLong  = Separator 'Longitude' Position | .
 * SpecLat   = Separator 'Latitude' Position | .
 * Position  = DMS | Decimal .
 * DMS       = Deg SpecDeg [Min SpecMin [Sec SpecSec]] .
 * Deg       = Separator Number from -360 to 360 Separator .
 * SpecDeg   = 'ø' | 'degrees' | 'D' | .
 * Min       = Separator Number from 0 to 59 Separator .
 * SpecMin   = ''' | 'minutes' | 'M' | .
 * Sec       = Separator Number from 0 to 59 Separator .
 * SpecSec   = '"' | 'seconds' | 'S' | .
 * Decimal   = Separator Sign Separator Digits '.' Digits .
 * Sign      = '+' | '-' | .
 * Digits    = '0' thru '9' .
 * Separator = ' ' | ',' | ';' | '\t' | '\n' | '\r' | .
 *
 * Note: The input is assumed to be in the order of Longitude Latitude, 
 * unless otherwise specified.
 */

#define ISSEPARATOR(c) (ItiStrIsCharInString ((c), " ,;\t\n\r"))

static BOOL PartialICmp (PSZ pszSource, PSZ pszPartial, PUSHORT pusLen);

static BOOL ParseInput (PSZ     *ppszSource, 
                        PSZ     pszFormatInfo,
                        PULONG  pulLong,
                        PULONG  pulLat);


/*
 * FormatInfo = '#' | whatever else .
 *
 * A '#' tells the check function to not quote the string.
 */


USHORT EXPORT CheckPosition (PSZ     pszSource,
                             PSZ     pszDest,
                             USHORT  usDestMax,
                             PSZ     pszFormatInfo,
                             PUSHORT pusDestLength)
   {
   ULONG ulLong, ulLat;
   USHORT usLen, usTemp;

   if (usDestMax < 2)
      return ITIFMT_DEST_TOO_SMALL;

   ulLong = 0;
   ulLat = 0;
   if (!ParseInput (&pszSource, pszFormatInfo, &ulLong, &ulLat))
      return ITIFMT_BAD_DATA;

   if (pszFormatInfo == NULL || *pszFormatInfo != '#')
      {
      ItiStrCpy (pszDest, "0x", usDestMax);
      usLen = 2;
      }
   else
      {
      *pszDest = '\0';
      usLen = 0;
      }

   pszDest += usLen;
   usDestMax -= usLen;

   usTemp = ItiStrULONGToHexString (ulLong, pszDest, usDestMax);
   usLen += usTemp;
   pszDest += usTemp;
   usDestMax -= usTemp;

   usTemp = ItiStrULONGToHexString (ulLat, pszDest, usDestMax);
   usLen += usTemp;
   pszDest += usTemp;
   usDestMax -= usTemp;

   if (pusDestLength)
      *pusDestLength = usLen;
   return 0;
   }



static BOOL ParsePosition (PSZ     *ppszSource, 
                           PSZ     pszFormatInfo,
                           PULONG  pulPosition);

static BOOL ParseSpecLong (PSZ     *ppszSource, 
                           PSZ     pszFormatInfo,
                           PULONG  pulLong,
                           PULONG  pulLat);

static BOOL ParseSpecLat (PSZ     *ppszSource, 
                          PSZ     pszFormatInfo, 
                          PULONG  pulLong, 
                          PULONG  pulLat); 



static BOOL ParseInput (PSZ     *ppszSource, 
                        PSZ     pszFormatInfo,
                        PULONG  pulLong,
                        PULONG  pulLat)
   {
   BOOL bLong = FALSE;
   BOOL bLat  = FALSE;

   if (!ParseSpecLat (ppszSource, pszFormatInfo, pulLong, pulLat))
      if (!ParseSpecLong (ppszSource, pszFormatInfo, pulLong, pulLat))
         {
         bLong = ParsePosition (ppszSource, pszFormatInfo, pulLong);
         bLat =  ParsePosition (ppszSource, pszFormatInfo, pulLat);
         return bLong && bLat;
         }
      else
         return ParseSpecLat (ppszSource, pszFormatInfo, pulLong, pulLat);
   else
      if (!ParseSpecLong (ppszSource, pszFormatInfo, pulLong, pulLat))
         {
         bLong = ParsePosition (ppszSource, pszFormatInfo, pulLong);
         bLat =  ParsePosition (ppszSource, pszFormatInfo, pulLat);
         return bLong && bLat;
         }
      else
         return TRUE;
   }


static BOOL ParseSeparator (PSZ     *ppszSource, 
                            PSZ     pszFormatInfo);

static BOOL ParseDecimal (PSZ     *ppszSource, 
                          PSZ     pszFormatInfo,
                          PULONG  pulPosition);

static BOOL ParseDMS (PSZ     *ppszSource, 
                      PSZ     pszFormatInfo,
                      PULONG  pulPosition);



static BOOL ParsePosition (PSZ     *ppszSource, 
                           PSZ     pszFormatInfo,
                           PULONG  pulPosition)
   {
   ParseSeparator (ppszSource, pszFormatInfo);

   if (!ParseDecimal (ppszSource, pszFormatInfo, pulPosition))
      return ParseDMS (ppszSource, pszFormatInfo, pulPosition);
   return TRUE;
   }



static BOOL ParseSpecLong (PSZ     *ppszSource, 
                           PSZ     pszFormatInfo,
                           PULONG  pulLong,
                           PULONG  pulLat)
   {
   USHORT usLen;

   ParseSeparator (ppszSource, pszFormatInfo);

   if (PartialICmp (*ppszSource, "longitude", &usLen) && usLen > 1)
      {
      *ppszSource += usLen;
      return ParsePosition (ppszSource, pszFormatInfo, pulLong);
      }

   return FALSE;
   }



static BOOL ParseSpecLat (PSZ     *ppszSource, 
                          PSZ     pszFormatInfo,
                          PULONG  pulLong,
                          PULONG  pulLat)
   {
   USHORT usLen;

   ParseSeparator (ppszSource, pszFormatInfo);

   if (PartialICmp (*ppszSource, "latitude", &usLen) && usLen > 1)
      {
      *ppszSource += usLen;
      return ParsePosition (ppszSource, pszFormatInfo, pulLat);
      }

   return FALSE;
   }



static BOOL ParseDecimal (PSZ     *ppszSource, 
                          PSZ     pszFormatInfo,
                          PULONG  pulPosition)
   {
   PSZ pszSourceCopy;
   PSZ pszDecimal;
   PSZ pszDegrees;
   PSZ pszEnd;
   SHORT sSign;
   SHORT sDegrees;
   USHORT usMinutes, usSeconds;
   DOUBLE dPosition;

   pszSourceCopy = *ppszSource;
   ParseSeparator (&pszSourceCopy, pszFormatInfo);

   sSign = 1;
   switch (*pszSourceCopy)
      {
      case '+':
         pszSourceCopy += 1;
         break;

      case '-':
         pszSourceCopy += 1;
         sSign = -1;
         break;
      }
   ParseSeparator (&pszSourceCopy, pszFormatInfo);

   /* set up our pointers */
   pszDegrees = pszSourceCopy;
   pszDecimal = NULL;
   for (pszEnd = pszSourceCopy; 
        *pszEnd && (ItiCharIsDigit (*pszEnd) || *pszEnd == '.') && 
        !ISSEPARATOR (*pszEnd); 
        pszEnd++)
      {
      if (*pszEnd == '.')
         if (pszDecimal == NULL)
            pszDecimal = pszEnd;
         else
            return FALSE;  /* is this correct? or do I break? */
      }

   if (pszDecimal == NULL)
      return FALSE;

   sscanf (pszDegrees, "%lf", &dPosition);
   sDegrees = ((SHORT) (dPosition)) * sSign;
   dPosition = (dPosition - sDegrees) * 60;
   usMinutes = (USHORT) dPosition;
   dPosition = (dPosition - usMinutes) * 60;
//   /* do I always want to round only this one up? */
//   usSeconds = (USHORT) (dPosition + 0.5);
   usSeconds = (USHORT) (dPosition);

   /* -- Adjustments for "rounding" */
   if (usSeconds > 59)
      {
      usMinutes++;
      usSeconds = 0;
      }
   if (usMinutes > 59)
      {
      sDegrees++;
      usMinutes = 0;
      }

   if (sDegrees > 360 || sDegrees < -360 ||
       usMinutes > 59 || usSeconds > 59)
      return FALSE;

   *pulPosition = MakePosition (sDegrees, usMinutes, usSeconds);
   *ppszSource = pszEnd;

   return TRUE;
   }



static BOOL ParseSpecDeg (PSZ *ppszSource);
static BOOL ParseSpecMin (PSZ *ppszSource);
static BOOL ParseSpecSec (PSZ *ppszSource);
static BOOL ParseDeg (PSZ     *ppszSource, 
                      PSZ     pszFormatInfo,
                      PULONG  pulPosition);
static BOOL ParseMin (PSZ     *ppszSource, 
                      PSZ     pszFormatInfo,
                      PULONG  pulPosition);
static BOOL ParseSec (PSZ     *ppszSource, 
                      PSZ     pszFormatInfo,
                      PULONG  pulPosition);


/* 4/7/93 mdh: added code to check for DDDMMSS, DDMMSS  BSR# 930406-4601 */
static BOOL DoDMS (PPSZ    ppszSource,
                   PULONG  pulPosition)
   {
   PSZ pszStart, pszEnd;
   USHORT i, j;
   char szDeg [4], szMin [3], szSec [3];

   pszEnd = pszStart = *ppszSource;
   while (*pszEnd && ItiCharIsDigit (*pszEnd))
      pszEnd++;
   switch (pszEnd - pszStart)
      {
      case 5: /*   DMMSS */
         i = 1;
         break;

      case 6: /*  DDMMSS */
         i = 2;
         break;

      case 7: /* DDDMMSS */
         i = 3;
         break;

      default:
         return FALSE;
      }
   ItiMemCpy (szDeg, pszStart, i);
   ItiMemCpy (szMin, pszStart + i, 2);
   ItiMemCpy (szSec, pszStart + i + 2, 2);
   szDeg [i] = '\0';
   szMin [2] = '\0';
   szSec [2] = '\0';
   if (!ItiStrToUSHORT (szDeg, &j))
      return FALSE;
   *pulPosition |= MakeDegrees (j);
   if (!ItiStrToUSHORT (szMin, &j))
      return FALSE;
   *pulPosition |= MakeMinutes (j);
   if (!ItiStrToUSHORT (szSec, &j))
      return FALSE;
   *pulPosition |= MakeSeconds (j);

   *ppszSource = pszEnd + 1;
   return TRUE;
   }

static BOOL ParseDMS (PSZ     *ppszSource, 
                      PSZ     pszFormatInfo,
                      PULONG  pulPosition)
   {
   *pulPosition = 0;

   /* 4/7/93 mdh: added code to check for DDDMMSS, DDMMSS BSR# 930406-4601 */
   if (DoDMS (ppszSource, pulPosition))
      return TRUE;

   if (ParseDeg (ppszSource, pszFormatInfo, pulPosition))
      {
      ParseSpecDeg (ppszSource);
      if (ParseMin (ppszSource, pszFormatInfo, pulPosition))
         {
         ParseSpecMin (ppszSource);
         if (ParseSec (ppszSource, pszFormatInfo, pulPosition))
            {
            ParseSpecSec (ppszSource);
            }
         }
      return TRUE;
      }
   return FALSE;
   }




static BOOL ParseDeg (PSZ     *ppszSource, 
                      PSZ     pszFormatInfo,
                      PULONG  pulPosition)
   {
   PSZ   pszSourceCopy;
   SHORT sDeg;

   pszSourceCopy = *ppszSource;
   ParseSeparator (&pszSourceCopy, pszFormatInfo);

   if (!ItiStrToSHORT (pszSourceCopy, &sDeg))
      return FALSE;

   if (sDeg > 360 || sDeg < -360)
      return FALSE;

   if (*pszSourceCopy == '-' || *pszSourceCopy == '+')
      pszSourceCopy++;

   while (ItiCharIsDigit (*pszSourceCopy))
      pszSourceCopy++;

   *pulPosition |= MakeDegrees (sDeg);

   ParseSeparator (&pszSourceCopy, pszFormatInfo);
   *ppszSource = pszSourceCopy;
   return TRUE;
   }


static BOOL ParseSpecDeg (PSZ *ppszSource)
   {
   USHORT usLen;

   if (**ppszSource == 'ø')
      {
      *ppszSource += 1;
      return TRUE;
      }
   if (PartialICmp (*ppszSource, "degrees", &usLen))
      {
      *ppszSource += usLen;
      return TRUE;
      }
   return TRUE;
   }





static BOOL ParseMin (PSZ     *ppszSource, 
                      PSZ     pszFormatInfo,
                      PULONG  pulPosition)
   {
   PSZ    pszSourceCopy;
   USHORT usMin;

   pszSourceCopy = *ppszSource;
   ParseSeparator (&pszSourceCopy, pszFormatInfo);

   if (!ItiStrToUSHORT (pszSourceCopy, &usMin))
      return FALSE;

   if (usMin > 59)
      return FALSE;

   if (*pszSourceCopy == '-' || *pszSourceCopy == '+')
      pszSourceCopy++;

   while (ItiCharIsDigit (*pszSourceCopy))
      pszSourceCopy++;

   *pulPosition |= MakeMinutes (usMin);

   ParseSeparator (&pszSourceCopy, pszFormatInfo);
   *ppszSource = pszSourceCopy;
   return TRUE;
   }


static BOOL ParseSpecMin (PSZ *ppszSource)
   {
   USHORT usLen;

   if (**ppszSource == '\'')
      {
      *ppszSource += 1;
      return TRUE;
      }
   if (PartialICmp (*ppszSource, "minutes", &usLen))
      {
      *ppszSource += usLen;
      return TRUE;
      }
   return TRUE;
   }

   



static BOOL ParseSec (PSZ     *ppszSource, 
                      PSZ     pszFormatInfo,
                      PULONG  pulPosition)
   {
   PSZ    pszSourceCopy;
   USHORT usSec;

   pszSourceCopy = *ppszSource;
   ParseSeparator (&pszSourceCopy, pszFormatInfo);

   if (!ItiStrToUSHORT (pszSourceCopy, &usSec))
      return FALSE;

   if (usSec > 59)
      return FALSE;

   if (*pszSourceCopy == '-' || *pszSourceCopy == '+')
      pszSourceCopy++;

   while (ItiCharIsDigit (*pszSourceCopy))
      pszSourceCopy++;

   *pulPosition |= MakeSeconds (usSec);

   ParseSeparator (&pszSourceCopy, pszFormatInfo);
   *ppszSource = pszSourceCopy;
   return TRUE;
   }


static BOOL ParseSpecSec (PSZ *ppszSource)
   {
   USHORT usLen;

   if (**ppszSource == '"')
      {
      *ppszSource += 1;
      return TRUE;
      }
   if (PartialICmp (*ppszSource, "seconds", &usLen))
      {
      *ppszSource += usLen;
      return TRUE;
      }
   return TRUE;
   }




static BOOL ParseSeparator (PSZ     *ppszSource, 
                            PSZ     pszFormatInfo)
   {
   while (**ppszSource != '\0' && ISSEPARATOR (**ppszSource))
      {
      *ppszSource += 1;
      }
   return TRUE;
   }






static BOOL PartialICmp (PSZ pszSource, PSZ pszPartial, PUSHORT pusLen)
   {
   *pusLen = 0;
   while (*pszSource && *pszPartial && 
          ItiCharLower (*pszPartial) == ItiCharLower (*pszSource))
      {
      pszPartial++;
      pszSource++;
      *pusLen += 1;
      }
   return *pusLen != 0;
   }

