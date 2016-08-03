/** @file
  This file includes a memory call back function notified when MRC is done,
  following action is performed in this file,
    1. ICH initialization after MRC.
    2. SIO initialization.
    3. Install ResetSystem and FinvFv PPI.
    4. Set MTRR for PEI
    5. Create FV HOB and Flash HOB

  Copyright (c) 2010 - 2015, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PlatformPei.h"
#include <Ppi/Cache.h>
#include <Library/BaseCryptLib.h>
#include <Guid/PlatformInfo.h>
#include "ChvAccess.h"

/**
  This function will be called when MRC is done.

  @param[in]   PeiServices         General purpose services available to every PEIM.
  @param[in]   NotifyDescriptor    Information about the notify event..
  @param[in]   Ppi                 The notify context.

  @retval EFI_SUCCESS              If the function completed successfully.

**/
EFI_STATUS
EFIAPI
MemoryDiscoveredPpiNotifyCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS       Status;
  EFI_BOOT_MODE    BootMode;
  UINT32           Pages;
  VOID*            Memory;
  UINTN            Size;

  Status = (*PeiServices)->GetBootMode (
                             (const EFI_PEI_SERVICES **)PeiServices,
                             &BootMode
                             );

  if (BootMode == BOOT_IN_RECOVERY_MODE) {
    //
    // Install Recovery PPI
    //
    Status = InitializeRecovery ((EFI_PEI_SERVICES **)PeiServices);
    ASSERT_EFI_ERROR (Status);

  }

  if (BootMode != BOOT_ON_S3_RESUME) {
    Size =  PcdGet32(PcdFlashFvRecovery2Size);
    if (Size > 0) {
      Pages=  (Size + 0xFFF)/0x1000;

      Memory = AllocatePages ( Pages );
      CopyMem(Memory , (VOID *) PcdGet32(PcdFlashFvRecovery2Base) , Size);

      //
      // We don't verify just load
      //
      PeiServicesInstallFvInfoPpi (
        NULL,
        (VOID *) Memory,
        PcdGet32 (PcdFlashFvRecovery2Size),
        NULL,
        NULL
      );
    }

    if (BootMode != BOOT_IN_RECOVERY_MODE){
      Size =  PcdGet32(PcdFlashFvMainSize);
      Pages=  (Size + 0xFFF)/0x1000;

      Memory = AllocatePages (Pages);
      CopyMem(Memory , (VOID *) PcdGet32(PcdFlashFvMainBase) , Size);

      PeiServicesInstallFvInfoPpi (
        NULL,
        (VOID *) Memory,
        PcdGet32 (PcdFlashFvMainSize),
        NULL,
        NULL
        );
    }
  }

  if (BootMode == BOOT_ON_S3_RESUME) {
    PeiServicesInstallFvInfoPpi (
      NULL,
      (VOID *) (UINTN) (PcdGet32 (PcdFlashFvRecovery2Base)),
      PcdGet32 (PcdFlashFvRecovery2Size),
      NULL,
      NULL
      );
  }

  return EFI_SUCCESS;
}

