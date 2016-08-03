/** @file
  Platform Initialization Driver.

  Copyright (c) 1999 - 2015, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PlatformDxe.h"
#include "Platform.h"
#include <Protocol/PchPlatformPolicy.h>
#include <Protocol/CpuIo2.h>
#include <Protocol/ExitPmAuth.h>
#include <Library/S3BootScriptLib.h>
#include <Library/PciLib.h>
#include <Guid/EventGroup.h>
#include <Guid/Chv2Variable.h>
#include <Library/PcdLib.h>
#include <Include/Library/BiosIdLib.h>
#include <Protocol/IgdOpRegion.h>
#include <KscLib.h>
#include <Library/GpioLib.h>
#include <SaveMemoryConfig.h>

#define EFI_MEMORY_PORT_IO                0x4000000000000000ULL

#define WC_PMIC_READ_INSTRUCTION_DELAY 10
#define WC_PMIC_WRITE_INSTRUCTION_DELAY 100

SYSTEM_CONFIGURATION            mSystemConfiguration;
EFI_PLATFORM_INFO_HOB           mPlatformInfo;
EFI_EVENT                       mReadyToBootEvent;
UINTN                           SpiBase;


CHAR16
EFIAPI
InternalCharToUpper (
  IN      CHAR16                    Char
  );

BOOLEAN
EFIAPI
InternalIsHexaDecimalDigitCharacter (
  IN      CHAR16                    Char
  );

UINTN
EFIAPI
InternalHexCharToUintn (
  IN      CHAR16                    Char
  );

UINTN
EFIAPI
StrHex2Uintn (
  IN      CHAR16              *String
  )
{
  UINTN     Result;

  //
  // ASSERT String is less long than PcdMaximumUnicodeStringLength.
  // Length tests are performed inside StrLen().
  //
  ASSERT (StrSize (String) != 0);

  //
  // Ignore the pad spaces (space or tab)
  //
  while ((*String == L' ') || (*String == L'\t')) {
    String++;
  }

  //
  // Ignore leading Zeros after the spaces
  //
  while (*String == L'0') {
    String++;
  }

  if (InternalCharToUpper (*String) == L'X') {
    if (*(String - 1) != L'0') {
      return 0;
    }
    //
    // Skip the 'X'
    //
    String++;
  }

  Result = 0;

  while (InternalIsHexaDecimalDigitCharacter (*String)) {
    //
    // If the Hex Number represented by String overflows according
    // to the range defined by UINTN, then ASSERT().
    //
    ASSERT (Result <= ((((UINTN) ~0) - InternalHexCharToUintn (*String)) >> 4));

    Result = (Result << 4) + InternalHexCharToUintn (*String);
    String++;
  }

  return Result;
}

/**
  Read RTC content through its registers.

  @param  Address  Address offset of RTC. It is recommended to use macros such as
                   RTC_ADDRESS_SECONDS.

  @return The data of UINT8 type read from RTC.
**/
UINT8
RtcRead (
  IN  UINT8 Address
  )
{
  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, (UINT8) (Address | (UINT8) (IoRead8 (PCAT_RTC_ADDRESS_REGISTER) & 0x80)));
  return IoRead8 (PCAT_RTC_DATA_REGISTER);
}

/**
  Write RTC through its registers.

  @param  Address  Address offset of RTC. It is recommended to use macros such as
                   RTC_ADDRESS_SECONDS.
  @param  Data     The content you want to write into RTC.

**/
VOID
RtcWrite (
  IN  UINT8   Address,
  IN  UINT8   Data
  )
{
  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, (UINT8) (Address | (UINT8) (IoRead8 (PCAT_RTC_ADDRESS_REGISTER) & 0x80)));
  IoWrite8 (PCAT_RTC_DATA_REGISTER, Data);
}

VOID
CheckValidDate (
  VOID
  )
{
  EFI_TIME       Time;
  UINT8          Century;
  EFI_STATUS     Status;
  BIOS_ID_IMAGE  BiosIdImage;
  CHAR16         *TempString;

  TempString = NULL;
  //
  // Get the Time/Date values.
  //
  Time.Second  = RtcRead (RTC_ADDRESS_SECONDS);
  Time.Minute  = RtcRead (RTC_ADDRESS_MINUTES);
  Time.Hour    = RtcRead (RTC_ADDRESS_HOURS);
  Time.Day     = RtcRead (RTC_ADDRESS_DAY_OF_THE_MONTH);
  Time.Month   = RtcRead (RTC_ADDRESS_MONTH);
  Century      = RtcRead (RTC_ADDRESS_CENTURY);
  Time.Year    = (UINT16) ((Century * 0x100) + RtcRead (RTC_ADDRESS_YEAR));
  //
  // Perform date check and correction
  // Mark any date less than year 2005 or more than year 2050 as invalid
  //
  if ((Time.Year < 0x2005) || (Time.Year > 0x2050)) {
    DEBUG ((EFI_D_ERROR, "Invalid date, set date and time to match BIOS ID\n"));
    Status = GetBiosId (&BiosIdImage);
    if (Status == EFI_SUCCESS) {
      TempString = AllocateZeroPool (5);
      if (TempString != NULL) {
        //
        // Year
        //
        *(TempString + 0) = '2';
        *(TempString + 1) = '0';
        *(TempString + 2) = BiosIdImage.BiosIdString.TimeStamp[0];
        *(TempString + 3) = BiosIdImage.BiosIdString.TimeStamp[1];
        *(TempString + 4) = '\0';
        Time.Year = (UINT16) StrHex2Uintn (TempString);
        //
        // Month
        //
        *(TempString + 0) = BiosIdImage.BiosIdString.TimeStamp[2];
        *(TempString + 1) = BiosIdImage.BiosIdString.TimeStamp[3];
        *(TempString + 2) = '\0';
        Time.Month = (UINT8) StrHex2Uintn (TempString);
        //
        // Day
        //
        *(TempString + 0) = BiosIdImage.BiosIdString.TimeStamp[4];
        *(TempString + 1) = BiosIdImage.BiosIdString.TimeStamp[5];
        *(TempString + 2) = '\0';
        Time.Day = (UINT8) StrHex2Uintn (TempString);
        //
        // Hour
        //
        *(TempString + 0) = BiosIdImage.BiosIdString.TimeStamp[6];
        *(TempString + 1) = BiosIdImage.BiosIdString.TimeStamp[7];
        *(TempString + 2) = '\0';
        Time.Hour = (UINT8) StrHex2Uintn (TempString);
        //
        // Minute
        //
        *(TempString + 0) = BiosIdImage.BiosIdString.TimeStamp[8];
        *(TempString + 1) = BiosIdImage.BiosIdString.TimeStamp[9];
        *(TempString + 2) = '\0';
        Time.Minute = (UINT8) StrHex2Uintn (TempString);
        //
        // Program the correct date and time
        //
        RtcWrite (RTC_ADDRESS_SECONDS, Time.Second);
        RtcWrite (RTC_ADDRESS_MINUTES, Time.Minute);
        RtcWrite (RTC_ADDRESS_HOURS, Time.Hour);
        RtcWrite (RTC_ADDRESS_DAY_OF_THE_MONTH, Time.Day);
        RtcWrite (RTC_ADDRESS_MONTH, Time.Month);
        RtcWrite (RTC_ADDRESS_YEAR, (UINT8) Time.Year);
        RtcWrite (RTC_ADDRESS_CENTURY, Century);

        //
        // W/A for Hardware issue in case of CHT platform when CMOS is reset and time goes to 12hr mode
        // BIOS needs only 24hr mode
        //

        RtcWrite(RTC_ADDRESS_REGISTER_B, (RtcRead(RTC_ADDRESS_REGISTER_B) | BIT1 ));

        //
        // Free allocated TempString
        //
        FreePool (TempString);
      }
    }
  }
  DEBUG ((EFI_D_ERROR, "Date and time is now: %X/%X/%X %X:%X:%X\n", Time.Year, Time.Month, Time.Day, Time.Hour, Time.Minute, Time.Second));
}













