/** @file
  Copyright (c) 2015 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _EEPROM_I2C_LIB_
#define _EEPROM_I2C_LIB_
////
//// Header files
////
#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/EepromDataLib.h>
#include <Library/GpioLib.h>
#include <Library/PcdLib.h>
#include <Library/TimerLib.h>

#include <PlatformBaseAddresses.h>
#include <SaAccess.h>
#include <ScAccess.h>
#include <ScRegs/RegsI2c.h>

//
// Defines
//
#define   EEPROM_WRITE_TIMEOUT        5
#define   FIFO_WRITE_DELAY            5
#define   EEPROM_ROUTINE_DELAY        10
#define   INVALID_I2C_ADDRESS         0xFF
#define   MAX_I2C_ADDRESS             0x7F
#define   MAX_I2C_BUS                 7

//
// Enums
//
typedef enum {
  Standard_Speed = 1,
  Fast_Speed     = 2,
  High_Speed     = 3,
  Max_Speed      = 3
} I2C_SPEED;

//
// Externs
//
extern BOOLEAN   gI2cDebugFlag;

//
// Structures
//
typedef struct _LPSS_PCI_DEVICE_INFO {
  UINT8        Segment;
  UINT8        BusNum;
  UINT8        DeviceNum;
  UINT8        FunctionNum;
  UINT32       Bar0;
  UINT32       Bar1;
} LPSS_PCI_DEVICE_INFO;

typedef struct _LPSS_I2C_CLOCK_SCL_INFO {
  UINT16       SS_SCL_HCNT;
  UINT16       SS_SCL_LCNT;
  UINT16       FS_SCL_HCNT;
  UINT16       FS_SCL_LCNT;
  UINT16       HS_SCL_HCNT;
  UINT16       HS_SCL_LCNT;
} LPSS_I2C_CLOCK_SCL_INFO;

//
// Functions
//
//
//  Desc:   Initializes the controller and returns the MMIO base address
//  Input:  Bus                   - I2C controller, 0 based
//          Address               - 7-bit slave address
//          Speed                 - Uses the I2C_SPEED enum to set the controller speed
//          I2cBaseAddress        - Pointer to the MMIO base address for the I2C controller
//  Output: EFI_SUCCESS           - Initialization completed successfully
//          EFI_DEVICE_ERROR      - I2C controller error
//          EFI_INVALID_PARAMETER - Invalid input parameter
//
EFI_STATUS
I2cInit (
  IN       UINT8     Bus,
  IN       UINT16    Address,
  IN       UINT8     Speed,
  IN OUT   UINT32   *I2cBaseAddress
  );

EFI_STATUS
I2cPoll (
  IN       UINT32     I2cBaseAddress
  );

//
//  Desc:   Read a byte from the I2C controller
//  Input:  I2cBaseAddress        - MMIO base address for the I2C controller
//          Data                  - Pointer to where to store the data
//          Start                 - Send start bit?
//          End                   - Send end bit?
//  Output: EFI_SUCCESS           - Read completed successfully
//          EFI_DEVICE_ERROR      - I2C controller error
//          EFI_INVALID_PARAMETER - Invalid input parameter
//
EFI_STATUS
I2cRead (
  IN       UINT32     I2cBaseAddress,
  IN OUT   UINT8     *Data,
  IN       BOOLEAN    Start,
  IN       BOOLEAN    End
  );

//
//  Desc:   Resets the I2C controller into a known good state
//  Input:  I2cBaseAddress        - MMIO base address for the I2C controller
//  Output: EFI_SUCCESS           - Write completed successfully
//          EFI_DEVICE_ERROR      - I2C controller error
//          EFI_INVALID_PARAMETER - Invalid input parameter
//
EFI_STATUS
I2cReset (
  IN       UINT32    I2cBaseAddress,
  IN       UINT8     Bus,
  IN       UINT16    Address,
  IN       UINT8     Speed
  );

EFI_STATUS
I2cSendCommand (
  IN       UINT32     I2cBaseAddress,
  IN       UINT32    *Data,
  IN       BOOLEAN    Start,
  IN       BOOLEAN    End
  );

//
//  Desc:   Set I2C slave offset
//  Input:  I2cBaseAddress        - MMIO base address for the I2C controller
//          Offset                - Pointer to offset data
//          Size                  - Size of the offset data
//  Output: EFI_SUCCESS           - Write completed successfully
//          EFI_DEVICE_ERROR      - I2C controller error
//          EFI_INVALID_PARAMETER - Invalid input parameter
//
EFI_STATUS
I2cSetOffset (
  IN       UINT32     I2cBaseAddress,
  IN       UINT8     *Offset,
  IN       UINT8      Size
  );

//
//  Desc:   Write a byte to the I2C controller
//  Input:  I2cBaseAddress        - MMIO base address for the I2C controller
//          Data                  - Data from the I2C controller
//          Start                 - Send start bit?
//          End                   - Send end bit?
//  Output: EFI_SUCCESS           - Write completed successfully
//          EFI_DEVICE_ERROR      - I2C controller error
//          EFI_INVALID_PARAMETER - Invalid input parameter
//
EFI_STATUS
I2cWrite (
  IN       UINT32     I2cBaseAddress,
  IN       UINT8      Data,
  IN       BOOLEAN    Start,
  IN       BOOLEAN    End
  );

#endif // _EEPROM_I2C_LIB_

