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
    Device (ACD1) {
        Name (_ADR, 0x1A)  // _ADR: Address
        Name (_HID, "INT34E0")  // _HID: Hardware ID
        Name (_CID, "INT34E0")  // _CID: Compatible ID
        Name (_DDN, "Intel(R) Smart Sound Technology Audio Codec")  // _DDN: DOS Device Name
        Name (_UID, One)  // _UID: Unique ID

        Name (SBUF, ResourceTemplate () {
          I2CSerialBus (
            0x1A,               // SlaveAddress: bus address
            ,                   // SlaveMode: default to ControllerInitiated
            400000,             // ConnectionSpeed: in Hz
            ,                   // Addressing Mode: default to 7 bit
            "\\_SB.PCI0.I2C1",  // ResourceSource: I2C bus controller name
            ,                   // Descriptor Name: creates name for offset of resource descriptor
          )  // VendorData
          GpioInt (Level, ActiveLow, ExclusiveAndWake, PullUp, 0,"\\_SB.GPO0") {64} //  AUD_INT
        })

        Method (_CRS, 0, Serialized) { // _CRS: Current Resource Settings
          Return (SBUF)
        }

        Method (_STA, 0, NotSerialized) { // _STA: Status
            If (LEqual (ADOS, 0x02)) {
                Return (0x0F)
            }
            Return (Zero)
        }

        Method (_DIS, 0, NotSerialized) { // _DIS: Disable Device
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
    }
}

