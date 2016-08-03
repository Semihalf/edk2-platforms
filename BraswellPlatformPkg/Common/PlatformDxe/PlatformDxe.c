/** @file
  Platform Initialization Driver.

  Copyright (c) 1999 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PlatformDxe.h"
#include "Platform.h"
#include <Protocol/PchPlatformPolicy.h>
#include <Protocol/CpuIo2.h>
#include <Protocol/ExitPmAuth.h>
#include <Library/S3BootScriptLib.h>
#include <Library/PciLib.h>
#include <Guid/EventGroup.h>
#include <Guid/Chv2Variable.h>
#include <Library/PcdLib.h>
#include <Include/Library/BiosIdLib.h>
#include <Protocol/IgdOpRegion.h>
#include <KscLib.h>
#include <Library/GpioLib.h>
#include <SaveMemoryConfig.h>


/**
  This is the standard EFI driver point for the Driver. This
  driver is responsible for setting up any platform specific policy or
  initialization information.

  @param[in]  ImageHandle     Handle for the image of this driver.
  @param[in]  SystemTable     Pointer to the EFI System Table.

  @retval   EFI_SUCCESS       Policy decisions set.

**/
EFI_STATUS
EFIAPI
PlatformDxeEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  VOID  *Registration;

  //
  // Initialize platform policies.
  //
  PolicyInitDxeEntryPoint (ImageHandle, SystemTable);

  //
  // Register callback to gIgdOpRegionProtocolGuid, which is installed by ChvInit.
  //
  EfiCreateProtocolNotifyEvent (&gIgdOpRegionProtocolGuid, TPL_CALLBACK, SiliconInitDoneCallback, NULL, &Registration);

  //
  // Save MRC data for S3.
  //
  SaveMemoryConfigEntryPoint ();

  //
  // Save LPSS and SCC configuration for S3.
  //
  EfiCreateProtocolNotifyEvent (&gExitPmAuthProtocolGuid, TPL_CALLBACK, SaveLpssSccPciMode, NULL, &Registration);

  //
  // Register platform hook points.
  //
  CallPlatformHooks (ImageHandle, SystemTable);

  return EFI_SUCCESS;
}
