/*
 * File: ul_mode_control.h
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

#ifndef RTW_HEADER_ul_mode_control_h_
#define RTW_HEADER_ul_mode_control_h_
#include <string.h>
#ifndef ul_mode_control_COMMON_INCLUDES_
# define ul_mode_control_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* ul_mode_control_COMMON_INCLUDES_ */

/* Macros for accessing real-time model data structure */

/* Block signals and states (auto storage) for system '<Root>' */
typedef struct {
  int32_T y_dly;                       /* '<S41>/Delay1' */
  uint32_T sum;                        /* '<S1>/MATLAB Function1' */
  int16_T u_dly;                       /* '<S41>/Delay' */
  uint16_T cnt;                        /* '<S1>/MATLAB Function1' */
  uint16_T avg;                        /* '<S1>/MATLAB Function1' */
  uint8_T icLoad;                      /* '<S41>/Delay' */
  uint8_T icLoad_e;                    /* '<S41>/Delay1' */
  boolean_T sum_not_empty;             /* '<S1>/MATLAB Function1' */
} ul_D_Work;

/* External inputs (root inport signals with auto storage) */
typedef struct {
  uint16_T ul_mode;                    /* '<Root>/ul_mode' */
  int16_T p_cmd;                       /* '<Root>/p_cmd' */
  int16_T q_cmd;                       /* '<Root>/q_cmd' */
  int16_T uac_now;                     /* '<Root>/uac_now' */
  int16_T uac_rate;                    /* '<Root>/uac_rate' */
  int16_T p_rate;                      /* '<Root>/p_rate' */
  int16_T p_max;                       /* '<Root>/p_max' */
  int16_T cosPhi;                      /* '<Root>/cosPhi' */
  int16_T cosMin;                      /* '<Root>/cosMin' */
  int16_T m2V1;                        /* '<Root>/m2V1' */
  int16_T m2V2;                        /* '<Root>/m2V2' */
  int16_T m2V3;                        /* '<Root>/m2V3' */
  int16_T m2V4;                        /* '<Root>/m2V4' */
  int16_T m2Q1;                        /* '<Root>/m2Q1' */
  int16_T m2Q2;                        /* '<Root>/m2Q2' */
  int16_T m2Q3;                        /* '<Root>/m2Q3' */
  int16_T m2Q4;                        /* '<Root>/m2Q4' */
  int16_T P3neg;                       /* '<Root>/P3neg' */
  int16_T P2neg;                       /* '<Root>/P2neg' */
  int16_T P2pos;                       /* '<Root>/P2pos' */
  int16_T P3pos;                       /* '<Root>/P3pos' */
  int16_T Q3neg;                       /* '<Root>/Q3neg' */
  int16_T Q2neg;                       /* '<Root>/Q2neg' */
  int16_T Q2pos;                       /* '<Root>/Q2pos' */
  int16_T Q3pos;                       /* '<Root>/Q3pos' */
  int16_T m3V1;                        /* '<Root>/m3V1' */
  int16_T m3V2;                        /* '<Root>/m3V2' */
  int16_T m3P1;                        /* '<Root>/m3P1' */
  int16_T m3P2;                        /* '<Root>/m3P2' */
  uint16_T lpf_times;                  /* '<Root>/lpf_times' */
  int16_T f_now;                       /* '<Root>/f_now' */
  int16_T f_rate;                      /* '<Root>/f_rate' */
  int16_T pfr_dbUF;                    /* '<Root>/pfr_dbUF' */
  int16_T pfr_kUF;                     /* '<Root>/pfr_kUF' */
  int16_T pfr_dbOF;                    /* '<Root>/pfr_dbOF' */
  int16_T pfr_kOF;                     /* '<Root>/pfr_kOF' */
  int16_T pfr_upLmt;                   /* '<Root>/pfr_upLmt' */
  int16_T pfr_lwLmt;                   /* '<Root>/pfr_lwLmt' */
  int16_T pfr_Tresp_ms;                /* '<Root>/pfr_Tresp_ms' */
  int16_T Ts_ms;                       /* '<Root>/Ts_ms' */
} ul_ExternalInputs;

/* External outputs (root outports fed by signals with auto storage) */
typedef struct {
  int16_T p_cmd_out;                   /* '<Root>/p_cmd_out' */
  int16_T q_cmd_out;                   /* '<Root>/q_cmd_out' */
  int16_T freq_lpf;                    /* '<Root>/freq_lpf' */
} ul_ExternalOutputs;

/* Block signals and states (auto storage) */
extern ul_D_Work ul_DWork;

/* External inputs (root inport signals with auto storage) */
extern ul_ExternalInputs ul_U;

/* External outputs (root outports fed by signals with auto storage) */
extern ul_ExternalOutputs ul_Y;

/* Model entry point functions */
extern void ul_mode_control_initialize(void);
extern void ul_mode_control_step(void);

/*-
 * These blocks were eliminated from the model due to optimizations:
 *
 * Block '<S3>/Switch1' : Unused code path elimination
 * Block '<S3>/Switch2' : Unused code path elimination
 * Block '<S11>/Bitwise Operator' : Unused code path elimination
 * Block '<S38>/Compare' : Unused code path elimination
 * Block '<S38>/Constant' : Unused code path elimination
 * Block '<S41>/Add1' : Unused code path elimination
 * Block '<S41>/Constant1' : Unused code path elimination
 * Block '<S41>/Constant6' : Unused code path elimination
 * Block '<S41>/Data Type Conversion1' : Unused code path elimination
 * Block '<S41>/Delay2' : Unused code path elimination
 * Block '<S41>/Display' : Unused code path elimination
 * Block '<S41>/Display1' : Unused code path elimination
 * Block '<S41>/Product4' : Unused code path elimination
 * Block '<S41>/Product5' : Unused code path elimination
 * Block '<S41>/Data Type Conversion2' : Eliminate redundant data type conversion
 */

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Note that this particular code originates from a subsystem build,
 * and has its own system numbers different from the parent model.
 * Refer to the system hierarchy for this subsystem below, and use the
 * MATLAB hilite_system command to trace the generated code back
 * to the parent model.  For example,
 *
 * hilite_system('feedback_control/ul_mode_control')    - opens subsystem feedback_control/ul_mode_control
 * hilite_system('feedback_control/ul_mode_control/Kp') - opens and selects block Kp
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'feedback_control'
 * '<S1>'   : 'feedback_control/ul_mode_control'
 * '<S2>'   : 'feedback_control/ul_mode_control/MATLAB Function1'
 * '<S3>'   : 'feedback_control/ul_mode_control/Subsystem'
 * '<S4>'   : 'feedback_control/ul_mode_control/const-powerfactor'
 * '<S5>'   : 'feedback_control/ul_mode_control/const-wat-var'
 * '<S6>'   : 'feedback_control/ul_mode_control/getbit0'
 * '<S7>'   : 'feedback_control/ul_mode_control/getbit1'
 * '<S8>'   : 'feedback_control/ul_mode_control/getbit2'
 * '<S9>'   : 'feedback_control/ul_mode_control/getbit3'
 * '<S10>'  : 'feedback_control/ul_mode_control/getbit4'
 * '<S11>'  : 'feedback_control/ul_mode_control/getbit5'
 * '<S12>'  : 'feedback_control/ul_mode_control/pre_freq_reg'
 * '<S13>'  : 'feedback_control/ul_mode_control/vol-var'
 * '<S14>'  : 'feedback_control/ul_mode_control/volt-wat'
 * '<S15>'  : 'feedback_control/ul_mode_control/wat-var'
 * '<S16>'  : 'feedback_control/ul_mode_control/Subsystem/Subsystem'
 * '<S17>'  : 'feedback_control/ul_mode_control/Subsystem/Subsystem1'
 * '<S18>'  : 'feedback_control/ul_mode_control/Subsystem/Subsystem/MATLAB Function'
 * '<S19>'  : 'feedback_control/ul_mode_control/Subsystem/Subsystem1/MATLAB Function1'
 * '<S20>'  : 'feedback_control/ul_mode_control/const-powerfactor/const_powerfactor_controller'
 * '<S21>'  : 'feedback_control/ul_mode_control/const-powerfactor/const_powerfactor_controller/MATLAB Function'
 * '<S22>'  : 'feedback_control/ul_mode_control/getbit0/Bit Shift'
 * '<S23>'  : 'feedback_control/ul_mode_control/getbit0/Compare To Zero'
 * '<S24>'  : 'feedback_control/ul_mode_control/getbit0/Bit Shift/bit_shift'
 * '<S25>'  : 'feedback_control/ul_mode_control/getbit1/Bit Shift'
 * '<S26>'  : 'feedback_control/ul_mode_control/getbit1/Compare To Zero'
 * '<S27>'  : 'feedback_control/ul_mode_control/getbit1/Bit Shift/bit_shift'
 * '<S28>'  : 'feedback_control/ul_mode_control/getbit2/Bit Shift'
 * '<S29>'  : 'feedback_control/ul_mode_control/getbit2/Compare To Zero'
 * '<S30>'  : 'feedback_control/ul_mode_control/getbit2/Bit Shift/bit_shift'
 * '<S31>'  : 'feedback_control/ul_mode_control/getbit3/Bit Shift'
 * '<S32>'  : 'feedback_control/ul_mode_control/getbit3/Compare To Zero'
 * '<S33>'  : 'feedback_control/ul_mode_control/getbit3/Bit Shift/bit_shift'
 * '<S34>'  : 'feedback_control/ul_mode_control/getbit4/Bit Shift'
 * '<S35>'  : 'feedback_control/ul_mode_control/getbit4/Compare To Zero'
 * '<S36>'  : 'feedback_control/ul_mode_control/getbit4/Bit Shift/bit_shift'
 * '<S37>'  : 'feedback_control/ul_mode_control/getbit5/Bit Shift'
 * '<S38>'  : 'feedback_control/ul_mode_control/getbit5/Compare To Zero'
 * '<S39>'  : 'feedback_control/ul_mode_control/getbit5/Bit Shift/bit_shift'
 * '<S40>'  : 'feedback_control/ul_mode_control/pre_freq_reg/lpf_1st_order'
 * '<S41>'  : 'feedback_control/ul_mode_control/pre_freq_reg/lpf_1st_order/Subsystem1'
 * '<S42>'  : 'feedback_control/ul_mode_control/vol-var/vol-var-controller'
 * '<S43>'  : 'feedback_control/ul_mode_control/volt-wat/volt-wat-controller'
 * '<S44>'  : 'feedback_control/ul_mode_control/wat-var/wat-var-controller'
 */

#include "micro.h"
typedef struct 
{
	volatile micro_set *pMs;
	ul_ExternalInputs *pRtu;
	ul_ExternalOutputs *pRty;
    int16_T pCmd;
    int16_T qCmd;
    int16_T freqCmd;
} ul_mode_opt_t; 

extern ul_mode_opt_t umo;
extern void ul_mode_task(const void *pdata);


#endif                                 /* RTW_HEADER_ul_mode_control_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
