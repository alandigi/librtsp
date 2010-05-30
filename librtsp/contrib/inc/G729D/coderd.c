/*-------------------------------------------------------------------*
 *           The ITU-T G.729 8 kbit/s with annex D 6.4 kbit/s        *
 *                                                                   *
 *                                                                   *
 *-------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "g729dcodec.h"

Word16 prm[PRM_SIZE_6K];   /* Analysis parameters.       */
extern Word16 *new_speech; /* Pointer to new speech data  */
Word16 *speech_buf;
void InitG729D_encoder()
{
    Word16 i;
    CODEC_MODE = 1;  //6.4kps mode
    Init_Pre_Process();
    Init_Coder_ld8k();
    for( i=0; i< max(PRM_SIZE, PRM_SIZE_6K); i++)
    {
      prm[i] = (Word16)0;
    }
    return;
}

static int prm_to_BYTE(Word16 *prm, BYTE *encoedata)
{
    Word16 i;
    Word16 j;
    Word16 value;
    Word16 bit;
    Word16 count;
    BYTE tmpdata;
    tmpdata = 0;
    count = 0;

    for (i = 0; i < PRM_SIZE_6K; i++)
    {
        value = prm[i];
        value <<= (16 - bitsno_6k[i]);
        for ( j = 0; j < bitsno_6k[i]; ++j)
        {
            bit = value & (Word16)0x8000;      /* get msb */
            tmpdata <<= 1;

            if (bit)
            {
                tmpdata |= (BYTE)0x01;
            }
            count++;
            if ( 8 == count) // A byte
            {
                *encoedata++ = tmpdata;
                tmpdata = 0;
                count = 0;
            }
            value <<= 1;
        }
    }
    return 0;
}


Word16 G729D_encode(Word16 *rawdata, Word16 inlen, BYTE *encodeddata, Word16 size)
{
    static Word16 syn[L_FRAME];           /* Buffer for synthesis speech */
    memcpy(new_speech, rawdata, sizeof(Word16)*inlen);
    Pre_Process(rawdata, inlen);
    Coder_ld8kD(prm, syn);

    prm_to_BYTE(prm, encodeddata);
    return 8;
}
