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



extern HMODULE hmod;
extern PGLOBALS pglobals;
extern PSZ pszImportDatabase;
   
#ifndef INIT_INCL

#define INIT_INCL

#define   NUMBER_OF_KEYS     400
#define   KEY_LEN             8
#define   SZLEN              16


#define NUMBER_OF_MIKEYS    30
#define MIKEY      0
#define QTY        1
#define EXTAMT     2
#define MI_ID      3
#define MI_SPECYR  4

#define HPKEY      0
#define HP_ID      1
#define HP_SPECYR  2

#endif  /* INIT_INCL */


extern USHORT EXPORT ResetCalcHPMIItem (PSZ pszProdDatabase);
