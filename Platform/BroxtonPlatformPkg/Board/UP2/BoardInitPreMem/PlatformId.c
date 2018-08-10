/** @file
  Implement Platform ID code.

  Copyright (c) 2015 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/I2CLib.h>
#include <Library/GpioLib.h>
#include <Guid/PlatformInfo.h>
#include "PlatformId.h"

//
// Board ID pin definiton
//
// Bit     SoC Pin            Value (Leaf Hill)  Value (Minnow Board 3)    Value (Benson Glacier)   Value (Aurora Glacier)
// 0    GPIO_224 (J45)         PU (1)              PU (1)                    PD  (0)                  PD  (0)
// 1    GPIO_213 (M47)         PU (1)              PU (1)                    PD  (0)                  PU  (1)
// 2    GPIO_223 (H48)         PU (1)              PU (1)                    PU  (1)                  PU  (1)
// 3    GP_CAMERASB10 (R34)    PD (0)              PU (1)                    PU  (1)                  PU  (1)
//

EFI_STATUS
EFIAPI
Up2GetEmbeddedBoardIdFabId(
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  OUT UINT8                     *BoardId,
  OUT UINT8                     *FabId
  )
{
  //
  // Hardcode to UP2.
  //
  *BoardId = 0x01; 
  DEBUG ((DEBUG_INFO,  "BoardId is: %02X\n", *BoardId));
  
  //
  // Hardcode to FAB_A.
  //
  *FabId = 0x00;
  DEBUG ((EFI_D_INFO,  "FabId is: %02X\n", *FabId));


  return EFI_SUCCESS;
}


