/** @file
  Multiplatform initialization header file.
  This file includes package header files, library classes.

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _MB3M_TYPEC_LIB_H_
#define _MB3M_TYPEC_LIB_H_

#include <PiDxe.h>
#include <FrameworkPei.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/GpioLib.h>
#include <Library/I2cLib.h>
#include <Library/TimerLib.h>

//
// Parade Tech PS8750 TypeC MUX
//
#define PARADE_MUX_I2C_BUS          I2C_SMBUS_BUS
#define PARADE_MUX_ADDRESS          0x90     // 0x10 0x30 0x50 0x90
#define A_STATUS                    (PARADE_MUX_ADDRESS >> 1)
#define   R_CC_STATUS                  0x18
#define   R_FIRMWARE_VERSION           0x90
#define A_MUX                       (A_STATUS + 0x01)
#define   R_DP_AUX_SNOOP_BW            0x10
#define   R_DP_AUX_SNOOP_LC            0x11
#define   R_DP_AUX_SNOOP_L0            0x12
#define   R_DP_AUX_SNOOP_L1            0x13
#define   R_DP_AUX_SNOOP_L2            0x14
#define   R_DP_AUX_SNOOP_L3            0x15
#define   R_SNOOP_D3_STATE             0x1E
#define   R_MUX_STATUS                 0x80
#define     B_DP_SELECTED                BIT7
#define     B_USB_SELECTED               BIT6
#define     B_FLIPPED                    BIT5
#define     B_OPERATION_MODE             BIT4
#define   R_MUX_DP_TRAINING            0x83
#define     B_LINK_TRAINING_DISABLE      BIT4
#define   R_MUX_DP_AUX_INTERCEPT       0x85
#define   R_MUX_DP_EQ_CONFIG           0x86
#define   R_MUX_DP_OUTPUT_CONFIG       0x87
#define   R_MUX_USB_SS2C_EQ            0x90
#define   R_MUX_USB_SS2C_CH_CONFIG     0x91
#define   R_MUX_USB_SS2C_HS_THRESH     0x93
#define   R_MUX_USB_SS2C_LFPS_THRESH   0x94
#define   R_MUX_USB_SS2C_DE            0x95
#define   R_MUX_USB_SS2C_SWING_ADJ     0x96
#define   R_MUX_USB_C2SS_CH_CONFIG     0x99
#define   R_MUX_USB_C2SS_EQ            0x9B
#define   R_MUX_USB_C2SS_HS_THRESH     0x9C
#define   R_MUX_USB_C2SS_LFPS_THRESH   0x9D
#define   R_MUX_USB_C2SS_DE            0x9E
#define   R_MUX_USB_C2SS_SWING_ADJ     0x9F
#define   R_MUX_USB_DCI_CFG            0xA3
#define   R_MUX_HPD_ASSERT             0xBC
#define     B_IN_HPD                     BIT7
#define     B_HPD_IRQ                    BIT1
#define   R_CC_STATUS_1                0xEC
#define   R_CC_STATUS_2                0xED
#define   R_CC_STATUS_3                0xEE
#define   R_MUX_USB_STATUS             0xF5
#define A_PDO                       (A_STATUS + 0x05)
#define   R_INITIATED_PDO_0            0x00
#define   R_INITIATED_PDO_1            0x01
#define   R_INITIATED_PDO_2            0x02
#define   R_INITIATED_PDO_3            0x03
#define   R_INITIATED_PDO_4            0x04
#define   R_INITIATED_PDO_5            0x05
#define   R_INITIATED_PDO_6            0x06
#define   R_INITIATED_PDO_7            0x07
#define   R_INITIATED_PDO_8            0x08
#define   R_INITIATED_PDO_9            0x09
#define   R_INITIATED_PDO_A            0x0A
#define   R_INITIATED_PDO_B            0x0B
#define   R_INITIATED_PDO_C            0x0C
#define   R_INITIATED_PDO_D            0x0D
#define   R_INITIATED_PDO_E            0x0E
#define   R_INITIATED_PDO_F            0x0F
#define   R_CHARGER_INITIATED_PDOs_0   0x10
#define   R_CHARGER_INITIATED_PDOs_1   0x11
#define   R_CHARGER_INITIATED_PDOs_2   0x12
#define   R_CHARGER_INITIATED_PDOs_3   0x13
#define   R_CHARGER_INITIATED_PDOs_4   0x14
#define   R_CHARGER_INITIATED_PDOs_5   0x15
#define   R_CHARGER_INITIATED_PDOs_6   0x16
#define   R_CHARGER_INITIATED_PDOs_7   0x17
#define   R_CHARGER_INITIATED_PDOs_8   0x18
#define   R_CHARGER_INITIATED_PDOs_9   0x19
#define   R_CHARGER_INITIATED_PDOs_A   0x1A
#define   R_CHARGER_INITIATED_PDOs_B   0x1B
#define   R_CHARGER_INITIATED_PDOs_C   0x1C
#define   R_CHARGER_INITIATED_PDOs_D   0x1D
#define   R_CHARGER_INITIATED_PDOs_E   0x1E
#define   R_CHARGER_INITIATED_PDOs_F   0x1F
#define   R_SINK_CHOSEN_PDO_INDEX      0x20
#define   R_SINK_REQUEST_CURRENT       0x21
#define   R_PS_RDY_TRIGGER             0x22
#define   R_DP_ALT_MODE_STATUS         0x2F
#define   R_CHARGER_PDO_SELECT_INDEX   0x30
#define   R_CHARGER_CURRENT_REQUEST    0x31
#define   R_REQUEST_MESSAGE_TRIGGER    0x32
#define   R_CHARGER_PS_RDY_STATUS      0x33
#define A_MAX_ADDRESS               (A_STATUS + 0x06)
#define MUX_TABLE_NULL              0xFFFF
#define MUX_RETRY_COUNT             0x03
#define MUX_TABLE_STRING_LENGTH     32

#define MUX_HPD_GPIO                NW_GPIO_200
#define MUX_I2C_DELAY               500

typedef struct {
  UINT8         Address;
  UINT8         Register;
  UINT16        Data;
  UINT8         OrgData;
  CHAR8         String[MUX_TABLE_STRING_LENGTH];
} MUX_PROGRAMMING_TABLE;

EFI_STATUS
EFIAPI
MB3SetupTypecMuxAux (
  VOID
  );

#endif

