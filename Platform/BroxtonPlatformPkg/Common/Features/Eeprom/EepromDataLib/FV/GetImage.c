/** @file
  DXE GetImage code for FV EEPROM raw data library instance.

  Copyright (c) 2015 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "EepromDataFvLib.h"

#include <PiDxe.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/LoadedImage.h>

EFI_STATUS
EFIAPI
GetImage (
  IN  EFI_GUID            *NameGuid,
  OUT VOID               **Buffer,
  OUT UINTN               *Size
  )
{
  UINT32                          AuthenticationStatus;
  EFI_FIRMWARE_VOLUME2_PROTOCOL  *Fv;
  EFI_HANDLE                     *HandleBuffer;
  UINTN                           HandleCount;
  UINTN                           Index;
  EFI_STATUS                      Status;

  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting...\n", __FUNCTION__, __LINE__));

  Status  = EFI_NOT_FOUND;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - LocateHandleBuffer(gEfiFirmwareVolume2ProtocolGuid)[%d] --> %r\n", __FUNCTION__, __LINE__, HandleCount, Status));
  if (EFI_ERROR (Status)) {
    //
    // Couldn't find the FV2 protocol. Bail.
    //
    goto Exit;
  }

  //
  // Find desired image in all FVs
  //
  for (Index = 0; Index < HandleCount; ++Index) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiFirmwareVolume2ProtocolGuid,
                    (VOID **) &Fv
                    );
    if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - HandleProtocol(gEfiFirmwareVolume2ProtocolGuid) --> %r\n", __FUNCTION__, __LINE__, Status));
    if (EFI_ERROR (Status)) {
      goto Exit;
    }

    //
    // Found an FV, try to find file.
    //
    *Buffer = NULL;
    *Size   = 0;
    Status  = Fv->ReadSection (
                    Fv,
                    NameGuid,
                    EFI_SECTION_RAW,
                    0,
                    Buffer,
                    Size,
                    &AuthenticationStatus
                    );
    if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Fv->ReadSection() --> %r\n", __FUNCTION__, __LINE__, Status));
    if (!EFI_ERROR (Status)) {
      //
      // Found the file. Bail.
      //
      break;
    }
  }

  //
  // Did we find the file?
  //
  if (Index == HandleCount) {
    //
    // Nope. Clear buffer and size, then bail.
    //
    *Buffer = EepromFreePool (*Buffer);
    *Size   = 0;
    Status  = EFI_NOT_FOUND;
  } else {
    Status = EFI_SUCCESS;
  }

Exit:
  EepromFreePool (HandleBuffer);
  return Status;
}
