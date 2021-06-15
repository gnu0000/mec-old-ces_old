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


BOOL GetDBPath (CHAR cDestDrive, 
                PSZ pszDestPath, 
                PSZ pszDBPath, 
                BOOL bNoCheck);


BOOL GetProdDatabaseSize (PUSHORT pusDBSize, 
                          PUSHORT pusDBPct, 
                          CHAR cDBDrive, 
                          BOOL bNoCheck);


BOOL GetImpDatabaseSize (PUSHORT pusDBSize, 
                         PUSHORT pusDBPct, 
                         USHORT usProdDBSize, 
                         CHAR cDBDrive, 
                         BOOL bNoCheck);


BOOL ConfirmInfo (USHORT usProdDB, 
                  USHORT usProdPct, 
                  USHORT usImpDB, 
                  USHORT usImpPct);


BOOL CreateSQL (PSZ    pszFile,
                PSZ    pszPath,
                PSZ    pszDataDevice, 
                PSZ    pszDataFile, 
                PSZ    pszLogDevice, 
                PSZ    pszLogFile, 
                USHORT usProdDBSize, 
                USHORT usProdPct, 
                USHORT usImpDBSize, 
                USHORT usImpPct);
