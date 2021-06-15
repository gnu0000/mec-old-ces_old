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


#define IDD_INITIAL           259
#define IDD_FIND              260
#define IDD_SORT              665
#define DID_FIELD_COMBO_BOX   257

#define DID_OLDJOBID          100
#define DID_JOBID             101
#define DID_JOBIDST           201
#define DID_CHECKEDBY         102
#define DID_APPROVEDBY        103
#define DID_ESTIMATOR         104
#define DID_DATECHECKED       105
#define DID_DATEAPPROVED      106
#define DID_JOBDESCR          107
#define DID_JOBDESCRST        207
#define DID_WORKTYPE          108
#define DID_WORKDAYCAL        109
#define DID_LETTINGMONTH      110
#define DID_LOCATION          111
#define DID_DISTRICT          112
#define DID_CONSTRPCT         113
#define DID_CONTINGPCT        114
#define DID_JOBALTGRID        115
#define DID_DESCRIP           116
#define DID_JOBALTID          117
#define DID_COMMENT           118

#define DID_JOBBREAKID        119
#define DID_LENGTH            120
#define DID_PAVEMENTWIDTH     121
#define DID_PAVEMENTDEPTH     122
#define DID_STRUCTID          123
#define DID_STRUCTLENGTH      124
#define DID_STRUCTWIDTH       125
#define DID_ROADTYPE          126
#define DID_STRUCTTYPE        127
#define DID_USRGRPRIV         128
#define DID_USERID            129
#define DID_USERNAME          130
#define DID_DAVISBACON        131
#define DID_MONTH             132
#define DID_YEAR              133
#define DID_WORKINGDAYS       134
#define DID_DETAILEDEST       135
#define DID_APPLYESTMETHODS   136
// define DID_UNITSYS           137

#define DID_ITEM_NUMBER       140
#define DID_ITEM_NUMBERST     240

#define DID_DESCRIPTION       141
#define DID_DESCRIPTIONST     241

#define DID_UNIT              142
#define DID_QUANTITY          143
#define DID_UNIT_PRICE        144
#define DID_SUPPLEMENTAL      145
#define DID_METHOD            146
#define DID_ALT_GROUP         147
#define DID_ALT               148
#define DID_DELAY             149
#define DID_DURATION          150
#define DID_DURATIONST        250

#define DID_PROGRAMID         168
#define DID_STARTDATE         151
#define DID_ENDDATE           152

#define DID_LETTINGDATE       153
#define DID_PRIORITY          154
#define DID_WORKBEGINDATE     155
#define DID_WORKENDDATE       156
#define DID_INFLPRED          157
#define DID_ACCESSPRIV        158

#define DID_PCTFUNDED         159 
#define DID_FUNDID            160

#define DID_DAYSBEFORESTART   161
#define DID_DAYSBEFORESTARTST 261

#define DID_ADD1              162
#define DID_CHANGE1           163
#define DID_DELETE1           164

#define DID_ADD2              165
#define DID_CHANGE2           166
#define DID_DELETE2           167
#define DID_LINK              168
#define DID_LAG               169

#define DID_COUNTYCAT         170

#define DID_BASEDATE          405

//#define DID_COSTSHEET         290
//#define DID_CBESTID           290


#define DID_JOBITEMKEY        290





#define  PurgeBasedateD_MIN     21
#define  PurgeBasedateD_01        21
#define  PurgeBasedateD_02        22
#define  PurgeBasedateD_03        23
#define  PurgeBasedateD_04        24
#define  PurgeBasedateD_05        25
#define  PurgeBasedateD_06        26
#define  PurgeBasedateD_07        27
#define  PurgeBasedateD_08        28
#define  PurgeBasedateD_09        29
#define  PurgeBasedateD_10        30
#define  PurgeBasedateD_11        31
#define  PurgeBasedateD_12        32
#define  PurgeBasedateD_13        33
#define  PurgeBasedateD_14        34
#define  PurgeBasedateD_15        35
#define  PurgeBasedateD_16        36
#define  PurgeBasedateD_17        37
#define  PurgeBasedateD_18        38
#define  PurgeBasedateD_MAX     38

#define  PurgeBasedateD_19        39
#define  PurgeBasedateD_20        40
#define  PurgeBasedateD_21        41
 

// #define  UNREF_BD   " WHERE BaseDate > '1/1/1900' AND BaseDate < (select MAX(BaseDate) from BaseDate) AND BaseDate NOT IN (select DISTINCT BaseDate from Job) "

#define UNREF_BD " BD,LongText LT where BD.Comment=LT.LongTextKey and LT.ShortText NOT like '%nreferen%' and BD.BaseDate>'1/1/1900' and BD.BaseDate<(select MAX(BaseDate) from BaseDate) and BD.BaseDate NOT IN (select DISTINCT BaseDate from Job)"

