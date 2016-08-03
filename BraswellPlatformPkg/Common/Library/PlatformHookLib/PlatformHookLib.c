/** @file
  Null Platform Hook Library instance.

  Copyright (c) 2015, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Base.h>
#include <Library/PlatformHookLib.h>

#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <PchRegs.h>
#include <PlatformBaseAddresses.h>
#include <IndustryStandard/Pci.h>




#define PCI_IDX             0xCF8
#define PCI_DAT             0xCFC

#define PCI_LPC_BASE    (0x8000F800)
#define PCI_LPC_REG(x)  (PCI_LPC_BASE + (x))

#define V_PCH_ILB_IRQE_UARTIRQEN_IRQ4             BIT4 // UART IRQ4 Enable
#ifndef PCIEX_BASE_ADDRESS
#define PCIEX_BASE_ADDRESS                        ((UINTN)PcdGet64(PcdPciExpressBaseAddress))
#endif
#define PciD31F0RegBase                           PCIEX_BASE_ADDRESS + (UINT32) (31 << 15)

VOID
EnableInternalUart (
  VOID
  )
{
  //
  // Program and enable PMC Base.
  //
  IoWrite32 (PCI_IDX, PCI_LPC_REG(R_PCH_LPC_PMC_BASE));
  IoWrite32 (PCI_DAT, (PMC_BASE_ADDRESS | B_PCH_LPC_PMC_BASE_EN));

  //
  // Program and enable iLB Base.
  //
  IoWrite32 (PCI_IDX, PCI_LPC_REG(R_PCH_LPC_ILB_BASE));
  IoWrite32 (PCI_DAT, (ILB_BASE_ADDRESS | B_PCH_LPC_ILB_BASE_EN));
  //
  // Enable internal UART interrupt.
  //
  MmioOr32 (ILB_BASE_ADDRESS + R_PCH_ILB_IRQE, (UINT32) (B_PCH_ILB_IRQE_UARTIRQEN_IRQ4));
  //
  // GPIO: N67(HV_DDI2_DDC_SCL) - Setting Mode 3
  // UART0_TXD
  //
  MmioWrite32 ((UINTN) (IO_BASE_ADDRESS + 0xD438), (UINT32) 0x00930300);
  //
  // GPIO: N62(HV_DDI2_DDC_SDA) - Setting Mode 3
  // UART0_RXD
  //
  MmioWrite32 ((UINTN) (IO_BASE_ADDRESS + 0xD410), (UINT32) 0x00930300);
  MmioOr32 (
    (UINTN) (PciD31F0RegBase + R_PCH_LPC_UART_CTRL),
    (UINT32) (B_PCH_LPC_UART_CTRL_COM1_EN)
    );
}


#define MC_MCR      0x000000D0
#define MC_MDR      0x000000D4
#define MC_MCRX     0x000000D8
#define MC_MCRXX    0x000000DC

#define MSG_BUS_ENABLED     0x000000F0
#define MSGBUS_MASKHI       0xFFFFFF00
#define MSGBUS_MASKLO       0x000000FF

#define MESSAGE_BYTE_EN          BIT4
#define MESSAGE_WORD_EN          BIT4 | BIT5
#define MESSAGE_DWORD_EN         BIT4 | BIT5 | BIT6 | BIT7

#define SIDEBAND_OPCODE          0x78
#define MEMREAD_OPCODE           0x00000000
#define MEMWRITE_OPCODE          0x01000000


RETURN_STATUS
PlatformPchMsgBusAndThenOrEx32 (
  IN     UINT8                           PortId,
  IN     UINT32                          Register,
  IN     UINT32                          AndData,
  IN     UINT32                          OrData,
  IN     UINT8                           ReadOpCode,
  IN     UINT8                           WriteOpCode,
  IN     UINT8                           Bar,          OPTIONAL
  IN     UINT8                           Device,       OPTIONAL
  IN     UINT8                           Function      OPTIONAL
  )
{
  UINT32  Data32;
  UINTN   PciExpressBase;

  PciExpressBase = (UINTN) PcdGet64 (PcdPciExpressBaseAddress);
  //PciExpressBase = 0xE0000000;

  MmioWrite32 (
    (UINTN) (PciExpressBase + MC_MCRXX),
    (UINT32) (((Bar & 0x07) << 8) | (((Device << 3) | (Function)) & 0xFF))
    );
  MmioWrite32 (
    (UINTN) (PciExpressBase + MC_MCRX),
    (UINT32) (Register & MSGBUS_MASKHI)
    );
  MmioWrite32 (
    (UINTN) (PciExpressBase + MC_MCR),
    (UINT32) ((ReadOpCode << 24) | (PortId << 16) | ((Register & MSGBUS_MASKLO) << 8) | MESSAGE_DWORD_EN)
    );
  Data32 = MmioRead32 ((UINTN) (PciExpressBase + MC_MDR));
  Data32 = (UINT32) ((Data32 & AndData) | OrData);


  MmioWrite32 (
    (UINTN) (PciExpressBase + MC_MCRXX),
    (UINT32) (((Bar & 0x07) << 8) | (((Device << 3) | (Function)) & 0xFF))
    );
  MmioWrite32 (
    (UINTN) (PciExpressBase + MC_MCRX),
    (UINT32) (Register & MSGBUS_MASKHI)
    );
  MmioWrite32 (
    (UINTN) (PciExpressBase + MC_MDR),
    (UINT32) (Data32)
    );
  MmioWrite32 (
    (UINTN) (PciExpressBase + MC_MCR),
    (UINT32) ((WriteOpCode << 24) | (PortId << 16) | ((Register & MSGBUS_MASKLO) << 8) | MESSAGE_DWORD_EN)
    );

  return RETURN_SUCCESS;
}

VOID
EnableLpssHsUart (
  VOID
  )
{
   UINTN                 PciLibAddress;
   UINT32                LpssMmioBase0;
   UINT32                Buffer32;

  //
  //LPSS HS-UART0
  //UART1_CTS_B 1 UART1_CTS_B IOBASE + 0x04818 [31:0] = 0x00910300
  //UART1_RTS_B 1 UART1_RTS_B IOBASE + 0x04800 [31:0] = 0x00910300
  //UART1_RXD 1 UART1_RXD IOBASE + 0x04808 [31:0] = 0x00910300
  //UART1_TXD 1 UART1_TXD IOBASE + 0x04828 [31:0] = 0x00910300
  MmioWrite32 ((UINTN) (IO_BASE_ADDRESS + 0x04818), (UINT32) 0x00910300);
  MmioWrite32 ((UINTN) (IO_BASE_ADDRESS + 0x04800), (UINT32) 0x00910300);
  MmioWrite32 ((UINTN) (IO_BASE_ADDRESS + 0x04808), (UINT32) 0x00910300);
  MmioWrite32 ((UINTN) (IO_BASE_ADDRESS + 0x04828), (UINT32) 0x00910300);

  PlatformPchMsgBusAndThenOrEx32(
    0xA0,
    0x258,
    0xFFFFFFFF,
    (BIT27),
    0x06,
    0x07,
    0x00,
    0x00,
    0x00
    );

  PciLibAddress = PCI_LIB_ADDRESS (0, 30, 3, 0);
  PciWrite32 ((PciLibAddress + PCI_BASE_ADDRESSREG_OFFSET), (UINT32)PcdGet64 (PcdSerialRegisterBase));
  LpssMmioBase0 = (UINT32)PcdGet64 (PcdSerialRegisterBase);

  PciOr16 (PciLibAddress + PCI_COMMAND_OFFSET, (EFI_PCI_COMMAND_BUS_MASTER | EFI_PCI_COMMAND_MEMORY_SPACE));

  Buffer32 = (UINT32) (B_PCH_LPSS_HSUART_MEM_PCP_CLKUPDATE |
            (0x3D09 << 16) |
            (0x1B00 << 1) |
            B_PCH_LPSS_HSUART_MEM_PCP_CLKEN);
  MmioWrite32 (
  (UINTN) (LpssMmioBase0 + R_PCH_LPSS_HSUART_MEM_PCP),
  Buffer32
  );

  Buffer32 = (UINT32) (B_PCH_LPSS_HSUART_MEM_RESETS_FUNC | B_PCH_LPSS_HSUART_MEM_RESETS_APB);
  MmioWrite32 (
  (UINTN) (LpssMmioBase0 + R_PCH_LPSS_HSUART_MEM_RESETS),
  Buffer32
  );

  PlatformPchMsgBusAndThenOrEx32 (
    0xA0,
    0x258,
    (UINT32) ~((BIT19 | BIT18)),
    (BIT18 | BIT1),
    0x06,
    0x07,
    0x00,
    0x00,
    0x00
  );

  return;
}

/**
  Performs platform specific initialization required for the CPU to access
  the hardware associated with a SerialPortLib instance.  This function does
  not intiailzie the serial port hardware itself.  Instead, it initializes
  hardware devices that are required for the CPU to access the serial port
  hardware.  This function may be called more than once.

  @retval RETURN_SUCCESS       The platform specific initialization succeeded.
  @retval RETURN_DEVICE_ERROR  The platform specific initialization could not be completed.

**/
RETURN_STATUS
EFIAPI
PlatformHookSerialPortInitialize (
  VOID
  )
{
  if (PcdGet64 (PcdSerialRegisterBase) == 0) {
    return RETURN_SUCCESS;
  }

  if ((BOOLEAN)PcdGetBool (PcdSerialUseMmio) == FALSE) {

    EnableInternalUart ();
  } else {

    EnableLpssHsUart();
  }

  return  RETURN_SUCCESS;
}

