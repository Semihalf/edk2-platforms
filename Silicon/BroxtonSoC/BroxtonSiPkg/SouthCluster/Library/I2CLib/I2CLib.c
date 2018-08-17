/** @file
  I2C library instance.

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "I2cLib.h"

//
// List of I2C controllers
//
I2C_LPSS_PCI_DEVICE_INFO  mI2cLpssPciDeviceList[] = {
  {0, DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_LPSS_I2C0,   PCI_FUNCTION_NUMBER_LPSS_I2C0, LPSS_I2C0_TMP_BAR0 + (LPSS_I2C_TMP_BAR0_DELTA*0), LPSS_I2C0_TMP_BAR0 + (LPSS_I2C_TMP_BAR0_DELTA*0) + LPSS_I2C_TMP_BAR1_OFFSET},
  {0, DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_LPSS_I2C0,   PCI_FUNCTION_NUMBER_LPSS_I2C1, LPSS_I2C0_TMP_BAR0 + (LPSS_I2C_TMP_BAR0_DELTA*1), LPSS_I2C0_TMP_BAR0 + (LPSS_I2C_TMP_BAR0_DELTA*1) + LPSS_I2C_TMP_BAR1_OFFSET},
  {0, DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_LPSS_I2C0,   PCI_FUNCTION_NUMBER_LPSS_I2C2, LPSS_I2C0_TMP_BAR0 + (LPSS_I2C_TMP_BAR0_DELTA*2), LPSS_I2C0_TMP_BAR0 + (LPSS_I2C_TMP_BAR0_DELTA*2) + LPSS_I2C_TMP_BAR1_OFFSET},
  {0, DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_LPSS_I2C0,   PCI_FUNCTION_NUMBER_LPSS_I2C3, LPSS_I2C0_TMP_BAR0 + (LPSS_I2C_TMP_BAR0_DELTA*3), LPSS_I2C0_TMP_BAR0 + (LPSS_I2C_TMP_BAR0_DELTA*3) + LPSS_I2C_TMP_BAR1_OFFSET},
  {0, DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_LPSS_I2C1,   PCI_FUNCTION_NUMBER_LPSS_I2C4, LPSS_I2C0_TMP_BAR0 + (LPSS_I2C_TMP_BAR0_DELTA*4), LPSS_I2C0_TMP_BAR0 + (LPSS_I2C_TMP_BAR0_DELTA*4) + LPSS_I2C_TMP_BAR1_OFFSET},
  {0, DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_LPSS_I2C1,   PCI_FUNCTION_NUMBER_LPSS_I2C5, LPSS_I2C0_TMP_BAR0 + (LPSS_I2C_TMP_BAR0_DELTA*5), LPSS_I2C0_TMP_BAR0 + (LPSS_I2C_TMP_BAR0_DELTA*5) + LPSS_I2C_TMP_BAR1_OFFSET},
  {0, DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_LPSS_I2C1,   PCI_FUNCTION_NUMBER_LPSS_I2C6, LPSS_I2C0_TMP_BAR0 + (LPSS_I2C_TMP_BAR0_DELTA*6), LPSS_I2C0_TMP_BAR0 + (LPSS_I2C_TMP_BAR0_DELTA*6) + LPSS_I2C_TMP_BAR1_OFFSET},
  {0, DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_LPSS_I2C1,   PCI_FUNCTION_NUMBER_LPSS_I2C7, LPSS_I2C0_TMP_BAR0 + (LPSS_I2C_TMP_BAR0_DELTA*7), LPSS_I2C0_TMP_BAR0 + (LPSS_I2C_TMP_BAR0_DELTA*7) + LPSS_I2C_TMP_BAR1_OFFSET}
};
#define I2C_LPSS_PCI_DEVICE_NUMBER  (sizeof (mI2cLpssPciDeviceList) / sizeof (I2C_LPSS_PCI_DEVICE_INFO))

//
// List of I2C controller clock values
//
LPSS_I2C_CLOCK_SCL_INFO  mLPSS_I2C_CLOCK_SCL_INFO[] = {
  {0x244, 0x2D0, 0x64, 0xC8, 0x06, 0x13},
  {0x244, 0x2D0, 0x64, 0xC8, 0x06, 0x13},
  {0x244, 0x2D0, 0x64, 0xC8, 0x06, 0x13},
  {0x244, 0x2DA, 0x1E, 0x3C, 0x06, 0x13},
  {0x244, 0x2DA, 0x1E, 0x50, 0x06, 0x13},
  {0x244, 0x2D0, 0x69, 0xC8, 0x06, 0x13},
  {0x244, 0x2D0, 0x69, 0xC8, 0x06, 0x13},
  {0x244, 0x2D0, 0x70, 0xC8, 0x06, 0x13}
};
#define LPSS_I2C_CLOCK_SCL_INFO_NUMBER  (sizeof (mLPSS_I2C_CLOCK_SCL_INFO) / sizeof (LPSS_I2C_CLOCK_SCL_INFO))

//
// List of I2C controller PAD settings
//
BXT_GPIO_PAD_INIT  mI2C_LPSS_PAD_INFO[] = {
  BXT_GPIO_PAD_CONF (L"GPIO_124 LPSS_I2C0_SDA", M1, NA, NA, NA, NA, Wake_Disabled, P_20K_H, NA, NA, D1RxDRx1I, EnPu, GPIO_PADBAR + 0x0000, WEST),
  BXT_GPIO_PAD_CONF (L"GPIO_125 LPSS_I2C0_SCL", M1, NA, NA, NA, NA, Wake_Disabled, P_20K_H, NA, NA, D1RxDRx1I, EnPu, GPIO_PADBAR + 0x0008, WEST),
  BXT_GPIO_PAD_CONF (L"GPIO_126 LPSS_I2C1_SDA", M1, NA, NA, NA, NA, Wake_Disabled, P_20K_H, NA, NA, D1RxDRx1I, EnPu, GPIO_PADBAR + 0x0010, WEST),
  BXT_GPIO_PAD_CONF (L"GPIO_127 LPSS_I2C1_SCL", M1, NA, NA, NA, NA, Wake_Disabled, P_20K_H, NA, NA, D1RxDRx1I, EnPu, GPIO_PADBAR + 0x0018, WEST),
  BXT_GPIO_PAD_CONF (L"GPIO_128 LPSS_I2C2_SDA", M1, NA, NA, NA, NA, Wake_Disabled, P_20K_H, NA, NA, D1RxDRx1I, EnPu, GPIO_PADBAR + 0x0020, WEST),
  BXT_GPIO_PAD_CONF (L"GPIO_129 LPSS_I2C2_SCL", M1, NA, NA, NA, NA, Wake_Disabled, P_20K_H, NA, NA, D1RxDRx1I, EnPu, GPIO_PADBAR + 0x0028, WEST),
  BXT_GPIO_PAD_CONF (L"GPIO_130 LPSS_I2C3_SDA", M1, NA, NA, NA, NA, Wake_Disabled, P_20K_H, NA, NA, D1RxDRx1I, EnPu, GPIO_PADBAR + 0x0030, WEST),
  BXT_GPIO_PAD_CONF (L"GPIO_131 LPSS_I2C3_SCL", M1, NA, NA, NA, NA, Wake_Disabled, P_20K_H, NA, NA, D1RxDRx1I, EnPu, GPIO_PADBAR + 0x0038, WEST),
  BXT_GPIO_PAD_CONF (L"GPIO_132 LPSS_I2C4_SDA", M1, NA, NA, NA, NA, Wake_Disabled, P_20K_H, NA, NA, D1RxDRx1I, EnPu, GPIO_PADBAR + 0x0040, WEST),
  BXT_GPIO_PAD_CONF (L"GPIO_133 LPSS_I2C4_SCL", M1, NA, NA, NA, NA, Wake_Disabled, P_20K_H, NA, NA, D1RxDRx1I, EnPu, GPIO_PADBAR + 0x0048, WEST),
  BXT_GPIO_PAD_CONF (L"GPIO_134 LPSS_I2C5_SDA", M1, NA, NA, NA, NA, Wake_Disabled, P_20K_H, NA, NA, D1RxDRx1I, EnPu, GPIO_PADBAR + 0x0050, WEST),
  BXT_GPIO_PAD_CONF (L"GPIO_135 LPSS_I2C5_SCL", M1, NA, NA, NA, NA, Wake_Disabled, P_20K_H, NA, NA, D1RxDRx1I, EnPu, GPIO_PADBAR + 0x0058, WEST),
  BXT_GPIO_PAD_CONF (L"GPIO_136 LPSS_I2C6_SDA", M1, NA, NA, NA, NA, Wake_Disabled, P_20K_H, NA, NA, D1RxDRx1I, EnPu, GPIO_PADBAR + 0x0060, WEST),
  BXT_GPIO_PAD_CONF (L"GPIO_137 LPSS_I2C6_SCL", M1, NA, NA, NA, NA, Wake_Disabled, P_20K_H, NA, NA, D1RxDRx1I, EnPu, GPIO_PADBAR + 0x0068, WEST),
  BXT_GPIO_PAD_CONF (L"GPIO_138 LPSS_I2C7_SDA", M1, NA, NA, NA, NA, Wake_Disabled, P_20K_H, NA, NA, D0RxDRx0I, EnPu, GPIO_PADBAR + 0x0070, WEST),
  BXT_GPIO_PAD_CONF (L"GPIO_139 LPSS_I2C7_SCL", M1, NA, NA, NA, NA, Wake_Disabled, P_20K_H, NA, NA, D0RxDRx0I, EnPu, GPIO_PADBAR + 0x0078, WEST)
};

BOOLEAN   gI2cDebugFlag = FALSE;

////
//// Internal I2C functions
////
//
//  Desc:   Clears the interrupts on this I2C controller
//  Input:  I2cBaseAddress        - Pointer to the MMIO base address for the I2C controller
//  Output: NA
//
VOID
I2cClearInterrupts (
  IN UINT32       I2cBaseAddress
  )
{
  MmioRead32 (I2cBaseAddress + R_IC_CLR_INTR);
  return;
}

//
//  Desc:   Clears the TX Abort on this I2C controller
//  Input:  I2cBaseAddress        - Pointer to the MMIO base address for the I2C controller
//  Output: NA
//
VOID
I2cClearTxAbort (
  IN UINT32       I2cBaseAddress
  )
{
  MmioRead32 (I2cBaseAddress + R_IC_CLR_TX_ABRT);
  return;
}

//
//  Desc:   Disable this I2C controller
//  Input:  I2cBaseAddress        - Pointer to the MMIO base address for the I2C controller
//  Output: Status                - EFI_SUCCESS   - I2C host controller is completely inactive
//                                  EFI_NOT_READY - I2C host controller is still in an enabled state
//
EFI_STATUS
I2cDisable (
  IN UINT32       I2cBaseAddress
  )
{
  UINT32       NumTries;
  EFI_STATUS   Status;

  //
  // Initialize variables
  //
  Status = EFI_SUCCESS;

  //
  // Disable I2C controller
  //
  MmioWrite32 (I2cBaseAddress + R_IC_ENABLE, 0);
  NumTries = 10000;   // 0.1 seconds
  while (0 != (MmioRead32 (I2cBaseAddress + R_IC_ENABLE_STATUS) & 0x03)) {
    MicroSecondDelay (10);
    NumTries --;
    if (0 == NumTries) {
      Status = EFI_NOT_READY;
      goto Exit;
    }
  }

Exit:
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - Ending with %r\n", __FUNCTION__, __LINE__, Status));
  }
  return Status;
}

/**
  Enable I2C host controller

  @param[in]  I2CBaseAddress    - BAR0 address of I2C host controller

  @retval EFI_SUCCESS           - I2C host controller is in an enabled state.
  @retval EFI_NOT_READY         - I2C host controller is still inactive.
**/
EFI_STATUS
I2cEnable (
  IN UINT32       I2cBaseAddress
  )
{
  UINT32       NumTries;
  EFI_STATUS   Status;

  //
  // Initialize variables
  //
  NumTries = 10000;   // 0.1 seconds
  Status   = EFI_SUCCESS;

  //
  // Enable I2C controller
  //
  MmioWrite32 (I2cBaseAddress + R_IC_ENABLE, I2C_ENABLE_ENABLE);
  while (I2C_ENABLE_ENABLE != (MmioRead32 (I2cBaseAddress + R_IC_ENABLE_STATUS) & I2C_ENABLE_ENABLE)) {
    MicroSecondDelay (10);
    NumTries --;
    if (0 == NumTries) {
      Status = EFI_NOT_READY;
      goto Exit;
    }
  }

Exit:
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - Ending with %r\n", __FUNCTION__, __LINE__, Status));
  }
  return Status;
}

UINT16
I2cGetTxAbortStatus (
  IN UINT32       I2cBaseAddress
  )
{
  UINT16   TxAbortStatus;

  if (I2cBaseAddress == 0) {
    TxAbortStatus = 0xFFFF;
  } else {
    TxAbortStatus = (UINT16) (MmioRead32 (I2cBaseAddress + R_IC_TX_ABRT_SOURCE) & 0xFFFF);
  }
  return TxAbortStatus;
}

/**
  Get I2C controller raw interrupt status

  @param[in]  I2CBaseAddress    - BAR0 address of I2C host controller

  @retval UINT16                - Raw interrupt status bit flags
**/
UINT16
I2cGetRawStatus (
  IN UINT32       I2cBaseAddress
  )
{
  UINT16   RawStatus;

  if (I2cBaseAddress == 0) {
    RawStatus = 0xFFFF;
  } else {
    RawStatus = (UINT16) (MmioRead32 (I2cBaseAddress + R_IC_RAW_INTR_STAT) & 0x3FFF);
  }

  return RawStatus;
}

/**
  Get I2C controller RX FIFO count

  @param[in]  I2CBaseAddress    - BAR0 address of I2C host controller

  @retval UINT16                - RX FIFO count
**/
UINT16
I2cGetRxFifo (
  IN UINT32       I2cBaseAddress
  )
{
  UINT16   RxFifo;

  if (I2cBaseAddress == 0) {
    RxFifo = 0xFFFF;
  } else {
    RxFifo = (UINT16) (MmioRead32 (I2cBaseAddress + R_IC_RXFLR) & 0x01FF);
  }

  return RxFifo;
}

/**
  Get I2C controller status

  @param[in]  I2CBaseAddress    - BAR0 address of I2C host controller

  @retval UINT16                - Status bit flags
**/
UINT16
I2cGetStatus (
  IN UINT32       I2cBaseAddress
  )
{
  UINT16   I2cStatus;

  if (I2cBaseAddress == 0) {
    I2cStatus = 0xFFFF;
  } else {
    I2cStatus = (UINT16) (MmioRead32 (I2cBaseAddress + R_IC_STATUS) & 0x007F);
  }

  return I2cStatus;
}

/**
  Get I2C controller TX FIFO count

  @param[in]  I2CBaseAddress    - BAR0 address of I2C host controller

  @retval UINT16                - TX FIFO count
**/
UINT16
I2cGetTxFifo (
  IN UINT32       I2cBaseAddress
  )
{
  UINT16   TxFifo;

  if (I2cBaseAddress == 0) {
    TxFifo = 0xFFFF;
  } else {
    TxFifo = (UINT16) (MmioRead32 (I2cBaseAddress + R_IC_TXFLR) & 0x01FF);
  }

  return TxFifo;
}

EFI_STATUS
I2cProgramPad (
  IN   UINT8   Bus
  )
{
  UINT8        index;
  EFI_STATUS   Status;

  if (gI2cDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Programming PADs for bus #%d\n", __FUNCTION__, __LINE__, Bus));

  //
  // Initialize variables
  //
  Status = EFI_SUCCESS;

  //
  // Sanity checks
  //
  if (Bus > MAX_I2C_BUS) {
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  //
  // Program SDA/SCL
  //
  for (index = 0; index < 2; index++) {
    if (gI2cDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Programming PAD %s\n", __FUNCTION__, __LINE__, mI2C_LPSS_PAD_INFO[(Bus * 2) + index].pad_name));
    GpioPadConfigTable (1, &mI2C_LPSS_PAD_INFO[(Bus * 2) + index]);
  }

  //
  // Pause a bit
  //
  MicroSecondDelay (I2C_ROUTINE_DELAY);

Exit:
  return Status;
}

/**
  Set the I2C controller bus clock frequency.

  The software and controller do a best case effort of using the specified
  frequency for the I2C bus.  If the frequency does not match exactly then
  the controller will use a slightly lower frequency for the I2C to avoid
  exceeding the operating conditions for any of the I2C devices on the bus.
  For example if 400 KHz was specified and the controller's divide network
  only supports 402 KHz or 398 KHz then the controller would be set to 398
  KHz.  However if the desired frequency is 400 KHz and the controller only
  supports 1 MHz and 100 KHz then this routine would return EFI_UNSUPPORTED.

  @param[in]  Bus               - I2C Bus number to which the I2C device has been connected
  @param[in]  I2CBaseAddress    - BAR0 address of I2C host controller

  @retval EFI_SUCCESS           - The bus frequency was set successfully.
**/
EFI_STATUS
I2cSetBusFrequency (
  IN  UINT8    Bus,
  IN  UINT32   I2cBaseAddress
  )
{
  EFI_STATUS   Status;

  //
  // Initialize variables
  //
  Status = EFI_SUCCESS;

  if (gI2cDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - I2cBusFrequencySet bus: %d\n", __FUNCTION__, __LINE__, Bus));
  ASSERT ((Bus < LPSS_I2C_CLOCK_SCL_INFO_NUMBER));
  //
  //  Set the 100 KHz clock divider according to SV result and I2C spec
  //
  MmioWrite32 (I2cBaseAddress + R_IC_SS_SCL_HCNT, (UINT16) mLPSS_I2C_CLOCK_SCL_INFO[Bus].SS_SCL_HCNT);
  MmioWrite32 (I2cBaseAddress + R_IC_SS_SCL_LCNT, (UINT16) mLPSS_I2C_CLOCK_SCL_INFO[Bus].SS_SCL_LCNT);
  if (gI2cDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - I2cBusFrequencySet R_IC_SS_SCL_HCNT: 0x%08X, R_IC_SS_SCL_LCNT: 0x%08X\r\n",
                       __FUNCTION__,
                       __LINE__,
                       MmioRead32 (I2cBaseAddress + R_IC_SS_SCL_HCNT),
                       MmioRead32 (I2cBaseAddress + R_IC_SS_SCL_LCNT)));
  //
  //  Set the 400 KHz clock divider according to SV result and I2C spec
  //
  MmioWrite32 (I2cBaseAddress + R_IC_FS_SCL_HCNT, (UINT16) mLPSS_I2C_CLOCK_SCL_INFO[Bus].FS_SCL_HCNT);
  MmioWrite32 (I2cBaseAddress + R_IC_FS_SCL_LCNT, (UINT16) mLPSS_I2C_CLOCK_SCL_INFO[Bus].FS_SCL_LCNT);
  if (gI2cDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - I2cBusFrequencySet R_IC_FS_SCL_HCNT: 0x%08X, R_IC_FS_SCL_LCNT: 0x%08X\r\n",
                       __FUNCTION__,
                       __LINE__,
                       MmioRead32 (I2cBaseAddress + R_IC_FS_SCL_HCNT),
                       MmioRead32 (I2cBaseAddress + R_IC_FS_SCL_LCNT)));
  //
  //  Set the 3.4MHz clock divider according to SV result and I2C spec
  //
  MmioWrite32 (I2cBaseAddress + R_IC_HS_SCL_HCNT, (UINT16)mLPSS_I2C_CLOCK_SCL_INFO[Bus].HS_SCL_HCNT);
  MmioWrite32 (I2cBaseAddress + R_IC_HS_SCL_LCNT, (UINT16)mLPSS_I2C_CLOCK_SCL_INFO[Bus].HS_SCL_LCNT);
  if (gI2cDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - I2cBusFrequencySet R_IC_HS_SCL_HCNT: 0x%08X, R_IC_HS_SCL_LCNT: 0x%08X\r\n",
                       __FUNCTION__,
                       __LINE__,
                       MmioRead32 (I2cBaseAddress + R_IC_HS_SCL_HCNT),
                       MmioRead32 (I2cBaseAddress + R_IC_HS_SCL_LCNT)));

  //
  // Set hold register
  //
  MmioWrite32 (I2cBaseAddress + R_IC_SDA_HOLD, (UINT16) 0x06);
  if (gI2cDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - I2cBusFrequencySet R_IC_SDA_HOLD: 0x%08X\r\n", __FUNCTION__, __LINE__, MmioRead32 (I2cBaseAddress + R_IC_SDA_HOLD)));

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - Ending with %r\n", __FUNCTION__, __LINE__, Status));
  }
  return Status;
}

/**
  Program LPSS I2C PCI controller's BAR0 and enable memory decode.

  @param[in]  Bus               - I2C Bus number to which the I2C device has been connected

  @retval EFI_SUCCESS           - I2C controller's BAR0 is programmed and memory decode enabled.
  @retval EFI_NOT_READY         - I2C controller's is not exist or its function has been disabled.
  @retval EFI_DEVICE_ERROR      - I2C controller can't be enabled.
**/
EFI_STATUS
ProgramPciLpssI2C (
  IN UINT8        Bus
  )
{
  UINT32       Data32;
  UINT32       I2CBar0;
  UINT32       I2CBar1;
  UINTN        PciMmBase;
  UINT32       PmcBase;
  EFI_STATUS   Status;
  UINT32       I2cPortDisable[] = {
    B_PMC_FUNC_DIS_LPSS_I2C0,
    B_PMC_FUNC_DIS_LPSS_I2C1,
    B_PMC_FUNC_DIS_LPSS_I2C2,
    B_PMC_FUNC_DIS_LPSS_I2C3,
    B_PMC_FUNC_DIS_LPSS_I2C4,
    B_PMC_FUNC_DIS_LPSS_I2C5,
    B_PMC_FUNC_DIS_LPSS_I2C6,
    B_PMC_FUNC_DIS_LPSS_I2C7
  };

  //
  // Initialize variables
  //
  PciMmBase = 0;
  Status    = EFI_SUCCESS;

  //
  // Set PADs to I2C mode
  //
  I2cProgramPad (Bus);

  //
  // Check PMC disable register
  //
  PmcBase = PMC_BASE_ADDRESS;
  Data32  = MmioRead32 (PmcBase + R_PMC_FUNC_DIS);

  if (Data32 == 0xFFFFFFFF) {
    if (gI2cDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - ProgramPciLpssI2C() PMC disable register not available. [%08x]\n", __FUNCTION__, __LINE__, PMC_BASE_ADDRESS));
  } else {
    if ((Data32 & I2cPortDisable[Bus]) != 0) {
      // This I2C port is disabled. Turn it on.
      Data32 &= ~I2cPortDisable[Bus];
      MmioWrite32 (PmcBase + R_PMC_FUNC_DIS, Data32);
      if (gI2cDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - ProgramPciLpssI2C() enable I2C controller #%x\n", __FUNCTION__, __LINE__, Bus));
      // Make sure it took.
      if (Data32 != MmioRead32 (PmcBase + R_PMC_FUNC_DIS)) {
        DEBUG ((DEBUG_ERROR, "%a (#%4d) - ProgramPciLpssI2C() failed to enable I2C controller #%x [%08x:%08x]\n",
            __FUNCTION__,
            __LINE__,
            Bus,
            Data32,
            MmioRead32 (PmcBase + R_PMC_FUNC_DIS)));
        Status = EFI_DEVICE_ERROR;
        goto Exit;
      }
    }
  }

  if (gI2cDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - ProgramPciLpssI2C()------------BusNo=%x\n", __FUNCTION__, __LINE__, Bus));

  PciMmBase = MmPciAddress (
                mI2cLpssPciDeviceList[Bus].Segment,
                mI2cLpssPciDeviceList[Bus].BusNum,
                mI2cLpssPciDeviceList[Bus].DeviceNum,
                mI2cLpssPciDeviceList[Bus].FunctionNum,
                0
                );
  if (gI2cDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Program Pci Lpss I2C Device  %x %x %x PciMmBase:%x\n",
                       __FUNCTION__,
                       __LINE__,
                       mI2cLpssPciDeviceList[Bus].BusNum,
                       mI2cLpssPciDeviceList[Bus].DeviceNum,
                       mI2cLpssPciDeviceList[Bus].FunctionNum, PciMmBase));

  //
  // Check if device present
  //
  if (MmioRead32 (PciMmBase) != 0xFFFFFFFF) {
    if ((MmioRead32 (PciMmBase + R_LPSS_IO_STSCMD) & B_LPSS_IO_STSCMD_MSE)) {
      //
      // In Pei stage, we always disable Bus master, and memory space enabling for BAR re-programming
      // In DXE stage, will read existing BAR value instead of re-programming
      //
      I2CBar0 = MmioRead32 (PciMmBase + R_LPSS_IO_BAR) & B_LPSS_IO_BAR_BA;
      I2CBar1 = MmioRead32 (PciMmBase + R_LPSS_IO_BAR1) & B_LPSS_IO_BAR_BA;
      if ((I2CBar0 != (UINT32) mI2cLpssPciDeviceList[Bus].Bar0) || (I2CBar1 != (UINT32) mI2cLpssPciDeviceList[Bus].Bar1)) {
        mI2cLpssPciDeviceList[Bus].Bar0 = MmioRead32 (PciMmBase + R_LPSS_IO_BAR) & B_LPSS_IO_BAR_BA;     // get the address allocated.
        mI2cLpssPciDeviceList[Bus].Bar1 = MmioRead32 (PciMmBase + R_LPSS_IO_BAR1) & B_LPSS_IO_BAR_BA;
        if (gI2cDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Get bar0:0x%x bar1:0x%x\n",
                             __FUNCTION__,
                             __LINE__,
                             mI2cLpssPciDeviceList[Bus].Bar0,
                             mI2cLpssPciDeviceList[Bus].Bar1));
      }
    } else {
      //
      // Program BAR 0
      //
      ASSERT (((mI2cLpssPciDeviceList[Bus].Bar0 & B_LPSS_IO_BAR_BA) == mI2cLpssPciDeviceList[Bus].Bar0) && (mI2cLpssPciDeviceList[Bus].Bar0 != 0));
      MmioWrite32 ((UINTN) (PciMmBase + R_LPSS_IO_BAR), (UINT32) (mI2cLpssPciDeviceList[Bus].Bar0 & B_LPSS_IO_BAR_BA));
      //
      // Program BAR 1
      //
      ASSERT (((mI2cLpssPciDeviceList[Bus].Bar1 & B_LPSS_IO_BAR1_BA) == mI2cLpssPciDeviceList[Bus].Bar1) && (mI2cLpssPciDeviceList[Bus].Bar1 != 0));
      MmioWrite32 ((UINTN) (PciMmBase + R_LPSS_IO_BAR1), (UINT32) (mI2cLpssPciDeviceList[Bus].Bar1 & B_LPSS_IO_BAR1_BA));
      //
      // Bus Master Enable & Memory Space Enable
      //
      MmioOr32 ((UINTN) (PciMmBase + R_LPSS_IO_STSCMD), (UINT32) (B_LPSS_IO_STSCMD_BME | B_LPSS_IO_STSCMD_MSE));
      ASSERT (MmioRead32 (mI2cLpssPciDeviceList[Bus].Bar0) != 0xFFFFFFFF);
    }

    //
    // Release Resets
    //
    MmioWrite32 (mI2cLpssPciDeviceList[Bus].Bar0 + R_LPSS_IO_MEM_RESETS, B_LPSS_IO_MEM_HC_RESET_REL | B_LPSS_IO_MEM_iDMA_RESET_REL);

    if (gI2cDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - ProgramPciLpssI2C() Programmed()\n", __FUNCTION__, __LINE__));
    Status = EFI_SUCCESS;
    goto Exit;
  } else {
     DEBUG ((DEBUG_ERROR, "%a (#%4d) - Pci Lpss I2C Device  %x %x %x does not exist!\n",
       __FUNCTION__,
       __LINE__,
       mI2cLpssPciDeviceList[Bus].BusNum,
       mI2cLpssPciDeviceList[Bus].DeviceNum,
       mI2cLpssPciDeviceList[Bus].FunctionNum));

     Status = EFI_NOT_READY;
     goto Exit;
  }

Exit:
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - Ending with %r\n", __FUNCTION__, __LINE__, Status));
  }
  return Status;
}

////
//// Public I2C functions
////
//
//  Desc:   Initializes the controller and returns the MMIO base address
//  Input:  Bus                   - I2C controller, 0 based
//          Address               - 7-bit slave address
//          Speed                 - Uses the I2C_SPEED_ENUM enum to set the controller speed
//          I2cBaseAddress        - Pointer to the MMIO base address for the I2C controller
//  Output: EFI_SUCCESS           - Initialization completed successfully
//          EFI_DEVICE_ERROR      - I2C controller error
//          EFI_INVALID_PARAMETER - Invalid input parameter
//
EFI_STATUS
I2cInit (
  IN       UINT8     Bus,
  IN       UINT16    Address,
  IN       UINT8     Speed,
  IN OUT   UINT32   *I2cBaseAddress
  )
{
  UINT32       BaseAddress;
  UINTN        PciMmBase;
  EFI_STATUS   Status;

  //
  //  Sanity checks
  //
  if (Bus > MAX_I2C_BUS) {
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  if (Address > MAX_I2C_ADDRESS) {
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  if (Speed > Max_Speed) {
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  if (I2cBaseAddress == NULL) {
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  //
  // Initialize variables
  //
  *I2cBaseAddress = 0;

  //
  // Get current MMIO base address
  //
  PciMmBase = MmPciAddress (
                mI2cLpssPciDeviceList[Bus].Segment,
                mI2cLpssPciDeviceList[Bus].BusNum,
                mI2cLpssPciDeviceList[Bus].DeviceNum,
                mI2cLpssPciDeviceList[Bus].FunctionNum,
                0
                );
  BaseAddress = MmioRead32 (PciMmBase + R_LPSS_IO_BAR) & B_LPSS_IO_BAR_BA;
  if (gI2cDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - I2CBaseAddress = 0x%08x:0x%08x \n", __FUNCTION__, __LINE__, BaseAddress, (UINT32) mI2cLpssPciDeviceList[Bus].Bar0));

  //
  // Skip reinit if targeting the same I2C bus
  //
  if (BaseAddress == (UINT32) mI2cLpssPciDeviceList[Bus].Bar0) {
    MmioWrite32 (BaseAddress + R_IC_TAR, Address);
    *I2cBaseAddress = BaseAddress;
    Status = EFI_SUCCESS;
    goto Exit;
  }

  //
  // Program I2C controller
  //
  Status = ProgramPciLpssI2C (Bus);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ProgramPciLpssI2C failed! %r\n", __FUNCTION__, __LINE__, Status));
    goto Exit;
  }

  //
  // Retrieve I2C MMIO base address
  //
  BaseAddress = (UINT32) mI2cLpssPciDeviceList[Bus].Bar0;
  if (gI2cDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - I2CBaseAddress = 0x%x \n", __FUNCTION__, __LINE__, BaseAddress));

  //
  // Reset controller
  //
  Status = I2cReset (BaseAddress, Bus, Address, Speed);
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  //
  // Pause a bit
  //
  MicroSecondDelay (I2C_ROUTINE_DELAY);

  //
  // Pass out MMIO base
  //
  *I2cBaseAddress = BaseAddress;
  Status = EFI_SUCCESS;

Exit:
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - Ending with %r\n", __FUNCTION__, __LINE__, Status));
  }
  return Status;
}

//
//  Desc:   Polls the I2C controller with reads until it responds.
//  Input:  I2cBaseAddress        - Pointer to the MMIO base address for the I2C controller
//  Output: EFI_SUCCESS           - Initialization completed successfully
//          EFI_DEVICE_ERROR      - I2C controller error
//
EFI_STATUS
I2cPoll (
  IN       UINT32     I2cBaseAddress
  )
{
  EFI_STATUS   Status;
  UINT16       Timeout;
  UINT8        Value;

  //
  // Use a read to poll the slave
  //
  Status  = EFI_DEVICE_ERROR;
  Timeout = 0;
  while (EFI_ERROR (Status) && Timeout < 1000) {
    MicroSecondDelay (10);
    Status = I2cRead (I2cBaseAddress, &Value, TRUE, TRUE);
    Timeout++;
  }

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - Ending with %r\n", __FUNCTION__, __LINE__, Status));
  }
  return Status;
}

//
//  Desc:   Read a byte from the I2C controller
//  Input:  I2cBaseAddress        - MMIO base address for the I2C controller
//          Data                  - Pointer to where to store the data
//          Start                 - Send start bit?
//          End                   - Send end bit?
//  Output: EFI_SUCCESS           - Read completed successfully
//          EFI_DEVICE_ERROR      - I2C controller error
//          EFI_INVALID_PARAMETER - Invalid input parameter
//
EFI_STATUS
I2cRead (
  IN       UINT32     I2cBaseAddress,
  IN OUT   UINT8     *Data,
  IN       BOOLEAN    Start,
  IN       BOOLEAN    End
  )
{
  UINT32      Data32;
  EFI_STATUS  Status;

  if (gI2cDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting\n", __FUNCTION__, __LINE__));

  //
  // Sanity checks
  //
  if (Data == NULL) {
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  //
  // Send CMD for read
  //
  Data32 = B_READ_CMD;
  Status = I2cSendCommand (I2cBaseAddress, &Data32, Start, End);
  *Data  = (UINT8) (Data32 & 0xFF);

Exit:
  //
  // Pause a bit
  //
  MicroSecondDelay (I2C_ROUTINE_DELAY);
  //
  // Display error messages
  //
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - Ending with %r\n", __FUNCTION__, __LINE__, Status));
  }
  return Status;
}

//
//  Desc:   Resets the I2C controller into a known good state
//  Input:  I2cBaseAddress        - MMIO base address for the I2C controller
//          Bus                   - I2C controller, 0 based
//          Address               - 7-bit slave address
//          Speed                 - Uses the I2C_SPEED_ENUM enum to set the controller speed
//  Output: EFI_SUCCESS           - Write completed successfully
//          EFI_DEVICE_ERROR      - I2C controller error
//          EFI_INVALID_PARAMETER - Invalid input parameter
//
EFI_STATUS
I2cReset (
  IN       UINT32    I2cBaseAddress,
  IN       UINT8     Bus,
  IN       UINT16    Address,
  IN       UINT8     Speed
  )
{
  UINT16       I2cMode;
  UINT32       NumTries;
  EFI_STATUS   Status;

  //
  // Wait for master activity to stop
  //
  NumTries = 10000; // 1 seconds
  while ((STAT_MST_ACTIVITY == (I2cGetStatus (I2cBaseAddress) & STAT_MST_ACTIVITY))) {
    MicroSecondDelay (10);
    NumTries--;
    if (0 == NumTries) {
      DEBUG ((DEBUG_ERROR, "%a(#%4d) - Try timeout\n", __FUNCTION__, __LINE__));
      Status = EFI_DEVICE_ERROR;
      goto Exit;
    }
  }
  //
  // Abort controller
  //
  MmioWrite32 (I2cBaseAddress + R_IC_ENABLE, I2C_ENABLE_ABORT);
  MicroSecondDelay (10 * I2C_ROUTINE_DELAY);
  //
  // Disable I2C controller
  //
  Status = I2cDisable (I2cBaseAddress);
  if (gI2cDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - I2cDisable Status = %r\n", __FUNCTION__, __LINE__, Status));
  if (EFI_ERROR (Status)) {
    goto Exit;
  }
  MicroSecondDelay (I2C_ROUTINE_DELAY);
  //
  // Set I2C controller speed
  //
  I2cSetBusFrequency (Bus, I2cBaseAddress); // Set I2cMode
  MicroSecondDelay (I2C_ROUTINE_DELAY);
  switch (Speed) {
    case Standard_Speed:
      //100K
      I2cMode = V_SPEED_STANDARD;
      break;

    case Fast_Speed:
      //400K
      I2cMode = V_SPEED_FAST;
      break;

    case High_Speed:
      //3.4M
      I2cMode = V_SPEED_HIGH;
      break;

    default:
      //400K
      I2cMode = V_SPEED_FAST;
  }
  I2cMode |= B_IC_RESTART_EN | B_IC_SLAVE_DISABLE | B_MASTER_MODE;
  //
  // Set slave address
  //
  MmioWrite32 (I2cBaseAddress + R_IC_INTR_MASK, 0x0);
  if (Address > MAX_I2C_ADDRESS) {
    Address = (Address & 0x3FF) | IC_TAR_10BITADDR_MASTER;
  }
  MmioWrite32 (I2cBaseAddress + R_IC_TAR, Address);
  MicroSecondDelay (I2C_ROUTINE_DELAY);
  //
  // Set RX & TX FIFO full threshold to 1 byte
  //
  MmioWrite32 (I2cBaseAddress + R_IC_RX_TL, 0);
  MmioWrite32 (I2cBaseAddress + R_IC_TX_TL, 0);
  MicroSecondDelay (I2C_ROUTINE_DELAY);
  //
  // Set I2C Mode
  //
  MmioWrite32 (I2cBaseAddress + R_IC_CON, I2cMode);
  if (gI2cDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - I2cMode: 0x%04x\r\n", __FUNCTION__, __LINE__, I2cMode));
  MicroSecondDelay (I2C_ROUTINE_DELAY);
  //
  // Enable I2C controller
  //
  Status = I2cEnable (I2cBaseAddress);
  if (gI2cDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - I2cEnable Status = %r\n", __FUNCTION__, __LINE__, Status));
  if (EFI_ERROR (Status)) {
    goto Exit;
  }
  MicroSecondDelay (I2C_ROUTINE_DELAY);
  //
  // Clear TX abort
  //
  I2cClearTxAbort (I2cBaseAddress);
  MicroSecondDelay (I2C_ROUTINE_DELAY);
  //
  // Clear interrupts
  //
  I2cClearInterrupts (I2cBaseAddress);
  MicroSecondDelay (I2C_ROUTINE_DELAY);

Exit:
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - Ending with %r\n", __FUNCTION__, __LINE__, Status));
  }
  return Status;
}

//
//  Desc:   Write a byte to the I2C controller
//  Input:  I2cBaseAddress        - MMIO base address for the I2C controller
//          Data                  - Data from the I2C controller
//          Start                 - Send start bit?
//          End                   - Send end bit?
//  Output: EFI_SUCCESS           - Write completed successfully
//          EFI_DEVICE_ERROR      - I2C controller error
//          EFI_INVALID_PARAMETER - Invalid input parameter
//
EFI_STATUS
I2cSendCommand (
  IN       UINT32     I2cBaseAddress,
  IN       UINT32    *Data,
  IN       BOOLEAN    Start,
  IN       BOOLEAN    End
  )
{
  BOOLEAN     CommandSent;
  UINT32      Count;
  UINT32      CountOut;
  UINT32      Data32;
  BOOLEAN     ReadFlag;
  EFI_STATUS  Status;
  UINT16      TxAbortStatus;

  //
  // Initialize variables
  //
  CommandSent = FALSE;
  Count       = 0;
  CountOut    = 0x00000100;
  Status      = EFI_NOT_READY;
  if ((*Data & B_READ_CMD) == B_READ_CMD) {
    ReadFlag = TRUE;
  } else {
    ReadFlag = FALSE;
  }

  //
  // Send a command byte
  //
  while (CountOut-- > 0) {
    //
    // Check for NACK
    //
    if ((I2cGetRawStatus (I2cBaseAddress) & I2C_INTR_TX_ABRT) != 0) {
      TxAbortStatus = I2cGetTxAbortStatus (I2cBaseAddress);
      DEBUG ((DEBUG_ERROR, "%a (#%4d) - TX ABRT [%04x]\n", __FUNCTION__, __LINE__, TxAbortStatus));
      if (gI2cDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - RX FIFO = %04x\n", __FUNCTION__, __LINE__, I2cGetRxFifo (I2cBaseAddress)));
      if (gI2cDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - TX FIFO = %04x\n", __FUNCTION__, __LINE__, I2cGetTxFifo (I2cBaseAddress)));
      //
      // Clear TX Abort
      //
      I2cClearTxAbort (I2cBaseAddress);
      MicroSecondDelay (I2C_WRITE_TIMEOUT);
      //
      // Clear interrupts
      //
      I2cClearInterrupts (I2cBaseAddress);
      MicroSecondDelay (I2C_WRITE_TIMEOUT);
      //
      // Set status
      //
      if (TxAbortStatus & (I2C_ABRT_7B_ADDR_NOACK | I2C_ABRT_10ADDR1_NOACK | I2C_ABRT_10ADDR2_NOACK)) {
        DEBUG ((DEBUG_ERROR, "%a(#%4d) - Nobody home!\n", __FUNCTION__, __LINE__));
        Status = EFI_NO_RESPONSE;
      } else {
        Status = EFI_DEVICE_ERROR;
      }
      goto Exit;
    }
    //
    // Determine if another byte was received and we were expecting it
    //
    if (((I2cGetStatus (I2cBaseAddress) & STAT_RFNE) != 0) && ReadFlag) {
      *Data = MmioRead32 (I2cBaseAddress + R_IC_DATA_CMD) & 0xFF;
      if (gI2cDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - MmioRead32, byte 0x%02x was received [%d:%d]\n", __FUNCTION__, __LINE__, *Data, Start, End));
      MicroSecondDelay (FIFO_WRITE_DELAY);
      //
      // Now empty the RX FIFO if stop bit set
      //
      while (End && ((I2cGetStatus (I2cBaseAddress) & STAT_RFNE) == STAT_RFNE)) {
        MmioRead32 (I2cBaseAddress + R_IC_DATA_CMD);
        MicroSecondDelay (FIFO_WRITE_DELAY);
      }
      if (gI2cDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - RX FIFO = %04x\n", __FUNCTION__, __LINE__, I2cGetRxFifo (I2cBaseAddress)));
      if (gI2cDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - TX FIFO = %04x\n", __FUNCTION__, __LINE__, I2cGetTxFifo (I2cBaseAddress)));
      Status = EFI_SUCCESS;
      goto Exit;
    }
    //
    // Wait for room in TX buffer
    //
    if ((I2cGetStatus (I2cBaseAddress) & STAT_TFNF) == 0) {
      MicroSecondDelay (FIFO_WRITE_DELAY);
      continue;
    }
    if (!CommandSent) {
      //
      // Send CMD
      //
      Data32 = *Data;
      if (Start) Data32 |= B_CMD_RESTART;
      if (End)   Data32 |= B_CMD_STOP;
      MmioWrite32 (I2cBaseAddress + R_IC_DATA_CMD, Data32);
      CommandSent = TRUE;
    }
    //
    // Add a small delay to work around some odd behavior being seen.  Without this delay bytes get dropped.
    //
    MicroSecondDelay (I2C_WRITE_TIMEOUT);
    //
    // Time out check for write CMD
    //
    while (!ReadFlag) {
      if ((I2cGetRawStatus (I2cBaseAddress) & I2C_INTR_TX_ABRT) != 0) {
        TxAbortStatus = I2cGetTxAbortStatus (I2cBaseAddress);
        DEBUG ((DEBUG_ERROR, "%a (#%4d) - TX ABRT [%04x]\n", __FUNCTION__, __LINE__, TxAbortStatus));
        //
        // Clear TX Abort
        //
        I2cClearTxAbort (I2cBaseAddress);
        MicroSecondDelay (I2C_WRITE_TIMEOUT);
        //
        // Clear interrupts
        //
        I2cClearInterrupts (I2cBaseAddress);
        MicroSecondDelay (I2C_WRITE_TIMEOUT);
        //
        // Set status
        //
        if (TxAbortStatus & (I2C_ABRT_7B_ADDR_NOACK | I2C_ABRT_10ADDR1_NOACK | I2C_ABRT_10ADDR2_NOACK)) {
          DEBUG ((DEBUG_ERROR, "%a(#%4d) - Nobody home!\n", __FUNCTION__, __LINE__));
          Status = EFI_NO_RESPONSE;
        } else {
          Status = EFI_DEVICE_ERROR;
        }
      }
      if (I2cGetTxFifo (I2cBaseAddress) == 0) {
        if (gI2cDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - MmioRead32, byte 0x%04x was sent [%d:%d]\n", __FUNCTION__, __LINE__, Data32, Start, End));
        if (gI2cDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - RX FIFO = %04x\n", __FUNCTION__, __LINE__, I2cGetRxFifo (I2cBaseAddress)));
        if (gI2cDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - TX FIFO = %04x\n", __FUNCTION__, __LINE__, I2cGetTxFifo (I2cBaseAddress)));
        Status = EFI_SUCCESS;
        goto Exit;
      }
      MicroSecondDelay (I2C_WRITE_TIMEOUT);
      if (Count++ < 1024) { //to avoid sys hung without ul-pmc device on RVP
        continue; //Waiting the last request to get data and make (ReceiveDataEnd > ReadBuffer) =TRUE.
      } else {
        DEBUG ((DEBUG_ERROR, "%a (#%4d) - hardware timeout, 1024 times try!\n", __FUNCTION__, __LINE__));
        Status = EFI_TIMEOUT;
        goto Exit;
      }
    }
  }

  //
  // Check for count out
  //
  if (CountOut == 0) {
    Status = EFI_TIMEOUT;
  }

Exit:
  //
  // Pause a bit
  //
  MicroSecondDelay (I2C_ROUTINE_DELAY);
  //
  // Display error messages
  //
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - Ending with %r   [%x]\n", __FUNCTION__, __LINE__, Status, CountOut));
  }
  return Status;
}

//
//  Desc:   Set I2C target slave offset
//  Input:  I2cBaseAddress        - MMIO base address for the I2C controller
//          Offset                - Pointer to offset data
//          Size                  - Size of the offset data
//  Output: EFI_SUCCESS           - Write completed successfully
//          EFI_DEVICE_ERROR      - I2C controller error
//          EFI_INVALID_PARAMETER - Invalid input parameter
//
EFI_STATUS
I2cSetOffset (
  IN       UINT32     I2cBaseAddress,
  IN       UINT8     *Offset,
  IN       UINT8      Size
  )
{
  UINT8        index;
  EFI_STATUS   Status;

  if (gI2cDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting\n", __FUNCTION__, __LINE__));

  //
  //  Sanity checks
  //
  if (Offset == NULL) {
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  if (Size == 0) {
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  //
  // Set offset
  //
  for (index = 0; index < Size; index++) {
    if (index == 0) {
      //
      // First byte of the offset
      //
      Status = I2cWrite (I2cBaseAddress, Offset[index], TRUE, FALSE);
    } else {
      Status = I2cWrite (I2cBaseAddress, Offset[index], FALSE, FALSE);
    }
    if (EFI_ERROR (Status)) {
      goto Exit;
    }
    //
    // Pause a bit
    //
    MicroSecondDelay (I2C_WRITE_TIMEOUT);
  }

Exit:
  //
  // Pause a bit
  //
  MicroSecondDelay (I2C_ROUTINE_DELAY);
  //
  // Display error messages
  //
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - Ending with %r\n", __FUNCTION__, __LINE__, Status));
  }
  return Status;
}

//
//  Desc:   Write a byte to the I2C controller
//  Input:  I2cBaseAddress        - MMIO base address for the I2C controller
//          Data                  - Data from the I2C controller
//          Start                 - Send start bit?
//          End                   - Send end bit?
//  Output: EFI_SUCCESS           - Write completed successfully
//          EFI_DEVICE_ERROR      - I2C controller error
//          EFI_INVALID_PARAMETER - Invalid input parameter
//
EFI_STATUS
I2cWrite (
  IN       UINT32     I2cBaseAddress,
  IN       UINT8      Data,
  IN       BOOLEAN    Start,
  IN       BOOLEAN    End
  )
{
  UINT32      Data32;
  EFI_STATUS  Status;

  if (gI2cDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting\n", __FUNCTION__, __LINE__));

  //
  // Send CMD for write
  //
  Data32 = Data;
  Status = I2cSendCommand (I2cBaseAddress, &Data32, Start, End);
  //
  // Pause a bit
  //
  MicroSecondDelay (I2C_ROUTINE_DELAY);
  //
  // Display error messages
  //
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - Ending with %r\n", __FUNCTION__, __LINE__, Status));
  }
  return Status;
}

/**
  Read bytes from I2C Device
  This is actual I2C hardware operation function.

  @param[in]  BusNo                 I2C Bus number to which the I2C device has been connected
  @param[in]  SlaveAddress          Slave address of the I2C device (7-bit)
  @param[in]  ReadBytes             Number of bytes to be read
  @param[out] ReadBuffer            Address to which the value read has to be stored
  @param[in]  Start                 It controls whether a RESTART is issued before the byte is sent or received.
  @param[in]  End                   It controls whether a STOP is issued after the byte is sent or received.

  @retval     EFI_SUCCESS           The byte value read successfully
  @retval     EFI_DEVICE_ERROR      Operation failed
  @retval     EFI_TIMEOUT           Hardware retry timeout
  @retval     Others                Failed to read a byte via I2C

**/
EFI_STATUS
ByteReadI2C_Basic (
  IN  UINT8        BusNo,
  IN  UINT8        SlaveAddress,
  IN  UINTN        ReadBytes,
  OUT UINT8        *ReadBuffer,
  IN  UINT8        Start,
  IN  UINT8        End
  )
{
  UINT32              I2cBaseAddress;
  UINTN               index;
  EFI_STATUS          Status;

  //
  // Init I2C controller
  //
  Status = I2cInit (BusNo, SlaveAddress, Standard_Speed, &I2cBaseAddress);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - I2cInit() = %r\n", __FUNCTION__, __LINE__, Status));
    goto Exit;
  }
  //
  // Read from I2C device
  //
  for (index = 0; index < ReadBytes; index++) {
    Status = I2cRead (I2cBaseAddress, &ReadBuffer[index], Start, End);
    if (EFI_ERROR (Status)) {
      //
      // Something went wrong. Bail from this for loop.
      //
      DEBUG ((DEBUG_ERROR, "%a (#%4d) - I2cRead() = %r\n", __FUNCTION__, __LINE__, Status));
      break;
    }
  }
Exit:
  return Status;
}

/**
  Write bytes to I2C Device
  This is actual I2C hardware operation function.

  @param[in]  BusNo                 I2C Bus number to which the I2C device has been connected
  @param[in]  SlaveAddress          Slave address of the I2C device (7-bit)
  @param[in]  WriteBytes            Number of bytes to be written
  @param[in]  WriteBuffer           Address to which the byte value has to be written
  @param[in]  Start                 It controls whether a RESTART is issued before the byte is sent or received.
  @param[in]  End                   It controls whether a STOP is issued after the byte is sent or received.

  @retval     EFI_SUCCESS           The byte value written successfully
  @retval     EFI_DEVICE_ERROR      Operation failed
  @retval     EFI_TIMEOUT           Hardware retry timeout
  @retval     Others                Failed to write a byte via I2C

**/
EFI_STATUS
ByteWriteI2C_Basic (
  IN  UINT8        BusNo,
  IN  UINT8        SlaveAddress,
  IN  UINTN        WriteBytes,
  IN  UINT8        *WriteBuffer,
  IN  UINT8        Start,
  IN  UINT8        End
  )
{
  UINT32              I2cBaseAddress;
  UINTN               index;
  EFI_STATUS          Status;

  //
  // Init I2C controller
  //
  Status = I2cInit (BusNo, SlaveAddress, Standard_Speed, &I2cBaseAddress);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - I2cInit() = %r\n", __FUNCTION__, __LINE__, Status));
    goto Exit;
  }
  //
  // Write to I2C device
  //
  for (index = 0; index < WriteBytes; index++) {
    Status = I2cWrite (I2cBaseAddress, WriteBuffer[index], Start, End);
    if (EFI_ERROR (Status)) {
      //
      // Something went wrong. Bail from this for loop.
      //
      DEBUG ((DEBUG_ERROR, "%a (#%4d) - I2cWrite() = %r\n", __FUNCTION__, __LINE__, Status));
      break;
    }
  }
Exit:
  return Status;
}

/**
  Read bytes from I2C Device

  @param[in]  BusNo               I2C Bus number to which the I2C device has been connected
  @param[in]  SlaveAddress        Slave address of the I2C device (7-bit)
  @param[in]  Offset              Register offset from which the data has to be read
  @param[in]  ReadBytes           Number of bytes to be read
  @param[out] ReadBuffer          Address to which the value read has to be stored

  @retval     EFI_SUCCESS         Read bytes from I2C device successfully
  @retval     Others              Return status depends on ByteReadI2C_Basic

**/
EFI_STATUS
ByteReadI2C (
  IN  UINT8        BusNo,
  IN  UINT8        SlaveAddress,
  IN  UINT8        Offset,
  IN  UINTN        ReadBytes,
  OUT UINT8        *ReadBuffer
  )
{
  UINT32              I2cBaseAddress;
  EFI_STATUS          Status;

  //
  // Init I2C controller
  //
  Status = I2cInit (BusNo, SlaveAddress, Standard_Speed, &I2cBaseAddress);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - I2cInit() = %r\n", __FUNCTION__, __LINE__, Status));
    goto Exit;
  }
  //
  // Set offset
  //
  Status = I2cSetOffset (I2cBaseAddress, &Offset, 1);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - I2cSetOffset() = %r [%02x:%02x:%04x]\n", __FUNCTION__, __LINE__, Status, BusNo, SlaveAddress, Offset));
    goto Exit;
  }
  Status = ByteReadI2C_Basic (BusNo, SlaveAddress, ReadBytes, ReadBuffer, TRUE, TRUE);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ByteReadI2C_Basic() = %r\n", __FUNCTION__, __LINE__, Status));
    goto Exit;
  }

Exit:
  return Status;
}

/**
  Write bytes to I2C Device

  @param[in]  BusNo               I2C Bus number to which the I2C device has been connected
  @param[in]  SlaveAddress        Slave address of the I2C device (7-bit)
  @param[in]  Offset              Register offset from which the data has to be read
  @param[in]  WriteBytes          Number of bytes to be written
  @param[in]  WriteBuffer         Address to which the byte value has to be written

  @retval     EFI_SUCCESS         Write bytes to I2C device successfully
  @retval     Others              Return status depends on ByteWriteI2C_Basic

**/
EFI_STATUS
ByteWriteI2C (
  IN  UINT8        BusNo,
  IN  UINT8        SlaveAddress,
  IN  UINT8        Offset,
  IN  UINTN        WriteBytes,
  IN  UINT8        *WriteBuffer
  )
{
  UINT32              I2cBaseAddress;
  EFI_STATUS          Status;

  //
  // Init I2C controller
  //
  Status = I2cInit (BusNo, SlaveAddress, Standard_Speed, &I2cBaseAddress);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - I2cInit() = %r\n", __FUNCTION__, __LINE__, Status));
    goto Exit;
  }
  //
  // Set offset
  //
  Status = I2cSetOffset (I2cBaseAddress, &Offset, 1);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - I2cSetOffset() = %r [%02x:%02x:%04x]\n", __FUNCTION__, __LINE__, Status, BusNo, SlaveAddress, Offset));
    goto Exit;
  }
  Status = ByteWriteI2C_Basic (BusNo, SlaveAddress, WriteBytes, WriteBuffer, FALSE, TRUE);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ByteWriteI2C_Basic() = %r\n", __FUNCTION__, __LINE__, Status));
    goto Exit;
  }

Exit:
  return Status;
}

