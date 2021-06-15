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


/*
 * DSSImp.h
 * Mark Hampton
 *
 */


/* defines for import queries */
#define COPYBIDDER                   1
#define COPYFACILITYMATERIAL         2
#define COPYITEMBID                  3
#define COPYPROJECT                  4
#define COPYPROJECTCATEGORY          5
#define COPYPROJECTITEM              6
#define COPYPROPOSAL                 7
#define COPYPROPOSALITEM             8
#define COPYPROPOSALMAJORITEM        9
#define COPYVENDORFACILITY          10

#if !defined(INDV_SELECT)
#define INDV_SELECT                200
#define INDV_INSERT                300
#endif


#define PROPSTDITEM                 11
#define PROPSTDITEMLIST             12
#define PROJSTDITEM                 13
#define PROJPROPSTDITEM             14
#define BIDDERITEMKEY               15
#define PROPOSALITEMKEY             16
#define PROPOSALITEMKEYLIST         17

#define PROPMAKESTDITEMLIST         19
#define DELTEMPLISTTABLE            20

#define MAKEPROJSTDITEMLIST         22
#define PROJSTDITEMLIST             23
#define DELBADKEYTEXT               24

#define CALCPROPOSALAWA             25
#define CALCPROPOSALEST             26
#define CALCPROPOSALLOW             27
#define CALCPROJECTAWA              28
#define CALCPROJECTEST              29
#define CALCPROJECTLOW              30
#define CALCBIDDER                  31
#define CALCPROPOSALAWAZERO         32
#define CALCPROPOSALESTZERO         33
#define CALCPROPOSALLOWZERO         34
#define CALCPROJECTAWAZERO          35
#define CALCPROJECTESTZERO          36
#define CALCPROJECTLOWZERO          37
#define CALCBIDDERZERO              38
#define CALCLOWBIDDER               39

#define COPYVENDOR                  40

#define MAKEHPMI                    41
#define CALCHPMIEXTENSION           42
#define CALCHPMIQUANTITY            43
#define CALCHPMIUNITPRICE           44
#define COPYHPMI                    45
#define UPDATEVENDOR                46
#define SETKMVENDOR                 47

/* code table queries */
#define COPYCODETABLE               50
#define COPYCODEVALUE               51

/* county queries */
#define COPYCOUNTY                  52
#define COPYCOUNTYMAP               53

#define COPYFUND                    54

#define COPYMAJORITEM               55
#define COPYMAP                     56
#define COPYMAPVERTEX               57
#define COPYMATERIAL                58
#define COPYSIDEPENDENCY            59
#define COPYSIMAJORITEM             60
#define STANDARDITEMKEY             61
#define COPYVENDORADDRESS           62
#define COPYVENDORCLASS             63
#define COPYZONE                    64
#define COPYZONEMAP                 65
#define COPYZONETYPE                66

#define COPYPARAMETRICPROFILE       67
#define COPYSIGMAJORITEM            68
#define COPYMIPA                    69
#define COPYMIPR                    70
#define COPYMIPV                    71
#define COPYMIQA                    72
#define COPYMIQR                    73
#define COPYMIQV                    74

/* pes import defines */
#define PROJITEMKEY                 75
#define CALCPROJITEM                76
#define COPYPROJITEM                77
#define COPYCATEGORY                78
#define COPYPESPROJECT              79
#define LISTBADPESPROJECTS          80
#define DELETEOLDPESPROJ            81
#define GETPESPROJECTKEYS           82
#define GETPESCATEGORYKEYS          83
#define LISTBADJOBITEMS             84
#define DELETEJOBITEMS              85
#define GETPESJOBKEYS               86
#define LISTNEWJOBITEMS             87
#define INSERTJOBITEMS              88
#define LISTBADJOBPROJITEMS         89
#define DELETEJOBPROJITEMS          90
#define GETJOBKEYS                  91
#define INSERTJOBPROJITEMS          92
#define UPDATEJOBQUAN               93

#define MISTANDARDITEMKEY           94

/* defines for export queries and export header lines */
#define EXP_JOB                     1
#define EXP_JOBUSER                 2
#define EXP_JOBALTGROUP             3
#define EXP_JOBALT                  4
#define EXP_JOBCOMMENT              5
#define EXP_JOBBREAK                6
#define EXP_JOBBREAKFUND            7
#define EXP_JOBITEM                 8
#define EXP_JOBITEMDEP              9
#define EXP_WORKINGDAYCAL           10
#define EXP_WORKINGDAYPRE           11
#define EXP_JOBBREAKPROF            12
#define EXP_JOBBREAKMAJORITEM       13
#define EXP_JOBCBE                  14
#define EXP_JOBCOSTSHEET            15
#define EXP_JOBCREWEQUIPMENTUSED    16
#define EXP_JOBCREWLABORUSED        17
#define EXP_JOBCREWUSED             18
#define EXP_JOBMATERIALUSED         19

/* catalogs */
#define EXP_AREATYPE                100
#define EXP_AREA                    101
#define EXP_AREACOUNTY              102
#define EXP_COUNTY                  103
#define EXP_COUNTYMAP               104
#define EXP_MAP                     105
#define EXP_MAPVERTEX               106
#define EXP_ZONETYPE                107
#define EXP_ZONE                    108
#define EXP_ZONEMAP                 109
#define EXP_CODETABLE               110
#define EXP_CODEVALUE               111
#define EXP_FUND                    112
#define EXP_DEFAULT                 113
#define EXP_USER                    114
#define EXP_USERCATALOG             115
#define EXP_USERAPPLICATION         116
#define EXP_DEFAULTJOBUSER          117
#define EXP_DEFAULTPROGRAMUSER      118
#define EXP_CBE                     120
#define EXP_TASK                    121
#define EXP_CBECOSTSHEET            122
#define EXP_CBECREWUSED             123
#define EXP_CBECREWEQUIPMENTUSED    124
#define EXP_CBECREWLABORUSED        125
#define EXP_CBEMATERIALUSED         126
#define EXP_CREW                    127
#define EXP_CREWEQUIPMENT           128
#define EXP_CREWLABOR               129
#define EXP_EQUIPMENT               130
#define EXP_EQUIPMENTRATE           131
#define EXP_LABOR                   132
#define EXP_LABORERWAGE             133
#define EXP_MATERIAL                134
#define EXP_MATERIALPRICE           135
#define EXP_COSTSHEET               136
#define EXP_CREWUSED                137
#define EXP_CREWEQUIPMENTUSED       138
#define EXP_CREWLABORUSED           139
#define EXP_MATERIALUSED            140
#define EXP_MAJORITEM               141
#define EXP_MAJORITEMSTDITEM        142
#define EXP_MAJORITEMPRICEREG       143
#define EXP_MAJORITEMPRICEVAR       144
#define EXP_MAJORITEMPRICEADJ       145
#define EXP_MAJORITEMQTYREG         146
#define EXP_MAJORITEMQTYVAR         147
#define EXP_MAJORITEMQTYADJ         148
#define EXP_PARAMETRICPROFILE       149
#define EXP_SIGMAJORITEM            150
#define EXP_PEMETHAVG               151
#define EXP_WAUP                    152
#define EXP_PEMETHREG               153
#define EXP_SEASONADJ               154
#define EXP_WORKTYPEADJ             155
#define EXP_AREAADJ                 156
#define EXP_PROGRAM                 157
#define EXP_PROGRAMFUND             158
#define EXP_PROGRAMJOB              159
#define EXP_PROGRAMJOBDEPEND        160
#define EXP_INFLATIONPRED           161
#define EXP_INFLATIONCHANGE         162
#define EXP_STANDARDITEM            163
#define EXP_STANDARDITEMDEPEND      164
#define EXP_VENDOR                  165
#define EXP_VENDORADDRESS           166
#define EXP_VENDORCLASS             167
#define EXP_PROGRAMUSER             168
#define EXP_MAJORITEMSTDITEM_NULSY  169
#define EXP_MAJORITEM_NULSY         170


/*
 * This function parses pszStdItemNumber, and returns a pointer to the
 * spec year string.  It also null terminates the item number.
 * pszStdItemNumber should be in the form: "item_number spec_year".  This
 * function assumes that item numbers may not have spaces in them.
 */ 

extern PSZ GetSpecYear (PSZ pszStdItemNumber);


/*
 * ItiDllQueryCompatibility returns TRUE if this DLL is compatable
 * with the caller's version numer, otherwise FALSE is returned.
 */
extern BOOL EXPORT ItiDllQueryCompatibility (PSZ pszCallerVersion);
