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
    Device (ACD0) { // Audio Codec driver I2C
      Name (_ADR, 0x1A)
      Name (_HID, "INT34C1")
      Name (_CID, "INT34C1")
      Name (_DDN, "Intel(R) Smart Sound Technology Audio Codec" )
      Name (_UID, 1)

      Name (SBUF, ResourceTemplate () {
        I2CSerialBus (
          0x1A,               // SlaveAddress: bus address
          ,                   // SlaveMode: default to ControllerInitiated
          400000,             // ConnectionSpeed: in Hz
          ,                   // Addressing Mode: default to 7 bit
          "\\_SB.PCI0.I2C1",  // ResourceSource: I2C bus controller name
          ,                   // Descriptor Name: creates name for offset of resource descriptor
        )  // VendorData
        GpioInt (Level, ActiveLow, ExclusiveAndWake, PullUp, 0,"\\_SB.GPO0") {20} //  AUD_INT
      })

      Method (_CRS, 0x0, NotSerialized) {
        Return (SBUF)
      }

      Method (_STA, 0x0, NotSerialized) {
        If (LEqual (ADOS, 1)) {  // 1-WM8281
          Return (0x0F)
        }
        Return (0x0)
      }

      Method (_DIS, 0x0, NotSerialized) {
      }

      Method (DEVS) {
        Return (88)
      }

      Method (DEVC) {
        Return (Buffer () {
            0x54, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x10, 0xFF, 0xFF, 0xFF,
            0x32, 0xFF, 0xFF, 0xFF,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x37, 0x03, 0xD0, 0x83,
            0x00, 0x00, 0x70, 0xC0,
            0x00, 0x00, 0x00, 0x00,
            0x04, 0x00, 0x01, 0x02,
            0x0F, 0x00, 0x00, 0x00,
            0x0F, 0x00, 0x00, 0x00,
            0x02, 0x40, 0x00, 0x00,
            0x04, 0x00, 0x00, 0x00,
            0x00, 0x0F, 0x07, 0x07,
            0x20, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00
        })
      }
    } // Device (ACD0)
}

