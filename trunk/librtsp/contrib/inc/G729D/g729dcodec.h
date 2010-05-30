#ifndef G729DCODEC_INCLUDED
#define G729DCODEC_INCLUDED

#include <stdio.h>
#include <stdlib.h>

#include "typedef.h"
#include "basic_op.h"
#include "ld8k.h"
#include "ld8kd.h"
#include "tabld8kd.h"

#ifndef BYTE
#define BYTE unsigned char
#endif
void InitG729D_encoder();
void InitG729D_decoder();
/**
@param Word16 *rawdata:    input data, require PCM 16bits/sample, 8KHz??
@param Word16 inlen:       input data len should awlays be L_FRAME
@param BYTE  *encodeddata:g729d encoded data
@param Word16 outlen:      size of encodeddata, always be 8/10ms frame
@return Word16             output size
*/
Word16 G729D_encode(Word16 *rawdata, Word16 inlen, BYTE *encodeddata, Word16 size);


/**
@param BYTE  *encodeddata: g729d encoded data
@param Word16 inlen:       input data len always be 8/10ms frame
@param Word16 *out:        output data, PCM 16bits/sample 8KHz??
@param Word16 outlen:      size of out
@return Word16             output size
*/
Word16 G729D_decode(BYTE *encodeddata, Word16 inlen, Word16 *out, Word16 size);

#endif // G729DCODEC_INCLUDED
