/** @file
  Generic template for a child of the IchSmm driver.

  Copyright (c) 2004 - 2015, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PlatformSmm.h"
#include <Protocol/CpuIo2.h>

/**
  Initializes the SMM Handler Driver

  @param[in]   ImageHandle
  @param[in]   SystemTable

  @retval    None

**/
EFI_STATUS
EFIAPI
InitializePlatformSmm (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                                Status;
  EFI_HANDLE                                Handle;
  EFI_SMM_POWER_BUTTON_REGISTER_CONTEXT     PowerButtonContext;
  EFI_SMM_POWER_BUTTON_DISPATCH2_PROTOCOL   *PowerButtonDispatch;
  EFI_SMM_ICHN_DISPATCH_PROTOCOL            *IchnDispatch;
  EFI_SMM_SX_DISPATCH2_PROTOCOL             *SxDispatch;
  EFI_SMM_SX_REGISTER_CONTEXT               EntryDispatchContext;
  EFI_SMM_SW_DISPATCH2_PROTOCOL             *SwDispatch;
  EFI_SMM_SW_REGISTER_CONTEXT               SwContext;

  //
  // Get the Power Button protocol
  //
  Status = gSmst->SmmLocateProtocol (
                  &gEfiSmmPowerButtonDispatch2ProtocolGuid,
                  NULL,
                  (VOID **) &PowerButtonDispatch
                  );
  ASSERT_EFI_ERROR (Status);


  PowerButtonContext.Phase = EfiPowerButtonEntry;
  Status = PowerButtonDispatch->Register (
                                  PowerButtonDispatch,
                                  PowerButtonCallback,
                                  &PowerButtonContext,
                                  &Handle
                                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Get the Sx dispatch protocol
  //
  Status = gSmst->SmmLocateProtocol (
                  &gEfiSmmSxDispatch2ProtocolGuid,
                  NULL,
                  (VOID **) &SxDispatch
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Register entry phase call back function
  //
  EntryDispatchContext.Type  = SxS3;
  EntryDispatchContext.Phase = SxEntry;
  Status = SxDispatch->Register (
                         SxDispatch,
                         S3SleepEntryCallBack,
                         &EntryDispatchContext,
                         &Handle
                         );

  EntryDispatchContext.Type  = SxS4;
  Status = SxDispatch->Register (
                         SxDispatch,
                         S4SleepCallBack,
                         &EntryDispatchContext,
                         &Handle
                         );
  ASSERT_EFI_ERROR (Status);

  EntryDispatchContext.Type  = SxS5;
  Status = SxDispatch->Register (
                         SxDispatch,
                         S5SleepCallBack,
                         &EntryDispatchContext,
                         &Handle
                         );
  ASSERT_EFI_ERROR (Status);

  //
  //  Get the Sw dispatch protocol
  //
  Status = gSmst->SmmLocateProtocol (
                  &gEfiSmmSwDispatch2ProtocolGuid,
                  NULL,
                  (VOID **) &SwDispatch
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Sample software SMI handler.
  // Register ACPI enable handler
  //
  SwContext.SwSmiInputValue = ACPI_ENABLE;
  Status = SwDispatch->Register (
                         SwDispatch,
                         EnableAcpiCallback,
                         &SwContext,
                         &Handle
                         );
  ASSERT_EFI_ERROR (Status);

  //
  // Sample software SMI handler.
  // Register ACPI disable handler.
  //
  SwContext.SwSmiInputValue = ACPI_DISABLE;
  Status = SwDispatch->Register (
                         SwDispatch,
                         DisableAcpiCallback,
                         &SwContext,
                         &Handle
                         );
  ASSERT_EFI_ERROR (Status);

  //
  // Get the ICHn protocol
  //
  Status = gSmst->SmmLocateProtocol (
                  &gEfiSmmIchnDispatchProtocolGuid,
                  NULL,
                  (VOID **) &IchnDispatch
                  );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

/**
  @param[in]  DispatchHandle      The handle of this callback, obtained when registering
  @param[in]  DispatchContext     The predefined context which contained sleep type and phase

  @retval  EFI_SUCCESS            Operation successfully performed

**/
EFI_STATUS
S3SleepEntryCallBack (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  CONST VOID                    *DispatchContext,
  IN  OUT VOID                      *CommBuffer  OPTIONAL,
  IN  UINTN                         *CommBufferSize  OPTIONAL
  )

{
//
// Add code here
//


  return EFI_SUCCESS;
}

/**
  When a power button event happens, it shuts off the machine.

  @param[in]  DispatchHandle      The handle of this callback, obtained when registering
  @param[in]  DispatchContext     The predefined context which contained sleep type and phase

  @retval

**/
EFI_STATUS
PowerButtonCallback (
  IN  EFI_HANDLE                              DispatchHandle,
  IN  CONST VOID                              *DispatchContext,
  IN  OUT VOID                                *CommBuffer  OPTIONAL,
  IN  UINTN                                   *CommBufferSize  OPTIONAL
  )
{
  //
  // Add code here
  //

  return EFI_SUCCESS;
}

/**
  @param[in]  DispatchHandle      The handle of this callback, obtained when registering
  @param[in]  DispatchContext     The predefined context which contained sleep type and phase

**/
EFI_STATUS
EFIAPI
S5SleepCallBack (
  IN  EFI_HANDLE                              DispatchHandle,
  IN  CONST VOID                              *DispatchContext,
  IN  OUT VOID                                *CommBuffer  OPTIONAL,
  IN  UINTN                                   *CommBufferSize  OPTIONAL
  )
{
  //
  // Add code here
  //

  return EFI_SUCCESS;
}

/**
  @param[in]  DispatchHandle      The handle of this callback, obtained when registering
  @param[in]  DispatchContext     The predefined context which contained sleep type and phase

  @retval                         Clears the Save State bit in the clock.

**/
EFI_STATUS
EFIAPI
S4SleepCallBack (
  IN  EFI_HANDLE                              DispatchHandle,
  IN  CONST VOID                              *DispatchContext,
  IN  OUT VOID                                *CommBuffer  OPTIONAL,
  IN  UINTN                                   *CommBufferSize  OPTIONAL
  )
{
  //
  // Add code here
  //

  return EFI_SUCCESS;
}



/**
  Sample software SMI handler to enable ACPI mode.


  @param[in]  DispatchHandle       EFI Handle
  @param[in]  DispatchContext      Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT

  @retval Nothing

**/
EFI_STATUS
EFIAPI
EnableAcpiCallback (
  IN  EFI_HANDLE                              DispatchHandle,
  IN  CONST VOID                              *DispatchContext,
  IN  OUT VOID                                *CommBuffer  OPTIONAL,
  IN  UINTN                                   *CommBufferSize  OPTIONAL
  )
{

  return EFI_SUCCESS;
}

/**
  Sample Software SMI handler.


  @param[in] DispatchHandle      EFI Handle
  @param[in] DispatchContext     Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT

  @retval Nothing

**/
EFI_STATUS
EFIAPI
DisableAcpiCallback (
  IN  EFI_HANDLE                              DispatchHandle,
  IN  CONST VOID                              *DispatchContext,
  IN  OUT VOID                                *CommBuffer  OPTIONAL,
  IN  UINTN                                   *CommBufferSize  OPTIONAL
  )
{


  return EFI_SUCCESS;
}








