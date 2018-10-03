/** @file
  Copyright (c) 2015 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _EEPROM_LIB_
#define _EEPROM_LIB_
////
//// Header files
////
#include <Uefi.h>

#include <EepromStruct.h>
#include <Library/EepromDataLib.h>
#include <Library/EepromPlatformLib.h>


////
//// defines
////
#define EEPROM_SIGNATURE_SIZE   9


////
//// Externs
////
extern BOOLEAN    mEepromLibDebugFlag;


////
//// Functions
////
VOID
EepromDumpParagraph (
  IN   UINTN   DebugMask,
  IN   VOID   *Ptr,
  IN   UINTN   Count
  );

//
// Desc:        Registers the raw data libraries
// Variables:   None
// Return:      EFI_SUCCESS
//
EFI_STATUS
EFIAPI
EepromInitConstructor (VOID);

EFI_STATUS
EFIAPI
EraseEeprom (
  IN       UINT8     LibraryIndex
  );

EFI_STATUS
EFIAPI
GetEepromStructure (
  IN       UINT8      LibraryIndex,
  IN OUT   CHAR8      Signature[EEPROM_SIGNATURE_SIZE],
  IN OUT   UINT32    *StructureIndex,
  IN OUT   UINT8    **Buffer,
  IN OUT   UINT32    *Size
  );

/**
  Returns the EEPROM structure data

  @param[in]   LibraryIndex         Which library to use to get the structure
  @param[in]   Signature            Signature of the structure we are looking for
  @param[out]  StructureIndex       Index of where to start looking for the next structure
  @param[in]   HeaderSize           Size of the structure header
  @param[out]  StructureHeader      Header of the structure we're looking for
                                    NOTE: Caller is responsible for freeing this memory.
  @param[out]  StructureData        Buffer containing the structure data
  @param[out]  DataSize             Size of the StructureData buffer

  @retval      EFI_SUCCESS          Structure data found
  @retval      EFI_NOT_FOUND        Structure data not found
  @retval      EFI_NOT_READY        Structure data not ready yet
**/
EFI_STATUS
EFIAPI
GetEepromStructureData (
  IN       UINT8     *LibraryIndex,
  IN       CHAR8      Signature[EEPROM_SIGNATURE_SIZE],
  IN OUT   UINT32    *StructureIndex,
  IN       UINT32     HeaderSize,
  OUT      UINT8    **StructureHeader,
  OUT      UINT8    **StructureData,
  OUT      UINT32    *DataSize
  );

/**
  Returns the EEPROM image size

  @param[in]   LibraryIndex         Which library to use to get the structure

  @retval      UINT32               Size of that image
**/
UINT32
EFIAPI
GetImageSize (
  IN       UINT8      LibraryIndex
  );

EFI_STATUS
EFIAPI
GetNextEepromStructure (
  IN       UINT8      LibraryIndex,
  IN OUT   UINT32    *Index,
  IN OUT   UINT8    **Buffer,
  IN OUT   UINT32    *Size
  );

UINT8
EFIAPI
GetValidEepromLibrary (
  IN       BOOLEAN   CopyToMemory
  );

BOOLEAN
EFIAPI
InPeiPhase (VOID);

EFI_STATUS
EFIAPI
ValidateEeprom (
  IN       UINT8     LibraryIndex
  );

#endif // _EEPROM_LIB_

