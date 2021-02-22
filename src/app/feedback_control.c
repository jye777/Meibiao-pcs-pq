/*
 * File: feedback_control.c
 *
 * Code generated for Simulink model 'feedback_control'.
 *
 * Model version                  : 1.111
 * Simulink Coder version         : 8.11 (R2016b) 25-Aug-2016
 * C/C++ source code generated on : Wed Jan 13 16:40:43 2021
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: STMicroelectronics->ST10/Super10
 * Code generation objectives:
 *    1. Execution efficiency
 *    2. RAM efficiency
 * Validation result: Not run
 */

#include "feedback_control.h"
#ifndef UCHAR_MAX
#include <limits.h>
#endif

#if ( UCHAR_MAX != (0xFFU) ) || ( SCHAR_MAX != (0x7F) )
#error Code was generated for compiler with different sized uchar/char. \
Consider adjusting Test hardware word size settings on the \
Hardware Implementation pane to match your compiler word sizes as \
defined in limits.h of the compiler. Alternatively, you can \
select the Test hardware is the same as production hardware option and \
select the Enable portable word sizes option on the Code Generation > \
Verification pane for ERT based targets, which will disable the \
preprocessor word size checks.
#endif

#if ( USHRT_MAX != (0xFFFFU) ) || ( SHRT_MAX != (0x7FFF) )
#error Code was generated for compiler with different sized ushort/short. \
Consider adjusting Test hardware word size settings on the \
Hardware Implementation pane to match your compiler word sizes as \
defined in limits.h of the compiler. Alternatively, you can \
select the Test hardware is the same as production hardware option and \
select the Enable portable word sizes option on the Code Generation > \
Verification pane for ERT based targets, which will disable the \
preprocessor word size checks.
#endif

#if ( UINT_MAX != (0xFFFFFFFFU) ) || ( INT_MAX != (0x7FFFFFFF) )
#error Code was generated for compiler with different sized uint/int. \
Consider adjusting Test hardware word size settings on the \
Hardware Implementation pane to match your compiler word sizes as \
defined in limits.h of the compiler. Alternatively, you can \
select the Test hardware is the same as production hardware option and \
select the Enable portable word sizes option on the Code Generation > \
Verification pane for ERT based targets, which will disable the \
preprocessor word size checks.
#endif

#if ( ULONG_MAX != (0xFFFFFFFFUL) ) || ( LONG_MAX != (0x7FFFFFFFL) )
#error Code was generated for compiler with different sized ulong/long. \
Consider adjusting Test hardware word size settings on the \
Hardware Implementation pane to match your compiler word sizes as \
defined in limits.h of the compiler. Alternatively, you can \
select the Test hardware is the same as production hardware option and \
select the Enable portable word sizes option on the Code Generation > \
Verification pane for ERT based targets, which will disable the \
preprocessor word size checks.
#endif

/* Block signals and states (auto storage) */
D_Work rtDWork;

/* External inputs (root inport signals with auto storage) */
ExternalInputs rtU;

/* External outputs (root outports fed by signals with auto storage) */
ExternalOutputs rtY;
extern int32_T rt_sqrt_Us32_Ys32_Is64_f_s(int32_T u);
int32_T MultiWord2sLong(const uint32_T u[]);
void sMultiWordShr(const uint32_T u1[], int16_T n1, uint16_T n2, uint32_T y[],
                   int16_T n);
void sMultiWordMul(const uint32_T u1[], int16_T n1, const uint32_T u2[], int16_T
                   n2, uint32_T y[], int16_T n);
void sLong2MultiWord(int32_T u, uint32_T y[], int16_T n);
boolean_T sMultiWordLe(const uint32_T u1[], const uint32_T u2[], int16_T n);
int16_T sMultiWordCmp(const uint32_T u1[], const uint32_T u2[], int16_T n);
int32_T div_nde_s32_floor(int32_T numerator, int32_T denominator);
int32_T MultiWord2sLong(const uint32_T u[])
{
  return (int32_T)u[0];
}

void sMultiWordShr(const uint32_T u1[], int16_T n1, uint16_T n2, uint32_T y[],
                   int16_T n)
{
  int16_T nb;
  int16_T i;
  uint32_T ys;
  uint32_T yi;
  uint32_T u1i;
  int16_T nc;
  uint16_T nr;
  uint16_T nl;
  int16_T i1;
  nb = (int16_T)(n2 >> 5);
  i = 0;
  ys = ((u1[n1 - 1] & 2147483648UL) != 0UL) ? MAX_uint32_T : 0UL;
  if (nb < n1) {
    nc = n + nb;
    if (nc > n1) {
      nc = n1;
    }

    nr = n2 - (((uint16_T)nb) << 5);
    if (nr > 0U) {
      nl = 32U - nr;
      u1i = u1[nb];
      for (i1 = nb + 1; i1 < nc; i1++) {
        yi = (u1i >> nr);
        u1i = u1[i1];
        y[i] = (u1i << nl) | yi;
        i++;
      }

      yi = (u1i >> nr);
      u1i = (nc < n1) ? u1[nc] : ys;
      y[i] = (u1i << nl) | yi;
      i++;
    } else {
      for (i1 = nb; i1 < nc; i1++) {
        y[i] = u1[i1];
        i++;
      }
    }
  }

  while (i < n) {
    y[i] = ys;
    i++;
  }
}

void sMultiWordMul(const uint32_T u1[], int16_T n1, const uint32_T u2[], int16_T
                   n2, uint32_T y[], int16_T n)
{
  int16_T i;
  int16_T j;
  int16_T k;
  int16_T nj;
  uint32_T u1i;
  uint32_T yk;
  uint32_T a1;
  uint32_T a0;
  uint32_T b1;
  uint32_T w10;
  uint32_T w01;
  uint32_T cb;
  boolean_T isNegative1;
  boolean_T isNegative2;
  uint32_T cb1;
  uint32_T cb2;
  isNegative1 = ((u1[n1 - 1] & 2147483648UL) != 0UL);
  isNegative2 = ((u2[n2 - 1] & 2147483648UL) != 0UL);
  cb1 = 1UL;

  /* Initialize output to zero */
  for (k = 0; k < n; k++) {
    y[k] = 0UL;
  }

  for (i = 0; i < n1; i++) {
    cb = 0UL;
    u1i = u1[i];
    if (isNegative1) {
      u1i = (~u1i) + cb1;
      cb1 = (uint32_T)(u1i < cb1);
    }

    a1 = (u1i >> 16U);
    a0 = u1i & 65535UL;
    cb2 = 1UL;
    k = n - i;
    nj = (n2 <= k) ? n2 : k;
    k = i;
    for (j = 0; j < nj; j++) {
      yk = y[k];
      u1i = u2[j];
      if (isNegative2) {
        u1i = (~u1i) + cb2;
        cb2 = (uint32_T)(u1i < cb2);
      }

      b1 = (u1i >> 16U);
      u1i &= 65535UL;
      w10 = a1 * u1i;
      w01 = a0 * b1;
      yk += cb;
      cb = (uint32_T)(yk < cb);
      u1i *= a0;
      yk += u1i;
      cb += (yk < u1i);
      u1i = (w10 << 16U);
      yk += u1i;
      cb += (yk < u1i);
      u1i = (w01 << 16U);
      yk += u1i;
      cb += (yk < u1i);
      y[k] = yk;
      cb += (w10 >> 16U);
      cb += (w01 >> 16U);
      cb += a1 * b1;
      k++;
    }

    if (k < n) {
      y[k] = cb;
    }
  }

  /* Apply sign */
  if (isNegative1 != isNegative2) {
    cb = 1UL;
    for (k = 0; k < n; k++) {
      yk = (~y[k]) + cb;
      y[k] = yk;
      cb = (uint32_T)(yk < cb);
    }
  }
}

void sLong2MultiWord(int32_T u, uint32_T y[], int16_T n)
{
  uint32_T yi;
  int16_T i;
  y[0] = (uint32_T)u;
  yi = (u < 0L) ? MAX_uint32_T : 0UL;
  for (i = 1; i < n; i++) {
    y[i] = yi;
  }
}

boolean_T sMultiWordLe(const uint32_T u1[], const uint32_T u2[], int16_T n)
{
  return sMultiWordCmp(u1, u2, n) <= 0;
}

int16_T sMultiWordCmp(const uint32_T u1[], const uint32_T u2[], int16_T n)
{
  int16_T y;
  uint32_T su1;
  uint32_T su2;
  int16_T i;
  su1 = u1[n - 1] & 2147483648UL;
  su2 = u2[n - 1] & 2147483648UL;
  if ((su1 ^ su2) != 0UL) {
    y = (su1 != 0UL) ? -1 : 1;
  } else {
    y = 0;
    i = n;
    while ((y == 0) && (i > 0)) {
      i--;
      su1 = u1[i];
      su2 = u2[i];
      if (su1 != su2) {
        y = (su1 > su2) ? 1 : -1;
      }
    }
  }

  return y;
}

int32_T div_nde_s32_floor(int32_T numerator, int32_T denominator)
{
  return ((((numerator < 0L) != (denominator < 0L)) && ((numerator % denominator)
            != 0L)) ? -1L : 0L) + (numerator / denominator);
}

int32_T rt_sqrt_Us32_Ys32_Is64_f_s(int32_T u)
{
  int32_T y;
  int32_T tmp01_y;
  int32_T shiftMask;
  int64m_T tmp03_u;
  int16_T iBit;
  int64m_T tmp;
  uint32_T tmp_0;
  uint32_T tmp_1;

  /* Fixed-Point Sqrt Computation by the bisection method. */
  y = 0L;
  shiftMask = 1073741824L;
  sLong2MultiWord(u, &tmp03_u.chunks[0U], 2);
  for (iBit = 0; iBit < 31; iBit++) {
    tmp01_y = y | shiftMask;
    tmp_0 = (uint32_T)tmp01_y;
    tmp_1 = (uint32_T)tmp01_y;
    sMultiWordMul(&tmp_0, 1, &tmp_1, 1, &tmp.chunks[0U], 2);
    if (sMultiWordLe(&tmp.chunks[0U], &tmp03_u.chunks[0U], 2)) {
      y = tmp01_y;
    }

    shiftMask = (int32_T)(((uint32_T)shiftMask) >> 1U);
  }

  return y;
}

/* Model step function */
void feedback_control_step(void)
{
  int32_T rtb_DataTypeConversion1;
  int16_T rtb_DataTypeConversion3;
  int16_T rtb_Abs;
  int32_T rtb_Divide_p;
  int96m_T tmp;
  int96m_T tmp_0;
  int64m_T tmp_1;
  int64m_T tmp_2;
  uint32_T tmp_3;
  uint32_T tmp_4;
  int96m_T tmp_5;
  int16_T rtb_Abs_0;

  /* DataTypeConversion: '<S5>/Data Type Conversion1' incorporates:
   *  Inport: '<Root>/p_rate'
   */
  rtb_DataTypeConversion1 = rtU.p_rate;

  /* DataTypeConversion: '<S5>/Data Type Conversion3' incorporates:
   *  DataTypeConversion: '<S5>/Data Type Conversion1'
   *  Gain: '<S5>/Gain'
   *  Inport: '<Root>/p_cmd'
   *  Inport: '<Root>/p_rate'
   *  Product: '<S5>/Divide'
   */
  rtb_DataTypeConversion3 = (int16_T)div_nde_s32_floor(((int32_T)rtU.p_cmd) <<
    13, rtU.p_rate);

  /* SwitchCase: '<Root>/Switch Case' incorporates:
   *  Inport: '<Root>/ul_mode'
   *  Inport: '<S4>/p_pu'
   *  Inport: '<S6>/p_pu'
   *  Inport: '<S7>/p_pu'
   */
  switch ((int32_T)rtU.ul_mode) {
   case 1L:
    /* Outputs for IfAction SubSystem: '<Root>/const-powerfactor' incorporates:
     *  ActionPort: '<S4>/Action Port'
     */
    /* Switch: '<S17>/Switch_3' incorporates:
     *  Constant: '<S17>/Constant1'
     *  Inport: '<Root>/cosPhi'
     *  RelationalOperator: '<S17>/RO_2'
     */
    if (8192 < rtU.cosPhi) {
      rtb_Abs = 8192;
    } else {
      rtb_Abs = rtU.cosPhi;
    }

    /* End of Switch: '<S17>/Switch_3' */

    /* Switch: '<S17>/Switch_2' incorporates:
     *  Constant: '<S17>/Constant2'
     *  RelationalOperator: '<S17>/RO_3'
     */
    if (-8192 > rtb_Abs) {
      rtb_Abs = -8192;
    }

    /* End of Switch: '<S17>/Switch_2' */
    rtDWork.p_pu_d = rtb_DataTypeConversion3;

    /* Abs: '<S17>/Abs' incorporates:
     *  Inport: '<S4>/p_pu'
     */
    if (rtb_Abs < 0) {
      rtb_Abs_0 = -rtb_Abs;
    } else {
      rtb_Abs_0 = rtb_Abs;
    }

    /* End of Abs: '<S17>/Abs' */

    /* Switch: '<S17>/Switch_1' incorporates:
     *  Inport: '<Root>/cosMin'
     *  RelationalOperator: '<S17>/RO_1'
     */
    if (rtb_Abs_0 < rtU.cosMin) {
      rtDWork.Switch_1 = rtDWork.p_pu_d;
    } else {
      /* Gain: '<S17>/Gain2' */
      tmp_1 = rtConstP.pooled1;

      /* Product: '<S17>/Divide3' incorporates:
       *  Constant: '<S17>/Constant'
       *  Gain: '<S17>/Gain1'
       *  Product: '<S17>/Divide1'
       *  Product: '<S17>/Divide2'
       *  Sqrt: '<S17>/Reciprocal Sqrt'
       *  Sum: '<S17>/Sum1'
       */
      tmp_3 = (uint32_T)div_nde_s32_floor(rt_sqrt_Us32_Ys32_Is64_f_s(67108864L -
        (((int32_T)rtb_Abs) * rtb_Abs)) << 13, rtb_Abs);
      tmp_4 = (uint32_T)rtDWork.p_pu_d;
      sMultiWordMul(&tmp_3, 1, &tmp_4, 1, &tmp_2.chunks[0U], 2);

      /* Gain: '<S17>/Gain2' */
      sMultiWordMul(&tmp_1.chunks[0U], 2, &tmp_2.chunks[0U], 2, &tmp_0.chunks[0U],
                    3);
      sMultiWordShr(&tmp_0.chunks[0U], 3, 59U, &tmp.chunks[0U], 3);
      rtb_Divide_p = MultiWord2sLong(&tmp.chunks[0U]);

      /* Saturate: '<S17>/Saturation' incorporates:
       *  DataTypeConversion: '<S17>/Data Type Conversion4'
       */
      if (rtb_Divide_p > 32767L) {
        rtDWork.Switch_1 = MAX_int16_T;
      } else if (rtb_Divide_p < -32767L) {
        rtDWork.Switch_1 = -32767;
      } else {
        rtDWork.Switch_1 = (int16_T)rtb_Divide_p;
      }

      /* End of Saturate: '<S17>/Saturation' */
    }

    /* End of Switch: '<S17>/Switch_1' */
    /* End of Outputs for SubSystem: '<Root>/const-powerfactor' */
    break;

   case 2L:
    /* Outputs for IfAction SubSystem: '<Root>/vol-var' incorporates:
     *  ActionPort: '<S6>/Action Port'
     */
    rtDWork.p_pu_k = rtb_DataTypeConversion3;

    /* Product: '<S18>/Divide' incorporates:
     *  Gain: '<S18>/Gain'
     *  Inport: '<Root>/uac_now'
     *  Inport: '<Root>/uac_rate'
     *  Inport: '<S6>/p_pu'
     */
    rtb_Divide_p = div_nde_s32_floor(((int32_T)rtU.uac_now) << 13, rtU.uac_rate);

    /* Switch: '<S18>/Switch_1' incorporates:
     *  Inport: '<Root>/m2Q1'
     *  Inport: '<Root>/m2V1'
     *  Inport: '<Root>/m2V2'
     *  Inport: '<Root>/m2V3'
     *  Inport: '<Root>/m2V4'
     *  RelationalOperator: '<S18>/RO_1'
     *  RelationalOperator: '<S18>/RO_2'
     *  RelationalOperator: '<S18>/RO_3'
     *  RelationalOperator: '<S18>/RO_4'
     *  Switch: '<S18>/Switch_2'
     *  Switch: '<S18>/Switch_3'
     *  Switch: '<S18>/Switch_4'
     */
    if (rtb_Divide_p < rtU.m2V1) {
      rtb_Divide_p = rtU.m2Q1;
    } else if (rtb_Divide_p < rtU.m2V2) {
      /* Switch: '<S18>/Switch_2' incorporates:
       *  Gain: '<S18>/Gain1'
       *  Gain: '<S18>/Gain3'
       *  Inport: '<Root>/m2Q1'
       *  Inport: '<Root>/m2Q2'
       *  Inport: '<Root>/m2V2'
       *  Product: '<S18>/Divide2'
       *  Product: '<S18>/Divide4'
       *  Sum: '<S18>/Sum1'
       *  Sum: '<S18>/Sum2'
       *  Sum: '<S18>/Sum3'
       *  Sum: '<S18>/Sum6'
       */
      rtb_Divide_p = ((div_nde_s32_floor(((int32_T)(rtU.m2Q2 - rtU.m2Q1)) << 13,
        rtU.m2V2 - rtU.m2V1) * (rtb_Divide_p - rtU.m2V2)) >> 13) + rtU.m2Q2;
    } else if (rtb_Divide_p > rtU.m2V4) {
      /* Switch: '<S18>/Switch_3' incorporates:
       *  Inport: '<Root>/m2Q4'
       *  Switch: '<S18>/Switch_2'
       */
      rtb_Divide_p = rtU.m2Q4;
    } else if (((int16_T)rtb_Divide_p) > rtU.m2V3) {
      /* Switch: '<S18>/Switch_4' incorporates:
       *  Gain: '<S18>/Gain2'
       *  Gain: '<S18>/Gain4'
       *  Inport: '<Root>/m2Q3'
       *  Inport: '<Root>/m2Q4'
       *  Inport: '<Root>/m2V3'
       *  Inport: '<Root>/m2V4'
       *  Product: '<S18>/Divide1'
       *  Product: '<S18>/Divide3'
       *  Sum: '<S18>/Sum4'
       *  Sum: '<S18>/Sum5'
       *  Sum: '<S18>/Sum7'
       *  Sum: '<S18>/Sum8'
       *  Switch: '<S18>/Switch_2'
       *  Switch: '<S18>/Switch_3'
       */
      rtb_Divide_p = ((div_nde_s32_floor(((int32_T)(rtU.m2Q4 - rtU.m2Q3)) << 13,
        rtU.m2V4 - rtU.m2V3) * (rtb_Divide_p - rtU.m2V3)) >> 13) + rtU.m2Q3;
    } else {
      /* Switch: '<S18>/Switch_4' incorporates:
       *  Constant: '<S18>/Constant'
       *  Switch: '<S18>/Switch_2'
       *  Switch: '<S18>/Switch_3'
       */
      rtb_Divide_p = 0L;
    }

    /* End of Switch: '<S18>/Switch_1' */

    /* DataTypeConversion: '<S18>/Data Type Conversion4' */
    rtDWork.DataTypeConversion4_c = (int16_T)rtb_Divide_p;

    /* End of Outputs for SubSystem: '<Root>/vol-var' */
    break;

   case 3L:
    /* Outputs for IfAction SubSystem: '<Root>/wat-var' incorporates:
     *  ActionPort: '<S8>/Action Port'
     */
    /* DataTypeConversion: '<S20>/Data Type Conversion5' incorporates:
     *  DataTypeConversion: '<S20>/Data Type Conversion'
     *  Gain: '<S20>/Gain'
     *  Inport: '<Root>/p_rate'
     *  Product: '<S20>/Divide'
     */
    rtDWork.DataTypeConversion5 = (int16_T)div_nde_s32_floor(((int32_T)
      rtb_DataTypeConversion3) << 13, rtU.p_rate);

    /* Switch: '<S20>/Switch_1' incorporates:
     *  Inport: '<Root>/P2neg'
     *  Inport: '<Root>/P2pos'
     *  Inport: '<Root>/P3neg'
     *  Inport: '<Root>/P3pos'
     *  RelationalOperator: '<S20>/RO_1'
     *  RelationalOperator: '<S20>/RO_2'
     *  RelationalOperator: '<S20>/RO_3'
     *  RelationalOperator: '<S20>/RO_4'
     *  Switch: '<S20>/Switch_2'
     *  Switch: '<S20>/Switch_3'
     *  Switch: '<S20>/Switch_4'
     */
    if (rtDWork.DataTypeConversion5 < rtU.P3neg) {
      /* DataTypeConversion: '<S20>/Data Type Conversion4' incorporates:
       *  Inport: '<Root>/Q3neg'
       */
      rtDWork.DataTypeConversion4_m = rtU.Q3neg;
    } else if (rtDWork.DataTypeConversion5 < rtU.P2neg) {
      /* Switch: '<S20>/Switch_2' incorporates:
       *  DataTypeConversion: '<S20>/Data Type Conversion4'
       *  Gain: '<S20>/Gain1'
       *  Gain: '<S20>/Gain3'
       *  Inport: '<Root>/P2neg'
       *  Inport: '<Root>/Q2neg'
       *  Inport: '<Root>/Q3neg'
       *  Product: '<S20>/Divide2'
       *  Product: '<S20>/Divide4'
       *  Sum: '<S20>/Sum1'
       *  Sum: '<S20>/Sum2'
       *  Sum: '<S20>/Sum3'
       *  Sum: '<S20>/Sum6'
       */
      rtDWork.DataTypeConversion4_m = (int16_T)(((div_nde_s32_floor(((int32_T)
        (rtU.Q2neg - rtU.Q3neg)) << 13, rtU.P2neg - rtU.P3neg) *
        (rtDWork.DataTypeConversion5 - rtU.P2neg)) >> 13) + rtU.Q2neg);
    } else if (rtDWork.DataTypeConversion5 > rtU.P3pos) {
      /* Switch: '<S20>/Switch_3' incorporates:
       *  DataTypeConversion: '<S20>/Data Type Conversion4'
       *  Inport: '<Root>/Q3pos'
       *  Switch: '<S20>/Switch_2'
       */
      rtDWork.DataTypeConversion4_m = rtU.Q3pos;
    } else if (rtDWork.DataTypeConversion5 > rtU.P2pos) {
      /* Switch: '<S20>/Switch_4' incorporates:
       *  DataTypeConversion: '<S20>/Data Type Conversion4'
       *  Gain: '<S20>/Gain2'
       *  Gain: '<S20>/Gain4'
       *  Inport: '<Root>/P2pos'
       *  Inport: '<Root>/P3pos'
       *  Inport: '<Root>/Q2pos'
       *  Inport: '<Root>/Q3pos'
       *  Product: '<S20>/Divide1'
       *  Product: '<S20>/Divide3'
       *  Sum: '<S20>/Sum4'
       *  Sum: '<S20>/Sum5'
       *  Sum: '<S20>/Sum7'
       *  Sum: '<S20>/Sum8'
       *  Switch: '<S20>/Switch_2'
       *  Switch: '<S20>/Switch_3'
       */
      rtDWork.DataTypeConversion4_m = (int16_T)(((div_nde_s32_floor(((int32_T)
        (rtU.Q3pos - rtU.Q2pos)) << 13, rtU.P3pos - rtU.P2pos) *
        (rtDWork.DataTypeConversion5 - rtU.P2pos)) >> 13) + rtU.Q2pos);
    } else {
      /* DataTypeConversion: '<S20>/Data Type Conversion4' incorporates:
       *  Constant: '<S20>/Constant'
       *  Switch: '<S20>/Switch_2'
       *  Switch: '<S20>/Switch_3'
       *  Switch: '<S20>/Switch_4'
       */
      rtDWork.DataTypeConversion4_m = 0;
    }

    /* End of Switch: '<S20>/Switch_1' */
    /* End of Outputs for SubSystem: '<Root>/wat-var' */
    break;

   case 5L:
    /* Outputs for IfAction SubSystem: '<Root>/volt-wat' incorporates:
     *  ActionPort: '<S7>/Action Port'
     */
    rtDWork.p_pu = rtb_DataTypeConversion3;

    /* Product: '<S19>/Divide' incorporates:
     *  Gain: '<S19>/Gain'
     *  Inport: '<Root>/uac_now'
     *  Inport: '<Root>/uac_rate'
     *  Inport: '<S7>/p_pu'
     */
    rtb_Divide_p = div_nde_s32_floor(((int32_T)rtU.uac_now) << 13, rtU.uac_rate);

    /* Switch: '<S19>/Switch_1' incorporates:
     *  Inport: '<Root>/m3P1'
     *  Inport: '<Root>/m3V1'
     *  Inport: '<Root>/m3V2'
     *  RelationalOperator: '<S19>/RO_1'
     *  RelationalOperator: '<S19>/RO_3'
     *  Switch: '<S19>/Switch_3'
     */
    if (rtb_Divide_p < rtU.m3V1) {
      rtb_Divide_p = rtU.m3P1;
    } else if (rtb_Divide_p > rtU.m3V2) {
      /* Switch: '<S19>/Switch_3' incorporates:
       *  Inport: '<Root>/m3P2'
       */
      rtb_Divide_p = rtU.m3P2;
    } else {
      /* Switch: '<S19>/Switch_3' incorporates:
       *  Gain: '<S19>/Gain1'
       *  Gain: '<S19>/Gain3'
       *  Inport: '<Root>/m3P1'
       *  Inport: '<Root>/m3P2'
       *  Inport: '<Root>/m3V2'
       *  Product: '<S19>/Divide2'
       *  Product: '<S19>/Divide4'
       *  Sum: '<S19>/Sum1'
       *  Sum: '<S19>/Sum2'
       *  Sum: '<S19>/Sum3'
       *  Sum: '<S19>/Sum6'
       */
      rtb_Divide_p = ((div_nde_s32_floor(((int32_T)(rtU.m3P2 - rtU.m3P1)) << 13,
        rtU.m3V2 - rtU.m3V1) * (rtb_Divide_p - rtU.m3V2)) >> 13) + rtU.m3P2;
    }

    /* End of Switch: '<S19>/Switch_1' */

    /* DataTypeConversion: '<S19>/Data Type Conversion4' */
    rtDWork.DataTypeConversion4 = (int16_T)rtb_Divide_p;

    /* End of Outputs for SubSystem: '<Root>/volt-wat' */
    break;
  }

  /* End of SwitchCase: '<Root>/Switch Case' */

  /* Switch: '<S1>/Switch5' incorporates:
   *  Constant: '<S10>/Constant'
   *  Constant: '<S11>/Constant'
   *  Constant: '<S12>/Constant'
   *  Constant: '<S9>/Constant'
   *  Inport: '<Root>/ul_mode'
   *  RelationalOperator: '<S10>/Compare'
   *  RelationalOperator: '<S11>/Compare'
   *  RelationalOperator: '<S12>/Compare'
   *  RelationalOperator: '<S9>/Compare'
   *  Switch: '<S1>/Switch6'
   *  Switch: '<S1>/Switch7'
   *  Switch: '<S1>/Switch8'
   */
  if (rtU.ul_mode == 1) {
    rtb_DataTypeConversion3 = rtDWork.p_pu_d;
  } else if (rtU.ul_mode == 2) {
    /* Switch: '<S1>/Switch6' */
    rtb_DataTypeConversion3 = rtDWork.p_pu_k;
  } else if (rtU.ul_mode == 3) {
    /* Switch: '<S1>/Switch7' incorporates:
     *  Switch: '<S1>/Switch6'
     */
    rtb_DataTypeConversion3 = rtDWork.DataTypeConversion5;
  } else {
    if (rtU.ul_mode == 5) {
      /* Switch: '<S1>/Switch8' incorporates:
       *  Switch: '<S1>/Switch6'
       *  Switch: '<S1>/Switch7'
       */
      rtb_DataTypeConversion3 = rtDWork.p_pu;
    }
  }

  /* End of Switch: '<S1>/Switch5' */

  /* Abs: '<Root>/Abs' */
  if (rtb_DataTypeConversion3 < 0) {
    rtb_Abs = -rtb_DataTypeConversion3;
  } else {
    rtb_Abs = rtb_DataTypeConversion3;
  }

  /* End of Abs: '<Root>/Abs' */

  /* Switch: '<Root>/Switch5' incorporates:
   *  Inport: '<Root>/p_max'
   *  RelationalOperator: '<Root>/RO_2'
   */
  if (rtb_Abs < rtU.p_max) {
    rtb_Abs = rtb_DataTypeConversion3;
  } else {
    rtb_Abs = rtU.p_max;
  }

  /* End of Switch: '<Root>/Switch5' */

  /* Outport: '<Root>/p_cmd_out' incorporates:
   *  Gain: '<Root>/Gain4'
   *  Inport: '<Root>/p_rate'
   *  Product: '<Root>/Product_6'
   */
  rtY.p_cmd_out = (int16_T)((((int32_T)rtb_Abs) * rtU.p_rate) >> 13);

  /* Switch: '<S2>/Switch5' incorporates:
   *  Constant: '<S13>/Constant'
   *  Constant: '<S14>/Constant'
   *  Constant: '<S15>/Constant'
   *  Constant: '<S16>/Constant'
   *  Inport: '<Root>/ul_mode'
   *  RelationalOperator: '<S13>/Compare'
   *  RelationalOperator: '<S14>/Compare'
   *  RelationalOperator: '<S15>/Compare'
   *  RelationalOperator: '<S16>/Compare'
   *  Switch: '<S2>/Switch6'
   *  Switch: '<S2>/Switch7'
   *  Switch: '<S2>/Switch8'
   */
  if (rtU.ul_mode == 1) {
    rtb_Abs = rtDWork.Switch_1;
  } else if (rtU.ul_mode == 2) {
    /* Switch: '<S2>/Switch6' */
    rtb_Abs = rtDWork.DataTypeConversion4_c;
  } else if (rtU.ul_mode == 3) {
    /* Switch: '<S2>/Switch7' incorporates:
     *  Switch: '<S2>/Switch6'
     */
    rtb_Abs = rtDWork.DataTypeConversion4_m;
  } else if (rtU.ul_mode == 5) {
    /* Switch: '<S2>/Switch8' incorporates:
     *  Switch: '<S2>/Switch6'
     *  Switch: '<S2>/Switch7'
     */
    rtb_Abs = rtDWork.DataTypeConversion4;
  } else {
    /* Switch: '<S2>/Switch8' incorporates:
     *  DataTypeConversion: '<S5>/Data Type Conversion4'
     *  Gain: '<S5>/Gain1'
     *  Inport: '<Root>/q_cmd'
     *  Product: '<S5>/Divide4'
     *  Switch: '<S2>/Switch6'
     *  Switch: '<S2>/Switch7'
     */
    rtb_Abs = (int16_T)div_nde_s32_floor(((int32_T)rtU.q_cmd) << 13,
      rtb_DataTypeConversion1);
  }

  /* End of Switch: '<S2>/Switch5' */

  /* Sum: '<Root>/Sum1' incorporates:
   *  Inport: '<Root>/p_max'
   *  Product: '<Root>/Product_1'
   *  Product: '<Root>/Product_2'
   */
  rtb_DataTypeConversion1 = (((int32_T)rtU.p_max) * rtU.p_max) - (((int32_T)
    rtb_DataTypeConversion3) * rtb_DataTypeConversion3);

  /* Switch: '<Root>/Switch1' incorporates:
   *  Constant: '<Root>/Constant1'
   *  RelationalOperator: '<Root>/RO_1'
   */
  if (rtb_DataTypeConversion1 < 0L) {
    rtb_DataTypeConversion1 = 0L;
  }

  /* End of Switch: '<Root>/Switch1' */

  /* Sqrt: '<Root>/Sqrt' */
  rtb_DataTypeConversion1 = rt_sqrt_Us32_Ys32_Is64_f_s(rtb_DataTypeConversion1);

  /* Abs: '<Root>/Abs1' */
  if (rtb_Abs < 0) {
    rtb_Abs_0 = -rtb_Abs;
  } else {
    rtb_Abs_0 = rtb_Abs;
  }

  /* End of Abs: '<Root>/Abs1' */

  /* Switch: '<Root>/Switch2' incorporates:
   *  RelationalOperator: '<Root>/RO_3'
   *  Switch: '<Root>/Switch3'
   */
  if (rtb_Abs_0 < rtb_DataTypeConversion1) {
    rtb_DataTypeConversion1 = rtb_Abs;
  } else {
    if (!(rtb_Abs > 0)) {
      /* Switch: '<Root>/Switch3' incorporates:
       *  UnaryMinus: '<Root>/Unary Minus'
       */
      rtb_DataTypeConversion1 = -rtb_DataTypeConversion1;
    }
  }

  /* End of Switch: '<Root>/Switch2' */

  /* Product: '<Root>/Product_7' incorporates:
   *  Inport: '<Root>/p_rate'
   */
  tmp_3 = (uint32_T)rtU.p_rate;
  tmp_4 = (uint32_T)rtb_DataTypeConversion1;
  sMultiWordMul(&tmp_3, 1, &tmp_4, 1, &tmp_2.chunks[0U], 2);

  /* Gain: '<Root>/Gain5' */
  sMultiWordMul(&rtConstP.pooled1.chunks[0], 2, &tmp_2.chunks[0U], 2,
                &tmp_5.chunks[0U], 3);
  sMultiWordShr(&tmp_5.chunks[0U], 3, 59U, &tmp_0.chunks[0U], 3);

  /* Outport: '<Root>/q_cmd_out' incorporates:
   *  Gain: '<Root>/Gain5'
   */
  rtY.q_cmd_out = (int16_T)MultiWord2sLong(&tmp_0.chunks[0U]);
}

/* Model initialize function */
void feedback_control_initialize(void)
{
  /* (no initialization code required) */
}

/* Model terminate function */
void feedback_control_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
