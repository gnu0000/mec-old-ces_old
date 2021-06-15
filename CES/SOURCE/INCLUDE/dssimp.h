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


BOOL EXPORT ItiExpGetWhereClause (PSZ     pszWhere,
                                  USHORT  usWhereLen,
                                  PSZ     pszTable,
                                  USHORT  usTableLen);

extern USHORT MapJobItemProjItem (PSZ pszJobKey);


USHORT EXPORT ResetCalcHPMIItem (PSZ pszProdDatabase);
