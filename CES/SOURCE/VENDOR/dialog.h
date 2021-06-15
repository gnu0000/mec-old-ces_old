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


#include "..\include\itirptdg.h"

#define DID_VENDORID            200
#define DID_SHORTNAME           201
#define DID_NAME                202
#define DID_TYPE                203
#define DID_CERTIFICATION       204
#define DID_DBECLASSIF          205
#define DID_VENADDRESS          206
#define DID_CITY                207
#define DID_STATE               208
#define DID_ZIPCODE             209
#define DID_PHONENUM            210
#define DID_ADDRSEQNUM          211
#define DID_LOCATION            212

#define DID_ADD1                213
#define DID_CHANGE1             214
#define DID_DELETE1             215

#define DID_ADD2                216
#define DID_CHANGE2             217
#define DID_DELETE2             218


#define RPT_VEND_SESSION    "Vendor Listing"
#define IDD_RVEND           rVend_RptDlgBox
#define DID_KEY               60                    
#define DID_SELECTED         361               
#define DID_INCL_FACIL       362
