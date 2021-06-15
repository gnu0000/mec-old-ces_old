#+--------------------------------------------------------------------------+
#|                                                                          |
#|       Copyright (c) 1992 by AASHTO.  All Rights Reserved.                |
#|                                                                          |
#|       This program module is part of BAMS/CES, a module of BAMS,         |
#|       The American Association of State Highway and Transportation       |
#|       Officials' (AASHTO) Information System for Managing Transportation |
#|       Programs, a proprietary product of AASHTO, no part of which may be |
#|       reproduced or transmitted in any form or by any means, electronic, |
#|       mechanical, or otherwise, including photocopying and recording     |
#|       or in connection with any information storage or retrieval         |
#|       system, without permission in writing from AASHTO.                 |
#|                                                                          |
#+--------------------------------------------------------------------------+


RPT_0=rPrjt
RPT_1=rProp


!include "..\makeopts"

all:
   premake /AddDot $(RPT_0).pre $(RPT_0)       
   $(MAKE) -nologo -f $(RPT_0)          
   premake /AddDot $(RPT_1).pre $(RPT_1)       
   $(MAKE) -nologo -f $(RPT_1)          
   
