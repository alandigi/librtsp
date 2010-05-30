/* ITU-T G.729 Software Package Release 2 (November 2006) */
/* G.729 with ANNEX D   Version 1.1    Last modified: March 1998 */

#include "typedef.h"
#include "ld8k.h"
#include "ld8kd.h"
#include "tab_ld8k.h"
#include "tabld8kd.h"

/*-----------------------------------------------------*
 * Variables for codec switching                       *
 *-----------------------------------------------------*/
Word16 CODEC_MODE;
Word16 toggleFlag;

/*-----------------------------------------------------*
 | Tables for gain related routines . (6.4kbps)        |
 -----------------------------------------------------*/
Word16 gbk1_6k[NCODE1_6K][2] = {
/*  Q14    Q14 */
 { 5849,     0 },
 { 2929,  1078 },
 { 9425,  2731 },
 { 6068,  6093 },
 { 3617,  6747 },
 { 3171,  9280 },
 { 3915, 12872 },
 { 4987, 22294 }
};

Word16 gbk2_6k[NCODE2_6K][2] = {
/*  Q14    Q14 */
 {    0,  4175 },
 { 3988,     0 },
 { 4478,  7324 },
 { 7876,  7821 },
 {10291, 11385 },
 {10828, 27602 },
 {11956, 10735 },
 {16423, 15724 }
};

/* {36.632507,  2.514171},  {0.399259,  0.073709} */
/*  [0][0]      [0][1]       [1][0]     [1][1]    */
/*  Q9          Q13          Q16        Q18       */
Word16 coef_6k[2][2] = {
   { 18756, 20596},
   { 26166, 19322}
};
/*  [0][0]      [0][1]       [1][0]     [1][1]    */
/*  Q25         Q29          Q32        Q34       */
Word32 L_coef_6k[2][2] = {
   { 1229182965L, 1349785278L },
   { 1714804347L, 1266310978L }
};

Word16 thr1_6k[NCODE1_6K-NCAN1_6K] = { /* Q13 */
   9919,  /* 1.210869 */
  19675   /* 2.401702 */
};

Word16 thr2_6k[NCODE2_6K-NCAN2_6K] = { /* Q15 */
  17233,  /* 0.525915 */
  25144   /* 0.767320 */
};

Word16 map1_6k[NCODE1_6K] = { 0, 4, 6, 5, 2, 1, 7, 3 };

Word16 imap1_6k[NCODE1_6K] = { 0, 5, 4, 7, 1, 3, 2, 6 };

Word16 map2_6k[NCODE2_6K] = { 0, 4, 3, 7, 5, 1, 6, 2 };

Word16 imap2_6k[NCODE2_6K] = { 0, 5, 7, 2, 1, 4, 6, 3 };


/*-----------------------------------------------------*
 | Tables for algebraic codebook routines. (6.4kbps)   |
 -----------------------------------------------------*/
Word16 trackTable0[] = {
 1, 3, 6, 8, 11, 13, 16, 18, 21, 23, 26, 28, 31, 33, 36, 38};

Word16 trackTable1[] = {
0, 2, 4, 5, 7, 9, 10, 12, 14, 15, 17, 19, 20, 22, 24, 25,
27, 29, 30, 32, 34, 35, 37, 39, 1, 6, 11, 16, 21, 26, 31, 36};

Word16 grayEncode[] = {
0, 1, 3, 2, 6, 7, 5, 4, 12, 13, 15, 14, 10, 11, 9, 8,
24, 25, 27, 26, 30, 31, 29, 28, 20, 21, 23, 22, 18, 19, 17, 16};

Word16 grayDecode[] = {
0, 1, 3, 2, 7, 6, 4, 5, 15, 14, 12, 13, 8, 9, 11, 10,
31, 30, 28, 29, 24, 25, 27, 26, 16, 17, 19, 18, 23, 22, 20, 21};

/*-----------------------------------------------------*
 | Tables for the adaptive phase dispersion. (6.4kbps) |
 -----------------------------------------------------*/
/* post-processing in Q15 */
Word16 ph_imp_low[] = {
14690, 11518, 1268, -2762, -5672, 7514, -36, -2808, -3041, 4823,
2952, -8425, 3785, 1455, 2179, -8638, 8051, -2104, -1455, 777,
1108, -2386, 2254, -364, -675, -2104, 6046, -5682, 1072, 3123,
-5059, 5312, -2330, -3729, 6924, -3890, 675, -1776, 29, 10145};

Word16 ph_imp_mid[] = {
30274, 3831, -4037, 2972, -1049, -1003, 2477, -3044, 2815, -2232,
1753, -1612, 1714, -1776, 1543, -1009, 429, -170, 472, -1265,
2176, -2707, 2523, -1622, 344, 826, -1530, 1724, -1658, 1701,
-2064, 2644, -3061, 2897, -1979, 557, 780, -1370, 842, 655};

//#define NC0_B       7          /* number of first stage bits                */
//#define NC1_B       5          /* number of second stage bits               */
// 1+NC0_B+NC1_B*2+8+9+2+6+4+9+2+6 = 64 bit
Word16 bitsno_6k[PRM_SIZE_6K] = {1+NC0_B,         /* MA + 1st stage */
                                 NC1_B*2, /* 2nd stage */
                                 8,     9,2, 6,   /* first subframe  */
                                 4,     9,2, 6};  /* second subframe */

