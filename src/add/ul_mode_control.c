/*
 * File: ul_mode_control.c
 *
 * Code generated for Simulink model 'ul_mode_control'.
 *
 * Model version                  : 1.198
 * Simulink Coder version         : 8.11 (R2016b) 25-Aug-2016
 * C/C++ source code generated on : Sat Feb 20 14:50:40 2021
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
extern void MATLABFunction1(uint32_T rtu_sqValue, uint16_T *rty_root, uint16_T
  rtp_bit);
extern void BitShift(uint16_T rtu_u, uint16_T *rty_y);
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
 *    '<S17>/MATLAB Function1'
 *    '<S20>/MATLAB Function'
 */
void MATLABFunction1(uint32_T rtu_sqValue, uint16_T *rty_root, uint16_T rtp_bit)
{
  uint16_T tmp;
  uint16_T i;
  uint16_T data;
  uint16_T data1;

  /* MATLAB Function 'ul_mode_control/Subsystem/Subsystem1/MATLAB Function1': '<S19>:1' */
  /* bit = bitshift(bit,-1)+1; */
  /* '<S19>:1:3' */
  if (rtp_bit <= 15U) {
    tmp = (1U << rtp_bit);
  } else {
    tmp = 0U;
  }

  /* '<S19>:1:5' */
  i = 0U;

  /* '<S19>:1:7' */
  data = 0U;
  while (i <= rtp_bit) {
    /* '<S19>:1:9' */
    /* '<S19>:1:10' */
    data1 = data | tmp;
    if ((((uint32_T)data1) * data1) <= rtu_sqValue) {
      /* '<S19>:1:11' */
      /* '<S19>:1:12' */
      data = data1;
    }

    /* '<S19>:1:14' */
    tmp >>= 1U;

    /* '<S19>:1:15' */
    data1 = i + 1U;
    if (data1 < i) {
      data1 = MAX_uint16_T;
    }

    i = data1;
  }

  /* '<S19>:1:18' */
  *rty_root = data;
}

/*
 * Output and update for atomic system:
 *    '<S10>/Bit Shift'
 *    '<S11>/Bit Shift'
 */
void BitShift(uint16_T rtu_u, uint16_T *rty_y)
{
  /* MATLAB Function: '<S34>/bit_shift' */
  /* MATLAB Function 'HDL Operations/Bit Shift/bit_shift': '<S36>:1' */
  /* '<S36>:1:6' */
  *rty_y = (rtu_u << 5);
}

/* Model step function */
void ul_mode_control_step(void)
{
  uint32_T x;
  uint32_T rtb_Product_3;
  int16_T rtb_DataTypeConversion3;
  int16_T rtb_DataTypeConversion_b;
  int16_T rtb_Add_n;
  int16_T rtb_Switch_2_j;
  boolean_T rtb_Compare_o;
  boolean_T rtb_Compare_p;
  uint16_T rtb_y_p;
  int16_T rtb_Switch_3_j;
  uint32_T rtb_Switch1_c;
  int32_T rtb_Sum1;
  int32_T rtb_Divide;
  int32_T rtb_Switch5;

  /* Product: '<S17>/Product_3' incorporates:
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
  rtb_y_p = ul_DWork.cnt + 1U;
  if (rtb_y_p < ul_DWork.cnt) {
    rtb_y_p = MAX_uint16_T;
  }

  ul_DWork.cnt = rtb_y_p;

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
  /* Switch: '<S14>/Switch_2' incorporates:
   *  Constant: '<S14>/Constant1'
   *  Constant: '<S32>/Constant'
   *  Constant: '<S9>/Constant3'
   *  DataTypeConversion: '<S43>/Data Type Conversion4'
   *  Inport: '<Root>/ul_mode'
   *  MATLAB Function: '<S31>/bit_shift'
   *  RelationalOperator: '<S32>/Compare'
   *  S-Function (sfix_bitop): '<S9>/Bitwise Operator'
   */
  /* MATLAB Function 'HDL Operations/Bit Shift/bit_shift': '<S33>:1' */
  /* '<S33>:1:6' */
  if ((ul_U.ul_mode & 16U) > 0U) {
    /* Product: '<S43>/Divide' incorporates:
     *  DataTypeConversion: '<S43>/Data Type Conversion'
     *  Gain: '<S43>/Gain'
     *  Inport: '<Root>/uac_rate'
     */
    rtb_Divide = ul_div_nde_s32_floor(((int32_T)rtb_DataTypeConversion_b) << 13,
      ul_U.uac_rate);

    /* Switch: '<S43>/Switch_1' incorporates:
     *  Inport: '<Root>/m3P1'
     *  Inport: '<Root>/m3V1'
     *  Inport: '<Root>/m3V2'
     *  RelationalOperator: '<S43>/RO_1'
     *  RelationalOperator: '<S43>/RO_3'
     *  Switch: '<S43>/Switch_3'
     */
    if (rtb_Divide < ul_U.m3V1) {
      rtb_Divide = ul_U.m3P1;
    } else if (rtb_Divide > ul_U.m3V2) {
      /* Switch: '<S43>/Switch_3' incorporates:
       *  Inport: '<Root>/m3P2'
       */
      rtb_Divide = ul_U.m3P2;
    } else {
      /* Switch: '<S43>/Switch_3' incorporates:
       *  Gain: '<S43>/Gain1'
       *  Gain: '<S43>/Gain3'
       *  Inport: '<Root>/m3P1'
       *  Inport: '<Root>/m3P2'
       *  Inport: '<Root>/m3V2'
       *  Product: '<S43>/Divide2'
       *  Product: '<S43>/Divide4'
       *  Sum: '<S43>/Sum1'
       *  Sum: '<S43>/Sum2'
       *  Sum: '<S43>/Sum3'
       *  Sum: '<S43>/Sum6'
       */
      rtb_Divide = ((ul_div_nde_s32_floor(((int32_T)(ul_U.m3P2 - ul_U.m3P1)) <<
        13, ul_U.m3V2 - ul_U.m3V1) * (rtb_Divide - ul_U.m3V2)) >> 13) +
        ul_U.m3P2;
    }

    /* End of Switch: '<S43>/Switch_1' */
    rtb_Switch_2_j = (int16_T)rtb_Divide;
  } else {
    rtb_Switch_2_j = 0;
  }

  /* End of Switch: '<S14>/Switch_2' */
  /* End of Outputs for SubSystem: '<S9>/Bit Shift' */
  /* End of Outputs for SubSystem: '<S1>/volt-wat' */

  /* Delay: '<S41>/Delay' incorporates:
   *  Inport: '<Root>/f_rate'
   */
  if (ul_DWork.icLoad != 0) {
    ul_DWork.u_dly = ul_U.f_rate;
  }

  /* Gain: '<S40>/Gain' incorporates:
   *  Inport: '<Root>/pfr_Tresp_ms'
   */
  rtb_Add_n = (ul_U.pfr_Tresp_ms << 1);

  /* Sum: '<S40>/Add1' incorporates:
   *  Inport: '<Root>/Ts_ms'
   */
  rtb_Switch_3_j = ul_U.Ts_ms - rtb_Add_n;

  /* Sum: '<S40>/Add' incorporates:
   *  Inport: '<Root>/Ts_ms'
   */
  rtb_Add_n += ul_U.Ts_ms;

  /* Delay: '<S41>/Delay1' incorporates:
   *  Inport: '<Root>/f_rate'
   *  Product: '<S41>/Product6'
   */
  if (ul_DWork.icLoad_e != 0) {
    ul_DWork.y_dly = ((int32_T)rtb_Add_n) * ul_U.f_rate;
  }

  /* Sum: '<S41>/Add' incorporates:
   *  Delay: '<S41>/Delay'
   *  Delay: '<S41>/Delay1'
   *  Inport: '<Root>/Ts_ms'
   *  Inport: '<Root>/f_now'
   *  Product: '<S41>/Product'
   *  Product: '<S41>/Product1'
   *  Product: '<S41>/Product3'
   */
  rtb_Divide = ((((int32_T)ul_U.f_now) * ul_U.Ts_ms) + (((int32_T)ul_DWork.u_dly)
    * ul_U.Ts_ms)) - ul_div_nde_s32_floor(rtb_Switch_3_j * ul_DWork.y_dly,
    rtb_Add_n);

  /* DataTypeConversion: '<S41>/Data Type Conversion' incorporates:
   *  Product: '<S41>/Product2'
   */
  rtb_Add_n = (int16_T)ul_div_nde_s32_floor(rtb_Divide, rtb_Add_n);

  /* Outputs for Atomic SubSystem: '<S10>/Bit Shift' */

  /* Constant: '<S10>/Constant3' */
  BitShift(1U, &rtb_y_p);

  /* End of Outputs for SubSystem: '<S10>/Bit Shift' */

  /* Switch: '<S12>/Switch2' incorporates:
   *  Constant: '<S12>/Constant1'
   *  Constant: '<S35>/Constant'
   *  Inport: '<Root>/ul_mode'
   *  RelationalOperator: '<S35>/Compare'
   *  S-Function (sfix_bitop): '<S10>/Bitwise Operator'
   */
  if ((ul_U.ul_mode & rtb_y_p) > 0U) {
    /* Sum: '<S12>/Sum1' incorporates:
     *  Inport: '<Root>/f_rate'
     *  Inport: '<Root>/pfr_dbUF'
     */
    rtb_Sum1 = ((int32_T)ul_U.f_rate) - ul_U.pfr_dbUF;

    /* Switch: '<S12>/Switch4' incorporates:
     *  RelationalOperator: '<S12>/RO_2'
     */
    if (rtb_Sum1 > rtb_Add_n) {
      /* Sum: '<S12>/Sum3' */
      rtb_Switch5 = rtb_Sum1 - rtb_Add_n;

      /* Saturate: '<S12>/Saturation' */
      if (rtb_Switch5 > 4095L) {
        rtb_Switch5 = 4095L;
      } else {
        if (rtb_Switch5 < -4095L) {
          rtb_Switch5 = -4095L;
        }
      }

      /* End of Saturate: '<S12>/Saturation' */

      /* Product: '<S12>/Divide1' incorporates:
       *  Gain: '<S12>/Gain'
       *  Gain: '<S12>/Gain1'
       *  Inport: '<Root>/f_rate'
       *  Inport: '<Root>/pfr_kUF'
       *  Product: '<S12>/Divide'
       */
      rtb_Sum1 = ul_div_nde_s32_floor(ul_div_nde_s32_floor(rtb_Switch5 << 19,
        ul_U.f_rate) << 7, ul_U.pfr_kUF);

      /* Switch: '<S12>/Switch3' incorporates:
       *  Inport: '<Root>/pfr_upLmt'
       *  RelationalOperator: '<S12>/RO_3'
       */
      if (rtb_Sum1 > ul_U.pfr_upLmt) {
        rtb_Switch5 = ul_U.pfr_upLmt;
      } else {
        rtb_Switch5 = rtb_Sum1;
      }

      /* End of Switch: '<S12>/Switch3' */
    } else {
      /* Sum: '<S12>/Sum2' incorporates:
       *  Inport: '<Root>/f_rate'
       *  Inport: '<Root>/pfr_dbOF'
       */
      rtb_Sum1 = ((int32_T)ul_U.f_rate) + ul_U.pfr_dbOF;

      /* Switch: '<S12>/Switch5' incorporates:
       *  Constant: '<S12>/Constant2'
       *  RelationalOperator: '<S12>/RO_1'
       */
      if (rtb_Add_n > rtb_Sum1) {
        /* Sum: '<S12>/Sum4' */
        rtb_Switch5 = rtb_Sum1 - rtb_Add_n;

        /* Saturate: '<S12>/Saturation1' */
        if (rtb_Switch5 > 4095L) {
          rtb_Switch5 = 4095L;
        } else {
          if (rtb_Switch5 < -4095L) {
            rtb_Switch5 = -4095L;
          }
        }

        /* End of Saturate: '<S12>/Saturation1' */

        /* Product: '<S12>/Divide3' incorporates:
         *  Gain: '<S12>/Gain2'
         *  Gain: '<S12>/Gain3'
         *  Inport: '<Root>/f_rate'
         *  Inport: '<Root>/pfr_kOF'
         *  Product: '<S12>/Divide2'
         */
        rtb_Sum1 = ul_div_nde_s32_floor(ul_div_nde_s32_floor(rtb_Switch5 << 19,
          ul_U.f_rate) << 7, ul_U.pfr_kOF);

        /* Switch: '<S12>/Switch1' incorporates:
         *  Inport: '<Root>/pfr_lwLmt'
         *  RelationalOperator: '<S12>/RO_4'
         */
        if (rtb_Sum1 < ul_U.pfr_lwLmt) {
          rtb_Switch5 = ul_U.pfr_lwLmt;
        } else {
          rtb_Switch5 = rtb_Sum1;
        }

        /* End of Switch: '<S12>/Switch1' */
      } else {
        rtb_Switch5 = 0L;
      }

      /* End of Switch: '<S12>/Switch5' */
    }

    /* End of Switch: '<S12>/Switch4' */
  } else {
    rtb_Switch5 = 0L;
  }

  /* End of Switch: '<S12>/Switch2' */

  /* Sum: '<S3>/Sum3' */
  rtb_Sum1 = (((int32_T)rtb_DataTypeConversion3) + rtb_Switch_2_j) + rtb_Switch5;

  /* Outputs for Atomic SubSystem: '<S6>/Bit Shift' */
  /* RelationalOperator: '<S23>/Compare' incorporates:
   *  Constant: '<S23>/Constant'
   *  Constant: '<S6>/Constant3'
   *  Inport: '<Root>/ul_mode'
   *  MATLAB Function: '<S22>/bit_shift'
   *  S-Function (sfix_bitop): '<S6>/Bitwise Operator'
   */
  /* MATLAB Function 'HDL Operations/Bit Shift/bit_shift': '<S24>:1' */
  /* '<S24>:1:6' */
  rtb_Compare_o = ((ul_U.ul_mode & 1U) > 0U);

  /* End of Outputs for SubSystem: '<S6>/Bit Shift' */

  /* Outputs for Atomic SubSystem: '<S1>/const-powerfactor' */
  /* Switch: '<S20>/Switch_3' incorporates:
   *  Constant: '<S20>/Constant1'
   *  Inport: '<Root>/cosPhi'
   *  RelationalOperator: '<S20>/RO_2'
   */
  if (8192 < ul_U.cosPhi) {
    rtb_Switch_3_j = 8192;
  } else {
    rtb_Switch_3_j = ul_U.cosPhi;
  }

  /* End of Switch: '<S20>/Switch_3' */

  /* Switch: '<S20>/Switch_2' incorporates:
   *  Constant: '<S20>/Constant2'
   *  RelationalOperator: '<S20>/RO_3'
   */
  if (-8192 > rtb_Switch_3_j) {
    rtb_Switch_3_j = -8192;
  }

  /* End of Switch: '<S20>/Switch_2' */

  /* MATLAB Function: '<S20>/MATLAB Function' incorporates:
   *  Constant: '<S20>/Constant'
   *  DataTypeConversion: '<S20>/Data Type Conversion1'
   *  Product: '<S20>/Divide2'
   *  Sum: '<S20>/Sum1'
   *  Switch: '<S20>/Switch1'
   */
  MATLABFunction1((uint32_T)(67108864L - (((int32_T)rtb_Switch_3_j) *
    rtb_Switch_3_j)), &rtb_y_p, 14U);

  /* Switch: '<S4>/Switch_2' incorporates:
   *  Constant: '<S4>/Constant1'
   */
  if (rtb_Compare_o) {
    /* Abs: '<S20>/Abs' */
    if (rtb_Switch_3_j < 0) {
      rtb_Switch_2_j = -rtb_Switch_3_j;
    } else {
      rtb_Switch_2_j = rtb_Switch_3_j;
    }

    /* End of Abs: '<S20>/Abs' */

    /* Switch: '<S20>/Switch_1' incorporates:
     *  Inport: '<Root>/cosMin'
     *  RelationalOperator: '<S20>/RO_1'
     */
    if (rtb_Switch_2_j < ul_U.cosMin) {
      rtb_Switch_2_j = rtb_DataTypeConversion3;
    } else {
      /* Gain: '<S20>/Gain4' incorporates:
       *  DataTypeConversion: '<S20>/Data Type Conversion2'
       *  Gain: '<S20>/Gain1'
       *  Product: '<S20>/Divide1'
       *  Product: '<S20>/Divide3'
       */
      rtb_Switch5 = ((ul_div_nde_s32_floor(((int32_T)rtb_y_p) << 13,
        rtb_Switch_3_j) * rtb_DataTypeConversion3) >> 13);

      /* Saturate: '<S20>/Saturation' incorporates:
       *  DataTypeConversion: '<S20>/Data Type Conversion4'
       */
      if (rtb_Switch5 > 32767L) {
        rtb_Switch_2_j = MAX_int16_T;
      } else if (rtb_Switch5 < -32767L) {
        rtb_Switch_2_j = -32767;
      } else {
        rtb_Switch_2_j = (int16_T)rtb_Switch5;
      }

      /* End of Saturate: '<S20>/Saturation' */
    }

    /* End of Switch: '<S20>/Switch_1' */
  } else {
    rtb_Switch_2_j = 0;
  }

  /* End of Switch: '<S4>/Switch_2' */
  /* End of Outputs for SubSystem: '<S1>/const-powerfactor' */

  /* Outputs for Atomic SubSystem: '<S7>/Bit Shift' */
  /* RelationalOperator: '<S26>/Compare' incorporates:
   *  Constant: '<S26>/Constant'
   *  Constant: '<S7>/Constant3'
   *  Inport: '<Root>/ul_mode'
   *  MATLAB Function: '<S25>/bit_shift'
   *  S-Function (sfix_bitop): '<S7>/Bitwise Operator'
   */
  /* MATLAB Function 'HDL Operations/Bit Shift/bit_shift': '<S27>:1' */
  /* '<S27>:1:6' */
  rtb_Compare_p = ((ul_U.ul_mode & 4U) > 0U);

  /* End of Outputs for SubSystem: '<S7>/Bit Shift' */

  /* Outputs for Atomic SubSystem: '<S1>/wat-var' */
  /* Switch: '<S15>/Switch_2' incorporates:
   *  Constant: '<S15>/Constant1'
   */
  if (rtb_Compare_p) {
    /* Switch: '<S44>/Switch_1' incorporates:
     *  Constant: '<S44>/Constant'
     *  DataTypeConversion: '<S44>/Data Type Conversion4'
     *  Inport: '<Root>/P2neg'
     *  Inport: '<Root>/P2pos'
     *  Inport: '<Root>/P3neg'
     *  Inport: '<Root>/P3pos'
     *  Inport: '<Root>/Q3neg'
     *  RelationalOperator: '<S44>/RO_1'
     *  RelationalOperator: '<S44>/RO_2'
     *  RelationalOperator: '<S44>/RO_3'
     *  RelationalOperator: '<S44>/RO_4'
     *  Switch: '<S44>/Switch_2'
     *  Switch: '<S44>/Switch_3'
     *  Switch: '<S44>/Switch_4'
     */
    if (rtb_DataTypeConversion3 < ul_U.P3neg) {
      rtb_Switch_3_j = ul_U.Q3neg;
    } else if (rtb_DataTypeConversion3 < ul_U.P2neg) {
      /* Switch: '<S44>/Switch_2' incorporates:
       *  DataTypeConversion: '<S44>/Data Type Conversion4'
       *  Gain: '<S44>/Gain1'
       *  Gain: '<S44>/Gain3'
       *  Inport: '<Root>/P2neg'
       *  Inport: '<Root>/Q2neg'
       *  Inport: '<Root>/Q3neg'
       *  Product: '<S44>/Divide2'
       *  Product: '<S44>/Divide4'
       *  Sum: '<S44>/Sum1'
       *  Sum: '<S44>/Sum2'
       *  Sum: '<S44>/Sum3'
       *  Sum: '<S44>/Sum6'
       */
      rtb_Switch_3_j = (int16_T)(((ul_div_nde_s32_floor(((int32_T)(ul_U.Q2neg -
        ul_U.Q3neg)) << 13, ul_U.P2neg - ul_U.P3neg) * (rtb_DataTypeConversion3
        - ul_U.P2neg)) >> 13) + ul_U.Q2neg);
    } else if (rtb_DataTypeConversion3 > ul_U.P3pos) {
      /* Switch: '<S44>/Switch_3' incorporates:
       *  Inport: '<Root>/Q3pos'
       *  Switch: '<S44>/Switch_2'
       */
      rtb_Switch_3_j = ul_U.Q3pos;
    } else if (rtb_DataTypeConversion3 > ul_U.P2pos) {
      /* Switch: '<S44>/Switch_4' incorporates:
       *  DataTypeConversion: '<S44>/Data Type Conversion4'
       *  Gain: '<S44>/Gain2'
       *  Gain: '<S44>/Gain4'
       *  Inport: '<Root>/P2pos'
       *  Inport: '<Root>/P3pos'
       *  Inport: '<Root>/Q2pos'
       *  Inport: '<Root>/Q3pos'
       *  Product: '<S44>/Divide1'
       *  Product: '<S44>/Divide3'
       *  Sum: '<S44>/Sum4'
       *  Sum: '<S44>/Sum5'
       *  Sum: '<S44>/Sum7'
       *  Sum: '<S44>/Sum8'
       *  Switch: '<S44>/Switch_2'
       *  Switch: '<S44>/Switch_3'
       */
      rtb_Switch_3_j = (int16_T)(((ul_div_nde_s32_floor(((int32_T)(ul_U.Q3pos -
        ul_U.Q2pos)) << 13, ul_U.P3pos - ul_U.P2pos) * (rtb_DataTypeConversion3
        - ul_U.P2pos)) >> 13) + ul_U.Q2pos);
    } else {
      rtb_Switch_3_j = 0;
    }

    /* End of Switch: '<S44>/Switch_1' */
  } else {
    rtb_Switch_3_j = 0;
  }

  /* End of Switch: '<S15>/Switch_2' */
  /* End of Outputs for SubSystem: '<S1>/wat-var' */

  /* Switch: '<S3>/Switch4' incorporates:
   *  Switch: '<S3>/Switch6'
   */
  if (rtb_Compare_o) {
    rtb_Switch_3_j = rtb_Switch_2_j;
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
      rtb_Switch_3_j = (int16_T)ul_div_nde_s32_floor(((int32_T)ul_U.q_cmd) << 13,
        ul_U.p_rate);
    }
  }

  /* End of Switch: '<S3>/Switch4' */

  /* Outputs for Atomic SubSystem: '<S1>/vol-var' */
  /* Outputs for Atomic SubSystem: '<S8>/Bit Shift' */
  /* Switch: '<S13>/Switch_2' incorporates:
   *  Constant: '<S13>/Constant1'
   *  Constant: '<S29>/Constant'
   *  Constant: '<S8>/Constant3'
   *  DataTypeConversion: '<S42>/Data Type Conversion4'
   *  Inport: '<Root>/ul_mode'
   *  MATLAB Function: '<S28>/bit_shift'
   *  RelationalOperator: '<S29>/Compare'
   *  S-Function (sfix_bitop): '<S8>/Bitwise Operator'
   */
  /* MATLAB Function 'HDL Operations/Bit Shift/bit_shift': '<S30>:1' */
  /* '<S30>:1:6' */
  if ((ul_U.ul_mode & 2U) > 0U) {
    /* Product: '<S42>/Divide' incorporates:
     *  DataTypeConversion: '<S42>/Data Type Conversion'
     *  Gain: '<S42>/Gain'
     *  Inport: '<Root>/uac_rate'
     */
    rtb_Switch5 = ul_div_nde_s32_floor(((int32_T)rtb_DataTypeConversion_b) << 13,
      ul_U.uac_rate);

    /* Switch: '<S42>/Switch_1' incorporates:
     *  Inport: '<Root>/m2Q1'
     *  Inport: '<Root>/m2V1'
     *  Inport: '<Root>/m2V2'
     *  Inport: '<Root>/m2V3'
     *  Inport: '<Root>/m2V4'
     *  RelationalOperator: '<S42>/RO_1'
     *  RelationalOperator: '<S42>/RO_2'
     *  RelationalOperator: '<S42>/RO_3'
     *  RelationalOperator: '<S42>/RO_4'
     *  Switch: '<S42>/Switch_2'
     *  Switch: '<S42>/Switch_3'
     *  Switch: '<S42>/Switch_4'
     */
    if (rtb_Switch5 < ul_U.m2V1) {
      rtb_Switch5 = ul_U.m2Q1;
    } else if (rtb_Switch5 < ul_U.m2V2) {
      /* Switch: '<S42>/Switch_2' incorporates:
       *  Gain: '<S42>/Gain1'
       *  Gain: '<S42>/Gain3'
       *  Inport: '<Root>/m2Q1'
       *  Inport: '<Root>/m2Q2'
       *  Inport: '<Root>/m2V2'
       *  Product: '<S42>/Divide2'
       *  Product: '<S42>/Divide4'
       *  Sum: '<S42>/Sum1'
       *  Sum: '<S42>/Sum2'
       *  Sum: '<S42>/Sum3'
       *  Sum: '<S42>/Sum6'
       */
      rtb_Switch5 = ((ul_div_nde_s32_floor(((int32_T)(ul_U.m2Q2 - ul_U.m2Q1)) <<
        13, ul_U.m2V2 - ul_U.m2V1) * (rtb_Switch5 - ul_U.m2V2)) >> 13) +
        ul_U.m2Q2;
    } else if (rtb_Switch5 > ul_U.m2V4) {
      /* Switch: '<S42>/Switch_3' incorporates:
       *  Inport: '<Root>/m2Q4'
       *  Switch: '<S42>/Switch_2'
       */
      rtb_Switch5 = ul_U.m2Q4;
    } else if (((int16_T)rtb_Switch5) > ul_U.m2V3) {
      /* Switch: '<S42>/Switch_4' incorporates:
       *  Gain: '<S42>/Gain2'
       *  Gain: '<S42>/Gain4'
       *  Inport: '<Root>/m2Q3'
       *  Inport: '<Root>/m2Q4'
       *  Inport: '<Root>/m2V3'
       *  Inport: '<Root>/m2V4'
       *  Product: '<S42>/Divide1'
       *  Product: '<S42>/Divide3'
       *  Sum: '<S42>/Sum4'
       *  Sum: '<S42>/Sum5'
       *  Sum: '<S42>/Sum7'
       *  Sum: '<S42>/Sum8'
       *  Switch: '<S42>/Switch_2'
       *  Switch: '<S42>/Switch_3'
       */
      rtb_Switch5 = ((ul_div_nde_s32_floor(((int32_T)(ul_U.m2Q4 - ul_U.m2Q3)) <<
        13, ul_U.m2V4 - ul_U.m2V3) * (rtb_Switch5 - ul_U.m2V3)) >> 13) +
        ul_U.m2Q3;
    } else {
      /* Switch: '<S42>/Switch_4' incorporates:
       *  Constant: '<S42>/Constant'
       *  Switch: '<S42>/Switch_2'
       *  Switch: '<S42>/Switch_3'
       */
      rtb_Switch5 = 0L;
    }

    /* End of Switch: '<S42>/Switch_1' */
    rtb_DataTypeConversion3 = (int16_T)rtb_Switch5;
  } else {
    rtb_DataTypeConversion3 = 0;
  }

  /* End of Switch: '<S13>/Switch_2' */
  /* End of Outputs for SubSystem: '<S8>/Bit Shift' */
  /* End of Outputs for SubSystem: '<S1>/vol-var' */

  /* Sum: '<S3>/Sum2' */
  rtb_Switch_3_j += rtb_DataTypeConversion3;

  /* Sum: '<S17>/Sum2' incorporates:
   *  Product: '<S17>/Product_4'
   *  Product: '<S17>/Product_5'
   */
  rtb_Switch5 = (int32_T)(((uint32_T)(rtb_Sum1 * rtb_Sum1)) + (((int32_T)
    rtb_Switch_3_j) * rtb_Switch_3_j));

  /* MATLAB Function: '<S17>/MATLAB Function1' incorporates:
   *  Product: '<S17>/Product_7'
   *  Product: '<S17>/Product_8'
   */
  MATLABFunction1(ul_div_nzp_uus32_floor(rtb_Product_3 << 26, rtb_Switch5),
                  &rtb_y_p, 14U);

  /* RelationalOperator: '<S17>/RO_2' */
  if (rtb_Switch5 < 0L) {
    rtb_Switch5 = 0L;
  }

  rtb_Compare_o = (((uint32_T)rtb_Switch5) > rtb_Product_3);

  /* End of RelationalOperator: '<S17>/RO_2' */

  /* Switch: '<S17>/Switch7' incorporates:
   *  Gain: '<S17>/Gain2'
   *  Gain: '<S17>/Gain3'
   *  Product: '<S17>/Product_1'
   *  Product: '<S17>/Product_9'
   *  Switch: '<S17>/Switch1'
   */
  if (rtb_Compare_o) {
    rtb_Sum1 = (int16_T)((rtb_y_p * rtb_Sum1) >> 13);
    rtb_Switch_3_j = (int16_T)((((int32_T)rtb_y_p) * rtb_Switch_3_j) >> 13);
  }

  /* End of Switch: '<S17>/Switch7' */

  /* Outport: '<Root>/p_cmd_out' incorporates:
   *  Gain: '<S17>/Gain4'
   *  Inport: '<Root>/p_rate'
   *  Product: '<S17>/Product_2'
   */
  ul_Y.p_cmd_out = (int16_T)((rtb_Sum1 * ul_U.p_rate) >> 13);

  /* Outport: '<Root>/q_cmd_out' incorporates:
   *  Gain: '<S17>/Gain1'
   *  Inport: '<Root>/p_rate'
   *  Product: '<S17>/Product_10'
   */
  ul_Y.q_cmd_out = (int16_T)((((int32_T)ul_U.p_rate) * rtb_Switch_3_j) >> 13);

  /* Outport: '<Root>/freq_lpf' */
  ul_Y.freq_lpf = rtb_Add_n;

  /* Outputs for Atomic SubSystem: '<S11>/Bit Shift' */

  /* Constant: '<S11>/Constant3' */
  BitShift(1U, &rtb_y_p);

  /* End of Outputs for SubSystem: '<S11>/Bit Shift' */

  /* Update for Delay: '<S41>/Delay' incorporates:
   *  Update for Inport: '<Root>/f_now'
   */
  ul_DWork.icLoad = 0U;
  ul_DWork.u_dly = ul_U.f_now;

  /* Update for Delay: '<S41>/Delay1' */
  ul_DWork.icLoad_e = 0U;
  ul_DWork.y_dly = rtb_Divide;
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

  /* InitializeConditions for Delay: '<S41>/Delay' */
  ul_DWork.icLoad = 1U;

  /* InitializeConditions for Delay: '<S41>/Delay1' */
  ul_DWork.icLoad_e = 1U;

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

#define UM_TASK_PERIOD 5

enum
{
	UL_MODE_CONST_PF = 0,	//恒功率因数
	UL_MODE_VQ		 = 1,	//电压无功
	UL_MODE_PQ		 = 2,	//有功无功
	UL_MODE_CONST_Q	 = 3,	//恒无功
	UL_MODE_VP		 = 4,	//电压有功
	UL_MODE_PFR      = 6,   //一次调频模式
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
        pul_U->p_rate = pMs->P_rate;
        pul_U->p_max = pMs->P_max;
        pul_U->cosPhi = pMs->PF_x100;
        pul_U->cosMin = pMs->PF_min_x100;
        printf_debug8("---p_cmd[%d] q_cmd[%d]\n", pul_U->p_cmd, pul_U->q_cmd);
        printf_debug8("---p_rate[%d] p_max[%d] cosPhi[%d] cosMin[%d]\n", pul_U->p_rate, pul_U->p_max, pul_U->cosPhi, pul_U->cosMin);
    } else if(GETBIT(pul_U->ul_mode, UL_MODE_VQ) == 1) { //UL_MODE_VQ
		pul_U->uac_now = (int16_t)((fpga_pdatabuf_get[Addr_Param256] + fpga_pdatabuf_get[Addr_Param257] + fpga_pdatabuf_get[Addr_Param258]) / 3.0f);
		pul_U->uac_rate = pMs->Uac_rate;
		pul_U->p_rate = pMs->P_rate;
		pul_U->p_max = pMs->P_max;
		pul_U->m2V1 = pMs->VQ_V1;
		pul_U->m2V2 = pMs->VQ_V2;
		pul_U->m2V3 = pMs->VQ_V3;
		pul_U->m2V4 = pMs->VQ_V4;
		pul_U->m2Q1 = pMs->VQ_Q1;
		pul_U->m2Q2 = pMs->VQ_Q2;
		pul_U->m2Q3 = pMs->VQ_Q3;
		pul_U->m2Q4 = pMs->VQ_Q4;
        printf_debug8("---p_cmd[%d] q_cmd[%d]\n", pul_U->p_cmd, pul_U->q_cmd);
		printf_debug8("---uac_now[%d] uac_rate[%d] p_rate[%d] p_max[%d]\n", pul_U->uac_now, pul_U->uac_rate, pul_U->p_rate, 
pul_U->p_max);
	    printf_debug8("---v1-v4[%d][%d][%d][%d]\n", pul_U->m2V1, pul_U->m2V2, pul_U->m2V3, pul_U->m2V4);
		printf_debug8("---q1-q4[%d][%d][%d][%d]\n", pul_U->m2Q1, pul_U->m2Q2, pul_U->m2Q3, pul_U->m2Q4);
    } else if(GETBIT(pul_U->ul_mode, UL_MODE_PQ) == 1) { //UL_MODE_PQ
		pul_U->p_rate = pMs->P_rate;
		pul_U->p_max = pMs->P_max;
		pul_U->P3neg = pMs->PQ_n_P3;
		pul_U->P2neg = pMs->PQ_n_P2;
		pul_U->P2pos = pMs->PQ_p_P2;
		pul_U->P3pos = pMs->PQ_p_P3;
		pul_U->Q3neg = pMs->PQ_n_Q3;
		pul_U->Q2neg = pMs->PQ_n_Q2;
		pul_U->Q2pos = pMs->PQ_p_Q2;
		pul_U->Q3pos = pMs->PQ_p_Q3;
        printf_debug8("---p_cmd[%d] q_cmd[%d]\n", pul_U->p_cmd, pul_U->q_cmd);
		printf_debug8("---p_rate[%d] p_max[%d]\n", pul_U->p_rate, pul_U->p_max);
		printf_debug8("---pn32pp23[%d][%d][%d][%d]\n", pul_U->P3neg, pul_U->P2neg, pul_U->P2pos, pul_U->P3pos);
		printf_debug8("---qn32qp23[%d][%d][%d][%d]\n", pul_U->Q3neg, pul_U->Q2neg, pul_U->Q2pos, pul_U->Q3pos);
    } else if(GETBIT(pul_U->ul_mode, UL_MODE_CONST_Q) == 1) { //UL_MODE_CONST_Q
        printf_debug8("---p_cmd[%d] q_cmd[%d]\n", pul_U->p_cmd, pul_U->q_cmd);
    } else if(GETBIT(pul_U->ul_mode, UL_MODE_VP) == 1) { //UL_MODE_VP
		pul_U->uac_now = (int16_t)((fpga_pdatabuf_get[Addr_Param256] + fpga_pdatabuf_get[Addr_Param257] + fpga_pdatabuf_get[Addr_Param258]) / 3.0f);
		pul_U->uac_rate = pMs->Uac_rate;
		pul_U->p_rate = pMs->P_rate;
		pul_U->p_max = pMs->P_max;
		pul_U->m3V1 = pMs->VP_V1;
		pul_U->m3V2 = pMs->VP_V2;
		pul_U->m3P1 = pMs->VP_P1;
		pul_U->m3P2 = pMs->VP_P2;
        printf_debug8("---p_cmd[%d] q_cmd[%d]\n", pul_U->p_cmd, pul_U->q_cmd);
        printf_debug8("---uac_now[%d] uac_rate[%d] p_rate[%d] p_max[%d]\n", pul_U->uac_now, pul_U->uac_rate, pul_U->p_rate, pul_U->p_max);
		printf_debug8("---v12p12[%d][%d][%d][%d]\n", pul_U->m3V1, pul_U->m3V2, pul_U->m3P1, pul_U->m3P2);
    } else if(GETBIT(pul_U->ul_mode, UL_MODE_PFR) == 1) {
        pul_U->f_now = fpga_pdatabuf_get[Addr_Param271];
		pul_U->f_rate = pMs->Freq_rate;
        pul_U->pfr_dbUF = pMs->Pfr_dbUF;
        pul_U->pfr_kUF = pMs->Pfr_kUF;
        pul_U->pfr_dbOF = pMs->Pfr_dbOF;
        pul_U->pfr_kOF = pMs->Pfr_kOF;
        pul_U->pfr_upLmt = fpga_pdatabuf_get[Addr_Param30];
        pul_U->pfr_lwLmt = fpga_pdatabuf_get[Addr_Param29];
        pul_U->pfr_Tresp_ms = pMs->Pfr_Tresp_ms;
        pul_U->Ts_ms = UM_TASK_PERIOD;
        printf_debug8("---p_cmd[%d] q_cmd[%d]\n", pul_U->p_cmd, pul_U->q_cmd);
        printf_debug8("---f_now[%d] f_rate[%d]\n", pul_U->f_now, pul_U->f_rate);
        printf_debug8("---pfr_dbUF[%d] pfr_kUF[%d] pfr_dbOF[%d] pfr_kOF[%d]\n", pul_U->pfr_dbUF, pul_U->pfr_kUF, pul_U->pfr_dbOF, pul_U->pfr_kOF);
        printf_debug8("---pfr_upLmt[%d] pfr_lwLmt[%d]\n", pul_U->pfr_upLmt, pul_U->pfr_lwLmt);
        printf_debug8("---pfr_Tresp_ms[%d] Ts_ms[%d]\n", pul_U->pfr_Tresp_ms, pul_U->Ts_ms);
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
    fpga_write(Addr_Param54, pUmo->pRty->freq_lpf);
    printf_debug8("------p_out[%d] q_out[%d] freq_lpf[%d]\n\n", pUmo->pRty->p_cmd_out, pUmo->pRty->q_cmd_out, pUmo->pRty->freq_lpf);
#endif    
}


static void ul_mode_pwr_assign(ul_mode_opt_t *pUmo)
{
    fpga_write(Addr_Param26, pUmo->pCmd);
    fpga_write(Addr_Param27, pUmo->qCmd);
    fpga_write(Addr_Param54, pUmo->freqCmd);
}


void ul_mode_task(const void *pdata)
{
    ul_mode_opt_t *pUmo = (ul_mode_opt_t *)pdata;
    
    pUmo->pRtu = &ul_U;
    pUmo->pRty = &ul_Y;
    pUmo->pMs = &miro_write;
    
    ul_mode_control_initialize();

    //pUmo->pRtu->lpf_times = (uint16_T)(1000 / UM_TASK_PERIOD);
    pUmo->pRtu->lpf_times = pUmo->pMs->Lpf_times;

    while(1)
    {
        ul_input_vars_update(pUmo);

        if((pUmo->pRtu->ul_mode & 0x5F) != 0) {
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
