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


#include "..\include\dialog.h"
#include "..\include\itirptdg.h"



#define DID_MATERIALID           200
#define DID_MATERIALDESCR        201
#define DID_MATERIALUNIT         202
#define DID_MATERIALZONETYPE     203
#define DID_MATZONEID            204
#define DID_UNITPRICE            205


#define RPT_SESSION        "Material Listing" 
#define IDD_RNUM           RMAT_RptDlgBox
#define DID_KEY               55                    
#define DID_SELECTED         356               
#define DID_INCL_ZP          357
#define DID_INCL_FC          358

#define DID_MATERIAL           360
#define DID_UNIT               364
#define DID_ZONETYPE           368
