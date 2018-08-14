/** @file
  Gpio setting for multiplatform.

  Copyright (c) 2010 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "BoardGpios.h"
#include <Library/EepromPlatformLib.h>

/**
  Returns the Correct GPIO table for Mobile/Desktop respectively.
  Before call it, make sure PlatformInfoHob->BoardId&PlatformFlavor is get correctly.

  @param[in] PeiServices         General purpose services available to every PEIM.
  @param[in] PlatformInfoHob     PlatformInfoHob pointer with PlatformFlavor specified.

  @retval    EFI_SUCCESS         The function completed successfully.
  @retval    EFI_DEVICE_ERROR    KSC fails to respond.

**/
EFI_STATUS
Minnow3ModuleMultiPlatformGpioTableInit (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PLATFORM_INFO_HOB      *PlatformInfoHob
  )
{
  DEBUG ((DEBUG_INFO, "%a()...\n", __FUNCTION__));
  DEBUG ((DEBUG_INFO, "PlatformInfoHob->BoardId: 0x%02X\n", PlatformInfoHob->BoardId));

  //
  // Select/modify the GPIO initialization data based on the Board ID.
  //
  switch (PlatformInfoHob->BoardId) {
    case BOARD_ID_LFH_CRB:
    case BOARD_ID_MINNOW:
    case BOARD_ID_MINNOW_MODULE:
    case BOARD_ID_BENSON:
      PlatformInfoHob->PlatformGpioSetting_SW = &mMinnow3Module_GpioInitData_SW[0];
      PlatformInfoHob->PlatformGpioSetting_W  = &mMinnow3Module_GpioInitData_W[0];
      PlatformInfoHob->PlatformGpioSetting_NW = &mMinnow3Module_GpioInitData_NW[0];
      PlatformInfoHob->PlatformGpioSetting_N  = &mMinnow3Module_GpioInitData_N[0];
      break;
    default:
      PlatformInfoHob->PlatformGpioSetting_SW = &mMinnow3Module_GpioInitData_SW[0];
      PlatformInfoHob->PlatformGpioSetting_W  = &mMinnow3Module_GpioInitData_W[0];
      PlatformInfoHob->PlatformGpioSetting_NW = &mMinnow3Module_GpioInitData_NW[0];
      PlatformInfoHob->PlatformGpioSetting_N  = &mMinnow3Module_GpioInitData_N[0];
      break;
  }

  return EFI_SUCCESS;
}


/**
  Set GPIO Lock for security.

**/
VOID
Minnow3ModuleSetGpioPadCfgLock (
  VOID
  )
{
  UINT32   Data32;

  Data32 = 0;

  //
  // JTAG
  //
  GpioLockPadCfg (N_TCK);
  GpioLockPadCfg (N_TRST_B);
  GpioLockPadCfg (N_TMS);
  GpioLockPadCfg (N_TDI);
  GpioLockPadCfg (N_TDO);

  //
  // Power
  //
  GpioLockPadCfg (NW_PMIC_THERMTRIP_B);
  GpioLockPadCfg (NW_PROCHOT_B);

  //
  // Touch
  //
  GpioLockPadCfg (NW_GPIO_118);
  GpioLockPadCfg (NW_GPIO_119);
  GpioLockPadCfg (NW_GPIO_120);
  GpioLockPadCfg (NW_GPIO_121);
  GpioLockPadCfg (NW_GPIO_122);
  GpioLockPadCfg (NW_GPIO_123);

  //
  // SPI
  //
  GpioLockPadCfg (NW_GPIO_97);
  GpioLockPadCfg (NW_GPIO_98);
  GpioLockPadCfg (NW_GPIO_99);
  GpioLockPadCfg (NW_GPIO_100);
  GpioLockPadCfg (NW_GPIO_101);
  GpioLockPadCfg (NW_GPIO_102);
  GpioLockPadCfg (NW_GPIO_103);
  GpioLockPadCfg (NW_FST_SPI_CLK_FB);

  //
  // SMBus
  // Set SMBus GPIO PAD_CFG.PADRSTCFG to Powergood
  //
  Data32  = GpioPadRead (SW_SMB_ALERTB);
  Data32 &= ~(BIT31 | BIT30);
  GpioPadWrite (SW_SMB_ALERTB, Data32);

  Data32  = GpioPadRead (SW_SMB_CLK);
  Data32 &= ~(BIT31 | BIT30);
  GpioPadWrite (SW_SMB_CLK, Data32);

  Data32  = GpioPadRead (SW_SMB_DATA);
  Data32 &= ~(BIT31 | BIT30);
  GpioPadWrite (SW_SMB_DATA, Data32);

  GpioLockPadCfg (SW_SMB_ALERTB);
  GpioLockPadCfg (SW_SMB_CLK);
  GpioLockPadCfg (SW_SMB_DATA);
}

/**
  Programs the GPIOs according to the platform.
  Before call it, make sure PlatformInfoHob->BoardId&PlatformFlavor is get correctly.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] PlatformInfoHob      PlatformInfoHob pointer with PlatformFlavor specified.

  @retval    EFI_SUCCESS          The function completed successfully.
  @retval    EFI_DEVICE_ERROR     KSC fails to respond.

**/
EFI_STATUS
Minnow3ModuleMultiPlatformGpioProgram (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PLATFORM_INFO_HOB      *PlatformInfoHob
  )
{
  UINTN                             VariableSize;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI   *VariableServices;
  SYSTEM_CONFIGURATION              SystemConfiguration;

  DEBUG ((DEBUG_INFO, "%a(#%4d) - Starting...\n", __FUNCTION__, __LINE__));

  if (PlatformInfoHob->BoardId != BOARD_ID_MINNOW_MODULE) {
    DEBUG ((DEBUG_INFO, "%a(#%4d) - This is not a Minnow Board 3 Module\n", __FUNCTION__, __LINE__));
  } else {
    //
    // Get setup data
    //
    VariableSize = sizeof (SYSTEM_CONFIGURATION);
    ZeroMem (&SystemConfiguration, sizeof (SYSTEM_CONFIGURATION));
   (*PeiServices)->LocatePpi (
                      PeiServices,
                      &gEfiPeiReadOnlyVariable2PpiGuid,
                      0,
                      NULL,
                      (VOID **) &VariableServices
                       );
    VariableServices->GetVariable (
                        VariableServices,
                        PLATFORM_SETUP_VARIABLE_NAME,
                        &gEfiSetupVariableGuid,
                        NULL,
                        &VariableSize,
                        &SystemConfiguration
                        );
    //
    // PAD programming
    //
    DEBUG ((DEBUG_INFO, "%a(#%4d) - PAD programming, Board ID: 0x%X\n", __FUNCTION__, __LINE__, PlatformInfoHob->BoardId));
    GpioPadConfigTable (sizeof (mMinnow3Module_GpioInitData_N)  / sizeof (mMinnow3Module_GpioInitData_N[0]),  PlatformInfoHob->PlatformGpioSetting_N);
    GpioPadConfigTable (sizeof (mMinnow3Module_GpioInitData_NW) / sizeof (mMinnow3Module_GpioInitData_NW[0]), PlatformInfoHob->PlatformGpioSetting_NW);
    GpioPadConfigTable (sizeof (mMinnow3Module_GpioInitData_W)  / sizeof (mMinnow3Module_GpioInitData_W[0]),  PlatformInfoHob->PlatformGpioSetting_W);
    GpioPadConfigTable (sizeof (mMinnow3Module_GpioInitData_SW) / sizeof (mMinnow3Module_GpioInitData_SW[0]), PlatformInfoHob->PlatformGpioSetting_SW);
    DEBUG ((DEBUG_INFO, "%a(#%4d) - PAD programming done\n", __FUNCTION__, __LINE__));

    //
    // Dump Community registers
    //
    DumpGpioCommunityRegisters (NORTH);
    DumpGpioCommunityRegisters (NORTHWEST);
    DumpGpioCommunityRegisters (WEST);
    DumpGpioCommunityRegisters (SOUTHWEST);

    //
    // Dump GPIO tables
    //
    DEBUG ((DEBUG_INFO, "%a(#%4d) - Dump Community pad registers, Board ID: 0x%X\n", __FUNCTION__, __LINE__, PlatformInfoHob->BoardId));
    DumpGpioPadTable (sizeof (mMinnow3Module_GpioInitData_N)  / sizeof (mMinnow3Module_GpioInitData_N[0]),  PlatformInfoHob->PlatformGpioSetting_N);
    DumpGpioPadTable (sizeof (mMinnow3Module_GpioInitData_NW) / sizeof (mMinnow3Module_GpioInitData_NW[0]), PlatformInfoHob->PlatformGpioSetting_NW);
    DumpGpioPadTable (sizeof (mMinnow3Module_GpioInitData_W)  / sizeof (mMinnow3Module_GpioInitData_W[0]),  PlatformInfoHob->PlatformGpioSetting_W);
    DumpGpioPadTable (sizeof (mMinnow3Module_GpioInitData_SW) / sizeof (mMinnow3Module_GpioInitData_SW[0]), PlatformInfoHob->PlatformGpioSetting_SW);

    //
    // Now program any EEPROM defined GPIOs
    //
    EepromProgramGpioPads ();
  }

  return EFI_SUCCESS;
}

