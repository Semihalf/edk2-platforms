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

PAD_ID_INFO gMB3MRawBoardIdPadInfo[] = {
  {NW_PMIC_STDBY,   EnPd, P_20K_L}, // bit 0
  {NW_GPIO_213,     EnPd, P_20K_L}, // bit 1
  {NW_PMIC_RESET_B, EnPd, P_20K_L}, // bit 2
  {N_GPIO_27,       EnPd, P_20K_L}, // bit 3
  {N_GPIO_72,       EnPd, P_20K_L}, // bit 4
  {N_GPIO_64,       EnPd, P_20K_L}  // bit 5
};

//
// MinnowBoard v3 = 0x0000000F
//======================================================
// NW_PMIC_STDBY   - BOARD_ID0           - 10k PU  -> 1   xxxxxxx1
// NW_GPIO_213     - BOARD_ID1           - 10k PU  -> 1   xxxxxx1x
// NW_PMIC_RESET_B - BOARD_ID2           - 10k PU  -> 1   xxxxx1xx
// N_GPIO_27       - BOARD_ID3           - 10k PU  -> 1   xxxx1xxx
// N_GPIO_72       - GP_CAMERA_GP72      - Float   -> 0   xxx0xxxx
// N_GPIO_64       - GP_CAMERA_GP64      - Float   -> 0   xx0xxxxx
//======================================================  00001111b

// UpSquared = 0x00000010/11
//======================================================
// NW_PMIC_STDBY   - DDR_CH              - Unknown -> 1   xxxxxxx?
// NW_GPIO_213     - mSATA_PCIe_SEL_N    - Float   -> 0   xxxxxx0x
// NW_PMIC_RESET_B - NC                  - Float   -> 0   xxxxx0xx
// N_GPIO_27       - FPGA_fw_reload      - Float   -> 0   xxxx0xxx
// N_GPIO_72       - GP_CAMERASB10       - 1K PU   -> 1   xxx1xxxx
// N_GPIO_64       - GP_CAMERASB2        - Float   -> 0   xx0xxxxx
//======================================================  0001000?b

// Benson Glacier = 0x00000014
//======================================================
// NW_PMIC_STDBY   - BOARD_ID0           - 10k PD  -> 0   xxxxxxx0
// NW_GPIO_213     - BOARD_ID1           - 10k PD  -> 0   xxxxxx0x
// NW_PMIC_RESET_B - BOARD_ID2           - 10k PU  -> 1   xxxxx1xx
// N_GPIO_27       - NC                  - Float   -> 0   xxxx0xxx
// N_GPIO_72       - BOARD_ID3           - 10k PU  -> 1   xxx1xxxx
// N_GPIO_64       - NC                  - Float   -> 0   xx0xxxxx
//======================================================  00010100b

// Aurora Glacier = 0x00000016
//======================================================
// NW_PMIC_STDBY   - BOARD_ID0           - 10k PD  -> 0   xxxxxxx0
// NW_GPIO_213     - BOARD_ID1           - 10k PU  -> 1   xxxxxx1x
// NW_PMIC_RESET_B - BOARD_ID2           - 10k PU  -> 1   xxxxx1xx
// N_GPIO_27       - NC                  - Float   -> 0   xxxx0xxx
// N_GPIO_72       - BOARD_ID3           - 10k PU  -> 1   xxx1xxxx
// N_GPIO_64       - NC                  - Float   -> 0   xx0xxxxx
//======================================================  00010110b

// MinnowBoard v3 Module = 0x00000020
//======================================================
// NW_PMIC_STDBY   - NC                  - Float   -> 0   xxxxxxx0
// NW_GPIO_213     - NC                  - Float   -> 0   xxxxxx0x
// NW_PMIC_RESET_B - NC                  - Float   -> 0   xxxxx0xx
// N_GPIO_27       - NC                  - Float   -> 0   xxxx0xxx
// N_GPIO_72       - NC                  - Float   -> 0   xxx0xxxx
// N_GPIO_64       - CAM0_RST#           - 10k PU  -> 1   xx1xxxxx
//======================================================  00100000b

// LeafHill = 0x00000027
//======================================================
// NW_PMIC_STDBY   - BOARD_ID0           - 10k PU  -> 1   xxxxxxx1
// NW_GPIO_213     - BOARD_ID1           - 10k PU  -> 1   xxxxxx1x
// NW_PMIC_RESET_B - BOARD_ID2           - 10k PU  -> 1   xxxxx1xx
// N_GPIO_27       - SOC_GPIO27          - Float   -> 0   xxxx0xxx
// N_GPIO_72       - NC                  - Float   -> 0   xxx0xxxx
// N_GPIO_64       - MCSI1_XSHUTDN       - 0k PU   -> 1   xx1xxxxx
//======================================================  00100111b

TRANSLATE_ID_INFO gMB3MBoardIdInfo[] = {
  {0x0000000F, BOARD_ID_MINNOW,        "Minnow Board v3"},
  {0x00000010, BOARD_ID_UP2,           "Up Squared"},
  {0x00000011, BOARD_ID_UP2,           "Up Squared"},
  {0x00000014, BOARD_ID_BENSON,        "Benson Glacier"},
  {0x00000016, BOARD_ID_AURORA,        "Aurora Glacier"},
  {0x00000020, BOARD_ID_MINNOW_MODULE, "Minnow Board v3 Module"},
  {0x00000027, BOARD_ID_LFH_CRB,       "Leafhill"},
  {0xFFFFFFFF, BOARD_ID_APL_UNKNOWN,   "Unknown Board ID"}
};

PAD_ID_INFO gMB3MRawFabIdPadInfo[] = {
  {SW_GPIO_207,     EnPd, P_20K_L}  // bit 0 - GPIO 207
};

// MinnowBoard v3 Module, Fab A = 0x00000000
//======================================================
// SW_GPIO_207     - NC                  - Float   -> 0   xxxxxxx0
//======================================================  00000000b

// MinnowBoard v3 Module, Fab C = 0x00000001
//======================================================
// SW_GPIO_207     - PCIE_WAKE_LAN1_1V8# - 10k PU  -> 1   xxxxxxx1
//======================================================  00000001b

TRANSLATE_ID_INFO gMB3MFabIdInfo[] = {
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
  RawBoardId = Minnow3ModuleGetId (gMB3MRawBoardIdPadInfo, sizeof (gMB3MRawBoardIdPadInfo) / sizeof (gMB3MRawBoardIdPadInfo[0]));
  DEBUG ((DEBUG_INFO, "%a(#%3d) - Raw BoardId: %02X\n", __FUNCTION__, __LINE__, RawBoardId));

  //
  // Convert from a 32-bit raw BoardId to an 8-bit one.
  //
  for (index = 0; index < sizeof (gMB3MBoardIdInfo) / sizeof (gMB3MBoardIdInfo[0]); index++) {
    if ((gMB3MBoardIdInfo[index].RawId == RawBoardId) || (gMB3MBoardIdInfo[index].RawId == 0xFFFFFFFF)) {
      BoardId = gMB3MBoardIdInfo[index].TranslatedId;
      DEBUG ((DEBUG_INFO, "%a(#%3d) - BoardId: %02X = %a\n", __FUNCTION__, __LINE__, BoardId, gMB3MBoardIdInfo[index].Description));
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
  RawFabId = Minnow3ModuleGetId (gMB3MRawFabIdPadInfo, sizeof (gMB3MRawFabIdPadInfo) / sizeof (gMB3MRawFabIdPadInfo[0]));
  DEBUG ((DEBUG_INFO, "%a(#%3d) - Raw FabId: %02X\n", __FUNCTION__, __LINE__, RawFabId));

  //
  // Convert from a 32-bit raw FabId to an 8-bit one.
  //
  for (index = 0; index < sizeof (gMB3MFabIdInfo) / sizeof (gMB3MFabIdInfo[0]); index++) {
    if ((gMB3MFabIdInfo[index].RawId == RawFabId) || (gMB3MFabIdInfo[index].RawId == 0xFFFFFFFF)) {
      FabId = gMB3MFabIdInfo[index].TranslatedId;
      DEBUG ((DEBUG_INFO, "%a(#%3d) - FabId: %02X = %a\n", __FUNCTION__, __LINE__, FabId, gMB3MFabIdInfo[index].Description));
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

