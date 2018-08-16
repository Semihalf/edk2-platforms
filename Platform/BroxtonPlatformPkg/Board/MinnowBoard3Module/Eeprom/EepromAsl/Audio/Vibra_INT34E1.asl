/** @file

Copyright (c) 2017 - 2018 Intel Corporation.

This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

Scope (\_SB) {
  Device (VIBR) {
    Name (_ADR, Zero)                               // _ADR: Address
    Name (_HID, "INT34E1")                          // _HID: Hardware ID
    Name (_CID, "INT34E1")                          // _CID: Compatible ID
    Name (_DDN, "Intel(R) Vibra Driver - VIB8601")  // _DDN: DOS Device Name
    Name (_UID, One)                                // _UID: Unique ID
    Name (RBUF, ResourceTemplate () {
      // SOC GPIO_35
      GpioIo (Exclusive, PullDefault, , , IoRestrictionOutputOnly,
             "\\_SB.GPO0", , ResourceConsumer, ,
             )
          { // Pin list
            0x0023
          }
    })

    Method (_CRS, 0, NotSerialized) {                // _CRS: Current Resource Settings
      Return (RBUF)
    }

    Method (_STA, 0x0, NotSerialized) {
      //If (LNotEqual (OSSL, 1)) {                      // Android only
      //    Return (0xF)
      //}
      Return (0xF)
    }

  } // Device (VIBR)
}

