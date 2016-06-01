/** @file
  This file provides the definitions of GPIO Table structure

Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under
the terms and conditions of the BSD License that accompanies this distribution.
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _PLATFORM_GPIO_TABLE_H_
#define _PLATFORM_GPIO_TABLE_H_

#include "GpioAttributes.h"

typedef struct {
  UINT8 Usage;
  UINT8 FuncNum;
  UINT8 InitLevel;
  UINT8 TrigType;
  UINT8 DriveType;
  UINT8 PullDirection;
  UINT8 PullStrength;
  UINT8 IntType;
  UINT8 ShareMode;
  UINT8 DirectIrqNum;
  UINT8 IntLineNum;
  UINT8 LightModeBar;
  UINT8 GlitchFilterConfig;
  UINT8 InvertRxTx;
  BOOLEAN ReservedPin;
}PLATFORM_GPIO_TABLE_ROW;

#define GPIO_TABLE_NORTH_ROW_NUM      59
#define GPIO_TABLE_EAST_ROW_NUM       24
#define GPIO_TABLE_SOUTHEAST_ROW_NUM  55
#define GPIO_TABLE_SOUTHWEST_ROW_NUM  56
#define GPIO_TABLE_ROW_NUM            194

#endif
