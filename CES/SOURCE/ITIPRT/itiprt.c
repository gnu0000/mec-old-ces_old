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


#define     INCL_BASE
#define     INCL_DOS
#define     INCL_DEV
#define     INCL_GPI
#define     INCL_WIN
#include    "..\include\iti.h"

#include    "..\include\itibase.h"
#include    "..\include\itiutil.h"
#include    "..\include\itierror.h"
#include    "..\include\itiwin.h"

#include    "initdll.h"
#include    "..\include\itirpt.h"
#include    "..\include\itiprt.h"
#include    "..\include\itirdefs.h"

#include    <stdlib.h>
#include    <math.h>


/* ======================================================================= *\
 *
 *                                            CES Maintenance Revision Log
 *
 * Note: All code sections that were changed may be found by searching
 *       for the date (in d MMM yy format) of the change.
 *
 * DATE      COMMENT, BSR#
 * --------- -------------------------------------------------------------
 *  7 JUL 92 Added environment settings for Left/Right, Top/Bottom offsets
 *           to page boundries.                           BSR# 920706-4601
 *
\* ======================================================================= */



/* ======================================================================= *\
 *                                                    MANIFEST CONSTANTS   *
\* ======================================================================= */
#define  DLL_VERSION       1
#define  LEN_ESC_SEQ     128
#define  LEN_TMP_STR      63
#define  PT_ERROR_VAL     -1L

/* The following determines if line graphics rather than dash
 * characters are used for separator lines.
 */
// #define  GRAPHICS          1


/* These environment variables are a maintenance enhancment, 7 JUL 92 */
#define  CES_L_OFFSET  "CESLOFFSET"
#define  CES_R_OFFSET  "CESROFFSET"
#define  CES_T_OFFSET  "CESTOFFSET"
#define  CES_B_OFFSET  "CESBOFFSET"


/* ======================================================================= *\
 *                                                     MACRO DEFINITIONS   *
\* ======================================================================= */
/*  Macro to check if a Gpi function call returned the error code.
 */
#define GPIERR_CHK(v,s)    if ( v == GPI_ERROR)                             \
                              ItiPrtSetError( s )
#define GPIERR_CHK_RET(z)  else                                             \
                              return ( z )

/*  Macro to check if the initialization functions have been run;
 *  this macro is in all of the exported functions.
 */
#define INITCHK(e) extern INT initialized;                                  \
                   if (initialized < 1)                                     \
                      {                                                     \
                      ItiErrWriteDebugMessage("INIT FUNCTIONS NOT CALLED.");\
                      return( e );                                          \
                      }







/* ======================================================================= *\
 *                                        STRUCTURE AND TYPE DEFINITIONS   *
\* ======================================================================= */

typedef struct _PageFormat
         {
         LONG   lNumHdrLns;
         LONG   lNumFtrLns;

         LONG   lTopMarLns;   /* Lines for top margin.     ADDED 7 JUL 92 */
         LONG   lBotMarLns;   /* Lines for bottom margin.  ADDED 7 JUL 92 */

         LONG   lTxtLnsPg;    /* Number of text lines per page.           */
         LONG   lTxtLnHgt;    /* Height of text line in Page Space units. */
         LONG   lNumDisplayLns; /* TextLinesPerPage-(headerLns+footerLns  */
                                /*                   +Top/Bot margin Lns) */
         SIZEL  sizlPgDim;    /* Maximum X & Y of Page Space in units     */
                              /* used by call to GpiCreatePS.             */
         LONG   lLeftMargin;  /* Cols for left margin.    ADDED 7 JUL 92  */
         LONG   lRightMargin; /* Cols for right margin.   ADDED 7 JUL 92  */
         } PAGEFMT;   /* pgfmt */


typedef struct _StringLocation
         {
         POINTL ptl;
         PSZ    psz;
         } STRLOC;    /* strloc */




typedef struct _PageVeiwports
         {
         RECTL  rclPage;
         POINTL ptlPosition;
         } PGVP;    /* pgvp */




/* ======================================================================= *\
 *                                             MODULE'S GLOBAL VARIABLES   *
\* ======================================================================= */
static CHAR         szCompileDateTime[] = __DATE__ " " __TIME__ ;
static CHAR         szDllVersion[] = "1.0g  ItiPrt.dll";
static INT          initialized = 0;
static PAGEFMT      pgfmt;
static FONTMETRICS  fm;
static REPINFO      repi;
static PREPINFO     prepi = &repi;
static REPORTSTATUS enumReportStatus = NOTSTARTED;
static BOOL         bUsePrt = TRUE;
static USHORT       usCurrentPg = 0;
static USHORT       usCurDisplaySpcLnNum  = 0;
static USHORT       usCurrentColNum = 0;
static CHAR         szPrtDate[13];
static CHAR         szPrtTime[9];
static POINTL       ptlCurrentLoc = {0L, 0L};
static LONG         lPgStartingLn = 0L; /* 1L  maybe? */


static HPS    hpsPrtActive = NULL;
static LONG   lOpPrtActive = 0L;
static BOOL   bActive = TRUE;
static BOOL   bInitHF = FALSE; // used by InitHeaderFooter

static BOOL   bPrtToFile = FALSE;
static CHAR   szPrtFile[LGARRAY] = "";
static PSZ    pszPrtFile = NULL;
static PSZ    pszFooter  = NULL;

static CHAR   szDash [LGARRAY+1] = "";
static BOOL   bUseGraphics = FALSE;

/* -- for indentation. */
static INT  iLevel = 0;
static CHAR szText[256+1];
static INT iCurInLevel = 0;
static INT iCurExLevel = 0;



static HINI hiniPrf = 0L;






BOOL StartOfPgLn (LONG lLine, PPOINTL pptlBaseLnLoc);









PSZ ItiPrtIndentedStr (PSZ pszStr)
/* Primarily for use with the DBGMSG macro. */
/* Assumes pszStr is less than 80 characters. */
   {
   INT i;
   INT k;
   INT iTab = 3;
   BOOL bSameLevel = FALSE;


   if (pszStr == NULL)
      return pszStr;

   i = iLevel;
   while ((i<256) && (szText[i] != '\0'))
      szText[i++] = '\0';

   switch(*pszStr)
      {
      case 'I':
         if (iLevel < 170)
            for (k=0; k<iTab; k++)
               iLevel++;
         iCurInLevel++;
         break;

      case 'E':
         iCurExLevel++;
         if (iCurInLevel != iCurExLevel)
            {
            for (k=0; k<iTab; k++)
               {
               if (iLevel > 0)
                  iLevel--;
               }
            iCurExLevel--;
            }
         else
            {
            iCurInLevel--;
            }

         break;

      default:
         bSameLevel = TRUE;
         break;
      }

   for(k=0; k<iLevel; k++)
      szText[k] = ' ';
   szText[iLevel] = '\0';

   ItiStrCat(szText, pszStr, sizeof(szText));

   return(&szText[0]);
   }/* end of function */



















/* ------------------------------------------------------------------------
 * Itoa
 * Reference: K&R second edition, pg 64
 */
PSZ Itoa (INT iNum, CHAR str[], INT iBase)
   {
   INT  a,b,i,k;
   CHAR c;

   if ((k = iNum) < 0)
      iNum = -iNum; /* make it positive */

   i = 0;
   do{
     str[i++] = (CHAR)(iNum % iBase + '0');
     }while ((iNum /= iBase) > 0);

   if (k < 0)
      str[i++] = '-';
   str[i] = '\0';

   /* put back in order */
   for (a=0, b=i-1; a<b; a++, b--)
      {
      c = str[a];
      str[a] = str[b];
      str[b] = c;
      }
   return &str[0];
   }/* End of Function Itoa */



USHORT FPL NthSubStr (USHORT usNth, PSZ pszSource, PSZ pszSepChr, PSZ pszDest)
   {
   PSZ pszTmp   = pszSource;
   USHORT usCnt = 1;

   /* -- Check the intial conditions. */
   if ( (usNth != usCnt) && (pszSource != NULL) && (!(usNth <= 0)) )
      {
      while ((usCnt < usNth) && (*pszTmp != '\0'))
         { /* Find end of the substring. */
         while ((*pszTmp != '\0') && (*pszTmp != *pszSepChr))
            pszTmp++;
         usCnt++;
         *pszTmp++; /* Skip past the terminating sepChr. */
         }

      /* -- Check for unexpected end of source. */
      if ((*pszTmp == '\0') && (usCnt <= usNth))
         {
         *pszDest = '\0';
         return STR_ERR;
         }
      }
   else
      {
      if (usNth <= 0)
         {
         *pszDest = '\0';
         return STR_ERR;
         }
      }

   /* -- Copy substring into destination buffer. */
   while ( ((*pszDest = *pszTmp) != *pszSepChr) && (*pszTmp != '\0') )
      {
      pszDest++;
      pszTmp++;
      }
   if(*pszDest == *pszSepChr)
      *pszDest = '\0';

   return 0;
   }/* end of NthSubStr */








USHORT FPL EraseDevInfo (PDEVINFO pdevi)
   {
   USHORT j;

   DBGMSG(ItiPrtIndentedStr ("IN EraseDevInfo"));

   if (pdevi == NULL)
      return ITIRPT_ERR;

   /* -- Initialize buffer arrays. */
   for (j=0; j<SMARRAY; j++)
         pdevi->szDevice[j] = '\0';

   for (j=0; j<SMARRAY; j++)
         pdevi->szDeviceName[j] = '\0';

   for (j=0; j<SMARRAY; j++)
         pdevi->szDefaultDevice[j] = '\0';

   for (j=0; j<LGARRAY; j++)
         pdevi->szDetails[j] = '\0';

   for (j=0; j<LGARRAY; j++)
         pdevi->szDriverName[j] = '\0';

   for (j=0; j<LGARRAY; j++)
         pdevi->szLogPort[j] = '\0';

   pdevi->lDeviceType = 0L;

   /* -- Initialize the device open struct (dop). */
   pdevi->dop.pszLogAddress = pdevi->szLogPort;
   pdevi->dop.pszDriverName = pdevi->szDriverName;
   pdevi->dop.pdriv         = NULL;
   pdevi->dop.pszDataType   = NULL;
   pdevi->dop.pszComment    = NULL;
   pdevi->dop.pszQueueProcName = NULL;
   pdevi->dop.pszQueueProcParams = NULL;
   pdevi->dop.pszSpoolerParams = NULL;
   pdevi->dop.pszNetworkParams = NULL;

   DBGMSG(ItiPrtIndentedStr ("EXIT EraseDevInfo"));
   return 0;
   } /* END of function EraseDevInfo */




USHORT FPL SetMonitorInfo (PDEVINFO pdevi)
   {
   LONG   lSize    = 0L;
   USHORT usRetVal = 0;


   DBGMSG(ItiPrtIndentedStr ("IN SetMonitorInfo"));

   /* -- Erase any previous device info data. */
   if ((usRetVal = EraseDevInfo(pdevi)) != 0)
      return usRetVal;

   /* -- Set the presentaion space options. */
   pdevi->lPsOptions = (UNITS | GPIT_MICRO | GPIA_ASSOC);

   /* -- Set the device type. */
   pdevi->lDeviceType = OD_MEMORY;


   ItiStrCpy(pdevi->szDriverName,"DISPLAY", sizeof(pdevi->szDriverName));
      DBGMSG("\n pdevi->szDriverName used ");
      DBGMSG(pdevi->szDriverName);

   /* -- Set the logical port. */
   pdevi->dop.pszLogAddress = NULL;
      DBGMSG("pdevi->dop.pszLogAddress contains (should be null)");

   /* -- Set to null because we won't be suppling any driver specific info. */
   pdevi->dop.pdriv = NULL;

   pdevi->dop.pszDataType = NULL;
      DBGMSG("pdevi->dop.pszDataType is using NULL");

//////////////////////
//   if (ItiMbRegisterStaticWnd (ItiRpt_Static,  ItiDllRptWndProc,  hmod))
//      return FALSE;
//
     prepi->hwndWin = NULL;
//      WinCreateStdWindow(HWND_DESKTOP,
//                    WS_VISIBLE | FS_SIZEBORDER | FS_BORDER | FS_TASKLIST,
//                    FCF_BORDER | FCF_SYSMENU | FCF_SIZEBORDER | FCF_TITLEBAR
//                    | FCF_VERTSCROLL,
//                    );
//////////////////////

   DBGMSG("EXITING SetMonitorInfo");
   return usRetVal;

   }/* END OF FUNCTION SetMonitorInfo */



/* --------------------------------------------------------------------- *\
 * SetPrinterInfo
 *   Extracts the default printer information from the INI file to
 *   prepare for opening a device context.
\* --------------------------------------------------------------------- */
USHORT FPL SetPrinterInfo (PDEVINFO pdevi)
   {
   LONG   lSize    = 0L;
   USHORT usRetVal = 0;
   USHORT us = 0;


   DBGMSG("IN SetPrinterInfo");

   /* -- Erase any previous device info data. */
   if ((usRetVal = EraseDevInfo(pdevi)) != 0)
      return usRetVal;


   /* Get the default printer name from the OS2.INI file under:
    * section name "PM_SPOOLER"
    *     key name "PRINTER"
    */
   lSize = PrfQueryProfileString (HINI_PROFILE, "PM_SPOOLER", "PRINTER",
                                  "LPT1",
                                  pdevi->szDevice,
                                  sizeof(pdevi->szDevice));

   /* -- Remove the trailing separator semicolon. */
   pdevi->szDevice[(INT)lSize - 2] = '\0';
      DBGMSG("  szDevice HINI_PROFILE ");
      DBGMSG(pdevi->szDevice);


   /* Using the printer name we received, we again query the INI file
    * for the printer's detail string; which contains four fields separated
    * by semicolons.  The sub-string fields are:
    *
    *      PhysicalPort;DriverNames;Logical(queue)Port;NetworkParameters;
    */
   lSize = PrfQueryProfileString(HINI_PROFILE, "PM_SPOOLER_PRINTER",
                                 pdevi->szDevice, "",
                                 pdevi->szDetails,
                                 sizeof(pdevi->szDetails));
      DBGMSG("  pdevi->szDetails ");
      DBGMSG(pdevi->szDetails);

   /* -- Get the driver names from the 2nd sub-string. */
   NthSubStr(2, pdevi->szDetails,";", pdevi->szDriverName);
      DBGMSG("  pdevi->szDriverName(s) ");
      DBGMSG(pdevi->szDriverName);

   /* -- Remove alternate driver names, if they exist. */
   NthSubStr(1, pdevi->szDriverName,",", pdevi->szDriverName);

   /* -- Remove "descriptive text" from the name that is after the '.' */
   if (ItiStrIsCharInString ('.', pdevi->szDriverName))
      {
      us = 0;
      while ((us<sizeof(pdevi->szDriverName))
             &&(pdevi->szDriverName[us] != '\0'))
         {
         if (pdevi->szDriverName[us] == '.')
            {
            pdevi->szDriverName[us] = '\0';
            break;
            }
         us++;
         }
      }
    DBGMSG("  pdevi->szDriverName used ");
    DBGMSG(pdevi->szDriverName);




   /* -- Get the logical port from the 3rd sub-string. */
   NthSubStr(3, pdevi->szDetails,";", pdevi->szLogPort);
      DBGMSG("  pdevi->szLogPort extracted from INI file ");
      DBGMSG(pdevi->szLogPort);
      DBGMSG("  pdevi->dop.pszLogAddress contains ");
      DBGMSG(pdevi->dop.pszLogAddress);


   /* -- Set to null because we won't be suppling any driver specific info. */
   pdevi->dop.pdriv = NULL;


   /* -- Set the device type. */
   /* -- Iff it is to be sent to a file... */
   DosScanEnv("REPORTPRTF", &pszPrtFile);
   if ((bPrtToFile) || ((pszPrtFile != NULL) && (pszPrtFile[0] != '\0')) )
      {
      pdevi->lDeviceType = OD_DIRECT;
      if (bPrtToFile)
         pdevi->dop.pszLogAddress = szPrtFile;
      else
         pdevi->dop.pszLogAddress = pszPrtFile;
      }
   else
      pdevi->lDeviceType = OD_QUEUED;


   /* -- Set the presentaion space options. */
   pdevi->lPsOptions = (UNITS | GPIT_NORMAL | GPIA_ASSOC);

   lOpPrtActive = pdevi->lPsOptions;


   /* -- Send all output to the default queue. */
   pdevi->dop.pszDataType = "PM_Q_STD";
      DBGMSG("  pdevi->dop.pszDataType is using PM_Q_STD");



   DBGMSG("EXITING SetPrinterInfo");
   return usRetVal;

   }/* END OF FUNCTION SetPrinterInfo */







USHORT FPL SetDCandPS (VOID)
   {
   SIZEL   sizl = { 0L, 0L};    /* Use the same page size as the device. */

   DBGMSG(ItiPrtIndentedStr ("IN SetDCandPS"));

   /* -- First, get needed device information. */
   if (bUsePrt)
      {
      SetPrinterInfo (&(prepi->devi));
      /* -- Second, open the device context. */
      DBGMSG("About to use DevOpenDC...");
      prepi->hdc = DevOpenDC(prepi->hab, prepi->devi.lDeviceType, "*", 4L,
                             (PDEVOPENDATA)&(prepi->devi.dop), (HDC)NULL);
      }
   else
      {
      SetMonitorInfo (&(prepi->devi));
      /* -- Second, open the device context. */
      DBGMSG("About to use WinOpenWindowDC...");
      prepi->hdc = WinOpenWindowDC(prepi->hwndWin);
      }



   if (prepi->hdc == DEV_ERROR)
      {/* then check for error. */
      DBGMSG("EXIT SetDCandPS: FAILED hdc");
      return (ItiPrtSetError("Failed to open DEVICE CONTEXT."));
      }



   /* -- Third, create the presentation space and associate
    *    it with the device context.
    */
   DBGMSG("About to use GpiCreatePS...");
   prepi->hps = GpiCreatePS(prepi->hab, prepi->hdc, &sizl,
                            prepi->devi.lPsOptions);
   if (prepi->hps == GPI_ERROR)
      {/* then check for error. */
      DBGMSG("EXIT SetDCandPS: FAILED ps");
      return ItiPrtSetError("Failed to create Presentation Space.");
      }

   hpsPrtActive = prepi->hps;


   DBGMSG("EXIT SetDCandPS");
   return 0;
   }/* END OF FUNCTION SetDCandPS */






USHORT FPL UnSetDCandPS (VOID)
   {
   ERRORID errid = 0L;
   BOOL bOK;
   HMF  hmf;

   DBGMSG(ItiPrtIndentedStr ("In UnSetDCandPS"));

   /* -- Disasscociate the device context from the presentation space. */
   bOK = GpiAssociate(prepi->hps, (HDC)NULL);
   if ( bOK != GPI_OK)
      {/* then check for error. */
      return (ItiPrtSetError("Failed to DISasscociate the DC from the PS."));
      }

   /* -- Recover the resources from the presentation space. */
   bOK = GpiDestroyPS(prepi->hps);
   if ( bOK != GPI_OK)
      {/* then check for error. */
      return (ItiPrtSetError("Failed to destroy Presentation Space."));
      }

   /* -- Closeout the device context. */
   hmf = DevCloseDC(prepi->hdc);
   if ( hmf != (HMF)DEV_OK)
      {/* then check for error. */
      return (ItiPrtSetError("Failed to close Device Context."));
      }

   DBGMSG("Exit UnSetDCandPS");
   return 0;
   }/* END OF FUNCTION UnSetDCandPS */







//LONG ConvertColToColCoord (USHORT usCol)
//   /*
//    * Note that NO range checking is done.
//    */
//   {
//   LONG   lVal;
//   USHORT us;
//
//   DBGMSG( ItiPrtIndentedStr("In ConvertColToColCoord") );
//
//   if (usCol > CURRENT_COL)
//      {
//      /* -- Get any column spaces requested that are to be added *
//       * -- to the current column.                               */
//      us = usCol - CURRENT_COL;
//
//      /* -- Get the value of the current column. */
//      usCol = ItiPrtQueryCurrentColNum();
//
//      /* -- Add the additonal column spaces. */
//      usCol = usCol + us;
//      }
//   else if (usCol > (MAX_PAPER_COL))
//      {
//      /* -- Get the value of the current column. */
//      us = ItiPrtQueryCurrentColNum();
//
//      /* -- Remove any column spaces requested that are to be *
//       * -- subtracted from the current column.               */
//      us = us - (CURRENT_COL - usCol);
//
//      /* -- Set the column number. */
//      usCol = us;
//      }
//   else if (usCol == CURRENT_COL)
//      {
//      /* -- Get the value of the current column. */
//      usCol = ItiPrtQueryCurrentColNum();
//      }
//   else
//      {
//      /* -- Do nothing, the given usCol value is an absolute. */
//      }
//
//
//   lVal = (fm.lAveCharWidth * ((LONG)usCol + pgfmt.lLeftMargin)); // +LM 
//                                          /* ^ADDED 7 JUL 92 */
//
//   DBGMSG( ItiPrtIndentedStr("Exit ConvertColToColCoord") );
//   return (lVal);
//   }/* End of Function ConvertColToColCoord */


LONG ConvertColToColCoord (USHORT usCol)
   /*
    * Note that minimal range checking is done.
    * This entire function was reworked 7 JUL 92
    */
   {
   LONG   lVal;
   USHORT us;
   USHORT usColumnsPerPage;

   DBGMSG( ItiPrtIndentedStr("In ConvertColToColCoord") );


   usColumnsPerPage = (USHORT)(pgfmt.sizlPgDim.cx / fm.lAveCharWidth); 


   if (usCol > CURRENT_COL)
      {
      /* -- Get any column spaces requested that are to be added *
       * -- to the current column.                               */
      us = usCol - CURRENT_COL;

      /* -- Get the value of the current column. */
      usCol = ItiPrtQueryCurrentColNum();

      /* -- Add the additonal column spaces. */
      usCol = usCol + us;
      }

   else if (usCol == CURRENT_COL)
      {
      /* -- Get the value of the current column. */
      usCol = ItiPrtQueryCurrentColNum();
      }

   else if (usCol > (CURRENT_COL - MAX_PAPER_COL))
      {
      /* -- Get the value of the current column. */
      us = ItiPrtQueryCurrentColNum();

      /* -- Remove any column spaces requested that are to be *
       * -- subtracted from the current column.               *
       * -- and set the column number.                        */
      usCol = us - (CURRENT_COL - usCol);
      }

   else if (usCol >= FROM_RT_EDGE)                  /*   |        <--|    */
      {
      usCol -= FROM_RT_EDGE;
      usCol = usColumnsPerPage - usCol;
      }

   else if (usCol >= REL_LEFT)                      /*   |    .-->   |    */
      {
      /* -- Get the value of the current column. */
      us = ItiPrtQueryCurrentColNum();

      /* -- Remove the Position Value Mask *
       * -- and set the column number.     */
      usCol -= REL_LEFT;
      usCol += us;
      }

   else if (usCol == USE_RT_EDGE)                   /*   |         ->|<-  */
      {
      usCol = usColumnsPerPage;
      }

   else
      {
      /* -- Do nothing, the given usCol value is an absolute. */
      }


   lVal = (fm.lAveCharWidth * ((LONG)usCol + pgfmt.lLeftMargin)); // +LM
                                          /* ^ADDED 7 JUL 92 */
   if (lVal > pgfmt.sizlPgDim.cx)
      lVal = pgfmt.sizlPgDim.cx; 


   DBGMSG( ItiPrtIndentedStr("Exit ConvertColToColCoord") );
   return (lVal);
   }/* End of Function ConvertColToColCoord */







LONG ConvertLnToLnCoord (USHORT usLn)
   {
   POINTL  ptl = {0L, 0L};

   DBGMSG( ItiPrtIndentedStr("In ConvertLnToLnCoord") );

   if ((usLn < CURRENT_LN) && (usLn != 0)
       && ((LONG)usLn <= pgfmt.lNumDisplayLns))
      {
      /* -- If a specific line number was given we need to convert it into
       * -- a display line number value by adding the number of header lines
       * -- and adding the Top margin lines.                                */
      usLn = usLn + (USHORT)pgfmt.lNumHdrLns ;
//                  + (USHORT)pgfmt.lTopMarLns;
//                  /* ^ ADDED 7 JUL 92 */
      }
   else if (usLn > CURRENT_LN)
      {
      usLn = usCurDisplaySpcLnNum
             + (USHORT)pgfmt.lNumHdrLns
//             + (USHORT)pgfmt.lTopMarLns  /* ADDED 7 JUL 92 */
             + (usLn - CURRENT_LN);
      }
   else if (((LONG)usLn > pgfmt.lNumDisplayLns)  /* case of (CURRENT_LN - n) */
            && ((LONG)usLn > ((LONG) CURRENT_LN - pgfmt.lNumDisplayLns)) )
      {
      usLn = usCurDisplaySpcLnNum
             + (USHORT)pgfmt.lNumHdrLns
//             + (USHORT)pgfmt.lTopMarLns  /* ADDED 7 JUL 92 */
             - (CURRENT_LN - usLn);
      }
   else /* (usLn == CURRENT_LN) OR (usLn == 0) */
      {/* then use the current display line. */
      usLn = usCurDisplaySpcLnNum
             + (USHORT)pgfmt.lNumHdrLns ;
//             + (USHORT)pgfmt.lTopMarLns; /* ADDED 7 JUL 92 */
      }



   if (StartOfPgLn( (LONG)usLn, &ptl) )
      {
      DBGMSG( ItiPrtIndentedStr("Exit ConvertLnToLnCoord") );
      return (ptl.y);
      }
   else
      {
      DBGMSG( ItiPrtIndentedStr("Exit ConvertLnToLnCoord") );
      return (PT_ERROR_VAL);
      }

   }/* End of Function ConvertLnToLnCoord */











/* ------------------------------------------------------------------------
 * StartOfPgLn
 *
 *    This function
 *
 * Parameters:    LONG lLine
 *                PPOINTL pptlBaseLnLoc
 *
 * Return value:
 *
 * Comments:
 */
BOOL StartOfPgLn (LONG lLine, PPOINTL pptlBaseLnLoc)
   {
   LONG   ltmp = 0L;
   LONG   lDelta = 0L;

   DBGMSG( ItiPrtIndentedStr("In StartOfPgLn") );

   /* -- First check if the given line number is in a page. */
   if ((lLine < 0L)
       || (lLine > pgfmt.lTxtLnsPg) || (pptlBaseLnLoc == NULL))
      {
      pptlBaseLnLoc->x = PT_ERROR_VAL;
      pptlBaseLnLoc->y = PT_ERROR_VAL;
      return (FALSE);
      }

//   pptlBaseLnLoc->x = 0L;    // +LM
   pptlBaseLnLoc->x = ConvertColToColCoord(0); /* 7 JUL 92 */


   while (ltmp++ <= lLine)
      lDelta = lDelta + pgfmt.lTxtLnHgt;


   pptlBaseLnLoc->y = (pgfmt.sizlPgDim.cy - lDelta) + fm.lMaxDescender;

   DBGMSG( ItiPrtIndentedStr("Exit StartOfPgLn") );
   return (TRUE);
   }/* END OF FUNCTION */


























/* ------------------------------------------------------------------------
 * SetHeaderFooterLnCnt
 *
 *    This function
 *
 * Parameters:    none
 *
 * Return value:
 *
 * Comments:
 */
USHORT SetHeaderFooterLnCnt (VOID)
   {

   /*
    *  HeaderLn[0]  StateName
    *  HeaderLn[1]  AgencyName
    *  HeaderLn[2]  Title
    *  HeaderLn[3]  SubTitle    (exists iff it is not empty)
    *  HeaderLn[4]  SubSubTitle (exists iff it is not empty & SubTitle exists)
    *
    *  FooterLn[0]  FooterText  (exists iff it is not empty)
    */


   if (prepi->szSubTitle[0] == '\0')
      pgfmt.lNumHdrLns = 3L + lPgStartingLn;
   else
      {
      pgfmt.lNumHdrLns = 4L + lPgStartingLn;

      if (prepi->szSubSubTitle[0] != '\0')        // Added 24 Sep 91
         pgfmt.lNumHdrLns = pgfmt.lNumHdrLns + 1;
      }


   /* -- Now init the footer here. Added Feb 1992 */
   DosScanEnv("REPORTFOOTER", &pszFooter);
   if ((pszFooter != NULL) && (pszFooter[0] != '\0'))
      {
      ItiStrCpy(prepi->szFooter, pszFooter, sizeof prepi->szFooter);
      }
   else
      prepi->szFooter[0] = '\0';

   


   /* -- Set footer line count; readjust DisplayLn count. */
   if (prepi->szFooter[0] != '\0')
      pgfmt.lNumFtrLns = 2L; /* separator line + footer text line */
   else
      pgfmt.lNumFtrLns = 0L;


   /* -- Reduce the number of available display lines for the page. */
   pgfmt.lNumDisplayLns = pgfmt.lTxtLnsPg
                   - (pgfmt.lNumHdrLns + pgfmt.lNumFtrLns);
//                      + pgfmt.lTopMarLns + pgfmt.lBotMarLns);/* 7 JUL 92 */

   return 0;
   }/* END OF FUNCTION SetHeaderFooterLnCnt */









/* ------------------------------------------------------------------------
 * InitHeaderFooter
 *
 *    This function
 *
 * Parameters:    none
 *
 * Return value:
 *
 * Comments:
 */
USHORT InitHeaderFooter (VOID)
   {

   USHORT us;
   DATETIME date;
   INT iHr = 0;
   CHAR szTmp[LEN_TMP_STR];


   DBGMSG(ItiPrtIndentedStr ("In InitHeaderFooter"));

   if (bInitHF)
      return 0;

   us = SetHeaderFooterLnCnt();
   if (us > 0)
      return (us);

   us = DosGetDateTime(&date);
   if (us > 0)
      return (us);

   /* ----------------------------------------------------
    * Set the DATE.
    */
   /* -- First, set the month. */
   switch (date.month)
      {
      case 1: /* January */
         ItiStrCpy(szPrtDate, "Jan ", sizeof(szPrtDate));
         break;

      case 2: /* February */
         ItiStrCpy(szPrtDate, "Feb ", sizeof(szPrtDate));
         break;

      case 3: /* March */
         ItiStrCpy(szPrtDate, "Mar ", sizeof(szPrtDate));
         break;

      case 4: /* April */
         ItiStrCpy(szPrtDate, "Apr ", sizeof(szPrtDate));
         break;

      case 5: /* May */
         ItiStrCpy(szPrtDate, "May ", sizeof(szPrtDate));
         break;

      case 6: /* June */
         ItiStrCpy(szPrtDate, "Jun ", sizeof(szPrtDate));
         break;

      case 7: /* July */
         ItiStrCpy(szPrtDate, "Jul ", sizeof(szPrtDate));
         break;

      case 8: /* August */
         ItiStrCpy(szPrtDate, "Aug ", sizeof(szPrtDate));
         break;


      case 9: /* September */
         ItiStrCpy(szPrtDate, "Sep ", sizeof(szPrtDate));
         break;

      case 10: /* October */
         ItiStrCpy(szPrtDate, "Oct ", sizeof(szPrtDate));
         break;

      case 11: /* November */
         ItiStrCpy(szPrtDate, "Nov ", sizeof(szPrtDate));
         break;

      case 12: /* December */
         ItiStrCpy(szPrtDate, "Dec ", sizeof(szPrtDate));
         break;

      default:
         ItiStrCpy(szPrtDate, "ERR ", sizeof(szPrtDate));
         break;
      }/* end switch(month) */

   /* -- Second, set the day. */
   ItiStrCat(szPrtDate, Itoa((INT)date.day, szTmp, 10), sizeof(szPrtDate));
   ItiStrCat(szPrtDate, ", ", sizeof(szPrtDate));

   /* -- Third set the year. */
   ItiStrCat(szPrtDate, Itoa((INT)date.year, szTmp, 10), sizeof(szPrtDate));


   /* ----------------------------------------------------
    * Set the TIME
    */
   iHr = (INT) date.hours;
   if (iHr > 12)
      {
      iHr = iHr - 12;
      }

   Itoa(iHr, szTmp, 10);
   if (iHr < 10)
      {
      if (iHr == 0)
         {         /* it's midnight */
         ItiStrCpy(szPrtTime, "12", sizeof(szPrtTime));
         }
      else
         {
         ItiStrCpy(szPrtTime, " ", sizeof(szPrtTime));
         ItiStrCat(szPrtTime, szTmp, sizeof(szPrtTime));
         }
      }
   else
      ItiStrCpy(szPrtTime, szTmp, sizeof(szPrtTime));


   ItiStrCat(szPrtTime, ":", sizeof(szPrtTime));

   Itoa(date.minutes, szTmp, 10);
   if (date.minutes < 10)
      ItiStrCat(szPrtTime, "0", sizeof(szPrtTime));
   ItiStrCat(szPrtTime, szTmp, sizeof(szPrtTime));

   if (date.hours >= 12)
      ItiStrCat(szPrtTime, " PM", sizeof(szPrtTime));
   else
      ItiStrCat(szPrtTime, " AM", sizeof(szPrtTime));

//   /* -- Now init the footer here. Added Feb 1992 */
//   DosScanEnv("REPORTFOOTER", &pszFooter);
//   if ((pszFooter != NULL) && (pszFooter[0] != '\0'))
//      {
//      ItiStrCpy(prepi->szFooter, pszFooter, sizeof prepi->szFooter);
//      }
//   else
//      prepi->szFooter[0] = '\0';


   bInitHF = TRUE;
   DBGMSG(ItiPrtIndentedStr ("Exit InitHeaderFooter"));
   return 0;

   }/* END OF FUNCTION InitHeaderFooter */




/* ------------------------------------------------------------------------
 * PageMetrics
 *
 *    This function establishes the printed page edge boundries.
 *
 * Parameters:    none
 *
 * Return value:
 *
 * Comments:
 */
USHORT PageMetrics (VOID)
   {
   LONG lDeltaY          = 0L;   /* intermidiate variable. */

   LONG lTopMarLns    = 0L;   /* ADDED 7 JUL 92 */
   LONG lBottomMargin = 0L;   /* ADDED 7 JUL 92 */
   LONG lLeftMargin   = 0L;   /* ADDED 7 JUL 92 */
   LONG lRightMargin  = 0L;   /* ADDED 7 JUL 92 */
   PSZ  pszTOP    = NULL;     /* ADDED 7 JUL 92 */
   PSZ  pszBOTTOM = NULL;     /* ADDED 7 JUL 92 */
   PSZ  pszLEFT   = NULL;     /* ADDED 7 JUL 92 */
   PSZ  pszRIGHT  = NULL;     /* ADDED 7 JUL 92 */


   DBGMSG(ItiPrtIndentedStr ("IN PageMetrics"));

   /* -- Find out metrics info about the font that we are using. */
   if (GpiQueryFontMetrics(prepi->hps, sizeof(fm), &fm) != GPI_OK)
      {/* then check for error. */
      return ItiPrtSetError("GPI query font metrics error.");
      }

   /* -- Get the PS page dimensions. */
   if (GpiQueryPS(prepi->hps, &(pgfmt.sizlPgDim)) == GPI_ERROR)
      {/* then check for error. */
      return ItiPrtSetError("GPI query PS error looking for page parameters.");
      }


   /* -- Figure the height of a default font text line. */
   pgfmt.lTxtLnHgt = fm.lExternalLeading + fm.lMaxBaselineExt;

   /* -- Calculate the number of lines per page for the default font. */
   pgfmt.lTxtLnsPg = 0L;

   /* -- Added enhancments  7 JUL 92  -------------------------------------
      -- check environment for page Top/Bottom adjustments.              \/ */ 
                                                                          //
   lDeltaY = pgfmt.sizlPgDim.cy;                                          //
                                                                          //
   pgfmt.lTopMarLns = 0L;                                                 //
   DosScanEnv(CES_T_OFFSET, &pszTOP);                                     //
   if (pszTOP != NULL)                                                    //
      {                                                                   //
      if (ItiStrToLONG(pszTOP, &lTopMarLns))                              //
         {                                                                //
         pgfmt.lTopMarLns = lTopMarLns;                                   //
         pgfmt.sizlPgDim.cy += lTopMarLns * pgfmt.lTxtLnHgt;
         lDeltaY += (lTopMarLns * pgfmt.lTxtLnHgt);                       //
         if (lTopMarLns < 0)
            lPgStartingLn = labs(lTopMarLns);                             //
         }                                                                //
      }                                                                   //
                                                                          //
   pgfmt.lBotMarLns = 0L;                                                 //
   DosScanEnv(CES_B_OFFSET, &pszBOTTOM);                                  //
   if (pszBOTTOM != NULL)                                                 //
      {                                                                   //
      if (ItiStrToLONG(pszBOTTOM, &lBottomMargin))                        //
         {                                                                //
         pgfmt.lBotMarLns = lBottomMargin;                                //
         pgfmt.sizlPgDim.cy -= lBottomMargin * pgfmt.lTxtLnHgt;
         lDeltaY -= (lBottomMargin * pgfmt.lTxtLnHgt);                    //
         }                                                                //
      }                                                                   //
                                                                          //
                                                                          //
   /* -- Adjust the line count to allow for the top/bottom margins. */    //
   while (lDeltaY > pgfmt.lTxtLnHgt)                                      //
      {                                                                   //
      lDeltaY -= pgfmt.lTxtLnHgt; //Subtract a line hgt from the page hgt.//
      pgfmt.lTxtLnsPg++;          //Add that line  to the line count      //
      }                                                                   //
                                                                          //
                                                                          //
                                                                          //
                                                                          //
   pgfmt.lLeftMargin = 0L;                                                //
   DosScanEnv(CES_L_OFFSET, &pszLEFT);                                    //
   if (pszLEFT != NULL)                                                   //
      {                                                                   //
      if (ItiStrToLONG(pszLEFT, &lLeftMargin))                            //
         {                                                                //
         pgfmt.lLeftMargin = lLeftMargin;                                 //
         pgfmt.sizlPgDim.cx -= (fm.lAveCharWidth * pgfmt.lLeftMargin);    //
         }                                                                //
      }                                                                   //
                                                                          //
   pgfmt.lRightMargin = 0L;                                               //
   DosScanEnv(CES_R_OFFSET, &pszRIGHT);                                   //
   if (pszRIGHT != NULL)                                                  //
      {                                                                   //
      if (ItiStrToLONG(pszRIGHT, &lRightMargin))                          //
         {                                                                //
         pgfmt.lRightMargin = lRightMargin;                               //
         pgfmt.sizlPgDim.cx += (fm.lAveCharWidth * pgfmt.lRightMargin);   //
         }                                                                //
      }                                                                   //
                                                                          //
   /* -- Added enhancments  7 JUL 92  -----------------------------------/\ */



   /* -- There are no headers or footers yet. */
   pgfmt.lNumDisplayLns = pgfmt.lTxtLnsPg;
   pgfmt.lNumHdrLns = 0L;
   pgfmt.lNumFtrLns = 0L;

   /* -- Set the current location to the upper left corner of the page. */
   ptlCurrentLoc.y = pgfmt.sizlPgDim.cy
                     - (pgfmt.lTxtLnHgt
                        + (pgfmt.lTopMarLns*pgfmt.lTxtLnHgt)) /* 7 JUL 92 */
                     + fm.lMaxDescender;

   ptlCurrentLoc.x = 0L + (fm.lAveCharWidth * pgfmt.lLeftMargin);
                        // +LM  /* ^7 JUL 92 */


   DBGMSG(ItiPrtIndentedStr ("EXIT PageMetrics"));
   return 0;
   }/* END PageMetrics */







/* ------------------------------------------------------------------------
 * JustifiedXposFor
 *
 *    This function
 *
 * Parameters:    ALIGNMENT  align
 *                PSZ        psz
 *                LONG *     plConCatXPt
 *
 * Return value:
 *
 * Comments:
 */
LONG JustifiedXposFor (ALIGNMENT align, PSZ psz, LONG *plConCatXPt,
                       LONG lLtM, LONG lRtM)
   {
   POINTL  aptl[TXTBOX_COUNT];
   LONG    lXpt = 0L; /* Starting point of justified sting's baseline pt. */
   LONG    lRetVal;

   DBGMSG(ItiPrtIndentedStr("In JustifiedXposFor"));

   /* -- SOME initial error checking. */
   if ((psz == NULL) || (*psz == '\0'))
      {
      DBGMSG(" PARAMETER[2] WARNING: Null pointer or empty string for PSZ.");
      DBGMSG("Exit JustifiedXposFor");
      return (lXpt);
      }

   if (plConCatXPt == NULL)
      {
      DBGMSG(" PARAMETER[3] ERROR: Null pointer for ConCatXPt passed.");
      DBGMSG("Exit JustifiedXposFor");
      return (lXpt);
      }

   /* -- Get text box dimensions in delta amounts from 0,0. */
   lRetVal = GpiQueryTextBox(prepi->hps, ((LONG)ItiStrLen(psz)),
                             psz, TXTBOX_COUNT, aptl);
   GPIERR_CHK(lRetVal,"ERROR trying to calc the justified X position.");

   if ((align >= NONE) || (align < CENTER))
      {/* use NONE */
      /* The starting X point of the psz string should have been passed
       * in via the lConCatXPt value, it is used to calculate and return
       * the concatenation point that follows the given string.
       */
      lXpt = *plConCatXPt;

      /* Set the concat location point X coord. */
      *plConCatXPt = lXpt + aptl[TXTBOX_CONCAT].x;
      }
   else if (align >= LEFT)
      {/* use LEFT */
      if ((lLtM < 0) || (lLtM >= pgfmt.sizlPgDim.cx))
         lXpt = fm.lAveCharWidth * pgfmt.lLeftMargin;           // 7 JUL 92
//         lXpt = 0;
      else
//         lXpt = lLtM + (fm.lAveCharWidth * (pgfmt.lLeftMargin));// 7 JUL 92
         lXpt = lLtM;

      /* -- Set the concat location point X coord. */
      (*plConCatXPt) = lXpt + aptl[TXTBOX_CONCAT].x;
      }

   else if (align >= RIGHT)
      {/* use RIGHT */
      /* -- Set the location point X coord, *lRtM - txtBoxLength. */
      if ((lRtM == 0L) || (lRtM > pgfmt.sizlPgDim.cx))
        lXpt = pgfmt.sizlPgDim.cx - aptl[TXTBOX_CONCAT].x;
      else
        lXpt = lRtM - aptl[TXTBOX_CONCAT].x;
//        lXpt = lRtM - aptl[TXTBOX_CONCAT].x + (fm.lAveCharWidth * pgfmt.lRightMargin);// 7 JUL 92

      /* -- Set the concat location point X coord.
       *    Note that this value is useless if anything past
       *    it is outside of the page's display space.
       */
      *plConCatXPt = lXpt + aptl[TXTBOX_CONCAT].x;
      }

   else
      {/* use CENTER */
      if ((lRtM == 0L) || (lRtM > pgfmt.sizlPgDim.cx)
          ||(lRtM < (lLtM + fm.lMaxCharInc)) )
         lRtM = pgfmt.sizlPgDim.cx;
      else
         lRtM = lRtM;


      if ((lLtM < 0) || (lLtM >= pgfmt.sizlPgDim.cx) || (lLtM > lRtM) )
//         lLtM = 0;
         lLtM = (fm.lAveCharWidth * pgfmt.lLeftMargin);
      else
         lLtM = lLtM + (fm.lAveCharWidth * pgfmt.lLeftMargin);

      lXpt = (lRtM) - aptl[TXTBOX_CONCAT].x;
      lXpt = (LONG)(lLtM) + ( ((INT)lXpt - (INT)(lLtM)) / 2 );

      /* Set the concat location point X coord. */
      *plConCatXPt = lXpt + aptl[TXTBOX_CONCAT].x;
      }

   DBGMSG(ItiPrtIndentedStr("Exit JustifiedXposFor"));
   return (lXpt);

   }/* END OF FUNCTION JustifiedXposFor */





/* ------------------------------------------------------------------------
 * SeparatorLnSeg
 *
 *    This function
 *
 * Parameters:    LONG     lYpos
 *                LONG     lFromXpos
 *                LONG     lToXpos
 *
 * Return value:
 *
 * Comments:
 */
USHORT SeparatorLnSeg (LONG lYpos, LONG lFromXpos, LONG lToXpos)
   {
   POINTL  ptlCurPos;
   POINTL  ptl;
   LONG    lVal, lPgWidth;
   LONG    lNumChar = 0L;

   DBGMSG( ItiPrtIndentedStr("In SeparatorLnSeg") );

   ptlCurPos.y = 0L;
   ptlCurPos.x = 0L;

   ptl.y = lYpos;
   ptl.x = lFromXpos;

   /* -- Validate the starting point location. */
   if ( (ptl.x < 0) || (ptl.x > pgfmt.sizlPgDim.cx)
      ||(ptl.y < 0) || (ptl.y > pgfmt.sizlPgDim.cy) )
      return (ITIPRT_COORDINATE_ERR );

   /* -- Set the current location to the starting point. */
   lVal = GpiMove(prepi->hps, &ptl);
   GPIERR_CHK(lVal,"ERROR trying to move to given point.");

   /* -- Check the end point, reset if needed. */
   if (lToXpos > pgfmt.sizlPgDim.cx)
      lToXpos = pgfmt.sizlPgDim.cx;
   if (lToXpos <= 0L)
      lToXpos = 0L;


   if (bUseGraphics == TRUE)
      {
      /* -- Draw graphic separator line. */
      ptlCurPos.x = lFromXpos;
      ptlCurPos.y = lYpos + fm.lXHeight;

      ptl.x = lToXpos;
      ptl.y = lYpos + fm.lXHeight;

      /* -- Set the current location to the starting point. */
      lVal = GpiMove(prepi->hps, &ptlCurPos);
      GPIERR_CHK(lVal,"ERROR trying to move to given point.");

      /* -- Draw the line. */
      lVal = GpiLine(prepi->hps, &ptl);
      GPIERR_CHK(lVal,"ERROR trying to draw separator line.");

         /* -- Now a little bit thicker of a line for OS/2 ver 1.2. */
         ptlCurPos.y = ptlCurPos.y + 2;
         ptl.y = ptl.y + 2;

         /* -- Set the current location to the starting point. */
         lVal = GpiMove(prepi->hps, &ptlCurPos);
         GPIERR_CHK(lVal,"ERROR trying to move to given point.");

         /* -- Draw the line. */
         lVal = GpiLine(prepi->hps, &ptl);
         GPIERR_CHK(lVal,"ERROR trying to draw separator line.");
      }
   else /* the separator line is to be composed of dash characters. */
      {
      lPgWidth = (LONG)(pgfmt.sizlPgDim.cx / fm.lAveCharWidth);

      lNumChar =(lToXpos - lFromXpos) / fm.lAveCharWidth;
      if ( (lNumChar > lPgWidth)
         ||(lNumChar > 512L)) /* 512 is the GpiCharString limiting value. */
         lNumChar = lPgWidth;
      if (lNumChar < 1L)
         lNumChar = 1L;

      lVal = GpiCharString(prepi->hps, lNumChar, szDash);
      GPIERR_CHK(lVal,"ERROR trying to write dash characters of separator line.");
      }


   lVal = GpiQueryCurrentPosition(prepi->hps, &ptlCurrentLoc);
   GPIERR_CHK(lVal,"ERROR trying to query the current position.");

//   ptlCurrentLoc.x = 0L + (fm.lAveCharWidth * pgfmt.lLeftMargin);
//                     // +LM  /* ^7 JUL 92 */

   DBGMSG( ItiPrtIndentedStr("Exit SeparatorLnSeg") );
   return 0;
   }/* END OF FUNCTION */



















/* ------------------------------------------------------------------------
 * PrtHeader
 *
 *    This function
 *
 * Parameters:    none
 *
 * Return value:
 *
 * Comments:
 */
USHORT PrtHeader (VOID)
   {
   POINTL ptl, ptlConCat;
   LONG   ln = lPgStartingLn;
   CHAR   szPage[LEN_TMP_STR] = "Page ";
   CHAR   szDB[LEN_TMP_STR]   = "";
   CHAR   szTmp[LEN_TMP_STR]  = "";
   LONG   lRetVal;
   BOOL   bVal;
   PSZ    pszDataBaseName = NULL;


   DBGMSG( ItiPrtIndentedStr("In PrtHeader") );

   if (enumReportStatus == NOTSTARTED) //Added 30 Sep 91
      {
      ItiPrtSetError("Warning from PrtHeader; Report Status is NOTSTARTED.");
      bVal = GpiErase(prepi->hps);     /* Erase page space,      */
      return (enumReportStatus);
      }

   if (enumReportStatus == ENDED) //Added 3 Oct 91
      {
      ItiPrtSetError("Warning from PrtHeader; Report Status is ENDED.");
      bVal = GpiErase(prepi->hps);     /* Erase page space,      */
      return (enumReportStatus);
      }


   bVal = GpiErase(prepi->hps);     /* Erase page space,      */
   if (bVal == (BOOL)GPI_ERROR)     /* then check for error.  */
      {
      ItiPrtSetError("ERROR trying to ERASE PgSpace from PrtHeader.");
      }


   SetHeaderFooterLnCnt(); // Added 24 Sep 91


   /* --Line for State Name and page number. */
   DosScanEnv("SHOWDATABASE", &pszDataBaseName);
   if ((pszDataBaseName != NULL)
       && ((pszDataBaseName[0] == 'y') || (pszDataBaseName[0] == 'Y')) )
      {
      DosScanEnv("DATABASE", &pszDataBaseName);
      if ((pszDataBaseName != NULL) && (pszDataBaseName[0] != '\0'))
         {
         ItiStrCpy(szDB, "(", sizeof(szDB));
         ItiStrCat(szDB, pszDataBaseName, sizeof(szDB));
         ItiStrCat(szDB, ")", sizeof(szDB));
         }
      }


   ItiStrCat(szPage, Itoa((INT)usCurrentPg, szTmp, 10), sizeof(szPage));
   StartOfPgLn(ln++, &ptl);
   ptl.x = JustifiedXposFor (LEFT, szDB, &ptlConCat.x, 0, 0);          //FEB 92
   lRetVal = GpiCharStringAt(prepi->hps, &ptl, ItiStrLen(szDB), szDB); //FEB 92
   GPIERR_CHK(lRetVal,"ERROR trying to write header string.");         //FEB 92

   ptl.x = JustifiedXposFor (RIGHT, szPage, &ptlConCat.x, 0, 0);
   lRetVal = GpiCharStringAt(prepi->hps, &ptl, ItiStrLen(szPage), szPage);
   GPIERR_CHK(lRetVal,"ERROR trying to write header string.");

   ptl.x = JustifiedXposFor (CENTER, prepi->szStateName, &ptlConCat.x, 0, 0);
   lRetVal = GpiCharStringAt(prepi->hps, &ptl,
                   ItiStrLen(prepi->szStateName), prepi->szStateName);
   GPIERR_CHK(lRetVal,"ERROR trying to write header Page#Ln string.");


   /* --Line for Agency Name and date. */
   StartOfPgLn(ln++, &ptl);
   ptl.x = JustifiedXposFor (RIGHT, szPrtDate, &ptlConCat.x, 0, 0);
   lRetVal = GpiCharStringAt(prepi->hps, &ptl,
                             ItiStrLen(szPrtDate), szPrtDate);
   GPIERR_CHK(lRetVal,"ERROR trying to write header string.");

   ptl.x = JustifiedXposFor (CENTER, prepi->szAgencyName, &ptlConCat.x, 0, 0);
   lRetVal = GpiCharStringAt(prepi->hps, &ptl,
                   ItiStrLen(prepi->szAgencyName), prepi->szAgencyName);
   GPIERR_CHK(lRetVal,"ERROR trying to write header DateLn string.");


   /* --Line for report title and time. */
   StartOfPgLn(ln++, &ptl);
   ptl.x = JustifiedXposFor (RIGHT, szPrtTime, &ptlConCat.x, 0, 0);
   lRetVal = GpiCharStringAt(prepi->hps, &ptl,
                             ItiStrLen(szPrtTime), szPrtTime);
   GPIERR_CHK(lRetVal,"ERROR trying to write header TimeTitleLn string.");

   ptl.x = JustifiedXposFor (CENTER, prepi->szTitle, &ptlConCat.x, 0, 0);
   lRetVal = GpiCharStringAt(prepi->hps, &ptl,
                   ItiStrLen(prepi->szTitle), prepi->szTitle);
   GPIERR_CHK(lRetVal,"ERROR trying to write header TitleTimeLn string.");


   if ((prepi->szSubTitle[0] != '\0')
       && (ln <= pgfmt.lNumHdrLns))
      {
      StartOfPgLn(ln++, &ptl);
      ptl.x = JustifiedXposFor (CENTER, prepi->szSubTitle, &ptlConCat.x, 0, 0);
      lRetVal = GpiCharStringAt(prepi->hps, &ptl,
                      ItiStrLen(prepi->szSubTitle), prepi->szSubTitle);
      GPIERR_CHK(lRetVal,"ERROR trying to write header subtitle string.");
      }


   if ((prepi->szSubSubTitle[0] != '\0')
       && (ln <= pgfmt.lNumHdrLns))
      {
      StartOfPgLn(ln++, &ptl);
      ptl.x = JustifiedXposFor (CENTER, prepi->szSubSubTitle, &ptlConCat.x, 0, 0);
      lRetVal = GpiCharStringAt(prepi->hps, &ptl,
                      ItiStrLen(prepi->szSubSubTitle), prepi->szSubSubTitle);
      GPIERR_CHK(lRetVal,"ERROR trying to write header subsubtitle string.");
      }



   /* -- Draw the header's separator line. */
   StartOfPgLn(ln, &ptl);
   SeparatorLnSeg (ptl.y, ptl.x, pgfmt.sizlPgDim.cx);

   /* -- reset current position. */
   StartOfPgLn(++ln, &ptl);
   GpiMove(prepi->hps, &ptl);

   lRetVal = GpiQueryCurrentPosition(prepi->hps, &ptlCurrentLoc);
   GPIERR_CHK(lRetVal,"ERROR trying to query the current position.");

   if (enumReportStatus != NODEV)
      enumReportStatus = PRINTING;

   DBGMSG( ItiPrtIndentedStr("Exit PrtHeader") );
   return 0;
   }/* END OF FUNCTION PrtHeader */






/* ------------------------------------------------------------------------
 * PrtFooter
 *
 *    This function
 *
 * Parameters:    none
 *
 * Return value:
 *
 * Comments:
 */
USHORT PrtFooter (VOID)
   {
   LONG   ln;
   POINTL ptl;
   POINTL ptlConCat;
   LONG   lRetVal;

   DBGMSG(ItiPrtIndentedStr ("In PrtFooter"));

   if (pgfmt.lNumFtrLns == 0L)
      {
      DBGMSG(ItiPrtIndentedStr ("Exit PrtFooter, none are to be shown."));
      return 0;
      }

   ln = 1 + pgfmt.lNumHdrLns + pgfmt.lNumDisplayLns;

   StartOfPgLn(ln++, &ptl);
   SeparatorLnSeg (ptl.y, 0L, pgfmt.sizlPgDim.cx);


   StartOfPgLn(ln, &ptl);
   StartOfPgLn(ln, &ptlConCat);

   ptl.x = JustifiedXposFor (CENTER, prepi->szFooter, &ptlConCat.x, 0, 0);
   lRetVal = GpiCharStringAt(prepi->hps, &ptl,
                   ItiStrLen(prepi->szFooter), prepi->szFooter);
   GPIERR_CHK(lRetVal,"ERROR trying to write page's footer string.");


   lRetVal = GpiQueryCurrentPosition(prepi->hps, &ptlCurrentLoc);
   GPIERR_CHK(lRetVal,"ERROR trying to query the current position.");

   DBGMSG(ItiPrtIndentedStr ("Exit PrtFooter"));

   if (enumReportStatus != NODEV)
      enumReportStatus = PRINTING;

   return 0;
   }/* END OF FUNCTION PrtFooter */


BOOL IsAWordBreakChr (CHAR chr)
   {
   switch (chr)
      {
      case ' ': // blank
      case '.': // period
      case ',': // comma
      case ';': // semicolon
      case '!': // exclaimation
      case '-': // hyphen
      case '_': // underscore

		case '\n': // new line
		case '\r': // CR

      case '\0':// null marker
         return TRUE;

      default:
         return FALSE;
      }
   }




/* ------------------------------------------------------------------------
 * UnderLinedTextAt
 *
 *    This function
 *
 * Parameters:    PPOINTL  pptl
 *                LONG     lOnLine
 *
 * Return value:
 *
 * Comments:
 */
//USHORT EXPORT UnderLinedTextAt (PPOINTL pptl, LONG lOnLine, PSZ psz)
//   {
//   POINTL  aptl[TXTBOX_COUNT];
//
//   DBGMSG(ItiPrtIndentedStr (" *** In UnderLinedTextAt"));
//
//   /* -- Get text box dimensions. */
//   GpiQueryTextBox(prepi->hps, ((LONG)ItiStrLen(psz)), psz, TXTBOX_COUNT, aptl);
//
//   aptl[TXTBOX_BOTTOMLEFT].x += pptl->x;
//   aptl[TXTBOX_BOTTOMLEFT].y += pptl->y;
//   GpiMove(prepi->hps, &aptl[TXTBOX_BOTTOMLEFT]);
//
//   aptl[TXTBOX_BOTTOMRIGHT].x += pptl->x;
//   aptl[TXTBOX_BOTTOMRIGHT].y += pptl->y;
//   GpiLine(prepi->hps, &aptl[TXTBOX_BOTTOMRIGHT]);
//
//   GpiMove(prepi->hps, pptl);
//   GpiCharString(prepi->hps, ((LONG)ItiStrLen(psz)), psz);
//
//   DBGMSG(ItiPrtIndentedStr ("Exit UnderLinedTextAt"));
//   return 0;
//   }/* END OF FUNCTION UnderLinedTextAt */





/* ------------------------------------------------------------------------
 * PtInDisplaySpace
 *
 *    This function determines if a given point is within valid DISPLAY
 *    space on a page; ie not outside of the page boundries and not in
 *    the header or footer space.
 *
 * Parameters:    ptl   The point to be checked.
 *
 * Return value:  TRUE if within display space, FALSE otherwise.
 *
 * Comments:  The rightmost edge is defined as one lMaxCharInc LESS than
 *            the page's physical right edge.
 */
BOOL PtInDisplaySpace (POINTL ptl)
   {
   if ( (ptl.x < 0)
      ||(ptl.x > (pgfmt.sizlPgDim.cx  /* - fm.lMaxCharInc*/ ) )
      ||((INT)ptl.y < ( (INT)pgfmt.lNumFtrLns * (INT)pgfmt.lTxtLnHgt) )
      ||((INT)ptl.y > ( (INT)pgfmt.sizlPgDim.cy
                       - ( (INT)pgfmt.lNumHdrLns * (INT)pgfmt.lTxtLnHgt) )) )
      return (FALSE);
   else
      return (TRUE);

   }/* END OF FUNCTION PtInDisplaySpace */



/* ------------------------------------------------------------------------
 * ConvertInchesToPgUnits
 *
 *    This function Converts inches into the current PageUnits for use
 *    by the other functions.  (This is NOT a vector function.)
 *
 * Parameters:   USHORT usIntegerPart   Whole inch units.
 *               USHORT usFractionPart  Tenths of an inch, see Comments.
 *               PLONG  plPgUnits       The number of current page units
 *                                      closest to the given value.
 *
 * Return value: 0 if successful;
 *
 * Error:        ITIPRT_INIT_ERR
 *
 * Comments:     Currently the fractional portion is only allowed to
 *               be expressed to ONE decimal place.  Note that the
 *               default font average character width is almost 1/10".
 */
USHORT  ConvertInchesToPgUnits(USHORT usIntegerPart, USHORT usFractionPart, PLONG plPgUnits)
   {
   SIZEL  sizl;
   USHORT usIntg = 0;
   USHORT usFrac = 0;
   INITCHK(ITIPRT_NOT_INITIALIZED);

   DBGMSG(ItiPrtIndentedStr ("IN ConvertInchesToPgUnits"));

   *plPgUnits = GpiQueryPS(prepi->hps, &sizl);
   GPIERR_CHK( (*plPgUnits),"ERROR trying to query the presentation space.");
   GPIERR_CHK_RET(ITIPRT_INIT_ERR);


   /* -- Check for the Page Units. */
   if (((*plPgUnits) & PU_HIENGLISH) == PU_HIENGLISH)            /* .001in */
      {
      (*plPgUnits) = (usIntegerPart * 1000) + (usFractionPart  * 100);
      }
   else if ((*plPgUnits & PU_LOENGLISH) == PU_LOENGLISH)         /* .01in  */
      {
      (*plPgUnits) = (usIntegerPart * 100) + (usFractionPart * 10);
      }
   else if ((*plPgUnits & PU_HIMETRIC) == PU_HIMETRIC)           /* .01mm  */
      {
      (*plPgUnits) = (usIntegerPart * 2540) + (usFractionPart * 254);
      }
   else if ((*plPgUnits & PU_LOMETRIC) == PU_LOMETRIC)           /* .1mm   */
      {
      (*plPgUnits) = (usIntegerPart * 254) + (usFractionPart * 25);
      }
   else
      {
      DBGMSG("Other conversion units not yet implemented.");
      DBGMSG("EXIT ConvertInchesToPgUnits");
      return (ITIPRT_UNIT_ERR);
      }

   DBGMSG(ItiPrtIndentedStr ("EXIT ConvertInchesToPgUnits"));
   return (0);
   }/* END OF FUNCTION ConvertInchesToPgUnits */



/* ------------------------------------------------------------------------
 * ConvertColWidthsToPgUnits
 *
 *    This function Converts N columns into the current PageUnits for use
 *    by the other functions.  (This is NOT a vector function.)
 *
 * Parameters:   USHORT usNumOfColWidths
 *               PLONG  plPgUnits
 *                          The number of current page units closest
 *                          to the given parameter values.
 *
 * Comments:     The number of column widths given may be greater than
 *               what will fit between the page edges.
 */
USHORT ConvertColWidthsToPgUnits(USHORT usNumOfColWidths, PLONG plPgUnits)
   {
   INITCHK(ITIPRT_NOT_INITIALIZED);

   DBGMSG(ItiPrtIndentedStr ("IN ConvertColWidthsToPgUnits"));

   (*plPgUnits) = ((USHORT)fm.lAveCharWidth * usNumOfColWidths);

   DBGMSG(ItiPrtIndentedStr ("EXIT ConvertColWidthsToPgUnits"));
   return (0);
   }/* END OF FUNCTION ConvertColWidthsToPgUnits */







/* ======================================================================= *\
 *                                                               EXPORTS   *
\* ======================================================================= */
/* -- ItiDllQueryVersion returns the version number of this DLL. */
USHORT EXPORT ItiDllQueryVersion (VOID)
   {
   return (ITIPRT_DLL_VERSION);
   }







/* ------------------------------------------------------------------------
 * ItiPrtDLLVerChk
 *
 *    This function compares a given version number to the version
 *    number of the ItiPrt DLL.
 *
 * Parameters:    pszCallerVersion    The version string that is to be
 *                                    checked against.
 *
 * Return value:  TRUE  if the current version of the ItiPrt DLL is
 *                      compatable with the given usVerNum.
 *
 * Comments:
 */
BOOL EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion)
   {
   if ( ItiVerCmp(szDllVersion) )
      return (TRUE);
   return (FALSE);
   }







/* ------------------------------------------------------------------------
 * ItiPrtLoadDLLInfo
 *
 *    This function
 *
 * Parameters:    PREPT  prept
 *                INT    argcnt
 *                CHAR * argvars[]
 *
 * Return value:
 *
 * Comments:
 */
USHORT EXPORT ItiPrtLoadDLLInfo
                 (HAB hab, HMQ hmq, PREPT prept, INT argcnt, CHAR * argvars[])
   {
   INT    i;
   USHORT us = 0;
   PSZ    pszEnv = NULL;
   USHORT uspid = USHRT_MAX; // added 30 dec 91
   USHORT usWaitOnPID;       // added 30 dec 91

   DBGMSG("IN PrtDLLLoadInfo function.");

//   COMPILEDDLLDATE( "ItiPrt" );



   /* -- Initialize "dash" string for drawing separator lines. */
   for (us = 0; us < LGARRAY; us++)
      szDash[us] = '-';


   /* -- args not used for now, however to get thru the lint... */
   if ((argcnt == 0)&&(argvars == NULL))
      return (ITIRPT_INVALID_PARM);


   /* -- Check if any other process wants us to delay starting. */
   i = 1; /* argvars[1] should be the the DLL name or a usage request.   */
   while (i < argcnt) /* -- Check for and process any relevent options. */
      {
      if ((argvars[i][0] == '/')||(argvars[i][0] == '-'))
         {
         if (argvars[i][1] == ITIPRT_PIDWAIT)
            {
            ItiStrToUSHORT(argvars[i+1], &usWaitOnPID);
            while (0 == DosGetPPID(usWaitOnPID, &uspid))
               {
               DosSleep(500L);
               }
            }
         }
      i++; /* increment loop counter */
      }




   if (initialized >= 0)
      initialized++;
   else
      return (ITIPRT_CALL_VERSION_CHK);


   if (prept == NULL)
      return (ITIRPT_INVALID_PARM);

   /* ------------------------------------------------------- *\
    *    Initialize the report info structure.                *
   \* ------------------------------------------------------- */
   if (hab == NULL)
      {
      DBGMSG("EXIT PrtDLLLoadInfo function:  FAILED to get hab.");
      return (ITIPRT_INIT_ERR);
      }
   else
      prepi->hab = hab;

   if (hmq == NULL)
      {
      us = ItiPrtSetError("Failed to get hmq.");
      DBGMSG("EXIT PrtDLLLoadInfo function:  FAILED to get hmq.");
      return (ITIPRT_INIT_ERR);
      }
   else
      prepi->hmq = hmq;

   prepi->hdc = 0L;
   prepi->hps = 0L;

   for(us=0; us<sizeof(prepi->szDLLName); us++)
      prepi->szDLLName[us] = '\0';

   for(us=0; us<sizeof(prepi->szStatus); us++)
      prepi->szStatus[0] = '\0';

   prepi->hmod = 0;
   prepi->func = 0;



   /* -- Initialize State and Agency names. */
   us = ItiOpenProfile(prepi->hab, CES_INIPRF, &hiniPrf);

// test code
   us = ItiWriteProfileString (hiniPrf,
                             ITIRPT_APP_SECTION,
                             "STATE",
                             "State Name Here");



   us = ItiGetProfileString (hiniPrf,
                             ITIRPT_APP_SECTION,
                             "STATE",
                             prepi->szStateName,
                             sizeof(prepi->szStateName),
                             "State Name");
//   if (us != 0)
//      ItiStrCpy(prepi->szStateName, "State Name", sizeof(prepi->szStateName));


   us = ItiGetProfileString (hiniPrf,
                             ITIRPT_APP_SECTION,
                             "AGENCY",
                             prepi->szAgencyName,
                             sizeof(prepi->szAgencyName),
                             "Agency Name");
//   if (us != 0)
//      ItiStrCpy(prepi->szAgencyName, "Agency Name",
//                sizeof(prepi->szAgencyName));

   us = ItiCloseProfile(&hiniPrf);




   ItiStrCpy(prepi->szTitle,
                prept->szTitle,    sizeof(prepi->szTitle)   );
   ItiStrCpy(prepi->szSubTitle,
                prept->szSubTitle, sizeof(prepi->szSubTitle));
   ItiStrCpy(prepi->szSubSubTitle,
                prept->szSubSubTitle, sizeof(prepi->szSubSubTitle));
   ItiStrCpy(prepi->szFooter,
                prept->szFooter,   sizeof(prepi->szFooter)  );

   /* -- Setup device context and presentation space. */
   us = SetDCandPS();
   if (us != 0 )
      {
      us = ItiPrtSetError("Failed to establish PresentationSpace, DevContext.");
      DBGMSG("EXIT PrtDLLLoadInfo function:  FAILED to set DC & PS.");
      return (ITIPRT_INIT_ERR);
      }


   DosScanEnv(ITIPRT_USE_GRAPHICS, &pszEnv);/* Check for Laser printer use. */
   if ((pszEnv != NULL)
       && (*pszEnv == 'Y'))      //  || add test for laser printer here.
      {
      bUseGraphics = TRUE;
      }
   else
      {
      bUseGraphics = FALSE;
      }

   us = PageMetrics();
   us = InitHeaderFooter();

   /* -- Could do a beginReport here. */
   DBGMSG("EXIT PrtDLLLoadInfo function.");
   return (us);
   }/* END OF FUNCTION */




/* ------------------------------------------------------------------------
 * ItiPrtSetError
 *
 *    This function screens the returned error codes and sends pszErrorText
 *    to the debug window.
 *
 * Parameters: errid          The error ID value.
 *             pszErrorText   A pointer to the error message text string.
 *
 * Return value: 0 if severity level of the errid is just a warning.
 *
 *
 * Comments: Note some Win functions return ERRORID values.  However,
 *           you may create your own ERRORID value by using the macro
 *           MAKEERRORID(severityLevel, errorValue) where severityLevel
 *           can be any of the following:
 *
 *                       SEVERITY_NOERROR
 *                       SEVERITY_WARNING
 *                       SEVERITY_ERROR
 *                       SEVERITY_SEVERE
 *                       SEVERITY_UNRECOVERABLE
 *
 *           Note that in the macro <errorValue> is a USHORT value.
 */
USHORT EXPORT ItiPrtSetError(PSZ pszErrorText)
   {
   USHORT  usErrSeverity;
   USHORT  usErrValue;
   ERRORID errid;

   DBGMSG(ItiPrtIndentedStr ("In ItiPrtSetError"));

   errid = WinGetLastError(prepi->hab);
   usErrValue = (USHORT) ERRORIDERROR(errid);
   usErrSeverity = (USHORT) ERRORIDSEV(errid);

   ItiErrFreeErrorString (ItiErrGetErrorString(prepi->hab));

   /* -- Development version of error message set up. */
   ItiErrWriteDebugMessage (pszErrorText);
   /* ...insert other error message handling here.    */


   DBGMSG("Exit Leaving ItiPrtSetError");
   if (ERRORIDSEV(errid) == SEVERITY_WARNING)
      return 0;
   else
      return usErrValue;

   }/* end of ItiPrtSetError */




/* ------------------------------------------------------------------------
 * ItiPrtSkipLines
 *
 *    This function increments the current page display space line number;
 *    if current line ends up on a new page then the skipping stops, the
 *    current line then being one and the new page number is returned.
 *
 * Parameters:    usNumOfLinesToSkip
 *
 * Return value:  the current page number.
 *
 * Comments:
 */
USHORT EXPORT ItiPrtSkipLns (USHORT usNumLnsToSkip)
   {
   USHORT usCurrentPg;
   LONG   lng = 0L;
   USHORT us  = 0;

   DBGMSG(ItiPrtIndentedStr ("IN ItiPrtSkipLns"));

   usCurrentPg = ItiPrtQueryCurrentPgNum();

   while ( (ItiPrtQueryCurrentPgNum() == usCurrentPg )
          && (ItiPrtQueryCurrentLnNum() != 1) && (us < usNumLnsToSkip) )
      {
      if (ItiPrtQueryCurrentPgNum() == usCurrentPg )
         {
         ItiPrtIncCurrentLn();
         us++;
         }
      }

   usCurrentPg = ItiPrtQueryCurrentPgNum();

   DBGMSG(ItiPrtIndentedStr ("EXIT ItiPrtSkipLns"));
   return (usCurrentPg);

   }/* END OF FUNCTION SkipLns */





USHORT EXPORT ItiPrtKeepNLns (USHORT usNumLnsToKeepTogether)
   {
   USHORT usPgAtStartOfKeep;

   DBGMSG(ItiPrtIndentedStr ("IN ItiPrtKeepNLns"));
   usPgAtStartOfKeep = usCurrentPg;

   /* -- We want to keep X number of lines together so if fewer than      *
    * -- the desired number of lines remain on the page we will increment *
    * -- the current line until we are on the next page.                  */
   while ((LONG)usCurDisplaySpcLnNum
          >= (pgfmt.lNumDisplayLns - (LONG)usNumLnsToKeepTogether) )
      {
      ItiPrtIncCurrentLn ();
      if (usCurrentPg != usPgAtStartOfKeep)
         {
         return(PG_WAS_INC);
         break;
         }
      }

   DBGMSG(ItiPrtIndentedStr ("EXIT ItiPrtKeepNLns"));
   return (0);
   }/* END OF FUNCTION ItiPrtKeepNLns */





/* ------------------------------------------------------------------------
 * ItiPrtMoveTo
 *
 *    This function moves the current Gpi point location to the
 *    requested line and column if possible.
 *
 * Return value:  0 if successful.
 *
 * Comments:
 */
USHORT EXPORT ItiPrtMoveTo (USHORT usLn, USHORT usCol)
   {
   LONG lVal = 0L;
   POINTL ptlTmp;

   GpiQueryCurrentPosition(prepi->hps, &ptlCurrentLoc); //7 JUL 92
   ptlTmp = ptlCurrentLoc;

   ptlCurrentLoc.y = ConvertLnToLnCoord(usLn);
   if (ptlCurrentLoc.y == PT_ERROR_VAL)
      {
      ptlCurrentLoc = ptlTmp;
      return (ITIPRT_COORDINATE_ERR);
      }

   ptlCurrentLoc.x = ConvertColToColCoord(usCol);
   if ((ptlCurrentLoc.x == PT_ERROR_VAL)
       || (ptlCurrentLoc.x > pgfmt.sizlPgDim.cx))
      {
      ptlCurrentLoc = ptlTmp;
      return (ITIPRT_COORDINATE_ERR);
      }

   lVal = GpiMove(prepi->hps, &ptlCurrentLoc);
   if (lVal == GPI_OK)
      {
      /* -- Reset the current Ln (and Col?) values. */ // added 21 Jan 1992
      usCurDisplaySpcLnNum = (USHORT)(pgfmt.lNumDisplayLns
                                      - (ptlCurrentLoc.y / pgfmt.lTxtLnHgt));
      return (0);
      }
   else
      lVal = GpiMove(prepi->hps, &ptlTmp);

   return (PT_ERROR_VAL);

   }/* END OF FUNCTION ItiPrtMoveTo */






/* ------------------------------------------------------------------------
 * ItiPrtIncCurrentLn
 *
 *    This function increments the value of the current line number.
 *
 * Return value:  The new current line value.
 *
 * Comments:
 */
USHORT EXPORT ItiPrtIncCurrentLn (VOID)
   {
   USHORT usLnNum;

   DBGMSG(ItiPrtIndentedStr ("IN ItiPrtIncCurrentLn"));

   usCurDisplaySpcLnNum++;

   while (! ItiPrtStartOfDisplayLn (CURRENT_LN, &usLnNum) )
      {/* then we are incrementing onto the next page. */
      if (! ItiPrtNewPg() )
         ItiPrtSetError("PAGE ERROR during increment current line.");
      }

   DBGMSG(ItiPrtIndentedStr ("EXIT ItiPrtIncCurrentLn"));
   return (usLnNum);
   }/* END OF FUNCTION ItiPrtIncCurrentLn */





/* ------------------------------------------------------------------------
 * ItiPrtQueryCurrentColNum
 *
 *    This function gets the value of the current GPI location point.
 *
 *
 * Parameters:
 *
 * Return value:
 *
 * Comments:
 */
USHORT EXPORT ItiPrtQueryCurrentColNum (VOID)
   {
   LONG   lVal = GPI_OK;
   USHORT usHi = 0;
   USHORT usLo = 0;
   USHORT usAveCharWidth = 0;
   USHORT usCol = 0;
   POINTL ptl = {0L, 0L};
   INITCHK(ITIPRT_NOT_INITIALIZED);

   DBGMSG(ItiPrtIndentedStr ("IN ItiPrtQueryCurrentColNum"));

   lVal = GpiQueryCurrentPosition(prepi->hps, &ptl); 
   GPIERR_CHK(lVal,"ERROR trying to query the current point.");
//   GPIERR_CHK_RET(ITIPRT_COORDINATE_ERR);

   // +LM 

   /* Check high word, should be 0 since our page is not 65536 cols wide. */
   usHi = HIUSHORT(ptl.x);
   if (usHi != 0)
      {
      ItiPrtSetError("Current GPI ptl.x location outside page space.");
      return(ITIPRT_COORDINATE_ERR);
      }

   usLo = LOUSHORT(ptl.x);
   usAveCharWidth = LOUSHORT(fm.lAveCharWidth);

   /* -- Check if too close to page's edge. */
   usCol = (usLo / usAveCharWidth);
   if ( ((usLo % usAveCharWidth) > 0)
        &&(ptl.x < (pgfmt.sizlPgDim.cx-(fm.lAveCharWidth+fm.lAveCharWidth))))
      usCol = usCol + 1;


   usCurrentColNum = usCol;

   DBGMSG(ItiPrtIndentedStr("EXIT ItiPrtQueryCurrentColNum"));
   return (usCol);
   }/* END OF FUNCTION ItiPrtQueryCurrentColNum */




/* ------------------------------------------------------------------------
 * ItiPrtQueryCurrentLnNum
 *
 *    This function returns the value of the current line number.
 *
 * Parameters:    none
 *
 * Return value:  The current line number.
 *
 */
USHORT EXPORT ItiPrtQueryCurrentLnNum (VOID)
   {
   DBGMSG(ItiPrtIndentedStr("IN ItiPrtQueryCurrentLnNum"));
   DBGMSG(ItiPrtIndentedStr("EXIT ItiPrtQueryCurrentLnNum"));
   return (usCurDisplaySpcLnNum);
   } /* END OF FUNCTION ItiPrtQueryCurrentLnNum */






/* ------------------------------------------------------------------------
 * ItiPrtQueryCurrentPgNum
 *
 *    This function
 *
 * Parameters:    none
 *
 * Return value:
 *
 * Comments:
 */
USHORT EXPORT ItiPrtQueryCurrentPgNum (VOID)
   {
   INITCHK(ITIPRT_NOT_INITIALIZED);

   DBGMSG(ItiPrtIndentedStr("IN from ItiPrtQueryCurrentPgNum"));
   DBGMSG(ItiPrtIndentedStr("EXIT from ItiPrtQueryCurrentPgNum"));
   return (usCurrentPg);
   }



/* ------------------------------------------------------------------------
 * ItiPrtQueryNumOfDisplayLns
 *
 *     This function returns the number of lines in the display space of
 *   the page; ie. the number of lines on a page minus the number of lines
 *   currently used for the header and footer.
 *
 * Parameters:    none
 *
 * Return value:  number of lines
 *
 * Comments:
 */
USHORT EXPORT ItiPrtQueryTotalNumDisplayLns (VOID)
   {
   INITCHK(ITIPRT_NOT_INITIALIZED);

   DBGMSG(ItiPrtIndentedStr("IN from ItiPrtQueryNumOfDisplayLns"));
   DBGMSG(ItiPrtIndentedStr("EXIT from ItiPrtQueryNumOfDisplayLns"));
   return ((USHORT)pgfmt.lNumDisplayLns);
   }/* END OF FUNCTION ItiPrtQueryTotalNumDisplayLns */





USHORT EXPORT ItiPrtQueryRemainNumDisplayLns (VOID)
   {
   USHORT  us;

//   DBGMSG(ItiPrtIndentedStr("IN from ItiPrtQueryRemainNumDisplayLns"));

   us = (USHORT)pgfmt.lNumDisplayLns - usCurDisplaySpcLnNum;

//   DBGMSG(ItiPrtIndentedStr("EXIT from ItiPrtQueryRemainNumDisplayLns"));

   return (us);

   }/* END OF FUNCTION ItiPrtQueryRemainNumDisplayLns */




USHORT EXPORT ItiPrtConvertMmToCol (USHORT usMm,  PUSHORT pusCol)
   {
   SIZEL  sizl;
   float  fColConvertValue; /* width of the average column in millimeters */
   LONG   lPgUnits;
   INITCHK(ITIPRT_NOT_INITIALIZED);

   DBGMSG(ItiPrtIndentedStr("IN ConvertMmToCol"));

   if (pusCol == NULL)
      return (ITIRPT_INVALID_PARM);

   lPgUnits = GpiQueryPS(prepi->hps, &sizl);
   GPIERR_CHK(lPgUnits, "ERROR trying to query the presentation space.");
   GPIERR_CHK_RET(ITIPRT_INIT_ERR);

   /* -- Check the Page Units. */
   if ((lPgUnits & PU_HIENGLISH) == PU_HIENGLISH)              /* .001in */
      {
      fColConvertValue = (float)fm.lAveCharWidth * 0.0254;
      }
   else if ((lPgUnits & PU_LOENGLISH) == PU_LOENGLISH)         /* .01in  */
      {
      fColConvertValue = (float)fm.lAveCharWidth * 0.2540;
      }
   else if ((lPgUnits & PU_HIMETRIC) == PU_HIMETRIC)           /* .01mm  */
      {
      fColConvertValue = (float)fm.lAveCharWidth * 0.0100;
      }
   else if ((lPgUnits & PU_LOMETRIC) == PU_LOMETRIC)           /* .1mm   */
      {
      fColConvertValue = (float)fm.lAveCharWidth * 0.1000;
      }
   else
      {
      DBGMSG("Other conversion units not yet implemented.");
      DBGMSG(ItiPrtIndentedStr("EXIT ConvertMmToCol, units not imp"));
      return (ITIPRT_UNIT_ERR);
      }

   fColConvertValue = (float)usMm / fColConvertValue;

   (*pusCol) = (USHORT)fColConvertValue;
   (*pusCol)++;

   DBGMSG(ItiPrtIndentedStr("EXIT ConvertMmToCol"));
   return (0);
   }



USHORT EXPORT ItiPrtConvertColToMm (USHORT usCol, PUSHORT pusMm)
   {
   SIZEL  sizl;
   float  fColConvertValue; /* width of the average column in millimeters */
   LONG   lPgUnits;
   INITCHK(ITIPRT_NOT_INITIALIZED);

   DBGMSG(ItiPrtIndentedStr("IN ConvertColToMm"));

   if (pusMm == NULL)
      return (ITIRPT_INVALID_PARM);

   lPgUnits = GpiQueryPS(prepi->hps, &sizl);
   GPIERR_CHK(lPgUnits, "ERROR trying to query the presentation space.");
   GPIERR_CHK_RET(ITIPRT_INIT_ERR);

   /* -- Check the Page Units. */
   if ((lPgUnits & PU_HIENGLISH) == PU_HIENGLISH)              /* .001in */
      {
      fColConvertValue = (float)fm.lAveCharWidth * 0.0254;
      }
   else if ((lPgUnits & PU_LOENGLISH) == PU_LOENGLISH)         /* .01in  */
      {
      fColConvertValue = (float)fm.lAveCharWidth * 0.2540;
      }
   else if ((lPgUnits & PU_HIMETRIC) == PU_HIMETRIC)           /* .01mm  */
      {
      fColConvertValue = (float)fm.lAveCharWidth * 0.0100;
      }
   else if ((lPgUnits & PU_LOMETRIC) == PU_LOMETRIC)           /* .1mm   */
      {
      fColConvertValue = (float)fm.lAveCharWidth * 0.1000;
      }
   else
      {
      DBGMSG("Other conversion units not yet implemented.");
      DBGMSG(ItiPrtIndentedStr("EXIT ConvertColToMm"));
      return (ITIPRT_UNIT_ERR);
      }

   fColConvertValue = (float)usCol * fColConvertValue;

   (*pusMm) = (USHORT)fColConvertValue;
   if (((double)fColConvertValue - (double)(*pusMm)) > 0.0)
      (*pusMm)++;

   DBGMSG(ItiPrtIndentedStr("EXIT ConvertColToMm"));
   return (0);
   }





USHORT EXPORT ItiPrtChangeHeaderLn (USHORT usNamedLn, PSZ pszNewText)
   {
   USHORT us = 0;

   if (pszNewText != NULL)   /* if it is null we print the header as is. */
      switch (usNamedLn)
         {
         case ITIPRT_TITLE:
            {
            us = sizeof(prepi->szTitle);
            ItiStrCpy(prepi->szTitle, pszNewText, us);
            break;
            }

         case ITIPRT_SUBTITLE:
            {
            us = sizeof(prepi->szSubTitle);
            ItiStrCpy(prepi->szSubTitle, pszNewText, us);
            break;
            }

         case ITIPRT_SUB_SUBTITLE:
            {
            us = sizeof(prepi->szSubSubTitle);
            ItiStrCpy(prepi->szSubSubTitle, pszNewText, us);
            break;
            }


         default:
            {
//            return (ITIPRT_PRINTING_ERR);
            break;
            }
         }/* end of switch */


   us = PrtHeader();
   return (us);
   }/* END OF FUNCTION ItiPrtChangeHeaderLn */





/* ------------------------------------------------------------------------
 * ItiPrtStartOfDisplayLn
 *
 * Comments:
 */
BOOL EXPORT ItiPrtStartOfDisplayLn (USHORT usRequestedLn, PUSHORT pusLn)
   {
   BOOL  bRetVal = FALSE;
   INITCHK(FALSE);

   DBGMSG(ItiPrtIndentedStr("IN ItiPrtStartOfDisplayLn function."));

   if (pusLn == NULL)
      {
      DBGMSG("   Null parameter pointer given.");
      return (FALSE);
      }

   if ((usRequestedLn == CURRENT_LN)
       || ((LONG)usRequestedLn > pgfmt.lNumDisplayLns))
      *pusLn = usCurDisplaySpcLnNum;
   else
      *pusLn = usRequestedLn;


   bRetVal = StartOfPgLn(( (LONG)(*pusLn)
                           + pgfmt.lNumHdrLns + pgfmt.lTopMarLns),//7 JUL 92
                         &ptlCurrentLoc);
   GpiMove(prepi->hps, &ptlCurrentLoc);


   DBGMSG(ItiPrtIndentedStr("EXIT ItiPrtStartOfDisplayLn function."));
   return (bRetVal);
   }/* END OF FUNCTION ItiPrtStartOfDisplayLn */





/* ------------------------------------------------------------------------
 * ItiPrtPutTextAt
 *
 *    This function will write a string to the page space at the specified
 *    location point.
 *
 * Parameter    Description
 * ----------------------------------------------------------------------
 * usLn         Display line for text.
 *
 * usCol        Column for text.
 *
 * algnMode     Specifies the alignment of the string on the line.
 *              These values may be bitwise ORed together.  Note that
 *              the default values are NONE and CLIP.  The precedence
 *              order of the values within the subgroups are:
 *
 *                        CLIP > WRAP
 *
 *                        NONE > LEFT > RIGHT > CENTER
 *
 *                        LNINC
 *
 *              i.e. if given (CENTER | LEFT | CLIP) the text will be
 *              left aligned at <ptl> and clipped at the <lRtMargin>.
 *
 *              Note that the value LNINC can be bitwise ORed with the
 *              algnMode value to increment the current line after text
 *              is written.
 *
 *              <algnMode> can be any of the following values:
 *
 *                 Value      Meaning
 *                 ------------------------------------------------------
 *                 CLIP       (DEFAULT)  Any text that goes beyond the
 *                            <lRtMargin> is clipped from the page view.
 *
 *                 WRAP       Any text that goes beyond the <lRtMargin>
 *                            wrapped around to the next line with its
 *                            left margin being the same as the starting
 *                            point.
 *                       _ _ _ _ _ _ _ _ _ _ _ _ _ _
 *
 *                 NONE       (DEFAULT)  Start the text at the current
 *                            X,Y position;  The value <ptl> should be
 *                            set by a call to ItiPrtQueryCurrentPt()
 *                            which usually is the end of the text
 *                            written by the last GpiCharString or
 *                            GpiMove.
 *                            Note that this <ptl> point also becomes
 *                            the new left margin for wrapping.
 *
 *                 LEFT       The starting point is moved to the leftmost
 *                            point of the current display line.
 *
 *                 RIGHT      Calculates the needed starting point such
 *                            that the string will END at the rightmost
 *                            edge of the page space.  Will use the value
 *                            specified by <lRtMargin> if given, otherwise
 *                            defaults to the rightmost edge of the page.
 *
 *                 CENTER     Calculates the needed starting point to
 *                            center the given text on the current line.
 *
 *
 * usRtMCol     The number of columns FROM the LEFT side at which to
 *              stop the text.  If this value locates the point greater than the
 *              rightmost edge of the page, or if the given <lRtMCol> is less
 *              than the starting point plus one maximum character width, then
 *              the right margin defaults to the rightmost side of the page
 *              space.
 *
 *                 USE_RT_EDGE  Use the right page edge as the margin.
 *
 *              The number of columns may be bitwise ORed the following:
 *
 *                 FROM_RT_EDGE Specifies <usRtMCol> as the number of
 *                              columns from the RIGHT page space edge.
 *
 *                 REL_LEFT     Specifies <usRtMCol> as the number of
 *                              columns from the LEFT MARGIN given by <usCol>.
 *
 *
 * psz          The null terminated string that is to be printed.
 *
 *
 *
 * Return Value:
 *              0    if successful.
 *
 * Errors:
 *    Could be one of the following values:
 *              ITIRPT_INVALID_PARM
 *              ITIPRT_PRINTING_ERR
 *
 * Comments:
 *    Currently WRAP works only for LEFT justified text; I have not yet
 *    figured out what it means to wrap centered or right justified text;
 *    However, if LNINC is used with centered or right then the current
 *    line number is incremented.
 *
 */
USHORT EXPORT ItiPrtPutTextAt (USHORT usLn,        USHORT usCol,
                               ALIGNMENT algnMode, USHORT usRtMarginCol,
                               PSZ psz)
   {
   LONG    lRetVal  = 0L;
   LONG    lNumChar = 0L;
   LONG    lLtM;
   LONG    lRtM;
   LONG    lCharBetweenLR = 0L;
   POINTL  ptl;
   POINTL  ptlTmp;
   BOOL    bClip = TRUE;
   BOOL    bDone = FALSE;
   PSZ     pszNextStart;
   CHAR FAR * pLastChar;
   LONG    lNumChrForThisLine = 0L;
   INT     i = 0;
   INT     j;
   PSZ     pszChk;
   INITCHK(ITIPRT_NOT_INITIALIZED);

   DBGMSG(ItiPrtIndentedStr("IN ItiPrtPutTextAt"));


   /* -- Check if we were given a string with no-nos. */
   if (psz != NULL)
      {
      pszChk = psz;
      while ((pszChk != NULL) && (*pszChk != '\0'))
         {
         if ((*pszChk == 0x0d) || (*pszChk == 0x0a))
            *pszChk = ' ';
         pszChk++;
         }
      }
   else
      return 0;


   ptl.y = ConvertLnToLnCoord (usLn);
   ptl.x = ConvertColToColCoord (usCol);

//   if ((usCol >= FROM_RT_EDGE)                     /*   |        <--|    */
//       && (usCol < (CURRENT_COL - MAX_PAPER_COL)))
//      {
//      usCol = usCol - FROM_RT_EDGE;
//      ptl.x = ConvertColToColCoord (usCol);
//      ptl.x = pgfmt.sizlPgDim.cx - ptl.x;
//      }
//   else
//      {
//      ptl.x = ConvertColToColCoord (usCol);
//      }
//
   lLtM = ptl.x;

   /* -- Move to the given starting point so that  *
    * -- the right margin point may be calculated. */
   GpiMove(prepi->hps, &ptl);


   ptlTmp = ptl;
   ptlTmp.x = ptl.x + fm.lMaxCharInc;
   if ( !PtInDisplaySpace(ptlTmp) )
      {
      DBGMSG("ERROR: Invalid text location point attempted.");
      return (ITIRPT_INVALID_PARM);
      }



   lRtM = ConvertColToColCoord (usRtMarginCol);

//   /* -- Check the right margin. */
//   if (usRtMarginCol >= FROM_RT_EDGE)               /*   |        <--|    */
//      {
//      usRtMarginCol = usRtMarginCol - FROM_RT_EDGE;
//      lRtM = ConvertColToColCoord (usRtMarginCol);
//      lRtM = pgfmt.sizlPgDim.cx - lRtM;
//      }
//   else if (usRtMarginCol >= REL_LEFT)              /*   |    .-->   |    */
//      {
//      usRtMarginCol = usRtMarginCol - REL_LEFT;
//      lRtM = ConvertColToColCoord (usRtMarginCol);
//      lRtM -= (pgfmt.lLeftMargin * fm.lAveCharWidth); //7 JUL 92
//      lRtM = lRtM + ptl.x;
//      }
//   else if (usRtMarginCol == USE_RT_EDGE)           /*   |         ->|<-  */
//      {
//      lRtM = pgfmt.sizlPgDim.cx;
//      }
//   else
//      {
//      lRtM = ConvertColToColCoord (usRtMarginCol);
//      }
//
//   /* -- Check for a valid right margin value. */
//   if (lRtM > pgfmt.sizlPgDim.cx)
//      lRtM = pgfmt.sizlPgDim.cx;


   ptlTmp.x = lRtM; /* absolute X location */


   if ( ! PtInDisplaySpace(ptlTmp) )
      {
      DBGMSG("WARNING: Invalid right margin attempted, defaulting to edge.");
      lRtM = pgfmt.sizlPgDim.cx;
      }

   /* -- Check if the right margin is still too close to the left margin. */
   if (lRtM < (ptl.x + fm.lMaxCharInc))
      {
      DBGMSG("ERROR:  Right margin too close to left margin.");
      return (ITIPRT_PRINTING_ERR);
      }




   /* -- Check the given string. */
   if ((psz == NULL) || ((*psz) == '\0'))
      {
      DBGMSG("WARNING: Null string given as a parameter.");
      return 0;
      }


   /* -- Set initial length value and starting point of text. */
   lNumChar = (LONG)ItiStrLen(psz);
//   pszNextStart = psz;

   /* -- Select clip or wrap action IFF left justification. */
   if ( (WRAP & algnMode) && (!(CLIP & algnMode))
       && (algnMode>=LEFT) && (algnMode<NONE) )
      bClip = FALSE;      /* ie. use wrap mode. */

   /* -- How many chars between the margins? */
//   lCharBetweenLR = ( ((INT)lRtM - (INT)ptl.x) / (INT)fm.lAveCharWidth);
   lCharBetweenLR = (lRtM - lLtM) / fm.lAveCharWidth;

   /* -- Adjust number to write if in clip mode. */
   if (lNumChar > lCharBetweenLR)
      if (bClip)
         lNumChar = lCharBetweenLR;

/*
 *  Format the text according to the algnMode value.
 */
   /* -- Select the justification mode. */
   if ((algnMode >= NONE) || (algnMode < CENTER))
      {/* use NONE */
      /* -- Move to the given point and write the string. */
      GpiMove(prepi->hps, &ptl);
      lRetVal = GpiCharString(prepi->hps, lNumChar, psz);
      GPIERR_CHK(lRetVal,"ERROR trying to write string.");

      if (LNINC & algnMode)
         /* -- Increment to the next line. */
         ItiPrtIncCurrentLn();
      }

   else if (algnMode >= LEFT)
      {
      /* use LEFT */
      bDone = FALSE;
      pszNextStart = psz;
      do
         {
         /* -- Move to the given point. */
         GpiMove(prepi->hps, &ptl);

         lNumChar = (LONG) ItiStrLen (pszNextStart);
         if (lNumChar < lCharBetweenLR)
            {
            lRetVal = GpiCharString(prepi->hps, lNumChar, pszNextStart);
            GPIERR_CHK (lRetVal,"ERROR trying to write string.");
            bDone = TRUE;
            }

         else
            {
            /* -- For word wrapping on the line */
            lNumChrForThisLine = lCharBetweenLR;
            pLastChar = &pszNextStart[lNumChrForThisLine];        //Added 24Apr
                                                                  //
            if ((pszNextStart[lNumChrForThisLine] != ' ')         //
                && (pszNextStart[lNumChrForThisLine] != '\0'))    //
               {                                                  //
               while ((pLastChar != pszNextStart)                 //
							 && !IsAWordBreakChr( (*pLastChar) ) )			//

//                      &&((*pLastChar) != ' ')                     //
//                      &&((*pLastChar) != ',')                     //Added 16Mar
//                      &&((*pLastChar) != ';')                     // "
//                      &&((*pLastChar) != '.')                     // "
//                      &&((*pLastChar) != '\n')                    // "
//                      &&((*pLastChar) != '\r')                    //Added 9 Apr 1992
//                      &&((*pLastChar) != '!') )                   // "
                  {                                               //Added 24Apr
                  pLastChar--;                                    //
                  lNumChrForThisLine--;                           //
                  }
               }
                                                                  
            //Added 17 Jun 91
            /* -- If text is too long (ie no white space between margins... */
            if (lNumChrForThisLine == 0)
               lNumChrForThisLine = lCharBetweenLR;


            /* -- Write the text for this line. */
            lRetVal = GpiCharString(prepi->hps, lNumChrForThisLine, pszNextStart);
            GPIERR_CHK(lRetVal,"ERROR trying to write string.");

            /* -- Move pointer to start of next block of text. */
            pszNextStart += lNumChrForThisLine;

            /* -- Skip blanks at the start of a line. */       //Added
            while ((pszNextStart[0] != '\0')                   //   24 April
                   && (pszNextStart[0] == ' '))                //
               pszNextStart++;                                 //

            /* -- Don't do if only the null character is remaining. */
            if ((*pszNextStart) == '\0')
               bDone = TRUE;
            }

         if (bClip)
            bDone = TRUE;

         if ((LNINC & algnMode) || (!bClip))
            /* -- Wrap so increment to the next line. */
            ItiPrtIncCurrentLn();


         /* -- Reset to the left margin and the line for the next write. */
         ptl.x = lLtM;
//         ptl.y = ptlCurrentLoc.y;
         ptl.y -= pgfmt.lTxtLnHgt;  // 7 JUL 92


         } while (!bDone);
      }

   else if (algnMode >= RIGHT)
      {/* use RIGHT */
      ptl.x = JustifiedXposFor (RIGHT, psz, &ptlTmp.x, lLtM, lRtM);
      pszNextStart = psz;

      /* -- If needed, adjust left margin and which part of string to use.*/
      if (ptl.x < lLtM)
         {
         ptl.x = lLtM;
         while((*pszNextStart) != '\0')
            pszNextStart++;
         for(i=0; i<(INT)lNumChar; i++)
            pszNextStart--;
         }

      /* -- Move to the given point and write the string. */
      GpiMove(prepi->hps, &ptl);
      lRetVal = GpiCharString(prepi->hps, lNumChar, pszNextStart);
      GPIERR_CHK(lRetVal,"ERROR trying to write string.");

      if (LNINC & algnMode)
         /* -- Increment to the next line. */
         ItiPrtIncCurrentLn();
      }

   else
      {/* use CENTER */
      ptl.x = JustifiedXposFor (CENTER, psz, &ptlTmp.x, lLtM, lRtM);
      pszNextStart = psz;

      /* -- If needed, adjust left margin and which part of string to use.*/
      if (ptl.x < lLtM)
         {
         ptl.x = lLtM;
         j = ((INT)ItiStrLen(psz) - (INT)lNumChar) / 2;
         for(i=0; i<j; i++)
            pszNextStart++;
         }

      /* -- Move to the given point and write the string. */
      GpiMove(prepi->hps, &ptl);
      lRetVal = GpiCharString(prepi->hps, lNumChar, pszNextStart);
      GPIERR_CHK(lRetVal,"ERROR trying to write string.");

      if (LNINC & algnMode)
         /* -- Increment to the next line. */
         ItiPrtIncCurrentLn();
      }


   lRetVal = GpiQueryCurrentPosition(prepi->hps, &ptlCurrentLoc);
   GPIERR_CHK(lRetVal,"ERROR trying to query the current position.");


   DBGMSG(ItiPrtIndentedStr("EXIT ItiPrtPutTextAt"));

   if (enumReportStatus != NODEV)
      enumReportStatus = PRINTING;

   return 0;
   }/* END OF FUNCTION ItiPrtPutTextAt */






/* ------------------------------------------------------------------------
 * ItiPrtTranslateLocationValue
 *
 *    This function translates line and column values into a point
 *    location for use by other ItiPrt functions.
 *
 * Parameters:    USHORT usLine   Requested display space line.
 *                USHORT usCol    Requested character column on the line.
 *
 * Return value:
 *     A POINTL structure that contains the XY coordinates of
 *     the Baseline point of the requested line and column.
 *
 * Errors:
 *     Error point coordinates for XY are used.
 *
 * Comments:
 *     If usLine is zero, or larger than the number of lines in the
 *     page's display space, than the current line number is used.
 *     If the column value is greater than what is within the page
 *     edges then column 0 is used.
 */
USHORT EXPORT ItiPrtTranslateLocationValue
                 (USHORT usLn, USHORT usCol, PPOINTL pptl)
   {
   INITCHK(ITIPRT_INIT_ERR);

   DBGMSG(ItiPrtIndentedStr("IN ItiPrtTranslateLocationValue"));

   (*pptl).x = PT_ERROR_VAL;
   (*pptl).y = PT_ERROR_VAL;

   if (usLn == 0)
      {
      usLn = usCurDisplaySpcLnNum;
      }

   if (usLn > (USHORT)pgfmt.lNumDisplayLns)
      {
      DBGMSG("WARNING: Line not in display space, using default.");
      usLn = usCurDisplaySpcLnNum;
      }

   /* -- Find the Y coordinate of usLn. */
   if (! StartOfPgLn( (LONG)usLn, pptl) )
      {
      DBGMSG("ERROR: Failed to find the start point of usLn.");
      return (ITIPRT_COORDINATE_ERR);
      }

   /* -- Determine if the given column is on the page. */
   if ((INT)usCol > ( ((INT)pgfmt.sizlPgDim.cx) / ((INT)fm.lAveCharWidth)))
      {
      DBGMSG("WARNING: Column is not on the page, defaulting to column 0.");
      (*pptl).x = 0L;
      }
   else
      /* -- Compute the X coordinate for the column. */
      (*pptl).x = (LONG)((USHORT)fm.lAveCharWidth * usCol);

   DBGMSG(ItiPrtIndentedStr("EXIT ItiPrtTranslateLocationValue"));
   return (0);
   }/* END OF FUNCTION ItiPrtTranslateLocationValue */




/* ------------------------------------------------------------------------
 * ItiPrtDrawSeparatorLn
 *
 *    This function draws a separator line FromXpos ToXpos on the
 *    given display page line number.  The X positions can be values
 *    returned from ItiPrtConvertInchesToPgUnits or ItiPrtConvertColWidthsToPgUnits.
 *
 * Parameters: usLnNum        Display space line number.
 *             usFromCol      Starting column number.
 *             usToCol        Ending column number, or USE_RT_EDGE.
 *
 *
 * Return value: 0 if successful otherwise...
 *
 * Errors:     ITIPRT_COORDINATE_ERR
 *
 * Comments:   If the given line number is 0 then the current line is used.
 *             Also note that the current line number is NOT changed, ie
 *             you have to call ItiPrtIncCurrentLn.  This done so that it
 *             is convenient for you to draw more than one separator line
 *             segment on the same line.
 */
USHORT EXPORT ItiPrtDrawSeparatorLn (USHORT usLnNum, USHORT usFromCol, USHORT usToCol)
   {
   POINTL  ptl;
   LONG    lFromColCoord = 0L;
   LONG    lToColCoord   = 0L;
   INITCHK(ITIPRT_NOT_INITIALIZED);

   DBGMSG(ItiPrtIndentedStr("IN ItiPrtDrawSeparatorLn"));

//   if (usLnNum == 0)
//      usLnNum = usCurDisplaySpcLnNum;
//
//   /* -- First find the X,Y starting location of selected display line. */
//   if (! StartOfPgLn( ((LONG)(usLnNum) + pgfmt.lNumHdrLns), &ptl) )
//      {
//      DBGMSG("FAILED to locate separator line.");
//      return (ITIPRT_COORDINATE_ERR);
//      }

   ptl.y = ConvertLnToLnCoord (usLnNum);


   /* -- Convert column values into X-coordinate values. */
//   lFromColCoord = ConvertColToColCoord (usFromCol);

   if ((usFromCol >= FROM_RT_EDGE)               /*   |        <--|    */
       && (usFromCol < (CURRENT_COL - MAX_PAPER_COL)))
      {
      usFromCol = usFromCol - FROM_RT_EDGE;
      lFromColCoord = ConvertColToColCoord (usFromCol);
      lFromColCoord = pgfmt.sizlPgDim.cx - lFromColCoord;
      }
//   else if (usFromCol >= REL_LEFT)              /*   |    .-->   |    */
//      {
//      usFromCol = usFromCol - REL_LEFT;
//      lFromColCoord = ConvertColToColCoord (usFromCol);
//      lFromColCoord = lFromColCoord;
//      }
//   else if (usFromCol == USE_RT_EDGE)           /*   |         ->|<-  */
//      {
//      lFromColCoord = pgfmt.sizlPgDim.cx;
//      }
   else
      {
      lFromColCoord = ConvertColToColCoord (usFromCol);
      }



//////

   /* -- Check the right margin. Same block of code as in ...PutTextAt. */
   if (usToCol >= FROM_RT_EDGE)               /*   |        <--|    */
      {
      usToCol = usToCol - FROM_RT_EDGE;
      lToColCoord = ConvertColToColCoord (usToCol);
      lToColCoord = pgfmt.sizlPgDim.cx - lToColCoord;
      }
   else if (usToCol >= REL_LEFT)              /*   |    .-->   |    */
      {
      usToCol = usToCol - REL_LEFT;
      lToColCoord = ConvertColToColCoord (usToCol);
      lToColCoord = lToColCoord + lFromColCoord;
      }
   else if (usToCol == USE_RT_EDGE)           /*   |         ->|<-  */
      {
      lToColCoord = pgfmt.sizlPgDim.cx;
      }
   else
      {
      lToColCoord = ConvertColToColCoord (usToCol);
      }


//   if (usToCol == USE_RT_EDGE)                /*   |         ->|<-  */
//      {
//      lToColCoord = pgfmt.sizlPgDim.cx;
//      }
//   else
//      {
//      lToColCoord = ConvertColToColCoord (usToCol);
//      }


   /* -- Now using that display line's Y-coordinate... */
   SeparatorLnSeg (ptl.y, lFromColCoord, lToColCoord);

   DBGMSG(ItiPrtIndentedStr("EXIT ItiPrtDrawSeparatorLn"));
   return 0;
   }/* END OF FUNCTION */





/* ------------------------------------------------------------------------
 * ItiPrtNewPg
 *
 *    This function
 *
 * Parameters:    none
 *
 * Return value:
 *
 * Comments:
 *     Sends needed device escape codes and sets the ReportStatus.
 */
BOOL EXPORT ItiPrtNewPg (VOID)
   {
   LONG    lResult = 0;
   BOOL    bRetVal = TRUE;
   INITCHK(FALSE);

   DBGMSG(ItiPrtIndentedStr("IN ItiPrtNewPg"));


   if (bActive)  // also implies is enumReportStatus != NODEV
      {
      if (usCurrentPg == 1)
         PrtFooter();

      enumReportStatus = FORMATTING;             /* Set the report status. */

      /* -- Eject page from printer and prepare for new page. */
      lResult = DevEscape(prepi->hdc, DEVESC_NEWFRAME, 0L, NULL, NULL, NULL);
      if (lResult == DEV_OK)
         {
         enumReportStatus = PRINTING;
         usCurrentPg++;                          /* Increment page number. */
         usCurDisplaySpcLnNum = 1; /* Reset the display space line number. */
         bRetVal = (PrtHeader() == 0);           /* Set header onto page.  */
         bRetVal = (PrtFooter() | bRetVal);      /* Set footer onto page.  */
         }
      else
         {
         /* -- do error processing here. */
         enumReportStatus = PAUSED;
         ItiPrtSetError("ERROR during DevEscape invocation for a new page.");
         bRetVal = FALSE;
         }
      }

   DBGMSG(ItiPrtIndentedStr("EXIT ItiPrtNewPg"));
   return (bRetVal);

   }/* END ItiPrtNewPg */






USHORT EXPORT ItiPrtActive (BOOL bActivate)
   {
   SIZEL sizl = {2400L, 3180L}; /* Use the page size of a typ. device. */


   if (bActivate)
      {
      prepi->hps = hpsPrtActive;
      prepi->devi.lPsOptions = lOpPrtActive;
      bActive = TRUE; /* Note this is NOT bActivATE */
      }
   else                            // deactivate the module;
      {                            // also implies enumReportStatus == NODEV
      /* -- Save original settings. */
      hpsPrtActive = prepi->hps;
      lOpPrtActive = prepi->devi.lPsOptions;
           bActive = FALSE; /* Note this is NOT bActivATE. */

      /* -- Set the presentaion space options. */
      prepi->devi.lPsOptions = (UNITS | GPIT_NORMAL);

      prepi->hps = GpiCreatePS(prepi->hab, NULL, &sizl, prepi->devi.lPsOptions);
      if (prepi->hps == GPI_ERROR)
         {/* then check for error. */
         DBGMSG("EXIT ItiPrtActive: FAILED ps");
         return ItiPrtSetError("Failed to deactivate ItiPrt. ");
         }
      }

   return (0);

   }/* END ItiPrtActive */





/* ------------------------------------------------------------------------
 * ItiPrtBeginReport
 *
 *    This function
 *
 * Parameters:  pszDocumentName
 *
 * Return value:
 *
 * Comments:
 */
USHORT EXPORT ItiPrtBeginReport (PSZ pszDocumentName)
   {
   LONG   lResult = 0L;
   LONG   lCnt    = 0L;
   USHORT us      = 0;
   BOOL   bVal    = FALSE;
   CHAR   szRes[64] = "";
   CHAR   szMsg[128] = "\t Failed to start document: ";
   INITCHK(FALSE);

   DBGMSG("IN ItiPrtBeginReport");


   if (bActive)
      {
      if ((enumReportStatus == NOTSTARTED)
          || (enumReportStatus == ENDED)
          || (enumReportStatus == NODEV)   )
         {
         enumReportStatus = FORMATTING;          /* Set the report status. */

         /* -- START the report document. */
         lResult = DevEscape(prepi->hdc, DEVESC_STARTDOC,
                             (LONG)ItiStrLen(pszDocumentName),
                             pszDocumentName,
                             NULL, NULL);
         if (lResult == DEV_OK)
            {
            enumReportStatus = STARTED;

            bVal = GpiErase(prepi->hps);  // Added 30 Sep 91
            if (bVal == (BOOL)GPI_ERROR)
               {
               ItiPrtSetError("Warning, GpiErase Error in ItiPrtBeginReport.");
               }

            usCurrentPg = 1;
            usCurDisplaySpcLnNum = 1;

            /* the following initializations were added on 14 Aug 1991 */
            usCurrentColNum = 0;
            ItiPrtStartOfDisplayLn (usCurDisplaySpcLnNum, &us);
            bInitHF = FALSE;
            return (0);
            }
         else
            {
            enumReportStatus = NOTSTARTED;

            ItiStrLONGToString(lResult, szRes, sizeof szRes);
            ItiStrCat(szMsg, pszDocumentName, sizeof szMsg);
            ItiStrCat(szMsg, ", DevEscape returned ", sizeof szMsg);
            ItiStrCat(szMsg, szRes, sizeof szMsg);

            ItiPrtSetError(szMsg);
            DBGMSG("EXIT ItiPrtBeginReport");
            return (ITIPRT_INIT_ERR);
            }
         }
      }
   else
      {
      DBGMSG("EXIT ItiPrtBeginReport");
      enumReportStatus = NODEV;

      return (0);
      }

   } /* END ItiPrtBeginReport */






/* ------------------------------------------------------------------------
 * ItiPrtEndReport
 *
 *
 * Parameters:    none
 *
 * Return value:
 *
 * Comments:
 *    Sends needed device escape codes and sets the ReportStatus.
 */
BOOL EXPORT ItiPrtEndReport (VOID)
   {
   LONG   lResult = 0L;
   LONG   lCnt    = 0L;
   USHORT us      = 0;
   CHAR   achEscSeq[LEN_ESC_SEQ] = "";
   INITCHK(FALSE);

   DBGMSG("IN ItiPrtEndReport");

   if (bActive)
      {
      if ((enumReportStatus == ENDED) || (enumReportStatus == NODEV))
         return (TRUE);

      enumReportStatus = FORMATTING;             /* Set the report status. */

      /* -- END the report document. */
      lResult = DevEscape(prepi->hdc, DEVESC_ENDDOC, 0L, &achEscSeq[0], &lCnt, &achEscSeq[0]);
      if (lResult == DEV_OK)
         {
         enumReportStatus = ENDED;
         DBGMSG("EXIT ItiPrtEndReport");
         return TRUE;
         }
      else
         {
         /* -- do error processing here. */
         enumReportStatus = PAUSED;
         ItiPrtSetError("Failed to close the document.");
         DBGMSG("EXIT ItiPrtEndReport");
         return FALSE;
         }

      /* -- Close device context and presentation space. */
      us = UnSetDCandPS();
      if (us != 0)
         {
         us = ItiPrtSetError
                     ("Failed to close device context and presentation space.");
         return (FALSE);
         }
      }
   else
      return (TRUE);


   }/* END ItiPrtEndReport */




USHORT EXPORT ItiPrtWrappedLnCnt (PSZ pszString,
                                  USHORT usColWidth,
                                  BOOL bIgnorePgDim)
   {
   PSZ pszStart = pszString;
   PSZ pszStop;
   USHORT usLnCnt = 0;
   USHORT us, usNumColPerPg;


   if (usColWidth == 0)
      return 0;

   usNumColPerPg = (USHORT)(pgfmt.sizlPgDim.cx / fm.lAveCharWidth);

   if ((usColWidth > usNumColPerPg) && !bIgnorePgDim)
      usColWidth = usNumColPerPg;

   while ((pszStart != NULL) && (*pszStart != '\0'))
      {
      pszStop = pszStart;
      us = 0;
      while ((pszStart != NULL) && (*pszStart != '\0') && (us != usColWidth))
         {
         pszStart++;
         us++;
         }
      usLnCnt++;

      if (*pszStart != '\0')
         {
         while ((pszStart != pszStop) && (!IsAWordBreakChr(*pszStart)) )
            {
            pszStart--;
            }
//       if (pszStart == pszStop)
//          usLnCnt--;
         }

      }

   return usLnCnt;

   } /* End of function ItiPrtWrappedLnCnt */

