/*
 * radar_AHM2DSC.cpp
 *
 *  Created on: Jan 7, 2026
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023
 */


#include "radar_AHM2DSC.h"
#include "ahm_comm_def.h"
#include "ahm_common.h"
#include "stddef.h"
#include "string.h"
#include "f16tosingle.h"

using namespace rdr_AHM2DSC;


const RadarCommandItem_t radar_AHM2DSC::RadarCommandItemTbl[] = {
	{'s',RADARCOMM_START						,PT_NO		,0									},
	{'o',RADARCOMM_STOP							,PT_NO		,0									},
	{'d',RADARCOMM_GET_DATA						,PT_NO		,0									},
	{'f',RADARCOMM_SET_FRAME_RATE				,PT_U16		,0xFFFF								},
	{'p',RADARCOMM_SET_TX_POWER					,PT_U8		,0xFF								},
	{'x',RADARCOMM_SET_MIN_XRANGE				,PT_FLOAT	,0xFFFFFFFF							},
	{'X',RADARCOMM_SET_MAX_XRANGE				,PT_FLOAT	,0xFFFFFFFF							},
	{'0',RADARCOMM_SET_OFFSET					,PT_FLOAT	,0xFFFFFFFF							},
	{'l',RADARCOMM_SET_DECLUT_LENGTH			,PT_U16		,0xFFFF								},
	{'i',RADARCOMM_SET_ITERATIONS				,PT_U16		,0xFFFF								},
	{'e',RADARCOMM_SET_ELAB						,PT_U8		,0xFF								},
	{0x01,RADARCOMM_GET_VERSION					,PT_NO		,0									},
	{'?',RADARCOMM_UNKNOWN_COMMAND				,PT_NO		,0									},
	{'$',RADARCOMM_SET_DATA_CHANNEL				,PT_U8		,0xFF								},
	{'b',RADARCOMM_SET_OPTPROC					,PT_STRUCT	,2									},
	{0x07,RADARCOMM_GET_DATA_MULTIPLE			,PT_NO		,0									},
	{0x08,RADARCOMM_SET_SEQUENCE				,PT_STRUCT	,QUERY_PAYLOAD_STRUCT_VARIABLE		},
	{0x0A,RADARCOMM_SET_DATA_FMT				,PT_U8		,0xFF								},/**/
	{0x0B,RADARCOMM_SET_FREQ_CARRIER			,PT_U16		,0xFFFF								},
	{0x0C,RADARCOMM_SET_BW						,PT_U16		,0xFFFF								},
	{0x03,RADARCOMM_SG_BWMODE					,PT_U8		,0xFF								},
	{0x0D,RADARCOMM_SET_CODE					,PT_STRUCT	,QUERY_PAYLOAD_STRUCT_VARIABLE		},
	{0x0E,RADARCOMM_SET_GAIN					,PT_STRUCT	,4									},
	{0x0F,RADARCOMM_GET_IMG						,PT_NO		,0									},
	{0x05,RADARCOMM_GET_SAMPLING_RATE_MHZ		,PT_U16		,0xFFFF								},
	{'v',RADARCOMM_SET_ANT_ARRAY				,PT_STRUCT	,QUERY_PAYLOAD_STRUCT_VARIABLE		},
	{'m',RADARCOMM_SET_CANVAS					,PT_STRUCT	,QUERY_PAYLOAD_STRUCT_VARIABLE		},
	{0x04,RADARCOMM_SG_BAUDRATE					,PT_U32		,0xFFFFFFFF							},
	{0x02,RADARCOMM_SG_MULTISTREAM				,PT_U16	    ,0xFFFF								},
	{0x02,RADARCOMM_SG_MULTISTREAM				,PT_U16	    ,0xFFFF								},
	{0x84,RADARCOMM_SET_PROFILE					,PT_U8	    ,0xFF								},
	{0x80,RADARCOMM_SG_STATICOBJREMOVAL			,PT_U8	    ,0xFF								},
	{0x81,RADARCOMM_SG_STATICOBJMAPUPDATETIME	,PT_FLOAT   ,0xFFFFFFFF							},
	{0x82,RADARCOMM_SG_RXGAIN					,PT_U8	    ,0xFF								},
	{0x83,RADARCOMM_SG_EMBEDDEDIMGALGO			,PT_U8	    ,0xFF								},
	{'2',RADARCOMM_SG_BLOB_FILTER				,PT_STRUCT	    ,0xFFFFFFFF						},
	{'K',RADARCOMM_SG_KALMAN2D					,PT_STRUCT	    ,0xFFFFFFFF						},
	{110,RADARCOMM_SG_SNR0						,PT_FLOAT	    ,0xFFFFFFFF						},
	{44,RADARCOMM_SG_SNR1						,PT_FLOAT	    ,0xFFFFFFFF						},
	{45,RADARCOMM_SG_SNR2						,PT_FLOAT	    ,0xFFFFFFFF						},
	{95,RADARCOMM_SG_SNR3						,PT_FLOAT	    ,0xFFFFFFFF						},
	{'4',RADARCOMM_SG_SNRPwr0					,PT_FLOAT	    ,0xFFFFFFFF						},
	{'5',RADARCOMM_SG_SNRPwr1					,PT_FLOAT	    ,0xFFFFFFFF						},
	{'6',RADARCOMM_SG_SNRPwr2					,PT_FLOAT	    ,0xFFFFFFFF						},
	{'1',RADARCOMM_SG_SNRRanges					,PT_FLOAT	    ,0xFFFFFFFF						},
	{'E',RADARCOMM_GET_Detection2D				,PT_STRUCT	    ,0xFFFFFFFF						} };

radar_AHM2DSC::radar_AHM2DSC(rifll_h* val) : radar_AHM2D(val, rdr_AHM2DSC::HWID){
	RadarCommandItem = RadarCommandItemTbl;
	RadarCommandTblItems = sizeof(RadarCommandItemTbl)/sizeof(RadarCommandItemTbl[0]);
}
radar_AHM2DSC::~radar_AHM2DSC(){};


