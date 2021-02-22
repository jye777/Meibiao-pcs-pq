/*
 * File: ul_mode_control.c
 *
 * Code generated for Simulink model 'ul_mode_control'.
 *
 * Model version                  : 1.181
 * Simulink Coder version         : 8.11 (R2016b) 25-Aug-2016
 * C/C++ source code generated on : Wed Feb 03 14:20:40 2021
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: STMicroelectronics->ST10/Super10
 * Code generation objectives:
 *    1. Execution efficiency
 *    2. RAM efficiency
 * Validation result: Not run
 */

#include "ul_mode_control.h"
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
ul_D_Work ul_DWork;

/* External inputs (root inport signals with auto storage) */
ul_ExternalInputs ul_U;

/* External outputs (root outports fed by signals with auto storage) */
ul_ExternalOutputs ul_Y;
uint32_T ul_div_nzp_uus32_floor(uint32_T numerator, int32_T denominator);
int32_T ul_div_nde_s32_floor(int32_T numerator, int32_T denominator);
extern void MATLABFunction(uint32_T rtu_sqValue, uint16_T *rty_root, uint16_T
  rtp_bit);
uint32_T ul_div_nzp_uus32_floor(uint32_T numerator, int32_T denominator)
{
  uint32_T absDenominator;
  uint32_T tempAbsQuotient;
  boolean_T quotientNeedsNegation;
  absDenominator = (denominator < 0L) ? ((~((uint32_T)denominator)) + 1UL) :
    ((uint32_T)denominator);
  quotientNeedsNegation = (denominator < 0L);
  tempAbsQuotient = numerator / absDenominator;
  if (quotientNeedsNegation) {
    numerator %= absDenominator;
    if (numerator > 0UL) {
      tempAbsQuotient++;
    }
  }

  return quotientNeedsNegation ? ((uint32_T)(-((int32_T)tempAbsQuotient))) :
    tempAbsQuotient;
}

int32_T ul_div_nde_s32_floor(int32_T numerator, int32_T denominator)
{
  return ((((numerator < 0L) != (denominator < 0L)) && ((numerator % denominator)
            != 0L)) ? -1L : 0L) + (numerator / denominator);
}

/*
 * Output and update for atomic system:
 *    '<S14>/MATLAB Function'
 *    '<S15>/MATLAB Function1'
 *    '<S18>/MATLAB Function'
 */
void MATLABFunction(uint32_T rtu_sqValue, uint16_T *rty_root, uint16_T rtp_bit)
{
  uint16_T tmp;
  uint16_T i;
  uint16_T data;
  uint16_T data1;

  /* MATLAB Function 'ul_mode_control/Subsystem/Subsystem/MATLAB Function': '<S16>:1' */
  /* bit = bitshift(bit,-1)+1; */
  /* '<S16>:1:3' */
  if (rtp_bit <= 15U) {
    tmp = (1U << rtp_bit);
  } else {
    tmp = 0U;
  }

  /* '<S16>:1:5' */
  i = 0U;

  /* '<S16>:1:7' */
  data = 0U;
  while (i <= rtp_bit) {
    /* '<S16>:1:9' */
    /* '<S16>:1:10' */
    data1 = data | tmp;
    if ((((uint32_T)data1) * data1) <= rtu_sqValue) {
      /* '<S16>:1:11' */
      /* '<S16>:1:12' */
      data = data1;
    }

    /* '<S16>:1:14' */
    tmp >>= 1U;

    /* '<S16>:1:15' */
    data1 = i + 1U;
    if (data1 < i) {
      data1 = MAX_uint16_T;
    }

    i = data1;
  }

  /* '<S16>:1:18' */
  *rty_root = data;
}

/* Model step function */
void ul_mode_control_step(void)
{
  uint32_T x;
  uint32_T rtb_Product_3;
  int16_T rtb_DataTypeConversion3;
  int16_T rtb_DataTypeConversion_b;
  int16_T rtb_Switch_2_j;
  boolean_T rtb_Compare_o;
  boolean_T rtb_Compare_p;
  int16_T rtb_Switch_3_j;
  uint32_T rtb_Switch1_c;
  int32_T rtb_Divide;
  uint16_T rtb_root;
  int16_T rtb_Switch_3_a;

  /* Product: '<S15>/Product_3' incorporates:
   *  Inport: '<Root>/p_max'
   */
  rtb_Product_3 = (uint32_T)(((int32_T)ul_U.p_max) * ul_U.p_max);

  /* DataTypeConversion: '<S5>/Data Type Conversion3' incorporates:
   *  DataTypeConversion: '<S5>/Data Type Conversion1'
   *  Gain: '<S5>/Gain'
   *  Inport: '<Root>/p_cmd'
   *  Inport: '<Root>/p_rate'
   *  Product: '<S5>/Divide'
   */
  rtb_DataTypeConversion3 = (int16_T)ul_div_nde_s32_floor(((int32_T)ul_U.p_cmd) <<
    13, ul_U.p_rate);

  /* MATLAB Function: '<S1>/MATLAB Function1' incorporates:
   *  Inport: '<Root>/lpf_times'
   *  Inport: '<Root>/uac_now'
   */
  /* MATLAB Function 'ul_mode_control/MATLAB Function1': '<S2>:1' */
  if (!ul_DWork.sum_not_empty) {
    /* '<S2>:1:5' */
    ul_DWork.sum_not_empty = true;

    /* '<S2>:1:8' */
    rtb_DataTypeConversion_b = ul_U.uac_now;
    if (ul_U.uac_now < 0) {
      rtb_DataTypeConversion_b = 0;
    }

    ul_DWork.avg = (uint16_T)rtb_DataTypeConversion_b;
  }

  /* '<S2>:1:11' */
  rtb_root = ul_DWork.cnt + 1U;
  if (rtb_root < ul_DWork.cnt) {
    rtb_root = MAX_uint16_T;
  }

  ul_DWork.cnt = rtb_root;

  /* '<S2>:1:12' */
  rtb_DataTypeConversion_b = ul_U.uac_now;
  if (ul_U.uac_now < 0) {
    rtb_DataTypeConversion_b = 0;
  }

  rtb_Switch1_c = ul_DWork.sum + rtb_DataTypeConversion_b;
  if (rtb_Switch1_c < ul_DWork.sum) {
    rtb_Switch1_c = MAX_uint32_T;
  }

  ul_DWork.sum = rtb_Switch1_c;
  if (ul_DWork.cnt >= ul_U.lpf_times) {
    /* '<S2>:1:13' */
    /* '<S2>:1:14' */
    if (ul_U.lpf_times == 0U) {
      if (ul_DWork.sum == 0UL) {
        rtb_Switch1_c = 0UL;
      } else {
        rtb_Switch1_c = MAX_uint32_T;
      }
    } else {
      rtb_Switch1_c = ul_DWork.sum / ul_U.lpf_times;
      x = ul_DWork.sum - (rtb_Switch1_c * ul_U.lpf_times);
      if ((x > 0UL) && (x >= ((ul_U.lpf_times >> 1) + (((int16_T)ul_U.lpf_times)
             & 1)))) {
        rtb_Switch1_c++;
      }
    }

    if (rtb_Switch1_c > 65535UL) {
      rtb_Switch1_c = 65535UL;
    }

    ul_DWork.avg = (uint16_T)rtb_Switch1_c;

    /* '<S2>:1:15' */
    ul_DWork.cnt = 0U;

    /* '<S2>:1:16' */
    ul_DWork.sum = 0UL;
  }

  /* DataTypeConversion: '<S1>/Data Type Conversion' incorporates:
   *  MATLAB Function: '<S1>/MATLAB Function1'
   */
  /* '<S2>:1:18' */
  rtb_DataTypeConversion_b = (int16_T)ul_DWork.avg;

  /* Outputs for Atomic SubSystem: '<S1>/volt-wat' */
  /* Outputs for Atomic SubSystem: '<S9>/Bit Shift' */
  /* Switch: '<S12>/Switch_2' incorporates:
   *  Constant: '<S12>/Constant1'
   *  Constant: '<S30>/Constant'
   *  Constant: '<S9>/Constant3'
   *  DataTypeConversion: '<S36>/Data Type Conversion4'
   *  Inport: '<Root>/ul_mode'
   *  MATLAB Function: '<S29>/bit_shift'
   *  RelationalOperator: '<S30>/Compare'
   *  S-Function (sfix_bitop): '<S9>/Bitwise Operator'
   */
  /* MATLAB Function 'HDL Operations/Bit Shift/bit_shift': '<S31>:1' */
  /* '<S31>:1:6' */
  if ((ul_U.ul_mode & 16U) > 0U) {
    /* Product: '<S36>/Divide' incorporates:
     *  DataTypeConversion: '<S36>/Data Type Conversion'
     *  Gain: '<S36>/Gain'
     *  Inport: '<Root>/uac_rate'
     */
    rtb_Divide = ul_div_nde_s32_floor(((int32_T)rtb_DataTypeConversion_b) << 13,
      ul_U.uac_rate);

    /* Switch: '<S36>/Switch_1' incorporates:
     *  Inport: '<Root>/m3P1'
     *  Inport: '<Root>/m3V1'
     *  Inport: '<Root>/m3V2'
     *  RelationalOperator: '<S36>/RO_1'
     *  RelationalOperator: '<S36>/RO_3'
     *  Switch: '<S36>/Switch_3'
     */
    if (rtb_Divide < ul_U.m3V1) {
      rtb_Divide = ul_U.m3P1;
    } else if (rtb_Divide > ul_U.m3V2) {
      /* Switch: '<S36>/Switch_3' incorporates:
       *  Inport: '<Root>/m3P2'
       */
      rtb_Divide = ul_U.m3P2;
    } else {
      /* Switch: '<S36>/Switch_3' incorporates:
       *  Gain: '<S36>/Gain1'
       *  Gain: '<S36>/Gain3'
       *  Inport: '<Root>/m3P1'
       *  Inport: '<Root>/m3P2'
       *  Inport: '<Root>/m3V2'
       *  Product: '<S36>/Divide2'
       *  Product: '<S36>/Divide4'
       *  Sum: '<S36>/Sum1'
       *  Sum: '<S36>/Sum2'
       *  Sum: '<S36>/Sum3'
       *  Sum: '<S36>/Sum6'
       */
      rtb_Divide = ((ul_div_nde_s32_floor(((int32_T)(ul_U.m3P2 - ul_U.m3P1)) <<
        13, ul_U.m3V2 - ul_U.m3V1) * (rtb_Divide - ul_U.m3V2)) >> 13) +
        ul_U.m3P2;
    }

    /* End of Switch: '<S36>/Switch_1' */
    rtb_Switch_2_j = (int16_T)rtb_Divide;
  } else {
    rtb_Switch_2_j = 0;
  }

  /* End of Switch: '<S12>/Switch_2' */
  /* End of Outputs for SubSystem: '<S9>/Bit Shift' */
  /* End of Outputs for SubSystem: '<S1>/volt-wat' */

  /* Sum: '<S3>/Sum3' */
  rtb_Switch_2_j += rtb_DataTypeConversion3;

  /* Outputs for Atomic SubSystem: '<S6>/Bit Shift' */
  /* RelationalOperator: '<S21>/Compare' incorporates:
   *  Constant: '<S21>/Constant'
   *  Constant: '<S6>/Constant3'
   *  Inport: '<Root>/ul_mode'
   *  MATLAB Function: '<S20>/bit_shift'
   *  S-Function (sfix_bitop): '<S6>/Bitwise Operator'
   */
  /* MATLAB Function 'HDL Operations/Bit Shift/bit_shift': '<S22>:1' */
  /* '<S22>:1:6' */
  rtb_Compare_o = ((ul_U.ul_mode & 1U) > 0U);

  /* End of Outputs for SubSystem: '<S6>/Bit Shift' */

  /* Outputs for Atomic SubSystem: '<S1>/const-powerfactor' */
  /* Switch: '<S18>/Switch_3' incorporates:
   *  Constant: '<S18>/Constant1'
   *  Inport: '<Root>/cosPhi'
   *  RelationalOperator: '<S18>/RO_2'
   */
  if (8192 < ul_U.cosPhi) {
    rtb_Switch_3_j = 8192;
  } else {
    rtb_Switch_3_j = ul_U.cosPhi;
  }

  /* End of Switch: '<S18>/Switch_3' */

  /* Switch: '<S18>/Switch_2' incorporates:
   *  Constant: '<S18>/Constant2'
   *  RelationalOperator: '<S18>/RO_3'
   */
  if (-8192 > rtb_Switch_3_j) {
    rtb_Switch_3_j = -8192;
  }

  /* End of Switch: '<S18>/Switch_2' */

  /* MATLAB Function: '<S18>/MATLAB Function' incorporates:
   *  Constant: '<S18>/Constant'
   *  DataTypeConversion: '<S18>/Data Type Conversion1'
   *  Product: '<S18>/Divide2'
   *  Sum: '<S18>/Sum1'
   *  Switch: '<S18>/Switch1'
   */
  MATLABFunction((uint32_T)(67108864L - (((int32_T)rtb_Switch_3_j) *
    rtb_Switch_3_j)), &rtb_root, 14U);

  /* Switch: '<S4>/Switch_2' incorporates:
   *  Constant: '<S4>/Constant1'
   */
  if (rtb_Compare_o) {
    /* Abs: '<S18>/Abs' */
    if (rtb_Switch_3_j < 0) {
      rtb_Switch_3_a = -rtb_Switch_3_j;
    } else {
      rtb_Switch_3_a = rtb_Switch_3_j;
    }

    /* End of Abs: '<S18>/Abs' */

    /* Switch: '<S18>/Switch_1' incorporates:
     *  Inport: '<Root>/cosMin'
     *  RelationalOperator: '<S18>/RO_1'
     */
    if (rtb_Switch_3_a < ul_U.cosMin) {
      rtb_Switch_3_j = rtb_DataTypeConversion3;
    } else {
      /* Gain: '<S18>/Gain4' incorporates:
       *  DataTypeConversion: '<S18>/Data Type Conversion2'
       *  Gain: '<S18>/Gain1'
       *  Product: '<S18>/Divide1'
       *  Product: '<S18>/Divide3'
       */
      rtb_Divide = ((ul_div_nde_s32_floor(((int32_T)rtb_root) << 13,
        rtb_Switch_3_j) * rtb_DataTypeConversion3) >> 13);

      /* Saturate: '<S18>/Saturation' incorporates:
       *  DataTypeConversion: '<S18>/Data Type Conversion4'
       */
      if (rtb_Divide > 32767L) {
        rtb_Switch_3_j = MAX_int16_T;
      } else if (rtb_Divide < -32767L) {
        rtb_Switch_3_j = -32767;
      } else {
        rtb_Switch_3_j = (int16_T)rtb_Divide;
      }

      /* End of Saturate: '<S18>/Saturation' */
    }

    /* End of Switch: '<S18>/Switch_1' */
  } else {
    rtb_Switch_3_j = 0;
  }

  /* End of Switch: '<S4>/Switch_2' */
  /* End of Outputs for SubSystem: '<S1>/const-powerfactor' */

  /* Outputs for Atomic SubSystem: '<S7>/Bit Shift' */
  /* RelationalOperator: '<S24>/Compare' incorporates:
   *  Constant: '<S24>/Constant'
   *  Constant: '<S7>/Constant3'
   *  Inport: '<Root>/ul_mode'
   *  MATLAB Function: '<S23>/bit_shift'
   *  S-Function (sfix_bitop): '<S7>/Bitwise Operator'
   */
  /* MATLAB Function 'HDL Operations/Bit Shift/bit_shift': '<S25>:1' */
  /* '<S25>:1:6' */
  rtb_Compare_p = ((ul_U.ul_mode & 4U) > 0U);

  /* End of Outputs for SubSystem: '<S7>/Bit Shift' */

  /* Outputs for Atomic SubSystem: '<S1>/wat-var' */
  /* Switch: '<S13>/Switch_2' incorporates:
   *  Constant: '<S13>/Constant1'
   */
  if (rtb_Compare_p) {
    /* Switch: '<S37>/Switch_1' incorporates:
     *  Constant: '<S37>/Constant'
     *  DataTypeConversion: '<S37>/Data Type Conversion4'
     *  Inport: '<Root>/P2neg'
     *  Inport: '<Root>/P2pos'
     *  Inport: '<Root>/P3neg'
     *  Inport: '<Root>/P3pos'
     *  Inport: '<Root>/Q3neg'
     *  RelationalOperator: '<S37>/RO_1'
     *  RelationalOperator: '<S37>/RO_2'
     *  RelationalOperator: '<S37>/RO_3'
     *  RelationalOperator: '<S37>/RO_4'
     *  Switch: '<S37>/Switch_2'
     *  Switch: '<S37>/Switch_3'
     *  Switch: '<S37>/Switch_4'
     */
    if (rtb_DataTypeConversion3 < ul_U.P3neg) {
      rtb_DataTypeConversion3 = ul_U.Q3neg;
    } else if (rtb_DataTypeConversion3 < ul_U.P2neg) {
      /* Switch: '<S37>/Switch_2' incorporates:
       *  DataTypeConversion: '<S37>/Data Type Conversion4'
       *  Gain: '<S37>/Gain1'
       *  Gain: '<S37>/Gain3'
       *  Inport: '<Root>/P2neg'
       *  Inport: '<Root>/Q2neg'
       *  Inport: '<Root>/Q3neg'
       *  Product: '<S37>/Divide2'
       *  Product: '<S37>/Divide4'
       *  Sum: '<S37>/Sum1'
       *  Sum: '<S37>/Sum2'
       *  Sum: '<S37>/Sum3'
       *  Sum: '<S37>/Sum6'
       */
      rtb_DataTypeConversion3 = (int16_T)(((ul_div_nde_s32_floor(((int32_T)
        (ul_U.Q2neg - ul_U.Q3neg)) << 13, ul_U.P2neg - ul_U.P3neg) *
        (rtb_DataTypeConversion3 - ul_U.P2neg)) >> 13) + ul_U.Q2neg);
    } else if (rtb_DataTypeConversion3 > ul_U.P3pos) {
      /* Switch: '<S37>/Switch_3' incorporates:
       *  Inport: '<Root>/Q3pos'
       *  Switch: '<S37>/Switch_2'
       */
      rtb_DataTypeConversion3 = ul_U.Q3pos;
    } else if (rtb_DataTypeConversion3 > ul_U.P2pos) {
      /* Switch: '<S37>/Switch_4' incorporates:
       *  DataTypeConversion: '<S37>/Data Type Conversion4'
       *  Gain: '<S37>/Gain2'
       *  Gain: '<S37>/Gain4'
       *  Inport: '<Root>/P2pos'
       *  Inport: '<Root>/P3pos'
       *  Inport: '<Root>/Q2pos'
       *  Inport: '<Root>/Q3pos'
       *  Product: '<S37>/Divide1'
       *  Product: '<S37>/Divide3'
       *  Sum: '<S37>/Sum4'
       *  Sum: '<S37>/Sum5'
       *  Sum: '<S37>/Sum7'
       *  Sum: '<S37>/Sum8'
       *  Switch: '<S37>/Switch_2'
       *  Switch: '<S37>/Switch_3'
       */
      rtb_DataTypeConversion3 = (int16_T)(((ul_div_nde_s32_floor(((int32_T)
        (ul_U.Q3pos - ul_U.Q2pos)) << 13, ul_U.P3pos - ul_U.P2pos) *
        (rtb_DataTypeConversion3 - ul_U.P2pos)) >> 13) + ul_U.Q2pos);
    } else {
      rtb_DataTypeConversion3 = 0;
    }

    /* End of Switch: '<S37>/Switch_1' */
  } else {
    rtb_DataTypeConversion3 = 0;
  }

  /* End of Switch: '<S13>/Switch_2' */
  /* End of Outputs for SubSystem: '<S1>/wat-var' */

  /* Switch: '<S3>/Switch4' incorporates:
   *  Switch: '<S3>/Switch6'
   */
  if (rtb_Compare_o) {
    rtb_DataTypeConversion3 = rtb_Switch_3_j;
  } else {
    if (!rtb_Compare_p) {
      /* Switch: '<S3>/Switch6' incorporates:
       *  DataTypeConversion: '<S5>/Data Type Conversion1'
       *  DataTypeConversion: '<S5>/Data Type Conversion4'
       *  Gain: '<S5>/Gain1'
       *  Inport: '<Root>/p_rate'
       *  Inport: '<Root>/q_cmd'
       *  Product: '<S5>/Divide4'
       */
      rtb_DataTypeConversion3 = (int16_T)ul_div_nde_s32_floor(((int32_T)
        ul_U.q_cmd) << 13, ul_U.p_rate);
    }
  }

  /* End of Switch: '<S3>/Switch4' */

  /* Outputs for Atomic SubSystem: '<S1>/vol-var' */
  /* Outputs for Atomic SubSystem: '<S8>/Bit Shift' */
  /* Switch: '<S11>/Switch_2' incorporates:
   *  Constant: '<S11>/Constant1'
   *  Constant: '<S27>/Constant'
   *  Constant: '<S8>/Constant3'
   *  DataTypeConversion: '<S35>/Data Type Conversion4'
   *  Inport: '<Root>/ul_mode'
   *  MATLAB Function: '<S26>/bit_shift'
   *  RelationalOperator: '<S27>/Compare'
   *  S-Function (sfix_bitop): '<S8>/Bitwise Operator'
   */
  /* MATLAB Function 'HDL Operations/Bit Shift/bit_shift': '<S28>:1' */
  /* '<S28>:1:6' */
  if ((ul_U.ul_mode & 2U) > 0U) {
    /* Product: '<S35>/Divide' incorporates:
     *  DataTypeConversion: '<S35>/Data Type Conversion'
     *  Gain: '<S35>/Gain'
     *  Inport: '<Root>/uac_rate'
     */
    rtb_Divide = ul_div_nde_s32_floor(((int32_T)rtb_DataTypeConversion_b) << 13,
      ul_U.uac_rate);

    /* Switch: '<S35>/Switch_1' incorporates:
     *  Inport: '<Root>/m2Q1'
     *  Inport: '<Root>/m2V1'
     *  Inport: '<Root>/m2V2'
     *  Inport: '<Root>/m2V3'
     *  Inport: '<Root>/m2V4'
     *  RelationalOperator: '<S35>/RO_1'
     *  RelationalOperator: '<S35>/RO_2'
     *  RelationalOperator: '<S35>/RO_3'
     *  RelationalOperator: '<S35>/RO_4'
     *  Switch: '<S35>/Switch_2'
     *  Switch: '<S35>/Switch_3'
     *  Switch: '<S35>/Switch_4'
     */
    if (rtb_Divide < ul_U.m2V1) {
      rtb_Divide = ul_U.m2Q1;
    } else if (rtb_Divide < ul_U.m2V2) {
      /* Switch: '<S35>/Switch_2' incorporates:
       *  Gain: '<S35>/Gain1'
       *  Gain: '<S35>/Gain3'
       *  Inport: '<Root>/m2Q1'
       *  Inport: '<Root>/m2Q2'
       *  Inport: '<Root>/m2V2'
       *  Product: '<S35>/Divide2'
       *  Product: '<S35>/Divide4'
       *  Sum: '<S35>/Sum1'
       *  Sum: '<S35>/Sum2'
       *  Sum: '<S35>/Sum3'
       *  Sum: '<S35>/Sum6'
       */
      rtb_Divide = ((ul_div_nde_s32_floor(((int32_T)(ul_U.m2Q2 - ul_U.m2Q1)) <<
        13, ul_U.m2V2 - ul_U.m2V1) * (rtb_Divide - ul_U.m2V2)) >> 13) +
        ul_U.m2Q2;
    } else if (rtb_Divide > ul_U.m2V4) {
      /* Switch: '<S35>/Switch_3' incorporates:
       *  Inport: '<Root>/m2Q4'
       *  Switch: '<S35>/Switch_2'
       */
      rtb_Divide = ul_U.m2Q4;
    } else if (((int16_T)rtb_Divide) > ul_U.m2V3) {
      /* Switch: '<S35>/Switch_4' incorporates:
       *  Gain: '<S35>/Gain2'
       *  Gain: '<S35>/Gain4'
       *  Inport: '<Root>/m2Q3'
       *  Inport: '<Root>/m2Q4'
       *  Inport: '<Root>/m2V3'
       *  Inport: '<Root>/m2V4'
       *  Product: '<S35>/Divide1'
       *  Product: '<S35>/Divide3'
       *  Sum: '<S35>/Sum4'
       *  Sum: '<S35>/Sum5'
       *  Sum: '<S35>/Sum7'
       *  Sum: '<S35>/Sum8'
       *  Switch: '<S35>/Switch_2'
       *  Switch: '<S35>/Switch_3'
       */
      rtb_Divide = ((ul_div_nde_s32_floor(((int32_T)(ul_U.m2Q4 - ul_U.m2Q3)) <<
        13, ul_U.m2V4 - ul_U.m2V3) * (rtb_Divide - ul_U.m2V3)) >> 13) +
        ul_U.m2Q3;
    } else {
      /* Switch: '<S35>/Switch_4' incorporates:
       *  Constant: '<S35>/Constant'
       *  Switch: '<S35>/Switch_2'
       *  Switch: '<S35>/Switch_3'
       */
      rtb_Divide = 0L;
    }

    /* End of Switch: '<S35>/Switch_1' */
    rtb_DataTypeConversion_b = (int16_T)rtb_Divide;
  } else {
    rtb_DataTypeConversion_b = 0;
  }

  /* End of Switch: '<S11>/Switch_2' */
  /* End of Outputs for SubSystem: '<S8>/Bit Shift' */
  /* End of Outputs for SubSystem: '<S1>/vol-var' */

  /* Sum: '<S3>/Sum2' */
  rtb_DataTypeConversion_b += rtb_DataTypeConversion3;

  /* Sum: '<S15>/Sum2' incorporates:
   *  Product: '<S15>/Product_4'
   *  Product: '<S15>/Product_5'
   */
  rtb_Divide = (int32_T)(((uint32_T)(((int32_T)rtb_Switch_2_j) * rtb_Switch_2_j))
    + (((int32_T)rtb_DataTypeConversion_b) * rtb_DataTypeConversion_b));

  /* MATLAB Function: '<S15>/MATLAB Function1' incorporates:
   *  Product: '<S15>/Product_7'
   *  Product: '<S15>/Product_8'
   */
  MATLABFunction(ul_div_nzp_uus32_floor(rtb_Product_3 << 26, rtb_Divide),
                 &rtb_root, 14U);

  /* RelationalOperator: '<S15>/RO_2' */
  if (rtb_Divide < 0L) {
    rtb_Divide = 0L;
  }

  rtb_Compare_o = (((uint32_T)rtb_Divide) > rtb_Product_3);

  /* End of RelationalOperator: '<S15>/RO_2' */

  /* Switch: '<S15>/Switch7' incorporates:
   *  Gain: '<S15>/Gain2'
   *  Product: '<S15>/Product_9'
   */
  if (rtb_Compare_o) {
    rtb_DataTypeConversion3 = (int16_T)((((int32_T)rtb_root) * rtb_Switch_2_j) >>
      13);
  } else {
    rtb_DataTypeConversion3 = rtb_Switch_2_j;
  }

  /* End of Switch: '<S15>/Switch7' */

  /* Outport: '<Root>/p_cmd_out' incorporates:
   *  Gain: '<S15>/Gain4'
   *  Inport: '<Root>/p_rate'
   *  Product: '<S15>/Product_2'
   */
  ul_Y.p_cmd_out = (int16_T)((((int32_T)rtb_DataTypeConversion3) * ul_U.p_rate) >>
    13);

  /* Switch: '<S15>/Switch1' incorporates:
   *  Gain: '<S15>/Gain3'
   *  Product: '<S15>/Product_1'
   */
  if (rtb_Compare_o) {
    rtb_DataTypeConversion3 = (int16_T)((((int32_T)rtb_root) *
      rtb_DataTypeConversion_b) >> 13);
  } else {
    rtb_DataTypeConversion3 = rtb_DataTypeConversion_b;
  }

  /* End of Switch: '<S15>/Switch1' */

  /* Outport: '<Root>/q_cmd_out' incorporates:
   *  Gain: '<S15>/Gain1'
   *  Inport: '<Root>/p_rate'
   *  Product: '<S15>/Product_10'
   */
  ul_Y.q_cmd_out = (int16_T)((((int32_T)ul_U.p_rate) * rtb_DataTypeConversion3) >>
    13);

  /* Outputs for Atomic SubSystem: '<S10>/Bit Shift' */
  /* Switch: '<S3>/Switch1' incorporates:
   *  Constant: '<S10>/Constant3'
   *  Constant: '<S33>/Constant'
   *  Inport: '<Root>/ul_mode'
   *  MATLAB Function: '<S32>/bit_shift'
   *  RelationalOperator: '<S33>/Compare'
   *  S-Function (sfix_bitop): '<S10>/Bitwise Operator'
   */
  /* MATLAB Function 'HDL Operations/Bit Shift/bit_shift': '<S34>:1' */
  /* '<S34>:1:6' */
  if ((ul_U.ul_mode & 32U) > 0U) {
    rtb_DataTypeConversion3 = rtb_DataTypeConversion_b;
  } else {
    rtb_DataTypeConversion3 = rtb_Switch_2_j;
  }

  /* End of Switch: '<S3>/Switch1' */
  /* End of Outputs for SubSystem: '<S10>/Bit Shift' */

  /* Sum: '<S14>/Sum1' incorporates:
   *  Inport: '<Root>/p_max'
   *  Product: '<S14>/Product_1'
   *  Product: '<S14>/Product_2'
   */
  rtb_Divide = (((int32_T)ul_U.p_max) * ul_U.p_max) - (((int32_T)
    rtb_DataTypeConversion3) * rtb_DataTypeConversion3);

  /* Switch: '<S14>/Switch1' incorporates:
   *  Constant: '<S14>/Constant1'
   *  Constant: '<S14>/Constant2'
   *  DataTypeConversion: '<S14>/Data Type Conversion4'
   *  RelationalOperator: '<S14>/RO_1'
   */
  if (rtb_Divide < 0L) {
    rtb_Product_3 = 0UL;
  } else {
    rtb_Product_3 = (uint32_T)rtb_Divide;
  }

  /* End of Switch: '<S14>/Switch1' */

  /* MATLAB Function: '<S14>/MATLAB Function' */
  MATLABFunction(rtb_Product_3, &rtb_root, 14U);
}

/* Model initialize function */
void ul_mode_control_initialize(void)
{
  /* Registration code */

  /* states (dwork) */
  (void) memset((void *)&ul_DWork, 0,
                sizeof(ul_D_Work));

  /* external inputs */
  (void)memset((void *)(&ul_U), 0, sizeof(ul_ExternalInputs));

  /* external outputs */
  (void) memset((void *)&ul_Y, 0,
                sizeof(ul_ExternalOutputs));

  /* SystemInitialize for MATLAB Function: '<S1>/MATLAB Function1' */
  ul_DWork.sum_not_empty = false;
  ul_DWork.cnt = 0U;
  ul_DWork.sum = 0UL;
}


#include "fpga.h"
#include "para.h"
#include "tty.h"
extern short setting_data_handle(short addr, short value);


ul_mode_opt_t umo;

#define UNIT_PU	8192

enum
{
	UL_MODE_CONST_PF = 0,	//恒功率因数
	UL_MODE_VQ		 = 1,	//电压无功
	UL_MODE_PQ		 = 2,	//有功无功
	UL_MODE_CONST_Q	 = 3,	//恒无功
	UL_MODE_VP		 = 4,	//电压有功
};


static void ul_mode_judge(ul_mode_opt_t *pUmo)
{
    volatile micro_set *pMs = pUmo->pMs;
    ul_ExternalInputs *pul_U = pUmo->pRtu;
#if 0
    if(GETBIT(pMs->UL_strtg_en, 0) == 1) {
        pul_U->ul_mode = UL_MODE_CONST_Q;
    } else if(GETBIT(pMs->UL_strtg_en, 1) == 1) {
        pul_U->ul_mode = UL_MODE_CONST_PF;
    } else if(GETBIT(pMs->UL_strtg_en, 2) == 1) {
        pul_U->ul_mode = UL_MODE_PQ;
    } else if(GETBIT(pMs->UL_strtg_en, 3) == 1) {
        pul_U->ul_mode = UL_MODE_VQ;
    } else if(GETBIT(pMs->UL_strtg_en, 4) == 1) {
        pul_U->ul_mode = UL_MODE_VP;
    } else {
        pul_U->ul_mode = UL_MODE_NONE;
    }
#else
    pul_U->ul_mode = pMs->UL_strtg_en;
#endif
}

static void ul_input_vars_update(ul_mode_opt_t *pUmo)
{
    volatile micro_set *pMs = pUmo->pMs;
    ul_ExternalInputs *pul_U = pUmo->pRtu;
    
    ul_mode_judge(pUmo);
    
    pul_U->p_cmd = pUmo->pCmd;
    pul_U->q_cmd = pUmo->qCmd;
    
    if(GETBIT(pul_U->ul_mode, UL_MODE_CONST_PF) == 1) { //UL_MODE_CONST_PF
        pul_U->p_rate = setting_data_handle(Addr_Param26, pMs->P_rate * 10);
        pul_U->p_max = (int16_t)(pMs->P_max * UNIT_PU / 100);
        pul_U->cosPhi = (int16_t)(pMs->PF_x100 * UNIT_PU / 100);
        pul_U->cosMin = (int16_t)(pMs->PF_min_x100 * UNIT_PU / 100);
        printf_debug8("---p_cmd[%d] q_cmd[%d]\n", pul_U->p_cmd, pul_U->q_cmd);
        printf_debug8("---p_rate[%d] p_max[%d] cosPhi[%d] cosMin[%d]\n", pul_U->p_rate, pul_U->p_max, pul_U->cosPhi, 
pul_U->cosMin);
    } else if(GETBIT(pul_U->ul_mode, UL_MODE_VQ) == 1) { //UL_MODE_VQ
		pul_U->uac_now = (int16_t)((fpga_pdatabuf_get[Addr_Param256] + fpga_pdatabuf_get[Addr_Param257] + fpga_pdatabuf_get[
Addr_Param258]) / 3.0f);
		pul_U->uac_rate = setting_data_handle(Addr_Param256, pMs->Uac_rate * 10);
		pul_U->p_rate = setting_data_handle(Addr_Param26, pMs->P_rate * 10);
		pul_U->p_max = (int16_t)(pMs->P_max * UNIT_PU / 100);
		pul_U->m2V1 = (int16_t)(pMs->VQ_V1 * UNIT_PU / 100);
		pul_U->m2V2 = (int16_t)(pMs->VQ_V2 * UNIT_PU / 100);
		pul_U->m2V3 = (int16_t)(pMs->VQ_V3 * UNIT_PU / 100);
		pul_U->m2V4 = (int16_t)(pMs->VQ_V4 * UNIT_PU / 100);
		pul_U->m2Q1 = (int16_t)(pMs->VQ_Q1 * UNIT_PU / 100);
		pul_U->m2Q2 = (int16_t)(pMs->VQ_Q2 * UNIT_PU / 100);
		pul_U->m2Q3 = (int16_t)(pMs->VQ_Q3 * UNIT_PU / 100);
		pul_U->m2Q4 = (int16_t)(pMs->VQ_Q4 * UNIT_PU / 100);
        printf_debug8("---p_cmd[%d] q_cmd[%d]\n", pul_U->p_cmd, pul_U->q_cmd);
		printf_debug8("---uac_now[%d] uac_rate[%d] p_rate[%d] p_max[%d]\n", pul_U->uac_now, pul_U->uac_rate, pul_U->p_rate, 
pul_U->p_max);
		printf_debug8("---v1-v4[%d][%d][%d][%d]\n", pul_U->m2V1, pul_U->m2V2, pul_U->m2V3, pul_U->m2V4);
		printf_debug8("---q1-q4[%d][%d][%d][%d]\n", pul_U->m2Q1, pul_U->m2Q2, pul_U->m2Q3, pul_U->m2Q4);
    } else if(GETBIT(pul_U->ul_mode, UL_MODE_PQ) == 1) { //UL_MODE_PQ
		pul_U->p_rate = setting_data_handle(Addr_Param26, pMs->P_rate * 10);
		pul_U->p_max = (int16_t)(pMs->P_max * UNIT_PU / 100);
		pul_U->P3neg = (int16_t)(pMs->PQ_n_P3 * UNIT_PU / 100);
		pul_U->P2neg = (int16_t)(pMs->PQ_n_P2 * UNIT_PU / 100);
		pul_U->P2pos = (int16_t)(pMs->PQ_p_P2 * UNIT_PU / 100);
		pul_U->P3pos = (int16_t)(pMs->PQ_p_P3 * UNIT_PU / 100);
		pul_U->Q3neg = (int16_t)(pMs->PQ_n_Q3 * UNIT_PU / 100);
		pul_U->Q2neg = (int16_t)(pMs->PQ_n_Q2 * UNIT_PU / 100);
		pul_U->Q2pos = (int16_t)(pMs->PQ_p_Q2 * UNIT_PU / 100);
		pul_U->Q3pos = (int16_t)(pMs->PQ_p_Q3 * UNIT_PU / 100);
        printf_debug8("---p_cmd[%d] q_cmd[%d]\n", pul_U->p_cmd, pul_U->q_cmd);
		printf_debug8("---p_rate[%d] p_max[%d]\n", pul_U->p_rate, pul_U->p_max);
		printf_debug8("---pn32pp23[%d][%d][%d][%d]\n", pul_U->P3neg, pul_U->P2neg, pul_U->P2pos, pul_U->P3pos);
		printf_debug8("---qn32qp23[%d][%d][%d][%d]\n", pul_U->Q3neg, pul_U->Q2neg, pul_U->Q2pos, pul_U->Q3pos);
    } else if(GETBIT(pul_U->ul_mode, UL_MODE_CONST_Q) == 1) { //UL_MODE_CONST_Q
        printf_debug8("---p_cmd[%d] q_cmd[%d]\n", pul_U->p_cmd, pul_U->q_cmd);
    } else if(GETBIT(pul_U->ul_mode, UL_MODE_VP) == 1) { //UL_MODE_VP
		pul_U->uac_now = (int16_t)((fpga_pdatabuf_get[Addr_Param256] + fpga_pdatabuf_get[Addr_Param257] + fpga_pdatabuf_get[
Addr_Param258]) / 3.0f);
		pul_U->uac_rate = setting_data_handle(Addr_Param256, pMs->Uac_rate * 10);
		pul_U->p_rate = setting_data_handle(Addr_Param26, pMs->P_rate * 10);
		pul_U->p_max = (int16_t)(pMs->P_max * UNIT_PU / 100);
		pul_U->m3V1 = (int16_t)(pMs->VP_V1 * UNIT_PU / 100);
		pul_U->m3V2 = (int16_t)(pMs->VP_V2 * UNIT_PU / 100);
		pul_U->m3P1 = (int16_t)(pMs->VP_P1 * UNIT_PU / 100);
		pul_U->m3P2 = (int16_t)(pMs->VP_P2 * UNIT_PU / 100);
        printf_debug8("---p_cmd[%d] q_cmd[%d]\n", pul_U->p_cmd, pul_U->q_cmd);
        printf_debug8("---uac_now[%d] uac_rate[%d] p_rate[%d] p_max[%d]\n", pul_U->uac_now, pul_U->uac_rate, pul_U->
p_rate, pul_U->p_max);
		printf_debug8("---v12p12[%d][%d][%d][%d]\n", pul_U->m3V1, pul_U->m3V2, pul_U->m3P1, pul_U->m3P2);
    }
}


static void ul_mode_handle(ul_mode_opt_t *pUmo)
{
#if 0
    if(GETBIT(pUmo->pRtu->ul_mode, 5) == 0) { //Const-PF
        fpga_write(Addr_Param26, pUmo->pRty->p_cmd_out);
        fpga_write(Addr_Param27, pUmo->pRty->q_cmd_out);
        printf_debug8("------p_out[%d] q_out[%d]\n\n", pUmo->pRty->p_cmd_out, pUmo->pRty->q_cmd_out);
    } else {
        fpga_write(Addr_Param27, pUmo->pRty->p_cmd_out);
        fpga_write(Addr_Param26, pUmo->pRty->q_cmd_out);
        printf_debug8("------p_out[%d] q_out[%d]\n\n", pUmo->pRty->p_cmd_out, pUmo->pRty->q_cmd_out);
    }
#else
    fpga_write(Addr_Param26, pUmo->pRty->p_cmd_out);
    fpga_write(Addr_Param27, pUmo->pRty->q_cmd_out);
    printf_debug8("------p_out[%d] q_out[%d]\n\n", pUmo->pRty->p_cmd_out, pUmo->pRty->q_cmd_out);
#endif    
}


static void ul_mode_pwr_assign(ul_mode_opt_t *pUmo)
{
    fpga_write(Addr_Param26, pUmo->pCmd);
    fpga_write(Addr_Param27, pUmo->qCmd);
}


#define UM_TASK_PERIOD 5
void ul_mode_task(const void *pdata)
{
    ul_mode_opt_t *pUmo = (ul_mode_opt_t *)pdata;
    
    pUmo->pRtu = &ul_U;
    pUmo->pRty = &ul_Y;
    pUmo->pMs = &miro_write;
    
    ul_mode_control_initialize();

    pUmo->pRtu->lpf_times = (uint16_T)(1000 / UM_TASK_PERIOD);

    while(1)
    {
        ul_input_vars_update(pUmo);

        if((pUmo->pRtu->ul_mode & 0x1F) != 0) {
            ul_mode_control_step();
            ul_mode_handle(pUmo);
        } else {
            ul_mode_pwr_assign(pUmo);
        }
        
        osDelay(UM_TASK_PERIOD);
    }  
}


/*
 * File trailer for generated code.
 *
 * [EOF]
 */
