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
#include <Library/I2cLib.h>
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

EFI_STATUS
EFIAPI
Up2GetDdrId(
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  OUT UINT8                     *DdrId
  )
{
  BXT_CONF_PAD0   padConfg0;
  BXT_CONF_PAD1   padConfg1;
  IN UINT32       CommAndOffset;

  //
  // DDR_ID1    DDR_ID0  Memory
  // GPIO_215   GPIO_214
  //  0          0        2G
  //  0          1        4G
  //  1          0        8G
  //
  
  //BXT_GPIO_PAD_CONF(L"GPIO_214 PMIC_BCUDISW2",   M0   ,    GPO   ,GPIO_D,  HI     ,   NA      ,Wake_Disabled, P_20K_L,    NA   ,    NA  ,NA        ,     NA,  GPIO_PADBAR+0x00D8,  NORTHWEST),//Feature: BT WAKE to Device Net in Sch: NGFF_BT_DEV_WAKE_N
  //BXT_GPIO_PAD_CONF(L"GPIO_215 PMIC_BCUDISCRIT", M0   ,    GPO   ,GPIO_D,  HI     ,   NA      ,Wake_Disabled, P_20K_L,    NA   ,    NA  ,NA        ,     NA,  GPIO_PADBAR+0x00E0,  NORTHWEST),//Feature: RF_KILL_GPS       Net in Sch: RF_KILL_GPS_1P8_N

  //
  // DDR_ID0: GPIO_214
  //
  CommAndOffset = GetCommOffset (NORTHWEST, 0x00D8);
  padConfg0.padCnf0 = GpioPadRead (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET);
  padConfg0.r.PMode = 0;
  padConfg0.r.GPIORxTxDis = 0x1;
  GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET, padConfg0.padCnf0);
  //
  // Set to Pull Up 20K
  //
  padConfg1.r.Term = 0xC;
  GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF1_OFFSET, padConfg1.padCnf1);
  
  
  //
  // DDR_ID1: GPIO_215
  //
  CommAndOffset = GetCommOffset (NORTHWEST, 0x00E0);
  padConfg0.padCnf0 = GpioPadRead (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET);
  padConfg0.r.PMode = 0;
  padConfg0.r.GPIORxTxDis = 0x1;
  GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET, padConfg0.padCnf0);
  //Set to Pull Up 20K
  padConfg1.r.Term = 0xC;
  GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF1_OFFSET, padConfg1.padCnf1);
 

  *DdrId = (UINT8) (((GpioPadRead (GetCommOffset (NORTHWEST, 0x00D8) + BXT_GPIO_PAD_CONF0_OFFSET) & BIT1) >> 1) | \
                      (((GpioPadRead (GetCommOffset (NORTHWEST, 0x00E0) + BXT_GPIO_PAD_CONF0_OFFSET) & BIT1) >> 1) << 1));

  DEBUG ((EFI_D_INFO,  "Ddr4Id is: %02X\n", *DdrId));


  return EFI_SUCCESS;
}


