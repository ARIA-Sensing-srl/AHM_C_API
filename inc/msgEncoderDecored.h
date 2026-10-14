/*
 * msgEncoderDecored.h
 *
 *  Created on: 07/ago/2019
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - Confidential. Not for Public Release
 */

#ifndef MSGENCODERDECORED_H_
#define MSGENCODERDECORED_H_

/*
 * Encoding mode use byte stuff method with alternate chars
 *
 * */

#include "stdint.h"

#define MSG_FRAME_START 0xFF
#define MSG_FRAME_STOP 0x00
#define MSG_FRAME_STUFF 0x80
#define MSG_FRAME_START_ALT 0xFE
#define MSG_FRAME_STOP_ALT 0x01
#define MSG_FRAME_STUFF_ALT 0x81

#define MSG_SUCCESS 0
#define MSG_FAILURE -1
#define MSG_DECODING_ERROR -2 	//returned when message is not consistent
#define MSG_ENCODE_FAILURE -3

/*
MsgEncode: encode the message with byte stuffing mechanism (not include start and stop bytes)
*/
int MSGEncode(uint8_t * pSrc, uint8_t * pDst, uint16_t srcsize);	//pSrc and pDst MUST not be the same
/*
MSGComputeEncodedLength: compute size of stuffed version (without start and stop bytes)
*/
int MSGComputeEncodedLength(uint8_t * pSrc, uint16_t srcsize); 		//compute the encoded length of the plain message
/*
MSGDecode: destuff the input data (remove start and stop byte from the source array)
*/
int MSGDecode(uint8_t * pSrc, uint8_t * pDst, uint16_t srcsize);	//no restriction for pSrc and pDst

#endif /* MSGENCODERDECORED_H_ */
