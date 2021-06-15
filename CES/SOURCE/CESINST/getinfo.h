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


extern BOOL CheckForFile (PSZ pszFile, char cDrive);
extern BOOL SourceDrive (PCHAR pc, PSZ pszTestFile);
extern BOOL DestDrive (PCHAR pc);
extern BOOL DestPath (PCHAR pcDestDrv, PCHAR pszDestPath);
extern BOOL Confirm (CHAR cSrcDrive, CHAR cDestDrv, PSZ pszDestPath, PSZ pszDBPath);
extern BOOL CheckCreatePath (PCHAR pcDestDrv, PSZ pszDestPath);

extern BOOL UnzipFile (PGNUWIN pgw,
                       CHAR    cSrcDrive, 
                       CHAR    cDestDrive, 
                       PSZ     pszDestPath, 
                       PSZ     pszFile,
                       FILE    *pfLog);
