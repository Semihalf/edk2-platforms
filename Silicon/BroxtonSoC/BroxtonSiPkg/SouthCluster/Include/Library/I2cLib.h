/** @file
  Copyright (c) 2015 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _I2C_LIB_H_
#define _I2C_LIB_H_

////
//// Header files
////
#include <Uefi.h>


////
//// Defines
////
#define   I2C_WRITE_TIMEOUT           5
#define   FIFO_WRITE_DELAY            5
#define   I2C_ROUTINE_DELAY           10
#define   INVALID_I2C_ADDRESS         0xFF
#define   MAX_I2C_ADDRESS             0x7F
#define   MAX_I2C_BUS                 7


////
//// Enums
////
typedef enum {
  Standard_Speed = 1,
  Fast_Speed     = 2,
  High_Speed     = 3,
  Max_Speed      = 3
} I2C_SPEED_ENUM;


////
//// Functions
////
//
//  Desc:   Initializes the controller and returns the MMIO base address
//  Input:  Bus                   - I2C controller, 0 based
//          Address               - 7-bit slave address
//          Speed                 - Uses the I2C_SPEED_ENUM enum to set the controller speed
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

/**
  Read bytes from I2C Device
  This is actual I2C hardware operation function.

  @param[in]  BusNo                 I2C Bus number to which the I2C device has been connected
  @param[in]  SlaveAddress          Slave address of the I2C device (7-bit)
  @param[in]  ReadBytes             Number of bytes to be read
  @param[out] ReadBuffer            Address to which the value read has to be stored
  @param[in]  Start                 It controls whether a RESTART is issued before the byte is sent or received.
  @param[in]  End                   It controls whether a STOP is issued after the byte is sent or received.

  @retval     EFI_SUCCESS           The byte value read successfully
  @retval     EFI_DEVICE_ERROR      Operation failed
  @retval     EFI_TIMEOUT           Hardware retry timeout
  @retval     Others                Failed to read a byte via I2C

**/
EFI_STATUS
ByteReadI2C_Basic (
  IN  UINT8        BusNo,
  IN  UINT8        SlaveAddress,
  IN  UINTN        ReadBytes,
  OUT UINT8        *ReadBuffer,
  IN  UINT8        Start,
  IN  UINT8        End
  );

/**
  Write bytes to I2C Device
  This is actual I2C hardware operation function.

  @param[in]  BusNo                 I2C Bus number to which the I2C device has been connected
  @param[in]  SlaveAddress          Slave address of the I2C device (7-bit)
  @param[in]  WriteBytes            Number of bytes to be written
  @param[in]  WriteBuffer           Address to which the byte value has to be written
  @param[in]  Start                 It controls whether a RESTART is issued before the byte is sent or received.
  @param[in]  End                   It controls whether a STOP is issued after the byte is sent or received.

  @retval     EFI_SUCCESS           The byte value written successfully
  @retval     EFI_DEVICE_ERROR      Operation failed
  @retval     EFI_TIMEOUT           Hardware retry timeout
  @retval     Others                Failed to write a byte via I2C

**/
EFI_STATUS
ByteWriteI2C_Basic (
  IN  UINT8        BusNo,
  IN  UINT8        SlaveAddress,
  IN  UINTN        WriteBytes,
  IN  UINT8        *WriteBuffer,
  IN  UINT8        Start,
  IN  UINT8        End
  );

/**
  Read bytes from I2C Device

  @param[in]  BusNo               I2C Bus number to which the I2C device has been connected
  @param[in]  SlaveAddress        Slave address of the I2C device (7-bit)
  @param[in]  Offset              Register offset from which the data has to be read
  @param[in]  ReadBytes           Number of bytes to be read
  @param[out] ReadBuffer          Address to which the value read has to be stored

  @retval     EFI_SUCCESS         Read bytes from I2C device successfully
  @retval     Others              Return status depends on ByteReadI2C_Basic

**/
EFI_STATUS
ByteReadI2C (
  IN  UINT8        BusNo,
  IN  UINT8        SlaveAddress,
  IN  UINT8        Offset,
  IN  UINTN        ReadBytes,
  OUT UINT8        *ReadBuffer
  );

/**
  Write bytes to I2C Device

  @param[in]  BusNo               I2C Bus number to which the I2C device has been connected
  @param[in]  SlaveAddress        Slave address of the I2C device (7-bit)
  @param[in]  Offset              Register offset from which the data has to be read
  @param[in]  WriteBytes          Number of bytes to be written
  @param[in]  WriteBuffer         Address to which the byte value has to be written

  @retval     EFI_SUCCESS         Write bytes to I2C device successfully
  @retval     Others              Return status depends on ByteWriteI2C_Basic

**/
EFI_STATUS
ByteWriteI2C (
  IN  UINT8        BusNo,
  IN  UINT8        SlaveAddress,
  IN  UINT8        Offset,
  IN  UINTN        WriteBytes,
  IN  UINT8        *WriteBuffer
  );

#endif  // _I2C_LIB_H_

