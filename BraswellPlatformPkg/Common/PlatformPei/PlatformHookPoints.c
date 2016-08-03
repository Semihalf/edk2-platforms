/** @file
  This PEIM provides several hook points for initializing platform at different POST stage.

  Copyright (c) 2013 - 2015, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PlatformPei.h"
#include "PlatformBaseAddresses.h"
#include "PchRegs.h"
#include <Guid/PlatformInfo.h>
#include "ChvAccess.h"
#include <Library/GpioLib.h>
#include <Guid/Chv2Variable.h>
#include <Ppi/ChvPlatformPolicyPpi.h>
#include <Ppi/BoardDetection.h>
#include <Guid/SetupVariable.h>

EFI_STATUS
EFIAPI
PreMemoryInitialization(
  IN CONST EFI_PEI_SERVICES                     **PeiServices
  )
{
  //
  // Add code here for pre-memory initialization.
  //


  return EFI_SUCCESS;
}




EFI_STATUS
EFIAPI
PostMemoryInitialization(
  IN EFI_PEI_SERVICES                     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR            *NotifyDescriptor,
  IN VOID                                 *Ppi
  )
{
  //
  // Add code here for platform initialization after DRAM initialization but before chipset initialization.
  //

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
PostSiliconInitialization(
  IN EFI_PEI_SERVICES                     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR            *NotifyDescriptor,
  IN VOID                                 *Ppi
  )
{
  //
  // Add code here for platform initialization after all silicon initialization (FSP APIs) have been done.
  //


  return EFI_SUCCESS;
}

