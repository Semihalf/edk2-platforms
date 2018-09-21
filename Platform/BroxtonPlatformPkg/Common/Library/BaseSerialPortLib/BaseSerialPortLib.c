/** @file
  Serial I/O Port library functions with no library constructor/destructor.

  Copyright (c) 2012 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/SerialPortLib.h>
#include <Library/PlatformHookLib.h>
#include <Library/ScSerialIoUartLib.h>

#ifdef TRACE_HUB_DEBUGLIB_USAGE
#include <Library/TraceHubDebugLib.h>
#endif

//
// UART register offsets and bitfields
//
#define RXBUF_OFFSET      0x00
#define TXBUF_OFFSET      0x00
#define BAUD_LOW_OFFSET   0x00
#define BAUD_HIGH_OFFSET  0x01
#define IER_OFFSET        0x01
#define LCR_SHADOW_OFFSET 0x01
#define FCR_SHADOW_OFFSET 0x02
#define IR_CONTROL_OFFSET 0x02
#define FCR_OFFSET        0x02
#define EIR_OFFSET        0x02
#define BSR_OFFSET        0x03
#define LCR_OFFSET        0x03
#define MCR_OFFSET        0x04
#define LSR_OFFSET        0x05
#define MSR_OFFSET        0x06

//
// UART Register Bit Defines
//
#define FCR_FIFOE    BIT0
#define FCR_FIFO64   BIT5
#define LSR_TXRDY    0x20
#define LSR_RXDA     0x01
#define DLAB         BIT7
#define MCR_RTS      BIT1
#define MSR_CTS      BIT4
#define MSR_DSR      BIT5

#define MAX_BAUD_RATE     115200

RETURN_STATUS
EFIAPI
SerialPortInitialize (
  VOID
  )
{
  //
  // Initialize all 4 UARTs for possible debug use. All 4 UART PADs are programmed appropriately at this point.
  // We store the Debug Serial port in the scratchpad register of the UARTs.
  //
  PchSerialIoUartInit (0,
                       (PcdGet8 (PcdSerialFifoControl) & FCR_FIFOE) ? TRUE : FALSE,
                       PcdGet32 (PcdSerialBaudRate),
                       PcdGet8 (PcdSerialLineControl),
                       PcdGetBool (PcdSerialUseHardwareFlowControl));

  PchSerialIoUartInit (1,
                       (PcdGet8 (PcdSerialFifoControl) & FCR_FIFOE) ? TRUE : FALSE,
                       PcdGet32 (PcdSerialBaudRate),
                       PcdGet8 (PcdSerialLineControl),
                       PcdGetBool (PcdSerialUseHardwareFlowControl));

  PchSerialIoUartInit (2,
                       (PcdGet8 (PcdSerialFifoControl) & FCR_FIFOE) ? TRUE : FALSE,
                       PcdGet32 (PcdSerialBaudRate),
                       PcdGet8 (PcdSerialLineControl),
                       PcdGetBool (PcdSerialUseHardwareFlowControl));

  PchSerialIoUartInit (3,
                       (PcdGet8 (PcdSerialFifoControl) & FCR_FIFOE) ? TRUE : FALSE,
                       PcdGet32 (PcdSerialBaudRate),
                       PcdGet8 (PcdSerialLineControl),
                       PcdGetBool (PcdSerialUseHardwareFlowControl));

  //
  // Set default debug port now that we have initiazed the UARTs
  //
  PchSetDebugPort (PcdGet8 (PcdSerialIoUartNumber));

  return RETURN_SUCCESS;
}

/**
  Common function to write trace data to a chosen debug interface like
  UART Serial device, USB Serial device or Trace Hub device

  @param[in]  Buffer           Point of data buffer which need to be writed.
  @param[in]  NumberOfBytes    Number of output bytes which are cached in Buffer.

**/
UINTN
EFIAPI
SerialPortWrite (
  IN UINT8     *Buffer,
  IN UINTN     NumberOfBytes
  )
{
  PchSerialIoUartOut (PchGetDebugPort (), Buffer, NumberOfBytes);

  return RETURN_SUCCESS;
}

/**
  Common function to Read data from UART serial device, USB serial device and save the datas in buffer.

  @param[in]  Buffer           Point of data buffer which need to be writed.
  @param[in]  NumberOfBytes    Number of output bytes which are cached in Buffer.

**/
UINTN
EFIAPI
SerialPortRead (
  OUT UINT8     *Buffer,
  IN  UINTN     NumberOfBytes
  )
{

  PchSerialIoUartIn (PchGetDebugPort (), Buffer, NumberOfBytes, FALSE);

  return RETURN_SUCCESS;
}

/**
  Polls a serial device to see if there is any data waiting to be read.

  Polls a serial device to see if there is any data waiting to be read.
  If there is data waiting to be read from the serial device, then TRUE is returned.
  If there is no data waiting to be read from the serial device, then FALSE is returned.

  @retval TRUE             Data is waiting to be read from the serial device.
  @retval FALSE            There is no data waiting to be read from the serial device.

**/
BOOLEAN
EFIAPI
SerialPortPoll (
  VOID
  )
{

  BOOLEAN   Status;

  Status = FALSE;
  Status |= PchSerialIoUartPoll (PchGetDebugPort ());

  return Status;
}

