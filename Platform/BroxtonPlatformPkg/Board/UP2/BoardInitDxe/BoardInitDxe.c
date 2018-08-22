/** @file
  Board specific functions in DXE phase to be set as dynamic PCD and consumed by
  commmon platform code.

  Copyright (c) 2009 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "BoardInitDxe.h"

GET_BOARD_NAME mUp2GetBoardNamePtr = Up2GetBoardName;
SYSTEM_CONFIGURATION  mSystemConfigurationUp;

CHAR16*
EFIAPI
Up2GetBoardName (
  IN  UINT8                   BoardId
  )
{
  STATIC CHAR16  BoardName[40];

  DEBUG ((EFI_D_INFO,  "BoardInitDxe: GetBoardName\n"));
  UnicodeSPrint (BoardName, sizeof (BoardName), L"UP2 "); //MyBoardPorting

  if (BoardId != (UINT8) BOARD_ID_UP2) {
    return NULL;
  } else {
    DEBUG ((EFI_D_INFO,  "It is UP2.\n"));
    return BoardName;
  }
}

EFI_STATUS
UpdateSetupVariable (
  VOID
  )
{
  UINTN       VarSize;
  UINT32      VariableAttributes;
  EFI_STATUS  Status;

  //
  // Update default value of Setup Variable for PCIe W/A.
  // PCIe RealTek NIC on root port #1 blocks Windows 10 installation.
  //
  
  VarSize = sizeof (SYSTEM_CONFIGURATION);
  Status = gRT->GetVariable (
                    L"Setup",
                    &gEfiSetupVariableGuid,
                    &VariableAttributes,
                    &VarSize,
                    &mSystemConfigurationUp
                    );
  ASSERT_EFI_ERROR (Status);

  if (Status == EFI_SUCCESS) {
    if (mSystemConfigurationUp.PcieRootPortEn[3] != 0) {
      mSystemConfigurationUp.PcieRootPortEn[3]= (UINT8) 0;
      Status = gRT->SetVariable (
                      L"Setup",
                      &gEfiSetupVariableGuid,
                      VariableAttributes,
                      VarSize,
                      &mSystemConfigurationUp
                      );
      ASSERT_EFI_ERROR (Status);
    }
  }
  
  return Status;
}
/**
  Set PCDs for board specific functions

  @param[in]  ImageHandle   ImageHandle of the loaded driver.
  @param[in]  SystemTable   Pointer to the EFI System Table.

  @retval     EFI_SUCCESS   The handlers were registered successfully.

**/
EFI_STATUS
EFIAPI
Up2BoardInitDxeConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  UINT8       BoardId;

  BoardId = PcdGet8 (PcdBoardId);
  if (BoardId != (UINT8) BOARD_ID_UP2) {//MyBoardPorting
    return EFI_SUCCESS;
  }

  PcdSet64 (PcdGetBoardNameFunc, (UINT64) mUp2GetBoardNamePtr);
 
  UpdateSetupVariable();

  return EFI_SUCCESS;
}

