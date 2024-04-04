/******************************************************************************
* File Name    : r_dlms_data_ic.c
* Version      : 1.00
* Device(s)    : None
* Tool-Chain   :
* H/W Platform : Unified Energy Meter Platform
* Description  :
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 21.02.2013
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_dlms_typedef.h"				/* DLMS Type Definitions */
#include "r_dlms_obis_ic.h"				/* DLMS OBIS IC Definitions */
#include "r_dlms_obis.h"				/* DLMS OBIS Definitions */
#include "r_dlms_data.h"				/* DLMS DATA Definitions */
#include "r_dlms_data_ic.h"				/* DLMS DATA IC Definitions */

#include "emeter3_structs.h"
////#include "r_cg_userdefine.h"


/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
#if (defined(USED_CLASS_07) && USED_CLASS_07 == 1)
extern const scaler_unit_t		g_Class07_InstScalerBuffer[];

extern class07_blockload_entry_t	g_Class07_BlockLoadBuffer;
extern const scaler_unit_t		g_Class07_BlockloadScalerBuffer[];

extern class07_dailyload_entry_t	g_Class07_DailyLoadBuffer;
extern const scaler_unit_t		g_Class07_DailyloadScalerBuffer[];

extern class07_billing_entry_t		g_Class07_BillingBuffer;
extern const scaler_unit_t		g_Class07_BillingScalerBuffer[];

extern class07_event_entry_t		g_Class07_EventBuffer;
extern const scaler_unit_t		g_Class07_Event0ScalerBuffer[];
#endif






/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/

/****************************************************************************************
 * Access rights definition table
 * One row is defined, can be used multiple time
 *
 * Attribute: 32 bits, each 2 bits encode the access right for 1 attr
 * The LSB indicate access right for attr 1.
 * The MSB indicate access right for attr 16.
 *
 * Method: 16 bits, each 1 bit encode the access right for 1 methods
 * The LSB indicate access right for method 1.
 * The MSB indicate access right for method 16.
 ****************************************************************************************/
const access_right_t g_AccessRightTable[][OBIS_NUMBER_ASSOCIATION] =
{
 /*--------------------------------------------------------------------------*/
 /* RowID  |   Access Right Definition                                       */
 /*--------------------------------------------------------------------------*/
 /*         Assc0                 Assc1                 Assc2               */
 /*         No access           | Read-only           | Read-only           */
 /* 00 */ { {0x00000000, 0x0000}, {0x55555555, 0x0000}, {0x55555555, 0x0000}  },
  /*        Read-only           | Read-only           | Read-only          */
 /* 01 */ { {0x55555555, 0x0000}, {0x55555555, 0x0000}, {0x55555555, 0x0000}  },
 /*         No access           | Read-only           | Read-write          */
 /* 02 */ { {0x00000000, 0x0000}, {0x55555555, 0x0000}, {0x5555557D, 0xFFFF}  },
 /*         Read-only           | Read-only          | Read-write          */
 /* 03 */// { {0x55555555, 0x0000}, {0x55555555, 0x0000}, {0xFFFFFFFD, 0xFFFF} },
 /* 03 */ { {0x55555555, 0x0000}, {0x55555555, 0x0000}, {0x5555557D, 0xFFFF} },
 /* For Association LN */
 /* For AA 0 */
 /* 04 */ //{ {0x55540155, 0x0000}, {0x55555555, 0xFFFF}, {0x55557555, 0xFFFF}  },
 /* 04 */ { {0x55554555, 0x0000}, {0x55554555, 0x0000}, {0x55554555, 0x0000}  },
 /* For AA 1 */
/* 05 */ { {0x00000000, 0x0000}, {0x55554555, 0x0000}, {0x55556555, 0x0000}  },
/* For AA 2 */
/* 06 */ { {0x00000000, 0x0000}, {0x00000000, 0x0000}, {0x55554555, 0x0003}  },
  /* For Activity calendar */
  /*         Read-only           | Read-only          | Read-write          */
 /* 07 */ { {0x00000000, 0x0000}, {0x55555555, 0x0000}, {0xfffffd55, 0xFFFF}  },
 /* For Single action schedule */
  /*         Read-only           | Read-only          | Read-write          */
 /* 08 */ { {0x00000000, 0x0000}, {0x55555555, 0x0000}, {0x555555D5, 0xFFFF}  },
 /* Class_09 */
 /* 09  { {0x00000000, 0x0000}, {0x00000000, 0x0000}, {0x00000000, 0x0000}  },*/
 /* 09 */ { {0x00000000, 0x0000}, {0x55555555, 0x0000}, {0xffffffff, 0xFFFF}  }, 
 /* 10 */ //{ {0x00000000, 0x0000}, {0x55555555, 0x0000}, {0x55555555, 0xFFFF}  },
};

/*
                  16 15 14 13  12 11 10 9    8  7  6  5   4  3  2  1
0x55 55 55 D5  -  01 01 01 01  01 01 01 01  01 01 01 01  11 01 01 01

                  16 15  14 13     12 11  10 9      8  7   6  5      4  3   2  1
0xff ff fd 55  -  11 11  11 11     11 11  11 11     11 11  01 01     01 01  01 01
attributes 1,2,3,4,5,6 are read only - attributes 6 onwards are read write
*/

/****************************************************************************************
 * Class ID = 1, Data Child-table
 *
 * How to define a row of this table.
 *
 * Field 1. Fill the OBIS code first, 6 numbers (A, B, C, D, E, F)
 * Field 2. Specify which row on g_AccessRightTable will be used to declare the
 *          access right of the object.
 * Field 3. Select the value (CHOICE, buffer) that used for Attr 2 - value.
 *          If NULL is specified in buffer parameter, the buffer attr 2 will be get/set
 *          on R_OBIS_GetObjectAttr and R_OBIS_SetObjectAttr
 ****************************************************************************************/
#if (defined(USED_CLASS_01) && USED_CLASS_01 == 1)
/* Static parameters */
Unsigned32		g_Class01_Blockload_interval = CLASS07_BLOCKLOAD_INTERVAL;
const Unsigned32	g_Class01_Dailyload_interval = CLASS07_DAILYLOAD_INTERVAL;
Unsigned16		g_Class01_EventCode[7] = {0,0,0,0,0,0,0};
/* Variant parameters */


const class01_child_record_t g_ChildTableClass01[] =
{
	/*--------------------------------------------------------------------------------------------------------------------------------------*/
	/*          OBIS Code (A, B, C, D, E, F)    | Access Right         | DYN_VALUE(CHOICE, buffer, access)                                  */
	/*--------------------------------------------------------------------------------------------------------------------------------------*/
	/* Logical Device Name */
	/* 00 */ //{	{   0,   0,  42,   0,   0, 255 }, g_AccessRightTable[4], DYN_VALUE(CHOICE_OCTET_STRING(obis_strlen(g_Class01_LDN_name))	, g_Class01_LDN_name					, ATTR_ACCESS_MEMORY	)	},
	/* 00 */ {	{   0,   0,  42,   0,   0, 255 }, (access_right_t *)g_AccessRightTable[1], DYN_VALUE(CHOICE_OCTET_STRING(-1), NULL, ATTR_ACCESS_USERFUNC	)	}, 
	/* Meter serial number */
	/* 01 */ {	{   0,   0,  96,   1,   0, 255 }, (access_right_t *)g_AccessRightTable[1], DYN_VALUE(CHOICE_OCTET_STRING(-1), NULL, ATTR_ACCESS_USERFUNC	)	},
	/* Manufacture Name */
	/* 02 */ {	{   0,   0,  96,   1,   1, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_OCTET_STRING(-1), NULL, ATTR_ACCESS_USERFUNC	)	},
	/* Firmware version for meter */
	/* 03 */ {	{   1,   0,   0,   2,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_OCTET_STRING(-1), NULL, ATTR_ACCESS_USERFUNC	)	},
	/* Meter Type */
	/* 04 */ {	{   0,   0,  94,  91,   9, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_LONG_UNSIGNED	,   NULL, ATTR_ACCESS_USERFUNC	)	},
	/* Category */
	/* 05 */ {	{   0,   0,  94,   91, 11, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_OCTET_STRING(-1), NULL, ATTR_ACCESS_USERFUNC	)	},
	/* Current rating  */
	/* 06 */ {	{   0,   0,  94,   91, 12, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_OCTET_STRING(-1), NULL, ATTR_ACCESS_USERFUNC	)	},
	/* Meter Year of Manufacture */
	/* 07 */ {	{   0,   0,  96,   1,   4, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_LONG_UNSIGNED	, NULL	, ATTR_ACCESS_USERFUNC	)	},
	/* Programmable Parameters */
	/* Demand integration period */
	/* 08 */ {	{   1,   0,   0,   8,   0, 255 }, (access_right_t *)g_AccessRightTable[2], DYN_VALUE(CHOICE_LONG_UNSIGNED	, NULL	, ATTR_ACCESS_USERFUNC	)	},
	/* Block load capture period */
	/* 09 */ {	{   1,   0,   0,   8,   4, 255 }, (access_right_t *)g_AccessRightTable[2], DYN_VALUE(CHOICE_LONG_UNSIGNED	, NULL	, ATTR_ACCESS_USERFUNC	)	},
	/* Daily load capture period */
	/* 10 */ {	{   1,   0,   0,   8,   5, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG_UNSIGNED, (Unsigned32 *)&g_Class01_Dailyload_interval , ATTR_ACCESS_MEMORY	)	},
	
	/* Instantaneous Parameter */
	/* Cumulative tamper count */
	/* 11 */ {	{   0,   0,  94,   91,  0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_LONG_UNSIGNED	, NULL	, ATTR_ACCESS_USERFUNC	)	},
	/* Cumulative billing count */
	/* 12 */ {	{   0,   0,   0,    1,  0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG_UNSIGNED	, NULL	, ATTR_ACCESS_USERFUNC	)	},
	/* Cumulative programming count */
	/* 13 */ {	{   0,   0,  96,    2,  0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_LONG_UNSIGNED	, NULL	, ATTR_ACCESS_USERFUNC	)	},

	/* Event code object -Amendment3*/
	/* 14 */ {	{   0,   0,  96,   11,  0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_LONG_UNSIGNED , NULL	, ATTR_ACCESS_USERFUNC	)	},
	/* 15 */ {	{   0,   0,  96,   11,  1, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_LONG_UNSIGNED , NULL	, ATTR_ACCESS_USERFUNC	)	},
	/* 16 */ {	{   0,   0,  96,   11,  2, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_LONG_UNSIGNED	, NULL	, ATTR_ACCESS_USERFUNC	)	},
	/* 17 */ {	{   0,   0,  96,   11,  3, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_LONG_UNSIGNED	, NULL	, ATTR_ACCESS_USERFUNC	)	},
	/* 18 */ {	{   0,   0,  96,   11,  4, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_LONG_UNSIGNED	, NULL	, ATTR_ACCESS_USERFUNC	)	},
	/* 19 */ {	{   0,   0,  96,   11,  5, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_LONG_UNSIGNED	, NULL	, ATTR_ACCESS_USERFUNC	)	},

	/* Available Billing Period */
	/* 20 */ {	{   0,   0,  0,   1,  1, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_UNSIGNED   , NULL  , ATTR_ACCESS_USERFUNC	)	},
	/* No. Of Power Failure*/
	/* 21 */ {	{   0,   0,  96,   7,  0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG_UNSIGNED   , NULL  , ATTR_ACCESS_USERFUNC	)	},
	/*Internal CT ratio*/
	/* 22 */ {	{   1,   0,  0,   4,  2, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_LONG_UNSIGNED   , NULL  , ATTR_ACCESS_USERFUNC	)	},
	/*Internal VT ratio*/
	/* 23 */ {	{   1,   0,  0,   4,  3, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_LONG_UNSIGNED   , NULL  , ATTR_ACCESS_USERFUNC	)	},
	/*Phase Sequence*/
	/* 24 */ {	{   0,   0, 96,  50, 46, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_LONG_UNSIGNED   , NULL  , ATTR_ACCESS_USERFUNC	)	},
	/*Rising Demand kVa*/
	/* 25 */ {	{   0,   0, 96,  50, 42, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_LONG_UNSIGNED   , NULL  , ATTR_ACCESS_USERFUNC	)	},
	/*Rising Demand Elapsed Time*/
	/* 26 */ {	{   0,   0, 96,  50, 43, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_LONG_UNSIGNED   , NULL  , ATTR_ACCESS_USERFUNC	)	},
	/*SelfDiagnostic*/
	/* 27 */ {	{   1,   0, 96,  5, 0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_LONG_UNSIGNED,NULL,ATTR_ACCESS_USERFUNC	)	},	
};
const Unsigned16 g_ChildTableLengthClass01 = sizeof(g_ChildTableClass01) / sizeof(class01_child_record_t);
#endif

/****************************************************************************************
 * Class ID = 3, Register Child-table
 *
 * How to define a row of this table.
 *
 * Field 1. Fill the OBIS code first, 6 numbers (A, B, C, D, E, F)
 * Field 2. Specify which row on g_AccessRightTable will be used to declare the
 *          access right of the object.
 * Field 3. Select the value (CHOICE, buffer) that used for Attr 2 - value.
 *          If NULL is specified in buffer parameter, the buffer attr 2 will be get/set
 *          on R_OBIS_GetObjectAttr and R_OBIS_SetObjectAttr
 * Field 4. Select the scaler and unit for object
 ****************************************************************************************/
#if (defined(USED_CLASS_03) && USED_CLASS_03 == 1)

/* Variant parameters */

/* Block load profile parameter */

/* Billing profile parameter */
date_time_t		g_Class03_BillingDate;

const class03_child_record_t g_ChildTableClass03[] =
{
	/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*          OBIS Code (A, B, C, D, E, F)    | Access Right         | DYN_VALUE(CHOICE, buffer, access)                                                              | Scaler, unit                      */
	/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
	/* Instantaneous Parameters */

	/* 00 */ {	{   1,   0,  31,   7,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_AMPERE, PHY_UNIT_AMPERE		}	},	/* IR current  */
	/* 01 */ {	{   1,   0,  51,   7,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_AMPERE, PHY_UNIT_AMPERE		}	},	/* IY current */
	/* 02 */ {	{   1,   0,  71,   7,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_AMPERE, PHY_UNIT_AMPERE		}	},	/* IB current */
	/* 03 */ {	{   1,   0,  32,   7,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_VOLTAGE, PHY_UNIT_VOLT		}	},	/* VR Voltage  */
	/* 04 */ {	{   1,   0,  52,   7,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_VOLTAGE, PHY_UNIT_VOLT		}	},	/* VY Voltage  */
	/* 05 */ {	{   1,   0,  72,   7,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_VOLTAGE, PHY_UNIT_VOLT		}	},	/* VB Voltage  */
	/* 06 */ {	{   1,   0,  33,   7,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_PF, PHY_UNIT_NONE			}	},	/* R Signed Power Factor */
	/* 07 */ {	{   1,   0,  53,   7,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_PF, PHY_UNIT_NONE			}	},	/* Y Signed Power Factor */
	/* 08 */ {	{   1,   0,  73,   7,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_PF, PHY_UNIT_NONE			}	},	/* B Signed Power Factor */
	/* 09 */ {	{   1,   0,  13,   7,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_PF, PHY_UNIT_NONE			}	},	/* B Signed Power Factor */
	/* 10 */ {	{   1,   0,  14,   7,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_HERTZ, PHY_UNIT_HERTZ			}	},	/* Frequency */
	/* 11 */ {	{   1,   0,   9,   7,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_VA, PHY_UNIT_VOLT_AMPERE		}	},	/* Apparent Power */
	/* 12 */ {	{   1,   0,   1,   7,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_WATT, PHY_UNIT_WATT			}	},	/* Active Power   */
	/* 13 */ {	{   1,   0,   3,   7,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_WATT, PHY_UNIT_VAR			}	},	/* Reactive Power   */
	/* 14 */ {	{   1,   0,   1,   8,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_WH, PHY_UNIT_WATT_HOUR		}	},	/* Cumulative Energy - kWh */
	/* 15 */ {	{   1,   0,   5,   8,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_WH, PHY_UNIT_VAR_HOUR			}	},	/* Cumulative energy, kvarh (Lag) */
	/* 16 */ {	{   1,   0,   8,   8,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_VAH, PHY_UNIT_VAR_HOUR		}	},	/* Cumulative energy, kvarh (Lead)*/
	/* 17 */ {	{   1,   0,   9,   8,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_VAH, PHY_UNIT_VOLT_AMPERE_HOUR	}	},	/* Cumulative Energy - kVAh*/
	/* 18 */ {	{   0,   0,  94,  91,   8, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_NULL,PHY_UNIT_MIN			}	},	/* Cumulative power-failure duration */
	/* 19 */ {	{   0,   0,   0,   1,   2, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_DATE_TIME(&g_Class03_BillingDate, ATTR_ACCESS_MEMORY)	, {SCALAR_NULL, PHY_UNIT_NONE			}	},	/* Billing date */
	/* 20 */ {	{   1,   0,   1,   29,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_BL_WH, PHY_UNIT_WATT_HOUR		}	},	/* Block Energy KWH - load survey */
	/* 21 */ {	{   1,   0,   5,   29,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_BL_WH, PHY_UNIT_VAR_HOUR	}	},	/* Block energy kVARH(lag) - load survey   */
	/* 22 */ {	{   1,   0,   8,   29,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_BL_WH, PHY_UNIT_VAR_HOUR		}	},	/* Block Energy KVARH(lead) - load survey */
	/* 23 */ {	{   1,   0,   9,   29,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_BL_VAH, PHY_UNIT_VOLT_AMPERE_HOUR	}	},	/* Block energy kVAH - load survey   */
	/* 24 */ {	{   0,   0,   94,  91,   13, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_NULL,PHY_UNIT_MIN	}	},	/* Total power on duration - month   */
	/* 25 */ {	{   0,   0,   94,  91,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_NULL,PHY_UNIT_NONE	}	},	/* Total tamper Count Cumulative   */
	#ifdef BIDIR_METER
	/* 26 */ {	{   1,   0,  31,   27,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_AMPERE, PHY_UNIT_AMPERE		}	},	/* IR current - average  */
	/* 27 */ {	{   1,   0,  51,   27,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_AMPERE, PHY_UNIT_AMPERE		}	},	/* IY current - average */
	/* 28 */ {	{   1,   0,  71,   27,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_AMPERE, PHY_UNIT_AMPERE		}	},	/* IB current - average */
	/* 29 */ {	{   1,   0,  32,   27,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_VOLTAGE, PHY_UNIT_VOLT		}	},	/* VR Voltage - average */
	/* 30 */ {	{   1,   0,  52,   27,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_VOLTAGE, PHY_UNIT_VOLT		}	},	/* VY Voltage - average */
	/* 31 */ {	{   1,   0,  72,   27,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_VOLTAGE, PHY_UNIT_VOLT		}	},	/* VB Voltage - average */
	/* 32 */ {	{   1,   0,   2,   8,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_WH, PHY_UNIT_WATT_HOUR		}	},	/* Cumulative Energy - kWh export */
	/* 33 */ {	{   1,   0,   7,   8,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_WH, PHY_UNIT_VAR_HOUR			}	},	/* Cumulative energy, kvarh (Lag) export */
	/* 34 */ {	{   1,   0,   6,   8,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_VAH, PHY_UNIT_VAR_HOUR		}	},	/* Cumulative energy, kvarh (Lead) export*/
	/* 35 */ {	{   1,   0,  10,   8,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_VAH, PHY_UNIT_VOLT_AMPERE_HOUR	}	},	/* Cumulative Energy - kVAh export */
	#endif
	/* 36 */ {	{   0,   0,  96,  50,  11, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_WH_HIRES, PHY_UNIT_WATT_HOUR		}	},	/* High resolution - kWh */
	/* 37 */ {	{   0,   0,  96,  50,  12, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_WH_HIRES, PHY_UNIT_VOLT_AMPERE_HOUR		}	},	/* High resolution - kVAh */
	/* 38 */ {	{   0,   0,  96,  50,  13, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_VAH_HIRES, PHY_UNIT_VAR_HOUR		}	},	/* High resolution - kVArh- lag*/
	/* 39 */ {	{   0,   0,  96,  50,  14, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_VAH_HIRES, PHY_UNIT_VAR_HOUR	}	},	/* High resolution - kVArh - lead */	
	#ifdef SPLIT_ENERGIES_REQUIRED
	/* 40 */ {	{   0,   0,  96,  50,  15, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_WH, PHY_UNIT_WATT_HOUR		}	},	/* Cumulative Energy - kWh lag */
	/* 41 */ {	{   0,   0,  96,  50,  16, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_WH, PHY_UNIT_WATT_HOUR			}	},	/* Cumulative energy, kwh lead */
	/* 42 */ {	{   0,   0,  96,  50,  17, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_VAH, PHY_UNIT_VOLT_AMPERE_HOUR	}	},	/* Cumulative energy, kvah Lag */
	/* 43 */ {	{   0,   0,  96,  50,  18, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG, NULL, ATTR_ACCESS_USERFUNC)	, {SCALAR_VAH, PHY_UNIT_VOLT_AMPERE_HOUR	}	},	/* Cumulative Energy - kVAh lead */
	#endif
	};
const Unsigned16 g_ChildTableLengthClass03 = sizeof(g_ChildTableClass03) / sizeof(class03_child_record_t);
#endif
/****************************************************************************************
 * Class ID = 4, Extended Register Child-table
 *
 * How to define a row of this table.
 *
 * Field 1. Fill the OBIS code first, 6 numbers (A, B, C, D, E, F)
 * Field 2. Specify which row on g_AccessRightTable will be used to declare the
 *          access right of the object.
 * Field 3. Select the value (CHOICE, buffer) that used for Attr 2 - value.
 *          If NULL is specified in buffer parameter, the buffer attr 2 will be get/set
 *          on R_OBIS_GetObjectAttr and R_OBIS_SetObjectAttr
 * Field 4. Select the scaler and unit for object
 * Field 5. Select the status (CHOICE, buffer) that used for Attr 4 - value.
 *          If NULL is specified in buffer parameter, the buffer attr 2 will be get/set
 *          on R_OBIS_GetObjectAttr and R_OBIS_SetObjectAttr
 *
****************************************************************************************/
#if (defined(USED_CLASS_04) && USED_CLASS_04 == 1)

/* Static parameters */

/* Variant parameters */
const Unsigned8		g_Class04_ActiveMDStatus[]   = "OK";
const Unsigned8		g_Class04_ApparentMDStatus[] = "OK";
date_time_t			g_Class04_mdCaptureTime;


const class04_child_record_t g_ChildTableClass04[] =
{
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*          OBIS Code (A, B, C, D, E, F)    | Access Right         | DYN_VALUE(CHOICE, buffer, access)                                                        | Scaler, unit                    | DYN_STATUS(CHOICE, buffer, access)                                                                                                 | DYN_DATE_TIME(buffer, access)                                   */
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
	/* Maximum Demand  kW */
	/* 00 */ {	{   1,   0,   1,   6,   0, 255 },(access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG_UNSIGNED , NULL, ATTR_ACCESS_USERFUNC),					{SCALAR_MD, PHY_UNIT_WATT		}		, DYN_STATUS(CHOICE_OCTET_STRING(obis_strlen(g_Class04_ActiveMDStatus))	, (Unsigned8 *)g_Class04_ActiveMDStatus		, ATTR_ACCESS_MEMORY)	, DYN_DATE_TIME(&g_Class04_mdCaptureTime		, ATTR_ACCESS_MEMORY)	},
	/* Maximum Demand  kVA */		
	/* 01 */ {	{   1,   0,   9,   6,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG_UNSIGNED , NULL, ATTR_ACCESS_USERFUNC),					{SCALAR_MD, PHY_UNIT_VOLT_AMPERE	}	, DYN_STATUS(CHOICE_OCTET_STRING(obis_strlen(g_Class04_ApparentMDStatus))	, (Unsigned8 *)g_Class04_ApparentMDStatus	, ATTR_ACCESS_MEMORY)	, DYN_DATE_TIME(&g_Class04_mdCaptureTime	, ATTR_ACCESS_MEMORY)	},
#ifdef BIDIR_METER
	/* Maximum Demand  kW export*/
	/* 02 */ {	{   1,   0,   2,   6,   0, 255 },(access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG_UNSIGNED , NULL, ATTR_ACCESS_USERFUNC),					{SCALAR_MD, PHY_UNIT_WATT		}		, DYN_STATUS(CHOICE_OCTET_STRING(obis_strlen(g_Class04_ActiveMDStatus))	, (Unsigned8 *)g_Class04_ActiveMDStatus		, ATTR_ACCESS_MEMORY)	, DYN_DATE_TIME(&g_Class04_mdCaptureTime		, ATTR_ACCESS_MEMORY)	},
	/* Maximum Demand  kVA export */		
	/* 03 */ {	{   1,   0,  10,   6,   0, 255 }, (access_right_t *)g_AccessRightTable[0], DYN_VALUE(CHOICE_DOUBLE_LONG_UNSIGNED , NULL, ATTR_ACCESS_USERFUNC),					{SCALAR_MD, PHY_UNIT_VOLT_AMPERE	}	, DYN_STATUS(CHOICE_OCTET_STRING(obis_strlen(g_Class04_ApparentMDStatus))	, (Unsigned8 *)g_Class04_ApparentMDStatus	, ATTR_ACCESS_MEMORY)	, DYN_DATE_TIME(&g_Class04_mdCaptureTime	, ATTR_ACCESS_MEMORY)	},
#endif
};
const Unsigned16 g_ChildTableLengthClass04 = sizeof(g_ChildTableClass04) / sizeof(class04_child_record_t);
#endif

/****************************************************************************************
 * Class ID = 5, Demand Register Child-table
 *
 * How to define a row of this table.
 *
 * Field 1. Fill the OBIS code first, 6 numbers (A, B, C, D, E, F)
 * Field 2. Specify which row on g_AccessRightTable will be used to declare the
 *          access right of the object.
 * Field 3.Current average value
 * Field 4.Last average value
 * Field 5.Scaler, unit
 * Field 6.Status
 * Field 7.Capture time buffer
 * Field 8.Start time current buffer
 * Field 9.Period
 * Field 10.Number of periods
 ****************************************************************************************/
#if (defined(USED_CLASS_05) && USED_CLASS_05 == 1)

/* Static parameters */
const Unsigned8	g_Class05_Current_avg3_Status[]   = "OK";

/* Variant parameters */
Float32		g_Class05_Current_avg3 = 562.5f;
Float32		g_Class05_last_Current_avg3 = 562.5f;
date_time_t	g_Class05_Current_avg3_StartTime;
date_time_t	g_Class05_Current_avg3_Time;
Unsigned32	g_Class05_period = 1;
Unsigned16	g_Class05_nr_periods = 2;

const class05_child_record_t g_ChildTableClass05[] =
{
	/* Current avg. 3 */
	{
		{   1,   0,   1,   24,   0, 255 },															/* Field 1.Logical name (OBIS code) of the object. */
		g_AccessRightTable[0],																		/* Field 2.Access right definition for 1 object    */
		DYN_VALUE(CHOICE_FLOAT32, &g_Class05_Current_avg3, ATTR_ACCESS_MEMORY),						/* Field 3.Current average value                   */
		DYN_VALUE(CHOICE_FLOAT32, &g_Class05_last_Current_avg3, ATTR_ACCESS_MEMORY),				/* Field 4.Last average value                      */
		{0, PHY_UNIT_WATT_HOUR},																	/* Field 5.Scaler, unit                            */
		DYN_STATUS(CHOICE_OCTET_STRING(2), &g_Class05_Current_avg3_Status, ATTR_ACCESS_MEMORY),		/* Field 6.Status                                  */
		DYN_DATE_TIME(&g_Class05_Current_avg3_Time, ATTR_ACCESS_MEMORY),							/* Field 7.Capture time buffer                     */
		DYN_DATE_TIME(&g_Class05_Current_avg3_StartTime, ATTR_ACCESS_MEMORY),						/* Field 8.Start time current buffer               */
		&g_Class05_period,																			/* Field 9.Period                                  */
		&g_Class05_nr_periods,																		/* Field 10.Number of periods                      */
	}
};
const Unsigned16 g_ChildTableLengthClass05 = sizeof(g_ChildTableClass05) / sizeof(class05_child_record_t);
#endif

/****************************************************************************************
 * Class ID = 6, Register activation Child-table
 *
 * How to define a row of this table.
 *
 * Field 1. Fill the OBIS code first, 6 numbers (A, B, C, D, E, F)
 * Field 2. Specify which row on g_AccessRightTable will be used to declare the
 *          access right of the object.
 * Field 3.Current average value
 * Field 3.Register_assignment
 * Field 4.Register_assignment number
 * Field 5.Mask_list
 * Field 6.Mask_list number
 * Field 7.Active_mask
 ****************************************************************************************/
#if (defined(USED_CLASS_06) && USED_CLASS_06 == 1)
/* Variant parameters */
const Unsigned8	g_Class06_Mask_list1[]   = "RA mask 1";
const Unsigned8	g_Class06_index_list1[]   = {1,2};

/* Static parameters */
const obj_def_t		g_Class06_RA_assignment[] = {
	{ 1, {   0,   0,  96,   7,   0, 255 }}, /* Number of power failure */
	{ 1, {   0,   0,  94,  91,   0, 255 }}, /* Cucumulate Tamper count */
};
const register_act_mask_t	g_Class06_Mask_list[] = {
	{VALUE(CHOICE_OCTET_STRING(obis_strlen(g_Class06_Mask_list1)), &g_Class06_Mask_list1), g_Class06_index_list1, sizeof(g_Class06_index_list1)},
};

const class06_child_record_t g_ChildTableClass06[] =
{
	/* Register activation object */
	{
		{   0,   0,   14,   0,   0, 255 },																					/* Field 1.Logical name (OBIS code) of the object. */
		g_AccessRightTable[0],																								/* Field 2.Access right definition for 1 object    */
		g_Class06_RA_assignment,																							/* Field 3.Register_assignment                     */
		sizeof(g_Class06_RA_assignment) / sizeof(obj_def_t),																/* Field 4.Register_assignment number              */
		g_Class06_Mask_list,																								/* Field 5.Mask_list                               */
		sizeof(g_Class06_Mask_list) / sizeof(register_act_mask_t),															/* Field 6.Mask_list number                        */
		DYN_VALUE(CHOICE_OCTET_STRING(obis_strlen(g_Class06_Mask_list1)), &g_Class06_Mask_list1,ATTR_ACCESS_MEMORY),		/* Field 7.Active_mask                             */
	}
};
const Unsigned16 g_ChildTableLengthClass06 = sizeof(g_ChildTableClass06) / sizeof(class06_child_record_t);
#endif

/****************************************************************************************
 * Class ID = 7, Profile Generic Child-table
 *
 * How to define a row of this table.
 *
 * Field 1. Fill the OBIS code first, 6 numbers (A, B, C, D, E, F)
 * Field 2. Specify which row on g_AccessRightTable will be used to declare the
 *          access right of the object.
 * Field 3. Buffer
 * Field 4. Capture object list
 * Field 5. Capture object list length
 * Field 6. Capture period
 * Field 7. Sort method
 * Field 8. Sort object
 * Field 9. Entry in use
 * Field 10. Profile entries
 *
 * After define all above field, do the access of attr2-buffer inside the
 * R_OBIS_GetObjectAttr.
 *
 ****************************************************************************************/
#if (defined(USED_CLASS_07) && USED_CLASS_07 == 1)
/***************************************************************
 * Static shared parameters
 ***************************************************************/
/* Sort object */
const class07_capture_object_t g_Class07_SortObject =
{
	/* Empty */
	{0, 0, 0, 0, 0, 0},
	0,
	0,
	0,
};
/* Sort method */
const sort_method_t	g_Class07_SortMethod = SORT_METHOD_FIFO;

const Unsigned32				g_Class07_NoCapturePeriod = 0;
const Unsigned16				g_Class07_FixCurrentEntry = 0;
const Unsigned32				g_Class07_UniqueEntries = 1;


/***************************************************************
 * Instantaneous Parameters snapshot
 ***************************************************************/

/* Capture object list */
const class07_capture_object_t g_Class07_InstCaptureObjects[] =
{
	/*----------------------------------------------------------------------------*/
	/*          OBIS Code (A, B, C, D, E, F)    | Class ID	| Attr ID	| Data ID */
	/*----------------------------------------------------------------------------*/
	/* 00 */ {	{   0,   0,   1,   0,   0, 255 }, 	8	, 2	, 0		},	/* Clock */
	/* 01 */ {	{   1,   0,  31,   7,   0, 255 }, 	3	, 2	, 0		},	/* IR current  */
	/* 02 */ {	{   1,   0,  51,   7,   0, 255 }, 	3	, 2	, 0		},	/* IY current  */
	/* 03 */ {	{   1,   0,  71,   7,   0, 255 }, 	3	, 2	, 0		},	/* IB current  */
	
	/* 04 */ {	{   1,   0,  32,   7,   0, 255 }, 	3	, 2	, 0		},	/* VR Voltage  */
	/* 05 */ {	{   1,   0,  52,   7,   0, 255 }, 	3	, 2	, 0		},	/* VY Voltage  */
	/* 06 */ {	{   1,   0,  72,   7,   0, 255 }, 	3	, 2	, 0		},	/* VB Voltage  */
	
	/* 07 */ {	{   1,   0,  33,   7,   0, 255 }, 	3	, 2	, 0		},	/* Signed Power Factor R-phase */
	/* 08 */ {	{   1,   0,  53,   7,   0, 255 }, 	3	, 2	, 0		},	/* Signed Power Factor Y-phase*/
	/* 09 */ {	{   1,   0,  73,   7,   0, 255 }, 	3	, 2	, 0		},	/* Signed Power Factor B-phase*/
	/* 10 */ {	{   1,   0,  13,   7,   0, 255 }, 	3	, 2	, 0		},	/* Signed Power Factor Three_PH*/
	
	/* 11 */ {	{   1,   0,  14,   7,   0, 255 }, 	3	, 2	, 0		},	/* Frequency */
	/* 12 */ {	{   1,   0,   9,   7,   0, 255 }, 	3	, 2	, 0		},	/* Apparent Power */
	/* 13 */ {	{   1,   0,   1,   7,   0, 255 }, 	3	, 2	, 0		},	/* Signed Active Power   */
	/* 14 */ {	{   1,   0,   3,   7,   0, 255 }, 	3	, 2	, 0		},	/* Signed Reactive Power   */
	
	/* 15 */ {	{   0,   0,  96,   7,   0, 255 }, 	1	, 2	, 0		},	/* Number of power � failures */
	/* 16 */ {	{   0,   0,  94,  91,   8, 255 }, 	3	, 2	, 0		},	/* Cumulative power FAIL duration in minutes */
	/* 17 */ {	{   0,   0,  94,  91,   0, 255 }, 	1	, 2	, 0		},	/* Cumulative tamper count */
	/* 18 */ {	{   0,   0,   0,   1,   0, 255 }, 	1	, 2	, 0		},	/* Cumulative billing count */
	/* 19 */ {	{   0,   0,  96,   2,   0, 255 }, 	1	, 2	, 0		},	/* Cumulative programming count */
	
	/* 20 */ {	{   0,   0,   0,   1,   2, 255 }, 	3	, 2	, 0		},	/* Billing date */
	
	/* 21 */ {	{   1,   0,   1,   8,   0, 255 }, 	3	, 2	, 0		},	/* Cumulative Energy - kWh */
	/* 22 */ {	{   1,   0,   5,   8,   0, 255 }, 	3	, 2	, 0		},	/* Cumulative energy, kvarh (Lag)*/
	/* 23 */ {	{   1,   0,   8,   8,   0, 255 }, 	3	, 2	, 0		},	/* Cumulative energy, kvarh (Lead)*/
	/* 24 */ {	{   1,   0,   9,   8,   0, 255 }, 	3	, 2	, 0		},	/* Cumulative Energy - kVAh*/	
	
	/* 25 */ {	{   1,   0,   1,   6,   0, 255 }, 	4	, 2	, 0		},	/* Maximum Demand , kW */
	/* 26 */ {	{   1,   0,   1,   6,   0, 255 }, 	4	, 5	, 0		},	/* Maximum Demand , kW time */
	/* 27 */ {	{   1,   0,   9,   6,   0, 255 }, 	4	, 2	, 0		},	/* Maximum Demand , kVA */
	/* 28 */ {	{   1,   0,   9,   6,   0, 255 }, 	4	, 5	, 0		},	/* Maximum Demand , kVA time */
#ifdef BIDIR_METER	
	/* 29 */ {	{   1,   0,   2,   8,   0, 255 }, 	3	, 2	, 0		},	/* Cumulative Energy - kWh export added 22/02/2020 */
#endif	
};
const Unsigned16 	g_Class07_InstCaptureObjectsLength = sizeof(g_Class07_InstCaptureObjects) / sizeof(class07_capture_object_t);
/* Capture object list Scaler */
const class07_capture_object_t g_Class07_InstCaptureObjects_Scaler[] =
{
	/*----------------------------------------------------------------------------*/
	/*          OBIS Code (A, B, C, D, E, F)    | Class ID	| Attr ID	| Data ID */
	/*----------------------------------------------------------------------------*/
	///* 00 */ {	{   0,   0,   1,   0,   0, 255 }, 8			, 2			, 0		},	/* Clock */
	/* 01 */ {	{   1,   0,  31,   7,   0, 255 }, 3			, 3			, 0		},	/* IR current  */
	/* 02 */ {	{   1,   0,  51,   7,   0, 255 }, 3			, 3			, 0		},	/* IY current  */
	/* 03 */ {	{   1,   0,  71,   7,   0, 255 }, 3			, 3			, 0		},	/* IB current  */
	
	/* 04 */ {	{   1,   0,  32,   7,   0, 255 }, 3			, 3			, 0		},	/* VR Voltage  */
	/* 05 */ {	{   1,   0,  52,   7,   0, 255 }, 3			, 3			, 0		},	/* VY Voltage  */
	/* 06 */ {	{   1,   0,  72,   7,   0, 255 }, 3			, 3			, 0		},	/* VB Voltage  */
	
	/* 07 */ {	{   1,   0,  33,   7,   0, 255 }, 3			, 3			, 0		},	/* Signed Power Factor R-phase */
	/* 08 */ {	{   1,   0,  53,   7,   0, 255 }, 3			, 3			, 0		},	/* Signed Power Factor Y-phase*/
	/* 09 */ {	{   1,   0,  73,   7,   0, 255 }, 3			, 3			, 0		},	/* Signed Power Factor B-phase*/
	/* 10 */ {	{   1,   0,  13,   7,   0, 255 }, 3			, 3			, 0		},	/* Signed Power Factor Three_PH*/
	
	/* 11 */ {	{   1,   0,  14,   7,   0, 255 }, 3			, 3			, 0		},	/* Frequency */
	/* 12 */ {	{   1,   0,   9,   7,   0, 255 }, 3			, 3			, 0		},	/* Apparent Power */
	/* 13 */ {	{   1,   0,   1,   7,   0, 255 }, 3			, 3			, 0		},	/* Signed Active Power   */
	/* 14 */ {	{   1,   0,   3,   7,   0, 255 }, 3			, 3			, 0		},	/* Signed Reactive Power   */
	
	///* 15 */ {	{   0,   0,  96,   7,   0, 255 }, 1			, 2			, 0		},	/* Number of power � failures */
	/* 16 */ {	{   0,   0,  94,  91,   8, 255 }, 3			, 3			, 0		},	/* Cumulative power FAIL duration in minutes */
	///* 17 */ {	{   0,   0,  94,  91,   0, 255 }, 1			, 2			, 0		},	/* Cumulative tamper count */
	///* 18 */ {	{   0,   0,   0,   1,   0, 255 }, 1			, 2			, 0		},	/* Cumulative billing count */
	///* 19 */ {	{   0,   0,  96,   2,   0, 255 }, 1			, 2			, 0		},	/* Cumulative programming count */
	
	///* 20 */ {	{   0,   0,   0,   1,   2, 255 }, 3			, 3			, 0		},	/* Billing date */
	/* 21 */ {	{   1,   0,   1,   8,   0, 255 }, 3			, 3			, 0		},	/* Cumulative Energy - kWh */
	/* 22 */ {	{   1,   0,   5,   8,   0, 255 }, 3			, 3			, 0		},	/* Cumulative energy, kvarh (Lag)*/
	/* 23 */ {	{   1,   0,   8,   8,   0, 255 }, 3			, 3			, 0		},	/* Cumulative energy, kvarh (Lead)*/
	/* 24 */ {	{   1,   0,   9,   8,   0, 255 }, 3			, 3			, 0		},	/* Cumulative Energy - kVAh*/	
	
	/* 25 */ {	{   1,   0,   1,   6,   0, 255 }, 4			, 3			, 0		},	/* Maximum Demand , kW */
	///* 26 */ {	{   1,   0,   1,   6,   0, 255 }, 4			, 5			, 0		},	/* Maximum Demand , kW time */
	/* 27 */ {	{   1,   0,   9,   6,   0, 255 }, 4			, 3			, 0		},	/* Maximum Demand , kVA */
	///* 28 */ {	{   1,   0,   9,   6,   0, 255 }, 4			, 5			, 0		},	/* Maximum Demand , kVA time */
#ifdef BIDIR_METER	
	/* 29 */ {	{   1,   0,   2,   8,   0, 255 }, 3			, 3			, 0		},	/* Cumulative Energy - kWh export - added 22/02/2020*/
#endif	
};
const Unsigned16 	g_Class07_InstCaptureObjectsLength_Scaler = sizeof(g_Class07_InstCaptureObjects_Scaler) / sizeof(class07_capture_object_t);

buffer_t g_class07_InstSnapshot_Buf = {
	NULL,												/* Buffer of value                                     */
	NULL,												/* Pointer to access method option                     */
	(Unsigned16 *)&g_Class07_FixCurrentEntry,									/* Pointer to index of current entries in buffer       */
	0,												/* Number of entries per block to transfer             */
	90,												/* Length of encode 1 entry after filter  - 85 earlier             */
	0,												/* Number of remain entries in run-time                */
	0,												/* First entry to retrieve in run-time                 */
	0,												/* Last entry to retrieve in run-time                  */
	0,												/* Index of first value to retrieve in run-time        */
	0,												/* Index of last value to retrieve  in run-time        */
};

/***************************************************************
 * Instantaneous Scaler Profile
 ***************************************************************/
buffer_t g_class07_InstScaler_Buf = {
	(scaler_unit_t *)g_Class07_InstScalerBuffer,									/* Buffer of value                                     */
	NULL,												/* Pointer to access method option                     */
	(Unsigned16 *)&g_Class07_FixCurrentEntry,									/* Pointer to index of current entries in buffer       */
	0,												/* Number of entries per block to transfer             */
	98,												/* Length of encode 1 entry after filter               */
	0,												/* Number of remain entries in run-time                */
	0,												/* First entry to retrieve in run-time                 */
	0,												/* Last entry to retrieve in run-time                  */
	0,												/* Index of first value to retrieve in run-time        */
	0,												/* Index of last value to retrieve  in run-time        */
};

/***************************************************************
 * Block Load Profile
 ***************************************************************/

/* Capture object list */
const class07_capture_object_t g_Class07_BlockloadCaptureObjects[] =
{
//						u8	logical_name[6];						u16 class_id; u16 attr_id; u16 data_id	
	/*----------------------------------------------------------------------------*/
	/*          OBIS Code (A, B, C, D, E, F)    | Class ID	| Attr ID	| Data ID */
	/*----------------------------------------------------------------------------*/
	/* 00 */ {	{   0,   0,   1,    0,   0, 255 }, 	8			, 2			, 0		},	/* Clock */
#ifdef SURVEY_USES_AVERAGE_DATA	
	/* 01 */ {	{   1,   0,  31,   27,   0, 255 }, 	3			, 2			, 0		},	/* IR current  */
	/* 02 */ {	{   1,   0,  51,   27,   0, 255 }, 	3			, 2			, 0		},	/* IY current  */
	/* 03 */ {	{   1,   0,  71,   27,   0, 255 }, 	3			, 2			, 0		},	/* IB current  */
	
	/* 04 */ {	{   1,   0,  32,   27,   0, 255 }, 	3			, 2			, 0		},	/* VR Voltage  */
	/* 05 */ {	{   1,   0,  52,   27,   0, 255 }, 	3			, 2			, 0		},	/* VY Voltage  */
	/* 06 */ {	{   1,   0,  72,   27,   0, 255 }, 	3			, 2			, 0		},	/* VB Voltage  */
#endif

	/* 07 */ {	{   1,   0,   1,   29,   0, 255 }, 	3			, 2			, 0		},	/* Block Energy - kWh */
#ifdef SURVEY_USES_REACTIVE_DATA	
	/* 08 */ {	{   1,   0,   5,   29,   0, 255 }, 	3			, 2			, 0		},	/* Block Energy, kvarh (Lag)*/
	/* 09 */ {	{   1,   0,   8,   29,   0, 255 }, 	3			, 2			, 0		},	/* Block Energy, kvarh (Lead)*/
#endif	
	/* 10 */ {	{   1,   0,   9,   29,   0, 255 }, 	3			, 2			, 0		},	/* Block Energy - kVAh*/	
	
#ifdef SURVEY_USES_EXPORT_DATA	
	/* 11 */ {	{   1,   0,   2,   29,   0, 255 }, 	3			, 2			, 0		},	/* Block Energy - kWh export */
#endif

#ifdef SURVEY_USES_AVERAGE_PF
	/* 12 */ {	{   1,   0,  33,   27,   0, 255 }, 	3			, 2			, 0		},	/* PF average - R  */
	/* 13 */ {	{   1,   0,  53,   27,   0, 255 }, 	3			, 2			, 0		},	/* PF average - Y */
	/* 14 */ {	{   1,   0,  73,   27,   0, 255 }, 	3			, 2			, 0		},	/* PF average - B */
#endif	

#ifdef SURVEY_USES_FREQ_DATA
	/* 15 */ {	{   1,   0,  14,    7,   0, 255 }, 	3			, 2			, 0		},	/* Frequency  */
#endif

#ifdef SURVEY_USES_DEMAND_DATA
	/* 16 */ {	{   1,   0,   9,    7,   0, 255 }, 	3			, 2			, 0		},	/* Apparent Demand */
	/* 17 */ {	{   1,   0,   1,    7,   0, 255 }, 	3			, 2			, 0		},	/* Active Demand */
#endif
};

const Unsigned16 	g_Class07_BlockloadCaptureObjectsLength = sizeof(g_Class07_BlockloadCaptureObjects) / sizeof(class07_capture_object_t);

/* Capture object list Scaler */

const class07_capture_object_t g_Class07_BlockloadCaptureObjects_Scaler[] =
{
	/*----------------------------------------------------------------------------*/
	/*          OBIS Code (A, B, C, D, E, F)    | Class ID	| Attr ID	| Data ID */
	/*----------------------------------------------------------------------------*/
	///* 00 */ {	{   0,   0,   1,    0,   0, 255 }, 8			, 2			, 0		},	/* Clock */
#ifdef SURVEY_USES_AVERAGE_DATA	
	/* 01 */ {	{   1,   0,  31,   27,   0, 255 }, 3			, 3			, 0		},	/* IR current  */
	/* 02 */ {	{   1,   0,  51,   27,   0, 255 }, 3			, 3			, 0		},	/* IY current  */
	/* 03 */ {	{   1,   0,  71,   27,   0, 255 }, 3			, 3			, 0		},	/* IB current  */
	
	/* 04 */ {	{   1,   0,  32,   27,   0, 255 }, 3			, 3			, 0		},	/* VR Voltage  */
	/* 05 */ {	{   1,   0,  52,   27,   0, 255 }, 3			, 3			, 0		},	/* VY Voltage  */
	/* 06 */ {	{   1,   0,  72,   27,   0, 255 }, 3			, 3			, 0		},	/* VB Voltage  */
#endif

	/* 07 */ {	{   1,   0,   1,   29,   0, 255 }, 3			, 3			, 0		},	/* Block Energy - kWh */
	
#ifdef SURVEY_USES_REACTIVE_DATA
	/* 08 */ {	{   1,   0,   5,   29,   0, 255 }, 3			, 3			, 0		},	/* Block Energy, kvarh (Lag)*/
	/* 09 */ {	{   1,   0,   8,   29,   0, 255 }, 3			, 3			, 0		},	/* Block Energy, kvarh (Lead)*/
#endif

	/* 10 */ {	{   1,   0,   9,   29,   0, 255 }, 3			, 3			, 0		},	/* Block Energy - kVAh*/	
	
#ifdef SURVEY_USES_EXPORT_DATA	
	/* 11 */ {	{   1,   0,   2,   29,   0, 255 }, 3			, 3			, 0		},	/* Block Energy - kWh Export*/	
#endif	

#ifdef SURVEY_USES_AVERAGE_PF
	/* 12 */ {	{   1,   0,  33,   27,   0, 255 }, 	3			, 3			, 0		},	/* PF average - R  */
	/* 13 */ {	{   1,   0,  53,   27,   0, 255 }, 	3			, 3			, 0		},	/* PF average - Y */
	/* 14 */ {	{   1,   0,  73,   27,   0, 255 }, 	3			, 3			, 0		},	/* PF average - B */
#endif

#ifdef SURVEY_USES_FREQ_DATA
	/* 15 */ {	{   1,   0,  14,    7,   0, 255 }, 	3			, 3			, 0		},	/* Frequency  */
#endif

#ifdef SURVEY_USES_DEMAND_DATA
	/* 16 */ {	{   1,   0,   9,    7,   0, 255 }, 	3			, 3			, 0		},	/* Apparent Demand */
	/* 17 */ {	{   1,   0,   1,    7,   0, 255 }, 	3			, 3			, 0		},	/* Active Demand */
#endif
};
const Unsigned16 	g_Class07_BlockloadCaptureObjectsLength_Scaler = sizeof(g_Class07_BlockloadCaptureObjects_Scaler) / sizeof(class07_capture_object_t);


Unsigned16			g_Class07_Blockload_CurrentEntry = 0;	
Unsigned32		g_Class07_Blockload_CapturePeriod = CLASS07_BLOCKLOAD_INTERVAL;
Unsigned32			g_Class07_Blockload_EntriesInUse = 0;
/*
 * Number of entries is defined as in r_dlms_data.c,
 * the value is specified by macro CLASS07_BLOCKLOAD_MAX_ENTRY
 */
 const Unsigned32	g_Class07_Blockload_MaxEntries = CLASS07_BLOCKLOAD_MAX_ENTRY;

buffer_t g_Class07_Blockload_Buf = {
	&g_Class07_BlockLoadBuffer,					// Buffer of value                                     
	NULL,																// Pointer to access method option
	&g_Class07_Blockload_CurrentEntry,	// Pointer to index of current entries in buffer
	0,																	// Number of entries per block to transfer
	71,																	// Length of encode 1 entry after filter 2 + 14 + 10*5 + 5(latest entry export kwh)
	0,																	// Number of remain entries in run-time
	0,																	// First entry to retrieve in run-time
	0,																	// Last entry to retrieve in run-time
	0,																	// Index of first value to retrieve in run-time
	0,																	// Index of last value to retrieve  in run-time
};
/***************************************************************
 * Block Load Scaler Profile
 ***************************************************************/
buffer_t g_Class07_BlockloadScaler_Buf = {
	(scaler_unit_t *)g_Class07_BlockloadScalerBuffer,								/* Buffer of value                                     */
	NULL,												/* Pointer to access method option                     */
	(Unsigned16 *)&g_Class07_FixCurrentEntry,									/* Pointer to index of current entries in buffer       */
	0,												/* Number of entries per block to transfer             */
	26,												/* Length of encode 1 entry after filter  */
	0,												/* Number of remain entries in run-time                */
	0,												/* First entry to retrieve in run-time                 */
	0,												/* Last entry to retrieve in run-time                  */
	0,												/* Index of first value to retrieve in run-time        */
	0,												/* Index of last value to retrieve  in run-time        */
};

/***************************************************************
 * Daily Load Profile 
 ***************************************************************/

/* Capture object list */
#ifdef SMALL_DAILY_LOG
	const class07_capture_object_t g_Class07_DailyloadCaptureObjects[] =
	{
		/*----------------------------------------------------------------------------*/
		/*          OBIS Code (A, B, C, D, E, F)    | Class ID	| Attr ID	| Data ID */
		/*----------------------------------------------------------------------------*/
		/* 00 */ {	{   0,   0,   1,   0,   0, 255 }, 	8			, 2			, 0		},	/* Clock */
		/* 01 */ {	{   1,   0,   1,   8,   0, 255 }, 	3			, 2			, 0		},	/* Cumulative Energy � kWh */
	#ifdef BIDIR_METER	
		/* 02 */ {	{   1,   0,   2,   8,   0, 255 }, 	3			, 2			, 0		},	/* Cumulative Energy � kWh export */
	#else	
		/* 02 */ {	{   1,   0,   9,   8,   0, 255 }, 	3			, 2			, 0		},	/* Cumulative Energy � kVAh */
	#endif
	};
	const Unsigned16 	g_Class07_DailyloadCaptureObjectsLength = sizeof(g_Class07_DailyloadCaptureObjects) / sizeof(class07_capture_object_t);
	/* Capture object list Scaler */
	const class07_capture_object_t g_Class07_DailyloadCaptureObjects_Scaler[] =
	{
		/*----------------------------------------------------------------------------*/
		/*          OBIS Code (A, B, C, D, E, F)    | Class ID	| Attr ID	| Data ID */
		/*----------------------------------------------------------------------------*/
		/* 00 */// {	{   0,   0,   1,   0,   0, 255 }, 	8			, 2			, 0		},	/* Clock */
		/* 01 */ {	{   1,   0,   1,   8,   0, 255 }, 	3			, 3			, 0		},	/* Cumulative Energy � kWh */
	#ifdef BIDIR_METER	
		/* 02 */ {	{   1,   0,   2,   8,   0, 255 }, 	3			, 3			, 0		},	/* Cumulative Energy � kWh */
	#else
		/* 02 */ {	{   1,   0,   9,   8,   0, 255 }, 	3			, 3			, 0		},	/* Cumulative Energy � kVAh */
	#endif
	};
#else
	#ifdef DAILY_POWER_ON_LOGGING
	const class07_capture_object_t g_Class07_DailyloadCaptureObjects[] =
	{
		/*----------------------------------------------------------------------------*/
		/*          OBIS Code (A, B, C, D, E, F)    | Class ID	| Attr ID	| Data ID */
		/*----------------------------------------------------------------------------*/
		/* 00 */ {	{   0,   0,   1,   0,   0, 255 }, 	8			, 2			, 0		},	/* Clock */
		/* 01 */ {	{   1,   0,   1,   8,   0, 255 }, 	3			, 2			, 0		},	/* Cumulative Energy � kWh */
		/* 02 */ {	{   1,   0,   9,   8,   0, 255 }, 	3			, 2			, 0		},	/* Cumulative Energy � kVAh */
		/* 03 */ {	{   0,   0,  94,  91,  13, 255 }, 	3			, 2			, 0		},	/* Daily Power On Minutes */
	};
	const Unsigned16 	g_Class07_DailyloadCaptureObjectsLength = sizeof(g_Class07_DailyloadCaptureObjects) / sizeof(class07_capture_object_t);
	/* Capture object list Scaler */
	const class07_capture_object_t g_Class07_DailyloadCaptureObjects_Scaler[] =
	{
		/*----------------------------------------------------------------------------*/
		/*          OBIS Code (A, B, C, D, E, F)    | Class ID	| Attr ID	| Data ID */
		/*----------------------------------------------------------------------------*/
		/* 00 */// {	{   0,   0,   1,   0,   0, 255 }, 	8			, 2			, 0		},	/* Clock */
		/* 01 */ {	{   1,   0,   1,   8,   0, 255 }, 	3			, 3			, 0		},	/* Cumulative Energy � kWh */
		/* 02 */ {	{   1,   0,   9,   8,   0, 255 }, 	3			, 3			, 0		},	/* Cumulative Energy � kVAh */
		/* 03 */ {	{   0,   0,  94,  91,  13, 255 }, 	3			, 3			, 0		},	/* Daily Power On Minutes */
	};
	#else
	const class07_capture_object_t g_Class07_DailyloadCaptureObjects[] =
	{
		/*----------------------------------------------------------------------------*/
		/*          OBIS Code (A, B, C, D, E, F)    | Class ID	| Attr ID	| Data ID */
		/*----------------------------------------------------------------------------*/
		/* 00 */ {	{   0,   0,   1,   0,   0, 255 }, 	8			, 2			, 0		},	/* Clock */
		/* 01 */ {	{   1,   0,   1,   8,   0, 255 }, 	3			, 2			, 0		},	/* Cumulative Energy � kWh */
		/* 02 */ {	{   1,   0,   9,   8,   0, 255 }, 	3			, 2			, 0		},	/* Cumulative Energy � kVAh */
		/* 03 */ {	{   1,   0,   5,   8,   0, 255 }, 	3			, 2			, 0		},	/* Cumulative Energy � kVArh lag */
		/* 04 */ {	{   1,   0,   8,   8,   0, 255 }, 	3			, 2			, 0		},	/* Cumulative Energy � kVArh lag */
	};
	const Unsigned16 	g_Class07_DailyloadCaptureObjectsLength = sizeof(g_Class07_DailyloadCaptureObjects) / sizeof(class07_capture_object_t);
	/* Capture object list Scaler */
	const class07_capture_object_t g_Class07_DailyloadCaptureObjects_Scaler[] =
	{
		/*----------------------------------------------------------------------------*/
		/*          OBIS Code (A, B, C, D, E, F)    | Class ID	| Attr ID	| Data ID */
		/*----------------------------------------------------------------------------*/
		/* 00 */// {	{   0,   0,   1,   0,   0, 255 }, 	8			, 2			, 0		},	/* Clock */
		/* 01 */ {	{   1,   0,   1,   8,   0, 255 }, 	3			, 3			, 0		},	/* Cumulative Energy � kWh */
		/* 02 */ {	{   1,   0,   9,   8,   0, 255 }, 	3			, 3			, 0		},	/* Cumulative Energy � kVAh */
		/* 03 */ {	{   1,   0,   5,   8,   0, 255 }, 	3			, 3			, 0		},	/* Cumulative Energy � kVArh lag */
		/* 04 */ {	{   1,   0,   8,   8,   0, 255 }, 	3			, 3			, 0		},	/* Cumulative Energy � kVArh lag */
	};
	#endif
#endif

const Unsigned16 	g_Class07_DailyloadCaptureObjectsLength_Scaler = sizeof(g_Class07_DailyloadCaptureObjects_Scaler) / sizeof(class07_capture_object_t);

Unsigned16			g_Class07_Dailyload_CurrentEntry = 0;
const Unsigned32		g_Class07_Dailyload_CapturePeriod = CLASS07_DAILYLOAD_INTERVAL;
Unsigned32			g_Class07_Dailyload_EntriesInUse = 0;
/*
 * Number of entries is defined in r_dlms_data.c,
 * the value is specified by macro CLASS07_DAILYLOAD_MAX_ENTRY
 */
const Unsigned32	g_Class07_Dailyload_MaxEntries = CLASS07_DAILYLOAD_MAX_ENTRY;

buffer_t g_Class07_Dailyload_Buf = {
	&g_Class07_DailyLoadBuffer,									/* Buffer of value                                     */
	NULL,												/* Pointer to access method option                     */
	&g_Class07_Dailyload_CurrentEntry,								/* Pointer to index of current entries in buffer       */
	0,												/* Number of entries per block to transfer             */
	26,												/* Length of encode 1 entry after filter               */
	0,												/* Number of remain entries in run-time                */
	0,												/* First entry to retrieve in run-time                 */
	0,												/* Last entry to retrieve in run-time                  */
	0,												/* Index of first value to retrieve in run-time        */
	0,												/* Index of last value to retrieve  in run-time        */
};

/***************************************************************
 * Daily Load Scaler Profile Scaler
 ***************************************************************/
buffer_t g_Class07_DailyloadScaler_Buf = {
	(scaler_unit_t *)g_Class07_DailyloadScalerBuffer,				/* Buffer of value                                     */
	NULL,											/* Pointer to access method option                     */
	(Unsigned16 *)&g_Class07_FixCurrentEntry,						/* Pointer to index of current entries in buffer       */
	0,												/* Number of entries per block to transfer             */
	20,												/* Length of encode 1 entry after filter               */
	0,												/* Number of remain entries in run-time                */
	0,												/* First entry to retrieve in run-time                 */
	0,												/* Last entry to retrieve in run-time                  */
	0,												/* Index of first value to retrieve in run-time        */
	0,												/* Index of last value to retrieve  in run-time        */
};

/***************************************************************
 * Billing Profile
 ***************************************************************/
/* Capture object list */

//#ifdef BIDIR_METER
const class07_capture_object_t g_Class07_BillingCaptureObjects[] =
{
/*----------------------------------------------------------------------------*/
/*          OBIS Code (A, B, C, D, E, F)    | Class ID	| Attr ID	| Data ID */
/*----------------------------------------------------------------------------*/
/* 00 */ {	{   0,   0,   0,   1,   2, 255 }, 	3	, 2	, 0	},	/* Billing date */
/* 01 */ {	{   1,   0,  13,   0,   0, 255 }, 	3	, 2	, 0	},	/* Average Power Factor for billing period */
/* 02 */ {	{   1,   0,   1,   8,   0, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy � kWh */
/* 03 */ {	{   1,   0,   1,   8,   1, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy kWh TZ1 */
/* 04 */ {	{   1,   0,   1,   8,   2, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy kWh TZ2 */
/* 05 */ {	{   1,   0,   1,   8,   3, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy kWh TZ3 */
/* 06 */ {	{   1,   0,   1,   8,   4, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy kWh TZ4 */
/* 07 */ {	{   1,   0,   1,   8,   5, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy kWh TZ5 */
#if (defined(TARIFFS) && TARIFFS == 8)
/* 05 */ {	{   1,   0,   1,   8,   6, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy kWh TZ6 */
/* 06 */ {	{   1,   0,   1,   8,   7, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy kWh TZ7 */
/* 07 */ {	{   1,   0,   1,   8,   8, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy kWh TZ8 */
#endif
/* 08 */ {	{   1,   0,   5,   8,   0, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy � kvarh (Lag)*/
/* 09 */ {	{   1,   0,   8,   8,   0, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy � kvarh (Lead) */
/* 10 */ {	{   1,   0,   9,   8,   0, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy � kVAh */
/* 11 */ {	{   1,   0,   9,   8,   1, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy kVAh TZ1 */
/* 12 */ {	{   1,   0,   9,   8,   2, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy kVAh TZ2 */
/* 13 */ {	{   1,   0,   9,   8,   3, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy kVAh TZ3 */
/* 14 */ {	{   1,   0,   9,   8,   4, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy kVAh TZ4 */
/* 15 */ {	{   1,   0,   9,   8,   5, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy kVAh TZ5 */
#if (defined(TARIFFS) && TARIFFS == 8)
/* 13 */ {	{   1,   0,   9,   8,   6, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy kVAh TZ6 */
/* 14 */ {	{   1,   0,   9,   8,   7, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy kVAh TZ7 */
/* 15 */ {	{   1,   0,   9,   8,   8, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy kVAh TZ8 */
#endif
/* 16 */ {	{   1,   0,   1,   6,   0, 255 }, 	4	, 2	, 0	},	/* MD kW */
/* 17 */ {	{   1,   0,   1,   6,   0, 255 }, 	4	, 5	, 0	},	/* MD kW Date Time*/
/* 18 */ {	{   1,   0,   1,   6,   1, 255 }, 	4	, 2	, 0	},	/* MD, kW for TZ1 */
/* 19 */ {	{   1,   0,   1,   6,   1, 255 }, 	4	, 5	, 0	},	/* MD kW Date Time*/
/* 20 */ {	{   1,   0,   1,   6,   2, 255 }, 	4	, 2	, 0	},	/* MD, kW for TZ2*/		
/* 21 */ {	{   1,   0,   1,   6,   2, 255 }, 	4	, 5	, 0	},	/* MD kW Date Time*/		
/* 22 */ {	{   1,   0,   1,   6,   3, 255 }, 	4	, 2	, 0	},	/* MD, kW for TZ3 */		
/* 23 */ {	{   1,   0,   1,   6,   3, 255 }, 	4	, 5	, 0	},	/* MD kW Date Time*/		
/* 24 */ {	{   1,   0,   1,   6,   4, 255 }, 	4	, 2	, 0	},	/* MD, kW for TZ4 */		
/* 25 */ {	{   1,   0,   1,   6,   4, 255 }, 	4	, 5	, 0	},	/* MD kW Date Time*/		
/* 26 */ {	{   1,   0,   1,   6,   5, 255 }, 	4	, 2	, 0	},	/* MD, kW for TZ5 */		
/* 27 */ {	{   1,   0,   1,   6,   5, 255 }, 	4	, 5	, 0	},	/* MD kW Date Time*/		
#if (defined(TARIFFS) && TARIFFS == 8)
/* 22 */ {	{   1,   0,   1,   6,   6, 255 }, 	4	, 2	, 0	},	/* MD, kW for TZ6 */		
/* 23 */ {	{   1,   0,   1,   6,   6, 255 }, 	4	, 5	, 0	},	/* MD kW Date Time*/		
/* 24 */ {	{   1,   0,   1,   6,   7, 255 }, 	4	, 2	, 0	},	/* MD, kW for TZ7 */		
/* 25 */ {	{   1,   0,   1,   6,   7, 255 }, 	4	, 5	, 0	},	/* MD kW Date Time*/		
/* 26 */ {	{   1,   0,   1,   6,   8, 255 }, 	4	, 2	, 0	},	/* MD, kW for TZ7 */		
/* 27 */ {	{   1,   0,   1,   6,   8, 255 }, 	4	, 5	, 0	},	/* MD kW Date Time*/		
#endif
/* 28 */ {	{   1,   0,   9,   6,   0, 255 }, 	4	, 2	, 0	},	/* MD kVA */		
/* 29 */ {	{   1,   0,   9,   6,   0, 255 }, 	4	, 5	, 0	},	/* MD kVA Date Time */		
/* 30 */ {	{   1,   0,   9,   6,   1, 255 }, 	4	, 2	, 0	},	/* MD, kVA for TZ1 */		
/* 31 */ {	{   1,   0,   9,   6,   1, 255 }, 	4	, 5	, 0	},	/* MD kVA Date Time */		
/* 32 */ {	{   1,   0,   9,   6,   2, 255 }, 	4	, 2	, 0	},	/* MD, kVA for TZ2 */		
/* 33 */ {	{   1,   0,   9,   6,   2, 255 }, 	4	, 5	, 0	},	/* MD kVA Date Time */		
/* 34 */ {	{   1,   0,   9,   6,   3, 255 }, 	4	, 2	, 0	},	/* MD, kVA for TZ3*/		
/* 35 */ {	{   1,   0,   9,   6,   3, 255 }, 	4	, 5	, 0	},	/* MD kVA Date Time */		
/* 36 */ {	{   1,   0,   9,   6,   4, 255 }, 	4	, 2	, 0	},	/* MD, kVA for TZ4 */		
/* 37 */ {	{   1,   0,   9,   6,   4, 255 }, 	4	, 5	, 0	},	/* MD kVA Date Time */		
/* 38 */ {	{   1,   0,   9,   6,   5, 255 }, 	4	, 2	, 0	},	/* MD, kVA for TZ5 */		
/* 39 */ {	{   1,   0,   9,   6,   5, 255 }, 	4	, 5	, 0	},	/* MD kVA Date Time */		
#if (defined(TARIFFS) && TARIFFS == 8)
/* 34 */ {	{   1,   0,   9,   6,   6, 255 }, 	4	, 2	, 0	},	/* MD, kVA for TZ6*/		
/* 35 */ {	{   1,   0,   9,   6,   6, 255 }, 	4	, 5	, 0	},	/* MD kVA Date Time */		
/* 36 */ {	{   1,   0,   9,   6,   7, 255 }, 	4	, 2	, 0	},	/* MD, kVA for TZ7 */		
/* 37 */ {	{   1,   0,   9,   6,   7, 255 }, 	4	, 5	, 0	},	/* MD kVA Date Time */		
/* 38 */ {	{   1,   0,   9,   6,   8, 255 }, 	4	, 2	, 0	},	/* MD, kVA for TZ8 */		
/* 39 */ {	{   1,   0,   9,   6,   8, 255 }, 	4	, 5	, 0	},	/* MD kVA Date Time */		
#endif
/* 40 */ {	{   0,   0,  94,  91,  13, 255 }, 	3	, 2	, 0	},	/* Billing power ON duration in minutes (During Billing period) */
#ifdef BILLING_HAS_TAMPER_COUNT
/* 40 */ {	{   0,   0,  94,  91,   0, 255 }, 	3	, 2	, 0	},	/* Cumulative Tamper Count */
#endif
#ifdef TOD_HAS_KVARH_LAG
/* 41 */ {	{   1,   0,   5,   8,   1, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy kVArh lag TZ1 */
/* 42 */ {	{   1,   0,   5,   8,   2, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy kVArh lag TZ2 */
/* 43 */ {	{   1,   0,   5,   8,   3, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy kVArh lag TZ3 */
/* 44 */ {	{   1,   0,   5,   8,   4, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy kVArh lag TZ4 */
/* 45 */ {	{   1,   0,   5,   8,   5, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy kVArh lag TZ5 */
#if (defined(TARIFFS) && TARIFFS == 8)
/* 43 */ {	{   1,   0,   5,   8,   6, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy kVArh lag TZ6 */
/* 44 */ {	{   1,   0,   5,   8,   7, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy kVArh lag TZ7 */
/* 45 */ {	{   1,   0,   5,   8,   8, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy kVArh lag TZ8 */
#endif
#endif
#ifdef BIDIR_METER
/* 46 */ {	{   1,   0,   2,   8,   0, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy � kWh -export */
/* 47 */ {	{   1,   0,   2,   8,   1, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy kWh TZ1 -export */
/* 48 */ {	{   1,   0,   2,   8,   2, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy kWh TZ2 -export */
/* 49 */ {	{   1,   0,   2,   8,   3, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy kWh TZ3 -export */
/* 50 */ {	{   1,   0,   2,   8,   4, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy kWh TZ4 -export */
/* 51 */ {	{   1,   0,   2,   8,   5, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy kWh TZ5 -export */
/* 52 */ {	{   1,   0,   5,   9,   0, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy � kvarh (Lag) -export */
/* 53 */ {	{   1,   0,   8,   9,   0, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy � kvarh (Lead) -export */
/* 54 */ {	{   1,   0,   10,   8,   0, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy � kVAh -export */
/* 55 */ {	{   1,   0,   10,   8,   1, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy kVAh TZ1 -export */
/* 56 */ {	{   1,   0,   10,   8,   2, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy kVAh TZ2 -export */
/* 57 */ {	{   1,   0,   10,   8,   3, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy kVAh TZ3 -export */
/* 58 */ {	{   1,   0,   10,   8,   4, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy kVAh TZ4 -export */
/* 59 */ {	{   1,   0,   10,   8,   5, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy kVAh TZ5 -export */
/* 60 */ {	{   1,   0,   2,   6,   0, 255 }, 	4	, 2	, 0	},	/* MD kW -export */
/* 61 */ {	{   1,   0,   2,   6,   0, 255 }, 	4	, 5	, 0	},	/* MD kW Date Time -export */
/* 62 */ {	{   1,   0,   2,   6,   1, 255 }, 	4	, 2	, 0	},	/* MD, kW for TZ1 -export */
/* 62 */ {	{   1,   0,   2,   6,   1, 255 }, 	4	, 5	, 0	},	/* MD kW Date Time -export */
/* 64 */ {	{   1,   0,   2,   6,   2, 255 }, 	4	, 2	, 0	},	/* MD, kW for TZ2 -export */		
/* 65 */ {	{   1,   0,   2,   6,   2, 255 }, 	4	, 5	, 0	},	/* MD kW Date Time -export */		
/* 66 */ {	{   1,   0,   2,   6,   3, 255 }, 	4	, 2	, 0	},	/* MD, kW for TZ3 -export */		
/* 67 */ {	{   1,   0,   2,   6,   3, 255 }, 	4	, 5	, 0	},	/* MD kW Date Time -export */		
/* 68 */ {	{   1,   0,   2,   6,   4, 255 }, 	4	, 2	, 0	},	/* MD, kW for TZ4 -export */		
/* 69 */ {	{   1,   0,   2,   6,   4, 255 }, 	4	, 5	, 0	},	/* MD kW Date Time -export */		
/* 70 */ {	{   1,   0,   2,   6,   5, 255 }, 	4	, 2	, 0	},	/* MD, kW for TZ5 -export */		
/* 71 */ {	{   1,   0,   2,   6,   5, 255 }, 	4	, 5	, 0	},	/* MD kW Date Time -export */		
/* 72 */ {	{   1,   0,   10,   6,   0, 255 }, 	4	, 2	, 0	},	/* MD kVA -export */		
/* 73 */ {	{   1,   0,   10,   6,   0, 255 }, 	4	, 5	, 0	},	/* MD kVA Date Time -export */		
/* 74 */ {	{   1,   0,   10,   6,   1, 255 }, 	4	, 2	, 0	},	/* MD, kVA for TZ1 -export */		
/* 75 */ {	{   1,   0,   10,   6,   1, 255 }, 	4	, 5	, 0	},	/* MD kVA Date Time -export */		
/* 76 */ {	{   1,   0,   10,   6,   2, 255 }, 	4	, 2	, 0	},	/* MD, kVA for TZ2 -export */		
/* 77 */ {	{   1,   0,   10,   6,   2, 255 }, 	4	, 5	, 0	},	/* MD kVA Date Time -export */		
/* 78 */ {	{   1,   0,   10,   6,   3, 255 }, 	4	, 2	, 0	},	/* MD, kVA for TZ3 -export */		
/* 79 */ {	{   1,   0,   10,   6,   3, 255 }, 	4	, 5	, 0	},	/* MD kVA Date Time -export */		
/* 80 */ {	{   1,   0,   10,   6,   4, 255 }, 	4	, 2	, 0	},	/* MD, kVA for TZ4 -export */		
/* 81 */ {	{   1,   0,   10,   6,   4, 255 }, 	4	, 5	, 0	},	/* MD kVA Date Time -export */		
/* 82 */ {	{   1,   0,   10,   6,   5, 255 }, 	4	, 2	, 0	},	/* MD, kVA for TZ5 -export */		
/* 83 */ {	{   1,   0,   10,   6,   5, 255 }, 	4	, 5	, 0	},	/* MD kVA Date Time -export */		
/* 83 */ {	{   1,   0,   2,   8,   9, 255 },   3	, 2	, 0	},	/* kwh_nonSolar_export */
/* 84 */ {	{   1,   0,   1,   8,   9, 255 },   3	, 2	, 0	},	/* kwh_Solar_import */
/* 85 */ {	{   1,   0,   2,   8,  10, 255 },   3	, 2	, 0	},	/* kwh_Solar_export */
#endif
#ifdef SPLIT_ENERGIES_REQUIRED
/* 84,46,41 */ {	{   0,   0,  96,  50,  15, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy � kWh Lag - TZ 9 used */
/* 85,47,42 */ {	{   0,   0,  96,  50,  16, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy � kWh Lead - TZ 10 used*/
/* 86,48,43 */ {	{   0,   0,  96,  50,  17, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy � kVAh Lag - TZ 9 used*/
/* 87,49,44 */ {	{   0,   0,  96,  50,  18, 255 }, 	3	, 2	, 0	},	/* Cumulative Energy � kVAh Lead - TZ 10 used*/
#endif
};
const Unsigned16 	g_Class07_BillingCaptureObjectsLength = sizeof(g_Class07_BillingCaptureObjects) / sizeof(class07_capture_object_t);
/* Capture object list Scaler */
const class07_capture_object_t g_Class07_BillingCaptureObjects_Scaler[] =
{
		/*----------------------------------------------------------------------------*/
	/*          OBIS Code (A, B, C, D, E, F)    | Class ID	| Attr ID	| Data ID */
	/*----------------------------------------------------------------------------*/
	///*00 */ {	{   0,   0,   0,   1,   2, 255 }, 8			, 2			, 0		},	/* Billing date */
	/* 01 */ {	{   1,   0,  13,   0,   0, 255 }, 3			, 3			, 0		},	/* Average Power Factor for billing period */
	/* 02 */ {	{   1,   0,   1,   8,   0, 255 }, 3			, 3			, 0		},	/* Cumulative Energy � kWh */
	/* 03 */ {	{   1,   0,   1,   8,   1, 255 }, 3			, 3			, 0		},	/* Cumulative Energy kWh TZ1 */
	/* 04 */ {	{   1,   0,   1,   8,   2, 255 }, 3			, 3			, 0		},	/* Cumulative Energy kWh TZ2 */
	/* 05 */ {	{   1,   0,   1,   8,   3, 255 }, 3			, 3			, 0		},	/* Cumulative Energy kWh TZ3 */
	/* 06 */ {	{   1,   0,   1,   8,   4, 255 }, 3			, 3			, 0		},	/* Cumulative Energy kWh TZ4 */
	/* 07 */ {	{   1,   0,   1,   8,   5, 255 }, 3			, 3			, 0		},	/* Cumulative Energy kWh TZ5 */
#if (defined(TARIFFS) && TARIFFS == 8)
	/* 05 */ {	{   1,   0,   1,   8,   6, 255 }, 3			, 3			, 0		},	/* Cumulative Energy kWh TZ6 */
	/* 06 */ {	{   1,   0,   1,   8,   7, 255 }, 3			, 3			, 0		},	/* Cumulative Energy kWh TZ7 */
	/* 07 */ {	{   1,   0,   1,   8,   8, 255 }, 3			, 3			, 0		},	/* Cumulative Energy kWh TZ8 */
#endif
	/* 08 */ {	{   1,   0,   5,   8,   0, 255 }, 3			, 3			, 0		},	/* Cumulative Energy � kvarh (Lag)*/
	/* 09 */ {	{   1,   0,   8,   8,   0, 255 }, 3			, 3			, 0		},	/* Cumulative Energy � kvarh (Lead) */
	/* 10 */ {	{   1,   0,   9,   8,   0, 255 }, 3			, 3			, 0		},	/* Cumulative Energy � kVAh */
	/* 11 */ {	{   1,   0,   9,   8,   1, 255 }, 3			, 3			, 0		},	/* Cumulative Energy kVAh TZ1 */
	/* 12 */ {	{   1,   0,   9,   8,   2, 255 }, 3			, 3			, 0		},	/* Cumulative Energy kVAh TZ2 */
	/* 13 */ {	{   1,   0,   9,   8,   3, 255 }, 3			, 3			, 0		},	/* Cumulative Energy kVAh TZ3 */
	/* 14 */ {	{   1,   0,   9,   8,   4, 255 }, 3			, 3			, 0		},	/* Cumulative Energy kVAh TZ4 */
	/* 15 */ {	{   1,   0,   9,   8,   5, 255 }, 3			, 3			, 0		},	/* Cumulative Energy kVAh TZ5 */
#if (defined(TARIFFS) && TARIFFS == 8)
	/* 13 */ {	{   1,   0,   9,   8,   6, 255 }, 3			, 3			, 0		},	/* Cumulative Energy kVAh TZ6 */
	/* 14 */ {	{   1,   0,   9,   8,   7, 255 }, 3			, 3			, 0		},	/* Cumulative Energy kVAh TZ7 */
	/* 15 */ {	{   1,   0,   9,   8,   8, 255 }, 3			, 3			, 0		},	/* Cumulative Energy kVAh TZ8 */
#endif
	/* 16 */ {	{   1,   0,   1,   6,   0, 255 }, 4			, 3			, 0		},	/* MD kW */
	/* 18 */ {	{   1,   0,   1,   6,   1, 255 }, 4			, 3			, 0		},	/* MD, kW for TZ1 */
	/* 20 */ {	{   1,   0,   1,   6,   2, 255 }, 4			, 3			, 0		},	/* MD, kW for TZ2*/
	/* 22 */ {	{   1,   0,   1,   6,   3, 255 }, 4			, 3			, 0		},	/* MD, kW for TZ3 */
	/* 24 */ {	{   1,   0,   1,   6,   4, 255 }, 4			, 3			, 0		},	/* MD, kW for TZ4 */
	/* 26 */ {	{   1,   0,   1,   6,   5, 255 }, 4			, 3			, 0		},	/* MD, kW for TZ5 */
#if (defined(TARIFFS) && TARIFFS == 8)
	/* 22 */ {	{   1,   0,   1,   6,   6, 255 }, 4			, 3			, 0		},	/* MD, kW for TZ6 */
	/* 24 */ {	{   1,   0,   1,   6,   7, 255 }, 4			, 3			, 0		},	/* MD, kW for TZ7 */
	/* 26 */ {	{   1,   0,   1,   6,   8, 255 }, 4			, 3			, 0		},	/* MD, kW for TZ8 */
#endif
	/* 28 */ {	{   1,   0,   9,   6,   0, 255 }, 4			, 3			, 0		},	/* MD kVA */
	/* 30 */ {	{   1,   0,   9,   6,   1, 255 }, 4			, 3			, 0		},	/* MD, kVA for TZ1 */
	/* 32 */ {	{   1,   0,   9,   6,   2, 255 }, 4			, 3			, 0		},	/* MD, kVA for TZ2 */
	/* 34 */ {	{   1,   0,   9,   6,   3, 255 }, 4			, 3			, 0		},	/* MD, kVA for TZ3*/
	/* 36 */ {	{   1,   0,   9,   6,   4, 255 }, 4			, 3			, 0		},	/* MD, kVA for TZ4 */
	/* 38 */ {	{   1,   0,   9,   6,   5, 255 }, 4			, 3			, 0		},	/* MD, kVA for TZ5 */
#if (defined(TARIFFS) && TARIFFS == 8)
	/* 34 */ {	{   1,   0,   9,   6,   6, 255 }, 4			, 3			, 0		},	/* MD, kVA for TZ6*/
	/* 36 */ {	{   1,   0,   9,   6,   7, 255 }, 4			, 3			, 0		},	/* MD, kVA for TZ7 */
	/* 38 */ {	{   1,   0,   9,   6,   8, 255 }, 4			, 3			, 0		},	/* MD, kVA for TZ8 */
#endif
	/* 40 */ {	{   0,   0,  94,  91,  13, 255 }, 3			, 3			, 0		},	/* Billing power ON duration in minutes (During Billing period) */

#ifdef BILLING_HAS_TAMPER_COUNT
/* 40 */ {	{   0,   0,  94,  91,   0, 255 }, 	3	, 3	, 0	},	/* Cumulative Tamper Count */
#endif

#ifdef TOD_HAS_KVARH_LAG
	/* 41 */ {	{   1,   0,   5,   8,   1, 255 }, 3			, 3			, 0		},	/* Cumulative Energy kVArhlag TZ1 */
	/* 42 */ {	{   1,   0,   5,   8,   2, 255 }, 3			, 3			, 0		},	/* Cumulative Energy kVArhlag TZ2 */
	/* 43 */ {	{   1,   0,   5,   8,   3, 255 }, 3			, 3			, 0		},	/* Cumulative Energy kVArhlag TZ3 */
	/* 44 */ {	{   1,   0,   5,   8,   4, 255 }, 3			, 3			, 0		},	/* Cumulative Energy kVArhlag TZ4 */
	/* 45 */ {	{   1,   0,   5,   8,   5, 255 }, 3			, 3			, 0		},	/* Cumulative Energy kVArhlag TZ5 */
	#if (defined(TARIFFS) && TARIFFS == 8)
	/* 43 */ {	{   1,   0,   5,   8,   6, 255 }, 3			, 3			, 0		},	/* Cumulative Energy kVArhlag TZ6 */
	/* 44 */ {	{   1,   0,   5,   8,   7, 255 }, 3			, 3			, 0		},	/* Cumulative Energy kVArhlag TZ7 */
	/* 45 */ {	{   1,   0,   5,   8,   8, 255 }, 3			, 3			, 0		},	/* Cumulative Energy kVArhlag TZ8 */
	#endif
#endif	
#ifdef BIDIR_METER
	/* 46 */ {	{   1,   0,   2,   8,   0, 255 }, 3			, 3			, 0		},	/* Cumulative Energy � kWh -export */
	/* 47 */ {	{   1,   0,   2,   8,   1, 255 }, 3			, 3			, 0		},	/* Cumulative Energy kWh TZ1 -export */
	/* 48 */ {	{   1,   0,   2,   8,   2, 255 }, 3			, 3			, 0		},	/* Cumulative Energy kWh TZ2 -export */
	/* 49 */ {	{   1,   0,   2,   8,   3, 255 }, 3			, 3			, 0		},	/* Cumulative Energy kWh TZ3 -export */
	/* 50 */ {	{   1,   0,   2,   8,   4, 255 }, 3			, 3			, 0		},	/* Cumulative Energy kWh TZ4 -export */
	/* 51 */ {	{   1,   0,   2,   8,   5, 255 }, 3			, 3			, 0		},	/* Cumulative Energy kWh TZ5 -export */
	/* 52 */ {	{   1,   0,   5,   9,   0, 255 }, 3			, 3			, 0		},	/* Cumulative Energy � kvarh (Lag)-export */
	/* 53 */ {	{   1,   0,   8,   9,   0, 255 }, 3			, 3			, 0		},	/* Cumulative Energy � kvarh (Lead)-export  */
	/* 54 */ {	{   1,   0,   10,   8,   0, 255 }, 3			, 3			, 0		},	/* Cumulative Energy � kVAh -export */
	/* 55 */ {	{   1,   0,   10,   8,   1, 255 }, 3			, 3			, 0		},	/* Cumulative Energy kVAh TZ1 -export */
	/* 56 */ {	{   1,   0,   10,   8,   2, 255 }, 3			, 3			, 0		},	/* Cumulative Energy kVAh TZ2 -export */
	/* 57 */ {	{   1,   0,   10,   8,   3, 255 }, 3			, 3			, 0		},	/* Cumulative Energy kVAh TZ3 -export */
	/* 58 */ {	{   1,   0,   10,   8,   4, 255 }, 3			, 3			, 0		},	/* Cumulative Energy kVAh TZ4 -export */
	/* 59 */ {	{   1,   0,   10,   8,   5, 255 }, 3			, 3			, 0		},	/* Cumulative Energy kVAh TZ5 -export */
	/* 60 */ {	{   1,   0,   2,   6,   0, 255 }, 4			, 3			, 0		},	/* MD kW */
	/* 62 */ {	{   1,   0,   2,   6,   1, 255 }, 4			, 3			, 0		},	/* MD, kW for TZ1 */
	/* 64 */ {	{   1,   0,   2,   6,   2, 255 }, 4			, 3			, 0		},	/* MD, kW for TZ2*/
	/* 66 */ {	{   1,   0,   2,   6,   3, 255 }, 4			, 3			, 0		},	/* MD, kW for TZ3 */
	/* 68 */ {	{   1,   0,   2,   6,   4, 255 }, 4			, 3			, 0		},	/* MD, kW for TZ4 */
	/* 70 */ {	{   1,   0,   2,   6,   5, 255 }, 4			, 3			, 0		},	/* MD, kW for TZ5 */
	/* 72 */ {	{   1,   0,   10,   6,   0, 255 }, 4			, 3			, 0		},	/* MD kVA */
	/* 74 */ {	{   1,   0,   10,   6,   1, 255 }, 4			, 3			, 0		},	/* MD, kVA for TZ1 */
	/* 76 */ {	{   1,   0,   10,   6,   2, 255 }, 4			, 3			, 0		},	/* MD, kVA for TZ2 */
	/* 78 */ {	{   1,   0,   10,   6,   3, 255 }, 4			, 3			, 0		},	/* MD, kVA for TZ3*/
	/* 80 */ {	{   1,   0,   10,   6,   4, 255 }, 4			, 3			, 0		},	/* MD, kVA for TZ4 */
	/* 82 */ {	{   1,   0,   10,   6,   5, 255 }, 4			, 3			, 0		},	/* MD, kVA for TZ5 */
	
	/* 83 */ {	{   1,   0,   2,   8,   9, 255 }, 3			, 3			, 0		},	/* kwh_nonSolar_export */
	/* 84 */ {	{   1,   0,   1,   8,   9, 255 }, 3			, 3			, 0		},	/* kwh_Solar_import */
	/* 85 */ {	{   1,   0,   2,   8,  10, 255 }, 3			, 3			, 0		},	/* kwh_Solar_export */
	
#endif	
#ifdef SPLIT_ENERGIES_REQUIRED
/* 86 */ {	{   1,   0,   1,   8,   9, 255 }, 	3	, 3	, 0	},	/* Cumulative Energy � kWh Lag - TZ 9 used */
/* 87 */ {	{   1,   0,   1,   8,   10, 255 }, 	3	, 3	, 0	},	/* Cumulative Energy � kWh Lead - TZ 10 used*/
/* 88 */ {	{   1,   0,   9,   8,   9, 255 }, 	3	, 3	, 0	},	/* Cumulative Energy � kVAh Lag - TZ 9 used*/
/* 89 */ {	{   1,   0,   9,   8,   10, 255 }, 	3	, 3	, 0	},	/* Cumulative Energy � kVAh Lead - TZ 10 used*/
#endif
};
const Unsigned16 	g_Class07_BillingCaptureObjectsLength_Scaler = sizeof(g_Class07_BillingCaptureObjects_Scaler) / sizeof(class07_capture_object_t);

Unsigned16				g_Class07_Billing_CurrentEntry = 1;
Unsigned32				g_Class07_Billing_EntriesInUse = 1;
//Unsigned32				g_Class07_Billing_EntriesInUse_New = 0;
/*
 * Number of entries is defined in r_dlms_data.c,
 * the value is specified by macro CLASS07_BILLING_MAX_ENTRY
 */
const Unsigned32	g_Class07_Billing_MaxEntries = 13;//CLASS07_BILLING_MAX_ENTRY;

buffer_t g_class07_Billing_Buf = {
	&g_Class07_BillingBuffer,									/* Buffer of value                                     */
	NULL,												/* Pointer to access method option                     */
	&g_Class07_Billing_CurrentEntry,								/* Pointer to index of current entries in buffer       */
	0,												/* Number of entries per block to transfer             */
	86,												/* Length of encode 1 entry after filter               */
	0,												/* Number of remain entries in run-time                */
	0,												/* First entry to retrieve in run-time                 */
	0,												/* Last entry to retrieve in run-time                  */
	0,												/* Index of first value to retrieve in run-time        */
	0,												/* Index of last value to retrieve  in run-time        */
};

/***************************************************************
 * Billing Scaler Profile
***************************************************************/
buffer_t g_class07_BillingScaler_Buf = {
	(scaler_unit_t *)g_Class07_BillingScalerBuffer,									/* Buffer of value                                     */
	NULL,												/* Pointer to access method option                     */
	(Unsigned16 *)&g_Class07_FixCurrentEntry,									/* Pointer to index of current entries in buffer       */
	0,												/* Number of entries per block to transfer             */
	92,												/* Length of encode 1 entry after filter               */
	0,												/* Number of remain entries in run-time                */
	0,												/* First entry to retrieve in run-time                 */
	0,												/* Last entry to retrieve in run-time                  */
	0,												/* Index of first value to retrieve in run-time        */
	0,												/* Index of last value to retrieve  in run-time        */
};

/***************************************************************
 * Event 0 Log Scaler Profile
***************************************************************/
buffer_t g_class07_EventScaler_Buf = {
	(scaler_unit_t *)g_Class07_Event0ScalerBuffer,									/* Buffer of value                                     */
	NULL,												/* Pointer to access method option                     */
	(Unsigned16 *)&g_Class07_FixCurrentEntry,									/* Pointer to index of current entries in buffer       */
	0,												/* Number of entries per block to transfer             */
	38,												/* Length of encode 1 entry after filter               */
	0,												/* Number of remain entries in run-time                */
	0,												/* First entry to retrieve in run-time                 */
	0,												/* Last entry to retrieve in run-time                  */
	0,												/* Index of first value to retrieve in run-time        */
	0,												/* Index of last value to retrieve  in run-time        */
};

Unsigned16 g_Class07_Event_CurrentEntry[6];
Unsigned32 g_Class07_Event_EntriesInUse[6];
/***************************************************************
 * Event 0 Log Profile
 ***************************************************************/
/* Capture object list */
const class07_capture_object_t g_Class07_Event0CaptureObjects[] =
{
	/*----------------------------------------------------------------------------*/
	/*          OBIS Code (A, B, C, D, E, F)    | Class ID	| Attr ID	| Data ID */
	/*----------------------------------------------------------------------------*/
	/* 00 */ {	{   0,   0,   1,   0,   0, 255 }, 	8			, 2			, 0		},	/* Clock */
	/* 01 */ {	{   0,   0,  96,  11,   0, 255 }, 	1			, 2			, 0		},	/* Event code  */
	/* 02 */ {	{   1,   0,  31,   7,   0, 255 }, 	3			, 2			, 0		},	/* IR current  */
	/* 03 */ {	{   1,   0,  51,   7,   0, 255 }, 	3			, 2			, 0		},	/* IY current  */
	/* 04 */ {	{   1,   0,  71,   7,   0, 255 }, 	3			, 2			, 0		},	/* IB current  */
	/* 05 */ {	{   1,   0,  32,   7,   0, 255 }, 	3			, 2			, 0		},	/* VR Voltage  */
	/* 06 */ {	{   1,   0,  52,   7,   0, 255 }, 	3			, 2			, 0		},	/* VY Voltage  */
	/* 07 */ {	{   1,   0,  72,   7,   0, 255 }, 	3			, 2			, 0		},	/* VB Voltage  */
	/* 08 */ {	{   1,   0,  33,   7,   0, 255 }, 	3			, 2			, 0		},	/* Signed Power Factor R-phase */
	/* 09 */ {	{   1,   0,  53,   7,   0, 255 }, 	3			, 2			, 0		},	/* Signed Power Factor Y-phase*/
	/* 10 */ {	{   1,   0,  73,   7,   0, 255 }, 	3			, 2			, 0		},	/* Signed Power Factor B-phase*/
	/* 11 */ {	{   1,   0,   1,   8,   0, 255 }, 	3			, 2			, 0		},	/* Cumulative Energy - kWh */
#ifdef EVENTDATA_RECORDS_KVAH
	/* 12 */ {	{   1,   0,   9,   8,   0, 255 }, 	3			, 2			, 0		},	/* Cumulative Energy - kVAh */
#endif
};
const class07_capture_object_t g_Class07_Event1CaptureObjects[] =
{
	/*----------------------------------------------------------------------------*/
	/*          OBIS Code (A, B, C, D, E, F)    | Class ID	| Attr ID	| Data ID */
	/*----------------------------------------------------------------------------*/
	/* 00 */ {	{   0,   0,   1,   0,   0, 255 }, 8			, 2			, 0		},	/* Clock */
	/* 01 */ {	{   0,   0,  96,  11,   1, 255 }, 1			, 2			, 0		},	/* Event code  */
	/* 02 */ {	{   1,   0,  31,   7,   0, 255 }, 3			, 2			, 0		},	/* IR current  */
	/* 03 */ {	{   1,   0,  51,   7,   0, 255 }, 3			, 2			, 0		},	/* IY current  */
	/* 04 */ {	{   1,   0,  71,   7,   0, 255 }, 3			, 2			, 0		},	/* IB current  */
	/* 05 */ {	{   1,   0,  32,   7,   0, 255 }, 3			, 2			, 0		},	/* VR Voltage  */
	/* 06 */ {	{   1,   0,  52,   7,   0, 255 }, 3			, 2			, 0		},	/* VY Voltage  */
	/* 07 */ {	{   1,   0,  72,   7,   0, 255 }, 3			, 2			, 0		},	/* VB Voltage  */
	/* 08 */ {	{   1,   0,  33,   7,   0, 255 }, 3			, 2			, 0		},	/* Signed Power Factor R-phase */
	/* 09 */ {	{   1,   0,  53,   7,   0, 255 }, 3			, 2			, 0		},	/* Signed Power Factor Y-phase*/
	/* 10 */ {	{   1,   0,  73,   7,   0, 255 }, 3			, 2			, 0		},	/* Signed Power Factor B-phase*/
	/* 11 */ {	{   1,   0,   1,   8,   0, 255 }, 3			, 2			, 0		},	/* Cumulative Energy - kWh */
#ifdef EVENTDATA_RECORDS_KVAH
	/* 12 */ {	{   1,   0,   9,   8,   0, 255 }, 	3			, 2			, 0		},	/* Cumulative Energy - kVAh */
#endif
};
const class07_capture_object_t g_Class07_Event2CaptureObjects[] =
{
	/*----------------------------------------------------------------------------*/
	/*          OBIS Code (A, B, C, D, E, F)    | Class ID	| Attr ID	| Data ID */
	/*----------------------------------------------------------------------------*/
	/* 00 */ {	{   0,   0,   1,   0,   0, 255 }, 8			, 2			, 0		},	/* Clock */
	/* 01 */ {	{   0,   0,  96,  11,   2, 255 }, 1			, 2			, 0		},	/* Event code  */
	/* 02 */ {	{   1,   0,  31,   7,   0, 255 }, 3			, 2			, 0		},	/* IR current  */
	/* 03 */ {	{   1,   0,  51,   7,   0, 255 }, 3			, 2			, 0		},	/* IY current  */
	/* 04 */ {	{   1,   0,  71,   7,   0, 255 }, 3			, 2			, 0		},	/* IB current  */
	/* 05 */ {	{   1,   0,  32,   7,   0, 255 }, 3			, 2			, 0		},	/* VR Voltage  */
	/* 06 */ {	{   1,   0,  52,   7,   0, 255 }, 3			, 2			, 0		},	/* VY Voltage  */
	/* 07 */ {	{   1,   0,  72,   7,   0, 255 }, 3			, 2			, 0		},	/* VB Voltage  */
	/* 08 */ {	{   1,   0,  33,   7,   0, 255 }, 3			, 2			, 0		},	/* Signed Power Factor R-phase */
	/* 09 */ {	{   1,   0,  53,   7,   0, 255 }, 3			, 2			, 0		},	/* Signed Power Factor Y-phase*/
	/* 10 */ {	{   1,   0,  73,   7,   0, 255 }, 3			, 2			, 0		},	/* Signed Power Factor B-phase*/
	/* 11 */ {	{   1,   0,   1,   8,   0, 255 }, 3			, 2			, 0		},	/* Cumulative Energy - kWh */
#ifdef EVENTDATA_RECORDS_KVAH
	/* 12 */ {	{   1,   0,   9,   8,   0, 255 }, 	3			, 2			, 0		},	/* Cumulative Energy - kVAh */
#endif
};
const class07_capture_object_t g_Class07_Event3CaptureObjects[] =
{
	/*----------------------------------------------------------------------------*/
	/*          OBIS Code (A, B, C, D, E, F)    | Class ID	| Attr ID	| Data ID */
	/*----------------------------------------------------------------------------*/
	/* 00 */ {	{   0,   0,   1,   0,   0, 255 }, 8			, 2			, 0		},	/* Clock */
	/* 01 */ {	{   0,   0,  96,  11,   3, 255 }, 1			, 2			, 0		},	/* Event code  */
	/* 02 */ {	{   1,   0,  31,   7,   0, 255 }, 3			, 2			, 0		},	/* IR current  */
	/* 03 */ {	{   1,   0,  51,   7,   0, 255 }, 3			, 2			, 0		},	/* IY current  */
	/* 04 */ {	{   1,   0,  71,   7,   0, 255 }, 3			, 2			, 0		},	/* IB current  */
	/* 05 */ {	{   1,   0,  32,   7,   0, 255 }, 3			, 2			, 0		},	/* VR Voltage  */
	/* 06 */ {	{   1,   0,  52,   7,   0, 255 }, 3			, 2			, 0		},	/* VY Voltage  */
	/* 07 */ {	{   1,   0,  72,   7,   0, 255 }, 3			, 2			, 0		},	/* VB Voltage  */
	/* 08 */ {	{   1,   0,  33,   7,   0, 255 }, 3			, 2			, 0		},	/* Signed Power Factor R-phase */
	/* 09 */ {	{   1,   0,  53,   7,   0, 255 }, 3			, 2			, 0		},	/* Signed Power Factor Y-phase*/
	/* 10 */ {	{   1,   0,  73,   7,   0, 255 }, 3			, 2			, 0		},	/* Signed Power Factor B-phase*/
	/* 11 */ {	{   1,   0,   1,   8,   0, 255 }, 3			, 2			, 0		},	/* Cumulative Energy - kWh */
#ifdef EVENTDATA_RECORDS_KVAH
	/* 12 */ {	{   1,   0,   9,   8,   0, 255 }, 	3			, 2			, 0		},	/* Cumulative Energy - kVAh */
#endif
};
const class07_capture_object_t g_Class07_Event4CaptureObjects[] =
{
	/*----------------------------------------------------------------------------*/
	/*          OBIS Code (A, B, C, D, E, F)    | Class ID	| Attr ID	| Data ID */
	/*----------------------------------------------------------------------------*/
	/* 00 */ {	{   0,   0,   1,   0,   0, 255 }, 8			, 2			, 0		},	/* Clock */
	/* 01 */ {	{   0,   0,  96,  11,   4, 255 }, 1			, 2			, 0		},	/* Event code  */
	/* 02 */ {	{   1,   0,  31,   7,   0, 255 }, 3			, 2			, 0		},	/* IR current  */
	/* 03 */ {	{   1,   0,  51,   7,   0, 255 }, 3			, 2			, 0		},	/* IY current  */
	/* 04 */ {	{   1,   0,  71,   7,   0, 255 }, 3			, 2			, 0		},	/* IB current  */
	/* 05 */ {	{   1,   0,  32,   7,   0, 255 }, 3			, 2			, 0		},	/* VR Voltage  */
	/* 06 */ {	{   1,   0,  52,   7,   0, 255 }, 3			, 2			, 0		},	/* VY Voltage  */
	/* 07 */ {	{   1,   0,  72,   7,   0, 255 }, 3			, 2			, 0		},	/* VB Voltage  */
	/* 08 */ {	{   1,   0,  33,   7,   0, 255 }, 3			, 2			, 0		},	/* Signed Power Factor R-phase */
	/* 09 */ {	{   1,   0,  53,   7,   0, 255 }, 3			, 2			, 0		},	/* Signed Power Factor Y-phase*/
	/* 10 */ {	{   1,   0,  73,   7,   0, 255 }, 3			, 2			, 0		},	/* Signed Power Factor B-phase*/
	/* 11 */ {	{   1,   0,   1,   8,   0, 255 }, 3			, 2			, 0		},	/* Cumulative Energy - kWh */
#ifdef EVENTDATA_RECORDS_KVAH
	/* 12 */ {	{   1,   0,   9,   8,   0, 255 }, 	3			, 2			, 0		},	/* Cumulative Energy - kVAh */
#endif
};
const class07_capture_object_t g_Class07_Event5CaptureObjects[] =
{
	/*----------------------------------------------------------------------------*/
	/*          OBIS Code (A, B, C, D, E, F)    | Class ID	| Attr ID	| Data ID */
	/*----------------------------------------------------------------------------*/
	/* 00 */ {	{   0,   0,   1,   0,   0, 255 }, 8			, 2			, 0		},	/* Clock */
	/* 01 */ {	{   0,   0,  96,  11,   5, 255 }, 1			, 2			, 0		},	/* Event code  */
	/* 02 */ {	{   1,   0,  31,   7,   0, 255 }, 3			, 2			, 0		},	/* IR current  */
	/* 03 */ {	{   1,   0,  51,   7,   0, 255 }, 3			, 2			, 0		},	/* IY current  */
	/* 04 */ {	{   1,   0,  71,   7,   0, 255 }, 3			, 2			, 0		},	/* IB current  */
	/* 05 */ {	{   1,   0,  32,   7,   0, 255 }, 3			, 2			, 0		},	/* VR Voltage  */
	/* 06 */ {	{   1,   0,  52,   7,   0, 255 }, 3			, 2			, 0		},	/* VY Voltage  */
	/* 07 */ {	{   1,   0,  72,   7,   0, 255 }, 3			, 2			, 0		},	/* VB Voltage  */
	/* 08 */ {	{   1,   0,  33,   7,   0, 255 }, 3			, 2			, 0		},	/* Signed Power Factor R-phase */
	/* 09 */ {	{   1,   0,  53,   7,   0, 255 }, 3			, 2			, 0		},	/* Signed Power Factor Y-phase*/
	/* 10 */ {	{   1,   0,  73,   7,   0, 255 }, 3			, 2			, 0		},	/* Signed Power Factor B-phase*/
	/* 11 */ {	{   1,   0,   1,   8,   0, 255 }, 3			, 2			, 0		},	/* Cumulative Energy - kWh */
#ifdef EVENTDATA_RECORDS_KVAH
	/* 12 */ {	{   1,   0,   9,   8,   0, 255 }, 	3			, 2			, 0		},	/* Cumulative Energy - kVAh */
#endif
};
const Unsigned16 	g_Class07_Event0CaptureObjectsLength = sizeof(g_Class07_Event0CaptureObjects) / sizeof(class07_capture_object_t);
/* Capture object list */
const class07_capture_object_t g_Class07_Event0CaptureObjects_Scaler[] =
{
	/*----------------------------------------------------------------------------*/
	/*          OBIS Code (A, B, C, D, E, F)    | Class ID	| Attr ID	| Data ID */
	/*----------------------------------------------------------------------------*/
	///* 00 */ {	{   0,   0,   1,   0,   0, 255 }, 8			, 2			, 0		},	/* Clock */
	///* 01 */ {	{   0,   0,  96,  11,   0, 255 }, 1			, 2			, 0		},	/* Event code  */
	
	/* 02 */ {	{   1,   0,  31,   7,   0, 255 }, 3			, 2			, 0		},	/* IR current  */
	/* 03 */ {	{   1,   0,  51,   7,   0, 255 }, 3			, 2			, 0		},	/* IY current  */
	/* 04 */ {	{   1,   0,  71,   7,   0, 255 }, 3			, 2			, 0		},	/* IB current  */
	
	/* 05 */ {	{   1,   0,  32,   7,   0, 255 }, 3			, 2			, 0		},	/* VR Voltage  */
	/* 06 */ {	{   1,   0,  52,   7,   0, 255 }, 3			, 2			, 0		},	/* VY Voltage  */
	/* 07 */ {	{   1,   0,  72,   7,   0, 255 }, 3			, 2			, 0		},	/* VB Voltage  */
	
	/* 08 */ {	{   1,   0,  33,   7,   0, 255 }, 3			, 2			, 0		},	/* Signed Power Factor R-phase */
	/* 09 */ {	{   1,   0,  53,   7,   0, 255 }, 3			, 2			, 0		},	/* Signed Power Factor Y-phase*/
	/* 10 */ {	{   1,   0,  73,   7,   0, 255 }, 3			, 2			, 0		},	/* Signed Power Factor B-phase*/
	
	/* 11 */ {	{   1,   0,   1,   8,   0, 255 }, 3			, 2			, 0		},	/* Cumulative Energy - kWh */
#ifdef EVENTDATA_RECORDS_KVAH
	/* 12 */ {	{   1,   0,   9,   8,   0, 255 }, 	3			, 2			, 0		},	/* Cumulative Energy - kVAh */
#endif
};
const Unsigned16 	g_Class07_Event0CaptureObjectsLength_Scaler = sizeof(g_Class07_Event0CaptureObjects_Scaler) / sizeof(class07_capture_object_t);
/*
 * Number of entries is defined as in r_dlms_data.c,
 * the value is specified by macro CLASS07_EVENT_MAX_ENTRY
 */
// const Unsigned32	g_Class07_Event0_MaxEntries = CLASS07_EVENT_MAX_ENTRY;
// Check again CLASS07_EVENT_MAX_PROFILE
#ifdef KERALA_UNIDIR
Unsigned32	g_Class07_Event_MaxEntries[TOT_EVENT_TYPE] = {100,200,100,20,100,1,0,};
#else
Unsigned32	g_Class07_Event_MaxEntries[TOT_EVENT_TYPE] = {150,150,50,20,40,1,0,};
#endif
buffer_t g_class07_Event0_Buf = {
	//g_Class07_EventBuffer[0],									/* Buffer of value                                     */
	&g_Class07_EventBuffer,										/* Buffer of value*/
	NULL,												/* Pointer to access method option                     */
	&g_Class07_Event_CurrentEntry[0],								/* Pointer to index of current entries in buffer       */
	0,												/* Number of entries per block to transfer             */
	39,												/* Length of encode 1 entry after filter               */
	0,												/* Number of remain entries in run-time                */
	0,												/* First entry to retrieve in run-time                 */
	0,												/* Last entry to retrieve in run-time                  */
	0,												/* Index of first value to retrieve in run-time        */
	0,												/* Index of last value to retrieve  in run-time        */
};

/***************************************************************
 * Event 1 Log Profile
 ***************************************************************/
/* Capture object list - reuse from Event 0 */

/*
 * Number of entries is defined as in r_dlms_data.c,
 * the value is specified by macro CLASS07_EVENT_MAX_ENTRY
 */
//const Unsigned32	g_Class07_Event1_MaxEntries = CLASS07_EVENT_MAX_ENTRY;

buffer_t g_class07_Event1_Buf = {
	//g_Class07_EventBuffer[1],									/* Buffer of value                                     */
	&g_Class07_EventBuffer,										/* Buffer of value*/
	NULL,												/* Pointer to access method option                     */
	&g_Class07_Event_CurrentEntry[1],								/* Pointer to index of current entries in buffer       */
	0,												/* Number of entries per block to transfer             */
	39,												/* Length of encode 1 entry after filter               */
	0,												/* Number of remain entries in run-time                */
	0,												/* First entry to retrieve in run-time                 */
	0,												/* Last entry to retrieve in run-time                  */
	0,												/* Index of first value to retrieve in run-time        */
	0,												/* Index of last value to retrieve  in run-time        */
};

/***************************************************************
 * Event 2 Log Profile
 ***************************************************************/
/* Capture object list - reuse from Event 0 */

/*
 * Number of entries is defined as in r_dlms_data.c,
 * the value is specified by macro CLASS07_EVENT_MAX_ENTRY
 */
//const Unsigned32	g_Class07_Event2_MaxEntries = CLASS07_EVENT_MAX_ENTRY;

buffer_t g_class07_Event2_Buf = {
	//g_Class07_EventBuffer[2],									/* Buffer of value                                     */
	&g_Class07_EventBuffer,										/* Buffer of value*/
	NULL,												/* Pointer to access method option                     */
	&g_Class07_Event_CurrentEntry[2],								/* Pointer to index of current entries in buffer       */
	0,												/* Number of entries per block to transfer             */
	39,												/* Length of encode 1 entry after filter               */
	0,												/* Number of remain entries in run-time                */
	0,												/* First entry to retrieve in run-time                 */
	0,												/* Last entry to retrieve in run-time                  */
	0,												/* Index of first value to retrieve in run-time        */
	0,												/* Index of last value to retrieve  in run-time        */
};

/***************************************************************
 * Event 3 Log Profile
 ***************************************************************/
/* Capture object list - reuse from Event 0 */

/*
 * Number of entries is defined as in r_dlms_data.c,
 * the value is specified by macro CLASS07_EVENT_MAX_ENTRY
 */
//const Unsigned32	g_Class07_Event3_MaxEntries = CLASS07_EVENT_MAX_ENTRY;

buffer_t g_class07_Event3_Buf = {
	//g_Class07_EventBuffer[3],									/* Buffer of value                                     */
	&g_Class07_EventBuffer,										/* Buffer of value*/
	NULL,												/* Pointer to access method option                     */
	&g_Class07_Event_CurrentEntry[3],								/* Pointer to index of current entries in buffer       */
	0,												/* Number of entries per block to transfer             */
	39,												/* Length of encode 1 entry after filter               */
	0,												/* Number of remain entries in run-time                */
	0,												/* First entry to retrieve in run-time                 */
	0,												/* Last entry to retrieve in run-time                  */
	0,												/* Index of first value to retrieve in run-time        */
	0,												/* Index of last value to retrieve  in run-time        */
};

/***************************************************************
 * Event 4 Log Profile
 ***************************************************************/
/* Capture object list - reuse from Event 0 */

/*
 * Number of entries is defined as in r_dlms_data.c,
 * the value is specified by macro CLASS07_EVENT_MAX_ENTRY
 */
//const Unsigned32	g_Class07_Event4_MaxEntries = CLASS07_EVENT_MAX_ENTRY;

buffer_t g_class07_Event4_Buf = {	
	&g_Class07_EventBuffer,						/* Buffer of value*/
	NULL,								/* Pointer to access method option                     */
	&g_Class07_Event_CurrentEntry[4],				/* Pointer to index of current entries in buffer       */
	0,								/* Number of entries per block to transfer             */
	39,								/* Length of encode 1 entry after filter               */
	0,								/* Number of remain entries in run-time                */
	0,								/* First entry to retrieve in run-time                 */
	0,								/* Last entry to retrieve in run-time                  */
	0,								/* Index of first value to retrieve in run-time        */
	0,								/* Index of last value to retrieve  in run-time        */
};
/***************************************************************
 * Event 5 Log Profile
 ***************************************************************/
/* Capture object list - reuse from Event 0 */

/*
 * Number of entries is defined as in r_dlms_data.c,
 * the value is specified by macro CLASS07_EVENT_MAX_ENTRY
 */
const Unsigned32	g_Class07_Event5_MaxEntries = 1;//CLASS07_EVENT_MAX_ENTRY;

buffer_t g_class07_Event5_Buf = {	
	&g_Class07_EventBuffer,						/* Buffer of value*/
	NULL,								/* Pointer to access method option                     */
	&g_Class07_Event_CurrentEntry[5],				/* Pointer to index of current entries in buffer       */
	0,								/* Number of entries per block to transfer             */
	39,								/* Length of encode 1 entry after filter               */
	0,								/* Number of remain entries in run-time                */
	0,								/* First entry to retrieve in run-time                 */
	0,								/* Last entry to retrieve in run-time                  */
	0,								/* Index of first value to retrieve in run-time        */
	0,								/* Index of last value to retrieve  in run-time        */
};
/***************************************************************
 * Name plate details
 ***************************************************************/

/* Capture object list */
const class07_capture_object_t g_Class07_NamePlateCaptureObjects[] =
{
	/*----------------------------------------------------------------------------*/
	/*          OBIS Code (A, B, C, D, E, F)    | Class ID	| Attr ID	| Data ID */
	/*----------------------------------------------------------------------------*/
	/* 00 */ {	{   0,   0,  96,   1,   0, 255 }, 1			, 2			, 0		},	/* Meter serial number */
	/* 01 */ {	{   0,   0,  96,   1,   1, 255 }, 1			, 2			, 0		},	/* Manufacturer name  */
	/* 02 */ {	{   1,   0,   0,   2,   0, 255 }, 1			, 2			, 0		},	/* Firmware version for meter  */
	/* 03 */ {	{   0,   0,  94,  91,   9, 255 }, 1			, 2			, 0		},	/* Meter type */
	/* 04 */ {	{   0,   0,  94,  91,  11, 255 }, 1			, 2			, 0		},	/* Category */
	/* 05 */ {	{   0,   0,  94,  91,  12, 255 }, 1			, 2			, 0		},	/* Current rating */
	/* 05 */ {	{   1,   0,   0,   4,   2, 255 }, 1			, 2			, 0		},	/* Internal CT ratio */
	/* 05 */ {	{   1,   0,   0,   4,   3, 255 }, 1			, 2			, 0		},	/* Internal VT ratio */
	/* 06 */ {	{   0,   0,  96,   1,   4, 255 }, 1			, 2			, 0		},	/* Meter year of manufacture */
};
const Unsigned16 	g_Class07_NamePlateCaptureObjectsLength = sizeof(g_Class07_NamePlateCaptureObjects) / sizeof(class07_capture_object_t);

buffer_t g_class07_NamePlate_Buf = {
	NULL,									/* Buffer of value                                     */
	NULL,									/* Pointer to access method option                     */
	(Unsigned16 *)&g_Class07_FixCurrentEntry,						/* Pointer to index of current entries in buffer       */
	0,									/* Number of entries per block to transfer             */
	128,									/* Length of encode 1 entry after filter               */
	0,									/* Number of remain entries in run-time                */
	0,									/* First entry to retrieve in run-time                 */
	0,									/* Last entry to retrieve in run-time                  */
	0,									/* Index of first value to retrieve in run-time        */
	0,									/* Index of last value to retrieve  in run-time        */
};

const class07_child_record_t g_ChildTableClass07[] =
{
	// 0 - Instantaneous parameters snapshot  
	{
		{   1,   0,   94,   91,   0, 255 },				// Field 1. Logical name (OBIS code) of the object.  
		(access_right_t *)g_AccessRightTable[0],						// Field 2. Access right definition for 1 object     
		&g_class07_InstSnapshot_Buf,					// Field 3. Buffer                                   
		(class07_capture_object_t *)g_Class07_InstCaptureObjects,					// Field 4. Capture object list                      
		(Unsigned16 *)&g_Class07_InstCaptureObjectsLength,				// Field 5. Capture object list length               
		(Unsigned32 *)&g_Class07_NoCapturePeriod,					// Field 6. Capture period                           
		(sort_method_t *)&g_Class07_SortMethod,						// Field 7. Sort method                              
		(class07_capture_object_t *)&g_Class07_SortObject,						// Field 8. Sort object                              
		(Unsigned32 *)&g_Class07_UniqueEntries,					// Field 9. Entry in use                             
		(Unsigned32 *)&g_Class07_UniqueEntries,					// Field 10. Profile entries                         
	},
	// 1 - Instantaneous Scaler Profile  
	{
		{   1,   0,   94,  91,   3, 255 },				// Field 1. Logical name (OBIS code) of the object.  
		(access_right_t *)g_AccessRightTable[0],						// Field 2. Access right definition for 1 object     
		&g_class07_InstScaler_Buf,					// Field 3. Buffer                                   
		(class07_capture_object_t *)g_Class07_InstCaptureObjects_Scaler,				// Field 4. Capture object list                      
		(Unsigned16 *)&g_Class07_InstCaptureObjectsLength_Scaler,			// Field 5. Capture object list length               
		(Unsigned32 *)&g_Class07_NoCapturePeriod,					// Field 6. Capture period                           
		(sort_method_t *)&g_Class07_SortMethod,						// Field 7. Sort method                              
		(class07_capture_object_t *)&g_Class07_SortObject,						// Field 8. Sort object                              
		(Unsigned32 *)&g_Class07_UniqueEntries,					// Field 9. Entry in use                             
		(Unsigned32 *)&g_Class07_UniqueEntries,					// Field 10. Profile entries                         
	},
	// 2 - Block Load Profile  
	{
		{   1,   0,   99,   1,   0, 255 },				// Field 1. Logical name (OBIS code) of the object.  
		(access_right_t *)g_AccessRightTable[0],						// Field 2. Access right definition for 1 object     
		&g_Class07_Blockload_Buf,					// Field 3. Buffer                                   
		(class07_capture_object_t *)g_Class07_BlockloadCaptureObjects,				// Field 4. Capture object list                      
		(Unsigned16 *)&g_Class07_BlockloadCaptureObjectsLength,			// Field 5. Capture object list length               
		(Unsigned32 *)&g_Class07_Blockload_CapturePeriod,				// Field 6. Capture period                           
		(sort_method_t *)&g_Class07_SortMethod,						// Field 7. Sort method                              
		(class07_capture_object_t *)&g_Class07_SortObject,						// Field 8. Sort object                              
		&g_Class07_Blockload_EntriesInUse,				// Field 9. Entry in use                             
		(Unsigned32 *)&g_Class07_Blockload_MaxEntries,				// Field 10. Profile entries                         
	},
	// 3 - Block Load Scaler Profile  
	{
		{   1,   0,   94,  91,   4, 255 },				// Field 1. Logical name (OBIS code) of the object.  
		(access_right_t *)g_AccessRightTable[0],						// Field 2. Access right definition for 1 object     
		&g_Class07_BlockloadScaler_Buf,					// Field 3. Buffer                                   
		(class07_capture_object_t *)g_Class07_BlockloadCaptureObjects_Scaler,			// Field 4. Capture object list                      
		(Unsigned16 *)&g_Class07_BlockloadCaptureObjectsLength_Scaler,		// Field 5. Capture object list length               
		(Unsigned32 *)&g_Class07_NoCapturePeriod,					// Field 6. Capture period                           
		(sort_method_t *)&g_Class07_SortMethod,						// Field 7. Sort method                              
		(class07_capture_object_t *)&g_Class07_SortObject,						// Field 8. Sort object                              
		(Unsigned32 *)&g_Class07_UniqueEntries,					// Field 9. Entry in use                             
		(Unsigned32 *)&g_Class07_UniqueEntries,					// Field 10. Profile entries                         
	},
	// 4 - Daily Load Profile  
	{
		{   1,   0,   99,   2,   0, 255 },				// Field 1. Logical name (OBIS code) of the object.  
		(access_right_t *)g_AccessRightTable[0],						// Field 2. Access right definition for 1 object     
		&g_Class07_Dailyload_Buf,					// Field 3. Buffer                                   
		(class07_capture_object_t *)g_Class07_DailyloadCaptureObjects,				// Field 4. Capture object list                      
		(Unsigned16 *)&g_Class07_DailyloadCaptureObjectsLength,			// Field 5. Capture object list length               
		(Unsigned32 *)&g_Class07_Dailyload_CapturePeriod,				// Field 6. Capture period                           
		(sort_method_t *)&g_Class07_SortMethod,						// Field 7. Sort method                              
		(class07_capture_object_t *)&g_Class07_SortObject,						// Field 8. Sort object                              
		&g_Class07_Dailyload_EntriesInUse,				// Field 9. Entry in use                             
		(Unsigned32 *)&g_Class07_Dailyload_MaxEntries,				// Field 10. Profile entries                         
	},
	// 5 - Daily Load Scaler Profile  
	{
		{   1,   0,   94,  91,   5, 255 },				// Field 1. Logical name (OBIS code) of the object.  
		(access_right_t *)g_AccessRightTable[0],						// Field 2. Access right definition for 1 object     
		&g_Class07_DailyloadScaler_Buf,					// Field 3. Buffer                                   
		(class07_capture_object_t *)g_Class07_DailyloadCaptureObjects_Scaler,			// Field 4. Capture object list                      
		(Unsigned16 *)&g_Class07_DailyloadCaptureObjectsLength_Scaler,		// Field 5. Capture object list length               
		(Unsigned32 *)&g_Class07_NoCapturePeriod,					// Field 6. Capture period                           
		(sort_method_t *)&g_Class07_SortMethod,						// Field 7. Sort method                              
		(class07_capture_object_t *)&g_Class07_SortObject,						// Field 8. Sort object                              
		(Unsigned32 *)&g_Class07_UniqueEntries,					// Field 9. Entry in use                             
		(Unsigned32 *)&g_Class07_UniqueEntries,					// Field 10. Profile entries                         
	},
	// 6 - Billing profile  
	{
		{   1,   0,   98,   1,   0, 255 },				// Field 1. Logical name (OBIS code) of the object.  
		(access_right_t *)g_AccessRightTable[0],						// Field 2. Access right definition for 1 object     
		&g_class07_Billing_Buf,						// Field 3. Buffer                                   
		(class07_capture_object_t *)g_Class07_BillingCaptureObjects,				// Field 4. Capture object list                      
		(Unsigned16 *)&g_Class07_BillingCaptureObjectsLength,				// Field 5. Capture object list length               
		(Unsigned32 *)&g_Class07_NoCapturePeriod,					// Field 6. Capture period                           
		(sort_method_t *)&g_Class07_SortMethod,						// Field 7. Sort method                              
		(class07_capture_object_t *)&g_Class07_SortObject,						// Field 8. Sort object                              
		&g_Class07_Billing_EntriesInUse,				// Field 9. Entry in use                             
		(Unsigned32 *)&g_Class07_Billing_MaxEntries,					// Field 10. Profile entries                         
	},
	// 7 - Billing Scaler Profile  
	{
		{   1,   0,   94,  91,   6, 255 },				// Field 1. Logical name (OBIS code) of the object.  
		(access_right_t *)g_AccessRightTable[0],						// Field 2. Access right definition for 1 object     
		&g_class07_BillingScaler_Buf,					// Field 3. Buffer                                   
		(class07_capture_object_t *)g_Class07_BillingCaptureObjects_Scaler,				// Field 4. Capture object list                      
		(Unsigned16 *)&g_Class07_BillingCaptureObjectsLength_Scaler,			// Field 5. Capture object list length               
		(Unsigned32 *)&g_Class07_NoCapturePeriod,					// Field 6. Capture period                           
		(sort_method_t *)&g_Class07_SortMethod,						// Field 7. Sort method                              
		(class07_capture_object_t *)&g_Class07_SortObject,						// Field 8. Sort object                              
		(Unsigned32 *)&g_Class07_UniqueEntries,					// Field 9. Entry in use                             
		(Unsigned32 *)&g_Class07_UniqueEntries,					// Field 10. Profile entries                         
	},
	// 8 - Event 0 Scaler Profile  
	{
		{   1,   0,   94,  91,   7, 255 },				// Field 1. Logical name (OBIS code) of the object.  
		(access_right_t *)g_AccessRightTable[0],						// Field 2. Access right definition for 1 object     
		&g_class07_EventScaler_Buf,					// Field 3. Buffer                                   
		(class07_capture_object_t *)g_Class07_Event0CaptureObjects_Scaler,				// Field 4. Capture object list                      
		(Unsigned16 *)&g_Class07_Event0CaptureObjectsLength_Scaler,			// Field 5. Capture object list length               
		(Unsigned32 *)&g_Class07_NoCapturePeriod,					// Field 6. Capture period                           
		(sort_method_t *)&g_Class07_SortMethod,						// Field 7. Sort method                              
		(class07_capture_object_t *)&g_Class07_SortObject,						// Field 8. Sort object                              
		(Unsigned32 *)&g_Class07_UniqueEntries,					// Field 9. Entry in use                             
		(Unsigned32 *)&g_Class07_UniqueEntries,					// Field 10. Profile entries                         
	},
	// 9 - Event 0 profile-Amendment3 
	{
		{   0,   0,   99,  98,   0, 255 },				// Field 1. Logical name (OBIS code) of the object.  
		(access_right_t *)g_AccessRightTable[0],						// Field 2. Access right definition for 1 object     
		&g_class07_Event0_Buf,						// Field 3. Buffer                                   
		(class07_capture_object_t *)g_Class07_Event0CaptureObjects,					// Field 4. Capture object list                      
		(Unsigned16 *)&g_Class07_Event0CaptureObjectsLength,				// Field 5. Capture object list length               
		(Unsigned32 *)&g_Class07_NoCapturePeriod,					// Field 6. Capture period                           
		(sort_method_t *)&g_Class07_SortMethod,						// Field 7. Sort method                              
		(class07_capture_object_t *)&g_Class07_SortObject,						// Field 8. Sort object                              
		&g_Class07_Event_EntriesInUse[0],				// Field 9. Entry in use                             
		&g_Class07_Event_MaxEntries[0],					// Field 10. Profile entries                         
	},
	// 10 - Event 1 profile-Amendment3 
	{
		{   0,   0,   99,  98,   1, 255 },				// Field 1. Logical name (OBIS code) of the object.  
		(access_right_t *)g_AccessRightTable[0],						// Field 2. Access right definition for 1 object     
		&g_class07_Event1_Buf,						// Field 3. Buffer                                   
		(class07_capture_object_t *)g_Class07_Event1CaptureObjects,					// Field 4. Capture object list                      
		(Unsigned16 *)&g_Class07_Event0CaptureObjectsLength,				// Field 5. Capture object list length               
		(Unsigned32 *)&g_Class07_NoCapturePeriod,					// Field 6. Capture period                           
		(sort_method_t *)&g_Class07_SortMethod,						// Field 7. Sort method                              
		(class07_capture_object_t *)&g_Class07_SortObject,						// Field 8. Sort object                              
		&g_Class07_Event_EntriesInUse[1],				// Field 9. Entry in use                             
		&g_Class07_Event_MaxEntries[1],					// Field 10. Profile entries                         
	},
	// 11 - Event 2 profile-Amendment3 
	{
		{   0,   0,   99,  98,   2, 255 },				// Field 1. Logical name (OBIS code) of the object.  
		(access_right_t *)g_AccessRightTable[0],						// Field 2. Access right definition for 1 object     
		&g_class07_Event2_Buf,						// Field 3. Buffer                                   
		(class07_capture_object_t *)g_Class07_Event2CaptureObjects,					// Field 4. Capture object list                      
		(Unsigned16 *)&g_Class07_Event0CaptureObjectsLength,				// Field 5. Capture object list length               
		(Unsigned32 *)&g_Class07_NoCapturePeriod,					// Field 6. Capture period                           
		(sort_method_t *)&g_Class07_SortMethod,						// Field 7. Sort method                              
		(class07_capture_object_t *)&g_Class07_SortObject,						// Field 8. Sort object                              
		&g_Class07_Event_EntriesInUse[2],				// Field 9. Entry in use                             
		&g_Class07_Event_MaxEntries[2],					// Field 10. Profile entries                         
	},
	// 12 - Event 3 profile-Amendment3 
	{
		{   0,   0,   99,  98,   3, 255 },				// Field 1. Logical name (OBIS code) of the object.  
		(access_right_t *)g_AccessRightTable[0],						// Field 2. Access right definition for 1 object     
		&g_class07_Event3_Buf,						// Field 3. Buffer                                   
		(class07_capture_object_t *)g_Class07_Event3CaptureObjects,					// Field 4. Capture object list                      
		(Unsigned16 *)&g_Class07_Event0CaptureObjectsLength,				// Field 5. Capture object list length               
		(Unsigned32 *)&g_Class07_NoCapturePeriod,					// Field 6. Capture period                           
		(sort_method_t *)&g_Class07_SortMethod,						// Field 7. Sort method                              
		(class07_capture_object_t *)&g_Class07_SortObject,						// Field 8. Sort object                              
		&g_Class07_Event_EntriesInUse[3],				// Field 9. Entry in use                             
		&g_Class07_Event_MaxEntries[3],					// Field 10. Profile entries                         
	},
	// 13 - Event 4 profile-Amendment3 
	{
		{   0,   0,   99,  98,   4, 255 },				// Field 1. Logical name (OBIS code) of the object.  
		(access_right_t *)g_AccessRightTable[0],						// Field 2. Access right definition for 1 object     
		&g_class07_Event4_Buf,						// Field 3. Buffer                                   
		(class07_capture_object_t *)g_Class07_Event4CaptureObjects,					// Field 4. Capture object list                      
		(Unsigned16 *)&g_Class07_Event0CaptureObjectsLength,				// Field 5. Capture object list length               
		(Unsigned32 *)&g_Class07_NoCapturePeriod,					// Field 6. Capture period                           
		(sort_method_t *)&g_Class07_SortMethod,						// Field 7. Sort method                              
		(class07_capture_object_t *)&g_Class07_SortObject,						// Field 8. Sort object                              
		&g_Class07_Event_EntriesInUse[4],				// Field 9. Entry in use                             
		&g_Class07_Event_MaxEntries[4],					// Field 10. Profile entries                         
	},
	// 14 - Event 5 profile-Amendment3 
	{
		{   0,   0,   99,  98,   5, 255 },				// Field 1. Logical name (OBIS code) of the object.  
		(access_right_t *)g_AccessRightTable[0],						// Field 2. Access right definition for 1 object     
		&g_class07_Event5_Buf,						// Field 3. Buffer                                   
		(class07_capture_object_t *)g_Class07_Event5CaptureObjects,					// Field 4. Capture object list                      
		(Unsigned16 *)&g_Class07_Event0CaptureObjectsLength,				// Field 5. Capture object list length               
		(Unsigned32 *)&g_Class07_NoCapturePeriod,					// Field 6. Capture period                           
		(sort_method_t *)&g_Class07_SortMethod,						// Field 7. Sort method                              
		(class07_capture_object_t *)&g_Class07_SortObject,						// Field 8. Sort object                              
		&g_Class07_Event_EntriesInUse[5],				// Field 9. Entry in use                             
		&g_Class07_Event_MaxEntries[5],					// Field 10. Profile entries                         
	},
	// 15 - Name Plate profile  
	{
		{   0,   0,   94,  91,   10, 255 },				// Field 1. Logical name (OBIS code) of the object.  
		(access_right_t *)g_AccessRightTable[0],						// Field 2. Access right definition for 1 object     
		&g_class07_NamePlate_Buf,					// Field 3. Buffer                                   
		(class07_capture_object_t *)g_Class07_NamePlateCaptureObjects,				// Field 4. Capture object list                      
		(Unsigned16 *)&g_Class07_NamePlateCaptureObjectsLength,			// Field 5. Capture object list length               
		(Unsigned32 *)&g_Class07_NoCapturePeriod,					// Field 6. Capture period                           
		(sort_method_t *)&g_Class07_SortMethod,						// Field 7. Sort method                              
		(class07_capture_object_t *)&g_Class07_SortObject,						// Field 8. Sort object                              
		(Unsigned32 *)&g_Class07_UniqueEntries,					// Field 9. Entry in use                             
		(Unsigned32 *)&g_Class07_UniqueEntries,					// Field 10. Profile entries                         
	},
};
const Unsigned16 g_ChildTableLengthClass07 = sizeof(g_ChildTableClass07) / sizeof(class07_child_record_t);
#endif

/***************************************************************************************
 * Class ID = 8, Clock Child-table
 *
 * NOTE: THE CLASS 08 ALLOW ONLY 1 INSTANCE TO BE IMPLEMENTED.
 *
 * How to define a row of this table.
 *
 * Field 1. Fill the OBIS code first, 6 numbers (A, B, C, D, E, F)
 * Field 2. Specify which row on g_AccessRightTable will be used to declare the object access right
 * Field 3. Select the buffer Attr 2 - time (date_time_t *)
 * Field 4. Select the buffer Attr 3 - time_zone (Integer16 *)
 * Field 5. Select the buffer Attr 4 - status (Unsigned8 *)
 * Field 6. Select the buffer Attr 5 - daylight_savings_begin (date_time_t *)
 * Field 7. Select the buffer Attr 6 - daylight_savings_end (date_time_t *)
 * Field 8. Select the buffer Attr 7 - daylight_savings_deviation (Integer8 *)
 * Field 9. Select the buffer Attr 8 - daylight_savings_enabled (Unsigned8 *)
 * Field 10. Select the buffer Attr 9 - clock_base (clock_base_t *)
 *
 * If NULL is specified in buffer parameter, the buffer attr will be get/set
 * on R_OBIS_GetObjectAttr and R_OBIS_SetObjectAttr
 ****************************************************************************************/
#if (defined(USED_CLASS_08) && USED_CLASS_08 == 1)
Integer16		g_Class08_RTC_TimeZone= 330;
Unsigned8		g_Class08_RTC_Status;
const date_time_t	g_Class08_RTC_DaylightSavingsBegin= {NOT_SPECIFIED,NOT_SPECIFIED,NOT_SPECIFIED,NOT_SPECIFIED,NOT_SPECIFIED,NOT_SPECIFIED,
											 NOT_SPECIFIED,NOT_SPECIFIED,NOT_SPECIFIED,0x80,0x00,NOT_SPECIFIED};
const date_time_t	g_Class08_RTC_DaylightSavingsEnd= {NOT_SPECIFIED,NOT_SPECIFIED,NOT_SPECIFIED,NOT_SPECIFIED,NOT_SPECIFIED,NOT_SPECIFIED,
											 NOT_SPECIFIED,NOT_SPECIFIED,NOT_SPECIFIED,0x80,0x00,NOT_SPECIFIED};
const Integer8		g_Class08_RTC_DaylightSavingsDeviation;
const Unsigned8		g_Class08_RTC_DaylightSavingsEnabled = 0;
const clock_base_t	g_Class08_RTC_ClockBase = CLOCK_BASE_INTERNAL_CRYSTAL;

const class08_child_record_t g_ChildTableClass08[] =
{
	/* RTC Clock */
	{
		{   0,   0,   1,   0,   0, 255 },										/* Field 1.  OBIS */
		(access_right_t *)g_AccessRightTable[3],												/* Field 2.  Access Right */
		DYN_DATE_TIME(NULL, ATTR_ACCESS_USERFUNC),									/* Field 3.  Attr 2 - time */
		&g_Class08_RTC_TimeZone ,								                        /* Field 4.  Attr 3 - time_zone */
		DYN_VALUE(CHOICE_UNSIGNED, &g_Class08_RTC_Status, ATTR_ACCESS_MEMORY),						/* Field 5.  Attr 4 - status */
		(date_time_t *)&g_Class08_RTC_DaylightSavingsBegin,										/* Field 6.  Attr 5 - daylight_savings_begin */
		(date_time_t *)&g_Class08_RTC_DaylightSavingsEnd,										/* Field 7.  Attr 6 - daylight_savings_end */
		(Integer8 *)&g_Class08_RTC_DaylightSavingsDeviation,									/* Field 8.  Attr 7 - daylight_savings_deviation */
		(Unsigned8 *)&g_Class08_RTC_DaylightSavingsEnabled,										/* Field 9.  Attr 8 - daylight_savings_enabled */
		(clock_base_t *)&g_Class08_RTC_ClockBase,											/* Field 10. Attr 8 - clock_base */
	},
};
const Unsigned16 g_ChildTableLengthClass08 = sizeof(g_ChildTableClass08) / sizeof(class08_child_record_t);
#endif

/****************************************************************************************
 * Class ID = 9, Script table Child-table
 *
 *
 * How to define a row of this table.
 *
 * Field 1. Fill the OBIS code first, 6 numbers (A, B, C, D, E, F)
 * Field 2. Specify which row on g_AccessRightTable will be used to declare the object access right
 * Field 3. Buffer pointer of script (script_t *)
 * Field 4. Number of script
 *
 ****************************************************************************************/
#if (defined(USED_CLASS_09) && USED_CLASS_09 == 1)

/* Static parameters */
const Unsigned16		g_Class09_param = 0;		/* Capture all */
const Unsigned16		g_Class09_param2 = 0xFF;	/* Not specific */

/* Capture Data of Billing Period Profile */
const script_action_spec_t g_Class09_billaction[] =
{
	{SERVICE_EXECUTE_METHOD,								/* Service id */
	7,											/* Class id */
	{   1,   0,   98,   1,   0, 255 },							/* Logical name */
	2,											/* index of attribute/method */
	VALUE(CHOICE_INTEGER, (Unsigned16 *)&g_Class09_param),						/* Paramenters */
	},
};

/* MDI Reset / End of Billing Period Script */
const script_t g_Class09_Script_Billing[] =
{
	/* Capture Data of Billing Period Profile */
	{0x0001,										/* Script ID*/
	(script_action_spec_t *)g_Class09_billaction,									/* Action specification*/
	sizeof(g_Class09_billaction)/sizeof(script_action_spec_t),				/* Number of action specification*/
	},
};

/* Tariffication Script Table action */
const script_action_spec_t g_Class09_tariffaction[] =
{
	/* Cumulative energy, kWh for TZ1 */
	{
	SERVICE_WRITE_ATTRIBUTE,								/* Service id */
	3,											/* Class id */
	{   1,   0,   1,   8,   1,   255 },							/* Logical name */
	2,											/* index of attribute/method */
	VALUE(CHOICE_INTEGER, (Unsigned16 *)&g_Class09_param2),						/* Paramenters */
	},
	/* Cumulative energy, kWh for TZ2 */
	{
	SERVICE_WRITE_ATTRIBUTE,								/* Service id */
	3,											/* Class id */
	{   1,   0,   1,   8,   2,   255 },							/* Logical name */
	2,											/* index of attribute/method */
	VALUE(CHOICE_INTEGER, (Unsigned16 *)&g_Class09_param2),						/* Paramenters */
	},
	/* Cumulative energy, kWh for TZ3 */
	{
	SERVICE_WRITE_ATTRIBUTE,								/* Service id */
	3,											/* Class id */
	{   1,   0,   1,   8,   3,   255 },							/* Logical name */
	2,											/* index of attribute/method */
	VALUE(CHOICE_INTEGER, (Unsigned16 *)&g_Class09_param2),						/* Paramenters */
	},
	/* Cumulative energy, kWh for TZ4 */
	{
	SERVICE_WRITE_ATTRIBUTE,								/* Service id */
	3,											/* Class id */
	{   1,   0,   1,   8,   4,   255 },							/* Logical name */
	2,											/* index of attribute/method */
	VALUE(CHOICE_INTEGER, (Unsigned16 *)&g_Class09_param2),						/* Paramenters */
	}
};

/*  Tariffication Script Table Script */
const script_t g_Class09_Script_Tariff[] =
{
	/* Capture Data of Billing Period Profile */
	{
	0x0001,													/* Script ID*/
	(script_action_spec_t *)&g_Class09_tariffaction[0],										/* Action specification*/
	1,													/* Number of action specification*/
	},
	/* Capture Data of Billing Period Profile */
	{
	0x0002,													/* Script ID*/
	(script_action_spec_t *)&g_Class09_tariffaction[1],										/* Action specification*/
	1,													/* Number of action specification*/
	},
	/* Capture Data of Billing Period Profile */
	{
	0x0003,													/* Script ID*/
	(script_action_spec_t *)&g_Class09_tariffaction[2],										/* Action specification*/
	1,													/* Number of action specification*/
	},
	/* Capture Data of Billing Period Profile */
	{
	0x0004,													/* Script ID*/
	(script_action_spec_t *)&g_Class09_tariffaction[3],										/* Action specification*/
	1,													/* Number of action specification*/
	}
};

const class09_child_record_t g_ChildTableClass09[] =
{
	/* MDI Reset / End of Billing Period Script Table */
	{
		{   0,   0,   10,   0,   1, 255 },						/* Field 1. OBIS */
		(access_right_t *)g_AccessRightTable[9],								/* Field 2. Access Right */
		(script_t *)g_Class09_Script_Billing,							/* Field 3. Buffer pointer of script (script_t *) */
		sizeof(g_Class09_Script_Billing)/sizeof(script_t)				/* Field 4. Number of script */
	},
	/* Tariffication Script Table */
	{
		{   0,   0,   10,   0,  100, 255 },						/* Field 1. OBIS */
		(access_right_t *)g_AccessRightTable[9],								/* Field 2. Access Right */
		(script_t *)g_Class09_Script_Tariff,							/* Field 3. Buffer pointer of script (script_t *) */
		sizeof(g_Class09_Script_Tariff)/sizeof(script_t)				/* Field 4. Number of script */
	}
};
const Unsigned16 g_ChildTableLengthClass09 = sizeof(g_ChildTableClass09) / sizeof(class09_child_record_t);
#endif

/****************************************************************************************
 * Class ID = 10, Schedule Child-table
 *
 * How to define a row of this table.
 *
 * Field 1. Fill the OBIS code first, 6 numbers (A, B, C, D, E, F)
 * Field 2. Specify which row on g_AccessRightTable will be used to declare the
 *          access right of the object.
 * Field 3. Specifies the different entries of schedule_table_entry_t
 * Field 4. Number of entries
 *
****************************************************************************************/
#if (defined(USED_CLASS_10) && USED_CLASS_10 == 1)
/* Parameters definition */
const Unsigned8	g_Class10_schedule_enable[] = {0};
const time_t		g_Class10_switch_time;
const date_t		g_Class10_begin_date;
const date_t		g_Class10_end_date;

const schedule_table_entry_t  g_Class10_schedule_lists[] =
{
	{0x01,				 					/* Index */
	&g_Class10_schedule_enable[0],			/* Enable */
	{   0,   0,   10,   0,   100, 255 },	/* The logical name of the �Script table� object */
	0x0001,									/* Script_identifier of the script to be executed */
	&g_Class10_switch_time,					/* switch_time */
	0xFFFF,									/* validity_window */
	0x01,									/* Day of week. Interpreted as unsigned. 1...7 */
	0x01,									/* Link to the IC �Special days table�, day_id; */
	&g_Class10_begin_date,					/* begin_date */
	&g_Class10_end_date,					/* end_date */
	},
};

Unsigned8 g_Class10_nr_entries = sizeof(g_Class10_schedule_lists) / sizeof(schedule_table_entry_t);

const class10_child_record_t g_ChildTableClass10[] =
{
	/* Abtract Schedule object*/
	{
		{0, 0, 12, 0, 0, 255 },				/* Field 1. Logical name (OBIS code) of the object. */
		g_AccessRightTable[0],				/* Field 2. Access right definition for 1 object    */
		g_Class10_schedule_lists,			/* Field 3. Specifies the different entries         */
		&g_Class10_nr_entries,				/* Field 4. Number of entries                       */
	},
};
const Unsigned16 g_ChildTableLengthClass10 = sizeof(g_ChildTableClass10) / sizeof(class10_child_record_t);
#endif

/****************************************************************************************
 * Class ID = 11, Special days table Child-table
 *
 * How to define a row of this table.
 *
 * Field 1. Fill the OBIS code first, 6 numbers (A, B, C, D, E, F)
 * Field 2. Specify which row on g_AccessRightTable will be used to declare the
 *          access right of the object.
 * Field 3. Specifies the different entries of spec_day_entry_t
 * Field 4. Number of entries
 *
****************************************************************************************/
#if (defined(USED_CLASS_11) && USED_CLASS_11 == 1)
/* Variable parameters */
const date_t		g_Class11_special_date;

/* Static parameters */
const spec_day_entry_t  g_Class11_spec_day_lists[] =
{
	{0x0001,				/* Index */
	&g_Class11_special_date,				/* specialday_date */
	0x01,									/* exec_specdays in the related object of IC �Schedule� */
	},
};

const Unsigned8 g_Class11_nr_entries = sizeof(g_Class11_spec_day_lists) / sizeof(spec_day_entry_t);

const class11_child_record_t g_ChildTableClass11[] =
{
	/* Abstract Special days table objects*/
	{
		{0, 0, 11, 0, 0, 255 },				/* Field 1. Logical name (OBIS code) of the object. */
		g_AccessRightTable[0],				/* Field 2. Access right definition for 1 object    */
		g_Class11_spec_day_lists,			/* Field 3. Specifies the different entries         */
		&g_Class11_nr_entries,				/* Field 4. Number of entries                       */
	},
};
const Unsigned16 g_ChildTableLengthClass11 = sizeof(g_ChildTableClass11) / sizeof(class11_child_record_t);
#endif

/* Class ID = 15, Association LN Child-table */
#if (defined(USED_CLASS_15) && USED_CLASS_15 == 1)
const Unsigned8 secret0[] ="";		/* 16 byte */
association_status_t g_ChildTableClass15_assc0_status = ASSC_STATUS_NON_ASSOCIATED;
Unsigned8 secret1[9];		/* 9 byte */
association_status_t g_ChildTableClass15_assc1_status = ASSC_STATUS_NON_ASSOCIATED;
Unsigned8 secret2[17];		/* 17 byte */
association_status_t g_ChildTableClass15_assc2_status = ASSC_STATUS_NON_ASSOCIATED;
association_status_t g_ChildTableClass15_assc3_status = ASSC_STATUS_NON_ASSOCIATED;
association_status_t g_ChildTableClass15_assc4_status = ASSC_STATUS_NON_ASSOCIATED;

/* for assc2 (HLS : Secret) */
authentication_status_t g_ChildTableClass15_assc2_authen_status = AUTHEN_STATUS_PENDING;
authentication_status_t g_ChildTableClass15_assc3_authen_status = AUTHEN_STATUS_PENDING;
authentication_status_t g_ChildTableClass15_assc4_authen_status = AUTHEN_STATUS_PENDING;

//void *secret0 = NULL;
//void *secret1 = NULL;
//Unsigned8 *secret0;// ="ABCDEFGH";
//Unsigned8 *secret1;//="RENESAS_P6wRJ21F";
//extern Unsigned8 secret0[];// ="ABCDEFGH";

const class15_child_record_t g_ChildTableClass15[] = 
{
	/* Assc0 */
	{
		/* OBIS Code */
		{   0,   0,  40,   0,   1, 255 },
		
		/* Access right */
		(access_right_t *)g_AccessRightTable[4],
	
		/* Associated partners id                            */
		/*   client_SAP : unsigned                           */
		/*   server SAP : long-unsigned                      */
		{0x10, 0x0001},

		/* Context id of application context                 */
		/* (Only select one of following)                    */
		CONTEXT_ID1_LN_NO_CIPHERING,
		//CONTEXT_ID2_SN_NO_CIPHERING,
		//CONTEXT_ID3_LN_WITH_CIPHERING,
		//CONTEXT_ID4_SN_WITH_CIPHERING,
		
		/* Mechanism id of authentication mechanism name     */
		/* (Only select one of following)                    */
		MECHANISM_ID0_LOWEST_SECURITY,
		//MECHANISM_ID1_LOW_SECURITY,
		//MECHANISM_ID2_HIGH_SECURITY,
		//MECHANISM_ID3_HIGH_SECURITY_MD5,
		//MECHANISM_ID4_HIGH_SECURITY_SHA1,
		//MECHANISM_ID5_HIGH_SECURITY_GMAC,

		/* Conformance block of association                  */
		/* (Un-comment to enable features)                   */
		{
			/* Byte 0 */
			//CONFORMANCE_BYTE0_SN_READ                |
			//CONFORMANCE_BYTE0_SN_WRITE               |
			//CONFORMANCE_BYTE0_SN_UNCONFIRMED_WRITE   |
			CONFORMANCE_BYTE0_NONE,
			
			/* Byte 1 */
			//CONFORMANCE_BYTE1_LN_ATTR0_GET               |
			//CONFORMANCE_BYTE1_LN_PRIORITY_MGMT           |
			//CONFORMANCE_BYTE1_LN_ATTR0_SET               |
			//CONFORMANCE_BYTE1_BLOCK_TRANSFER_GET         |
			//CONFORMANCE_BYTE1_BLOCK_TRANSFER_SET         |
			//CONFORMANCE_BYTE1_LN_BLOCK_TRANSFER_ACTION   |
			//CONFORMANCE_BYTE1_MULTI_REFERENCES           |
			//CONFORMANCE_BYTE1_SN_INFORMATION_REPORT      |
			CONFORMANCE_BYTE1_NONE,
			
			/* Byte 2 */
			//CONFORMANCE_BYTE2_SN_PARAMETERIZED_ACCESS    |
			CONFORMANCE_BYTE2_LN_GET                     |
			// CONFORMANCE_BYTE2_LN_SET                     |
			//CONFORMANCE_BYTE2_LN_SELECTIVE_ACCESS        |
			//CONFORMANCE_BYTE2_LN_EVENT_NOTIFICATION      |
			// CONFORMANCE_BYTE2_LN_ACTION                  |
			CONFORMANCE_BYTE2_NONE,
		},
		
		/* DLMS version number                               */
		6,
		
		/* Secret key : LLS (password) or HLS (encrypt key)  */
		(Unsigned8 *)&secret0,
		
		/* Shared key : HLS (StoC)                           */
		NULL,
		
		/* Association status                                */
		&g_ChildTableClass15_assc0_status,
		
		/* Authentication status (required for HLS only)     */
		NULL,
		
		/* Security setup reference                          */
		NULL,
	},
	
	/* Assc1 */
	{
		/* OBIS Code */
		{   0,   0,  40,   0,   2, 255 },
		
		/* Access right */
		(access_right_t *)g_AccessRightTable[5],
		
		/* Associated partners id                            */
		/*   client_SAP : unsigned                           */
		/*   server SAP : long-unsigned                      */
		{0x20, 0x0001},

		/* Context id of application context                 */
		/* (Only select one of following)                    */
		CONTEXT_ID1_LN_NO_CIPHERING,
		//CONTEXT_ID2_SN_NO_CIPHERING,
		//CONTEXT_ID3_LN_WITH_CIPHERING,
		//CONTEXT_ID4_SN_WITH_CIPHERING,
		
		/* Mechanism id of authentication mechanism name     */
		/* (Only select one of following)                    */
		//MECHANISM_ID0_LOWEST_SECURITY,
		MECHANISM_ID1_LOW_SECURITY,
		//MECHANISM_ID2_HIGH_SECURITY,
		//MECHANISM_ID3_HIGH_SECURITY_MD5,
		//MECHANISM_ID4_HIGH_SECURITY_SHA1,
		//MECHANISM_ID5_HIGH_SECURITY_GMAC,

		/* Conformance block of association                  */
		/* (Un-comment to enable features)                   */
		{
			/* Byte 0 */
			//CONFORMANCE_BYTE0_SN_READ                |
			//CONFORMANCE_BYTE0_SN_WRITE               |
			//CONFORMANCE_BYTE0_SN_UNCONFIRMED_WRITE   |
			CONFORMANCE_BYTE0_NONE,
			
			/* Byte 1 */
			//CONFORMANCE_BYTE1_LN_ATTR0_GET               |
			//CONFORMANCE_BYTE1_LN_PRIORITY_MGMT           |
			//CONFORMANCE_BYTE1_LN_ATTR0_SET               |
			CONFORMANCE_BYTE1_BLOCK_TRANSFER_GET         |
			//CONFORMANCE_BYTE1_BLOCK_TRANSFER_SET         |
			//CONFORMANCE_BYTE1_LN_BLOCK_TRANSFER_ACTION   |
			//CONFORMANCE_BYTE1_MULTI_REFERENCES           |
			//CONFORMANCE_BYTE1_SN_INFORMATION_REPORT      |
			CONFORMANCE_BYTE1_NONE,
			
			/* Byte 2 */
			//CONFORMANCE_BYTE2_SN_PARAMETERIZED_ACCESS    |
			CONFORMANCE_BYTE2_LN_GET                     |
			//CONFORMANCE_BYTE2_LN_SET                     |
			CONFORMANCE_BYTE2_LN_SELECTIVE_ACCESS        |
			//CONFORMANCE_BYTE2_LN_EVENT_NOTIFICATION      |
			//CONFORMANCE_BYTE2_LN_ACTION                  |
			CONFORMANCE_BYTE2_NONE,
		},
		
		/* DLMS version number                               */
		6,
		
		/* Secret key : LLS (password) or HLS (encrypt key)  */
		(Unsigned8 *)&secret1,
		
		/* Shared key : HLS (StoC)                           */
		NULL,
		
		/* Association status                                */
		&g_ChildTableClass15_assc1_status,
		
		/* Authentication status (required for HLS only)     */
		NULL,
		
		/* Security setup reference                          */
		NULL,
	},
	/* Assc2 */
	{
		/* OBIS Code */
		{   0,   0,  40,   0,   3, 255 },
		
		/* Access right */
		(access_right_t *)g_AccessRightTable[6],
		
		/* Associated partners id                            */
		/*   client_SAP : unsigned                           */
		/*   server SAP : long-unsigned                      */
		{0x30, 0x0001},

		/* Context id of application context                 */
		/* (Only select one of following)                    */
		CONTEXT_ID1_LN_NO_CIPHERING,
		//CONTEXT_ID2_SN_NO_CIPHERING,
		//CONTEXT_ID3_LN_WITH_CIPHERING,
		//CONTEXT_ID4_SN_WITH_CIPHERING,
		
		/* Mechanism id of authentication mechanism name     */
		/* (Only select one of following)                    */
		//MECHANISM_ID0_LOWEST_SECURITY,
		//MECHANISM_ID1_LOW_SECURITY,
		MECHANISM_ID2_HIGH_SECURITY,
		//MECHANISM_ID3_HIGH_SECURITY_MD5,
		//MECHANISM_ID4_HIGH_SECURITY_SHA1,
		//MECHANISM_ID5_HIGH_SECURITY_GMAC,

		/* Conformance block of association                  */
		/* (Un-comment to enable features)                   */
		{
			/* Byte 0 */
			//CONFORMANCE_BYTE0_SN_READ                |
			//CONFORMANCE_BYTE0_SN_WRITE               |
			//CONFORMANCE_BYTE0_SN_UNCONFIRMED_WRITE   |
			CONFORMANCE_BYTE0_NONE,
			
			/* Byte 1 */
			//CONFORMANCE_BYTE1_LN_ATTR0_GET               |
			//CONFORMANCE_BYTE1_LN_PRIORITY_MGMT           |
			//CONFORMANCE_BYTE1_LN_ATTR0_SET               |
			CONFORMANCE_BYTE1_BLOCK_TRANSFER_GET         |
			CONFORMANCE_BYTE1_BLOCK_TRANSFER_SET         |
			//CONFORMANCE_BYTE1_LN_BLOCK_TRANSFER_ACTION   |
			//CONFORMANCE_BYTE1_MULTI_REFERENCES           |
			//CONFORMANCE_BYTE1_SN_INFORMATION_REPORT      |
			CONFORMANCE_BYTE1_NONE,
			
			/* Byte 2 */
			//CONFORMANCE_BYTE2_SN_PARAMETERIZED_ACCESS    |
			CONFORMANCE_BYTE2_LN_GET                     |
			CONFORMANCE_BYTE2_LN_SET                     |
			CONFORMANCE_BYTE2_LN_SELECTIVE_ACCESS        |
			//CONFORMANCE_BYTE2_LN_EVENT_NOTIFICATION      |
			CONFORMANCE_BYTE2_LN_ACTION                  |
			CONFORMANCE_BYTE2_NONE,
		},
		
		/* DLMS version number                               */
		6,
		
		/* Secret key : LLS (password) or HLS (encrypt key)  */
		(Unsigned8 *)&secret2,
		
		/* Shared key : HLS (StoC)                           */
		(Unsigned8 *)"RENESAS_P6wRJ21F",
		
		/* Association status                                */
		&g_ChildTableClass15_assc2_status,
		
		/* Authentication status (required for HLS only)     */
		&g_ChildTableClass15_assc2_authen_status,
		
		/* Security setup reference                          */
		NULL,
	},

};
const Unsigned16 g_ChildTableLengthClass15 = sizeof(g_ChildTableClass15) / sizeof(class15_child_record_t);
#endif

/****************************************************************************************
 * Class ID = 17, SAP assignment Child-table
 *
 * How to define a row of this table.
 *
 * Field 1. Fill the OBIS code first, 6 numbers (A, B, C, D, E, F)
 * Field 2. Specify which row on g_AccessRightTable will be used to declare the
 *          access right of the object.
 * Field 3. Select the value (asslist_entry_t) that used for Attr 2 - SAP_assignment_list.
 * Field 4. Select the number of SAP assignment entries.
 *
****************************************************************************************/
#if (defined(USED_CLASS_17) && USED_CLASS_17 == 1)

/* Static parameters */
//const Unsigned8	g_LDN_name1[] = "RES 5418 1";
//const Unsigned8	g_LDN_name2[] = "RES 5418 2";
//const Unsigned8	g_LDN_name3[] = "RES 5418 3";

const Unsigned8	g_LDN_name1[] = "EEOEPL";
const Unsigned8	g_LDN_name2[] = "EEOEPL 1";
const Unsigned8	g_LDN_name3[] = "EEOEPL 2";

//const Unsigned8	g_LDN_name1[] = "UEIL";
//const Unsigned8	g_LDN_name2[] = "UEIL 1";
//const Unsigned8	g_LDN_name3[] = "UEIL 2";

/*
const asslist_entry_t  g_Class17_SAP_lists[] = {
{0x0001, VALUE(CHOICE_OCTET_STRING(obis_strlen(g_LDN_name1)), g_LDN_name1)},
{0x0002, VALUE(CHOICE_OCTET_STRING(obis_strlen(g_LDN_name2)), g_LDN_name2)},
{0x0003, VALUE(CHOICE_OCTET_STRING(obis_strlen(g_LDN_name3)), g_LDN_name3)},
};
*/


const asslist_entry_t  g_Class17_SAP_lists[] = {
{0x0001, VALUE(CHOICE_OCTET_STRING(obis_strlen(g_LDN_name1)), g_LDN_name1)},
};


const class17_child_record_t g_ChildTableClass17[] =
{
	/*--------------------------------------------------------------------------------------------------------------------------------------*/
	/*  OBIS Code (A, B, C, D, E, F)    | Access Right         | SAP assignment list   | Size of SAP assignment list                        */
	/*--------------------------------------------------------------------------------------------------------------------------------------*/
	/* SAP assignment */
	{
		{0, 0, 41, 0, 0, 255 },			g_AccessRightTable[0],	g_Class17_SAP_lists,	sizeof(g_Class17_SAP_lists) / sizeof(asslist_entry_t),
	},
};
const Unsigned16 g_ChildTableLengthClass17 = sizeof(g_ChildTableClass17) / sizeof(class17_child_record_t);
#endif

/****************************************************************************************
 * Class ID = 18, Image transfer Child-table
 *
 * How to define a row of this table.
 *
 * Field 1. Fill the OBIS code first, 6 numbers (A, B, C, D, E, F)
 * Field 2. Specify which row on g_AccessRightTable will be used to declare the
 *          access right of the object.
 * Field 3. Img block size
 * Field 4. Img transfer blocks status
 * Field 5. Img first not transfer block number
 * Field 6. Image_transfer_enabled
 * Field 7. Image_transfer_status
 * Field 8. Image_to_activate_info
 * Field 9. Image_to_activate_info number
 *
****************************************************************************************/
#if (defined(USED_CLASS_18) && USED_CLASS_18 == 1)
/* Static parameters */

/* Variable parameters */
const Unsigned32		g_Class18_img_block_size = 32;
Unsigned8			g_Class18_status[] = "0";
Unsigned32		g_Class18_first_not_transfer;
Unsigned8			g_Class18_img_trans_enabled = 0xFF;
const img_transfer_status_t g_Class18_trans_status = IMG_TRANS_NOT_INIT;

Unsigned32			g_Class18_actinfo_img_size;
Unsigned8			g_Class18_actinfo_id[] = "0";
Unsigned8			g_Class18_actinfo_sign[] = "0";
img_to_activate_info_t g_Class18_activate_info[] = {
	/* activate_info 0 */
	{
	DYN_VALUE(CHOICE_DOUBLE_LONG_UNSIGNED, &g_Class18_actinfo_img_size, ATTR_ACCESS_MEMORY),
	DYN_VALUE(CHOICE_OCTET_STRING(obis_strlen(g_Class18_actinfo_id)), g_Class18_actinfo_id, ATTR_ACCESS_MEMORY),
	DYN_VALUE(CHOICE_OCTET_STRING(obis_strlen(g_Class18_actinfo_sign)), g_Class18_actinfo_sign, ATTR_ACCESS_MEMORY),
	},
};

Unsigned8 g_Class18_activate_info_nr = sizeof(g_Class18_activate_info)/sizeof(img_to_activate_info_t);

const class18_child_record_t g_ChildTableClass18[] =
{
	/* Image transfer object */
	{
		{0, 0, 44, 0, 0, 255 },																							/* Field 1. Logical name (OBIS code) of the object. */
		g_AccessRightTable[0],																							/* Field 2. Access right definition for 1 object    */
		&g_Class18_img_block_size,																						/* Field 3. Img block size                          */
		DYN_VALUE(CHOICE_BIT_STRING(obis_strlen(g_Class18_status)), g_Class18_status, ATTR_ACCESS_MEMORY),				/* Field 4. Img transfer blocks status              */
		DYN_VALUE(CHOICE_DOUBLE_LONG_UNSIGNED, &g_Class18_first_not_transfer, ATTR_ACCESS_MEMORY),						/* Field 5. Img first not transfer block number     */
		&g_Class18_img_trans_enabled,																					/* Field 6. Image_transfer_enabled                  */
		DYN_VALUE(CHOICE_ENUM, &g_Class18_trans_status, ATTR_ACCESS_MEMORY),											/* Field 7. Image_transfer_status                   */
		g_Class18_activate_info,																						/* Field 8. Image_to_activate_info                  */
		&g_Class18_activate_info_nr,																					/* Field 9. Image_to_activate_info number           */
	},
};
const Unsigned16 g_ChildTableLengthClass18 = sizeof(g_ChildTableClass18) / sizeof(class18_child_record_t);
#endif

/****************************************************************************************
 * Class ID = 19, IEC local port setup Child-table
 *
 * How to define a row of this table.
 *
 * Field 1. Fill the OBIS code first, 6 numbers (A, B, C, D, E, F)
 * Field 2. Specify which row on g_AccessRightTable will be used to declare the
 *          access right of the object.
 * Field 3. Default mode
 * Field 4. Default baud rate
 * Field 5. baud rate proposed by the meter
 * Field 6. Respond time
 * Field 7. Device address
 * Field 8. Password 1
 * Field 9. Password 2
 * Field 10. Password w5
 *
****************************************************************************************/
#if (defined(USED_CLASS_19) && USED_CLASS_19 == 1)
/* Static parameters */

/* Variable parameters */
const Unsigned8		g_Class19_device_addr[] = "RES001";
const Unsigned8		g_Class19_device_addr_len = obis_strlen(g_Class19_device_addr);

const Unsigned8		g_Class19_pass_p1[] = "p1";
const Unsigned8		g_Class19_pass_p1_len = obis_strlen(g_Class19_device_addr);
const Unsigned8		g_Class19_pass_p2[] = "p2";
const Unsigned8		g_Class19_pass_p2_len = obis_strlen(g_Class19_pass_p2);
const Unsigned8		g_Class19_pass_w5[] = "w5";
const Unsigned8		g_Class19_pass_w5_len = obis_strlen(g_Class19_pass_w5);

const protocal_mode_t		g_Class19_default_mode = PROTOCAL_DLMSUA;
const baud_rate_t		g_Class19_default_baud = OBIS_BAUD_CONVERT(BAUD_RATE_300);
const baud_rate_t		g_Class19_prop_baud = OBIS_BAUD_CONVERT(BAUD_RATE_9600);
const response_time_t		g_Class19_response_time = TIME_200_MS;

const class19_child_record_t g_ChildTableClass19[] =
{
	/* IEC electrical port setup */
	{
		{0, 0, 20, 0, 1, 255 },								/* Field 1. Logical name (OBIS code) of the object. */
		g_AccessRightTable[0],								/* Field 2. Access right definition for 1 object    */
		&g_Class19_default_mode,							/* Field 3. Default mode                            */
		&g_Class19_default_baud,							/* Field 4. Default baud rate                       */
		&g_Class19_prop_baud,								/* Field 5. baud rate proposed by the meter         */
		&g_Class19_response_time,							/* Field 6. Respond time                            */
		g_Class19_device_addr,								/* Field 7. Device address                          */
		&g_Class19_device_addr_len,							/* Field 8. Device address length                   */
		g_Class19_pass_p1,									/* Field 9. Password 1                              */
		&g_Class19_pass_p1_len,								/* Field 10. Password 1 length                      */
		g_Class19_pass_p2,									/* Field 11. Password 2                             */
		&g_Class19_pass_p2_len,								/* Field 12. Password 2 length                      */
		g_Class19_pass_w5,									/* Field 13. Password w5                            */
		&g_Class19_pass_w5_len,								/* Field 14. Password w5 length                     */
	},
};
const Unsigned16 g_ChildTableLengthClass19 = sizeof(g_ChildTableClass19) / sizeof(class19_child_record_t);
#endif

/****************************************************************************************
 * Class ID = 20, Activity calendar Child-table
 *
 * How to define a row of this table.
 *
 * Field 1. Fill the OBIS code first, 6 numbers (A, B, C, D, E, F)
 * Field 2. Specify which row on g_AccessRightTable will be used to declare the
 *          access right of the object.
 * Field 3.  Calendar name active
 * Field 4.  Calendar name active
 * Field 5.  Season profile active
 * Field 6.  Number of season profile active
 * Field 7.  Week profile active
 * Field 8.  Number of week profile active
 * Field 9.  Day profile active
 * Field 10. Number of day profile active
 * Field 11. Calendar name passive
 * Field 12. Calendar name passive
 * Field 13. Season profile passive
 * Field 14. Number of season profile passive
 * Field 15. Week profile passive
 * Field 16. Number of week profile passive
 * Field 17. Day profile passive
 * Field 18. Number of day profile passive
 * Field 19. Activate_calendar_passive
 *
****************************************************************************************/
#if (defined(USED_CLASS_20) && USED_CLASS_20 == 1)

/* Static parameters */
// each start_time table is 16 bytes

// stime_t is the truncated version of time_t - uses only hours and minutes

//-----------------------------------------------------------------------------------------------------------------
#if (defined(PVVNL_UNI_DIR_METER)) 
stime_t   TZ_start_time_active0[] =
{	{ 5,	0, }, { 6,  0, }, {	8,	0, },	{	11,	0, },	{	17,	0, },	{	18,	0, },	{22,	0, },	{23,	0, },};

stime_t   TZ_start_time_active1[] =
{	{ 5,	0, }, { 6,  0, }, {	8,	0, },	{	11,	0, },	{	17,	0, },	{	18,	0, },	{22,	0, },	{23,	0, },};

// this is for winter days - shared between working days and holidays
stime_t   TZ_start_time_active2[] =
{	{ 5,	0, }, { 6,  0, }, {	8,	0, },	{	11,	0, },	{	17,	0, },	{	18,	0, },	{22,	0, },	{23,	0, },};

stime_t   TZ_start_time_active3[] =
{	{ 5,	0, }, { 6,  0, }, {	8,	0, },	{	11,	0, },	{	17,	0, },	{	18,	0, },	{22,	0, },	{23,	0, },};

//this one is for summer - working day
		/*----------------------------------------------------------------------------------------*/
		/*  Start time          |  OBIS Code (A, B, C, D, E, F)    |  Script_identifier           */
		/*----------------------------------------------------------------------------------------*/
// the column for OBIS code has been removed - will be filled by code		
	day_profile_action_t	day_action_active0[] =
	{
		{&TZ_start_time_active0[0],		0x0005,},
		{&TZ_start_time_active0[1], 	0x0006,},
		{&TZ_start_time_active0[2],		0x0007,},
		{&TZ_start_time_active0[3],		0x0008,},
		{&TZ_start_time_active0[4],		0x0001,},
		{&TZ_start_time_active0[5],		0x0002,},
		{&TZ_start_time_active0[6],		0x0003,},
		{&TZ_start_time_active0[7],		0x0004,},
	};

//this one is for summer - holiday
		/*----------------------------------------------------------------------------------------*/
		/*  Start time          |  OBIS Code (A, B, C, D, E, F)    |  Script_identifier           */
		/*----------------------------------------------------------------------------------------*/
	day_profile_action_t	day_action_active1[] =
	{
		{&TZ_start_time_active1[0],		0x0005,},
		{&TZ_start_time_active1[1],		0x0006,},
		{&TZ_start_time_active1[2],		0x0007,},
		{&TZ_start_time_active1[3],		0x0008,},
		{&TZ_start_time_active1[4],		0x0001,},
		{&TZ_start_time_active1[5],		0x0002,},
		{&TZ_start_time_active1[6],		0x0003,},
		{&TZ_start_time_active1[7],		0x0004,},
	};


// this one is for winter	- working day
	day_profile_action_t	day_action_active2[] =
	{
		/*----------------------------------------------------------------------------------------*/
		/*  Start time          |  OBIS Code (A, B, C, D, E, F)    |  Script_identifier           */
		/*----------------------------------------------------------------------------------------*/
		{&TZ_start_time_active2[0],	0x0005,},
		{&TZ_start_time_active2[1],	0x0006,},
		{&TZ_start_time_active2[2],	0x0007,},
		{&TZ_start_time_active2[3],	0x0008,},
		{&TZ_start_time_active2[4],	0x0001,},
		{&TZ_start_time_active2[5],	0x0002,},
		{&TZ_start_time_active2[6],	0x0003,},
		{&TZ_start_time_active2[7],	0x0004,},
	};
	

// this one is for a winter	- holiday
	day_profile_action_t	day_action_active3[] =
	{
		/*----------------------------------------------------------------------------------------*/
		/*  Start time          |  OBIS Code (A, B, C, D, E, F)    |  Script_identifier           */
		/*----------------------------------------------------------------------------------------*/
		{&TZ_start_time_active3[0],	0x0005,},
		{&TZ_start_time_active3[1],	0x0006,},
		{&TZ_start_time_active3[2],	0x0007,},
		{&TZ_start_time_active3[3],	0x0008,},
		{&TZ_start_time_active3[4],	0x0001,},
		{&TZ_start_time_active3[5],	0x0002,},
		{&TZ_start_time_active3[6],	0x0003,},
		{&TZ_start_time_active3[7],	0x0004,},
	};	
#endif

//-----------------------------------------------------------------------------------------------------------------
#if (defined(GUJARAT_UNI_DIR_METER) || defined(GUJARAT_BI_DIR_METER)) 
stime_t   TZ_start_time_active0[] =
{	{ 0,	0, }, { 6,  0, }, {	7,	0, },	{	11,	0, },	{	18,	0, },	{	22,	0, },	{	0,	0, },	{	0,	0, },};

stime_t   TZ_start_time_active1[] =
{	{ 0,	0, },	{ 6,  0, },	{	7,	0, },	{	11,	0, },	{	18,	0, },	{	22,	0, },	{	0,	0, },	{	0,	0, },};

// this is for winter days - shared between working days and holidays
stime_t   TZ_start_time_active2[] =
{	{ 0,	0, },	{ 6,  0, },	{	7,	0, },	{	11,	0, },	{	18,	0, },	{	22,	0, },	{	0,	0, },	{	0,	0, },};

stime_t   TZ_start_time_active3[] =
{	{ 0,	0, },	{ 6,  0, },	{	7,	0, },	{	11,	0, },	{	18,	0, },	{	22,	0, },	{	0,	0, },	{	0,	0, },};

//this one is for summer - working day
		/*----------------------------------------------------------------------------------------*/
		/*  Start time          |  OBIS Code (A, B, C, D, E, F)    |  Script_identifier           */
		/*----------------------------------------------------------------------------------------*/
// the column for OBIS code has been removed - will be filled by code		
	day_profile_action_t	day_action_active0[] =
	{
		{&TZ_start_time_active0[0],		0x0002,},
		{&TZ_start_time_active0[1], 	0x0003,},
		{&TZ_start_time_active0[2],		0x0001,},
		{&TZ_start_time_active0[3],		0x0004,},
		{&TZ_start_time_active0[4],		0x0001,},
		{&TZ_start_time_active0[5],		0x0002,},
		{&TZ_start_time_active0[6],		0x0000,},
		{&TZ_start_time_active0[7],		0x0000,},
	};

//this one is for summer - holiday
		/*----------------------------------------------------------------------------------------*/
		/*  Start time          |  OBIS Code (A, B, C, D, E, F)    |  Script_identifier           */
		/*----------------------------------------------------------------------------------------*/
	day_profile_action_t	day_action_active1[] =
	{
		{&TZ_start_time_active1[0],		0x0002,},
		{&TZ_start_time_active1[1],		0x0003,},
		{&TZ_start_time_active1[2],		0x0001,},
		{&TZ_start_time_active1[3],		0x0004,},
		{&TZ_start_time_active1[4],		0x0001,},
		{&TZ_start_time_active1[5],		0x0002,},
		{&TZ_start_time_active1[6],		0x0000,},
		{&TZ_start_time_active1[7],		0x0000,},
	};


// this one is for winter	- working day
	day_profile_action_t	day_action_active2[] =
	{
		/*----------------------------------------------------------------------------------------*/
		/*  Start time          |  OBIS Code (A, B, C, D, E, F)    |  Script_identifier           */
		/*----------------------------------------------------------------------------------------*/
		{&TZ_start_time_active2[0],	0x0002,},
		{&TZ_start_time_active2[1],	0x0003,},
		{&TZ_start_time_active2[2],	0x0001,},
		{&TZ_start_time_active2[3],	0x0004,},
		{&TZ_start_time_active2[4],	0x0001,},
		{&TZ_start_time_active2[5],	0x0002,},
		{&TZ_start_time_active2[6],	0x0000,},
		{&TZ_start_time_active2[7],	0x0000,},
	};
	

// this one is for a winter	- holiday
	day_profile_action_t	day_action_active3[] =
	{
		/*----------------------------------------------------------------------------------------*/
		/*  Start time          |  OBIS Code (A, B, C, D, E, F)    |  Script_identifier           */
		/*----------------------------------------------------------------------------------------*/
		{&TZ_start_time_active3[0],	0x0002,},
		{&TZ_start_time_active3[1],	0x0003,},
		{&TZ_start_time_active3[2],	0x0001,},
		{&TZ_start_time_active3[3],	0x0004,},
		{&TZ_start_time_active3[4],	0x0001,},
		{&TZ_start_time_active3[5],	0x0002,},
		{&TZ_start_time_active3[6],	0x0000,},
		{&TZ_start_time_active3[7],	0x0000,},
	};	
#endif
// below this lince - code need not be changed for all cases	
//const day_profile_t		g_Class20_day_profile_active[] =
day_profile_t		g_Class20_day_profile_active[] =
{
	/*----------------------------------------------------------------------------------------*/
	/*  day id  | Pointer to day_profile_action_t |  number of day_profile_action_t   */
	/*----------------------------------------------------------------------------------------*/
	{	0x01,		day_action_active0, (sizeof(day_action_active0) / sizeof(day_profile_action_t))		},			/* The summer working day_profile */
	{	0x02,		day_action_active1,	(sizeof(day_action_active1) / sizeof(day_profile_action_t))		},			/* The summer holiday profile */
	{	0x03,		day_action_active2,	(sizeof(day_action_active2) / sizeof(day_profile_action_t))		},			/* The summer working day_profile */
	{	0x04,		day_action_active3,	(sizeof(day_action_active3) / sizeof(day_profile_action_t))		},			/* The summer holiday profile */
};

Unsigned8				g_Class20_week_name1[] = "summer_week";
Unsigned8				g_Class20_week_name2[] = "winter_week";

week_profile_t	g_Class20_week_profile_active[] =
{// in sequence from Monday to Sunday
	{	(Unsigned8*)g_Class20_week_name1,	obis_strlen(g_Class20_week_name1),
	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	},
	{	(Unsigned8*)g_Class20_week_name2,	obis_strlen(g_Class20_week_name2),
	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,},
};

date_time_t				g_Class20_start_time_active1 = {0x00, 0x00, 0x00, 0x00, NOT_SPECIFIED, 0x00, 
													0x00, 0x00, NOT_SPECIFIED, NOT_SPECIFIED, NOT_SPECIFIED, NOT_SPECIFIED };
date_time_t				g_Class20_start_time_active2 = {0x00, 0x00, 0x00, 0x00, NOT_SPECIFIED, 0x00, 
													0x00, 0x00, NOT_SPECIFIED, NOT_SPECIFIED, NOT_SPECIFIED, NOT_SPECIFIED };

Unsigned8				g_Class20_season_name1[] = "summer_ssn";
Unsigned8				g_Class20_season_name2[] = "winter_ssn";

season_profile_t	g_Class20_season_profile_active[] =
{
	{
	(Unsigned8*)g_Class20_season_name1,
	obis_strlen(g_Class20_season_name1),
	&g_Class20_start_time_active1,
	(Unsigned8*)g_Class20_week_name1,					/* The week_profile active in summer  */
	obis_strlen(g_Class20_week_name1),		/* The week_profile name len  */
	},
	{
	(Unsigned8*)g_Class20_season_name2,
	obis_strlen(g_Class20_season_name2),
	&g_Class20_start_time_active2,
	(Unsigned8*)g_Class20_week_name2,					/* The week_profile active in winter  */
	obis_strlen(g_Class20_week_name2),		/* The week_profile name len  */
	}
};


//------------------------------------------------------------------------------------------------------------------------
// a duplicate set is required for the passive case to allow change

// this is for summer days - shared between working days and holidays
stime_t   TZ_start_time_passive0[] =
{	{ 0,	0,},	{ 0,  0,},	{	0,	0,},	{	0,	0,},	{	0,	0,},	{	0,	0,},	{	0,	0,},	{	0,	0,},};

stime_t   TZ_start_time_passive1[] =
{	{ 0,	0,},	{ 0,  0,},	{	0,	0,},	{	0,	0,},	{	0,	0,},	{	0,	0,},	{	0,	0,},	{	0,	0,},};

// this is for winter days - shared between working days and holidays
stime_t   TZ_start_time_passive2[] =
{	{ 0,	0,},	{ 0,  0,},	{	0,	0,},	{	0,	0,},	{	0,	0,},	{	0,	0,},	{	0,	0,},	{	0,	0,},};

stime_t   TZ_start_time_passive3[] =
{	{ 0,	0,},	{ 0,  0,},	{	0,	0,},	{	0,	0,},	{	0,	0,},	{	0,	0,},	{	0,	0,},	{	0,	0,},};

//this one is for summer - working day
	day_profile_action_t	day_action_passive0[] =
	{
		/*----------------------------------------------------------------------------------------*/
		/*  Start time          |  OBIS Code (A, B, C, D, E, F)    |  Script_identifier           */
		/*----------------------------------------------------------------------------------------*/
		{&TZ_start_time_passive0[0],0x0000,},
		{&TZ_start_time_passive0[1],0x0000,},
		{&TZ_start_time_passive0[2],0x0000,},
		{&TZ_start_time_passive0[3],0x0000,},
		{&TZ_start_time_passive0[4],0x0000,},
		{&TZ_start_time_passive0[5],0x0000,},
		{&TZ_start_time_passive0[6],0x0000,},
		{&TZ_start_time_passive0[7],0x0000,},
	};

//this one is for summer - holiday
	day_profile_action_t	day_action_passive1[] =
	{
		/*----------------------------------------------------------------------------------------*/
		/*  Start time          |  OBIS Code (A, B, C, D, E, F)    |  Script_identifier           */
		/*----------------------------------------------------------------------------------------*/
		{&TZ_start_time_passive1[0],0x0000,},
		{&TZ_start_time_passive1[1],0x0000,},
		{&TZ_start_time_passive1[2],0x0000,},
		{&TZ_start_time_passive1[3],0x0000,},
		{&TZ_start_time_passive1[4],0x0000,},
		{&TZ_start_time_passive1[5],0x0000,},
		{&TZ_start_time_passive1[6],0x0000,},
		{&TZ_start_time_passive1[7],0x0000,},
	};


// this one is for winter	- working day
	day_profile_action_t	day_action_passive2[] =
	{
		/*----------------------------------------------------------------------------------------*/
		/*  Start time          |  OBIS Code (A, B, C, D, E, F)    |  Script_identifier           */
		/*----------------------------------------------------------------------------------------*/
		{&TZ_start_time_passive2[0],0x0000,},
		{&TZ_start_time_passive2[1],0x0000,},
		{&TZ_start_time_passive2[2],0x0000,},
		{&TZ_start_time_passive2[3],0x0000,},
		{&TZ_start_time_passive2[4],0x0000,},
		{&TZ_start_time_passive2[5],0x0000,},
		{&TZ_start_time_passive2[6],0x0000,},
		{&TZ_start_time_passive2[7],0x0000,},
	};
	

// this one is for a winter	- holiday
	day_profile_action_t	day_action_passive3[] =
	{
		/*----------------------------------------------------------------------------------------*/
		/*  Start time          |  OBIS Code (A, B, C, D, E, F)    |  Script_identifier           */
		/*----------------------------------------------------------------------------------------*/
		{&TZ_start_time_passive3[0],0x0000,},
		{&TZ_start_time_passive3[1],0x0000,},
		{&TZ_start_time_passive3[2],0x0000,},
		{&TZ_start_time_passive3[3],0x0000,},
		{&TZ_start_time_passive3[4],0x0000,},
		{&TZ_start_time_passive3[5],0x0000,},
		{&TZ_start_time_passive3[6],0x0000,},
		{&TZ_start_time_passive3[7],0x0000,},
	};	

//const day_profile_t		g_Class20_day_profile_passive[] =
day_profile_t		g_Class20_day_profile_passive[] =
{
	/*----------------------------------------------------------------------------------------*/
	/*  day id  | Pointer to day_profile_action_t |  number of day_profile_action_t   */
	/*----------------------------------------------------------------------------------------*/
	{	0x01,		day_action_passive0,					(sizeof(day_action_passive0) / sizeof(day_profile_action_t))		},			/* The summer working day_profile */
	{	0x02,		day_action_passive1,					(sizeof(day_action_passive1) / sizeof(day_profile_action_t))		},			/* The summer holiday profile */
	{	0x03,		day_action_passive2,					(sizeof(day_action_passive2) / sizeof(day_profile_action_t))		},			/* The summer working day_profile */
	{	0x04,		day_action_passive3,					(sizeof(day_action_passive3) / sizeof(day_profile_action_t))		},			/* The summer holiday profile */
};

week_profile_t	g_Class20_week_profile_passive[] =
{// one for each season
	{
	(Unsigned8*)g_Class20_week_name1,	obis_strlen(g_Class20_week_name1),
	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,
	},
	{
	(Unsigned8*)g_Class20_week_name2,	obis_strlen(g_Class20_week_name2),
	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,	0x01,
	},
};

date_time_t				g_Class20_start_time_passive1 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
													0x00, 0x00, NOT_SPECIFIED, NOT_SPECIFIED, NOT_SPECIFIED, NOT_SPECIFIED };
date_time_t				g_Class20_start_time_passive2 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
													0x00, 0x00, NOT_SPECIFIED, NOT_SPECIFIED, NOT_SPECIFIED, NOT_SPECIFIED };

//const season_profile_t	g_Class20_season_profile_passive[] =
season_profile_t	g_Class20_season_profile_passive[] =
{
	{
	(Unsigned8*)g_Class20_season_name1,
	obis_strlen(g_Class20_season_name1),
	&g_Class20_start_time_passive1,
	(Unsigned8*)g_Class20_week_name1,					/* The week_profile active in this season  */
	obis_strlen(g_Class20_week_name1),		/* The week_profile name len  */
	},
	{
	(Unsigned8*)g_Class20_season_name2,
	obis_strlen(g_Class20_season_name2),
	&g_Class20_start_time_passive2,
	(Unsigned8*)g_Class20_week_name2,					/* The week_profile active in this season  */
	obis_strlen(g_Class20_week_name2),		/* The week_profile name len  */
	}
};

/* Active calendar */
//const Unsigned8			g_Class20_calendar_active[10] = "calendar1\0";	// if this line is used then memcpy in R_OBIS_Class20_Activate_PassiveCalendar gives an error since the variable is a const
const Unsigned8			g_Class20_calendar_active[10] = "calendar1\0";
const Unsigned8			g_Class20_calendar_active_len = obis_strlen(g_Class20_calendar_active);

/* Passive calendar */
//const Unsigned8			g_Class20_calendar_passive[10] = "calendar2\0";
const Unsigned8			g_Class20_calendar_passive[10] = "calendar2\0";	// if this line is used then memcpy in R_OBIS_Class20_Activate_PassiveCalendar gives an error since the variable is a const
const Unsigned8			g_Class20_calendar_passive_len = obis_strlen(g_Class20_calendar_passive);

date_time_t				g_Class20_ac_time = {0,0,0,0,NOT_SPECIFIED,0,0,0,NOT_SPECIFIED,0x80,0x00,NOT_SPECIFIED};

const class20_child_record_t g_ChildTableClass20[] =
{
	/* Activity calendar */
	{
		{0, 0, 13, 0, 0, 255 },								/* Field 1.  Logical name (OBIS code) of the object.		*/
		(access_right_t *)g_AccessRightTable[7],								/* Field 2.  Access right definition for 1 object 		*/
		(Unsigned8*)g_Class20_calendar_active,							/* Field 3.  Calendar name active  				*/
		(Unsigned8 *)&g_Class20_calendar_active_len,							/* Field 4.  Calendar name active len				*/
		&g_Class20_season_profile_active[0],							/* Field 5.  Season profile active 				*/
		2,										/* Field 6.  Number of season profile active 			*/
		&g_Class20_week_profile_active[0],							/* Field 7.  Week profile active 				*/
		2,										/* Field 8.  Number of week profile active 			*/
		&g_Class20_day_profile_active[0],							/* Field 9.  Day profile active 				*/
		4,										/* Field 10. Number of day profile active 			*/
		(Unsigned8*)g_Class20_calendar_passive,							/* Field 11. Calendar name passive  				*/
		(Unsigned8 *)&g_Class20_calendar_passive_len,						/* Field 12. Calendar name passive len 				*/
		&g_Class20_season_profile_passive[0],							/* Field 13. Season profile passive 				*/
		2,										/* Field 14. Number of season profile passive 			*/
		&g_Class20_week_profile_passive[0],							/* Field 15. Week profile passive 				*/
		2,										/* Field 16. Number of week profile passive 			*/
		&g_Class20_day_profile_passive[0],							/* Field 17. Day profile passive 				*/
		4,										/* Field 18. Number of day profile passive			*/
		&g_Class20_ac_time								/* Field 19. Activate_calendar_passive 				*/
	}
};
const Unsigned16 g_ChildTableLengthClass20 = sizeof(g_ChildTableClass20) / sizeof(class20_child_record_t);
#endif

/****************************************************************************************
 * Class ID = 21, Register monitor Child-table
 *
 * How to define a row of this table.
 *
 * Field 1. Fill the OBIS code first, 6 numbers (A, B, C, D, E, F)
 * Field 2. Specify which row on g_AccessRightTable will be used to declare the
 *          access right of the object.
 * Field 3. Threshold values
 * Field 4. Number of threshold values
 * Field 5. Monitor value
 * Field 6. Array of Action
 * Field 7. Number of Action
 *
****************************************************************************************/
#if (defined(USED_CLASS_21) && USED_CLASS_21 == 1)

/* Static parameters */

/* Thresholds definition */
const Unsigned16 g_Class21_threshold1 = 8;

const value_t g_Class21_threshold_list[] =
{
	VALUE(CHOICE_LONG_UNSIGNED		, &g_Class21_threshold1),
};

/* Monitored value definition */

const value_definition_t g_Class21_monitor =
{ /* NumberPowerFailure */
	1,									/* class id */
	{   0,   0,  96,   7,   0, 255 },	/* logical_name */
	2,									/* index */
};

/* Actions definition */

const action_item_t g_Class21_action_item[] =
{
	/*----------------------------------------------------*/
	/*  �Script table� logical name  | Script_identifier  */
	/*----------------------------------------------------*/
	{{   0,   0,   10,   0,   1, 255 }, 0x0002,},
	{{   0,   0,   10,   0,   1, 255 }, 0x0003,},
};

const action_set_t g_Class21_action_list[] =
{
	/*--------------------------------------------*/
	/*  Action up             | Action down       */
	/*--------------------------------------------*/
	{&g_Class21_action_item[0],	&g_Class21_action_item[1]},
};

const class21_child_record_t g_ChildTableClass21[] =
{
	/* Abstract Special days table object*/
	{
		{0, 0, 16, 0, 0, 255 },												/* Field 1. Logical name (OBIS code) of the object. */
		g_AccessRightTable[0],												/* Field 2. Access right definition for 1 object    */
		g_Class21_threshold_list,											/* Field 3. Threshold values                        */
		sizeof(g_Class21_threshold_list) / sizeof(value_t),					/* Field 4. Number of threshold values              */
		&g_Class21_monitor,													/* Field 5. Monitor value                           */
		g_Class21_action_list,												/* Field 6. Array of Action                         */
		sizeof(g_Class21_action_list) / sizeof(action_set_t),				/* Field 7. Number of Action                        */
	},
};
const Unsigned16 g_ChildTableLengthClass21 = sizeof(g_ChildTableClass21) / sizeof(class21_child_record_t);
#endif


/****************************************************************************************
 * Class ID = 22, Single action schedule Child-table
 *
 * How to define a row of this table.
 *
 * Field 1. Fill the OBIS code first, 6 numbers (A, B, C, D, E, F)
 * Field 2. Specify which row on g_AccessRightTable will be used to declare the
 *          access right of the object.
 * Field 3. Default mode
 * Field 4. Default baud rate
 * Field 5. baud rate proposed by the meter
 * Field 6. Respond time
 * Field 7. Device address
 * Field 8. Password 1
 * Field 9. Password 2
 * Field 10. Password w5
 *
****************************************************************************************/
#if (defined(USED_CLASS_22) && USED_CLASS_22 == 1)

/* Static parameters */
const action_item_t g_Class22_billscript =
{
	/*--------------------------------------------------------------------*/
	/*  logical name of the �Script table� object | Script_identifier     */
	/*--------------------------------------------------------------------*/
	{   0,   0,   10,   0,   1, 255 },             0x0001,
};

///* date_t format */
date_t g_Class22_billdate =
{
	0xFF,		/* year_high */
	0xFF,				/* year_low */
	0xFF,					/* month */
	1,											/* day_of_month */
	0xFF						/* day_of_week */
};

///* time_t format */
 time_t g_Class22_billtime =
{
	0,											/* hour */
	0,											/* minute */
	0,											/* second */
	TIME_HUNDREDTHS_NOT_SPECIFIED				/* Hundredths */
};

const execution_time_date_t g_Class22_bill_exetime[] =
{
	{
		&g_Class22_billtime,
		&g_Class22_billdate,
	},
};

const single_action_t g_Class22_billtype = SINGLE_ACTION_1_TIME_WILDCARD;
//const single_action_t g_Class22_billtype = SINGLE_ACTION_N_TIME;


const class22_child_record_t g_ChildTableClass22[] =
{
	/* End of billing period */
	{
		{0, 0, 15, 0, 0, 255 },											/* Field 1. Logical name (OBIS code) of the object. */
		(access_right_t *)g_AccessRightTable[8],											/* Field 2. Access right definition for 1 object    */
		(action_item_t *)&g_Class22_billscript,											/* Field 3. Executed script                         */
		(single_action_t *)&g_Class22_billtype,											/* Field 4. Type                                    */
		(execution_time_date_t *)g_Class22_bill_exetime,											/* Field 4. Execution time                          */
		sizeof(g_Class22_bill_exetime)/sizeof(execution_time_date_t),	/* Field 4. Number of execution time                */
	},
};
const Unsigned16 g_ChildTableLengthClass22 = sizeof(g_ChildTableClass22) / sizeof(class22_child_record_t);
#endif

/****************************************************************************************
 * Class ID = 23, IEC HDLC setup Child-table
 *
 * How to define a row of this table.
 *
 * Field 1. Fill the OBIS code first, 6 numbers (A, B, C, D, E, F)
 * Field 2. Specify which row on g_AccessRightTable will be used to declare the
 *          access right of the object.
 * Field 3. Communication speed
 * Field 4. Window size transmit
 * Field 5. Window size receive
 * Field 6. Max info field length transmit speed
 * Field 7. Max info field length receive speed
 * Field 8. Inter_octet_time_out
 * Field 9. Inactivity_time_out
 * Field 10. Device address
 *
****************************************************************************************/
#if (defined(USED_CLASS_23) && USED_CLASS_23 == 1)

/* Static parameters */
const Unsigned16		g_Class23_device_addr = 0x0100;

/* Variable parameters */
const baud_rate_t		g_Class23_comm_speed = (baud_rate_t)OBIS_BAUD_CONVERT(BAUD_RATE_9600);
const Unsigned8			g_Class23_window_size_transmit = 1;
const Unsigned8			g_Class23_window_size_receive = 1;
const Unsigned16		g_Class23_max_info_field_length_transmit = MAX_SERVER_BUFFER_SIZE;
const Unsigned16		g_Class23_max_info_field_length_receive = MAX_SERVER_BUFFER_SIZE;
const Unsigned16		g_Class23_inter_octet_time_out = 100;
const Unsigned16		g_Class23_inactivity_time_out = 5000;

const class23_child_record_t g_ChildTableClass23[] =
{
	/* IEC HDLC setup objects */
	{
		{0, 0, 22, 0, 0, 255 },							/* Field 1. Logical name (OBIS code) of the object. */
		(access_right_t *)g_AccessRightTable[0],							/* Field 2. Access right definition for 1 object    */
		(baud_rate_t *)&g_Class23_comm_speed,							/* Field 3. Communication speed                     */
		(Unsigned8 *)&g_Class23_window_size_transmit,				/* Field 4. Window size transmit                    */
		(Unsigned8 *)&g_Class23_window_size_receive,					/* Field 5. Window size receive                     */
		(Unsigned16 *)&g_Class23_max_info_field_length_transmit,		/* Field 6. Max info field length transmit speed    */
		(Unsigned16 *)&g_Class23_max_info_field_length_receive,		/* Field 7. Max info field length receive speed     */
		(Unsigned16 *)&g_Class23_inter_octet_time_out,				/* Field 8. Inter_octet_time_out                    */
		(Unsigned16 *)&g_Class23_inactivity_time_out,					/* Field 9. Inactivity_time_out                     */
		(Unsigned16 *)&g_Class23_device_addr,							/* Field 10. Device address                          */
	},
};
const Unsigned16 g_ChildTableLengthClass23 = sizeof(g_ChildTableClass23) / sizeof(class23_child_record_t);
#endif

/****************************************************************************************
 * Class ID = 24, IEC HDLC setup Child-table
 *
 * How to define a row of this table.
 *
 * Field 1. Fill the OBIS code first, 6 numbers (A, B, C, D, E, F)
 * Field 2. Specify which row on g_AccessRightTable will be used to declare the
 *          access right of the object.
 * Field 3. Secondary address
 * Field 4. Fatal error
 * Field 5. Mode
 * Field 6. Comm speed
 * Field 7. Primary address list
 * Field 8. Primary address list number
 * Field 9. TAB(i) list
 * Field 10. TAB(i) list number

 *
****************************************************************************************/
#if (defined(USED_CLASS_24) && USED_CLASS_24 == 1)
/* Static parameters */
/* Variable parameters */
const iec_working_mode_t	g_Class24_mode = IEC_INACTIVE;
const baud_rate_t			g_Class24_comm_speed = OBIS_BAUD_CONVERT(HDLC_BAUDRATE);

Unsigned8					g_Class24_address_list[]= {0x01, 0x02};
Unsigned8					g_Class24_address_list_len = obis_strlen(g_Class24_address_list);

Integer8					g_Class24_tabi_list[]= {-1, 0, 1};
Unsigned8					g_Class24_tabi_list_len = obis_strlen(g_Class24_tabi_list);

fatal_error_t		g_Class24_error = ERROR_NONE;

const class24_child_record_t g_ChildTableClass24[] =
{
	/* IEC twisted pair (1) setup */
	{
		{0, 0, 23, 0, 0, 255 },											/* Field 1. Logical name (OBIS code) of the object. */
		g_AccessRightTable[0],											/* Field 2. Access right definition for 1 object    */
		/* For version 0*/
		{0, 1, 23, 0, 0, 255 },											/* Field 3. Secondary address                          */
		DYN_VALUE(CHOICE_ENUM, &g_Class24_error, ATTR_ACCESS_MEMORY),	/* Field 4. Fatal error                                */
		/* For version 1*/
		&g_Class24_mode,												/* Field 5. Mode                                    */
		&g_Class24_comm_speed,											/* Field 6. Communication speed                     */
		/* Common */
		g_Class24_address_list,											/* Field 7. Primary address list                       */
		&g_Class24_address_list_len,									/* Field 8. Primary address list number                */
		g_Class24_tabi_list,											/* Field 9. TAB(i) list                                */
		&g_Class24_tabi_list_len,										/* Field 10. TAB(i) list number                         */

	},
};
const Unsigned16 g_ChildTableLengthClass24 = sizeof(g_ChildTableClass24) / sizeof(class24_child_record_t);
#endif


/****************************************************************************************
 * Class ID = 26, Utility tables Child-table
 *
 * How to define a row of this table.
 *
 * Field 1. Fill the OBIS code first, 6 numbers (A, B, C, D, E, F)
 * Field 2. Specify which row on g_AccessRightTable will be used to declare the
 *          access right of the object.
 * Field 3. Table ID
 * Field 4. Length
 * Field 5. Buffer
 *
****************************************************************************************/
#if (defined(USED_CLASS_26) && USED_CLASS_26 == 1)


const Unsigned8		g_Class26_Obj0_Buffer[] = {
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
};
const Unsigned32	g_Class26_Obj0_Length = sizeof(g_Class26_Obj0_Buffer);

Unsigned16 g_class26_Obj0_current_entries;

buffer_t g_class26_Obj0_Buf = {
	g_Class26_Obj0_Buffer,						/* Buffer of value                                     */
	NULL,										/* Pointer to access method option                     */
	&g_class26_Obj0_current_entries,			/* Pointer to index of current entries in buffer       */
	0,											/* Number of entries per block to transfer             */
	2,											/* Length of encode 1 entry after filter               */
	0,											/* Number of remain entries in run-time                */
	0,											/* First entry to retrieve in run-time                 */
	0,											/* Last entry to retrieve in run-time                  */
	0,											/* Index of first value to retrieve in run-time        */
	0,											/* Index of last value to retrieve  in run-time        */
};

const class26_child_record_t g_ChildTableClass26[] =
{
	/* Standard tables */
	{
		{0, 0, 65, 0, 0, 255 },							/* Field 1. Logical name (OBIS code) of the object. */
		g_AccessRightTable[0],							/* Field 2. Access right definition for 1 object    */
		1,												/* Field 3. Table ID                                */
		&g_Class26_Obj0_Length,							/* Field 4. Length                                  */
		&g_class26_Obj0_Buf,							/* Field 5. Buffer                                  */
	},
};
const Unsigned16 g_ChildTableLengthClass26 = sizeof(g_ChildTableClass26) / sizeof(class26_child_record_t);
#endif

/****************************************************************************************
 * Class ID = 27, Modem configuration Child-table
 *
 * How to define a row of this table.
 *
 * Field 1. Fill the OBIS code first, 6 numbers (A, B, C, D, E, F)
 * Field 2. Specify which row on g_AccessRightTable will be used to declare the
 *          access right of the object.
 * Field 3. Communication speed
 * Field 4. Initialization_string
 * Field 5. Initialization_string number
 * Field 6. modem profile
 * Field 7. modem profile size
 *
****************************************************************************************/
#if (defined(USED_CLASS_27) && USED_CLASS_27 == 1)
/* Static parameters */
const baud_rate_t			g_Class27_default_baud = BAUD_9600;

const Unsigned8 g_Class27_modem_command0[]  = "OK";
const Unsigned8 g_Class27_modem_command1[]  = "CONNECT";
const Unsigned8 g_Class27_modem_command2[]  = "RING";
const Unsigned8 g_Class27_modem_command3[]  = "NO CARRIER";
const Unsigned8 g_Class27_modem_command4[]  = "ERROR";
const Unsigned8 g_Class27_modem_command5[]  = "CONNECT 1 200";
const Unsigned8 g_Class27_modem_command6[]  = "NO DIAL TONE";
const Unsigned8 g_Class27_modem_command7[]  = "BUSY";
const Unsigned8 g_Class27_modem_command8[]  = "NO ANSWER";
const Unsigned8 g_Class27_modem_command9[]  = "CONNECT 600";
const Unsigned8 g_Class27_modem_command10[] = "CONNECT 2 400";
const Unsigned8 g_Class27_modem_command11[] = "CONNECT 4 800";
const Unsigned8 g_Class27_modem_command12[] = "CONNECT 9 600";
const Unsigned8 g_Class27_modem_command13[] = "CONNECT 14 400";
const Unsigned8 g_Class27_modem_command14[] = "CONNECT 28 800";
const Unsigned8 g_Class27_modem_command15[] = "CONNECT 33 600";
const Unsigned8 g_Class27_modem_command16[] = "CONNECT 56 000";

/* Variable parameters */
Unsigned16						g_Class27_init_string_delay_response;
init_string_element_t g_Class27_init_string[] = {
	/* init_string 1*/
	{
		g_Class27_modem_command1,					/*  request              */
		obis_strlen(g_Class27_modem_command1),		/*  size of request      */
		g_Class27_modem_command0,					/*  response             */
		obis_strlen(g_Class27_modem_command0),		/*  size of response     */
		&g_Class27_init_string_delay_response,		/*  delay_after_response */
	},
} ;
Unsigned8 g_Class27_init_string_nr = sizeof(g_Class27_init_string)/sizeof(init_string_element_t);

value_t g_Class27_modem_profile[] ={
	VALUE(CHOICE_OCTET_STRING(obis_strlen(g_Class27_modem_command0 )), g_Class27_modem_command0 ),
	VALUE(CHOICE_OCTET_STRING(obis_strlen(g_Class27_modem_command1 )), g_Class27_modem_command1 ),
	VALUE(CHOICE_OCTET_STRING(obis_strlen(g_Class27_modem_command2 )), g_Class27_modem_command2 ),
	VALUE(CHOICE_OCTET_STRING(obis_strlen(g_Class27_modem_command3 )), g_Class27_modem_command3 ),
	VALUE(CHOICE_OCTET_STRING(obis_strlen(g_Class27_modem_command4 )), g_Class27_modem_command4 ),
	VALUE(CHOICE_OCTET_STRING(obis_strlen(g_Class27_modem_command6 )), g_Class27_modem_command6 ),
	VALUE(CHOICE_OCTET_STRING(obis_strlen(g_Class27_modem_command7 )), g_Class27_modem_command7 ),
	VALUE(CHOICE_OCTET_STRING(obis_strlen(g_Class27_modem_command8 )), g_Class27_modem_command8 ),
};
Unsigned8 g_Class27_modem_profile_nr = sizeof(g_Class27_modem_profile)/sizeof(value_t);


const class27_child_record_t g_ChildTableClass27[] =
{
	/* Modem configuration */
	{
		{0, 0, 2, 0, 0, 255 },											/* Field 1. Logical name (OBIS code) of the object. */
		g_AccessRightTable[0],											/* Field 2. Access right definition for 1 object    */
		&g_Class27_default_baud,										/* Field 3. Communication speed                     */
		g_Class27_init_string,											/* Field 4. Initialization_string                   */
		&g_Class27_init_string_nr,										/* Field 5. Initialization_string number            */
		g_Class27_modem_profile,										/* Field 6. modem profile                           */
		&g_Class27_modem_profile_nr,									/* Field 7. modem profile size                      */
	},
};
const Unsigned16 g_ChildTableLengthClass27 = sizeof(g_ChildTableClass27) / sizeof(class27_child_record_t);
#endif

/****************************************************************************************
 * Class ID = 28, Auto answer Child-table
 *
 * How to define a row of this table.
 *
 * Field 1. Fill the OBIS code first, 6 numbers (A, B, C, D, E, F)
 * Field 2. Specify which row on g_AccessRightTable will be used to declare the
 *          access right of the object.
 * Field 3. Mode
 * Field 4. Listen window
 * Field 5. Listen window number
 * Field 6. Status
 * Field 7. Number of calls
 * Field 8. Number of rings
 * Field 9. List of allowed callers
 * Field 10. Allowed callers number
 *
****************************************************************************************/
#if (defined(USED_CLASS_28) && USED_CLASS_28 == 1)
/* Static parameters */
const working_mode_t			g_Class28_mode = WORK_MODE_DEDICATED;

/* Variable parameters */
date_time_t						g_Class28_start_time;
date_time_t						g_Class28_end_time;

window_element_t				g_Class28_listen_window[] = {
	{
		&g_Class28_start_time,		/* start time */
		&g_Class28_end_time,		/* end time   */
	},
};

Unsigned8						g_Class28_nr_listen_window = sizeof(g_Class28_listen_window)/sizeof(window_element_t);
windows_status_t				g_Class28_windows_status = WINDOWS_INACTIVE;
Unsigned8						g_Class28_call_nr;
nr_rings_t						g_Class28_ring_nr;

Unsigned8					g_Class28_call_id1[] = "*9000";
allowed_callers_element_t		g_Class28_callerlist[] = {
	{
		g_Class28_call_id1,				/* caller_id        */
		sizeof(g_Class28_call_id1),		/* caller_id length */
		CALL_NORMAL					/* call_type */
	},
};
Unsigned8						g_Class28_callerlist_nr = sizeof(g_Class28_callerlist)/sizeof(allowed_callers_element_t);

const class28_child_record_t g_ChildTableClass28[] =
{
	/* Auto answer */
	{
		{0, 0, 2, 2, 0, 255 },													/* Field 1. Logical name (OBIS code) of the object. */
		g_AccessRightTable[0],													/* Field 2. Access right definition for 1 object    */
		&g_Class28_mode,														/* Field 3. Mode                                    */
		g_Class28_listen_window,												/* Field 4. Listen window                           */
		&g_Class28_nr_listen_window,											/* Field 5. Listen window number                    */
		DYN_VALUE(CHOICE_ENUM, &g_Class28_windows_status, ATTR_ACCESS_MEMORY),	/* Field 6. Status                                  */
		&g_Class28_call_nr,														/* Field 7. Number of calls                         */
		&g_Class28_ring_nr,														/* Field 8. Number of rings                         */
		g_Class28_callerlist,													/* Field 9. List of allowed callers                 */
		&g_Class28_callerlist_nr,												/* Field 10. Allowed callers number                 */
	},
};
const Unsigned16 g_ChildTableLengthClass28 = sizeof(g_ChildTableClass28) / sizeof(class28_child_record_t);
#endif

/****************************************************************************************
 * Class ID = 29, Auto connect Child-table
 *
 * How to define a row of this table.
 *
 * Field 1. Fill the OBIS code first, 6 numbers (A, B, C, D, E, F)
 * Field 2. Specify which row on g_AccessRightTable will be used to declare the
 *          access right of the object.
 * Field 3. Mode
 * Field 4. Repetitions
 * Field 5. Repetitions delay
 * Field 6. Calling window
 * Field 7. Calling window number
 * Field 8. Destination list
 * Field 9. Destination list number
 *
****************************************************************************************/
#if (defined(USED_CLASS_29) && USED_CLASS_29 == 1)
/* Static parameters */
const dial_mode_t				g_Class29_mode = DIAL_MODE_NO_AUTO;

/* Variable parameters */
Unsigned8						g_Class29_repetitions;
Unsigned16						g_Class29_repetition_delay;

date_time_t						g_Class29_start_time;
date_time_t						g_Class29_end_time;

window_element_t				g_Class29_calling_window = {
	&g_Class29_start_time,		/* start time */
	&g_Class29_end_time,		/* end time   */
};

Unsigned8						g_Class29_nr_calling_window;

Unsigned8						g_Class29_dest0[] = "DEST 0";
Unsigned8						g_Class29_dest1[] = "DEST 1";
value_t g_Class29_destination_list[] ={
	VALUE(CHOICE_OCTET_STRING(obis_strlen(g_Class29_dest0 )), g_Class29_dest0 ),
	VALUE(CHOICE_OCTET_STRING(obis_strlen(g_Class29_dest1 )), g_Class29_dest1 ),
};
Unsigned8 g_Class29_destination_list_nr = sizeof(g_Class29_destination_list)/sizeof(value_t);



const class29_child_record_t g_ChildTableClass29[] =
{
	/* Auto connect */
	{
		{0, 0, 2, 1, 0, 255 },													/* Field 1. Logical name (OBIS code) of the object. */
		g_AccessRightTable[0],													/* Field 2. Access right definition for 1 object    */
		&g_Class29_mode,														/* Field 3. Mode                                    */
		&g_Class29_repetitions,													/* Field 4. Repetitions                             */
		&g_Class29_repetition_delay,											/* Field 5. Repetitions delay                       */
		&g_Class29_calling_window,												/* Field 6. Calling window                          */
		&g_Class29_nr_calling_window,											/* Field 7. Calling window number                   */
		g_Class29_destination_list,												/* Field 8. Destination list                        */
		&g_Class29_destination_list_nr,											/* Field 9. Destination list number                 */
	},
};
const Unsigned16 g_ChildTableLengthClass29 = sizeof(g_ChildTableClass29) / sizeof(class29_child_record_t);
#endif

/****************************************************************************************
 * Class ID = 41, TCP UDP setup Child-table
 *
 * How to define a row of this table.
 *
 * Field 1. Fill the OBIS code first, 6 numbers (A, B, C, D, E, F)
 * Field 2. Specify which row on g_AccessRightTable will be used to declare the
 *          access right of the object.
 * Field 3. TCP UDP port
 * Field 4. IP reference
 * Field 5. MSS
 * Field 6. Number of sim conn
 * Field 7. Inactivity time out
 *
****************************************************************************************/
#if (defined(USED_CLASS_41) && USED_CLASS_41 == 1)
/* Static parameters */

/* Variable parameters */
Unsigned16			g_Class41_tcp_udp_port = 1;
Unsigned16			g_Class41_mss = 576;
Unsigned8			g_Class41_nb_of_sim_conn = 1;
Unsigned16			g_Class41_inactivity_time_out = 180;

const class41_child_record_t g_ChildTableClass41[] =
{
	/* TCP UDP setup */
	{
		{0, 0, 25, 0, 0, 255 },													/* Field 1. Logical name (OBIS code) of the object. */
		g_AccessRightTable[0],													/* Field 2. Access right definition for 1 object    */
		&g_Class41_tcp_udp_port,												/* Field 3. TCP UDP port                            */
		{0, 0, 25, 1, 0, 255 },													/* Field 4. IP reference                            */
		&g_Class41_mss,															/* Field 5. Maximum Segment Size                    */
		&g_Class41_nb_of_sim_conn,												/* Field 6. Number of sim conn                      */
		&g_Class41_inactivity_time_out,											/* Field 7. Inactivity time out                     */
	},
};
const Unsigned16 g_ChildTableLengthClass41 = sizeof(g_ChildTableClass41) / sizeof(class41_child_record_t);
#endif

/****************************************************************************************
 * Class ID = 61, Register table Child-table
 *
 * How to define a row of this table.
 *
 * Field 1. Fill the OBIS code first, 6 numbers (A, B, C, D, E, F)
 * Field 2. Specify which row on g_AccessRightTable will be used to declare the
 *          access right of the object.
 * Field 3. Table cell values
 * Field 4. Table cell number
 * Field 5. Table cell definition
 * Field 6. Scaler unit
 *
****************************************************************************************/
#if (defined(USED_CLASS_61) && USED_CLASS_61 == 1)

/* table cell definition type */

const Unsigned8	g_Class61_value1[] = "1";
const Unsigned8	g_Class61_value2[] = "2";
const Unsigned8	g_Class61_value3[] = "3";


dyn_value_t g_Class61_table_cell_values[] = {
	DYN_VALUE(CHOICE_OCTET_STRING(obis_strlen(g_Class61_value1)), g_Class61_value1, ATTR_ACCESS_MEMORY),
	DYN_VALUE(CHOICE_OCTET_STRING(obis_strlen(g_Class61_value2)), g_Class61_value2, ATTR_ACCESS_MEMORY),
	DYN_VALUE(CHOICE_OCTET_STRING(obis_strlen(g_Class61_value3)), g_Class61_value3, ATTR_ACCESS_MEMORY),
};

Unsigned8 g_Class61_table_cell_values_nr = sizeof(g_Class61_table_cell_values)/sizeof(dyn_value_t);

Unsigned8 g_Class61_group_E[] = {0, 1, 2};
table_cell_def_t g_Class61_table_cell = {
	4,										/* Class ID          */
	{   1,   0,   1,   6,   255, 255 },		/* The logical name  */
	g_Class61_group_E,						/* group_E_values    */
	sizeof(g_Class61_group_E),				/* number of group_E */
	2,										/* attribute_index   */
};

const class61_child_record_t g_ChildTableClass61[] =
{
	/* General use */
	{
		{1, 0, 98, 10, 0, 0 },										/* Field 1. Logical name (OBIS code) of the object. */
		g_AccessRightTable[0],										/* Field 2. Access right definition for 1 object    */
		g_Class61_table_cell_values,								/* Field 3. Table cell values                       */
		&g_Class61_table_cell_values_nr,							/* Field 4. Table cell number                       */
		&g_Class61_table_cell,										/* Field 5. Table cell definition                   */
		{0, PHY_UNIT_WATT_HOUR		},								/* Field 5. Scaler unit                             */
	},
};
const Unsigned16 g_ChildTableLengthClass61 = sizeof(g_ChildTableClass61) / sizeof(class61_child_record_t);
#endif

/****************************************************************************************
 * Class ID = 63, Status mapping Child-table
 *
 * How to define a row of this table.
 *
 * Field 1. Fill the OBIS code first, 6 numbers (A, B, C, D, E, F)
 * Field 2. Specify which row on g_AccessRightTable will be used to declare the
 *          access right of the object.
 * Field 3. Status word
 * Field 4. Mapping table
 *
****************************************************************************************/
#if (defined(USED_CLASS_63) && USED_CLASS_63 == 1)

/* table cell definition type */

const Unsigned8	g_Class63_status[] = "FF";


Unsigned16 g_Class63_entry_list[] = {1,2,4};
mapping_table_t g_Class63_mapping_table = {
	0x01,															/* ref_table_id                           */
	g_Class63_entry_list,											/* first_entry or table_entries           */
	sizeof(g_Class63_entry_list)/ sizeof(Unsigned16),				/* number of entries, 1 for first entry   */
};

const class63_child_record_t g_ChildTableClass63[] =
{
	/* Manufacturer specific */
	{
		{0, 0, 96, 50, 255, 255 },																					/* Field 1. Logical name (OBIS code) of the object. */
		g_AccessRightTable[0],																						/* Field 2. Access right definition for 1 object    */
		DYN_VALUE(CHOICE_OCTET_STRING(obis_strlen(g_Class63_status)), g_Class63_status, ATTR_ACCESS_MEMORY),		/* Field 3. Status word                          */
		&g_Class63_mapping_table,																					/* Field 4. Mapping table                        */
	},
};
const Unsigned16 g_ChildTableLengthClass63 = sizeof(g_ChildTableClass63) / sizeof(class63_child_record_t);
#endif

/****************************************************************************************
 * Class ID = 64, Security setup Child-table
 *
 * How to define a row of this table.
 *
 * Field 1. Fill the OBIS code first, 6 numbers (A, B, C, D, E, F)
 * Field 2. Specify which row on g_AccessRightTable will be used to declare the
 *          access right of the object.
 * Field 3. Select the value of security_policy.
 * Field 4. Select the value of security_suite.
 * Field 5. client_system_title.
 * Field 6. server_system_title.
 * Field 7. master_key.
 * Field 8. global unicast key.
 * Field 9. global broadcast key.
 * Field 10. dedicated unicast key.
 * Field 11. authentication key.
 * Field 12. frame counter.
 *
****************************************************************************************/
#if (defined(USED_CLASS_64) && USED_CLASS_64 == 1)

/*
 * Object 0
 */

/* Will be updated from client */
Unsigned8				g_Class64_obj0_client_sys_title[8];

/* Security policy indicate how strengh the security will be applied to the message */
security_policy_t		g_Class64_obj0_security_policy = SECURITY_NONE;

/* Keys for cipher/decipher on ciphering context */
Unsigned8				g_Class64_obj0_global_unicast_key[16] =  "Renesas012345678";
Unsigned8				g_Class64_obj0_global_broadcast_key[16] = "PASSPASS12345678";
Unsigned8				g_Class64_obj0_dedicated_unicast_key[16];
Unsigned8				g_Class64_obj0_authentication_key[16] = "Authen0123456789";
Unsigned32				g_Class64_obj0_frame_counter = 0;
Unsigned8				g_Class64_obj0_server_sys_title[8] = "RES01234";
security_suit_t			g_Class64_obj0_security_suit = SECURITY_AES_SUIT;

/* Static parameters */
const Unsigned8			g_Class64_obj0_master_key[16] = "RES0123456789SER";

/*
 * Object 1
 */
 
/* Will be updated from client */
Unsigned8				g_Class64_obj1_client_sys_title[8];

/* Security policy indicate how strengh the security will be applied to the message */
security_policy_t		g_Class64_obj1_security_policy = SECURITY_NONE;

/* Keys for cipher/decipher on ciphering context */
Unsigned8				g_Class64_obj1_global_unicast_key[16] = "Renesas012345678";
Unsigned8				g_Class64_obj1_global_broadcast_key[16] = "PASSPASS12345678";
Unsigned8				g_Class64_obj1_dedicated_unicast_key[16];
Unsigned8				g_Class64_obj1_authentication_key[16] = "Authen0123456789";
Unsigned32				g_Class64_obj1_frame_counter = 0;
Unsigned8				g_Class64_obj1_server_sys_title[8] = "RES01234";
security_suit_t			g_Class64_obj1_security_suit = SECURITY_AES_SUIT;

/* Static parameters */
const Unsigned8			g_Class64_obj1_master_key[16] = "RES0123456789SER";

const class64_child_record_t g_ChildTableClass64[] =
{
	/* security setup */
	{
		{0, 0, 43, 0, 0, 255 },					/* Field 1. Logical name (OBIS code) of the object. */
		g_AccessRightTable[1],					/* Field 2. Access right definition for 1 object    */
		&g_Class64_obj0_security_policy,		/* Field 3. security_policy.                        */
		&g_Class64_obj0_security_suit,			/* Field 4. security_suite.                         */
		g_Class64_obj0_client_sys_title,		/* Field 5. client_system_title.                    */
		g_Class64_obj0_server_sys_title,		/* Field 6. server_system_title.                    */
		
		/* Following is the keys for ciphering context, can be changed from client */
		g_Class64_obj0_master_key,				/* Field 7. global unicast key                      */
		g_Class64_obj0_global_unicast_key,		/* Field 8. global unicast key                      */
		g_Class64_obj0_global_broadcast_key,	/* Field 9. global broadcast key                    */
		g_Class64_obj0_dedicated_unicast_key,	/* Field 10. dedicated unicast key                  */
		g_Class64_obj0_authentication_key,		/* Field 11. authentication key                     */
		&g_Class64_obj0_frame_counter,			/* Field 12. frame counter started from 0           */
	},
	
	/* security setup */
	{
		{0, 0, 43, 0, 1, 255 },					/* Field 1. Logical name (OBIS code) of the object. */
		g_AccessRightTable[1],					/* Field 2. Access right definition for 1 object    */
		&g_Class64_obj1_security_policy,		/* Field 3. security_policy.                        */
		&g_Class64_obj1_security_suit,			/* Field 4. security_suite.                         */
		g_Class64_obj1_client_sys_title,		/* Field 5. client_system_title.                    */
		g_Class64_obj1_server_sys_title,		/* Field 6. server_system_title.                    */
		
		/* Following is the keys for ciphering context, can be changed from client */
		g_Class64_obj1_master_key,				/* Field 7. global unicast key                      */
		g_Class64_obj1_global_unicast_key,		/* Field 8. global unicast key                      */
		g_Class64_obj1_global_broadcast_key,	/* Field 9. global broadcast key                    */
		g_Class64_obj1_dedicated_unicast_key,	/* Field 10. dedicated unicast key                  */
		g_Class64_obj1_authentication_key,		/* Field 11. authentication key                     */
		&g_Class64_obj1_frame_counter,			/* Field 12. frame counter started from 0           */
	},
};
const Unsigned16 g_ChildTableLengthClass64 = sizeof(g_ChildTableClass64) / sizeof(class64_child_record_t);
#endif

/****************************************************************************************
 * Class ID = 70, Disconnect control Child-table
 *
 * How to define a row of this table.
 *
 * Field 1. Fill the OBIS code first, 6 numbers (A, B, C, D, E, F)
 * Field 2. Specify which row on g_AccessRightTable will be used to declare the
 *          access right of the object.
 * Field 3. Output state
 * Field 4. Control state
 * Field 5. Control mode
 *
****************************************************************************************/
#if (defined(USED_CLASS_70) && USED_CLASS_70 == 1)

/* Static parameters */
const control_mode_t g_Class70_mode = CONTROL_MODE_BCFG_AE;

/* Variable parameters */
Unsigned8			g_Class70_output_state = 0;
control_mode_t 		g_Class70_control_state = CONTROL_MODE_NONE;

const class70_child_record_t g_ChildTableClass70[] =
{
	/* Disconnect control object */
	{
		{0, 0, 96, 3, 10, 255 },														/* Field 1. Logical name (OBIS code) of the object. */
		g_AccessRightTable[0],															/* Field 2. Access right definition for 1 object    */
		DYN_VALUE(CHOICE_BOOLEAN, &g_Class70_output_state, ATTR_ACCESS_MEMORY),			/* Field 3. Output state                            */
		DYN_VALUE(CHOICE_ENUM, &g_Class70_control_state, ATTR_ACCESS_MEMORY),			/* Field 4. Control state                           */
		&g_Class70_mode,																/* Field 5. Control mode                            */
	},
};
const Unsigned16 g_ChildTableLengthClass70 = sizeof(g_ChildTableClass70) / sizeof(class70_child_record_t);
#endif

/****************************************************************************************
 * Class ID = 71, Limiter Child-table
 *
 * How to define a row of this table.
 *
 * Field 1. Fill the OBIS code first, 6 numbers (A, B, C, D, E, F)
 * Field 2. Specify which row on g_AccessRightTable will be used to declare the
 *          access right of the object.
 * Field 3.  Monitored value
 * Field 4.  Threshold active
 * Field 5.  Threshold normal
 * Field 6.  Threshold emergency
 * Field 7.  Min over threshold duration
 * Field 8.  Min under threshold duration
 * Field 9.  Emergency_profile
 * Field 10. Emergency profile group id list
 * Field 11. Emergency profile group id list number
 * Field 12. Emergency profile active
 * Field 13. Actions
 *
****************************************************************************************/
#if (defined(USED_CLASS_71) && USED_CLASS_71 == 1)

/* Static parameters */
const value_def_t g_Class71_value_def = {
	1,										/* Class ID          */
	{   0,   0,  96,   7,   0, 255 },		/* The logical name  */
	2,										/* attribute_index   */
};

Unsigned16 g_Class71_threshold;
Unsigned32 g_Class71_min_over;
Unsigned32 g_Class71_min_under;

date_time_t g_Class71_eme_act_time;
eme_profile_t g_Class71_eme_profile = {
	1,										/* emergency_profile_id       */
	&g_Class71_eme_act_time,				/* emergency_activation_time  */
	1,										/*emergency_duration          */
};

Unsigned16 g_Class71_eme_profile_id_list[] = {1};
Unsigned8  g_Class71_eme_profile_id_list_nr = sizeof(g_Class71_eme_profile_id_list) / sizeof(Unsigned16);

Unsigned8  g_Class71_eme_profile_active  = 0;

/* Actions definition */
const action_item_t g_Class71_action_item[] =
{
	/*----------------------------------------------------*/
	/*  �Script table� logical name  | Script_identifier  */
	/*----------------------------------------------------*/
	{{   0,   0,   10,   0,   1, 255 }, 0x0002,},
	{{   0,   0,   10,   0,   1, 255 }, 0x0003,},
};

const action_set_t g_Class71_action_list[] =
{
	/*--------------------------------------------*/
	/*  Action up             | Action down       */
	/*--------------------------------------------*/
	{&g_Class71_action_item[0],	&g_Class71_action_item[1]},
};

const class71_child_record_t g_ChildTableClass71[] =
{
	/* Limiter */
	{
		{0, 0, 17, 0, 0, 255 },															/* Field 1.  Logical name (OBIS code) of the object.        */
		g_AccessRightTable[0],															/* Field 2.  Access right definition for 1 object           */
		&g_Class71_value_def,															/* Field 3.  Monitored value                                */
		DYN_VALUE(CHOICE_LONG_UNSIGNED, &g_Class71_threshold, ATTR_ACCESS_MEMORY),		/* Field 4.  Threshold active                               */
		VALUE(CHOICE_LONG_UNSIGNED, &g_Class71_threshold),								/* Field 5.  Threshold normal                               */
		VALUE(CHOICE_LONG_UNSIGNED, &g_Class71_threshold),								/* Field 6.  Threshold emergency                            */
		&g_Class71_min_over,															/* Field 7.  Min over threshold duration                    */
		&g_Class71_min_under,															/* Field 8.  Min under threshold duration                   */
		&g_Class71_eme_profile,															/* Field 9.  Emergency_profile                              */
		g_Class71_eme_profile_id_list,													/* Field 10. Emergency profile group id list                */
		&g_Class71_eme_profile_id_list_nr,												/* Field 11. Emergency profile group id list number         */
		DYN_VALUE(CHOICE_BOOLEAN, &g_Class71_eme_profile_active, ATTR_ACCESS_MEMORY),	/* Field 12. Emergency profile active                       */
		g_Class71_action_list,															/* Field 13. Actions                                        */
	},
};
const Unsigned16 g_ChildTableLengthClass71 = sizeof(g_ChildTableClass71) / sizeof(class71_child_record_t);
#endif

/****************************************************************************************
 * Class ID = 101, ZigBee SAS Startup 
 *
 * How to define a row of this table.
 *
 * Field 1.  Fill the OBIS code first, 6 numbers (A, B, C, D, E, F)
 * Field 2.  Specify which row on g_AccessRightTable will be used to declare the
 *          access right of the object.
 * Field 3.  Short address
 * Field 4.  Extended Pan ID
 * Field 5.  Pan ID
 * Field 6.  Channel Mask
 * Field 7.  Protocol version
 * Field 8.  Stack profile
 * Field 9.  Start-up control
 * Field 10. Trust centre address
 * Field 11. Link key
 * Field 12. Network key
 * Field 13. Use insecure join
 *
****************************************************************************************/
#if (defined(USED_CLASS_101) && USED_CLASS_101 == 1)

Unsigned16 				g_Class101_short_address = 0x0000;
Unsigned8				g_Class101_extended_pan_ID[8];
Unsigned16 				g_Class101_pan_ID = 0x1111;
Unsigned32 				g_Class101_channel_mask = 0;
Unsigned8 				g_Class101_protocol_version = 0x01;
zigbee_stack_profile_t 	g_Class101_stack_profile = ZIGBEE_PRO;
Unsigned8 				g_Class101_startup_control = 2;
Unsigned8 				g_Class101_use_insecure_join = 1;
Unsigned8				g_Class101_trust_centre_address[8];
Unsigned8				g_Class101_link_key[16];
Unsigned8				g_Class101_network_key[16];

const class101_child_record_t g_ChildTableClass101[] =
{
	/* ZigBee SAS Startup */
	{
		{0, 0, 30, 0, 0, 255 },							/* Field 1.  Logical name (OBIS code) of the object			*/
		g_AccessRightTable[0],							/* Field 2.  Access right definition for 1 object			*/
		&g_Class101_short_address,						/* Field 3.  Short address									*/
		g_Class101_extended_pan_ID,						/* Field 4.  Extended Pan ID								*/
		&g_Class101_pan_ID,								/* Field 5.  Pan ID											*/
		&g_Class101_channel_mask,						/* Field 6.  Channel Mask									*/
		&g_Class101_protocol_version,					/* Field 7.  Protocol version								*/
		&g_Class101_stack_profile,						/* Field 8.  Stack profile									*/
		&g_Class101_startup_control,					/* Field 9.  Start-up control								*/
		g_Class101_trust_centre_address,				/* Field 10. Trust centre address							*/
		g_Class101_link_key,							/* Field 11. Link key										*/
		g_Class101_network_key,							/* Field 12. Network key									*/
		&g_Class101_use_insecure_join,					/* Field 13. Use insecure join								*/
	},
};
const Unsigned16 g_ChildTableLengthClass101  = sizeof(g_ChildTableClass101) / sizeof(class101_child_record_t);
#endif

/****************************************************************************************
 * Class ID = 102, ZigBee SAS Join
 *
 * How to define a row of this table.
 *
 * Field 1.  Fill the OBIS code first, 6 numbers (A, B, C, D, E, F)
 * Field 2.  Specify which row on g_AccessRightTable will be used to declare the
 *          access right of the object.
 * Field 3.  Scan attemps
 * Field 4.  Time between scans
 * Field 5.  Rejoin interval
 * Field 6.  Rejoin retry interval
 *
****************************************************************************************/
#if (defined(USED_CLASS_102) && USED_CLASS_102 == 1)
Unsigned8 		g_Class102_scan_attempts = 3;
Unsigned16 		g_Class102_time_between_scans = 1;
Unsigned16 		g_Class102_rejoin_interval = 60;
Unsigned16 		g_Class102_rejoin_retry_interval = 900;

const class102_child_record_t g_ChildTableClass102[] =
{
	/* ZigBee SAS Join */
	{
		{0, 0, 30, 1, 0, 255 },							/* Field 1.  Logical name (OBIS code) of the object			*/
		g_AccessRightTable[0],							/* Field 2.  Access right definition for 1 object			*/
		&g_Class102_scan_attempts,						/* Field 3.  Scan attemps									*/
		&g_Class102_time_between_scans,					/* Field 4.  Time between scans								*/
		&g_Class102_rejoin_interval,					/* Field 5.  Rejoin interval								*/
		&g_Class102_rejoin_retry_interval,				/* Field 6.  Rejoin retry interval							*/
	},
};
const Unsigned16 g_ChildTableLengthClass102  = sizeof(g_ChildTableClass102) / sizeof(class102_child_record_t);
#endif

/****************************************************************************************
 * Class ID = 103, ZigBee SAS APS Fragment
 *
 * How to define a row of this table.
 *
 * Field 1.  Fill the OBIS code first, 6 numbers (A, B, C, D, E, F)
 * Field 2.  Specify which row on g_AccessRightTable will be used to declare the
 *          access right of the object.
 * Field 3.  APS interframe_delay
 * Field 4.  APS max window size
 *
****************************************************************************************/
#if (defined(USED_CLASS_103) && USED_CLASS_103 == 1)
Unsigned16	g_Class103_aps_interframe_delay = 50;
Unsigned16 	g_Class103_aps_max_window_size = 1;

const class103_child_record_t g_ChildTableClass103[] =
{
	/* ZigBee SAS APS Fragmentation */
	{
		{0, 0, 30, 2, 0, 255 },							/* Field 1.  Logical name (OBIS code) of the object			*/
		g_AccessRightTable[0],							/* Field 2.  Access right definition for 1 object			*/
		&g_Class103_aps_interframe_delay,				/* Field 3.  APS_interframe_delay							*/
		&g_Class103_aps_max_window_size,				/* Field 4.  APS max window size							*/
	},
};
const Unsigned16 g_ChildTableLengthClass103  = sizeof(g_ChildTableClass103) / sizeof(class103_child_record_t);
#endif

/****************************************************************************************
 * Class ID = 104, ZigBee Network Control
 *
 * How to define a row of this table.
 *
 * Field 1.  Fill the OBIS code first, 6 numbers (A, B, C, D, E, F)
 * Field 2.  Specify which row on g_AccessRightTable will be used to declare the
 *          access right of the object.
 * Field 3.  Enable Disable Joining
 * Field 4.  Join timeout
 * Field 5.  Active devices array
 * Field 6.  Number of active devices
 *
****************************************************************************************/
#if (defined(USED_CLASS_104) && USED_CLASS_104 == 1)
Unsigned8					g_Class104_enable_disable_joining = 0;
Unsigned16					g_Class104_join_timeout = 60;
zigbee_active_devices_t		g_Class104_active_devices[2];
Unsigned16					g_Class104_nr_active_device = sizeof(g_Class104_active_devices) / 
															sizeof(zigbee_active_devices_t);

/* Variables for methods */
zigbee_register_device_t	g_Class104_registered_devices[2];
Unsigned8					g_Class104_nr_registered_devices = sizeof(g_Class104_registered_devices) / 
																sizeof(zigbee_register_device_t);

const class104_child_record_t g_ChildTableClass104[] =
{
	/* ZigBee Network Control */
	{
		{0, 0, 30, 3, 0, 255 },							/* Field 1.  Logical name (OBIS code) of the object			*/
		g_AccessRightTable[0],							/* Field 2.  Access right definition for 1 object			*/
		&g_Class104_enable_disable_joining,				/* Field 3.  Enable Disable Joining							*/
		&g_Class104_join_timeout,						/* Field 4.  Join timeout									*/
		g_Class104_active_devices,						/* Field 5.  Active devices									*/
		&g_Class104_nr_active_device,					/* Field 6.  Number of active devices						*/
	},
};
const Unsigned16 g_ChildTableLengthClass104  = sizeof(g_ChildTableClass104) / sizeof(class104_child_record_t);
#endif

/****************************************************************************************
 * Class ID = 105, ZigBee Tunnel Setup
 *
 * How to define a row of this table.
 *
 * Field 1.  Fill the OBIS code first, 6 numbers (A, B, C, D, E, F)
 * Field 2.  Specify which row on g_AccessRightTable will be used to declare the
 *          access right of the object.
 * Field 3.  Max incoming data packet transfer size (in octet)
 * Field 4.  Max outgoing data packet transfer size (in octet)
 * Field 5.  Protocol address (companion specification)
 * Field 6.  Close tunnel timeout (seconds)
 *
****************************************************************************************/
#if (defined(USED_CLASS_105) && USED_CLASS_105 == 1)
Unsigned16	g_Class105_max_incoming_transfer_size = 0x05DC;
Unsigned16	g_Class105_max_outgoing_transfer_size = 0x05DC;
Unsigned8	g_Class105_protocol_address[6];
Unsigned16	g_Class105_close_tunnel_timeout	= 0xFFFF;

const class105_child_record_t g_ChildTableClass105[] =
{
	// ZigBee Network Control */
	{
		{0, 0, 30, 4, 0, 255 },							/* Field 1.  Logical name (OBIS code) of the object			*/
		g_AccessRightTable[0],							/* Field 2.  Access right definition for 1 object			*/
		&g_Class105_max_incoming_transfer_size,			/* Field 3.  Maximum Incoming Transfer size					*/
		&g_Class105_max_outgoing_transfer_size,			/* Field 4.  Maximum Outgoing Transfer size					*/
		g_Class105_protocol_address,					/* Field 5.  6 octets, protocol address						*/
		&g_Class105_close_tunnel_timeout,				/* Field 6.  Close tunnel timeout						*/
	},
};
const Unsigned16 g_ChildTableLengthClass105  = sizeof(g_ChildTableClass105) / sizeof(class105_child_record_t);
#endif
