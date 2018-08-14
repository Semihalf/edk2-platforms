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
#include "PlatformId.h"

PAD_ID_INFO gRawBoardIdPadInfo[] = {
  {NW_PMIC_STDBY,   EnPd, P_20K_L}, // bit 0
  {NW_GPIO_213,     EnPd, P_20K_L}, // bit 1
  {NW_PMIC_RESET_B, EnPd, P_20K_L}, // bit 2
  {NW_PMIC_PWRGOOD, EnPd, P_20K_L}, // bit 3
  {N_GPIO_27,       EnPd, P_20K_L}, // bit 4
  {N_GPIO_72,       EnPd, P_20K_L}, // bit 5
  {N_GPIO_64,       EnPd, P_20K_L}  // bit 6
};

//
// MinnowBoard v3 = 0x00000017
//===========================================
// NW_PMIC_STDBY   - BOARD_ID0 - 10k PU -> 1   xxxxxxx1
// NW_GPIO_213     - BOARD_ID1 - 10k PU -> 1   xxxxxx1x
// NW_PMIC_RESET_B - BOARD_ID2 - 10k PU -> 1   xxxxx1xx
// NW_PMIC_PWRGOOD -           - 10k PD -> 0   xxxx0xxx
// N_GPIO_27       - BOARD_ID3 - 10k PU -> 1   xxx1xxxx
// N_GPIO_72       -           - Float  -> 0   xx0xxxxx
// N_GPIO_64       -           - Float  -> 0   x0xxxxxx
//===========================================  00010111b

// Benson Glacier = 0x00000024
//===========================================
// NW_PMIC_STDBY   - BOARD_ID0 - 10k PD -> 0   xxxxxxx0
// NW_GPIO_213     - BOARD_ID1 - 10k PD -> 0   xxxxxx0x
// NW_PMIC_RESET_B - BOARD_ID2 - 10k PU -> 1   xxxxx1xx
// NW_PMIC_PWRGOOD -           - Float  -> 0   xxxx0xxx
// N_GPIO_27       -           - Float  -> 0   xxx0xxxx
// N_GPIO_72       - BOARD_ID3 - 10k PU -> 1   xx1xxxxx
// N_GPIO_64       -           - Float  -> 0   x0xxxxxx
//===========================================  00100100b

// Aurora Glacier = 0x00000026
//===========================================
// NW_PMIC_STDBY   - BOARD_ID0 - 10k PD -> 0   xxxxxxx0
// NW_GPIO_213     - BOARD_ID1 - 10k PU -> 1   xxxxxx1x
// NW_PMIC_RESET_B - BOARD_ID2 - 10k PU -> 1   xxxxx1xx
// NW_PMIC_PWRGOOD -           - Float  -> 0   xxxx0xxx
// N_GPIO_27       -           - Float  -> 0   xxx0xxxx
// N_GPIO_72       - BOARD_ID3 - 10k PU -> 1   xx1xxxxx
// N_GPIO_64       -           - Float  -> 0   x0xxxxxx
//===========================================  00100110b

// MinnowBoard v3 Module = 0x00000040
//===========================================
// NW_PMIC_STDBY   -           - Float  -> 0   xxxxxxx0
// NW_GPIO_213     -           - Float  -> 0   xxxxxx0x
// NW_PMIC_RESET_B -           - Float  -> 0   xxxxx0xx
// NW_PMIC_PWRGOOD -           - Float  -> 0   xxxx0xxx
// N_GPIO_27       -           - Float  -> 0   xxx0xxxx
// N_GPIO_72       -           - Float  -> 0   xx0xxxxx
// N_GPIO_64       -           - 10k PU -> 1   x1xxxxxx
//===========================================  01000000b

// LeafHill = 0x00000047
//===========================================
// NW_PMIC_STDBY   - BOARD_ID0 - 10k PU -> 1   xxxxxxx1
// NW_GPIO_213     - BOARD_ID1 - 10k PU -> 1   xxxxxx1x
// NW_PMIC_RESET_B - BOARD_ID2 - 10k PU -> 1   xxxxx1xx
// NW_PMIC_PWRGOOD - BOARD_ID3 - 10k PD -> 0   xxxx0xxx
// N_GPIO_27       -           - Float  -> 0   xxx0xxxx
// N_GPIO_72       -           - Float  -> 0   xx0xxxxx
// N_GPIO_64       -           - 0k PU  -> 1   x1xxxxxx
//===========================================  01000111b

TRANSLATE_ID_INFO gBoardIdInfo[] = {
  {0x00000017, BOARD_ID_MINNOW,        "Minnow Board v3"},
  {0x00000024, BOARD_ID_BENSON,        "Benson Glacier"},
  {0x00000026, BOARD_ID_AURORA,        "Aurora Glacier"},
  {0x00000040, BOARD_ID_MINNOW_MODULE, "Minnow Board v3 Module"},
  {0x00000047, BOARD_ID_LFH_CRB,       "Leafhill"},
  {0xFFFFFFFF, BOARD_ID_APL_UNKNOWN,   "Unknown Board ID"}
};

PAD_ID_INFO gRawFabIdPadInfo[] = {
  {SW_GPIO_207,     EnPd, P_20K_L}  // bit 0 - GPIO 207
};

// MinnowBoard v3 Module, Fab A = 0x00000000
//===========================================
// SW_GPIO_207                 - Float  -> 0   xxxxxxx0
//===========================================  00000000b

// MinnowBoard v3 Module, Fab C = 0x00000001
//===========================================
// SW_GPIO_207                 - 10k PU -> 1   xxxxxxx1
//===========================================  00000001b

TRANSLATE_ID_INFO gFabIdInfo[] = {
  {0x00000000,     FAB_ID_A, "Fab ID A"},
  {0x00000001,     FAB_ID_C, "Fab ID C"},
  {0xFFFFFFFF,  UNKNOWN_FAB, "Unknown Fab ID"}
};

PAD_ID_INFO gMB3MHwconfPadInfo[] = {
  {W_GPIO_128, DisPuPd, P_NONE}, // bit 0 - HWCONF0
  {W_GPIO_131, DisPuPd, P_NONE}, // bit 1 - HWCONF1
  {W_GPIO_130, DisPuPd, P_NONE}, // bit 2 - HWCONF2
  {W_GPIO_129, DisPuPd, P_NONE}, // bit 3 - HWCONF3
  {W_GPIO_139, DisPuPd, P_NONE}, // bit 4 - HWCONF4
  {W_GPIO_138, DisPuPd, P_NONE}, // bit 5 - HWCONF5
  {NW_GPIO_80, DisPuPd, P_NONE}, // bit 6 - HWCONF6
  {NW_GPIO_81, DisPuPd, P_NONE}, // bit 7 - HWCONF7
  {NW_GPIO_83, DisPuPd, P_NONE}  // bit 8 - HWCONF8
};

UINT32
EFIAPI
Minnow3ModuleGetId (
  IN   PAD_ID_INFO   *PadInfoPtr,
  IN   UINT8          NumberOfEntries
  )
{
  UINT32          CommAndOffset;
  UINT8           index;
  BXT_CONF_PAD0   padConfg0;
  BXT_CONF_PAD0   padConfg0Org;
  BXT_CONF_PAD1   padConfg1;
  BXT_CONF_PAD1   padConfg1Org;
  UINT32          ReturnId;

  //
  // Get ID from PAD
  //
  if ((PadInfoPtr == NULL) || (NumberOfEntries == 0)) {
    //
    // Nothing in structure. Skip.
    //
    ReturnId = 0xFFFFFFFF;
  } else {
    ReturnId = 0;
    for (index = 0; index < NumberOfEntries; index++) {
      //
      // Read original pad programming
      //
      CommAndOffset           = PadInfoPtr[index].CommAndOffset;
      padConfg0Org.padCnf0    = GpioPadRead (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET);
      padConfg1Org.padCnf1    = GpioPadRead (CommAndOffset + BXT_GPIO_PAD_CONF1_OFFSET);
      //
      // Set pad to be able to read the GPI level
      //
      padConfg0.padCnf0       = GpioPadRead (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET);
      padConfg1.padCnf1       = GpioPadRead (CommAndOffset + BXT_GPIO_PAD_CONF1_OFFSET);
      padConfg0.r.PMode       = M0;                        // Set to GPIO mode
      padConfg0.r.GPIORxTxDis = GPI;                       // Set to GPI
      padConfg1.r.IOSTerm     = PadInfoPtr[index].IOSTerm; // Enable pull-up/down
      padConfg1.r.Term        = PadInfoPtr[index].Term;    // Set pull-up/down value
      GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET, padConfg0.padCnf0);
      GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF1_OFFSET, padConfg1.padCnf1);
      //
      // Read the pad GPI level and OR into ID
      //
      padConfg0.padCnf0 = GpioPadRead (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET);
      ReturnId         |= padConfg0.r.GPIORxState << index;
      //
      // Restore orginal pad programming.
      //
      GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF0_OFFSET, padConfg0Org.padCnf0);
      GpioPadWrite (CommAndOffset + BXT_GPIO_PAD_CONF1_OFFSET, padConfg1Org.padCnf1);
    }
  }
  return ReturnId;
}

UINT8
EFIAPI
Minnow3ModuleGetCommonBoardId (
  VOID
  )
{
  UINT8    BoardId;
  UINT8    index;
  UINT32   RawBoardId;

  DEBUG ((DEBUG_INFO, "%a(#%3d) - Starting...\n", __FUNCTION__, __LINE__));

  //
  // Get BoardId
  //
  RawBoardId = Minnow3ModuleGetId (gRawBoardIdPadInfo, sizeof (gRawBoardIdPadInfo) / sizeof (gRawBoardIdPadInfo[0]));
  DEBUG ((DEBUG_INFO, "%a(#%3d) - Raw BoardId: %02X\n", __FUNCTION__, __LINE__, RawBoardId));

  //
  // Convert from a 32-bit raw BoardId to an 8-bit one.
  //
  for (index = 0; index < sizeof (gBoardIdInfo) / sizeof (gBoardIdInfo[0]); index++) {
    if ((gBoardIdInfo[index].RawId == RawBoardId) || (gBoardIdInfo[index].RawId == 0xFFFFFFFF)) {
      BoardId = gBoardIdInfo[index].TranslatedId;
      DEBUG ((DEBUG_INFO, "%a(#%3d) - BoardId: %02X = %a\n", __FUNCTION__, __LINE__, BoardId, gBoardIdInfo[index].Description));
      break;
    }
  }
  return BoardId;
}

UINT8
EFIAPI
Minnow3ModuleGetCommonFabId (
  VOID
  )
{
  UINT8    FabId;
  UINT8    index;
  UINT32   RawFabId;

  DEBUG ((DEBUG_INFO, "%a(#%3d) - Starting...\n", __FUNCTION__, __LINE__));

  //
  // Get FabId
  //
  RawFabId = Minnow3ModuleGetId (gRawFabIdPadInfo, sizeof (gRawFabIdPadInfo) / sizeof (gRawFabIdPadInfo[0]));
  DEBUG ((DEBUG_INFO, "%a(#%3d) - Raw FabId: %02X\n", __FUNCTION__, __LINE__, RawFabId));

  //
  // Convert from a 32-bit raw FabId to an 8-bit one.
  //
  for (index = 0; index < sizeof (gFabIdInfo) / sizeof (gFabIdInfo[0]); index++) {
    if ((gFabIdInfo[index].RawId == RawFabId) || (gFabIdInfo[index].RawId == 0xFFFFFFFF)) {
      FabId = gFabIdInfo[index].TranslatedId;
      DEBUG ((DEBUG_INFO, "%a(#%3d) - FabId: %02X = %a\n", __FUNCTION__, __LINE__, FabId, gFabIdInfo[index].Description));
      break;
    }
  }
  return FabId;
}

EFI_STATUS
EFIAPI
Minnow3ModuleGetEmbeddedBoardIdFabId (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  OUT UINT8                     *BoardId,
  OUT UINT8                     *FabId
  )
{
  DEBUG ((DEBUG_INFO, "%a(#%3d) - Starting...\n", __FUNCTION__, __LINE__));

  *BoardId = Minnow3ModuleGetCommonBoardId ();
  *FabId   = Minnow3ModuleGetCommonFabId ();
  Minnow3ModuleGetHwconfStraps ();

  if (*BoardId != BOARD_ID_MINNOW_MODULE) {
    *BoardId = BOARD_ID_APL_UNKNOWN;
    *FabId   = UNKNOWN_FAB;
  }

  return EFI_SUCCESS;
}

UINT32
EFIAPI
Minnow3ModuleGetHwconfStraps (
  VOID
  )
{
  UINT32   HwconfStraps;

  //
  // Get HWCONF straps
  //
  HwconfStraps = Minnow3ModuleGetId (gMB3MHwconfPadInfo, sizeof (gMB3MHwconfPadInfo) / sizeof (gMB3MHwconfPadInfo[0]));
  DEBUG ((DEBUG_INFO, "%a(#%3d) - HWCONF : %08X\n", __FUNCTION__, __LINE__, HwconfStraps));

  return HwconfStraps;
}

