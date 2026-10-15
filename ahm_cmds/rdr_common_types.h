/*
 * rdr_common_types.h
 *
 *  Created on: 20 feb 2025
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023
 */

#ifndef AHM_CMDS_RDR_COMMON_TYPES_H_
#define AHM_CMDS_RDR_COMMON_TYPES_H_
#include "stdint.h"
#define RDRCOM_STRUCT_PAYLOAD_MAX 255

typedef struct  _data_struct_t{
	unsigned char size;
	uint8_t buffer[RDRCOM_STRUCT_PAYLOAD_MAX];
}data_struct_t;

typedef union  _radar_command_payload {
	uint32_t	data_uint32;
	uint16_t    data_uint16;
	uint8_t		data_uint8;
	float		data_float;
	data_struct_t data_struct;
}radar_command_payload;

typedef enum  _payloadType_t{
	PT_NO=0,
	//regular response
	PT_U8=1,
	PT_U16=2,
	PT_U32=3,
	PT_FLOAT=4,
	PT_STRUCT=5,
	//special response
	PT_U8_SR=6,
	PT_U16_SR=7,
	PT_U32_SR=8,
	PT_FLOAT_SR=9,
	PT_STRUCT_SR=10,
}payloadType_t;

#define RDR_VERSION_SIZE 8
#define RDR_VERSION_CMD 0x01
#define RDR_VERSION_EXT_CMD 0x56

#define QUERY_PAYLOAD_STRUCT_VARIABLE 0xFFFFFFFF


#endif /* AHM_CMDS_RDR_COMMON_TYPES_H_ */
