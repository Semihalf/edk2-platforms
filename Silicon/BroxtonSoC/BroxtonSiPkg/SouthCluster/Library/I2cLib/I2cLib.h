/** @file
  Copyright (c) 2015 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _I2C_COMMON_H_
#define _I2C_COMMON_H_
////
//// Header files
////
#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/GpioLib.h>
#include <Library/I2cLib.h>
#include <Library/PcdLib.h>
#include <Library/TimerLib.h>

#include <PlatformBaseAddresses.h>
#include <SaAccess.h>
#include <ScAccess.h>
#include <ScRegs/RegsI2c.h>

//
// Externs
//
extern BOOLEAN   gI2cDebugFlag;

//
// Structures
//
typedef struct _I2C_LPSS_PCI_DEVICE_INFO {
  UINT8        Segment;
  UINT8        BusNum;
  UINT8        DeviceNum;
  UINT8        FunctionNum;
  UINT32       Bar0;
  UINT32       Bar1;
} I2C_LPSS_PCI_DEVICE_INFO;

typedef struct _LPSS_I2C_CLOCK_SCL_INFO {
  UINT16       SS_SCL_HCNT;
  UINT16       SS_SCL_LCNT;
  UINT16       FS_SCL_HCNT;
  UINT16       FS_SCL_LCNT;
  UINT16       HS_SCL_HCNT;
  UINT16       HS_SCL_LCNT;
} LPSS_I2C_CLOCK_SCL_INFO;

#endif // _I2C_COMMON_H_

