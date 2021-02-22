/*
 * File: feedback_control.h
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

#ifndef RTW_HEADER_feedback_control_h_
#define RTW_HEADER_feedback_control_h_
#ifndef feedback_control_COMMON_INCLUDES_
# define feedback_control_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* feedback_control_COMMON_INCLUDES_ */

/* Shared type includes */
#include "multiword_types.h"

/* Macros for accessing real-time model data structure */

/* Block signals and states (auto storage) for system '<Root>' */
typedef struct {
  int16_T p_pu;                        /* '<S7>/p_pu' */
  int16_T DataTypeConversion4;         /* '<S19>/Data Type Conversion4' */
  int16_T DataTypeConversion5;         /* '<S20>/Data Type Conversion5' */
  int16_T DataTypeConversion4_m;       /* '<S20>/Data Type Conversion4' */
  int16_T p_pu_k;                      /* '<S6>/p_pu' */
  int16_T DataTypeConversion4_c;       /* '<S18>/Data Type Conversion4' */
  int16_T p_pu_d;                      /* '<S4>/p_pu' */
  int16_T Switch_1;                    /* '<S17>/Switch_1' */
} D_Work;

/* Constant parameters (auto storage) */
typedef struct {
  /* Pooled Parameter (Expression: 1/8192)
   * Referenced by:
   *   '<Root>/Gain5'
   *   '<S17>/Gain2'
   */
  int64m_T pooled1;
} ConstParam;

/* External inputs (root inport signals with auto storage) */
typedef struct {
  int16_T ul_mode;                     /* '<Root>/ul_mode' */
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
} ExternalInputs;

/* External outputs (root outports fed by signals with auto storage) */
typedef struct {
  int16_T p_cmd_out;                   /* '<Root>/p_cmd_out' */
  int16_T q_cmd_out;                   /* '<Root>/q_cmd_out' */
} ExternalOutputs;

/* Block signals and states (auto storage) */
extern D_Work rtDWork;

/* External inputs (root inport signals with auto storage) */
extern ExternalInputs rtU;

/* External outputs (root outports fed by signals with auto storage) */
extern ExternalOutputs rtY;

/* Constant parameters (auto storage) */
extern const ConstParam rtConstP;

/* Model entry point functions */
extern void feedback_control_initialize(void);
extern void feedback_control_step(void);
extern void feedback_control_terminate(void);

/*-
 * These blocks were eliminated from the model due to optimizations:
 *
 * Block '<S3>/Gain1' : Unused code path elimination
 * Block '<S3>/Gain2' : Unused code path elimination
 * Block '<S3>/Gain4' : Unused code path elimination
 * Block '<S3>/Gain5' : Unused code path elimination
 * Block '<S3>/Product_1' : Unused code path elimination
 * Block '<S3>/Product_2' : Unused code path elimination
 * Block '<S3>/Product_3' : Unused code path elimination
 * Block '<S3>/Product_4' : Unused code path elimination
 * Block '<S3>/Product_5' : Unused code path elimination
 * Block '<S3>/Product_6' : Unused code path elimination
 * Block '<S3>/Product_7' : Unused code path elimination
 * Block '<S3>/RO_2' : Unused code path elimination
 * Block '<S3>/Sum1' : Unused code path elimination
 * Block '<S3>/Switch5' : Unused code path elimination
 * Block '<S3>/Switch6' : Unused code path elimination
 */

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'feedback_control'
 * '<S1>'   : 'feedback_control/Subsystem'
 * '<S2>'   : 'feedback_control/Subsystem1'
 * '<S3>'   : 'feedback_control/Subsystem2'
 * '<S4>'   : 'feedback_control/const-powerfactor'
 * '<S5>'   : 'feedback_control/const-wat-var'
 * '<S6>'   : 'feedback_control/vol-var'
 * '<S7>'   : 'feedback_control/volt-wat'
 * '<S8>'   : 'feedback_control/wat-var'
 * '<S9>'   : 'feedback_control/Subsystem/Compare To Constant4'
 * '<S10>'  : 'feedback_control/Subsystem/Compare To Constant5'
 * '<S11>'  : 'feedback_control/Subsystem/Compare To Constant6'
 * '<S12>'  : 'feedback_control/Subsystem/Compare To Constant7'
 * '<S13>'  : 'feedback_control/Subsystem1/Compare To Constant4'
 * '<S14>'  : 'feedback_control/Subsystem1/Compare To Constant5'
 * '<S15>'  : 'feedback_control/Subsystem1/Compare To Constant6'
 * '<S16>'  : 'feedback_control/Subsystem1/Compare To Constant7'
 * '<S17>'  : 'feedback_control/const-powerfactor/const-powerfactor-controller'
 * '<S18>'  : 'feedback_control/vol-var/vol-var-controller'
 * '<S19>'  : 'feedback_control/volt-wat/volt-wat-controller'
 * '<S20>'  : 'feedback_control/wat-var/wat-var-controller'
 */
#endif                                 /* RTW_HEADER_feedback_control_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
