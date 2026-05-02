/********************************************************************************
  * @file    cce4511_reg_def.h
  * @author  Sanjana S , Calixto Firmware Team
  * @version V1.0.0
  * @date    10-Jan-2026
  * @brief   This file contains all the CCE4511 Register Address.
  *
  ******************************************************************************
  * <h2><center>&copy; COPYRIGHT 2026 Calixto Systems Pvt Ltd</center></h2>
  ******************************************************************************
  */

#ifndef CCE4511_REG_DEF_H_INCLUDED
#define CCE4511_REG_DEF_H_INCLUDED

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Defs ----------------------------------------------------------------------*/

 /** Number of channel registers. */
 #define CCE4511_NUM_OF_CHN                                        (4U)

 /** Number of channel registers. */
 #define CCE4511_NUM_OF_REG_CHN                                    (32U)

 /*** CFG1 registers ***/

 /** CCE4511 PHY CFG1 Channel register Gate driver enable field bitmask */
 #define CCE4511_REG_CFG1_GTEN_MSK                                 (0x80U)
 /** CCE4511 PHY CFG1 Channel register Gate driver enabled bitmask */
 #define CCE4511_REG_CFG1_GTEN_EN                                  (0x80U)
 /** CCE4511 PHY CFG1 Channel register Gate driver disabled bitmask */
 #define CCE4511_REG_CFG1_GTEN_DIS                                 (0x00U)

 /** CCE4511 PHY CFG1 Channel register Output stage configuration field bitmask */
 #define CCE4511_REG_CFG1_DRV_MSK                                  (0x60U)
 /** CCE4511 PHY CFG1 Channel register Output stage configuration N-mode bitmask */
 #define CCE4511_REG_CFG1_DRV_NPN                                  (0x20U)
 /** CCE4511 PHY CFG1 Channel register Output stage configuration P-mode bitmask */
 #define CCE4511_REG_CFG1_DRV_PNP                                  (0x40U)
 /** CCE4511 PHY CFG1 Channel register Output stage configuration Push-Pull bitmask */
 #define CCE4511_REG_CFG1_DRV_PP                                   (0x60U)
 /** CCE4511 PHY CFG1 Channel register Output stage configuration CQ output disabled bitmask */
 #define CCE4511_REG_CFG1_DRV_DIS                                  (0x00U)

 /** CCE4511 PHY CFG1 Channel register Channel Source Select in SIO-Mode field bitmask */
 #define CCE4511_REG_CFG1_CSS_MSK                                  (0x10U)

 /** CCE4511 PHY CFG1 Channel register UART communication speed field bitmask */
 #define CCE4511_REG_CFG1_COM_MSK                                  (0x0CU)
 /** CCE4511 PHY CFG1 Channel register UART communication speed disabled bitmask */
 #define CCE4511_REG_CFG1_COM_DIS                                  (0x00U)
 /** CCE4511 PHY CFG1 Channel register UART communication speed COM1 bitmask */
 #define CCE4511_REG_CFG1_COM_COM1                                 (0x04U)
 /** CCE4511 PHY CFG1 Channel register UART communication speed COM2 bitmask */
 #define CCE4511_REG_CFG1_COM_COM2                                 (0x08U)
 /** CCE4511 PHY CFG1 Channel register UART communication speed COM3 bitmask */
 #define CCE4511_REG_CFG1_COM_COM3                                 (0x0CU)

 /** CCE4511 PHY CFG1 Channel register channel operation mode field bitmask */
 #define CCE4511_REG_CFG1_MODE_MSK                                 (0x03U)
 /** CCE4511 PHY CFG1 Channel register channel operation mode Standard I/O (SIO) bitmask */
 #define CCE4511_REG_CFG1_MODE_SIO                                 (0x00U)
 /** CCE4511 PHY CFG1 Channel register channel operation mode UART bitmask */
 #define CCE4511_REG_CFG1_MODE_UART                                (0x01U)
 /** CCE4511 PHY CFG1 Channel register channel operation mode Frame Handler bitmask */
 #define CCE4511_REG_CFG1_MODE_FRAMEHANDLER                        (0x02U)

 /*** CFG2 registers ***/

 /** CCE4511 PHY CFG2 Channel register UART frame transmission delay field bitmask */
 #define CCE4511_REG_CFG2_TOUT_GAP_RLX_MSK                         (0x80U)
 /** CCE4511 PHY CFG2 Channel register UART frame transmission delay strict timeout (<=3Tbit) bitmask */
 #define CCE4511_REG_CFG2_TOUT_GAP_RLX_STRICT                      (0x00U)
 /** CCE4511 PHY CFG2 Channel register UART frame transmission delay relaxed timeout (<=6Tbit) bitmask */
 #define CCE4511_REG_CFG2_TOUT_GAP_RLX_RELAXED                     (0x80U)

 /** CCE4511 PHY CFG2 Channel register CQ driver overcurrent detection field bitmask */
 #define CCE4511_REG_CFG2_CQ_OVC_EN_MSK                            (0x40U)
 /** CCE4511 PHY CFG2 Channel register CQ driver overcurrent detection disabled bitmask */
 #define CCE4511_REG_CFG2_CQ_OVC_EN_DIS                            (0x00U)
 /** CCE4511 PHY CFG2 Channel register CQ driver overcurrent detection enabled bitmask */
 #define CCE4511_REG_CFG2_CQ_OVC_EN_EN                             (0x40U)

 /** CCE4511 PHY CFG2 Channel register CQ driver current limit field bitmask */
 #define CCE4511_REG_CFG2_CQ_CLA_EN_MSK                            (0x20U)
 /** CCE4511 PHY CFG2 Channel register CQ driver current limit disabled bitmask */
 #define CCE4511_REG_CFG2_CQ_CLA_EN_DIS                            (0x00U)
 /** CCE4511 PHY CFG2 Channel register CQ driver current limit enabled bitmask */
 #define CCE4511_REG_CFG2_CQ_CLA_EN_EN                             (0x20U)

 /** CCE4511 PHY CFG2 Channel register CQ input buffer control field bitmask*/
 #define CCE4511_REG_CFG2_CQ_IBUF_CNTRL_MSK                        (0x18U)
 /** CCE4511 PHY CFG2 Channel register CQ input buffer control ratiometric thresholds bitmask*/
 #define CCE4511_REG_CFG2_CQ_IBUF_CNTRL_RATIOM                     (0x08U)
 /** CCE4511 PHY CFG2 Channel register CQ input buffer control fixed thresholds bitmask*/
 #define CCE4511_REG_CFG2_CQ_IBUF_CNTRL_FIXED                      (0x10U)
 /** CCE4511 PHY CFG2 Channel register CQ input buffer control combined thresholds bitmask*/
 #define CCE4511_REG_CFG2_CQ_IBUF_CNTRL_COMBINED                   (0x18U)

 /** CCE4511 PHY CFG2 Channel register Current sink value field bitmask*/
 #define CCE4511_REG_CFG2_ICQ_SEL_MSK                              (0x06U)
 /** CCE4511 PHY CFG2 Channel register Current sink value 2.5mA bitmask*/
 #define CCE4511_REG_CFG2_ICQ_SEL_2_5_MA                           (0x00U)
 /** CCE4511 PHY CFG2 Channel register Current sink value 5mA bitmask*/
 #define CCE4511_REG_CFG2_ICQ_SEL_5_MA                             (0x02U)
 /** CCE4511 PHY CFG2 Channel register Current sink value 7.5mA bitmask*/
 #define CCE4511_REG_CFG2_ICQ_SEL_7_5_MA                           (0x04U)
 /** CCE4511 PHY CFG2 Channel register Current sink value 10mA bitmask*/
 #define CCE4511_REG_CFG2_ICQ_SEL_10_MA                            (0x06U)

 /** CCE4511 PHY CFG2 Channel register Current sink configuration for CQ field bitmask*/
 #define CCE4511_REG_CFG2_ICQ_EN_MSK                               (0x01U)
 /** CCE4511 PHY CFG2 Channel register Current sink configuration for CQ enabled bitmask*/
 #define CCE4511_REG_CFG2_ICQ_EN_EN                                (0x01U)
 /** CCE4511 PHY CFG2 Channel register Current sink configuration for CQ disabled bitmask*/
 #define CCE4511_REG_CFG2_ICQ_EN_DIS                               (0x00U)

 /*** CFG3 registers ***/

 /** CCE4511 PHY CFG3 Channel register External NMOS current limit (gate regulation) field bitmask*/
 #define CCE4511_REG_CFG3_LPX_CLA_EN_MSK                           (0x80U)

 /** CCE4511 PHY CFG3 Channel register NMOS automatic switch-on after tLPXCLADIS field bitmask*/
 #define CCE4511_REG_CFG3_LPX_CLA_POWER_ON_MSK                     (0x40U)

 /** CCE4511 PHY CFG3 Channel register External NMOS overcurrent detection enable field bitmask*/
 #define CCE4511_REG_CFG3_LPX_OVC_EN_MSK                           (0x20U)

 /** CCE4511 PHY CFG3 Channel register NMOS automatic switch-on after tLPXOVCDIS field bitmask*/
 #define CCE4511_REG_CFG3_LPX_OVC_POWER_ON_MSK                     (0x10U)

 /** CCE4511 PHY CFG3 Channel register Channel CQ Output slew rate field bitmask*/
 #define CCE4511_REG_CFG3_SLEW_MSK                                 (0x0CU)
 /** CCE4511 PHY CFG3 Channel register Channel CQ Output slew rate 15V/us bitmask*/
 #define CCE4511_REG_CFG3_SLEW_15                                  (0x00U)
 /** CCE4511 PHY CFG3 Channel register Channel CQ Output slew rate 30V/us bitmask*/
 #define CCE4511_REG_CFG3_SLEW_30                                  (0x04U)
 /** CCE4511 PHY Channel register CFG3 Channel CQ Output slew rate 45V/us bitmask*/
 #define CCE4511_REG_CFG3_SLEW_45                                  (0x08U)
 /** CCE4511 PHY Channel register CFG3 Channel CQ Output slew rate 55V/us bitmask*/
 #define CCE4511_REG_CFG3_SLEW_55                                  (0x0CU)

 /** CCE4511 PHY CFG3 Channel register Response time of Devices (tA) timeout field bitmask*/
 #define CCE4511_REG_CFG3_TOUT_RESPONSE_RLX_MSK                    (0x03U)
 /** CCE4511 PHY CFG3 Channel register Response time of Devices (tA) timeout 10Tbit bitmask*/
 #define CCE4511_REG_CFG3_TOUT_RESPONSE_RLX_10_TBIT                (0x00U)
 /** CCE4511 PHY CFG3 Channel register Response time of Devices (tA) timeout 20Tbit bitmask*/
 #define CCE4511_REG_CFG3_TOUT_RESPONSE_RLX_20_TBIT                (0x01U)
 /** CCE4511 PHY CFG3 Channel register Response time of Devices (tA) timeout 30Tbit bitmask*/
 #define CCE4511_REG_CFG3_TOUT_RESPONSE_RLX_30_TBIT                (0x02U)
 /** CCE4511 PHY CFG3 Channel register Response time of Devices (tA) timeout 40Tbit bitmask*/
 #define CCE4511_REG_CFG3_TOUT_RESPONSE_RLX_40_TBIT                (0x03U)

 /*** SIO registers ***/

 /** CCE4511 PHY SIO Channel register Startup sequencer field bitmask*/
 #define CCE4511_REG_SIO_STARTUP_SEQ_MSK                           (0xC0U)
 /** CCE4511 PHY SIO Channel register Startup sequencer Start automated wake-up procedure bitmask*/
 #define CCE4511_REG_SIO_STARTUP_SEQ_WURQ                          (0x40U)
 /** CCE4511 PHY SIO Channel register Startup sequencer Start automated ready-pulse detection bitmask*/
 #define CCE4511_REG_SIO_STARTUP_SEQ_READY_PULSE                   (0x80U)
 /** CCE4511 PHY SIO Channel register Startup sequencer Start automated ready-pulse detection followed by an automated wake-up pulse bitmask*/
 #define CCE4511_REG_SIO_STARTUP_SEQ_READY_PULSE_WURQ              (0xC0U)

 /** CCE4511 PHY SIO Channel register automatic SKIP field bitmask*/
 #define CCE4511_REG_SIO_AUTO_SKIP_MSK                             (0x04U)

 /** CCE4511 PHY SIO Channel register CQ input value field bitmask*/
 #define CCE4511_REG_SIO_CQ_IN_MSK                                 (0x02U)

 /** CCE4511 PHY SIO Channel register CQ driver output value field bitmask*/
 #define CCE4511_REG_SIO_CQ_OUT_MSK                                (0x01U)
 /** CCE4511 PHY SIO Channel register CQ driver output value low bitmask*/
 #define CCE4511_REG_SIO_CQ_OUT_LOW                                (0x01U)
 /** CCE4511 PHY SIO Channel register CQ driver output value high bitmask*/
 #define CCE4511_REG_SIO_CQ_OUT_HIGH                               (0x00U)

 /*** FHC registers ***/

 /** CCE4511 PHY FHC Channel register Frame handler reset field bitmask*/
 #define CCE4511_REG_FHC_RST_MSK                                   (0x80U)

 /** CCE4511 PHY FHC Channel register Frame handler frame skip field bitmask*/
 #define CCE4511_REG_FHC_SKIP_MSK                                  (0x40U)

 /** CCE4511 PHY FHC Channel register Automatic CRC calculation mode field bitmask*/
 #define CCE4511_REG_FHC_CRC_MSK                                   (0x20U)
 /** CCE4511 PHY FHC Channel register Automatic CRC calculation mode enabled bitmask*/
 #define CCE4511_REG_FHC_CRC_EN                                    (0x20U)
 /** CCE4511 PHY FHC Channel register Automatic CRC calculation mode disabled bitmask*/
 #define CCE4511_REG_FHC_CRC_DIS                                   (0x00U)

 /*** CYCT registers ***/

 /** CCE4511 PHY CYCT Channel register Cycle timer multiplier field bitmask*/
 #define CCE4511_REG_CYCT_MULT_MSK                                 (0x3FU)

 /** CCE4511 PHY CYCT Channel register Cycle timer base/offset field bitmask*/
 #define CCE4511_REG_CYCT_BASE_MSK                                 (0xC0U)
 /** CCE4511 PHY CYCT Channel register Cycle timer base/offset BASE:100us, no OFFSET bitmask*/
 #define CCE4511_REG_CYCT_BASE_B100_O0                             (0x00U)
 /** CCE4511 PHY CYCT Channel register Cycle timer base/offset BASE:400us, OFFSET:6.4ms bitmask*/
 #define CCE4511_REG_CYCT_BASE_B140_O6P4                           (0x40U)
 /** CCE4511 PHY CYCT Channel register Cycle timer base/offset BASE:1.6ms, OFFSET:32ms bitmask*/
 #define CCE4511_REG_CYCT_BASE_B1P6_O32                            (0x80U)

 /*** STATUS_MODE_FH registers ***/

 /** CCE4511 PHY STATUS_MODE_FH Channel register Status of overvoltage detection at LP field bitmask*/
 #define CCE4511_REG_STATUS_MODE_FH_LP_OV_MSK                      (0x80U)

 /** CCE4511 PHY STATUS_MODE_FH Channel register Status of undervoltage detection at LP field bitmask*/
 #define CCE4511_REG_STATUS_MODE_FH_LP_UV_MSK                      (0x40U)

 /** CCE4511 PHY STATUS_MODE_FH Channel register Status of overcurrent- or current-limit-detection at LP field bitmask*/
 #define CCE4511_REG_STATUS_MODE_FH_LP_OVC_OR_CLA_MSK              (0x20U)

 /** CCE4511 PHY STATUS_MODE_FH Channel register Status of overcurrent-detection at CQ field bitmask*/
 #define CCE4511_REG_STATUS_MODE_FH_CQ_OVC_MSK                     (0x10U)

 /** CCE4511 PHY STATUS_MODE_FH Channel register Frame handler timeout field bitmask*/
 #define CCE4511_REG_STATUS_MODE_FH_TOUT_STAT_MSK                  (0x08U)

 /** CCE4511 PHY STATUS_MODE_FH Channel register Frame handler state field bitmask*/
 #define CCE4511_REG_STATUS_MODE_FH_STATE_MSK                      (0x07U)
 /** CCE4511 PHY STATUS_MODE_FH Channel register Frame handler state IDLE bitmask*/
 #define CCE4511_REG_STATUS_MODE_FH_STATE_IDLE                     (0x00U)
 /** CCE4511 PHY STATUS_MODE_FH Channel register Frame handler state Output required bitmask*/
 #define CCE4511_REG_STATUS_MODE_FH_STATE_TX_OUTPUT_REQ            (0x01U)
 /** CCE4511 PHY STATUS_MODE_FH Channel register Frame handler state Transmission active, no output required bitmask*/
 #define CCE4511_REG_STATUS_MODE_FH_STATE_TX_ACTIVE_OUTPUT_NOT_REQ (0x02U)
 /** CCE4511 PHY STATUS_MODE_FH Channel register Frame handler state Transmission active, output required bitmask*/
 #define CCE4511_REG_STATUS_MODE_FH_STATE_TX_ACTIVE_OUTPUT_REQ     (0x03U)
 /** CCE4511 PHY STATUS_MODE_FH Channel register Frame handler state Receiving active bitmask*/
 #define CCE4511_REG_STATUS_MODE_FH_STATE_RX_ACTIVE                (0x04U)
 /** CCE4511 PHY STATUS_MODE_FH Channel register Frame handler state Receiving active, input available bitmask*/
 #define CCE4511_REG_STATUS_MODE_FH_STATE_RX_ACTIVE_INPUT_AVL      (0x05U)
 /** CCE4511 PHY STATUS_MODE_FH Channel register Frame handler state Receiving active, error bitmask*/
 #define CCE4511_REG_STATUS_MODE_FH_STATE_RX_ACTIVE_ERR            (0x06U)
 /** CCE4511 PHY STATUS_MODE_FH Channel register Frame handler state Receiving active, input available, error bitmask*/
 #define CCE4511_REG_STATUS_MODE_FH_STATE_RX_ACTIVE_ERR_INPUT_AVL  (0x07U)

 /*** INT_EN_STAT registers ***/

 /** CCE4511 PHY INT_EN_STAT Channel register LP_OV interrupt field bitmask*/
 #define CCE4511_REG_INT_EN_STAT_IE_LP_OV_MSK                      (0x80U)
 /** CCE4511 PHY INT_EN_STAT Channel register LP_OV interrupt enabled bitmask*/
 #define CCE4511_REG_INT_EN_STAT_IE_LP_OV_EN                       (0x80U)
 /** CCE4511 PHY INT_EN_STAT Channel register LP_OV interrupt disabled bitmask*/
 #define CCE4511_REG_INT_EN_STAT_IE_LP_OV_DIS                      (0x00U)

 /** CCE4511 PHY INT_EN_STAT Channel register LP_UV interrupt field bitmask*/
 #define CCE4511_REG_INT_EN_STAT_IE_LP_UV_MSK                      (0x40U)
 /** CCE4511 PHY INT_EN_STAT Channel register LP_UV interrupt enabled bitmask*/
 #define CCE4511_REG_INT_EN_STAT_IE_LP_UV_EN                       (0x40U)
 /** CCE4511 PHY INT_EN_STAT Channel register LP_UV interrupt disabled bitmask*/
 #define CCE4511_REG_INT_EN_STAT_IE_LP_UV_DIS                      (0x00U)

 /** CCE4511 PHY INT_EN_STAT Channel register CH_DIS_LPX_CLA interrupt field bitmask*/
 #define CCE4511_REG_INT_EN_STAT_IE_CH_DIS_LPX_CLA_MSK             (0x20U)
 /** CCE4511 PHY INT_EN_STAT Channel register CH_DIS_LPX_CLA interrupt enabled bitmask*/
 #define CCE4511_REG_INT_EN_STAT_IE_CH_DIS_LPX_CLA_EN              (0x20U)
 /** CCE4511 PHY INT_EN_STAT Channel register CH_DIS_LPX_CLA interrupt disabled bitmask*/
 #define CCE4511_REG_INT_EN_STAT_IE_CH_DIS_LPX_CLA_DIS             (0x00U)

 /** CCE4511 PHY INT_EN_STAT Channel register CH_DIS_LPX_OVC interrupt field bitmask*/
 #define CCE4511_REG_INT_EN_STAT_IE_CH_DIS_LPX_OVC_MSK             (0x10U)
 /** CCE4511 PHY INT_EN_STAT Channel register CH_DIS_LPX_OVC interrupt enabled bitmask*/
 #define CCE4511_REG_INT_EN_STAT_IE_CH_DIS_LPX_OVC_EN              (0x10U)
 /** CCE4511 PHY INT_EN_STAT Channel register CH_DIS_LPX_OVC interrupt disabled bitmask*/
 #define CCE4511_REG_INT_EN_STAT_IE_CH_DIS_LPX_OVC_DIS             (0x00U)

 /** CCE4511 PHY INT_EN_STAT Channel register IE_CH_DIS_CQ_OVC interrupt field bitmask*/
 #define CCE4511_REG_INT_EN_STAT_IE_CH_DIS_CQ_OVC_MSK              (0x08U)
 /** CCE4511 PHY INT_EN_STAT Channel register IE_CH_DIS_CQ_OVC interrupt enabled bitmask*/
 #define CCE4511_REG_INT_EN_STAT_IE_CH_DIS_CQ_OVC_EN               (0x08U)
 /** CCE4511 PHY INT_EN_STAT Channel register IE_CH_DIS_CQ_OVC interrupt disabled bitmask*/
 #define CCE4511_REG_INT_EN_STAT_IE_CH_DIS_CQ_OVC_DIS              (0x00U)

 /** CCE4511 PHY INT_EN_STAT Channel register IE_LPX_CLA interrupt field bitmask*/
 #define CCE4511_REG_INT_EN_STAT_IE_LPX_CLA_MSK                    (0x04U)
 /** CCE4511 PHY INT_EN_STAT Channel register IE_LPX_CLA interrupt enabled field bitmask*/
 #define CCE4511_REG_INT_EN_STAT_IE_LPX_CLA_EN                     (0x04U)
 /** CCE4511 PHY INT_EN_STAT Channel register IE_LPX_CLA interrupt disabled field bitmask*/
 #define CCE4511_REG_INT_EN_STAT_IE_LPX_CLA_DIS                    (0x00U)

 /** CCE4511 PHY INT_EN_STAT Channel register IE_LPX_OVC interrupt field bitmask*/
 #define CCE4511_REG_INT_EN_STAT_IE_LPX_OVC_MSK                    (0x02U)
 /** CCE4511 PHY INT_EN_STAT Channel register IE_LPX_OVC interrupt enabled bitmask*/
 #define CCE4511_REG_INT_EN_STAT_IE_LPX_OVC_EN                     (0x02U)
 /** CCE4511 PHY INT_EN_STAT Channel register IE_LPX_OVC interrupt disabled bitmask*/
 #define CCE4511_REG_INT_EN_STAT_IE_LPX_OVC_DIS                    (0x00U)

 /** CCE4511 PHY INT_EN_STAT Channel register IE_CQ_OVC interrupt field bitmask*/
 #define CCE4511_REG_INT_EN_STAT_IE_CQ_OVC_MSK                     (0x01U)
 /** CCE4511 PHY INT_EN_STAT Channel register IE_CQ_OVC interrupt source enabled bitmask*/
 #define CCE4511_REG_INT_EN_STAT_IE_CQ_OVC_EN                      (0x01U)
 /** CCE4511 PHY INT_EN_STAT Channel register IE_CQ_OVC interrupt source disabled bitmask*/
 #define CCE4511_REG_INT_EN_STAT_IE_CQ_OVC_DIS                     (0x00U)

 /*** INT_SRC_STAT registers ***/

 /** CCE4511 PHY INT_SRC_STAT Channel register IS_LP_OV interrupt source field bitmask*/
 #define CCE4511_REG_INT_SRC_STAT_IS_LP_OV_MSK                      (0x80U)
 /** CCE4511 PHY INT_SRC_STAT Channel register IS_LP_UV interrupt source field bitmask*/
 #define CCE4511_REG_INT_SRC_STAT_IS_LP_UV_MSK                      (0x40U)
 /** CCE4511 PHY INT_SRC_STAT Channel register IS_CH_DIS_LPX_CLA interrupt source field bitmask*/
 #define CCE4511_REG_INT_SRC_STAT_IS_CH_DIS_LPX_CLA_MSK             (0x20U)
 /** CCE4511 PHY INT_SRC_STAT Channel register IS_CH_DIS_LPX_OVC interrupt source field bitmask*/
 #define CCE4511_REG_INT_SRC_STAT_IS_CH_DIS_LPX_OVC_MSK             (0x10U)
 /** CCE4511 PHY INT_SRC_STAT Channel register IS_CH_DIS_CQ_OVC interrupt source field bitmask*/
 #define CCE4511_REG_INT_SRC_STAT_IS_CH_DIS_CQ_OVC_MSK              (0x08U)
 /** CCE4511 PHY INT_SRC_STAT Channel register IS_LPX_CLA interrupt source field bitmask*/
 #define CCE4511_REG_INT_SRC_STAT_IS_LPX_CLA_MSK                    (0x04U)
 /** CCE4511 PHY INT_SRC_STAT Channel register IS_LPX_OVC interrupt source field bitmask*/
 #define CCE4511_REG_INT_SRC_STAT_IS_LPX_OVC_MSK                    (0x02U)
 /** CCE4511 PHY INT_SRC_STAT Channel register IS_CQ_OVC interrupt source field bitmask*/
 #define CCE4511_REG_INT_SRC_STAT_IS_CQ_OVC_MSK                     (0x01U)

 /*** INT_EN_SIO registers ***/

 /** CCE4511 PHY INT_EN_SIO Channel register IE_RP_ERR interrupt field bitmask*/
 #define CCE4511_REG_INT_EN_SIO_IE_RP_ERR_MSK                      (0x08U)
 /** CCE4511 PHY INT_EN_SIO Channel register IE_RP_ERR interrupt enabled bitmask*/
 #define CCE4511_REG_INT_EN_SIO_IE_RP_ERR_EN                       (0x08U)
 /** CCE4511 PHY INT_EN_SIO Channel register IE_RP_ERR interrupt disabled bitmask*/
 #define CCE4511_REG_INT_EN_SIO_IE_RP_ERR_DIS                      (0x00U)

 /** CCE4511 PHY INT_EN_SIO Channel register IE_READY_DET interrupt field bitmask*/
 #define CCE4511_REG_INT_EN_SIO_IE_READY_DET_MSK                   (0x04U)
 /** CCE4511 PHY INT_EN_SIO Channel register IE_READY_DET interrupt enabled bitmask*/
 #define CCE4511_REG_INT_EN_SIO_IE_READY_DET_EN                    (0x04U)
 /** CCE4511 PHY INT_EN_SIO Channel register IE_READY_DET interrupt disabled bitmask*/
 #define CCE4511_REG_INT_EN_SIO_IE_READY_DET_DIS                   (0x00U)

 /** CCE4511 PHY INT_EN_SIO Channel register IE_CQ_FALL interrupt field bitmask*/
 #define CCE4511_REG_INT_EN_SIO_IE_CQ_FALL_MSK                     (0x02U)
 /** CCE4511 PHY INT_EN_SIO Channel register IE_CQ_FALL interrupt enabled bitmask*/
 #define CCE4511_REG_INT_EN_SIO_IE_CQ_FALL_EN                      (0x02U)
 /** CCE4511 PHY INT_EN_SIO Channel register IE_CQ_FALL interrupt disabled bitmask*/
 #define CCE4511_REG_INT_EN_SIO_IE_CQ_FALL_DIS                     (0x00U)

 /** CCE4511 PHY INT_EN_SIO Channel register IE_CQ_RISE interrupt field bitmask*/
 #define CCE4511_REG_INT_EN_SIO_IE_CQ_RISE_MSK                     (0x01U)
 /** CCE4511 PHY INT_EN_SIO Channel register IE_CQ_RISE interrupt enabled bitmask*/
 #define CCE4511_REG_INT_EN_SIO_IE_CQ_RISE_EN                      (0x01U)
 /** CCE4511 PHY INT_EN_SIO Channel register IE_CQ_RISE interrupt disabled bitmask*/
 #define CCE4511_REG_INT_EN_SIO_IE_CQ_RISE_DIS                     (0x00U)

 /*** INT_SRC_SIO registers ***/

 /** CCE4511 PHY INT_SRC_SIO Channel register IS_RP_ERR interrupt source field bitmask*/
 #define CCE4511_REG_INT_SRC_SIO_IS_RP_ERR_MSK                     (0x08U)
 /** CCE4511 PHY INT_SRC_SIO Channel register IS_READY_DET interrupt source field bitmask*/
 #define CCE4511_REG_INT_SRC_SIO_IS_READY_DET_MSK                  (0x04U)
 /** CCE4511 PHY INT_SRC_SIO Channel register IS_CQ_FALL interrupt source field bitmask*/
 #define CCE4511_REG_INT_SRC_SIO_IS_CQ_FALL_MSK                    (0x02U)
 /** CCE4511 PHY INT_SRC_SIO Channel register IS_CQ_RISE interrupt source field bitmask*/
 #define CCE4511_REG_INT_SRC_SIO_IS_CQ_RISE_MSK                    (0x01U)

 /*** INT_EN_FH registers ***/

 /** CCE4511 PHY INT_EN_FH Channel register IE_ERR interrupt field bitmask*/
 #define CCE4511_REG_INT_EN_FH_IE_ERR_MSK                          (0x80U)
 /** CCE4511 PHY INT_EN_FH Channel register IE_ERR interrupt enabled bitmask*/
 #define CCE4511_REG_INT_EN_FH_IE_ERR_EN                           (0x80U)
 /** CCE4511 PHY INT_EN_FH Channel register IE_ERR interrupt disabled bitmask*/
 #define CCE4511_REG_INT_EN_FH_IE_ERR_DIS                          (0x00U)

 /** CCE4511 PHY INT_EN_FH Channel register IE_EOC interrupt field bitmask*/
 #define CCE4511_REG_INT_EN_FH_IE_EOC_MSK                          (0x40U)
 /** CCE4511 PHY INT_EN_FH Channel register IE_EOC interrupt enabled bitmask*/
 #define CCE4511_REG_INT_EN_FH_IE_EOC_EN                           (0x40U)
 /** CCE4511 PHY INT_EN_FH Channel register IE_EOC interrupt disabled bitmask*/
 #define CCE4511_REG_INT_EN_FH_IE_EOC_DIS                          (0x00U)

 /** CCE4511 PHY INT_EN_FH Channel register IE_WURQ interrupt field bitmask*/
 #define CCE4511_REG_INT_EN_FH_IE_WURQ_MSK                         (0x20U)
 /** CCE4511 PHY INT_EN_FH Channel register IE_WURQ interrupt enabled bitmask*/
 #define CCE4511_REG_INT_EN_FH_IE_WURQ_EN                          (0x20U)
 /** CCE4511 PHY INT_EN_FH Channel register IE_WURQ interrupt disabled bitmask*/
 #define CCE4511_REG_INT_EN_FH_IE_WURQ_DIS                         (0x00U)

 /** CCE4511 PHY INT_EN_FH Channel register IE_TOUT interrupt field bitmask*/
 #define CCE4511_REG_INT_EN_FH_IE_TOUT_MSK                         (0x10U)
 /** CCE4511 PHY INT_EN_FH Channel register IE_TOUT interrupt enabled bitmask*/
 #define CCE4511_REG_INT_EN_FH_IE_TOUT_EN                          (0x10U)
 /** CCE4511 PHY INT_EN_FH Channel register IE_TOUT interrupt disabled bitmask*/
 #define CCE4511_REG_INT_EN_FH_IE_TOUT_DIS                         (0x00U)

 /** CCE4511 PHY INT_EN_FH Channel register IE_SOT interrupt field bitmask*/
 #define CCE4511_REG_INT_EN_FH_IE_SOT_MSK                          (0x08U)
 /** CCE4511 PHY INT_EN_FH Channel register IE_SOT interrupt enabled bitmask*/
 #define CCE4511_REG_INT_EN_FH_IE_SOT_EN                           (0x08U)
 /** CCE4511 PHY INT_EN_FH Channel register IE_SOT interrupt disabled bitmask*/
 #define CCE4511_REG_INT_EN_FH_IE_SOT_DIS                          (0x00U)

 /** CCE4511 PHY INT_EN_FH Channel register IE_SOR interrupt field bitmask*/
 #define CCE4511_REG_INT_EN_FH_IE_SOR_MSK                          (0x04U)
 /** CCE4511 PHY INT_EN_FH Channel register IE_SOR interrupt enabled bitmask*/
 #define CCE4511_REG_INT_EN_FH_IE_SOR_EN                           (0x04U)
 /** CCE4511 PHY INT_EN_FH Channel register IE_SOR interrupt disabled bitmask*/
 #define CCE4511_REG_INT_EN_FH_IE_SOR_DIS                          (0x00U)

 /** CCE4511 PHY INT_EN_FH Channel register IE_LVL interrupt field bitmask*/
 #define CCE4511_REG_INT_EN_FH_IE_LVL_MSK                          (0x02U)
 /** CCE4511 PHY INT_EN_FH Channel register IE_LVL interrupt enabled bitmask*/
 #define CCE4511_REG_INT_EN_FH_IE_LVL_EN                           (0x02U)
 /** CCE4511 PHY INT_EN_FH Channel register IE_LVL interrupt disabled bitmask*/
 #define CCE4511_REG_INT_EN_FH_IE_LVL_DIS                          (0x00U)

 /** CCE4511 PHY INT_EN_FH Channel register IE_MSG interrupt field bitmask*/
 #define CCE4511_REG_INT_EN_FH_IE_MSG_MSK                          (0x01U)
 /** CCE4511 PHY INT_EN_FH Channel register IE_MSG interrupt enabled bitmask*/
 #define CCE4511_REG_INT_EN_FH_IE_MSG_EN                           (0x01U)
 /** CCE4511 PHY INT_EN_FH Channel register IE_MSG interrupt disabled bitmask*/
 #define CCE4511_REG_INT_EN_FH_IE_MSG_DIS                          (0x00U)

 /*** INT_SRC_FH registers ***/

 /** CCE4511 PHY INT_SRC_FH Channel register IS_ERR interrupt source field bitmask*/
 #define CCE4511_REG_INT_SRC_FH_IS_ERR_MSK                         (0x80U)
 /** CCE4511 PHY INT_SRC_FH Channel register IS_EOC interrupt source field bitmask*/
 #define CCE4511_REG_INT_SRC_FH_IS_EOC_MSK                         (0x40U)
 /** CCE4511 PHY INT_SRC_FH Channel register IS_WURQ interrupt source field bitmask*/
 #define CCE4511_REG_INT_SRC_FH_IS_WURQ_MSK                        (0x20U)
 /** CCE4511 PHY INT_SRC_FH Channel register IS_TOUT interrupt source field bitmask*/
 #define CCE4511_REG_INT_SRC_FH_IS_TOUT_MSK                        (0x10U)
 /** CCE4511 PHY INT_SRC_FH Channel register IS_SOT interrupt source field bitmask*/
 #define CCE4511_REG_INT_SRC_FH_IS_SOT_MSK                         (0x08U)
 /** CCE4511 PHY INT_SRC_FH Channel register IS_SOR interrupt source field bitmask*/
 #define CCE4511_REG_INT_SRC_FH_IS_SOR_MSK                         (0x04U)
 /** CCE4511 PHY INT_SRC_FH Channel register IS_LVL interrupt source field bitmask*/
 #define CCE4511_REG_INT_SRC_FH_IS_LVL_MSK                         (0x02U)
 /** CCE4511 PHY INT_SRC_FH Channel register IS_MSG interrupt source field bitmask*/
 #define CCE4511_REG_INT_SRC_FH_IS_MSG_MSK                         (0x01U)

 /*** LPX_CLA_TIME/LPX_OVC_TIME/QC_OVC_TIME registers ***/

 /** CCE4511 PHY LPX_CLA_TIME/LPX_OVC_TIME/QC_OVC_TIME Channel registers time FACTOR field bitmask*/
 #define CCE4511_REG_CURR_PROT_TIME_FACTOR_MSK                     (0x80U)
 /** CCE4511 PHY LPX_CLA_TIME/LPX_OVC_TIME/QC_OVC_TIME Channel registers time FACTOR 100 bitmask*/
 #define CCE4511_REG_CURR_PROT_TIME_FACTOR_100                     (0x80U)
 /** CCE4511 PHY LPX_CLA_TIME/LPX_OVC_TIME/QC_OVC_TIME Channel registers time FACTOR 10 bitmask*/
 #define CCE4511_REG_CURR_PROT_TIME_FACTOR_10                      (0x00U)

 /** CCE4511 PHY LPX_CLA_TIME/LPX_OVC_TIME/QC_OVC_TIME Channel registers time BASE field bitmask*/
 #define CCE4511_REG_CURR_PROT_TIME_BASE_MSK                       (0x60U)
 /** CCE4511 PHY LPX_CLA_TIME/LPX_OVC_TIME/QC_OVC_TIME Channel registers time BASE 10ms bitmask*/
 #define CCE4511_REG_CURR_PROT_TIME_BASE_10_MS                     (0x60U)
 /** CCE4511 PHY LPX_CLA_TIME/LPX_OVC_TIME/QC_OVC_TIME Channel registers time BASE 1ms bitmask*/
 #define CCE4511_REG_CURR_PROT_TIME_BASE_1_MS                      (0x40U)
 /** CCE4511 PHY LPX_CLA_TIME/LPX_OVC_TIME/QC_OVC_TIME Channel registers time BASE 100us bitmask*/
 #define CCE4511_REG_CURR_PROT_TIME_BASE_100_US                    (0x20U)
 /** CCE4511 PHY LPX_CLA_TIME/LPX_OVC_TIME/QC_OVC_TIME Channel registers time BASE 20us bitmask*/
 #define CCE4511_REG_CURR_PROT_TIME_BASE_20_US                     (0x00U)

 /** CCE4511 PHY LPX_CLA_TIME/LPX_OVC_TIME/QC_OVC_TIME Channel registers time MULT field bitmask*/
 #define CCE4511_REG_CURR_PROT_TIME_MULT_MSK                       (0x1FU)

 /*** PROT register ***/

 /** CCE4511 PHY PROT Chip control register Channel 3 disable on over-temperature field bitmask*/
 #define CCE4511_REG_PROT_PTEMP3_MSK                               (0x80U)
 /** CCE4511 PHY PROT Chip control register Channel 3 disable on over-temperature enabled bitmask*/
 #define CCE4511_REG_PROT_PTEMP3_EN                                (0x80U)
 /** CCE4511 PHY PROT Chip control register Channel 3 disable on over-temperature disabled bitmask*/
 #define CCE4511_REG_PROT_PTEMP3_DIS                               (0x00U)

 /** CCE4511 PHY PROT Chip control register Channel 3 disable on VS over-voltage field bitmask*/
 #define CCE4511_REG_PROT_POV3_MSK                                 (0x40U)
 /** CCE4511 PHY PROT Chip control register Channel 3 disable on VS over-voltage enabled bitmask*/
 #define CCE4511_REG_PROT_POV3_EN                                  (0x40U)
 /** CCE4511 PHY PROT Chip control register Channel 3 disable on VS over-voltage disabled bitmask*/
 #define CCE4511_REG_PROT_POV3_DIS                                 (0x00U)

 /** CCE4511 PHY PROT Chip control register Channel 2 disable on over-temperature field bitmask*/
 #define CCE4511_REG_PROT_PTEMP2_MSK                               (0x20U)
 /** CCE4511 PHY PROT Chip control register Channel 2 disable on over-temperature enabled bitmask*/
 #define CCE4511_REG_PROT_PTEMP2_EN                                (0x20U)
 /** CCE4511 PHY PROT Chip control register Channel 2 disable on over-temperature disabled bitmask*/
 #define CCE4511_REG_PROT_PTEMP2_DIS                               (0x00U)

 /** CCE4511 PHY PROT Chip control register Channel 2 disable on VS over-voltage field bitmask*/
 #define CCE4511_REG_PROT_POV2_MSK                                 (0x10U)
 /** CCE4511 PHY PROT Chip control register Channel 2 disable on VS over-voltage enabled bitmask*/
 #define CCE4511_REG_PROT_POV2_EN                                  (0x10U)
 /** CCE4511 PHY PROT Chip control register Channel 2 disable on VS over-voltage disabled bitmask*/
 #define CCE4511_REG_PROT_POV2_DIS                                 (0x00U)

 /** CCE4511 PHY PROT Chip control register Channel 1 disable on over-temperature field bitmask*/
 #define CCE4511_REG_PROT_PTEMP1_MSK                               (0x08U)
 /** CCE4511 PHY PROT Chip control register Channel 1 disable on over-temperature enabled bitmask*/
 #define CCE4511_REG_PROT_PTEMP1_EN                                (0x08U)
 /** CCE4511 PHY PROT Chip control register Channel 1 disable on over-temperature disabled bitmask*/
 #define CCE4511_REG_PROT_PTEMP1_DIS                               (0x00U)

 /** CCE4511 PHY PROT Chip control register Channel 1 disable on VS over-voltage field bitmask*/
 #define CCE4511_REG_PROT_POV1_MSK                                 (0x04U)
 /** CCE4511 PHY PROT Chip control register Channel 1 disable on VS over-voltage enabled bitmask*/
 #define CCE4511_REG_PROT_POV1_EN                                  (0x04U)
 /** CCE4511 PHY PROT Chip control register Channel 1 disable on VS over-voltage disabled bitmask*/
 #define CCE4511_REG_PROT_POV1_DIS                                 (0x00U)

 /** CCE4511 PHY PROT Chip control register Channel 0 disable on over-temperature field bitmask*/
 #define CCE4511_REG_PROT_PTEMP0_MSK                               (0x02U)
 /** CCE4511 PHY PROT Chip control register Channel 0 disable on over-temperature enabled bitmask*/
 #define CCE4511_REG_PROT_PTEMP0_EN                                (0x02U)
 /** CCE4511 PHY PROT Chip control register Channel 0 disable on over-temperature disabled bitmask*/
 #define CCE4511_REG_PROT_PTEMP0_DIS                               (0x00U)

 /** CCE4511 PHY PROT Chip control register Channel 0 disable on VS over-voltage field bitmask*/
 #define CCE4511_REG_PROT_POV0_MSK                                 (0x01U)
 /** CCE4511 PHY PROT Chip control register Channel 0 disable on VS over-voltage enabled bitmask*/
 #define CCE4511_REG_PROT_POV0_EN                                  (0x01U)
 /** CCE4511 PHY PROT Chip control register Channel 0 disable on VS over-voltage disabled bitmask*/
 #define CCE4511_REG_PROT_POV0_DIS                                 (0x00U)

 /*** MONITOR_EN register ***/

 /** CCE4511 PHY MONITOR_EN Chip control register Temperature monitoring field bitmask*/
 #define CCE4511_REG_MONITOR_EN_MON_TSD_MSK                        (0x80U)
 /** CCE4511 PHY MONITOR_EN Chip control register Temperature monitoring enabled bitmask*/
 #define CCE4511_REG_MONITOR_EN_MON_TSD_EN                         (0x80U)
 /** CCE4511 PHY MONITOR_EN Chip control register Temperature monitoring disabled bitmask*/
 #define CCE4511_REG_MONITOR_EN_MON_TSD_DIS                        (0x00U)

 /** CCE4511 PHY MONITOR_EN Chip control register LP voltage monitors field bitmask*/
 #define CCE4511_REG_MONITOR_EN_VMON_LP_MSK                        (0x78U)
 /** CCE4511 PHY MONITOR_EN Chip control register LP voltage monitors disable on all channels bitmask*/
 #define CCE4511_REG_MONITOR_EN_VMON_LP_DIS_ALL                    (0x00U)
 /** CCE4511 PHY MONITOR_EN Chip control register LP voltage monitors enable on Channel 0 bitmask*/
 #define CCE4511_REG_MONITOR_EN_VMON_CH0                           (0x08U)
 /** CCE4511 PHY MONITOR_EN Chip control register LP voltage monitors enable on Channel 1 bitmask*/
 #define CCE4511_REG_MONITOR_EN_VMON_CH1                           (0x10U)
 /** CCE4511 PHY MONITOR_EN Chip control register LP voltage monitors enable on Channel 2 bitmask*/
 #define CCE4511_REG_MONITOR_EN_VMON_CH2                           (0x20U)
 /** CCE4511 PHY MONITOR_EN Chip control register LP voltage monitors enable on Channel 3 bitmask*/
 #define CCE4511_REG_MONITOR_EN_VMON_CH3                           (0x40U)
 /** CCE4511 PHY MONITOR_EN Chip control register LP voltage monitors enable on all channels bitmask*/
 #define CCE4511_REG_MONITOR_EN_VMON_EN_ALL                        (0x78U)

 /** CCE4511 PHY MONITOR_EN Chip control register VS voltage monitor field bitmask*/
 #define CCE4511_REG_MONITOR_EN_VMON_VS_MSK                        (0x04U)
 /** CCE4511 PHY MONITOR_EN Chip control register VS voltage monitor enabled bitmask*/
 #define CCE4511_REG_MONITOR_EN_VMON_VS_EN                         (0x04U)
 /** CCE4511 PHY MONITOR_EN Chip control register VS voltage monitor disabled bitmask*/
 #define CCE4511_REG_MONITOR_EN_VMON_VS_DIS                        (0x00U)

 /** CCE4511 PHY MONITOR_EN Chip control register VDDIO voltage monitor field bitmask*/
 #define CCE4511_REG_MONITOR_EN_VMON_VDDIO_MSK                     (0x02U)
 /** CCE4511 PHY MONITOR_EN Chip control register VDDIO voltage monitor enabled bitmask*/
 #define CCE4511_REG_MONITOR_EN_VMON_VDDIO_EN                      (0x02U)
 /** CCE4511 PHY MONITOR_EN Chip control register VDDIO voltage monitor disabled bitmask*/
 #define CCE4511_REG_MONITOR_EN_VMON_VDDIO_DIS                     (0x00U)

 /** CCE4511 PHY MONITOR_EN Chip control register VDDD voltage monitor field bitmask*/
 #define CCE4511_REG_MONITOR_EN_VMON_VDDD_MSK                      (0x01U)
 /** CCE4511 PHY MONITOR_EN Chip control register VDDD voltage monitor enabled bitmask*/
 #define CCE4511_REG_MONITOR_EN_VMON_VDDD_EN                       (0x01U)
 /** CCE4511 PHY MONITOR_EN Chip control register VDDD voltage monitor disabled bitmask*/
 #define CCE4511_REG_MONITOR_EN_VMON_VDDD_DIS                      (0x00U)

 /*** INT_EN_ENV register ***/

 /** CCE4511 PHY INT_EN_ENV Chip control register IE_VS_OV interrupt field bitmask*/
 #define CCE4511_REG_INT_EN_ENV_IE_VS_OV_MSK                       (0x20U)
 /** CCE4511 PHY INT_EN_ENV Chip control register IE_VS_OV interrupt enabled bitmask*/
 #define CCE4511_REG_INT_EN_ENV_IE_VS_OV_EN                        (0x20U)
 /** CCE4511 PHY INT_EN_ENV Chip control register IE_VS_OV interrupt disabled bitmask*/
 #define CCE4511_REG_INT_EN_ENV_IE_VS_OV_DIS                       (0x00U)

 /** CCE4511 PHY INT_EN_ENV Chip control register IE_VS_UV interrupt field bitmask*/
 #define CCE4511_REG_INT_EN_ENV_IE_VS_UV_MSK                       (0x10U)
 /** CCE4511 PHY INT_EN_ENV Chip control register IE_VS_UV interrupt enabled bitmask*/
 #define CCE4511_REG_INT_EN_ENV_IE_VS_UV_EN                        (0x10U)
 /** CCE4511 PHY INT_EN_ENV Chip control register IE_VS_UV interrupt disabled bitmask*/
 #define CCE4511_REG_INT_EN_ENV_IE_VS_UV_DIS                       (0x00U)

 /** CCE4511 PHY INT_EN_ENV Chip control register IE_VDDA_UV interrupt field bitmask*/
 #define CCE4511_REG_INT_EN_ENV_IE_VDDA_UV_MSK                     (0x08U)
 /** CCE4511 PHY INT_EN_ENV Chip control register IE_VDDA_UV interrupt enabled bitmask*/
 #define CCE4511_REG_INT_EN_ENV_IE_VDDA_UV_EN                      (0x08U)
 /** CCE4511 PHY INT_EN_ENV Chip control register IE_VDDA_UV interrupt disabled bitmask*/
 #define CCE4511_REG_INT_EN_ENV_IE_VDDA_UV_DIS                     (0x00U)

 /** CCE4511 PHY INT_EN_ENV Chip control register IE_VDDIO_UV interrupt field bitmask*/
 #define CCE4511_REG_INT_EN_ENV_IE_VDDIO_UV_MSK                    (0x04U)
 /** CCE4511 PHY INT_EN_ENV Chip control register IE_VDDIO_UV interrupt enabled bitmask*/
 #define CCE4511_REG_INT_EN_ENV_IE_VDDIO_UV_EN                     (0x04U)
 /** CCE4511 PHY INT_EN_ENV Chip control register IE_VDDIO_UV interrupt disabled bitmask*/
 #define CCE4511_REG_INT_EN_ENV_IE_VDDIO_UV_DIS                    (0x00U)

 /** CCE4511 PHY INT_EN_ENV Chip control register IE_T_HIGH interrupt field bitmask*/
 #define CCE4511_REG_INT_EN_ENV_IE_T_HIGH_MSK                      (0x01U)
 /** CCE4511 PHY INT_EN_ENV Chip control register IE_T_HIGH interrupt enabled bitmask*/
 #define CCE4511_REG_INT_EN_ENV_IE_T_HIGH_EN                       (0x01U)
 /** CCE4511 PHY INT_EN_ENV Chip control register IE_T_HIGH interrupt disabled bitmask*/
 #define CCE4511_REG_INT_EN_ENV_IE_T_HIGH_DIS                      (0x00U)

 /*** INT_SRC_ENV register ***/

 /** CCE4511 PHY INT_SRC_ENV Chip control register IS_VS_OV interrupt source field bitmask*/
 #define CCE4511_REG_INT_SRC_ENV_IS_VS_OV_MSK                      (0x20U)
 /** CCE4511 PHY INT_SRC_ENV Chip control register IS_VS_UV interrupt source field bitmask*/
 #define CCE4511_REG_INT_SRC_ENV_IS_VS_UV_MSK                      (0x10U)
 /** CCE4511 PHY INT_SRC_ENV Chip control register IS_VDDA_UV interrupt source field bitmask*/
 #define CCE4511_REG_INT_SRC_ENV_IS_VDDA_UV_MSK                    (0x08U)
 /** CCE4511 PHY INT_SRC_ENV Chip control register IS_VDDIO_UV interrupt source field bitmask*/
 #define CCE4511_REG_INT_SRC_ENV_IS_VDDIO_UV_MSK                   (0x04U)
 /** CCE4511 PHY INT_SRC_ENV Chip control register IS_T_HIGH interrupt source field bitmask*/
 #define CCE4511_REG_INT_SRC_ENV_IS_T_HIGH_MSK                     (0x01U)

 /*** CFG register ***/

 /** CCE4511 PHY CFG Chip control register Chip-ready signal field bitmask*/
 #define CCE4511_REG_CFG_CHIP_READY_MSK                            (0x80U)
 /** CCE4511 PHY CFG Chip control register Chip-ready signal internal initialization finished bitmask*/
 #define CCE4511_REG_CFG_CHIP_READY_INIT_FINISHED                  (0x80U)
 /** CCE4511 PHY CFG Chip control register Chip-ready signal internal initialization ongoing bitmask*/
 #define CCE4511_REG_CFG_CHIP_READY_INIT_ONGOING                   (0x00U)

 /** CCE4511 PHY CFG Chip control register internal logic reset field bitmask*/
 #define CCE4511_REG_CFG_CHIP_RESET_MSK                            (0x40U)

 /** CCE4511 PHY CFG Chip control register TEST_FACTORY_RESET field bitmask*/
 #define CCE4511_REG_CFG_TEST_FACTORY_RESET_MSK                    (0x20U)

 /** CCE4511 PHY CFG Chip control register 1.8V voltage regulator control field bitmask*/
 #define CCE4511_REG_CFG_VDD_LDO_MSK                               (0x04U)
 /** CCE4511 PHY CFG Chip control register 1.8V voltage regulator enabled bitmask*/
 #define CCE4511_REG_CFG_VDD_LDO_EN                                (0x00U)
 /** CCE4511 PHY CFG Chip control register 1.8V voltage regulator disabled bitmask*/
 #define CCE4511_REG_CFG_VDD_LDO_DIS                               (0x04U)

 /** CCE4511 PHY CFG Chip control register IO-Pad Drive Strength Configuration field bitmask*/
 #define CCE4511_REG_CFG_PAD_DRV_STRENGTH_MSK                      (0x02U)
 /** CCE4511 PHY CFG Chip control register IO-Pad Drive Strength high bitmask*/
 #define CCE4511_REG_CFG_PAD_DRV_STRENGTH_HIGH                     (0x02U)
 /** CCE4511 PHY CFG Chip control register IO-Pad Drive Strength low bitmask*/
 #define CCE4511_REG_CFG_PAD_DRV_STRENGTH_LOW                      (0x00U)

 /** CCE4511 PHY CFG Chip control register Temperature monitor hysteresis field bitmask*/
 #define CCE4511_REG_CFG_TSD_HYST_EN_MSK                           (0x01U)
 /** CCE4511 PHY CFG Chip control register Temperature monitor hysteresis enabled bitmask*/
 #define CCE4511_REG_CFG_TSD_HYST_EN_EN                            (0x01U)
 /** CCE4511 PHY CFG Chip control register Temperature monitor hysteresis disabled bitmask*/
 #define CCE4511_REG_CFG_TSD_HYST_EN_DIS                           (0x00U)


 /* Global data type definitions -------------------------------------------------------------*/

#define CCE4511_REG_ADDR_CH_START_ADDRESS  (0x80UL)

typedef enum _cce4511_channel{
	CCE4511_CHN_0 = 0U,
	CCE4511_CHN_1,
	CCE4511_CHN_2,
	CCE4511_CHN_3
 }cce4511_channel_t;

 typedef enum _cce4511_reg_acc{
	CCE4511_REG_WRITE = 0U,
	CCE4511_REG_READ
 }cce4511_reg_acc_t;

 typedef enum _cce4511_pin_level{
	 CCE4511_PIN_LEVEL_LOW,
	 CCE4511_PIN_LEVEL_HIGH
 }cce4511_pin_level_t;

 typedef enum _cce4511_pin{
	 CCE4511_PIN_CQ,           /** CQ - pin4 */
	 CCE4511_PIN_LP            /** L+ power - pin1 */
 }cce4511_pin_t;

typedef enum _cce4511_reg_addr
{

/*	 Register map chip_control (Pg No. 35 in DS) */
 CCE4511_REV_REG_ADDR					= 0x00,
 CCE4511_PROT_REG_ADDR					= 0x01,
 CCE4511_INT_SRC_ENV_REG_ADDR			= 0x02,
 CCE4511_INT_EN_ENV_REG_ADDR			= 0x03,
 CCE4511_LED_SYNC_REG_ADDR				= 0x04,
 CCE4511_SYNC_REG_ADDR					= 0x05,
 CCE4511_MONITOR_EN_REG_ADDR			= 0x06,
 CCE4511_CFG_REG_ADDR					= 0x07,
 CCE4511_FLAG_ENV_REG_ADDR				= 0x18,

 /* Channel Registers -> 32 identical registers for each channel(Pg No. 39 in DS)  */

 /* Chennel 1 Register */
 CCE4511_CHN0_CFG1_REG_ADDR				=	0x80,
 CCE4511_CHN0_CFG2_REG_ADDR				=	0x81,
 CCE4511_CHN0_CFG3_REG_ADDR				=	0x82,
 CCE4511_CHN0_SIO_REG_ADDR				=	0x83,
 CCE4511_CHN0_UART_REG_ADDR				=	0x84,
 CCE4511_CHN0_FHC_REG_ADDR				=	0x85,
 CCE4511_CHN0_OD_REG_ADDR				=	0x86,
 CCE4511_CHN0_MPD_REG_ADDR				=	0x87,
 CCE4511_CHN0_DPD_REG_ADDR				=	0x88,
 CCE4511_CHN0_CYCT_REG_ADDR				=	0x89,
 CCE4511_CHN0_FHD_REG_ADDR				=	0x8A,
 CCE4511_CHN0_BLVL_REG_ADDR				=	0x8B,
 CCE4511_CHN0_LSEQ_A_REG_ADDR			=	0x8C,
 CCE4511_CHN0_LHLD_A_REG_ADDR			=	0x8D,
 CCE4511_CHN0_LSEQ_B_REG_ADDR			=	0x8E,
 CCE4511_CHN0_LHLD_B_REG_ADDR			=	0x8F,
 CCE4511_CHN0_CFG4_REG_ADDR				=	0x90,
 CCE4511_CHN0_WRP_SOTO_TIMER_REG_ADDR	=	0x91,
 CCE4511_CHN0_STATUS_REG_ADDR			=   0x92,
 CCE4511_CHN0_STATUS_MODE_FH_REG_ADDR	=	0x92,
 CCE4511_CHN0_STATUS_MODE_UART_REG_ADDR	=	0x92,
 CCE4511_CHN0_STATUS_MODE_SIO_REG_ADDR	=	0x92,
 CCE4511_CHN0_INT_SRC_STAT_REG_ADDR		=	0x93,
 CCE4511_CHN0_INT_EN_STAT_REG_ADDR		=	0x94,
 CCE4511_CHN0_INT_SRC_SIO_REG_ADDR		=	0x95,
 CCE4511_CHN0_INT_EN_SIO_REG_ADDR		=	0x96,
 CCE4511_CHN0_INT_SRC_UART_REG_ADDR		=	0x97,
 CCE4511_CHN0_INT_EN_UART_REG_ADDR		=	0x98,
 CCE4511_CHN0_INT_SRC_FH_REG_ADDR		=	0x99,
 CCE4511_CHN0_INT_EN_FH_REG_ADDR		=	0x9A,
 CCE4511_CHN0_LDRV_A_REG_ADDR			=	0x9B,
 CCE4511_CHN0_LDRV_B_REG_ADDR			=	0x9C,
 CCE4511_CHN0_LPX_CLA_TIME_REG_ADDR		=	0x9D,
 CCE4511_CHN0_LPX_OVC_TIME_REG_ADDR		=	0x9E,
 CCE4511_CHN0_CQ_OVC_TIME_REG_ADDR		=	0x9F,

 /* Channel 2 Register */

 CCE4511_CHN1_CFG1_REG_ADDR				=	0xA0,
 CCE4511_CHN1_CFG2_REG_ADDR				=	0xA1,
 CCE4511_CHN1_CFG3_REG_ADDR				=	0xA2,
 CCE4511_CHN1_SIO_REG_ADDR				=	0xA3,
 CCE4511_CHN1_UART_REG_ADDR				=	0xA4,
 CCE4511_CHN1_FHC_REG_ADDR				=	0xA5,
 CCE4511_CHN1_OD_REG_ADDR				=	0xA6,
 CCE4511_CHN1_MPD_REG_ADDR				=	0xA7,
 CCE4511_CHN1_DPD_REG_ADDR				=	0xA8,
 CCE4511_CHN1_CYCT_REG_ADDR				=	0xA9,
 CCE4511_CHN1_FHD_REG_ADDR				=	0xAA,
 CCE4511_CHN1_BLVL_REG_ADDR				=	0xAB,
 CCE4511_CHN1_LSEQ_A_REG_ADDR			=	0xAC,
 CCE4511_CHN1_LHLD_A_REG_ADDR			=	0xAD,
 CCE4511_CHN1_LSEQ_B_REG_ADDR			=	0xAE,
 CCE4511_CHN1_LHLD_B_REG_ADDR			=	0xAF,
 CCE4511_CHN1_CFG4_REG_ADDR				=	0xB0,
 CCE4511_CHN1_WRP_SOTO_TIMER_REG_ADDR	=	0xB1,
 CCE4511_CHN1_STATUS_REG_ADDR			=	0XB2,
 CCE4511_CHN1_STATUS_MODE_FH_REG_ADDR	=	0xB2,
 CCE4511_CHN1_STATUS_MODE_UART_REG_ADDR	=	0xB2,
 CCE4511_CHN1_STATUS_MODE_SIO_REG_ADDR	=	0xB2,
 CCE4511_CHN1_INT_SRC_STAT_REG_ADDR		=	0xB3,
 CCE4511_CHN1_INT_EN_STAT_REG_ADDR		=	0xB4,
 CCE4511_CHN1_INT_SRC_SIO_REG_ADDR		=	0xB5,
 CCE4511_CHN1_INT_EN_SIO_REG_ADDR		=	0xB6,
 CCE4511_CHN1_INT_SRC_UART_REG_ADDR		=	0xB7,
 CCE4511_CHN1_INT_EN_UART_REG_ADDR		=	0xB8,
 CCE4511_CHN1_INT_SRC_FH_REG_ADDR		=	0xB9,
 CCE4511_CHN1_INT_EN_FH_REG_ADDR		=	0xBA,
 CCE4511_CHN1_LDRV_A_REG_ADDR			=	0xBB,
 CCE4511_CHN1_LDRV_B_REG_ADDR			=	0xBC,
 CCE4511_CHN1_LPX_CLA_TIME_REG_ADDR		=	0xBD,
 CCE4511_CHN1_LPX_OVC_TIME_REG_ADDR		=	0xBE,
 CCE4511_CHN1_CQ_OVC_TIME_REG_ADDR		=	0xBF,

 /* Channel 3 Registers */

 CCE4511_CHN2_CFG1_REG_ADDR				=	0xC0,
 CCE4511_CHN2_CFG2_REG_ADDR				=	0xC1,
 CCE4511_CHN2_CFG3_REG_ADDR				=	0xC2,
 CCE4511_CHN2_SIO_REG_ADDR				=	0xC3,
 CCE4511_CHN2_UART_REG_ADDR				=	0xC4,
 CCE4511_CHN2_FHC_REG_ADDR				=	0xC5,
 CCE4511_CHN2_OD_REG_ADDR				=	0xC6,
 CCE4511_CHN2_MPD_REG_ADDR				=	0xC7,
 CCE4511_CHN2_DPD_REG_ADDR				=	0xC8,
 CCE4511_CHN2_CYCT_REG_ADDR				=	0xC9,
 CCE4511_CHN2_FHD_REG_ADDR				=	0xCA,
 CCE4511_CHN2_BLVL_REG_ADDR				=	0xCB,
 CCE4511_CHN2_LSEQ_A_REG_ADDR			=	0xCC,
 CCE4511_CHN2_LHLD_A_REG_ADDR			=	0xCD,
 CCE4511_CHN2_LSEQ_B_REG_ADDR			=	0xCE,
 CCE4511_CHN2_LHLD_B_REG_ADDR			=	0xCF,
 CCE4511_CHN2_CFG4_REG_ADDR				=	0xD0,
 CCE4511_CHN2_WRP_SOTO_TIMER_REG_ADDR	=	0xD1,
 CCE4511_CHN2_STATUS_REG_ADDR			=	0XD2,
 CCE4511_CHN2_STATUS_MODE_FH_REG_ADDR	=	0xD2,
 CCE4511_CHN2_STATUS_MODE_UART_REG_ADDR	=	0xD2,
 CCE4511_CHN2_STATUS_MODE_SIO_REG_ADDR	=	0xD2,
 CCE4511_CHN2_INT_SRC_STAT_REG_ADDR		=	0xD3,
 CCE4511_CHN2_INT_EN_STAT_REG_ADDR		=	0xD4,
 CCE4511_CHN2_INT_SRC_SIO_REG_ADDR		=	0xD5,
 CCE4511_CHN2_INT_EN_SIO_REG_ADDR		=	0xD6,
 CCE4511_CHN2_INT_SRC_UART_REG_ADDR		=	0xD7,
 CCE4511_CHN2_INT_EN_UART_REG_ADDR		=	0xD8,
 CCE4511_CHN2_INT_SRC_FH_REG_ADDR		=	0xD9,
 CCE4511_CHN2_INT_EN_FH_REG_ADDR		=	0xDA,
 CCE4511_CHN2_LDRV_A_REG_ADDR			=	0xDB,
 CCE4511_CHN2_LDRV_B_REG_ADDR			=	0xDC,
 CCE4511_CHN2_LPX_CLA_TIME_REG_ADDR		=	0xDD,
 CCE4511_CHN2_LPX_OVC_TIME_REG_ADDR		=	0xDE,
 CCE4511_CHN2_CQ_OVC_TIME_REG_ADDR		=	0xDF,

 /* Chennel 4 Registers */

 CCE4511_CHN3_CFG1_REG_ADDR				=	0xE0,
 CCE4511_CHN3_CFG2_REG_ADDR				=	0xE1,
 CCE4511_CHN3_CFG3_REG_ADDR				=	0xE2,
 CCE4511_CHN3_SIO_REG_ADDR				=	0xE3,
 CCE4511_CHN3_UART_REG_ADDR				=	0xE4,
 CCE4511_CHN3_FHC_REG_ADDR				=	0xE5,
 CCE4511_CHN3_OD_REG_ADDR				=	0xE6,
 CCE4511_CHN3_MPD_REG_ADDR				=	0xE7,
 CCE4511_CHN3_DPD_REG_ADDR				=	0xE8,
 CCE4511_CHN3_CYCT_REG_ADDR				=	0xE9,
 CCE4511_CHN3_FHD_REG_ADDR				=	0xEA,
 CCE4511_CHN3_BLVL_REG_ADDR				=	0xEB,
 CCE4511_CHN3_LSEQ_A_REG_ADDR			=	0xEC,
 CCE4511_CHN3_LHLD_A_REG_ADDR			=	0xED,
 CCE4511_CHN3_LSEQ_B_REG_ADDR			=	0xEE,
 CCE4511_CHN3_LHLD_B_REG_ADDR			=	0xEF,
 CCE4511_CHN3_CFG4_REG_ADDR				=	0xF0,
 CCE4511_CHN3_WRP_SOTO_TIMER_REG_ADDR	=	0xF1,
 CCE4511_CHN3_STATUS_REG_ADDR			=	0XF2,
 CCE4511_CHN3_STATUS_MODE_FH_REG_ADDR	=	0xF2,
 CCE4511_CHN3_STATUS_MODE_UART_REG_ADDR	=	0xF2,
 CCE4511_CHN3_STATUS_MODE_SIO_REG_ADDR	=	0xF2,
 CCE4511_CHN3_INT_SRC_STAT_REG_ADDR		=	0xF3,
 CCE4511_CHN3_INT_EN_STAT_REG_ADDR		=	0xF4,
 CCE4511_CHN3_INT_SRC_SIO_REG_ADDR		=	0xF5,
 CCE4511_CHN3_INT_EN_SIO_REG_ADDR		=	0xF6,
 CCE4511_CHN3_INT_SRC_UART_REG_ADDR		=	0xF7,
 CCE4511_CHN3_INT_EN_UART_REG_ADDR		=	0xF8,
 CCE4511_CHN3_INT_SRC_FH_REG_ADDR		=	0xF9,
 CCE4511_CHN3_INT_EN_FH_REG_ADDR		=	0xFA,
 CCE4511_CHN3_LDRV_A_REG_ADDR			=	0xFB,
 CCE4511_CHN3_LDRV_B_REG_ADDR			=	0xFC,
 CCE4511_CHN3_LPX_CLA_TIME_REG_ADDR		=	0xFD,
 CCE4511_CHN3_LPX_OVC_TIME_REG_ADDR		=	0xFE,
 CCE4511_CHN3_CQ_OVC_TIME_REG_ADDR		=	0xFF,
} cce4511_reg_addr;

#ifdef __cplusplus
}
#endif


/************** (C) COPYRIGHT 2026 Calixto Systems Pvt Ltd *****END OF FILE****/


#endif // CCE4511_REG_DEF_H_INCLUDED
