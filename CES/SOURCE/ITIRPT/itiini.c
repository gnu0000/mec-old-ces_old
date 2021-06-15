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
#define     INCL_WIN

#include    "..\include\iti.h"     

#include    "..\include\itibase.h"
#include    "..\include\itierror.h"     
#include    "..\include\itiutil.h"
#include    "..\include\itirdefs.h"

#include    <stdio.h>

#include    "ItiIni.h"




#define INI_FILE 1
#define INI_APP  2
#define INI_KEY  3
#define INI_VAL  4

#define BUF_SIZE  256

             
/* ======================================================================= *\
 *                                             MODULE'S GLOBAL VARIABLES   *
\* ======================================================================= */
static CHAR     szCompileDateTime[] = __DATE__ " " __TIME__ ;
static CHAR     szAppName[] = " CES Initialization File Utility ";
static INT      iLevel = 0;
static HAB      hab;
static HMQ      hmq;
static HMODULE  hmod;
static HINI     hiniPrf;




/* ======================================================================= *\
 *                                              MODULE UTILITY FUNCTIONS   *
\* ======================================================================= */




USHORT ItiIniStatus (PSZ pszReportName, PSZ pszBuffer, INT iBufferSize)
   {
   /* not yet implemented */
   return 0;
   }


VOID Show (HINI hiniPrf)
   {
   USHORT us;
   CHAR  szBuf[BUF_SIZE] = "";
   CHAR  szKey[BUF_SIZE] = "";
   CHAR  szVal[BUF_SIZE] = "";
   PSZ   pszBuf;
   PSZ   pszKey;
   PSZ   pszVal;
   ULONG ulKeyLen = BUF_SIZE;
   ULONG ulValLen = BUF_SIZE;
   ULONG ulBufLen = BUF_SIZE;


   PrfQueryProfileData(hiniPrf, NULL, NULL, &szBuf[0], &ulBufLen);
   pszBuf = &szBuf[0];
   while (*(pszBuf+1) != '\0')
      {
      printf ("\n");
      if (*pszBuf == '\0')
         pszBuf++;
      printf (pszBuf);
      printf ("\n");

      /* -- Show key strings. */
      ulKeyLen = BUF_SIZE;
      PrfQueryProfileData(hiniPrf, pszBuf, NULL, &szKey[0], &ulKeyLen);
      pszKey = &szKey[0];
      while (*(pszKey+1) != '\0')
         {
         printf ("   ");
         if (*pszKey == '\0')
            pszKey++;
         printf (pszKey);


         /* -- Show Val strings. */
         ulValLen = BUF_SIZE;
         PrfQueryProfileData(hiniPrf, pszBuf, pszKey, &szVal[0], &ulValLen);
         pszVal = &szVal[0];
         while (*(pszVal+1) != '\0')
            {
            printf (" = ");
            if (*pszVal == '\0')
               pszVal++;
            printf (pszVal);
            while (*pszVal != '\0')
               pszVal++;
            }
         pszVal = &szVal[0];
         for (us = 0; (ULONG)us < ulValLen; us++)
            {
            *pszVal = '\0';
            pszVal++;
            }
         pszVal = &szVal[0]; /* reset for new Val strings. */



         printf ("\n");
         while (*pszKey != '\0')
            pszKey++;
         }
      pszKey = &szKey[0];
      for (us = 0; (ULONG)us < ulKeyLen; us++)
         {
         *pszKey = '\0';
         pszKey++;
         }
      pszKey = &szKey[0]; /* reset for new Key strings. */


      /* -- Advance to the next Application section. */
      while (*pszBuf != '\0')
         pszBuf++;
      }

   }/* End of Function Show */
   




VOID DisplayUsage (VOID)
   {
   printf("\n An initialization file is divided into subsections as follows:  \n\n");

   printf("\r      SectionName  \n");
   printf("\r          KeyName = KeyValue \n\r");

   printf("\n where each section may have multiple unique KeyNames.\n");
   printf("\r Note that this utility program will NOT work on OS/2 system INI files.\n\n\r");


   printf("\n  To ADD or REPLACE a key value in a section: \n\r");

   printf("\n     ItiIni  [path]IniFileName  SectionName  KeyName  KeyValue \n\n");
   printf("\r  Note if KeyValue contains spaces then it must be a \"quoted string\".\n\n\r");


   printf("\n  To VIEW the contents of the ini file: \n\r");

   printf("\n     ItiIni  [path]IniFileName \n\n");
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
USHORT _cdecl main (int argc, char * argv[])
   {
   USHORT   us = 0;
   CHAR szMessage[128] = "";


//   COMPILEDDLLDATE( "ItiIni.EXE" );
   /* Misnomer, I know. */

   hab = WinInitialize(0);
   if (hab == NULL)           
      {
      return (13);
      }

//   hmq = WinCreateMsgQueue(hab, DEFAULT_QUEUE_SIZE);
//   if (hmq == NULL)           
//      {
//      return (13);
//      }



   if (argc < 2)
      {
      DisplayUsage();
      return 13;
      }

   us = ItiOpenProfile(hab, argv[INI_FILE], &hiniPrf);

   if (us == 0)
      {
      switch (argc)
         {
         case 2: /* display the inifile. */
            Show(hiniPrf);
            break;


         
         case 5: /* write to the ini file. */
            us = ItiWriteProfileString (hiniPrf,
                                        argv[INI_APP],
                                        argv[INI_KEY],
                                        argv[INI_VAL]);
            break;


         default:
            DisplayUsage();
            break;

         }/* end switch */

      us = ItiCloseProfile(&hiniPrf);
      }
   else
      {
      printf("\n   Unable to find/open ");
      printf(argv[INI_FILE]);
      printf(" initialization file. \n ");
      }



   /* -- Finished using the DLL. */
   ItiDllFreeDll (hmod);
//   WinDestroyMsgQueue(hmq);
   WinTerminate(hab);

   return (us);
   }/* END OF main */
