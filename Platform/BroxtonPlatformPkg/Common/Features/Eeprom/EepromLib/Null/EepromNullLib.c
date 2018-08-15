/** @file
  Common EEPROM library instance.

  Copyright (c) 2015 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "EepromNullLib.h"

EFI_STATUS
EFIAPI
EraseEeprom (
  IN       UINT8    LibraryIndex
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
GetEepromStructure (
  IN       UINT8      LibraryIndex,
  IN OUT   CHAR8      Signature[EEPROM_SIGNATURE_SIZE],
  IN OUT   UINT8    **Buffer,
  IN OUT   UINT32    *Size
  )
{
  return EFI_UNSUPPORTED;
}

UINT32
EFIAPI
GetImageSize (
  IN       UINT8      LibraryIndex
  )
{
  return 0;
}

EFI_STATUS
EFIAPI
GetNextEepromStructure (
  IN       UINT8      LibraryIndex,
  IN OUT   UINT32    *Index,
  IN OUT   UINT8    **Buffer,
  IN OUT   UINT32    *Size
  )
{
  return EFI_UNSUPPORTED;
}

UINT8
EFIAPI
GetValidEepromLibrary (
  IN       BOOLEAN   CopyToMemory
  )
{
  return EEPROM_NULL;
}

VOID
InitializeEepromPcds (VOID)
{
}

EFI_STATUS
EFIAPI
ValidateEeprom (
  IN       UINT8    LibraryIndex
  )
{
  return EFI_UNSUPPORTED;
}
