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
  Code in this function will be triggered immediately after PCI enumeration is done.

  @param[in] Event                A pointer to the Event that triggered the callback.
  @param[in] Context              A pointer to private data registered with the callback function.

**/
VOID
EFIAPI
EfiPciEnumerationCompleteProtocolCallback (
  IN EFI_EVENT                      Event,
  IN VOID                           *Context
  )
{
  //
  // Add code here.
  //


}

/**
  Code in this function will be triggered just before SMRAM is locked.

  @param[in] Event                A pointer to the Event that triggered the callback.
  @param[in] Context              A pointer to private data registered with the callback function.

**/
VOID
EFIAPI
SmmReadyToLockProtocolCallback (
  IN EFI_EVENT                      Event,
  IN VOID                           *Context
  )
{
  //
  // Add code here.
  //


}

/**
  Code in this function will be executed just before OS Loader is loaded and started.

  @param[in] Event                A pointer to the Event that triggered the callback.
  @param[in] Context              A pointer to private data registered with the callback function.

**/
VOID
ReadyToBootEventCallback (
  EFI_EVENT  Event,
  VOID       *Context
  )
{
  gBS->CloseEvent (Event);

  //
  // Add code here.
  //


}

/**
  Code in this function will be executed just before OS Loader hands off control to OS Kernel.

  @param[in] Event                A pointer to the Event that triggered the callback.
  @param[in] Context              A pointer to private data registered with the callback function.

**/
EFI_STATUS
EFIAPI
ExitBootServicesEventCallback (
  IN EFI_EVENT    Event,
  IN VOID         *Context

  )
{

  //
  // Closed the event to avoid call twice
  //
  gBS->CloseEvent (Event);

  //
  // Add code here.
  //



  return EFI_SUCCESS;
}

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
CallPlatformHooks (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  VOID               *Registration;
  EFI_EVENT          ReadyToBootEvent;
  EFI_EVENT          ExitBootServicesEvent;
  EFI_STATUS         Status;

  EfiCreateProtocolNotifyEvent (
    &gEfiPciEnumerationCompleteProtocolGuid,
    TPL_CALLBACK,
    EfiPciEnumerationCompleteProtocolCallback,
    NULL,
    &Registration
    );

  EfiCreateProtocolNotifyEvent (
    &gEfiDxeSmmReadyToLockProtocolGuid,
    TPL_CALLBACK,
    SmmReadyToLockProtocolCallback,
    NULL,
    &Registration
    );

  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             ReadyToBootEventCallback,
             NULL,
             &ReadyToBootEvent
             );

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  (EFI_EVENT_NOTIFY) ExitBootServicesEventCallback,
                  NULL,
                  &gEfiEventExitBootServicesGuid,
                  &ExitBootServicesEvent
                  );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}
