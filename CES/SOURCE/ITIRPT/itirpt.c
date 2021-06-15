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


#define     INCL_ERRORS 
#define     INCL_BASE
#define     INCL_DOS

#include    "..\include\iti.h"     

#include    "..\include\itibase.h"
#include    "..\include\itierror.h"     
#include    "..\include\itiglob.h"
#include    "..\include\itiutil.h"
#include    "..\include\itidbase.h"
#include    "..\include\itifmt.h"
#include    "..\include\itirdefs.h"

#include    "..\include\itirpt.h"

#include    <stdio.h>


             
/* ======================================================================= *\
 *                                             MODULE'S GLOBAL VARIABLES   *
\* ======================================================================= */
static CHAR     szCompileDateTime[] = __DATE__ " " __TIME__ ;
static CHAR     szAppName[] = "CES Report Module";
static INT      iLevel = 0;
static HAB      hab;
static HMQ      hmq;
static HMODULE  hmod;
static PGLOBALS pglobRpt;

static USHORT (EXPORT *pfnRptFunc) (HAB, HMQ, INT, CHAR **);
static BOOL   (EXPORT *fnVersion)  (USHORT);




/* ======================================================================= *\
 *                                              MODULE UTILITY FUNCTIONS   *
\* ======================================================================= */
/* ------------------------------------------------------------------------
 * RptSetError
 *
 *    This function screens the returned error codes and sends pszErrorText
 *    to the debug window.
 *
 * Parameters: errid          The error ID value.
 *
 *             pszErrorText   A pointer to the error message text string.
 *
 * Return value: 0 if severity level of the errid is just a warning or
 *               the USHORT error value of the errid.
 *
 * Comments: Note some Win functions return ERRORID values.  However,
 *           you may create your own ERRORID value by using the macro
 *           MAKEERRORID(severityLevel, errorValue) where severityLevel
 *           can be any of the following:
 *
 *                       SEVERITY_ERROR
 *                       SEVERITY_NOERROR
 *                       SEVERITY_SEVERE
 *                       SEVERITY_WARNING
 *                       SEVERITY_UNRECOVERABLE
 *
 *            errorValue is a USHORT value.
 */
USHORT FPL RptSetError(ERRORID errid, PSZ pszErrorText)
   {
   USHORT  usErrSeverity;
   USHORT  usErrValue;
   CHAR    szMsgBuff[1024] = "";


   DBGMSG("In RptSetError");

   usErrValue = ERRORIDERROR(errid);
   usErrSeverity = ERRORIDSEV(errid);


   /* -- Development version of error message set up. */
#ifdef DEVELOPMENT_CODE 
   DBGMSG(pszErrorText);
#else
   sprintf(szMsgBuff, " RptErrCode: %d, %s", usErrValue, pszErrorText);
   ItiErrWriteDebugMessage (szMsgBuff);
#endif
   /* insert other error message handling here. */

   DBGMSG("Exit Leaving RptSetError");
   if (ERRORIDSEV(errid) == SEVERITY_WARNING)
      return 0;
   else
      {
      return usErrValue;
      }

   }/* end of RptSetError */


/* ------------------------------------- End of general module utilities. */



USHORT FPL AccessRptDLL (PSZ pszRptName, PHMODULE phmod)
   {
   USHORT us = SEVERITY_UNRECOVERABLE;
   USHORT usDLLVersion = 0;

   
   DBGMSG("IN AccessRptDLL");

   if (pszRptName == NULL)
      return ITIRPT_INVALID_PARM;
   else
      { /* -- Check for existence of a run-time DLL named pszRptName.  */
      (*phmod) = ItiDllLoadDll(pszRptName, RPT_EXE_VERSION);
      if ((*phmod) == 0)
         return (RptSetError(MAKEERRORID(SEVERITY_ERROR,us),
                     "Failed to Access Report DLL"));
    
    
    //  /* -- Get the DLL's version function and check DLL version. */
    //  fnVersion = ItiDllQueryProcAddress(*phmod, VER_COMPAT_FUNC);
    //  if (fnVersion == NULL)
    //     return (RptSetError(MAKEERRORID(SEVERITY_ERROR,us),
    //                        "Failed to access function in Report DLL"));
    //  else
    //     {
    //     if ( !fnVersion(RPT_EXE_VERSION) )
    //        return (RptSetError(MAKEERRORID(SEVERITY_ERROR,ITIRPT_ERR),
    //                           "Incompatable report DLL version"));
    //     }

      /* -- Get the DLL's report printing function. */
      pfnRptFunc = ItiDllQueryProcAddress(*phmod, RPT_PRT_FUNC);
      if (pfnRptFunc == NULL)
         return (RptSetError(MAKEERRORID(SEVERITY_ERROR,us),
                            "Failed to get DLL Print Report function."));
   
      DBGMSG("EXIT AccessRptDLL");
      return 0;
      }
   }/* END OF FUNCTION AccessRptDLL */






USHORT ItiRptStatus (PSZ pszReportName, PSZ pszBuffer, INT iBufferSize)
   {
   /* not yet implemented */
   return 0;
   }




VOID DisplayUsage (VOID)
   {
   CHAR szMessage[] = "Usage Message";
           /* not yet written */
   DosExit(EXIT_PROCESS, 0);
   }


/*
 *  OptionSwitch
 *     Processes a command line option switch.
 */
USHORT OptionSwitch (PSZ pszOptSwitch)
   {

   switch(pszOptSwitch[1])
      {
      case 'h':
      case 'H':
      case '?':
         /* Usage inquiry. */
         DisplayUsage();
         return 0;
         break;

      default:
         break;
      }
   return 0;
   }




















/* ======================================================================= *\
\* ======================================================================= */
static USHORT DoReport (int cmdLnArgC, char * cmdLnArgV[])
   {
   USHORT   us = 0;
   INT i, j, inx;
   CHAR   szMessage[128] = "";
   CHAR   szBuff[512] = " \n\r      CES Report CmdLn: ";


   /* -- Write the command line to the debug file. */
   for (i=0; i < cmdLnArgC; i++)
      {
      ItiStrCat(szBuff, cmdLnArgV[i], sizeof szBuff);
      ItiStrCat(szBuff, "  ", sizeof szBuff);
      }
   ItiStrCat(szBuff, "\n\r", sizeof szBuff);
   ItiErrWriteDebugMessage(szBuff);
   szBuff[0] = '\0';

   if (cmdLnArgC == 1) /* no report dll name was given! */
      return ITIRPT_ERR;


   i = 1; /* cmdLnArgV[1] should be the the DLL name or a usage request.   */
   while (i < cmdLnArgC) /* -- Check for and process any relevent options. */
      {
      if ((cmdLnArgV[i][0] == '/')||(cmdLnArgV[i][0] == '-'))
         {
         us = OptionSwitch(cmdLnArgV[i]);
         /* us = ItiRptUtGetOption(cmdLnArgV[i][1], &cmdLnArgV[i+1][0], cmdLnArgC, cmdLnArgV); */
         }

      i++; /* increment loop counter */
      }


   hab = WinInitialize(0);
   if (hab == NULL)           
      {
      DBGMSG("EXIT DoReport function:  FAILED to get hab.");
      return (ITIRPT_ERR);
      }

   hmq = WinCreateMsgQueue(hab, DEFAULT_QUEUE_SIZE);
   if (hmq == NULL)           
      {
      DBGMSG("EXIT DoReport function:  FAILED to get hmq.");
      return (ITIRPT_ERR);
      }


   pglobRpt = ItiGlobInitGlobals (hab, hmq, NULL, NULL, szAppName, "ItiRpt", VERSION_STRING);
   if (pglobRpt == NULL)
      {
      DBGMSG("Could not initialize the Globals module!\r\n");
      return (ITIRPT_ERR);
      }
   pglobRpt->usVersionMajor = VERSION_MAJOR;
   pglobRpt->usVersionMinor = VERSION_MINOR;
   pglobRpt->pszVersionString = VERSION_STRING;
   pglobRpt->ucVersionChar = VERSION_CHAR;



   if (!ItiFmtInitialize ())
      {
      DBGMSG("Could not initialize the format module!\r\n");
      return (ITIRPT_ERR);
      }

   if (!ItiDbInit (0, 3))
      {
      DBGMSG("Could not initialize the database module!\r\n");
      return (ITIRPT_ERR);
      }




   /* -- Validate pszReportName, access its DLL, check its version. */
   us = AccessRptDLL(cmdLnArgV[1], &hmod);
   if (us != 0)
      {
      ItiStrCpy(szMessage, "Invalid Report Name: ", sizeof(szMessage));
      ItiStrCat(szMessage, cmdLnArgV[1], sizeof(szMessage));
      us = RptSetError(MAKEERRORID(SEVERITY_ERROR,us), szMessage );
      return (us);
      }
   
   us = pfnRptFunc(hab, hmq, cmdLnArgC, cmdLnArgV);
   if (us != 0)
      {
      us = RptSetError(MAKEERRORID(SEVERITY_ERROR,us),
                       "Failed to generate the report.");
      return us;
      }
   else
      {
      // delay loop to allow other threads to finish their log write.
      for (inx=0; inx < 5000; inx++)
         {
         j = inx % 3;
         }
      }


   /* -- Finished using the DLL. */
   ItiDllFreeDll (hmod);
   WinDestroyMsgQueue(hmq);
   WinTerminate(hab);

   ItiStrCpy(szMessage, "-- EXIT from ", sizeof(szMessage));
   ItiStrCat(szMessage, cmdLnArgV[0], (sizeof(szMessage)-20));
   us = RptSetError((ERRORID)0L, szMessage );
   return (us);
   }/* END OF DoReport */



#define STACK_SIZE   65534U

static PVOID   pvNewStack;
static SEL     selNewStack;
static SEL     selOldStack;
static USHORT  usOldSP;
static USHORT  usNewSP;

static int     iArgCount;
static char    **ppszArgv;
static USHORT  usRet;


int _cdecl main (int argc, char *argv [])
   {
/***********************************************/
/* DO NOT USE ANY LOCAL VARIABLES!!! WE RESET  */
/* THE STACK POINTER.  USE ONLY STATIC         */
/* VARIALBES.                                  */
/***********************************************/

   /* store old argc, argv */
   iArgCount = argc;
   ppszArgv = argv;

/************************************************/
/* DO NOT USE ARGC OR ARGV AFTER THIS POINT!!!! */
/************************************************/
   pvNewStack = ItiMemAllocSeg (STACK_SIZE, SEG_NONSHARED, MEM_ZERO_INIT);
   if (pvNewStack != NULL)
      {
      selNewStack = SELECTOROF (pvNewStack);

      /* calc new stack pointer, and force it to be on an even boundary */
      usNewSP = STACK_SIZE - (sizeof (USHORT) + STACK_SIZE % 2);

      _asm 
         {
         mov selOldStack, ss     /* save old stack segment */
         mov ss,selNewStack      /* load new stack segment */
         mov usOldSP, sp         /* save old stack pointer */
         mov sp, usNewSP         /* load new stack pointer */
         }
      }
   else
      {
      DosExit (1, EXIT_PROCESS);
      return 0;
      }

   usRet = DoReport (iArgCount, ppszArgv);


   if (selOldStack != 0)
      _asm
      {
      mov ss, selOldStack     /* restore beef stock segment */
      mov sp, usOldSP         /* restore beef stock offset */
      }

   DosExit (EXIT_PROCESS, 0);
   return (int) usRet;
   }

