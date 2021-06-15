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
 * Date.c
 * Mark Hampton
 * Charles Engelke
 * Timothy Blake
 * Copyright (C) 1991 Info Tech, Inc.
 * 
 * This file provides the DateTime and Month format method.
 * Parts of this code have been adapted from HighEst.
 *
 */

#include "..\include\iti.h"
#include "..\include\itifmt.h"
#include "..\include\itiutil.h"
#include "..\include\itierror.h"
#include "date.h"

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>


/* -- Basic BNF symbol characters used for the DateTime format: */
#define  BNF_HOUR            'h'
#define  BNF_MINUTE          'n'
#define  BNF_SECOND          's'
#define  BNF_AMPM            'A'
#define  BNF_pmam            'a'
#define  BNF_YEAR            'y'
#define  BNF_MONTH           'm'
#define  BNF_DAY             'd'
#define  BNF_TH              't'



#if !defined (min)
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

#if !defined (max)
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif



static PSZ apszMonths [13] =
   {
   "Not Available", "January", "February", "March", "April", "May", "June", 
   "July", "August", "September", "October", "November", "December"
   };


static PSZ apszDays [8] =
   {
   "Not Available", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
   "Friday", "Saturday"
   };






static BOOL ParseDate (PSZ       *ppszDate, 
                       PUSHORT   pusYear, 
                       PUSHORT   pusMonth, 
                       PUSHORT   pusDay);

static BOOL ParseTime (PSZ       *ppszTime,
                       PUSHORT   pusHour,
                       PUSHORT   pusMinute,
                       PUSHORT   pusSecond);

static BOOL ConvertToMonth (PSZ pszString, USHORT *pusMonth);

static USHORT DaysInMonth (USHORT usMonth, USHORT usYear);

static BOOL IsValidDay (USHORT usMonth, USHORT usDay, USHORT usYear);

static BOOL ConvertToNumber (PSZ pszString, PUSHORT pusResult);


static USHORT ConvertToDayOfYr (USHORT usYear, USHORT usMonth, USHORT usDay);

static USHORT YearOffset (USHORT usYr);

static PSZ DayOfWeek(USHORT usYear, USHORT usMonth, USHORT usDay);









USHORT EXPORT FmtMonth (PSZ     pszSource,
                        PSZ     pszDest,
                        USHORT  usDestMax,
                        PSZ     pszFormatInfo,
                        PUSHORT pusDestLength)
   {
   USHORT usYear, usMonth, usDay;
   USHORT usValue = 0;
   USHORT usCount;

   ParseDate (&pszSource, &usYear, &usMonth, &usDay);
   usValue = usMonth;

   for (usCount = 0; *pszFormatInfo == BNF_MONTH; pszFormatInfo++)
      usCount++;

   /* make sure there's enought room in the destination buffer */
   if (usDestMax <= (usCount + 1))
      {
      if (usDestMax > 0)
         *pszDest = '\0';
      return ITIFMT_TRUNCATED;
      }

   *pszDest = '\0';
   switch (usCount)
      {
      case 0:
//
//         added 9/25/92 mdh: this code allows the format
//                            "Month,d", which translates
//                            to the number of days in the
//                            month for the given year.  If
//                            no year is present in the string,
//                            a non-leap year is used.
//
         if (*pszFormatInfo == BNF_DAY)
            {
            if (usYear == 0)
               usYear = 1991;
            usDay = DaysInMonth (usMonth, usYear);
            ItiStrUSHORTToString (usDay, pszDest, usDestMax);
            }
         else
            return ITIFMT_FORMAT_ERROR;
         break;

      default:
         return ITIFMT_FORMAT_ERROR;

      case 1:
         if (*pszSource == '\0')
            {
            ItiStrCpy (pszDest, apszMonths [0], usDestMax);
            }
         else if (usValue < 10)
            {
            *pszDest       = (char) '0' + (char) usValue;
            *(pszDest + 1) = '\0';
            }
         else
            {
            *pszDest       = '1';
            *(pszDest + 1) = (char) '0' + (char) (usValue % 10);
            *(pszDest + 2) = '\0';
            }
         break;

      case 2:
         if (*pszSource == '\0')
            {
            ItiStrCpy (pszDest, apszMonths [0], usDestMax);
            }
         else
            { 
            if (usValue < 10)
               *pszDest= '0';
            else
               *pszDest = '1';
            *(pszDest + 1) = (char) '0' + (char) (usValue % 10);
            *(pszDest + 2) = '\0';
            }
         break;

      case 3:
         /* modified this section June 24 , 1991; tlb */

         /* -- Determine the numerical value of the month. */
         switch(pszSource[0])
            {
            case 'J':
            case 'j':
               switch (pszSource[3])
                  {
                  case 'U': /* janUary */
                  case 'u':
                     usValue = 1;
                  break;

                  case 'E': /* junE */
                  case 'e':
                     usValue = 6;
                  break;

                  case 'Y': /* julY */
                  case 'y':
                     usValue = 7;
                  break;

                  default:
                     usValue = 0;
                  break;
                  }
               break;

            case 'F': /* February */
            case 'f':
               usValue = 2;
               break;

            case 'M':
            case 'm':
               switch (pszSource[2])
                  {
                  case 'R': /* maRch */
                  case 'r':
                     usValue = 3;
                  break;

                  case 'Y': /* maY */
                  case 'y':
                     usValue = 5;
                  break;

                  default:
                     usValue = 0;
                  break;
                  }
               break;

            case 'A':
            case 'a':
               switch (pszSource[1])
                  {
                  case 'P': /* aPril */
                  case 'p':
                     usValue = 4;
                  break;

                  case 'U': /* aUgust */
                  case 'u':
                     usValue = 8;
                  break;

                  default:
                     usValue = 0;
                  break;
                  }
               break;

            case 'S': /* September */
            case 's':
               usValue = 9;
               break;

            case 'O': /* October */
            case 'o':
               usValue = 10;
               break;

            case 'N': /* November */
            case 'n':
               switch (pszSource[2])
                  {
                  case 'V': /* NOvember */
                  case 'v':
                     usValue = 11;
                  break;

                  default:  /* Not Available */
                     usValue = 0;
                  break;
                  }
               break;

            case 'D': /* December */
            case 'd':
               usValue = 12;
               break;

            default:
               usValue = 0;
               break;
            }/* end of switch ...[0] */

         ItiStrCpy (pszDest, apszMonths [usValue], 4);
         break;

      case 4:
         ItiStrCpy (pszDest, apszMonths [usValue], usDestMax);
         break;
      }/* end of switch usCount */

   if (pusDestLength)
      *pusDestLength = ItiStrLen (pszDest);

   return 0;
   }/* End of function FmtMonth */





static USHORT FmtDay (USHORT  usYear,
                      USHORT  usMonth,
                      USHORT  usDay,
                      PSZ     pszDest,
                      USHORT  usDestMax,
                      PSZ     pszFormatInfo,
                      PUSHORT pusDestLength)
/* added 24 June 1991, tlb */ 
   {
   USHORT usCount;
   USHORT usLength;
   PSZ    pszDay;



   pszDay = DayOfWeek(usYear, usMonth, usDay);


   for (usCount = 0; *pszFormatInfo == BNF_DAY; pszFormatInfo++)
      usCount++;

   /* make sure there's enought room in the destination buffer */
   if (usDestMax <= (usCount + 1))
      {
      if (usDestMax > 0)
         *pszDest = '\0';
      return ITIFMT_TRUNCATED;
      }

   switch (usCount)
      {
      case 0:
         return ITIFMT_FORMAT_ERROR;
         break;


      case 1:
         if ((usDay < 1) || (usDay > 31))
            {
            ItiStrCpy (pszDest, apszDays [0], usDestMax);
            usLength = ItiStrLen (pszDest);
            break;
            }

         ItiStrUSHORTToString (usDay, pszDest, usDestMax);
         usLength = ItiStrLen (pszDest);

         break;


      case 2:
         if ((usDay < 1) || (usDay > 31))
            {
            ItiStrCpy (pszDest, apszDays [0], usDestMax);
            usLength = ItiStrLen (pszDest);
            }
         else
            {
            if (usDay < 10)
               {
               *pszDest = '0';
               pszDest++;
               }
            ItiStrUSHORTToString (usDay, pszDest, usDestMax);
            usLength = ItiStrLen (pszDest);
            }
         break;


      case 3:
         ItiStrCpy (pszDest, pszDay, min(3,usDestMax) );
         usLength = ItiStrLen (pszDest);
         break;


      case 4:
         ItiStrCpy (pszDest, pszDay, usDestMax);
         usLength = ItiStrLen (pszDest);
         break;


      default:
         break;
      }/* end of switch usCount */


   if (pusDestLength)
      *pusDestLength = usLength;

   return 0;
   } /* End of function FmtDay */




static USHORT FmtDateYear (USHORT  usYear,
                           PSZ     pszDest,
                           USHORT  usDestMax,
                           PSZ     pszFormatInfo,
                           PUSHORT pusDestLength)
   {
   USHORT usCount;
   USHORT usLength;


   if ((pszDest == NULL) || (pszFormatInfo == NULL))
      return (ITIFMT_TRUNCATED);

   for (usCount = 0; *pszFormatInfo == BNF_YEAR; pszFormatInfo++)
      usCount++;

   /* make sure there's enought room in the destination buffer */
   if (usDestMax <= (usCount + 1))
      {
      if (usDestMax > 0)
         *pszDest = '\0';
      return ITIFMT_TRUNCATED;
      }

   switch (usCount)
      {
      case 2:
         usYear = usYear - (usYear / 1000) * 1000;
//         if (usYear < 2000)
//            usYear = usYear - 1900;
//         else
//            usYear = usYear - 2000;
//
         ItiStrUSHORTToString (usYear, pszDest, usDestMax);
         usLength = ItiStrLen (pszDest);

         break;


      case 4:
         ItiStrUSHORTToString (usYear, pszDest, usDestMax);
         usLength = ItiStrLen (pszDest);
         break;

      default:
         return ITIFMT_FORMAT_ERROR;
         break;

      }/* end of switch usCount */


   if (pusDestLength)
      *pusDestLength = usLength;

   return 0;
   } /* End of function FmtDateYear */






static USHORT FmtTH (PSZ     pszDest,
                     USHORT  usDestMax,
                     PSZ     pszFormatInfo,
                     PUSHORT pusDestLength)
   {
   USHORT usLength;
   CHAR   szQuip[3] = "th";

   if ((pszDest == NULL) || (pszFormatInfo == NULL))
      return (ITIFMT_TRUNCATED);
   else if (*(pszFormatInfo - 1) != BNF_DAY)
      {
      /* then we have a format not in the BNF,         *
       * ref "Format Methods 6-14-91 11:21 AM" page 2, *
       * should have "dt" or "ddt"                     */
      return (ITIFMT_TRUNCATED);
      }


   if ( *(pszDest - 2) != '1' )
      switch ( *(pszDest - 1) )
         {
         case '1': /* 1st 21st 31st */
            ItiStrCpy (szQuip, "st", sizeof(szQuip) );
            break;

         case '2': /* 2nd 22nd */
            ItiStrCpy (szQuip, "nd", sizeof(szQuip) );
            break;

         case '3': /* 3rd 23rd */
            ItiStrCpy (szQuip, "rd", sizeof(szQuip) );
            break;

         default:
            break;
         }

  
   ItiStrCat (pszDest, szQuip, usDestMax);

   usLength = usLength + 2;

   if (pusDestLength)
      *pusDestLength = usLength;

   return 0;
   } /* End of function FmtTH */







static USHORT FmtHour (USHORT  usHour,
                       PSZ     pszDest,
                       USHORT  usDestMax,
                       PSZ     pszFormatInfo,
                       PUSHORT pusDestLength)
   {
   USHORT usCount;
   USHORT usLength;
   CHAR   szHour[4] = "";
   BOOL   b12HrClk  = FALSE;


   if ((pszDest == NULL) || (pszFormatInfo == NULL))
      return (ITIFMT_TRUNCATED);

   for (usCount = 0; *pszFormatInfo == BNF_HOUR; pszFormatInfo++)
      usCount++;


   /* -- Check if we have AMPM in format string, if so use a 12 hr clock. */
   while (*pszFormatInfo != '\0')
      {
      if ((*pszFormatInfo == BNF_pmam) || (*pszFormatInfo == BNF_AMPM))
         b12HrClk = TRUE;
      pszFormatInfo++;
      }

   if (b12HrClk && (usHour > 12))
      usHour = usHour - 12;



   /* make sure there's enought room in the destination buffer */
   if (usDestMax <= (usCount + 1))
      {
      if (usDestMax > 0)
         *pszDest = '\0';
      return ITIFMT_TRUNCATED;
      }

   switch (usCount)
      {
      case 1:
         ItiStrUSHORTToString (usHour, pszDest, usDestMax);
         usLength = ItiStrLen (pszDest);
         break;


      case 2:
         ItiStrUSHORTToString (usHour, szHour, sizeof(szHour) );
         if (usHour < 10)
            ItiStrCpy(pszDest, "0", usDestMax);

         ItiStrCat(pszDest, szHour, usDestMax);
         usLength = ItiStrLen (pszDest);
         break;

      default:
         return ITIFMT_FORMAT_ERROR;
         break;

      }/* end of switch usCount */


   if (pusDestLength)
      *pusDestLength = usLength;

   return 0;
   } /* End of function FmtHour */








static USHORT FmtMinutes (USHORT  usMin,
                          PSZ     pszDest,
                          USHORT  usDestMax,
                          PSZ     pszFormatInfo,
                          PUSHORT pusDestLength)
   {
   USHORT usCount;
   USHORT usLength;
   CHAR   szMin[4] = "";


   if ((pszDest == NULL) || (pszFormatInfo == NULL))
      return (ITIFMT_TRUNCATED);

   for (usCount = 0; *pszFormatInfo == BNF_MINUTE; pszFormatInfo++)
      usCount++;

   /* make sure there's enought room in the destination buffer */
   if (usDestMax <= (usCount + 1))
      {
      if (usDestMax > 0)
         *pszDest = '\0';
      return ITIFMT_TRUNCATED;
      }

   switch (usCount)
      {
      case 2:
         ItiStrUSHORTToString (usMin, szMin, sizeof(szMin) );
         if (usMin < 10)
            ItiStrCpy(pszDest, "0", usDestMax);

         ItiStrCat(pszDest, szMin, usDestMax);
         usLength = ItiStrLen (pszDest);
         break;

      default:
         return ITIFMT_FORMAT_ERROR;
         break;

      }/* end of switch usCount */


   if (pusDestLength)
      *pusDestLength = usLength;

   return 0;
   } /* End of function FmtMinutes */









static USHORT FmtSeconds (USHORT  usSec,
                          PSZ     pszDest,
                          USHORT  usDestMax,
                          PSZ     pszFormatInfo,
                          PUSHORT pusDestLength)
   {
   USHORT usCount;
   USHORT usLength;
   CHAR   szSec[4] = "";

   if ((pszDest == NULL) || (pszFormatInfo == NULL))
      return (ITIFMT_TRUNCATED);

   for (usCount = 0; *pszFormatInfo == BNF_SECOND; pszFormatInfo++)
      usCount++;

   /* make sure there's enought room in the destination buffer */
   if (usDestMax <= (usCount + 1))
      {
      if (usDestMax > 0)
         *pszDest = '\0';
      return ITIFMT_TRUNCATED;
      }

   switch (usCount)
      {
      case 2:
         ItiStrUSHORTToString (usSec, szSec, sizeof(szSec) );
         if (usSec < 10)
            ItiStrCpy(pszDest, "0", usDestMax);

         ItiStrCat(pszDest, szSec, usDestMax);
         usLength = ItiStrLen (pszDest);
         break;

      default:
         return ITIFMT_FORMAT_ERROR;
         break;

      }/* end of switch usCount */


   if (pusDestLength)
      *pusDestLength = usLength;

   return 0;
   } /* End of function FmtSeconds */










static USHORT FmtAMPM (USHORT  usHours,
                       PSZ     pszDest,
                       USHORT  usDestMax,
                       PSZ     pszFormatInfo,
                       PUSHORT pusDestLength)
   {
   USHORT usCount = 0;
   USHORT usLength;
   CHAR   cFormatInfo = '\0';


   if ((pszDest == NULL) || (pszFormatInfo == NULL))
      return (ITIFMT_TRUNCATED);

   while ((*pszFormatInfo == BNF_pmam) || (*pszFormatInfo == BNF_AMPM))
      {
      cFormatInfo = *pszFormatInfo;
      usCount++;
      pszFormatInfo++;
      }

   if (usCount != 1)
      return ITIFMT_FORMAT_ERROR;


   /* make sure there's enought room in the destination buffer */
   if (usDestMax <= (usCount + 1))
      {
      if (usDestMax > 0)
         *pszDest = '\0';
      return ITIFMT_TRUNCATED;
      }

   switch (cFormatInfo)
      {
      case BNF_AMPM:
         if (usHours > 12)
            {
            ItiStrCpy(pszDest, "PM", usDestMax);
            }
         else
            ItiStrCpy(pszDest, "AM", usDestMax);

         usLength = ItiStrLen (pszDest);
         break;


      case BNF_pmam:
         if (usHours > 12)
            {
            ItiStrCpy(pszDest, "pm", usDestMax);
            }
         else
            ItiStrCpy(pszDest, "am", usDestMax);

         usLength = ItiStrLen (pszDest);
         break;


      default:
         return ITIFMT_FORMAT_ERROR;
         break;

      }/* end of switch usCount */


   if (pusDestLength)
      *pusDestLength = usLength;

   return 0;
   } /* End of function FmtAMPM */
















USHORT EXPORT FmtDateTime (PSZ     pszSource,
                           PSZ     pszDest,
                           USHORT  usDestMax,
                           PSZ     pszFormatInfo,
                           PUSHORT pusDestLength)
   {
   USHORT usYear, usMonth, usDay;
   USHORT usHour, usMinute, usSecond;
   CHAR   szNextChar[2] = "";
   PSZ    pszOriginalDest;
   BOOL   bNoDate, bNoTime;

   pszOriginalDest = pszDest;
   if ((pszDest == NULL) || (pszFormatInfo == NULL))
      return (ITIFMT_TRUNCATED);

   /* take care of NULLs */
   if (*pszSource == '\0')
      {
      ItiStrCpy (pszDest, "NA", usDestMax);
      if (pusDestLength)
         *pusDestLength = ItiStrLen (pszDest);

      return 0;
      }


   bNoDate = !ParseDate (&pszSource, &usYear, &usMonth, &usDay);

   bNoTime = !ParseTime (&pszSource, &usHour, &usMinute, &usSecond);
   if (bNoTime)
      usHour = usMinute = usSecond = 0;

   *pszDest = '\0';
   while (*pszFormatInfo != '\0')
      {
      /* -- Find the end of the pszDest to prepare to append. */
      while ((*pszDest != '\0') && (usDestMax > 0))
         {
         pszDest++;
         usDestMax--;
         }


      switch (*pszFormatInfo)
         {
         case BNF_MONTH:
            if (bNoDate)
               return ITIFMT_BAD_DATA;

            /* -- Month formatting. */
            FmtMonth (apszMonths [usMonth],
                      pszDest,
                      usDestMax,
                      pszFormatInfo,
                      pusDestLength);

            while (*pszFormatInfo == BNF_MONTH)
               pszFormatInfo++;

            break;



         case BNF_DAY:
            if (bNoDate)
               return ITIFMT_BAD_DATA;

            /* -- Day formatting. */
            FmtDay (usYear, usMonth, usDay,
                    pszDest,
                    usDestMax,
                    pszFormatInfo,
                    pusDestLength);

            while (*pszFormatInfo == BNF_DAY)
               pszFormatInfo++;

            break;



         case BNF_TH:
            if (bNoDate)
               return ITIFMT_BAD_DATA;

            /* -- "th" formatting. */
            FmtTH (pszDest,
                   usDestMax,
                   pszFormatInfo,
                   pusDestLength);

            while (*pszFormatInfo == BNF_TH)
               pszFormatInfo++;

            break;



         case BNF_YEAR:          
            if (bNoDate)
               return ITIFMT_BAD_DATA;

            /* -- Year formatting. */
            FmtDateYear (usYear,
                         pszDest,
                         usDestMax,
                         pszFormatInfo,
                         pusDestLength);

             while (*pszFormatInfo == BNF_YEAR)
                pszFormatInfo++;

             break;



         case BNF_HOUR:
            FmtHour (usHour,
                     pszDest,
                     usDestMax,
                     pszFormatInfo,
                     pusDestLength);

            while (*pszFormatInfo == BNF_HOUR)
               pszFormatInfo++;

            break;



         case BNF_MINUTE:
            FmtMinutes (usMinute,
                        pszDest,
                        usDestMax,
                        pszFormatInfo,
                        pusDestLength);

            while (*pszFormatInfo == BNF_MINUTE)
               pszFormatInfo++;

            break;
 


         case BNF_SECOND:
            FmtSeconds (usSecond,
                        pszDest,
                        usDestMax,
                        pszFormatInfo,
                        pusDestLength);

            while (*pszFormatInfo == BNF_SECOND)
               pszFormatInfo++;

            break;


         case BNF_AMPM:
         case BNF_pmam:
            FmtAMPM (usHour,
                     pszDest,
                     usDestMax,
                     pszFormatInfo,
                     pusDestLength);

            while ((*pszFormatInfo == BNF_pmam) || (*pszFormatInfo == BNF_AMPM))
               pszFormatInfo++;

            break;


         default:
            /* -- Skip over "Character"s that are not "Separator"s. */
            szNextChar[0] = *pszFormatInfo;
            pszFormatInfo++;
            ItiStrCat(pszDest, szNextChar, usDestMax);
            break;
         }/* end of switch */

   
      }/* -- End of while (*pszFormatInfo !=... */
  
   if (pusDestLength)
      *pusDestLength = ItiStrLen (pszOriginalDest);

   return 0;
   }/* End of function FmtDateTime */


   
   
   
/**********************************************************************
 * Date Parsing
 *********************************************************************/

static BOOL ParseDate (PSZ       *ppszDate, 
                       PUSHORT   pusYear, 
                       PUSHORT   pusMonth, 
                       PUSHORT   pusDay)
   {
   CHAR szField [MAX_DATE_STRING]="";
   CHAR szCopy [MAX_DATE_STRING]="";
   PSZ pszToken = NULL;
   USHORT usTemp = 0;
   CHAR szA[15]="";
   CHAR szB[15]="";
   CHAR szC[15]="";

   *pusMonth = *pusDay = *pusYear = 0;

   ItiStrCpy (szCopy, *ppszDate, sizeof szCopy);

   pszToken = ItiStrTok (szCopy, " -/,.");
   if (pszToken == NULL) 
      return FALSE;

// // New code to be tested:
//   ItiStrCpy (szA, pszToken, sizeof szA);
//
//   pszToken = ItiStrTok (NULL, " -/,.");
//   if (pszToken == NULL)
//      return FALSE;
//   ItiStrCpy (szB, pszToken, sizeof szB);
//
//   pszToken = ItiStrTok (NULL, " -/,.");
//   if (pszToken == NULL)
//      return FALSE;
//   ItiStrCpy (szC, pszToken, sizeof szC);
//
// /* -- Now that we have the pieces, we need to determine
//    -- if we have m-d-y or y-m-d order. (ignore d-m-y). */
//
//   /* -- If letters... */
//   if (ItiCharIsAlpha (*szA))
//      {            /* ...then assume mmm/dd/yy ordering; */
//      /* so check for a 3-letter representation: jan, feb, etc. */
//      if (!ConvertToMonth (szA, pusMonth))
//         return FALSE;
//
//      /* -- Got month okay, so convert day string to a number. */
//      if (!ConvertToNumber (szB, pusDay))
//         return FALSE;
//
//      /* -- Got day okay, so convert year string to a number. */
//      if (!ConvertToNumber (szC, pusYear))
//         return FALSE;
//      }/* end then clause of if (ItiCharIsAlpha (szA... */
//   else
//      { /* -- So still might be m-d-y only all numbers. */
//      if (!ConvertToNumber (szA, &usTemp))
//         return FALSE;
//
//      if (usTemp > 31)
//         {   /* -- Then assume we have y-m-d ordering. */
//         *pusYear = usTemp;
//
//         /* -- Got year okay, so convert month string to a number. */
//         if (!ConvertToNumber (szB, pusMonth))
//            return FALSE;
//
//         /* -- Got month okay, so convert day string to a number. */
//         if (!ConvertToNumber (szC, pusDay))
//            return FALSE;
//         }/* end if (usTemp... */
//      else
//         { /* ignoring the d-m-y format choice, assume we have m-d-y now. */
//         *pusMonth = usTemp;
//
//         /* -- Got month okay, so convert day string to a number. */
//         if (!ConvertToNumber (szB, pusDay))
//            return FALSE;
//
//         /* -- Got day okay, so convert year string to a number. */
//         if (!ConvertToNumber (szC, pusYear))
//            return FALSE;
//
//         }/* end if (usTemp... else clause */
//
//      }/* end if (ItiCharIsAlpha... else clause */
//   
//
//   if (*pusYear > 9999)
//      return FALSE;
//
//   /* -- Do the 2-digit Year check. */   
//   if (*pusYear < 100)
//      {
//      if (*pusYear < 70)
//         *pusYear += 2000;
//      else
//         *pusYear += 1900;
//      }
//
//   return IsValidDay (*pusMonth, *pusDay, *pusYear);

//////// Below is the orginal code, it assumed always a m-d-y format.
   ItiStrCpy (szField, pszToken, sizeof szField);

   if (ItiCharIsAlpha (*szField))
      {
      if (!ConvertToMonth (szField, pusMonth))
         return FALSE;
      pszToken = ItiStrTok (NULL, " -/,.");
      if (pszToken == NULL)
         return FALSE;
      ItiStrCpy (szField, pszToken, sizeof szField);
      if (!ConvertToNumber (szField, pusDay))
         return FALSE;
      }
   else
      {
      if (!ConvertToNumber (szField, pusMonth))
         return FALSE;
      pszToken = ItiStrTok (NULL, " -/,.");
      if (pszToken == NULL)
         return FALSE;
      ItiStrCpy (szField, pszToken, sizeof szField);
      if (ItiCharIsAlpha (*szField))
         {
         *pusDay = *pusMonth;
         if (!ConvertToMonth (szField, pusMonth))
            return FALSE;
         }
      else
         {
         if (!ConvertToNumber (szField, pusDay))
            return FALSE;
         }
      }

   pszToken = ItiStrTok (NULL, " -/,.");
   if (pszToken == NULL)
      return FALSE;
   ItiStrCpy (szField, pszToken, sizeof szField);
   if (!ConvertToNumber (szField, pusYear))
      return FALSE;

   if (*pusYear > 9999)
      return FALSE;

   if (*pusYear < 100)
      {
      if (*pusYear < 70)
         {
         *pusYear += 2000;
         }
      else
         {
         *pusYear += 1900;
         }
      }

   return IsValidDay (*pusMonth, *pusDay, *pusYear);

   } /* End of Function ParseDate */





static USHORT DaysInMonth (USHORT usMonth, USHORT usYear)
   {
   switch (usMonth)
      {
      /* months with 31 days */
      case 1:
      case 3:
      case 5:
      case 7:
      case 8:
      case 10:
      case 12:
         return 31;
         break;

      /* months with 30 days */
      case 4:
      case 6:
      case 9:
      case 11:
         return 30;
         break;

      /* february -- the month from hell */
      case 2:
         if ((usYear % 4) == 0)
            {
            if ((usYear % 400) != 0)
               {
               return 29;
               }
            else
               {
               return 28;
               }
            }
         else 
            {
            return 28;
            }
         break;

      default: /* months with zero days */
         return 0;
      }
   }




static BOOL IsValidDay (USHORT usMonth, USHORT usDay, USHORT usYear)
   {
   if ((usDay == 0) || (usYear == 0))
      return (FALSE);

   return (usDay <= DaysInMonth (usMonth, usYear));
   }




static BOOL ConvertToMonth (PSZ pszString, USHORT *pusMonth)
   {
   USHORT i;
   USHORT j;
   PSZ    pszTemp;

   *pusMonth = 0;

   if (ItiStrLen (pszString) < 3)
      return FALSE;

   for (i=0; i <= 12; i++)
      {
      pszTemp = apszMonths [i];

      for (j=0; pszTemp [j] != '\0' && pszString[j] != '\0'; j++)
         if (ItiCharLower (pszTemp [j]) != ItiCharLower (pszString [j]))
            break;

      if ('\0' == pszString [j])
         {
         *pusMonth = i;
         return TRUE;
         }
      }

   return FALSE;
   }



static BOOL ConvertToNumber (PSZ pszString, PUSHORT pusResult)
   {
   *pusResult = 0;

   if ('\0' == *pszString)
      return FALSE;

   return ItiStrToUSHORT (pszString, pusResult);
   }






/* ----------------------------------------------------------------------- *\
 * ConvertToDayOfYr                                                        *
 *                                                                         *
 * Given a year, month, and day this function returns which day of the     *
 * year that date falls upon; ie 1991, 2, 14 is the 46th day of the year.  *
 *                                                                         *
 * If an invalid date is given the return value is 0.                      *
\* ----------------------------------------------------------------------- */
static USHORT ConvertToDayOfYr (USHORT usYear, USHORT usMonth, USHORT usDay)
   {
   USHORT usCnt = 0;
   USHORT us;

   /* -- Check if we were given a valid date. */
   if ( !IsValidDay(usMonth, usDay, usYear) )
      return (0);

   /* -- Add up all of the days of the months prior to the given month. */
   for (us = 1; us < usMonth; us++)
      usCnt = usCnt + DaysInMonth (us, usYear);

   /* -- Now add the day of the given month to the total of days in the *
    *    previous months.                                               */
   usCnt = usCnt + usDay;

   return (usCnt);
   }/* End of function ConvertToDayOfYr */
   





/* ----------------------------------------------------------------------- *\
 * YearOffset                                                              *
 *                                                                         *
 * Given a year, this function returns the number of days the first of     *
 * January is offset into the week.                                        *
 *                                                                         *
 * For example, the first of January 1991 is a Tuesday, the THIRD          *
 * day of the week, so the offset for 1991 is TWO; ie the first            *
 * two days of the week (Sunday and Monday) are NOT in the year.           *
 *                                                                         *
 * If a year less than the base year defined in date.h file is given then  *
 * the return value is 0xffff.                                             *
\* ----------------------------------------------------------------------- */
static USHORT YearOffset (USHORT usYr)
   {
   USHORT usOffset = BASE_YEAR_OFFSET;
   USHORT us       = BASE_YEAR;
   USHORT usX;

   if (usYr < BASE_YEAR)
      return (0xffff);

   while (us < usYr)
      {
      /* -- Get the total number of days in the current year. */
      usX = ConvertToDayOfYr (us, 12, 31);

      usOffset = usOffset + (usX % 7);

      if (usOffset > 7)
         usOffset = usOffset - 7;

      us++; 
      }

   return (usOffset);
   }/* End of function YearOffset */
   


/* ----------------------------------------------------------------------- *\
 * DayOfWeek                                                               *
 *                                                                         *
 * Given a date returns a pointer to a text string of which day of the     *
 * it is; ie "Monday", "Tuesday", etc. or "NA" if given date is not valid. *
\* ----------------------------------------------------------------------- */
static PSZ DayOfWeek(USHORT usYear, USHORT usMonth, USHORT usDay)
   {
   USHORT usDayOfWk;
   USHORT usDayOfYr;

   usDayOfYr = ConvertToDayOfYr (usYear, usMonth, usDay);
   usDayOfWk = (usDayOfYr % 7) + YearOffset (usYear);

   if (usDayOfWk > 14)
      return (apszDays [0]); /* error condition */
   else if (usDayOfWk > 7)
      usDayOfWk = usDayOfWk - 7;

   return (apszDays [usDayOfWk]);
   }/* End of function DayOfWeek */




/**********************************************************************
 * Time Parsing
 *********************************************************************/




static BOOL IsValidTime (USHORT usHr, USHORT usMin, USHORT usSec)
   {
   if ((usHr  > 23) || (usMin > 59) || (usSec > 59))
      return (FALSE);
   else
      return (TRUE);

   }/* End of function IsValidTime */






static BOOL ParseTime (PSZ       *ppszTime,
                       PUSHORT   pusHour,
                       PUSHORT   pusMinute,
                       PUSHORT   pusSecond)
   {
   CHAR   szField [MAX_DATE_STRING], szCopy [MAX_DATE_STRING];
   PSZ    pszToken;
   USHORT us;

   *pusHour = *pusMinute = *pusSecond = 0;

   /* -- Assumption is that SQLServer has returned  *
    *    a DateTime format of "mmm dd yyyy hh:mmAM" */
   ItiStrCpy (szCopy, *ppszTime, sizeof szCopy);

   pszToken = ItiStrTok (szCopy, " -/,."); //get the month; skip
   if (pszToken == NULL) 
      return FALSE;
   pszToken = ItiStrTok (NULL, " -/,.");    //get the day; skip
   if (pszToken == NULL) 
      return FALSE;
   pszToken = ItiStrTok (NULL, " -/,.");    //get the year; skip
   if (pszToken == NULL) 
      return FALSE;



   /* -- Now get the hour. */
   pszToken = ItiStrTok (NULL, " :Mm");
   if (pszToken == NULL) 
      return FALSE;
   ItiStrCpy (szField, pszToken, sizeof szField);
   ItiStrToUSHORT(szField, pusHour);



   /* -- Now get the minute. */
   pszToken = ItiStrTok (NULL, " :Mm");
   if (pszToken != NULL)
      {
      ItiStrCpy (szField, pszToken, sizeof szField);
      ItiStrToUSHORT(szField, pusMinute);
   
      us = ItiStrLen(szField);
      if (us>0)
         us--;
      if ( ItiCharIsAlpha(szField[us]) )
         {
         if ((szField[us] == 'P') || (szField[us] == 'p'))
            {
            if (*pusHour < 12)
               *pusHour = *pusHour + 12;
            }
         }

      /* -- Now get the second, iff there was a minute. */
      pszToken = ItiStrTok (NULL, " :Mm");
      if (pszToken != NULL)
         {
         ItiStrCpy (szField, pszToken, sizeof szField);
         ItiStrToUSHORT(szField, pusSecond);
      
         us = ItiStrLen(szField);
         if (us>0)
            us--;
         if ( ItiCharIsAlpha(szField[us]) )
            {
            if ((szField[us] == 'P') || (szField[us] == 'p'))
               {
               if (*pusHour < 12)
                  *pusHour = *pusHour + 12;
               }
            }
         }/* end if pszToken... for second */

      }/* end if pszToken... for minute */


   return IsValidTime (*pusHour, *pusMinute, *pusSecond);
 
   }/* End of function ParseTime */


//USHORT EXPORT CheckDateTime (PSZ     pszSource,
//                             PSZ     pszDest,
//                             USHORT  usDestMax,
//                             PSZ     pszFormatInfo,
//                             PUSHORT pusDestLength)
//   {
//   if (pszFormatInfo == NULL || *pszFormatInfo == '\0')
//      return FmtDateTime (pszSource, pszDest, usDestMax, 
//                          "'mm/dd/yyyy hh:nn:ss'", pusDestLength);
//   else
//      return FmtDateTime (pszSource, pszDest, usDestMax, 
//                          pszFormatInfo, pusDestLength);
//   }






/*
 * FormatInfo = '#' | whatever else .
 *
 * A '#' tells the check function to not quote the string.
 */



USHORT EXPORT CheckDateTime (PSZ     pszSource,
                             PSZ     pszDest,
                             USHORT  usDestMax,
                             PSZ     pszFormatInfo,
                             PUSHORT pusDestLength)
   {
   USHORT usYear, usMonth, usDay;
   USHORT usHour, usMinute, usSecond;
   CHAR   szNextChar[2] = "";
   PSZ    pszOriginalDest;
   PSZ    pszOriginalSource;

   pszOriginalDest = pszDest;
   pszOriginalSource = pszSource;
   if (pszDest == NULL)
      return ITIFMT_TRUNCATED;

   /* take care of NULLs */
   if (*pszSource == '\0')
      {
      if (pszFormatInfo == NULL || *pszFormatInfo != '#')
         ItiStrCpy (pszDest, "''", usDestMax);
      if (pusDestLength)
         *pusDestLength = ItiStrLen (pszDest);

      return 0;
      }

   if (!ParseDate (&pszSource, &usYear, &usMonth, &usDay))
      {
      usYear = 1900;
      usMonth = 1;
      usDay = 1;
      pszSource = pszOriginalSource;
      }

   if (!ParseTime (&pszSource, &usHour, &usMinute, &usSecond))
      {
      usHour = 0;
      usMinute = 0;
      usSecond = 0;
      }

   if (pszFormatInfo == NULL || *pszFormatInfo == '\0')
      pszFormatInfo = "'mm/dd/yyyy hh:nn:ss'";
   else if (pszFormatInfo && *pszFormatInfo == '#')
      pszFormatInfo = "mm/dd/yyyy hh:nn:ss";

   *pszDest = '\0';
   while (*pszFormatInfo != '\0')
      {
      /* -- Find the end of the pszDest to prepare to append. */
      while ((*pszDest != '\0') && (usDestMax > 0))
         {
         pszDest++;
         usDestMax--;
         }


      switch (*pszFormatInfo)
         {
         case BNF_MONTH:
            /* -- Month formatting. */
            FmtMonth (apszMonths [usMonth],
                      pszDest,
                      usDestMax,
                      pszFormatInfo,
                      pusDestLength);

            while (*pszFormatInfo == BNF_MONTH)
               pszFormatInfo++;

            break;



         case BNF_DAY:
            /* -- Day formatting. */
            FmtDay (usYear, usMonth, usDay,
                    pszDest,
                    usDestMax,
                    pszFormatInfo,
                    pusDestLength);

            while (*pszFormatInfo == BNF_DAY)
               pszFormatInfo++;

            break;



         case BNF_TH:
            /* -- "th" formatting. */
            FmtTH (pszDest,
                   usDestMax,
                   pszFormatInfo,
                   pusDestLength);

            while (*pszFormatInfo == BNF_TH)
               pszFormatInfo++;

            break;



         case BNF_YEAR:          
             /* -- Year formatting. */
            FmtDateYear (usYear,
                         pszDest,
                         usDestMax,
                         pszFormatInfo,
                         pusDestLength);

             while (*pszFormatInfo == BNF_YEAR)
                pszFormatInfo++;

             break;



         case BNF_HOUR:
            FmtHour (usHour,
                     pszDest,
                     usDestMax,
                     pszFormatInfo,
                     pusDestLength);

            while (*pszFormatInfo == BNF_HOUR)
               pszFormatInfo++;

            break;



         case BNF_MINUTE:
            FmtMinutes (usMinute,
                        pszDest,
                        usDestMax,
                        pszFormatInfo,
                        pusDestLength);

            while (*pszFormatInfo == BNF_MINUTE)
               pszFormatInfo++;

            break;
 


         case BNF_SECOND:
            FmtSeconds (usSecond,
                        pszDest,
                        usDestMax,
                        pszFormatInfo,
                        pusDestLength);

            while (*pszFormatInfo == BNF_SECOND)
               pszFormatInfo++;

            break;


         case BNF_AMPM:
         case BNF_pmam:
            FmtAMPM (usHour,
                     pszDest,
                     usDestMax,
                     pszFormatInfo,
                     pusDestLength);

            while ((*pszFormatInfo == BNF_pmam) || (*pszFormatInfo == BNF_AMPM))
               pszFormatInfo++;

            break;


         default:
            /* -- Skip over "Character"s that are not "Separator"s. */
            szNextChar[0] = *pszFormatInfo;
            pszFormatInfo++;
            ItiStrCat(pszDest, szNextChar, usDestMax);
            break;
         }/* end of switch */

   
      }/* -- End of while (*pszFormatInfo !=... */
  
   if (pusDestLength)
      *pusDestLength = ItiStrLen (pszOriginalDest);

   return 0;
   }


USHORT EXPORT CheckMonth (PSZ     pszSource,
                          PSZ     pszDest,
                          USHORT  usDestMax,
                          PSZ     pszFormatInfo,
                          PUSHORT pusDestLength)
   {
   USHORT usMonth;

   if (ItiCharIsDigit (*pszSource))
      {
      if (!ItiStrToUSHORT (pszSource, &usMonth))
         return ITIFMT_BAD_DATA;
      }
   else if (*pszSource)
      {
      if (!ConvertToMonth (pszSource, &usMonth))
         return ITIFMT_BAD_DATA;
      }
   else 
      {
      /* null month */
      *pszDest = 0;
      if (pusDestLength)
         *pusDestLength = 0;
      return 0;
      }

   if (usMonth > 12)
      return ITIFMT_BAD_DATA;

   if (pusDestLength)
      *pusDestLength = ItiStrUSHORTToString (usMonth, pszDest, usDestMax);
   else
      ItiStrUSHORTToString (usMonth, pszDest, usDestMax);

   return 0;
   }





USHORT EXPORT ItiFmtParseDate (PSZ       *ppszDate, 
                                PUSHORT   pusYear, 
                                PUSHORT   pusMonth, 
                                PUSHORT   pusDay)
   {
   BOOL bRetVal;

   if (ppszDate == NULL)
      return (ERROR_INVALID_PARAMETER);
   
   bRetVal = ParseDate(ppszDate, pusYear, pusMonth, pusDay);

   if (bRetVal)
      return 0;
   else
      return 13; /* Failed to parse date. */

   }/* End of Function ItiFmtParseDate */




USHORT EXPORT ItiFmtParseTime (PSZ       *ppszTime, 
                                PUSHORT   pusHour,
                                PUSHORT   pusMinute,
                                PUSHORT   pusSecond)
   {
   BOOL bRetVal;

   if (ppszTime == NULL)
      return (ERROR_INVALID_PARAMETER);
   
   bRetVal = ParseTime(ppszTime, pusHour, pusMinute, pusSecond);
   return(bRetVal);

   }/* End of Function ItiFmtParseTime */


/*
 * Convert a calendar date to a julian day number.
 * This algorithm was stolen from "The C Users Journal",
 * February 1993 issue (page 30).
 *
*/
LONG EXPORT ItiFmtCalendarToJulian (PSZ pszCalDate)
   {
   LONG     lYear, lMonth, lDay;
   LONG     lJulDay;
   CHAR     szTmp[64];
   PSZ      pszToken;

   sprintf (szTmp, "%s", pszCalDate);
   pszToken = strtok (szTmp, "/");
   lMonth   = (LONG)(atoi (pszToken));
   pszToken = strtok (NULL, "/");
   lDay     = (LONG)(atoi (pszToken));     
   pszToken = strtok (NULL, "/");
   lYear    = (LONG)(atoi (pszToken));

   lJulDay  = lDay - 32075L + 1461L *
              (lYear + 4800 + (lMonth - 14L) / 12L) /
              4L + 367L * (lMonth - 2L - (lMonth - 14L) /
              12L * 12L) / 12L - 3L * ((lYear + 4900L +
              (lMonth - 14L) / 12L) / 100L) / 4L;
   return lJulDay;
   }

/*
 * Convert a julian day number to a calendar date.
 * This algorithm was stolen from "The C Users Journal",
 * February 1993 issue (page 30).
 *
*/
PSZ EXPORT ItiFmtJulianToCalendar (LONG lJulDate)
   {
   LONG    lTemp1, lTemp2, lYear, lMonth;
   USHORT  usYear, usMonth, usDay;
   CHAR    szReturn[64];
   PSZ     pszReturn;

   lTemp1 = lJulDate + 68569L;
   lTemp2 = 4L * lTemp1 / 146097L;
   lTemp1 = lTemp1 - (146097 * lTemp2 + 3L) / 4L;
   lYear  = 4000L * (lTemp1 + 1) / 1461001L;
   lTemp1 = lTemp1 - 1461L * lYear / 4L + 31;
   lMonth = 80L * lTemp1 / 2447L;

   usDay   = (USHORT) (lTemp1 - 2447L * lMonth / 80L);
   lTemp1 = lMonth / 11L;
   usMonth = (USHORT) (lMonth + 2L - 12L * lTemp1);
   usYear  = (USHORT) (100L * (lTemp2 - 49L) + lYear + lTemp1);

   sprintf(szReturn, "%2.2d/%2.2d/%4.4d",
           usMonth, usDay, usYear);

   pszReturn = szReturn;
   return pszReturn;

   }



