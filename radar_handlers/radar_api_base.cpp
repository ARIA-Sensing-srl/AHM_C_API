/*
 * radar_api_base.cpp
 *
 *  Created on: 21 feb 2025
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - Confidential. Not for Public Release
 */
#include "string.h"
#include "radar_api_base.h"
#include "ahm_common.h"
#include "radarData.h"
#include "radarDataVolume.h"



radar_api_base::radar_api_base(rifll_h* p, uint16_t hw) : HWCODE(hw){
	this->rh = p;
}
radar_api_base::radar_api_base() : HWCODE(0){}

radar_api_base::~radar_api_base(){

}

int radar_api_base::rdr_get_val(const RadarCommandItem_t * pitem , radar_command_payload * ppayload){
	uint8_t buffer[sizeof(radar_command_payload)+1];

	//put command
	buffer[0] = pitem->cmdCode;

	ppayload->data_uint32 = pitem->queryPayload;
	int size = 1;
	int psize = 0;
	switch(pitem->payloadType){
		case PT_FLOAT:
		case PT_U32:
			buffer[size+2] =  (ppayload->data_uint32 >> 16) & 0xFF;
			buffer[size+3] =  (ppayload->data_uint32 >> 24) & 0xFF;
			psize += 2;
		case PT_U16:
			buffer[size+1] = ((ppayload->data_uint16 >> 8) & 0xFF);
			psize += 1;
		case PT_U8:
			buffer[size] = (ppayload->data_uint8);
			psize += 1;
			break;

		case PT_STRUCT:
			{
				/*const data_struct_t * pstructPayload = (const data_struct_t *)(ppayload->data_uint32);
				memcpy((void*)&buffer[size], pstructPayload->buffer, pstructPayload->size);
				psize = pstructPayload->size;*/ //add nothing
				psize = pitem->queryPayload; //
			}
			break;
		case PT_NO:
		default:
			return -1; //error
	}
	if (pitem->payloadType != PT_STRUCT)
		size += psize;
	//io
	int ret = this->rh->txrxMessage(buffer, size, buffer, sizeof(buffer));
	if (ret <= 0)
		return -1; //errror

	//check command
	if (buffer[0] != pitem->cmdCode){
		return -1; //invalid returned command
	}

	//skip size check if structure is variable
	if (!((psize == QUERY_PAYLOAD_STRUCT_VARIABLE) &&(pitem->payloadType == PT_STRUCT))){
		if (ret -1 != psize){
			return -1; //invalid returned payload
		}
	}
	//copy payload
	if (pitem->payloadType != PT_STRUCT)
		memcpy((void*)ppayload, buffer+1, psize);
	else{
		psize = ret-1;
		memcpy((void*)ppayload->data_struct.buffer, buffer+1, psize);
		ppayload->data_struct.size = psize;
	}
	return 0;

}
int radar_api_base::rdr_set_val(const RadarCommandItem_t * pitem , radar_command_payload * ppayload){
	uint8_t buffer[sizeof(radar_command_payload)+1];

	//put command
	buffer[0] = pitem->cmdCode;
	int size = 1;
	int psize = 0;
	switch(pitem->payloadType){
		case PT_FLOAT:
		case PT_U32:
			buffer[size+2] =  (ppayload->data_uint32 >> 16) & 0xFF;
			buffer[size+3] =  (ppayload->data_uint32 >> 24) & 0xFF;
			psize += 2;
		case PT_U16:
			buffer[size+1] = ((ppayload->data_uint16 >> 8) & 0xFF);
			psize += 1;
		case PT_U8:
			buffer[size] = (ppayload->data_uint8);
			psize += 1;
			break;
		case PT_NO:
			psize = 0;
			break;
		case PT_STRUCT:
			//copy structure
			if (ppayload->data_struct.size){
				memcpy((void*)&buffer[size], (const void*)(ppayload->data_struct.buffer), ppayload->data_struct.size);
			}
			psize = ppayload->data_struct.size;
			break;
		default:
			return -1; //error
	}
	size += psize;
	//io
	int ret = this->rh->txrxMessage( buffer, size, buffer, sizeof(buffer));
	if (ret <= 0)
		return -1; //errror

	//check command
	if (buffer[0] != pitem->cmdCode){
		return -1; //invalid returned command
	}
	if (ret -1 != psize){
		return -1; //invalid returned payload
	}
	//copy payload
	if (psize){
		if (pitem->payloadType != PT_STRUCT)
			memcpy((void*)ppayload, buffer+1, psize);
		else{
			if (psize < sizeof(ppayload->data_struct.buffer)){
				memcpy((void*)ppayload->data_struct.buffer, buffer+1, psize);
				ppayload->data_struct.size = psize;
			} else
				return -1; //too big
		}
	}
	return 0;
}

const RadarCommandItem_t * radar_api_base::getCommandItem(uint32_t srcCmd,const RadarCommandItem_t * tbl, int tblSize){
	for(int i = 0; i < (tblSize); i++){
		if (tbl[i].cmd == srcCmd){
			return &tbl[i];
		}
	}
	return NULL;
}

int radar_api_base::getHW(rifll_h * h, uint16_t * pHWCode){
	uint8_t version[(RDR_VERSION_SIZE)+1]; //version + response command
	uint8_t reqPayload=RDR_VERSION_CMD;
	int ret = h->txrxMessage(&reqPayload, 1, version, sizeof(version));
	if (ret < 0)
		return RDR_ERR_COMM;
	//check
	if (version[0] != RDR_VERSION_CMD)
		return RDR_ERR_COMM;
	//check version
	*pHWCode = (uint16_t)version[3] | ((uint16_t)version[4])<<8;
	return 0;
}

int radar_api_base::getHW(uint16_t * pHWCode){
	return getHW(rh, pHWCode);
}
