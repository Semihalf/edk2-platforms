/** @file
  FV EEPROM raw data PEI library instance.

  Copyright (c) 2015 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "EepromDataFvLib.h"

#include <PiPei.h>
#include <Library/PeiServicesLib.h>

EFI_STATUS
EFIAPI
GetImage (
  IN  EFI_GUID            *NameGuid,
  OUT VOID               **Buffer,
  OUT UINTN               *Size
  )
{
  BOOLEAN                      FileFound;
  EFI_PEI_FILE_HANDLE          FileHandle;
  EFI_FV_FILE_INFO             FileInfo;
  UINTN                        Instance;
  EFI_COMMON_SECTION_HEADER   *Section;
  EFI_STATUS                   Status;
  EFI_PEI_FV_HANDLE            VolumeHandle;

  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting...\n", __FUNCTION__, __LINE__));

  //
  // Initialize variables
  //
  FileFound = FALSE;
  Instance  = 0;

  //
  // Search FVs for the file
  //
  while (TRUE) {
    //
    // Search thru the FVs
    //
    Status = PeiServicesFfsFindNextVolume (Instance++, &VolumeHandle);
    if (EFI_ERROR (Status)) {
      //
      // Error or end of FVs.
      //
      if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_ERROR, "%a (#%4d) - PeiServicesFfsFindNextVolume() -> %r\n", __FUNCTION__, __LINE__, Status));
      goto Exit;
    }
    //
    // Search the FV for the file.
    //
    Status = PeiServicesFfsFindFileByName (NameGuid, VolumeHandle, &FileHandle);
    if (EFI_ERROR (Status)) {
      //
      // File not found. Continue search.
      //
      if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_ERROR, "%a (#%4d) - PeiServicesFfsFindFileByName() -> %r\n", __FUNCTION__, __LINE__, Status));
      continue;
    }
    //
    // Search the file for the section.
    //
    Status = PeiServicesFfsFindSectionData (EFI_SECTION_RAW, FileHandle, Buffer);
    if (EFI_ERROR (Status)) {
      //
      // Section not found. Continue search.
      //
      if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_ERROR, "%a (#%4d) - PeiServicesFfsFindSectionData() -> %r\n", __FUNCTION__, __LINE__, Status));
      continue;
    }
    //
    // We've got the file and section.
    //
    Status = PeiServicesFfsGetFileInfo (FileHandle, &FileInfo);
    if (EFI_ERROR (Status)) {
      //
      // File info error. Continue search.
      //
      if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_ERROR, "%a (#%4d) - PeiServicesFfsGetFileInfo() -> %r\n", __FUNCTION__, __LINE__, Status));
      continue;
    }
    FileFound = TRUE;
    Section = (EFI_COMMON_SECTION_HEADER *) FileInfo.Buffer;
    if (IS_SECTION2 (Section)) {
      ASSERT (SECTION2_SIZE (Section) > 0x00FFFFFF);
      *Size = SECTION2_SIZE (Section) - sizeof (EFI_COMMON_SECTION_HEADER2);
    } else {
      *Size = SECTION_SIZE (Section) - sizeof (EFI_COMMON_SECTION_HEADER);
    }
    break;
  }

  if (FileFound) {
    Status = EFI_SUCCESS;
  } else {
    *Buffer = NULL;
    *Size   = 0;
  }

Exit:
  return Status;
}
