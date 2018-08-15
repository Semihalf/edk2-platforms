/** @file
  Copyright (c) 2015 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _EEPROM_DATA_LIB_COMMON_
#define _EEPROM_DATA_LIB_COMMON_
////
//// Header files
////
#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/EepromDataLib.h>
#include <Library/PrintLib.h>

////
//// Defines
////
#define ALLOCATION_HOB_HEADER_SIGNATURE   SIGNATURE_32 ('e', 'H', 'B', 'a')
#define MAX_EEPROM_HOB_SIZE               (25 * 1024)
#define MAX_EEPROM_ALLOCATION_FLAG        ((MAX_EEPROM_HOB_SIZE / 0x10) / 8)
#define MAX_EEPROM_ALLOCATION_SIZE        (MAX_EEPROM_HOB_SIZE - MAX_EEPROM_ALLOCATION_FLAG)

////
//// TypeDefs
////
typedef
EFI_STATUS
(EFIAPI *NEM_TO_MEMORY_EEPROM_FUNCTION) (VOID);

typedef
EFI_STATUS
(EFIAPI *READ_EEPROM_FUNCTION) (
  IN       UINT32    Offset,
  IN OUT   UINT32   *Size,
  IN OUT   UINT8    *Buffer,
  IN OUT   VOID     *FunctionInfo
  );

typedef
EFI_STATUS
(EFIAPI *WRITE_EEPROM_FUNCTION) (
  IN       UINT32    Offset,
  IN OUT   UINT32   *Size,
  IN OUT   UINT8    *Buffer,
  IN OUT   VOID     *FunctionInfo
  );

////
//// Structures
////
typedef struct {
   BOOLEAN                         Active;
   READ_EEPROM_FUNCTION            ReadFunction;
   WRITE_EEPROM_FUNCTION           WriteFunction;
} EEPROM_DATA_LIBRARY_INDEX;

typedef struct {
 UINT32   Signature;
  UINT8   UsageFlag[MAX_EEPROM_ALLOCATION_FLAG];   // Size / paragraph / byte size
} EEPROM_ALLOCATION_STRUCT;

////
//// Externs
////
extern EEPROM_DATA_LIBRARY_INDEX    mEepromDataLibIndex[];

////
//// Functions
////
//
// Desc:        Copies any data from NEM into real memory
// Variables:   NONE
// Return:      EFI_SUCCESS             Data copied successfully
//              EFI_UNSUPPORTED         This function is not supported
//              EFI_INVALID_PARAMETER   One of the parameters is invalid
//
EFI_STATUS
EFIAPI
NemToMemoryEepromEeprom (VOID);

//
// Desc:        Reads from the EEPROM and copies to the passed in buffer.
// Variables:   Offset         Start copying from the offset
//              Size           Size of the buffer and the number of bytes to copy
//              Buffer         Storage buffer for the copied data from the EEPROM
//              FunctionInfo   Pointer to function specific data
// Return:      EFI_SUCCESS             Data copied successfully
//              EFI_UNSUPPORTED         This function is not supported
//              EFI_INVALID_PARAMETER   One of the parameters is invalid
//              EFI_NOT_READY           Called before all necessary protocols available
//              EFI_DEVICE_ERROR        Communication error with device
//
EFI_STATUS
EFIAPI
ReadEepromEeprom (
  IN       UINT32    Offset,
  IN OUT   UINT32   *Size,
  IN OUT   UINT8    *Buffer,
  IN OUT   VOID     *FunctionInfo
  );

//
// Desc:        Writes to the EEPROM and copies to the passed in buffer.
// Variables:   Offset         Start copying from the offset
//              Size           Size of the buffer and the number of bytes to copy
//              Buffer         Storage buffer for the copied data to the EEPROM
//              FunctionInfo   Pointer to function specific data
// Return:      EFI_SUCCESS             Data copied successfully
//              EFI_UNSUPPORTED         This function is not supported
//              EFI_INVALID_PARAMETER   One of the parameters is invalid
//              EFI_NOT_READY           Called before all necessary protocols available
//              EFI_DEVICE_ERROR        Communication error with device
//
EFI_STATUS
EFIAPI
WriteEepromEeprom (
  IN       UINT32    Offset,
  IN OUT   UINT32   *Size,
  IN OUT   UINT8    *Buffer,
  IN OUT   VOID     *FunctionInfo
  );

//
// Desc:        Copies any data from NEM into real memory
// Variables:   NONE
// Return:      EFI_SUCCESS             Data copied successfully
//              EFI_UNSUPPORTED         This function is not supported
//              EFI_INVALID_PARAMETER   One of the parameters is invalid
//
EFI_STATUS
EFIAPI
NemToMemoryEepromFv (VOID);

//
// Desc:        Reads from the EEPROM and copies to the passed in buffer.
// Variables:   Offset         Start copying from the offset
//              Size           Size of the buffer and the number of bytes to copy
//              Buffer         Storage buffer for the copied data from the EEPROM
//              FunctionInfo   Pointer to function specific data
// Return:      EFI_SUCCESS             Data copied successfully
//              EFI_UNSUPPORTED         This function is not supported
//              EFI_INVALID_PARAMETER   One of the parameters is invalid
//              EFI_NOT_READY           Called before all necessary protocols available
//              EFI_DEVICE_ERROR        Communication error with device
//
EFI_STATUS
EFIAPI
ReadEepromFv (
  IN       UINT32    Offset,
  IN OUT   UINT32   *Size,
  IN OUT   UINT8    *Buffer,
  IN OUT   VOID     *FunctionInfo
  );

//
// Desc:        Writes to the EEPROM and copies to the passed in buffer.
// Variables:   Offset         Start copying from the offset
//              Size           Size of the buffer and the number of bytes to copy
//              Buffer         Storage buffer for the copied data to the EEPROM
//              FunctionInfo   Pointer to function specific data
// Return:      EFI_SUCCESS             Data copied successfully
//              EFI_UNSUPPORTED         This function is not supported
//              EFI_INVALID_PARAMETER   One of the parameters is invalid
//              EFI_NOT_READY           Called before all necessary protocols available
//              EFI_DEVICE_ERROR        Communication error with device
//
EFI_STATUS
EFIAPI
WriteEepromFv (
  IN       UINT32    Offset,
  IN OUT   UINT32   *Size,
  IN OUT   UINT8    *Buffer,
  IN OUT   VOID     *FunctionInfo
  );

//
// Desc:        Copies any data from NEM into real memory
// Variables:   NONE
// Return:      EFI_SUCCESS             Data copied successfully
//              EFI_UNSUPPORTED         This function is not supported
//              EFI_INVALID_PARAMETER   One of the parameters is invalid
//
EFI_STATUS
EFIAPI
NemToMemoryEepromMemory (VOID);

//
// Desc:        Reads from the EEPROM and copies to the passed in buffer.
// Variables:   Offset         Start copying from the offset
//              Size           Size of the buffer and the number of bytes to copy
//              Buffer         Storage buffer for the copied data from the EEPROM
//              FunctionInfo   Pointer to function specific data
// Return:      EFI_SUCCESS             Data copied successfully
//              EFI_UNSUPPORTED         This function is not supported
//              EFI_INVALID_PARAMETER   One of the parameters is invalid
//              EFI_NOT_READY           Called before all necessary protocols available
//              EFI_DEVICE_ERROR        Communication error with device
//
EFI_STATUS
EFIAPI
ReadEepromMemory (
  IN       UINT32    Offset,
  IN OUT   UINT32   *Size,
  IN OUT   UINT8    *Buffer,
  IN OUT   VOID     *FunctionInfo
  );

//
// Desc:        Writes to the EEPROM and copies to the passed in buffer.
// Variables:   Offset         Start copying from the offset
//              Size           Size of the buffer and the number of bytes to copy
//              Buffer         Storage buffer for the copied data to the EEPROM
//              FunctionInfo   Pointer to function specific data
// Return:      EFI_SUCCESS             Data copied successfully
//              EFI_UNSUPPORTED         This function is not supported
//              EFI_INVALID_PARAMETER   One of the parameters is invalid
//              EFI_NOT_READY           Called before all necessary protocols available
//              EFI_DEVICE_ERROR        Communication error with device
//
EFI_STATUS
EFIAPI
WriteEepromMemory (
  IN       UINT32    Offset,
  IN OUT   UINT32   *Size,
  IN OUT   UINT8    *Buffer,
  IN OUT   VOID     *FunctionInfo
  );

//
// Desc:        Copies any data from NEM into real memory
// Variables:   NONE
// Return:      EFI_SUCCESS             Data copied successfully
//              EFI_UNSUPPORTED         This function is not supported
//              EFI_INVALID_PARAMETER   One of the parameters is invalid
//
EFI_STATUS
EFIAPI
NemToMemoryEepromNull (VOID);

//
// Desc:        Reads from the EEPROM and copies to the passed in buffer.
// Variables:   Offset         Start copying from the offset
//              Size           Size of the buffer and the number of bytes to copy
//              Buffer         Storage buffer for the copied data from the EEPROM
//              FunctionInfo   Pointer to function specific data
// Return:      EFI_SUCCESS             Data copied successfully
//              EFI_UNSUPPORTED         This function is not supported
//              EFI_INVALID_PARAMETER   One of the parameters is invalid
//              EFI_NOT_READY           Called before all necessary protocols available
//              EFI_DEVICE_ERROR        Communication error with device
//
EFI_STATUS
EFIAPI
ReadEepromNull (
  IN       UINT32    Offset,
  IN OUT   UINT32   *Size,
  IN OUT   UINT8    *Buffer,
  IN OUT   VOID     *FunctionInfo
  );

//
// Desc:        Writes to the EEPROM and copies to the passed in buffer.
// Variables:   Offset         Start copying from the offset
//              Size           Size of the buffer and the number of bytes to copy
//              Buffer         Storage buffer for the copied data to the EEPROM
//              FunctionInfo   Pointer to function specific data
// Return:      EFI_SUCCESS             Data copied successfully
//              EFI_UNSUPPORTED         This function is not supported
//              EFI_INVALID_PARAMETER   One of the parameters is invalid
//              EFI_NOT_READY           Called before all necessary protocols available
//              EFI_DEVICE_ERROR        Communication error with device
//
EFI_STATUS
EFIAPI
WriteEepromNull (
  IN       UINT32    Offset,
  IN OUT   UINT32   *Size,
  IN OUT   UINT8    *Buffer,
  IN OUT   VOID     *FunctionInfo
  );

#endif // _EEPROM_DATA_LIB_COMMON_

