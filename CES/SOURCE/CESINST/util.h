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
 * util.h
 *
 * (C) 1992 Info Tech Inc.
 * This file is part the EBS module
 */

PVOID FreeData (PVOID p);
PSZ StrCpy (PSZ pszDest, PSZ pszSource, USHORT usDestMax);

USHORT ReadString (FILE *fp, PSZ *psz);
//BOOL   SetFlag (PFLAG pfFlags, USHORT uFlagIndex, BOOL bSet);
//BOOL   IsFlag (FLAG fFlags, USHORT uFlagIndex);

USHORT Msg (PSZ p1, PSZ p2, PSZ p3, PSZ p4, PSZ p5);
USHORT Warning (PSZ p1, PSZ p2, PSZ p3, PSZ p4, PSZ p5);
USHORT Error (PSZ p1, PSZ p2, PSZ p3, PSZ p4, PSZ p5);
USHORT Error1 (PSZ p1, PSZ p2);
//PSZ    DateNumStr (DATE dt);
PSZ    NumStr (USHORT u);

PVOID MyDosAllocSeg   (USHORT uSize);
PVOID MyDosReAllocSeg (PVOID p, USHORT uSize);
VOID  MyDosFreeSeg  (PVOID p);
PVOID MyDosAlloc2   (USHORT uSize, PSZ pszFile, USHORT usLine);
PVOID MyDosReAlloc2 (PVOID p, USHORT uSize, PSZ pszFile, USHORT usLine);
PSZ MyStrDup2 (PSZ psz, PSZ pszFile, USHORT usLine);
USHORT Beep (void);
USHORT GetConfigInfo (PSZ pszCfgFile, PSZ pszSection, PSZ *ppszData, BOOL bSkipBlank);

BOOL DriveExists (char cDrive);

PSZ GetNextPathElement (PSZ pszPath);



ULONG GetDriveFreeSpace (char cDrive);

USHORT ReplaceParams (PSZ    pszDest,
                      PSZ    pszSource,
                      USHORT usDestMax,
                      PSZ    apszTokens [], 
                      PSZ    apszValues [], 
                      USHORT usNumValues);

BOOL MakePath (PSZ pszPathName);

BOOL CheckForFile (PSZ pszFile, char cDrive);

