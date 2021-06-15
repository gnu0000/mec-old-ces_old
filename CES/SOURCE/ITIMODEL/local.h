/*--------------------------------------------------------------------------+
|                                                                           |
|       Copyright (c) 1992 by AASHTO.  All Rights Reserved.                 |
|                                                                           |
|       This program module is part of BAMS/CES, a module of BAMS,          |
|       The American Association of State Highway and Transportation        |
|       Officials' (AASHTO) Information System for Managing Transportation  |
|       Programs, a proprietary product of AASHTO, no part of which may be  |
|       reproduced or transmitted in any form or by any means, electronic,  |
|       mechanical, or otherwise, including photocopying and recording      |
|       or in connection with any information storage or retrieval          |
|       system, without permission in writing from AASHTO.                  |
|                                                                           |
+--------------------------------------------------------------------------*/


extern BOOL IsButtonChecked (HWND    hwnd,
                             USHORT  usControl);

extern BOOL IsSomethingSelected (HWND    hwnd,
                                 USHORT  usControl);



extern BOOL GetField (HWND     hwnd, 
                      USHORT   usControl, 
                      PSZ      pszBuffer, 
                      USHORT   usBufferSize);



extern void TranslateWildCards (PSZ psz);


extern void BuildKeyList (HWND    hwndList,
                          FILE    *pf,
                          USHORT  usBufferColumn);

