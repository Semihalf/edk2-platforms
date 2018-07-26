/** @file
  Dxe library for I2C bus driver.

@copyright
 Copyright (c) 1999 - 2018 Intel Corporation. All rights reserved

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#include <Library/DebugLib.h>
#include <Library/I2CLib.h>
#include <Library/TimerLib.h>
#include <PlatformBaseAddresses.h>
#include <SaAccess.h>
#include <ScAccess.h>
#include <ScRegs/RegsI2c.h>

#pragma pack(push, 1)
typedef struct _LPSS_PCI_DEVICE_INFO {
  UINTN        Segment;
  UINTN        BusNum;
  UINTN        DeviceNum;
  UINTN        FunctionNum;
  UINTN        Bar0;
  UINTN        Bar1;
} LPSS_PCI_DEVICE_INFO;

typedef enum {
  Standard_Speed = 1,
  Fast_Speed = 2,
  High_Speed = 3,
} I2C_SPEED;

typedef struct _LPSS_I2C_CLOCK_SCL_INFO {
  UINT8        I2c_Speed;
  UINT16       SS_SCL_HCNT;
  UINT16       SS_SCL_LCNT;
  UINT16       FS_SCL_HCNT;
  UINT16       FS_SCL_LCNT;
  UINT16       HS_SCL_HCNT;
  UINT16       HS_SCL_LCNT;
} LPSS_I2C_CLOCK_SCL_INFO;
#pragma pack(pop)

LPSS_PCI_DEVICE_INFO  mLpssPciDeviceList[] = {
  {0, DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_LPSS_I2C0,   PCI_FUNCTION_NUMBER_LPSS_I2C0, LPSS_I2C0_TMP_BAR0 + (LPSS_I2C_TMP_BAR0_DELTA*0), LPSS_I2C0_TMP_BAR0 + (LPSS_I2C_TMP_BAR0_DELTA*0) + LPSS_I2C_TMP_BAR1_OFFSET},
  {0, DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_LPSS_I2C0,   PCI_FUNCTION_NUMBER_LPSS_I2C1, LPSS_I2C0_TMP_BAR0 + (LPSS_I2C_TMP_BAR0_DELTA*1), LPSS_I2C0_TMP_BAR0 + (LPSS_I2C_TMP_BAR0_DELTA*1) + LPSS_I2C_TMP_BAR1_OFFSET},
  {0, DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_LPSS_I2C0,   PCI_FUNCTION_NUMBER_LPSS_I2C2, LPSS_I2C0_TMP_BAR0 + (LPSS_I2C_TMP_BAR0_DELTA*2), LPSS_I2C0_TMP_BAR0 + (LPSS_I2C_TMP_BAR0_DELTA*2) + LPSS_I2C_TMP_BAR1_OFFSET},
  {0, DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_LPSS_I2C0,   PCI_FUNCTION_NUMBER_LPSS_I2C3, LPSS_I2C0_TMP_BAR0 + (LPSS_I2C_TMP_BAR0_DELTA*3), LPSS_I2C0_TMP_BAR0 + (LPSS_I2C_TMP_BAR0_DELTA*3) + LPSS_I2C_TMP_BAR1_OFFSET},
  {0, DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_LPSS_I2C1,   PCI_FUNCTION_NUMBER_LPSS_I2C4, LPSS_I2C0_TMP_BAR0 + (LPSS_I2C_TMP_BAR0_DELTA*4), LPSS_I2C0_TMP_BAR0 + (LPSS_I2C_TMP_BAR0_DELTA*4) + LPSS_I2C_TMP_BAR1_OFFSET},
  {0, DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_LPSS_I2C1,   PCI_FUNCTION_NUMBER_LPSS_I2C5, LPSS_I2C0_TMP_BAR0 + (LPSS_I2C_TMP_BAR0_DELTA*5), LPSS_I2C0_TMP_BAR0 + (LPSS_I2C_TMP_BAR0_DELTA*5) + LPSS_I2C_TMP_BAR1_OFFSET},
  {0, DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_LPSS_I2C1,   PCI_FUNCTION_NUMBER_LPSS_I2C6, LPSS_I2C0_TMP_BAR0 + (LPSS_I2C_TMP_BAR0_DELTA*6), LPSS_I2C0_TMP_BAR0 + (LPSS_I2C_TMP_BAR0_DELTA*6) + LPSS_I2C_TMP_BAR1_OFFSET},
  {0, DEFAULT_PCI_BUS_NUMBER_SC, PCI_DEVICE_NUMBER_LPSS_I2C1,   PCI_FUNCTION_NUMBER_LPSS_I2C7, LPSS_I2C0_TMP_BAR0 + (LPSS_I2C_TMP_BAR0_DELTA*7), LPSS_I2C0_TMP_BAR0 + (LPSS_I2C_TMP_BAR0_DELTA*7) + LPSS_I2C_TMP_BAR1_OFFSET},
};

#define LPSS_PCI_DEVICE_NUMBER  sizeof(mLpssPciDeviceList)/sizeof(LPSS_PCI_DEVICE_INFO)

LPSS_I2C_CLOCK_SCL_INFO  mLPSS_I2C_CLOCK_SCL_INFO[] = {
  {Fast_Speed, 0x244, 0x2D0, 0x64, 0xC8, 0x06, 0x13},
  {Fast_Speed, 0x244, 0x2D0, 0x64, 0xC8, 0x06, 0x13},
  {Fast_Speed, 0x244, 0x2D0, 0x64, 0xC8, 0x06, 0x13},
  {High_Speed, 0x244, 0x2DA, 0x1E, 0x3C, 0x06, 0x13},
  {High_Speed, 0x244, 0x2DA, 0x1E, 0x50, 0x06, 0x13},
  {Fast_Speed, 0x244, 0x2D0, 0x69, 0xC8, 0x06, 0x13},
  {Fast_Speed, 0x244, 0x2D0, 0x69, 0xC8, 0x06, 0x13},
  {Fast_Speed, 0x244, 0x2D0, 0x70, 0xC8, 0x06, 0x13},
};

#define LPSS_I2C_CLOCK_SCL_INFO_NUMBER  sizeof(mLPSS_I2C_CLOCK_SCL_INFO)/sizeof(LPSS_I2C_CLOCK_SCL_INFO)


/**
  Program LPSS I2C PCI controller's BAR0 and enable memory decode.

  @param[in]  BusNo             - I2C Bus number to which the I2C device has been connected

  @retval EFI_SUCCESS           - I2C controller's BAR0 is programmed and memory decode enabled.
  @retval EFI_NOT_READY         - I2C controller's is not exist or its function has been disabled.
**/
EFI_STATUS
ProgramPciLpssI2C (
  IN UINT8        BusNo
  )
{
  UINTN  PciMmBase=0;
  UINT32 I2CBar0;
  UINT32 I2CBar1;
  UINT32 PmcBase;
  UINT32 D32;
  UINT32 I2cPortDisable[] = {
    B_PMC_FUNC_DIS_LPSS_I2C0,
    B_PMC_FUNC_DIS_LPSS_I2C1,
    B_PMC_FUNC_DIS_LPSS_I2C2,
    B_PMC_FUNC_DIS_LPSS_I2C3,
    B_PMC_FUNC_DIS_LPSS_I2C4,
    B_PMC_FUNC_DIS_LPSS_I2C5,
    B_PMC_FUNC_DIS_LPSS_I2C6,
    B_PMC_FUNC_DIS_LPSS_I2C7
  };

  DEBUG ((DEBUG_INFO, "ProgramPciLpssI2C() Start\n"));

  //
  // Check PMC disable register
  //
  PmcBase = PMC_BASE_ADDRESS;
  D32     = MmioRead32 (PmcBase + R_PMC_FUNC_DIS);

  if (D32 == 0xFFFFFFFF) {
    DEBUG ((DEBUG_INFO, "ProgramPciLpssI2C() PMC disable register not available. [%08x]\n", PMC_BASE_ADDRESS));
  } else {
    if ((D32 & I2cPortDisable[BusNo]) != 0) {
      // This I2C port is disabled. Turn it on.
      D32 &= ~I2cPortDisable[BusNo];
      MmioWrite32 (PmcBase + R_PMC_FUNC_DIS, D32);
      DEBUG ((DEBUG_INFO, "ProgramPciLpssI2C() enable I2C controller #%x\n", BusNo));
      // Make sure it took.
      if (D32 != MmioRead32 (PmcBase + R_PMC_FUNC_DIS)) {
        DEBUG ((DEBUG_ERROR, "ProgramPciLpssI2C() failed to enable I2C controller #%x [%08x:%08x]\n", BusNo, D32, MmioRead32 (PmcBase + R_PMC_FUNC_DIS)));
        return EFI_DEVICE_ERROR;
      }
    }
  }

  DEBUG ((DEBUG_INFO, "ProgramPciLpssI2C()------------BusNo=%x\n", BusNo));

  PciMmBase = MmPciAddress (
                mLpssPciDeviceList[BusNo].Segment,
                mLpssPciDeviceList[BusNo].BusNum,
                mLpssPciDeviceList[BusNo].DeviceNum,
                mLpssPciDeviceList[BusNo].FunctionNum,
                0
                );
  DEBUG ((DEBUG_INFO, "Program Pci Lpss I2C Device  %x %x %x PciMmBase:%x\n", \
      mLpssPciDeviceList[BusNo].BusNum, \
      mLpssPciDeviceList[BusNo].DeviceNum, \
      mLpssPciDeviceList[BusNo].FunctionNum, PciMmBase));

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
      if ((I2CBar0 != (UINT32)mLpssPciDeviceList[BusNo].Bar0) || (I2CBar1 != (UINT32)mLpssPciDeviceList[BusNo].Bar1)) {
        mLpssPciDeviceList[BusNo].Bar0 = MmioRead32 (PciMmBase + R_LPSS_IO_BAR) & B_LPSS_IO_BAR_BA;     // get the address allocated.
        mLpssPciDeviceList[BusNo].Bar1 = MmioRead32 (PciMmBase + R_LPSS_IO_BAR1) & B_LPSS_IO_BAR_BA;
        DEBUG ((DEBUG_INFO, "Get bar0:0x%x bar1:0x%x\n", mLpssPciDeviceList[BusNo].Bar0, mLpssPciDeviceList[BusNo].Bar1));
      }
    } else {
      //
      // Program BAR 0
      //
      ASSERT (((mLpssPciDeviceList[BusNo].Bar0 & B_LPSS_IO_BAR_BA) == mLpssPciDeviceList[BusNo].Bar0) && (mLpssPciDeviceList[BusNo].Bar0 != 0));
      MmioWrite32 ((UINTN) (PciMmBase + R_LPSS_IO_BAR), (UINT32) (mLpssPciDeviceList[BusNo].Bar0 & B_LPSS_IO_BAR_BA));
      //
      // Program BAR 1
      //
      ASSERT (((mLpssPciDeviceList[BusNo].Bar1 & B_LPSS_IO_BAR1_BA) == mLpssPciDeviceList[BusNo].Bar1) && (mLpssPciDeviceList[BusNo].Bar1 != 0));
      MmioWrite32 ((UINTN) (PciMmBase + R_LPSS_IO_BAR1), (UINT32) (mLpssPciDeviceList[BusNo].Bar1 & B_LPSS_IO_BAR1_BA));
      //
      // Bus Master Enable & Memory Space Enable
      //
      MmioOr32 ((UINTN) (PciMmBase + R_LPSS_IO_STSCMD), (UINT32) (B_LPSS_IO_STSCMD_BME | B_LPSS_IO_STSCMD_MSE));
      ASSERT (MmioRead32 (mLpssPciDeviceList[BusNo].Bar0) != 0xFFFFFFFF);
    }

    //
    // Release Resets
    //
    MmioWrite32 (mLpssPciDeviceList[BusNo].Bar0 + R_LPSS_IO_MEM_RESETS, B_LPSS_IO_MEM_HC_RESET_REL | B_LPSS_IO_MEM_iDMA_RESET_REL);

    DEBUG ((DEBUG_INFO, "ProgramPciLpssI2C() Programmed()\n"));
    return EFI_SUCCESS;
  } else {
     DEBUG ((DEBUG_ERROR, "Pci Lpss I2C Device  %x %x %x is not existing!\n",
       mLpssPciDeviceList[BusNo].BusNum,
       mLpssPciDeviceList[BusNo].DeviceNum,
       mLpssPciDeviceList[BusNo].FunctionNum));

     return EFI_NOT_READY;
  }
}

/**
  Disable I2C host controller

  @param[in]  I2CBaseAddress    - BAR0 address of I2C host controller

  @retval EFI_SUCCESS           - I2C host controller is completely inactive.
  @retval EFI_NOT_READY         - I2C host controller is still in an enabled state.
**/
EFI_STATUS
I2cDisable (
  IN UINTN        I2CBaseAddress
  )
{
  UINT32 NumTries = 10000;  // 0.1 seconds

  MmioWrite32 (I2CBaseAddress + R_IC_ENABLE, 0);
  while (0 != ( MmioRead32 ( I2CBaseAddress + R_IC_ENABLE_STATUS) & 1)) {
    MicroSecondDelay (10);
    NumTries --;
    if (0 == NumTries) return EFI_NOT_READY;
  }
  return EFI_SUCCESS;
}

/**
  Enable I2C host controller

  @param[in]  I2CBaseAddress    - BAR0 address of I2C host controller

  @retval EFI_SUCCESS           - I2C host controller is in an enabled state.
  @retval EFI_NOT_READY         - I2C host controller is still inactive.
**/
EFI_STATUS
I2cEnable (
  IN UINTN        I2CBaseAddress
  )
{
  UINT32 NumTries = 10000;  // 0.1 seconds

  MmioWrite32 (I2CBaseAddress + R_IC_ENABLE, 1);
  while (0 == (MmioRead32 (I2CBaseAddress + R_IC_ENABLE_STATUS) & 1)) {
    MicroSecondDelay (10);
    NumTries --;
    if (0 == NumTries) return EFI_NOT_READY;
  }
  return EFI_SUCCESS;
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

  @param[in]  BusNo             - I2C Bus number to which the I2C device has been connected
  @param[in]  I2CBaseAddress    - BAR0 address of I2C host controller
  @param[out] I2cMode           - I2C operation mode.
                                  Standard Speed: 100 KHz
                                  Fast Speed    : 400 KHz
                                  High Speed    : 3.4 MHz

  @retval EFI_SUCCESS           - The bus frequency was set successfully.
**/
EFI_STATUS
I2cBusFrequencySet (
  IN  UINT8   BusNo,
  IN  UINTN   I2CBaseAddress,
  OUT UINT16  *I2cMode
  )
{
  DEBUG ((DEBUG_INFO, "I2cBusFrequencySet bus: %d\r\n", BusNo));
  ASSERT ((BusNo < LPSS_I2C_CLOCK_SCL_INFO_NUMBER));
  //
  //  Set the 100 KHz clock divider according to SV result and I2C spec
  //
  MmioWrite32 (I2CBaseAddress + R_IC_SS_SCL_HCNT, (UINT16)mLPSS_I2C_CLOCK_SCL_INFO[BusNo].SS_SCL_HCNT);
  MmioWrite32 (I2CBaseAddress + R_IC_SS_SCL_LCNT, (UINT16)mLPSS_I2C_CLOCK_SCL_INFO[BusNo].SS_SCL_LCNT);
  DEBUG ((DEBUG_INFO, "I2cBusFrequencySet R_IC_SS_SCL_HCNT: 0x%08X, R_IC_SS_SCL_LCNT: 0x%08X\r\n",\
         MmioRead32 (I2CBaseAddress + R_IC_SS_SCL_HCNT), MmioRead32 (I2CBaseAddress + R_IC_SS_SCL_LCNT)));
  //
  //  Set the 400 KHz clock divider according to SV result and I2C spec
  //
  MmioWrite32 (I2CBaseAddress + R_IC_FS_SCL_HCNT, (UINT16)mLPSS_I2C_CLOCK_SCL_INFO[BusNo].FS_SCL_HCNT);
  MmioWrite32 (I2CBaseAddress + R_IC_FS_SCL_LCNT, (UINT16)mLPSS_I2C_CLOCK_SCL_INFO[BusNo].FS_SCL_LCNT);
  DEBUG ((DEBUG_INFO, "I2cBusFrequencySet R_IC_FS_SCL_HCNT: 0x%08X, R_IC_FS_SCL_LCNT: 0x%08X\r\n",\
         MmioRead32 (I2CBaseAddress + R_IC_FS_SCL_HCNT), MmioRead32 (I2CBaseAddress + R_IC_FS_SCL_LCNT)));
  //
  //  Set the 3.4MHz clock divider according to SV result and I2C spec
  //
  MmioWrite32 (I2CBaseAddress + R_IC_HS_SCL_HCNT, (UINT16)mLPSS_I2C_CLOCK_SCL_INFO[BusNo].HS_SCL_HCNT);
  MmioWrite32 (I2CBaseAddress + R_IC_HS_SCL_LCNT, (UINT16)mLPSS_I2C_CLOCK_SCL_INFO[BusNo].HS_SCL_LCNT);
  DEBUG ((DEBUG_INFO, "I2cBusFrequencySet R_IC_HS_SCL_HCNT: 0x%08X, R_IC_HS_SCL_LCNT: 0x%08X\r\n",\
         MmioRead32 (I2CBaseAddress + R_IC_HS_SCL_HCNT), MmioRead32 (I2CBaseAddress + R_IC_HS_SCL_LCNT)));

  switch (mLPSS_I2C_CLOCK_SCL_INFO[BusNo].I2c_Speed) {
    case Standard_Speed:
      MmioWrite32 ( I2CBaseAddress + R_IC_SDA_HOLD, (UINT16)0x06); //100K
      *I2cMode = V_SPEED_STANDARD;
      DEBUG ((DEBUG_INFO, "I2cBusFrequencySet I2cMode: 0x%04X\r\n", *I2cMode));
      break;

    case Fast_Speed:
      MmioWrite32 ( I2CBaseAddress + R_IC_SDA_HOLD, (UINT16)0x06); //400K
      *I2cMode = V_SPEED_FAST;
      DEBUG ((DEBUG_INFO, "I2cBusFrequencySet I2cMode: 0x%04X\r\n", *I2cMode));
      break;

    case High_Speed:
      MmioWrite32 ( I2CBaseAddress + R_IC_SDA_HOLD, (UINT16)0x06); //3.4M
      *I2cMode = V_SPEED_HIGH;
      DEBUG ((DEBUG_INFO, "I2cBusFrequencySet I2cMode: 0x%04X\r\n", *I2cMode));
      break;

    default:
      MmioWrite32 ( I2CBaseAddress + R_IC_SDA_HOLD, (UINT16)0x06); //400K
      *I2cMode = V_SPEED_FAST;
      DEBUG ((DEBUG_INFO, "I2cBusFrequencySet I2cMode: 0x%04X\r\n", *I2cMode));
  }

  //
  //  Select the frequency counter
  //  Enable restart condition,
  //  Enable master FSM, disable slave FSM
  //
  *I2cMode |= B_IC_RESTART_EN | B_IC_SLAVE_DISABLE | B_MASTER_MODE;
  DEBUG ((DEBUG_INFO, "I2cBusFrequencySet R_IC_SDA_HOLD: 0x%08X\r\n", MmioRead32 (I2CBaseAddress + R_IC_SDA_HOLD)));
  DEBUG ((DEBUG_INFO, "I2cBusFrequencySet I2cMode: 0x%04X\r\n", *I2cMode));

  return EFI_SUCCESS;
}

/**
  Initializes the host controller to execute I2C commands.

  @param[in]  BusNo               - I2C Bus number to which the I2C device has been connected
  @param[in]  SlaveAddress        - Slave address of the I2C device
  @param[out] I2CBaseAddress      - Return BAR0 address of I2C host controller

  @retval  EFI_SUCCESS            - Initialization on the I2C host controller completed.
  @retval  EFI_INVALID_PARAMETER  - Invalid slave address
  @retval  EFI_DEVICE_ERROR       - Operation failed, device error
  @retval  Others                 - Failed to initialize I2C host controller
**/
EFI_STATUS
I2CInit (
  IN  UINT8    BusNo,
  IN  UINT16   SlaveAddress,
  OUT UINTN    *I2CBaseAddress
  )
{
  EFI_STATUS Status;
  UINT32     NumTries;
  UINT16     I2cMode;
  UINTN      PciMmBase;
  UINTN      BaseAddress;

  //
  //  Verify the parameters
  //
  if (1023 < SlaveAddress) {
    Status = EFI_INVALID_PARAMETER;
    DEBUG ((DEBUG_INFO, "I2cStartRequest Exit with Status %r\r\n", Status));
    return Status;
  }

  PciMmBase = MmPciAddress (
                mLpssPciDeviceList[BusNo].Segment,
                mLpssPciDeviceList[BusNo].BusNum,
                mLpssPciDeviceList[BusNo].DeviceNum,
                mLpssPciDeviceList[BusNo].FunctionNum,
                0
                );

  BaseAddress = MmioRead32 (PciMmBase + R_LPSS_IO_BAR) & B_LPSS_IO_BAR_BA;

  //
  // Skip reinit if targeting the same I2C bus
  //
  if (BaseAddress == mLpssPciDeviceList[BusNo].Bar0) {
    MmioWrite32 (BaseAddress + R_IC_TAR, SlaveAddress);
    *I2CBaseAddress = BaseAddress;
    return EFI_SUCCESS;
  }

  Status = ProgramPciLpssI2C (BusNo);
  if (Status != EFI_SUCCESS) {
    DEBUG((DEBUG_ERROR, "ProgramPciLpssI2C failed ! %r\r\n", Status));
    return Status;
  }

  BaseAddress = (UINT32) mLpssPciDeviceList[BusNo].Bar0;
  DEBUG ((DEBUG_INFO, "I2CBaseAddress = 0x%x \n", BaseAddress));

  NumTries = 10000; // 1 seconds
  while ((STAT_MST_ACTIVITY == (MmioRead32 (BaseAddress + R_IC_STATUS) & STAT_MST_ACTIVITY))) {
    MicroSecondDelay (10);
    NumTries --;
    if (0 == NumTries) {
      DEBUG ((DEBUG_ERROR, "Try timeout\r\n"));
      return EFI_DEVICE_ERROR;
    }
  }

  Status = I2cDisable(BaseAddress);
  DEBUG ((DEBUG_INFO, "I2cDisable Status = %r\r\n", Status));
  I2cBusFrequencySet(BusNo, BaseAddress, &I2cMode); // Set I2cMode

  MmioWrite32 (BaseAddress + R_IC_INTR_MASK, 0x0);
  if (0x7f < SlaveAddress) {
    SlaveAddress = (SlaveAddress & 0x3ff) | IC_TAR_10BITADDR_MASTER;
  }
  MmioWrite32 (BaseAddress + R_IC_TAR, SlaveAddress);
  MmioWrite32 (BaseAddress + R_IC_RX_TL, 0);
  MmioWrite32 (BaseAddress + R_IC_TX_TL, 0);
  MmioWrite32 (BaseAddress + R_IC_CON, I2cMode);
  Status = I2cEnable(BaseAddress);

  DEBUG((DEBUG_INFO, "I2cEnable Status = %r\r\n", Status));
  MmioRead32 (BaseAddress + R_IC_CLR_TX_ABRT);
  *I2CBaseAddress = BaseAddress;
  return EFI_SUCCESS;
}

/**
  Read bytes from I2C Device
  This is actual I2C hardware operation function.

  @param[in]  BusNo             - I2C Bus number to which the I2C device has been connected
  @param[in]  SlaveAddress      - Slave address of the I2C device (7-bit)
  @param[in]  ReadBytes         - Number of bytes to be read
  @param[out] ReadBuffer        - Address to which the value read has to be stored
  @param[in]  Start             - It controls whether a RESTART is issued before the byte is sent or received.
  @param[in]  End               - It controls whether a STOP is issued after the byte is sent or received.

  @retval EFI_SUCCESS           - The byte value read successfully
  @retval EFI_DEVICE_ERROR      - Operation failed
  @retval EFI_TIMEOUT           - Hardware retry timeout
  @retval Others                - Failed to read a byte via I2C
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
  EFI_STATUS  Status;
  UINT32      I2cStatus;
  UINT16      ReceiveData;
  UINT8       *ReceiveDataEnd;
  UINT8       *ReceiveRequest;
  UINT16      raw_intr_stat;
  UINT32      Count = 0;
  UINTN       I2CBaseAddress;
  UINT8       *ReadPtr;

  //
  // Read should always after write, so, base address should already be initialized, then get base address directly
  //
  I2CBaseAddress = (UINT32) mLpssPciDeviceList[BusNo].Bar0;
  DEBUG ((DEBUG_INFO, "mLpssPciDeviceList returned base address = 0x%08x\n", I2CBaseAddress));

  Status = EFI_SUCCESS;

  ReceiveDataEnd = &ReadBuffer [ReadBytes];
  ReadPtr = ReadBuffer;
  if (ReadBytes) {
    ReceiveRequest = ReadBuffer;
    //DEBUG((DEBUG_INFO,"Read: ---------------%d bytes to RX\r\n", ReceiveDataEnd - ReceiveRequest));

    while ((ReceiveDataEnd > ReceiveRequest) || (ReceiveDataEnd > ReadPtr)) {
      //
      // Check for NACK
      //
      raw_intr_stat = (UINT16)MmioRead32 (I2CBaseAddress + R_IC_RAW_INTR_STAT);
      if (0 != (raw_intr_stat & I2C_INTR_TX_ABRT)) {
        MmioRead32 (I2CBaseAddress + R_IC_CLR_TX_ABRT);
        Status = EFI_DEVICE_ERROR;
        DEBUG ((DEBUG_ERROR, "TX ABRT ,%d bytes hasn't been transferred\r\n", ReceiveDataEnd - ReceiveRequest));
        break;
      }
      
      //
      // Determine if another byte was received
      //
      I2cStatus = (UINT16)MmioRead32 (I2CBaseAddress + R_IC_STATUS);
      if (0 != (I2cStatus & STAT_RFNE)) {
        ReceiveData = (UINT16)MmioRead32 (I2CBaseAddress + R_IC_DATA_CMD);
        *ReadPtr++ = (UINT8)ReceiveData;
        DEBUG ((DEBUG_INFO, "MmioRead32 ,1 byte 0x:%x is received\r\n", ReceiveData));
      }

      if (ReceiveDataEnd == ReceiveRequest) {
        MicroSecondDelay (FIFO_WRITE_DELAY);
        Count++;
        if (Count < 1024) { // sys hung  avoid no ul-pmc device
          continue; // Waiting the last request to get data and make (ReceiveDataEnd > ReadBuffer) =TRUE.
        } else {
          break;
        }
      }
      //
      // Wait until a read request will fit
      //
      if (0 == (I2cStatus & STAT_TFNF)) {
        MicroSecondDelay (10);
        continue;
      }
      //
      // Issue the next read request
      //
      if (End && Start) {
        MmioWrite32 (I2CBaseAddress + R_IC_DATA_CMD, B_READ_CMD|B_CMD_RESTART|B_CMD_STOP);
      } else if (!End && Start) {
        MmioWrite32 (I2CBaseAddress + R_IC_DATA_CMD, B_READ_CMD|B_CMD_RESTART);
      } else if (End && !Start) {
        MmioWrite32 (I2CBaseAddress + R_IC_DATA_CMD, B_READ_CMD|B_CMD_STOP);
      } else if (!End && !Start) {
        MmioWrite32 (I2CBaseAddress + R_IC_DATA_CMD, B_READ_CMD);
      }
      MicroSecondDelay (FIFO_WRITE_DELAY); //wait after send cmd

      ReceiveRequest += 1;
    }
  }
  return Status;

}

/**
  Write bytes to I2C Device
  This is actual I2C hardware operation function.

  @param[in]  BusNo             - I2C Bus number to which the I2C device has been connected
  @param[in]  SlaveAddress      - Slave address of the I2C device (7-bit)
  @param[in]  WriteBytes        - Number of bytes to be written
  @param[in]  WriteBuffer       - Address to which the byte value has to be written
  @param[in]  Start             - It controls whether a RESTART is issued before the byte is sent or received.
  @param[in]  End               - It controls whether a STOP is issued after the byte is sent or received.

  @retval EFI_SUCCESS           - The byte value written successfully
  @retval EFI_DEVICE_ERROR      - Operation failed
  @retval EFI_TIMEOUT           - Hardware retry timeout
  @retval Others                - Failed to write a byte via I2C
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
  UINT16      Data16;
  EFI_STATUS  Status;
  UINT32      I2cStatus;
  UINT8       *TransmitPtr;
  UINT8       *TransmitEnd;
  UINT16      raw_intr_stat;
  UINT32      Count=0;
  UINTN       I2CBaseAddress;

  Status = I2CInit (BusNo, SlaveAddress, &I2CBaseAddress);
  if (Status != EFI_SUCCESS) {
    DEBUG ((DEBUG_ERROR, "I2CInit failed ! %r\r\n", Status));
    return Status;
  }
  DEBUG ((DEBUG_INFO, "I2CInit returned base address = 0x%08x\n", I2CBaseAddress));
  
  TransmitPtr = WriteBuffer;
  TransmitEnd = &WriteBuffer [WriteBytes];
  if (WriteBytes) {
    raw_intr_stat = (UINT16)MmioRead32 (I2CBaseAddress + R_IC_RAW_INTR_STAT);
    if (0 != (raw_intr_stat & I2C_INTR_TX_ABRT)) {
      MmioRead32 (I2CBaseAddress + R_IC_CLR_TX_ABRT);
      DEBUG ((DEBUG_ERROR, "%a(#%d) - raw_intr_stat = %04x\n", __FUNCTION__, __LINE__, TransmitEnd, TransmitPtr, raw_intr_stat));
    }

    //DEBUG ((DEBUG_INFO, "Write: --------------%d bytes to TX\r\n", TransmitEnd - WriteBuffer));
    while (TransmitEnd > TransmitPtr) {
      I2cStatus = MmioRead32 (I2CBaseAddress + R_IC_STATUS);
      raw_intr_stat = (UINT16)MmioRead32 (I2CBaseAddress + R_IC_RAW_INTR_STAT);
      if (0 != (raw_intr_stat & I2C_INTR_TX_ABRT)) {
        MmioRead32 (I2CBaseAddress + R_IC_CLR_TX_ABRT);
        Status = EFI_DEVICE_ERROR;
        DEBUG ((DEBUG_ERROR, "%a(#%d) - TX ABRT TransmitEnd:0x%x WritePtr:0x%x\r\n", __FUNCTION__, __LINE__, TransmitEnd, TransmitPtr));
        break;
      }
      if (0 == (I2cStatus & STAT_TFNF)) {
        MicroSecondDelay (FIFO_WRITE_DELAY);
        continue;
      }

      Data16 = (UINT16) *TransmitPtr;
      if (End && Start) {
        Data16 |= (B_CMD_RESTART | B_CMD_STOP);
      } else if (!End && Start) {
        Data16 |= B_CMD_RESTART;
      } else if (End && !Start) {
        Data16 |= B_CMD_STOP;
      }
      Data16 = MmioWrite16 (I2CBaseAddress + R_IC_DATA_CMD, Data16);
      TransmitPtr++;

      //
      // Add a small delay to work around some odd behavior being seen.  Without this delay bytes get dropped.
      //
      MicroSecondDelay (FIFO_WRITE_DELAY);
      //
      // Time out
      //
      while (1) {
        raw_intr_stat = MmioRead16 (I2CBaseAddress + R_IC_RAW_INTR_STAT);
        if (0 != ( raw_intr_stat & I2C_INTR_TX_ABRT)) {
          MmioRead16 (I2CBaseAddress + R_IC_CLR_TX_ABRT);
          Status = EFI_DEVICE_ERROR;
          DEBUG ((DEBUG_ERROR, "TX ABRT TransmitEnd:0x%x WriteBuffer:0x%x\r\n", TransmitEnd, WriteBuffer));
        }
        if (0 == MmioRead16(I2CBaseAddress + R_IC_TXFLR)) break;

        MicroSecondDelay (FIFO_WRITE_DELAY);
        Count++;
        if (Count < 1024) { //to avoid sys hung without ul-pmc device on RVP
          continue; //Waiting the last request to get data and make (ReceiveDataEnd > ReadBuffer) =TRUE.
        } else {
          DEBUG ((DEBUG_ERROR, "hardware timeout, 1024 times try!\r\n"));
          Status = EFI_TIMEOUT;
          break;
        }
      }//while( 1 )

    }

  }
  if (EFI_ERROR (Status))
    DEBUG ((DEBUG_ERROR, "I2cStartRequest Exit with Status %r\r\n", Status));

  return Status;
}

/**
  Read bytes from I2C Device

  @param[in]  BusNo             - I2C Bus number to which the I2C device has been connected
  @param[in]  SlaveAddress      - Slave address of the I2C device (7-bit)
  @param[in]  Offset            - Register offset from which the data has to be read
  @param[in]  ReadBytes         - Number of bytes to be read
  @param[out] ReadBuffer        - Address to which the value read has to be stored

  @retval EFI_SUCCESS           - Read bytes from I2C device successfully
  @retval Others                - Return status depends on ByteReadI2C_Basic
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
  EFI_STATUS          Status;

  //DEBUG ((EFI_D_INFO, "ByteReadI2C:---offset:0x%x\n",Offset));
  Status = ByteWriteI2C_Basic (BusNo, SlaveAddress, 1, &Offset, TRUE, FALSE);
  if (!EFI_ERROR (Status)) {
    Status = ByteReadI2C_Basic (BusNo, SlaveAddress, ReadBytes, ReadBuffer, TRUE, TRUE);
  } else {
    DEBUG ((DEBUG_ERROR, "ByteReadI2C/ByteWriteI2C_Basic: %r\n", Status));
  }
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "ByteReadI2C: %r\n", Status));
  }

  return Status;
}

/**
  Write bytes to I2C Device

  @param[in]  BusNo             - I2C Bus number to which the I2C device has been connected
  @param[in]  SlaveAddress      - Slave address of the I2C device (7-bit)
  @param[in]  Offset            - Register offset from which the data has to be read
  @param[in]  WriteBytes        - Number of bytes to be written
  @param[in]  WriteBuffer       - Address to which the byte value has to be written

  @retval EFI_SUCCESS           - Write bytes to I2C device successfully
  @retval Others                - Return status depends on ByteWriteI2C_Basic
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
  EFI_STATUS          Status;

  //DEBUG ((EFI_D_INFO, "ByteWriteI2C:---offset/bytes/buf:0x%x,0x%x,0x%x,0x%x\n",Offset,WriteBytes,WriteBuffer,*WriteBuffer));
  Status = ByteWriteI2C_Basic (BusNo, SlaveAddress, 1, &Offset, TRUE, FALSE);
  if (!EFI_ERROR (Status)) {
    Status = ByteWriteI2C_Basic (BusNo, SlaveAddress, WriteBytes, WriteBuffer, FALSE, TRUE);
  } else {
    DEBUG ((DEBUG_ERROR, "ByteWriteI2C/ByteWriteI2C_Basic: %r\n", Status));
  }
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "ByteWriteI2C: %r\n", Status));
  }

  return Status;
}
