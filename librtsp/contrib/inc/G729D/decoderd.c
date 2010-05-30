#include <stdlib.h>
#include <stdio.h>

#include "g729dcodec.h"

Word16  synth_buf[L_FRAME+M], *synth; /* Synthesis                   */
Word16  voicing;                      /* voicing from previous frame */
Word16  parm[PRM_SIZE_6K+1]; /* Synthesis parameters  */
Word16  Az_dec[MP1*2], *ptr_Az;       /* Decoded Az for post-filter  */
Word16  T0_first;                     /* Pitch lag in 1st subframe   */
Word16  sf_voic;                      /* voicing for subframe        */
Word16  serial[max(SERIAL_SIZE, SERIAL_SIZE_6K)]; /* Serial stream   */

void InitG729D_decoder()
{

    Word16 i;
    CODEC_MODE = 1; //@6.4kps mode
    for ( i = 0; i < M; ++i)
        synth_buf[i] = 0;
    synth = synth_buf + M;

    Init_Decod_ld8k();
    Init_Post_Filter();
    Init_Post_Process();
    voicing = 60;
}

static void BYTE_to_parm(BYTE *encodeddata, Word16 *parmt)
{
    Word16 i;
    Word16 j;
    Word16 k;
    BYTE value;
    BYTE bit;
    Word16 count;
    Word16 tmpdata;
    tmpdata = 0;
    count = 0;

    tmpdata = 0;
    k = 0;
    for ( i = 0; i < 8; ++i )
    {
        value = encodeddata[i];
        for ( j = 0; j < 8; ++j)
        {
            bit = value & (BYTE)0x80; // get msb
            value <<= 1;
            tmpdata <<= 1;
            if ( bit )
            {
                tmpdata |= 0x0001;
            }
            ++count;
            if ( count == bitsno_6k[k])
            {
                k++;
                *parmt++ = tmpdata;
                count = 0;
                tmpdata = 0;
            }
        }
    }
}

Word16 G729D_decode(BYTE *encodeddata, Word16 inlen, Word16 *pst_out, Word16 size)
{
    Word16  i;
    parm[0] = 0;           /* No frame erasure */
//for (i=2; i < serial_size; i++)
//    if (serial[i] == 0 )
//       parm[0] = 1; /* frame erased     */

    BYTE_to_parm(encodeddata, &parm[1]);

    Decod_ld8kD(parm, voicing, synth, Az_dec, &T0_first);

    /* Postfilter */
    voicing = 0;
    ptr_Az = Az_dec;
    for( i = 0 ; i < L_FRAME; i += L_SUBFR)
    {
        Post(T0_first, &synth[i], ptr_Az, &pst_out[i], &sf_voic);
        if (sf_voic != 0)
        {
            voicing = sf_voic;
        }
        ptr_Az += MP1;
    }
    Copy(&synth_buf[L_FRAME], &synth_buf[0], M);
    Post_Process(pst_out, L_FRAME);

    return L_FRAME;
}
