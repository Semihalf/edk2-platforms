/** @file

Copyright (c) 2017 - 2018 Intel Corporation.

This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

Scope (\_SB.PCI0.I2C1) {
  Device (EEP0) {
    Name (_ADR, 0x50)      // _ADR: Address
    Name (_HID, "INT3500") // _HID: Hardware ID
    Name (_CID, "24m02")   // _CID: Compatible ID
    Name (_DDN, "EEPROM")  // _DDN: DOS Device Name
    Name (RBUF, ResourceTemplate () {
       I2cSerialBus (
           0x0050,                // SlaveAddress: bus address
       ControllerInitiated,   // SlaveMode: default to ControllerInitiated
       400000,                // ConnectionSpeed: in Hz
       AddressingMode7Bit,    // Addressing Mode: default to 7 bit
       "\\_SB.PCI0.I2C1",     // ResourceSource: I2C bus controller name
       0x00                   // Descriptor Name: creates name for offset of resource descriptor
   ) // Vendor Data
    })

    Method (_CRS, 0, NotSerialized) { // _CRS: Current Resource Settings
      Return (RBUF) /* \_SB_.PCI0.I2C1.EEP0.RBUF */
    }

    Method (_STA, 0, NotSerialized) { // _STA: Status
      Return (0x0F)
    }
  }
}

