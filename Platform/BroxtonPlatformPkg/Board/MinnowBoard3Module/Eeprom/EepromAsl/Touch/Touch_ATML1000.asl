/** @file

Copyright (c) 2017 - 2018 Intel Corporation.

This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

Scope (\_SB.PCI0.I2C3)
{
  Device (TCS0)
  {
    Name (_ADR, 0x0)
    Name (_HID, "ATML0001")
    Name (_CID, "PNP0C50")
    Name (_S0W, 0)

    Method (_PS3, 0, Serialized) {
      If(LEqual (\_SB.IPC1.PMIC.AVBL, 1)){
//        Store(0x00, \_SB.IPC1.PMIC.VP6B)  // Tianma - 0 put the device to power saving state
        Store(0x00, \_SB.IPC1.PMIC.VP3B)  // Truly  - 0 put the device to power saving state

        If(LEqual (\_SB.IPC1.PMIC.AVBG, 1)){
//           Store(0x01, \_SB.IPC1.PMIC.GP6B)  // Tianma
           Store(0x01, \_SB.IPC1.PMIC.GP3B)  // Truly
        }
        Sleep(120)
      }
    }

    Method (_PS0, 0, Serialized) {
      If(LEqual (\_SB.IPC1.PMIC.AVBL, 1)){
//        Store(0x01, \_SB.IPC1.PMIC.VP6B) // Tianma
        Store(0x01, \_SB.IPC1.PMIC.VP3B) // Truly

        If(LEqual (\_SB.IPC1.PMIC.AVBG, 1)){
//          Store(0x01, \_SB.IPC1.PMIC.GP6B)   // Tianma - 1 put the device to normal state
          Store(0x01, \_SB.IPC1.PMIC.GP3B)   // Truly  - 1 put the device to normal state
        }
        Sleep(120)
      }
    }

    Name (XBUF, ResourceTemplate (){ // resource fot HR16 truly panel Windows
      I2CSerialBus(0x4A, //SlaveAddress: bus address 1386->1664
                       , //SlaveMode: default to ControllerInitiated
                 400000, //400000 ConnectionSpeed: in Hz
                       , //Addressing Mode: default to 7 bit
      "\\_SB.PCI0.I2C3", //ResourceSource: I2C bus controller name
                       , //Descriptor Name: creates name for offset of resource descriptor
                     ) //VendorData
      GpioInt(Edge,  ActiveLow, ExclusiveAndWake, PullUp, 0, "\\_SB.GPO0", ) {22}  // GPIO_22 TOUCH_INT
      GpioIo(Exclusive, PullDefault, 0, 0, IoRestrictionOutputOnly, "\\_SB.GPO2", ) {28} // GPIO_152 RESET
    })

    Method(_CRS, 0x0, NotSerialized){
      Return (XBUF)
    }

    Method(_DSM, 0x4, Serialized){
      // BreakPoint
      Store ("Method _DSM begin", Debug)

      If(LEqual(Arg0, ToUUID("3CDFF6F7-4267-4555-AD05-B30A3D8938DE")))
      {
        // DSM Function
        switch(ToInteger(Arg2))
        {
          // Function 0: Query function, return based on revision
          case(0)
          {
            // DSM Revision
            switch(ToInteger(Arg1))
            {
              // Revision 1: Function 1 supported
              case(1)
              {
                Store ("Method _DSM Function Query", Debug)
                Return(Buffer(One) { 0x03 })
              }

              default
              {
                // Revision 2+: no functions supported
                Return(Buffer(One) { 0x00 })
              }
            }
          }

          // Function 1 : HID Function
          case(1)
          {
            Store ("Method _DSM Function HID", Debug)

            // HID Descriptor Address
            Return(0x0000)
          }

          default
          {
            // Functions 2+: not supported
            Return(0x0000)
          }
        }
      }else{
        // No other GUIDs supported
        Return(Buffer(One) { 0x00 })
      }
    }
    Method(_STA, 0x0, NotSerialized){
    If (LEqual(VTKB,2)){ // hide discrete touch when integrated touch is enabled
       Return(0x0)
    }
      Return(0xF)
    }
  }
}

