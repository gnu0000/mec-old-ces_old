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


extern BOOL ModifyConfigSys (char cDestDrive, PSZ pszDestPath);


extern BOOL GetConfigString (USHORT usID);
extern BOOL ConfirmConfigStrings (void);


#define LIBPATH      0
#define SERVER       1
#define DATABASE     2
#define HOSTNAME     3
#define USERNAME     4
#define PASSWORD     5
#define TMP          6
#define TEMP         7
#define STATENAME    8
#define AGENCYNAME   9

extern void SetInstEnviron (USHORT usIndex, PSZ psz);

