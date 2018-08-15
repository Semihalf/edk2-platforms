/** @file

  This application will read/write/verify/dump the EERPOM data.

  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials are licensed and made available under
  the terms and conditions of the BSD License which accompanies this distribution.
  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "EepromApp.h"

BOOLEAN           gDebugFlag = FALSE;

/**
  UEFI application entry point which has an interface similar to a
  standard C main function.

  The ShellCEntryLib library instance wrappers the actual UEFI application
  entry point and calls this ShellAppMain function.

  @param[in] Argc     The number of items in Argv.
  @param[in] Argv     Array of pointers to strings.

  @retval    0        The application exited normally.
  @retval    Other    An error occurred.

**/
INTN
EFIAPI
ShellAppMain (
  IN UINTN      Argc,
  IN CHAR16   **Argv
  )
{
  BOOLEAN         InvalidFlag;
  PROGRAM_INFO    ProgramInfo;
  SHELL_STATUS    Status;

  if (gDebugFlag) Print (L"%a(#%d) - Starting...\n", __FUNCTION__, __LINE__);

  // Initialize Variables
  InvalidFlag = FALSE;
  Status      = SHELL_SUCCESS;
  ZeroMem (&ProgramInfo, sizeof (ProgramInfo));


  // Check for invalid conditions
  if (Argv == NULL) {
    if (gDebugFlag) Print (L"ERROR: [#%d] Internal coding error!\n", __LINE__);
    Status = SHELL_INVALID_PARAMETER;
    goto Exit;
  }

  // Parse CLI parameters
  ParseParameters (Argc, Argv, &ProgramInfo);

  // Print App version
  Print (L"EepromApp - Version #%s\n", APP_VERSION);
  Print (L"Copyright (c) %s Intel Corporation. All rights reserved.\n\n", APP_COPYRIGHT);

  // Help requested?
  if (ProgramInfo.HelpFlag) {
    PrintHelp ();
    goto Exit;
  }

  // Check for invalid states
  if (CheckForInvalidOptions (&ProgramInfo) != SHELL_SUCCESS) {
    InvalidFlag = TRUE;
    Print (L"ERROR: Invalid CLI option(s)!\n");
    goto Exit;
  }

  //
  // Test GetValidEepromLibrary()
  //
  if (!ProgramInfo.ScanFlag) {
    //
    // Skip this if we are doing a scan.
    //
    Print (L"- GetValidEepromLibrary() = %a\n", mEepromLibraryString[GetValidEepromLibrary (TRUE)]);
  } else {
    // Scanning
    Status = ScanOption (&ProgramInfo);
    if (Status != SHELL_SUCCESS) {
      goto Exit;
    }
  }

  // Check?
  if (ProgramInfo.CheckFlag) {
    if (ProgramInfo.ReadLibraryIndex > EEPROM_DATA_LIBRARY_INDEX_MAX) {
      Print (L"ERROR: Library index you want to check is larger than %d!\n", EEPROM_DATA_LIBRARY_INDEX_MAX);
      Status = SHELL_INVALID_PARAMETER;
      goto Exit;
    }
    Status = CheckOption (&ProgramInfo);
  } else

  // Dump human?
  if (ProgramInfo.DumpHumanFlag) {
    if (ProgramInfo.DumpHumanLibraryIndex > EEPROM_DATA_LIBRARY_INDEX_MAX) {
      Print (L"ERROR: Library index you want to check is larger than %d!\n", EEPROM_DATA_LIBRARY_INDEX_MAX);
      Status = SHELL_INVALID_PARAMETER;
      goto Exit;
    }
    Status = DumpHumanOption (&ProgramInfo);
  } else

  // Dump raw?
  if (ProgramInfo.DumpRawFlag) {
    if (ProgramInfo.DumpRawLibraryIndex > EEPROM_DATA_LIBRARY_INDEX_MAX) {
      Print (L"ERROR: Library index you want to check is larger than %d!\n", EEPROM_DATA_LIBRARY_INDEX_MAX);
      Status = SHELL_INVALID_PARAMETER;
      goto Exit;
    }
    Status = DumpRawOption (&ProgramInfo);
  } else

  // Erase?
  if (ProgramInfo.EraseFlag) {
    if (ProgramInfo.EraseLibraryIndex > EEPROM_DATA_LIBRARY_INDEX_MAX) {
      Print (L"ERROR: Library index you want to check is larger than %d!\n", EEPROM_DATA_LIBRARY_INDEX_MAX);
      Status = SHELL_INVALID_PARAMETER;
      goto Exit;
    }
    Status = EraseOption (&ProgramInfo);
  } else

  // Read?
  if (ProgramInfo.ReadFlag) {
    if (ProgramInfo.ReadLibraryIndex > EEPROM_DATA_LIBRARY_INDEX_MAX) {
      Print (L"ERROR: Library index you want to read is larger than %d!\n", EEPROM_DATA_LIBRARY_INDEX_MAX);
      Status = SHELL_INVALID_PARAMETER;
      goto Exit;
    }
    if (ProgramInfo.ReadFileName == NULL) {
      Print (L"ERROR: Missing the filename!\n");
      Status = SHELL_INVALID_PARAMETER;
      goto Exit;
    }
    Status = ReadOption (&ProgramInfo);
  } else

  // Verify?
  if (ProgramInfo.VerifyFlag) {
    if (ProgramInfo.VerifyLibraryIndex > EEPROM_DATA_LIBRARY_INDEX_MAX) {
      Print (L"ERROR: Library index you want to verify is larger than %d!\n", EEPROM_DATA_LIBRARY_INDEX_MAX);
      Status = SHELL_INVALID_PARAMETER;
      goto Exit;
    }
    if (ProgramInfo.VerifyFileName == NULL) {
      Print (L"ERROR: Missing the filename!\n");
      Status = SHELL_INVALID_PARAMETER;
      goto Exit;
    }
    Status = VerifyOption (&ProgramInfo);
  } else

  // Write?
  if (ProgramInfo.WriteFlag) {
    if (ProgramInfo.WriteLibraryIndex > EEPROM_DATA_LIBRARY_INDEX_MAX) {
      Print (L"ERROR: Library index you want to write is larger than %d!\n", EEPROM_DATA_LIBRARY_INDEX_MAX);
      Status = SHELL_INVALID_PARAMETER;
      goto Exit;
    }
    if (ProgramInfo.WriteFileName == NULL) {
      Print (L"ERROR: Missing the filename!\n");
      Status = SHELL_INVALID_PARAMETER;
      goto Exit;
    }
    Status = WriteOption (&ProgramInfo);
  } else

  // This is invalid
  {
    InvalidFlag = TRUE;
    Print (L"ERROR: Invalid CLI option(s)!\n");
  }

Exit:
  // Invalid?
  if (InvalidFlag) {
    Status = SHELL_INVALID_PARAMETER;
  }

  //
  // Free resources
  //
  if (ProgramInfo.CheckFile != NULL) {
    FreePool (ProgramInfo.CheckFile);
    ProgramInfo.CheckFile = NULL;
  }
  if (ProgramInfo.DumpHumanFile != NULL) {
    FreePool (ProgramInfo.DumpHumanFile);
    ProgramInfo.DumpHumanFile = NULL;
  }
  if (ProgramInfo.DumpRawFile != NULL) {
    FreePool (ProgramInfo.DumpRawFile);
    ProgramInfo.DumpRawFile = NULL;
  }
  if (ProgramInfo.ReadFile != NULL) {
    FreePool (ProgramInfo.ReadFile);
    ProgramInfo.ReadFile = NULL;
  }
  if (ProgramInfo.VerifyFile != NULL) {
    FreePool (ProgramInfo.VerifyFile);
    ProgramInfo.VerifyFile = NULL;
  }
  if (ProgramInfo.WriteFile != NULL) {
    FreePool (ProgramInfo.WriteFile);
    ProgramInfo.WriteFile = NULL;
  }

  return Status;
}

SHELL_STATUS
BufferToMemory (
  IN  UINT8     *Buffer,
  IN  UINT32     BufferSize,
  OUT UINT8    **MemoryBuffer,
  OUT UINT32    *MemoryBufferSize
  )
{
  EFI_STATUS      EfiStatus;
  UINT8          *ImageBuffer;
  UINT32          ImageSize;
  SHELL_STATUS    Status;

  if (gDebugFlag) Print (L"%a(#%d) - Starting...\n", __FUNCTION__, __LINE__);

  //
  // Initialize variables
  //
  ImageBuffer = NULL;
  ImageSize   = 0;
  Status      = SHELL_SUCCESS;

  //
  // Sanity checks
  //
  if ((Buffer == NULL) || (BufferSize == 0)) {
    if (gDebugFlag) Print (L"ERROR: [#%d] Internal coding error!\n", __LINE__);
    Status = SHELL_INVALID_PARAMETER;
    goto Exit;
  }
  if ((MemoryBuffer == NULL) || (MemoryBufferSize == NULL)) {
    if (gDebugFlag) Print (L"ERROR: [#%d] Internal coding error!\n", __LINE__);
    Status = SHELL_INVALID_PARAMETER;
    goto Exit;
  }

  //
  // Find out the size of the image
  //
  ImageSize = GetImageSize (EEPROM_MEMORY);
  if (ImageSize > 0) {
    //
    // Something there to read
    //
    ImageBuffer = AllocateZeroPool (ImageSize);
    if (ImageBuffer == NULL) {
      Print (L"ERROR: [#%d] Failed to allocate buffer!\n", __LINE__);
      Status = SHELL_OUT_OF_RESOURCES;
      goto Exit;
    }

    //
    // Read in the memory image
    //
    EfiStatus = ReadEeprom (EEPROM_MEMORY, 0, &ImageSize, ImageBuffer, NULL);
    if (EFI_ERROR (EfiStatus)) {
      Print (L"ERROR: [#%d] Failed to read image (%r)!\n", __LINE__, EfiStatus);
      Status = SHELL_DEVICE_ERROR;
      goto Exit;
    }
  }

  //
  // Write out file image to memory
  //
  EfiStatus = WriteEeprom (EEPROM_MEMORY, 0, &BufferSize, Buffer, NULL);
  if (EFI_ERROR (EfiStatus)) {
    Print (L"ERROR: [#%d] Failed to write image (%r)!\n", __LINE__, EfiStatus);
    Status = SHELL_DEVICE_ERROR;
    goto Exit;
  }

  //
  // Send out info
  //
  *MemoryBuffer     = ImageBuffer;
  *MemoryBufferSize = ImageSize;

Exit:
  if (EFI_ERROR (EfiStatus)) {
    if (ImageBuffer != NULL) {
      FreePool (ImageBuffer);
      ImageBuffer = NULL;
    }
  }
  return Status;
}

SHELL_STATUS
CheckForInvalidOptions (
  IN PROGRAM_INFO    *ProgramInfo
  )
{
  BOOLEAN         InvalidFlag;
  SHELL_STATUS    Status;

  if (gDebugFlag) Print (L"%a(#%d) - Starting...\n", __FUNCTION__, __LINE__);

  //
  // Initialize variables
  //
  InvalidFlag = FALSE;
  Status      = SHELL_SUCCESS;

  //
  // Sanity checks
  //
  if (ProgramInfo == NULL) {
    if (gDebugFlag) Print (L"ERROR: [#%d] Internal coding error!\n", __LINE__);
    Status = SHELL_INVALID_PARAMETER;
    goto Exit;
  }

  //
  // Dump ProgramInfo for debugging
  //
  if (gDebugFlag) DumpProgramInfo (ProgramInfo);

  //
  // Check for invalid options
  //
  if ((ProgramInfo->DumpRawFlag || ProgramInfo->DumpHumanFlag) && (ProgramInfo->ReadFlag)) {
    InvalidFlag = TRUE;
    Print (L"ERROR: You can't dump and read at the same time!\n");
  } else

  if ((ProgramInfo->DumpRawFlag || ProgramInfo->DumpHumanFlag) && (ProgramInfo->VerifyFlag)) {
    InvalidFlag = TRUE;
    Print (L"ERROR: You can't dump and verify at the same time!\n");
  } else

  if ((ProgramInfo->DumpRawFlag || ProgramInfo->DumpHumanFlag) && (ProgramInfo->WriteFlag)) {
    InvalidFlag = TRUE;
    Print (L"ERROR: You can't dump and write at the same time!\n");
  } else

  if (ProgramInfo->CheckFlag && ProgramInfo->ReadFlag) {
    InvalidFlag = TRUE;
    Print (L"ERROR: You can't check and read at the same time!\n");
  } else

  if (ProgramInfo->CheckFlag && ProgramInfo->VerifyFlag) {
    InvalidFlag = TRUE;
    Print (L"ERROR: You can't check and verify at the same time!\n");
  } else

  if (ProgramInfo->CheckFlag && ProgramInfo->WriteFlag) {
    InvalidFlag = TRUE;
    Print (L"ERROR: You can't check and write at the same time!\n");
  } else

  if (ProgramInfo->ReadFlag && ProgramInfo->VerifyFlag) {
    InvalidFlag = TRUE;
    Print (L"ERROR: You can't read and verify at the same time!\n");
  } else

  if (ProgramInfo->ReadFlag && ProgramInfo->WriteFlag) {
    InvalidFlag = TRUE;
    Print (L"ERROR: You can't read and write at the same time!\n");
  } else

  if (ProgramInfo->VerifyFlag && ProgramInfo->WriteFlag) {
    InvalidFlag = TRUE;
    Print (L"ERROR: You can't verify and write at the same time!\n");
  } else

  if (ProgramInfo->CheckFlag && (ProgramInfo->CheckLibraryIndex == EEPROM_EEPROM) &&
      (!(ProgramInfo->BusFlag && ProgramInfo->AddressFlag) && !ProgramInfo->ScanFlag)) {
    InvalidFlag = TRUE;
    Print (L"ERROR: You can't check the security of the image in the EEPROM without specifying the EEPROM bus & address!\n");
  } else

  if (ProgramInfo->ReadFlag && (ProgramInfo->ReadLibraryIndex == EEPROM_EEPROM) &&
      (!(ProgramInfo->BusFlag && ProgramInfo->AddressFlag) && !ProgramInfo->ScanFlag)) {
    InvalidFlag = TRUE;
    Print (L"ERROR: You can't read the image from the EEPROM without specifying the EEPROM bus & address!\n");
  } else

  if (ProgramInfo->VerifyFlag && (ProgramInfo->VerifyLibraryIndex == EEPROM_EEPROM) &&
      (!(ProgramInfo->BusFlag && ProgramInfo->AddressFlag) && !ProgramInfo->ScanFlag)) {
    InvalidFlag = TRUE;
    Print (L"ERROR: You can't verify the image in the EEPROM without specifying the EEPROM bus & address!\n");
  } else

  if (ProgramInfo->WriteFlag && (ProgramInfo->VerifyLibraryIndex == EEPROM_EEPROM) &&
      (!(ProgramInfo->BusFlag && ProgramInfo->AddressFlag) && !ProgramInfo->ScanFlag)) {
    InvalidFlag = TRUE;
    Print (L"ERROR: You can't write the image in the EEPROM without specifying the EEPROM bus & address!\n");
  } else

  if (ProgramInfo->AddressFlag) {
    if (ProgramInfo->AddressValue > 0x7F) {
      Print (L"ERROR: I2C slave address is too large!\n");
      Status = SHELL_INVALID_PARAMETER;
      goto Exit;
    }
  } else

  if (ProgramInfo->BusFlag) {
    if (ProgramInfo->BusValue > 7) {
      Print (L"ERROR: I2C bus number is too large!\n");
      Status = SHELL_INVALID_PARAMETER;
      goto Exit;
    }
  }

  //
  // Set return value
  //
  if (InvalidFlag) {
    Status = SHELL_INVALID_PARAMETER;
  }

Exit:
  return Status;
}

SHELL_STATUS
CheckOption (
  IN PROGRAM_INFO    *ProgramInfo
  )
{
  EFI_STATUS              EfiStatus;
  UINT8                  *ImageBuffer;
  UINT32                  ImageSize;
  SHELL_STATUS            Status;

  if (gDebugFlag) Print (L"%a(#%d) - Starting...\n", __FUNCTION__, __LINE__);

  //
  // Initialize variables
  //
  ImageBuffer = NULL;
  Status      = SHELL_SUCCESS;

  //
  // Sanity checks
  //
  if (ProgramInfo == NULL) {
    if (gDebugFlag) Print (L"ERROR: [#%d] Internal coding error!\n", __LINE__);
    Status = SHELL_INVALID_PARAMETER;
    goto Exit;
  }
  if (ProgramInfo->CheckFileName != NULL) {
    if ((ProgramInfo->CheckFile == NULL) || (ProgramInfo->CheckFileSize == 0)) {
      if (gDebugFlag) Print (L"ERROR: [#%d] Image file is empty!\n", __LINE__);
      Status = SHELL_INVALID_PARAMETER;
      goto Exit;
    }
  }

  //
  // Checking security of file image
  //
  Print (L"  - Validating...\n");
  if (ProgramInfo->CheckFileName != NULL) {
    //
    // Swap File buffer with EEPROM_MEMORY
    //
    Status = BufferToMemory (ProgramInfo->CheckFile, ProgramInfo->CheckFileSize, &ImageBuffer, &ImageSize);
    if (Status != SHELL_SUCCESS) {
      Print (L"ERROR: [#%d] Failed to retrieve original memory image (%r)!\n", __LINE__, Status);
      Status = SHELL_DEVICE_ERROR;
      goto Exit;
    }

    //
    // Validate image
    //
    EfiStatus = ValidateEeprom (EEPROM_MEMORY);
    Print (L"  - File validation status = %r\n", EfiStatus);

    //
    // Was there something in memory before?
    //
    if (ImageBuffer != NULL) {
      //
      // Restore memory image
      //
      EfiStatus = WriteEeprom (EEPROM_MEMORY, 0, &ImageSize, ImageBuffer, NULL);
      if (EFI_ERROR (EfiStatus)) {
        Print (L"ERROR: [#%d] Failed to restore original memory image (%r)!\n", __LINE__, EfiStatus);
        Status = SHELL_DEVICE_ERROR;
        goto Exit;
      }

      //
      // Revalidate the memory image
      //
      EfiStatus = ValidateEeprom (EEPROM_MEMORY);
      if (EFI_ERROR (EfiStatus)) {
        Print (L"ERROR: [#%d] Failed to validate original memory image (%r)!\n", __LINE__, EfiStatus);
        Status = SHELL_DEVICE_ERROR;
        goto Exit;
      }
    }
  } else

  //
  // Just validate the current image
  //
  {
    EfiStatus = ValidateEeprom (ProgramInfo->CheckLibraryIndex);
    Print (L"  - Image validation status = %r\n", EfiStatus);
  }

  Status = SHELL_SUCCESS;

Exit:
  //
  // Free resources
  //
  if (ImageBuffer != NULL) {
    FreePool (ImageBuffer);
    ImageBuffer = NULL;
  }
  return Status;
}

SHELL_STATUS
DumpHumanOption (
  IN PROGRAM_INFO    *ProgramInfo
  )
{
  CHAR8                   AsciiData[32];
  VOID                   *Buffer;
  UINT32                  BufferSize;
  UINT32                  count;
  EEPROM_FUNCTION_INFO    EepromInfo;
  EFI_STATUS              EfiStatus;
  VOID                   *FunctionInfo;
  GENERIC_HEADER         *GenericHeader;
  UINT8                  *Hash;
  UINT32                  HashSize;
  UINT8                  *ImageBuffer;
  UINT32                  ImageSize;
  UINT32                  index;
  UINT8                   LibraryIndex;
  UINT8                  *Ptr;
  UINT8                  *Signature;
  UINT32                  SignatureSize;
  SHELL_STATUS            Status;
  UINT32                  StructureSize;

  if (gDebugFlag) Print (L"%a(#%d) - Starting...\n", __FUNCTION__, __LINE__);

  //
  // Initialize variables
  //
  ZeroMem (&EepromInfo, sizeof (EEPROM_FUNCTION_INFO));
  EepromInfo.Bus          = ProgramInfo->BusValue;
  EepromInfo.Address      = ProgramInfo->AddressValue;
  EepromInfo.LibraryIndex = EEPROM_EEPROM;
  Buffer                  = NULL;
  FunctionInfo            = NULL;
  Hash                    = NULL;
  HashSize                = 0;
  ImageBuffer             = NULL;
  LibraryIndex            = ProgramInfo->DumpHumanLibraryIndex;
  Signature               = NULL;
  SignatureSize           = 0;
  Status                  = SHELL_SUCCESS;

  //
  // Sanity checks
  //
  if (ProgramInfo == NULL) {
    if (gDebugFlag) Print (L"ERROR: [#%d] Internal coding error!\n", __LINE__);
    Status = SHELL_INVALID_PARAMETER;
    goto Exit;
  }
  if (ProgramInfo->DumpHumanFileName != NULL) {
    if ((ProgramInfo->DumpHumanFile == NULL) || (ProgramInfo->DumpHumanFileSize == 0)) {
      if (gDebugFlag) Print (L"ERROR: [#%d] Image file is empty!\n", __LINE__);
      Status = SHELL_INVALID_PARAMETER;
      goto Exit;
    }
  }

  if (ProgramInfo->DumpHumanFileName != NULL) {
    //
    // Swap File buffer with EEPROM_MEMORY
    //
    Status = BufferToMemory (ProgramInfo->DumpHumanFile, ProgramInfo->DumpHumanFileSize, &ImageBuffer, &ImageSize);
    if (Status != SHELL_SUCCESS) {
      Print (L"ERROR: [#%d] Failed to retrieve original memory image (%r)!\n", __LINE__, Status);
      Status = SHELL_DEVICE_ERROR;
      goto Exit;
    }

    //
    // Point to EEPROM_MEMORY library
    //
    LibraryIndex = EEPROM_MEMORY;
  }

  //
  // Parse the image into human readable form
  //
  index = 0;
  EfiStatus = EFI_SUCCESS;
  while (!EFI_ERROR (EfiStatus)) {
    EfiStatus = GetNextEepromStructure (LibraryIndex, &index, (UINT8 **) &Buffer, &BufferSize);
    if (!EFI_ERROR (EfiStatus)) {
      //
      // Got a structure
      //
      GenericHeader = (GENERIC_HEADER *) Buffer;
      CopyMem (AsciiData, GenericHeader->signature, sizeof (GenericHeader->signature));
      AsciiData[sizeof (GenericHeader->signature)] = 0;
      StructureSize = GenericHeader->length;
      if (GenericHeader->signature[0] != '$') {
        Print (L"ERROR: [#%d] Structure signature [%a] doesn't start with $!\n", __LINE__, AsciiData);
        EfiStatus = EFI_COMPROMISED_DATA;
        continue;
      }
      //
      // Dump generic header info
      //
      Print (L"Signature           = %a\n",   AsciiData);
      Print (L"Major version       = %04x\n", GenericHeader->vermajor);
      Print (L"Minor version       = %04x\n", GenericHeader->verminor);
      Print (L"Length              = %08x\n", StructureSize);
      //
      // Check for $AcpiTbl
      //
      if (AsciiStrnCmp (GenericHeader->signature, EEPROM_ACPI_TABLE_SIGNATURE, 8) == 0) {
        ACPI_TABLE   *AcpiTable;
        AcpiTable = (ACPI_TABLE *) Buffer;
        Ptr       = (UINT8 *) Buffer + sizeof (ACPI_TABLE);
        if (ProgramInfo->VerboseFlag) DumpParagraph (Ptr, (AcpiTable->length - (Ptr - (UINT8 *) Buffer)));
      } else
      //
      // Check for $BrdInfo
      //
      if (AsciiStrnCmp (GenericHeader->signature, EEPROM_BOARD_INFO_SIGNATURE, 8) == 0) {
        BOARD_INFO_TABLE   *BoardInfo;
        BoardInfo = (BOARD_INFO_TABLE *) Buffer;
        ZeroMem (AsciiData, 17);
        CopyMem (AsciiData, BoardInfo->manuname, 16);
        Print (L"Manufacturing name  = %a\n", AsciiData);
        ZeroMem (AsciiData, 17);
        CopyMem (AsciiData, BoardInfo->brdname, 16);
        Print (L"Board name          = %a\n", AsciiData);
        ZeroMem (AsciiData, 17);
        CopyMem (AsciiData, BoardInfo->brdserial, 16);
        Print (L"Board serial #      = %a\n",   AsciiData);
        Print (L"Board ID            = %08x\n", BoardInfo->boardid);
        Print (L"Fab ID              = %08x\n", BoardInfo->fabid);
        Print (L"EC ID               = %08x\n", BoardInfo->ecid);
        Print (L"Board type          = ");
        if (BoardInfo->boardtype == EEPROM_BOARD_TYPE_REDBOX) {
          Print (L"Main board (RedBox)\n");
        } else
        if (BoardInfo->boardtype == EEPROM_BOARD_TYPE_PLUGIN) {
          Print (L"Plug-in board\n");
        } else
        {
          Print (L"[%02x] - Unknown\n", BoardInfo->boardtype);
        }
      } else
      //
      // Check for $EeprMap
      //
      if (AsciiStrnCmp (GenericHeader->signature, EEPROM_MAP_SIGNATURE, 8) == 0) {
        EEPROM_MAP          *EepromMap;
        EEPROM_MAP_RECORD   *MapRecord;
        EepromMap = (EEPROM_MAP *) Buffer;
        MapRecord = (EEPROM_MAP_RECORD *) ((UINT8 *) Buffer + sizeof (EEPROM_MAP));
        ZeroMem (AsciiData, 17);
        CopyMem (AsciiData, EepromMap->partlabel, 16);
        Print (L"Part label          = %a\n",   AsciiData);
        Print (L"Block length        = %08x\n", EepromMap->blklength);
        Print (L"Page size           = %04x\n", EepromMap->pagesize);
        Print (L"Total part size     = %08x\n", EepromMap->partsize);
        Print (L"I2C bus number      = %02x\n", EepromMap->busnumber);
        Print (L"Map master          = ");
        if (EepromMap->master == 0) {
          Print (L"Not the master map\n");
        } else
        if (EepromMap->master == 1) {
          Print (L"Master map\n");
        } else
        {
          Print (L"[%02x] - Unknown\n", EepromMap->master);
        }
        Print (L"I2C speed           = ");
        if (EepromMap->speed == EEPROM_SPEED_STANDARD) {
          Print (L"100KHz (Standard speed)\n");
        } else
        if (EepromMap->speed == EEPROM_SPEED_FAST) {
          Print (L"400KHz (Fast speed)\n");
        } else
        if (EepromMap->speed == EEPROM_SPEED_HIGH) {
          Print (L"3.4MHz (High speed)\n");
        } else
        {
          Print (L"[%02x] - Unknown\n", EepromMap->speed);
        }
        //
        // Loop thru map records
        //
        while ((UINT8 *) MapRecord < ((UINT8 *) Buffer + EepromMap->length)) {
          ZeroMem (AsciiData, 17);
          CopyMem (AsciiData, MapRecord->maplabel, 16);
          Print (L" - Record label     = %a\n",   AsciiData);
          Print (L"   - Record length  = %08x\n", MapRecord->length);
          Print (L"   - Binary offset  = %08x\n", MapRecord->offset);
          Print (L"   - I2C address    = %02x\n", MapRecord->address);
          //
          // Increment record
          //
          MapRecord++;
        }
      } else
      //
      // Check for $Eeprom$ structure
      //
      if (AsciiStrnCmp (GenericHeader->signature, EEPROM_HEADER_SIGNATURE, 8) == 0) {
        EEPROM_HEADER   *EepromHeader;
        EepromHeader = (EEPROM_HEADER *) Buffer;
        Print (L"Image length        = %08x\n", EepromHeader->structlength);
        Print (L"Image CRC32         = %08x\n", EepromHeader->crc32);
        Print (L"Image CRC32 length  = %08x\n", EepromHeader->crclength);
        Print (L"Image version       = %08x\n", EepromHeader->version);
      } else
      //
      // Check for $GpioDat
      //
      if (AsciiStrnCmp (GenericHeader->signature, EEPROM_GPIO_SIGNATURE, 8) == 0) {
        GPIO_DATA_HEADER   *GpioHeader;
        GPIO_DATA_RECORD   *GpioRecord;
        GpioHeader = (GPIO_DATA_HEADER *) Buffer;
        GpioRecord = (GPIO_DATA_RECORD *)  ((UINT8 *) Buffer + sizeof (GPIO_DATA_HEADER));
        //
        // Loop thru GPIO records
        //
        while ((UINT8 *) GpioRecord < ((UINT8 *) Buffer + GpioHeader->length)) {
          ZeroMem (AsciiData, 17);
          CopyMem (AsciiData, GpioRecord->gpiolabel, 16);
          Print (L" - GPIO label       = %a\n",   AsciiData);
          Print (L"   - Record length  = %08x\n", GpioRecord->length);
          Print (L"   - GPIO offset    = %08x\n", GpioRecord->offset);
          Print (L"   - GPIO AND data  = %08x\n", GpioRecord->anddata);
          Print (L"   - GPIO OR data   = %08x\n", GpioRecord->ordata);
          Print (L"   - GPIO data size = %08x\n", GpioRecord->datasize);
          Print (L"   - GPIO data type = ");
          if (GpioRecord->datasize == EEPROM_GPIO_TYPE_IO) {
            Print (L"IO\n");
          } else
          if (GpioRecord->datasize == EEPROM_GPIO_TYPE_MMIO) {
            Print (L"MMIO\n");
          } else
          if (GpioRecord->datasize == EEPROM_GPIO_TYPE_PCI) {
            Print (L"PCI\n");
          } else
          if (GpioRecord->datasize == EEPROM_GPIO_TYPE_PCIE) {
            Print (L"PCIe\n");
          } else
          if (GpioRecord->datasize == EEPROM_GPIO_TYPE_PAD_OFFSET) {
            Print (L"PAD offset\n");
          } else
          {
            Print (L"[%02x] - Unknown\n", GpioRecord->datasize);
          }
          //
          // Increment record
          //
          GpioRecord++;
        }
      } else
      //
      // Check for $HdCodec
      //
      if (AsciiStrnCmp (GenericHeader->signature, EEPROM_HDA_CODEC_SIGNATURE, 8) == 0) {
        HDA_CODEC   *HdaCodec;
        HdaCodec = (HDA_CODEC *) Buffer;
        Ptr      = (UINT8 *) Buffer + sizeof (HDA_CODEC);
        if (ProgramInfo->VerboseFlag) DumpParagraph (Ptr, (HdaCodec->length - (Ptr - (UINT8 *) Buffer)));
      } else
      //
      // Check for $Logo$
      //
      if (AsciiStrnCmp (GenericHeader->signature, EEPROM_LOGO_DATA_SIGNATURE, 6) == 0) {
        LOGO_DATA   *LogoData;
        LogoData = (LOGO_DATA *) Buffer;
        Ptr      = (UINT8 *) Buffer + sizeof (LOGO_DATA);
        if (ProgramInfo->VerboseFlag) DumpParagraph (Ptr, (LogoData->length - (Ptr - (UINT8 *) Buffer)));
      } else
      //
      // Check for $MacInfo
      //
      if (AsciiStrnCmp (GenericHeader->signature, EEPROM_NIC_INFO_SIGNATURE, 8) == 0) {
        NIC_INFO   *NicInfo;
        NicInfo = (NIC_INFO *) Buffer;
        ZeroMem (AsciiData, 9);
        CopyMem (AsciiData, NicInfo->nicid, 8);
        Print (L"NIC ID              = %a\n",   AsciiData);
        Print (L"MAC address         = %12x\n", NicInfo->macaddr);
        Print (L"NIC number          = %2x\n",  NicInfo->nicnum);
        Ptr = (UINT8 *) Buffer + sizeof (NIC_INFO);
        if (ProgramInfo->VerboseFlag) DumpParagraph (Ptr, (NicInfo->length - (Ptr - (UINT8 *) Buffer)));
      } else
      //
      // Check for $MemCnfg
      //
      if (AsciiStrnCmp (GenericHeader->signature, EEPROM_MEMORY_DATA_SIGNATURE, 8) == 0) {
        UINT8          Slot;
        MEMORY_DATA   *MemoryData;
        MemoryData = (MEMORY_DATA *) Buffer;
        Print (L"SPD slot flags      = %04x\n", MemoryData->spdslot);
        if (MemoryData->spdslot == 0) {
          Print (L"  - SMIP binary.\n");
        } else {
          for (Slot = 0; Slot < 16; Slot++) {
            if (MemoryData->spdslot & (1 << Slot)) {
              Print (L"  - Slot %d support.\n", Slot + 1);
            }
          }
        }
        Ptr = (UINT8 *) Buffer + sizeof (MEMORY_DATA);
        if (ProgramInfo->VerboseFlag) DumpParagraph (Ptr, (MemoryData->length - (Ptr - (UINT8 *) Buffer)));
      } else
      //
      // Check for $PromSig
      //
      if (AsciiStrnCmp (GenericHeader->signature, EEPROM_SIGNATURE_SIGNATURE, 8) == 0) {
        UINT16            HashType;
        SIGNATURE_DATA   *SignatureData;
        SignatureData = (SIGNATURE_DATA *) Buffer;
        Hash          = (UINT8 *) Buffer + sizeof (SIGNATURE_DATA);
        HashType = (SignatureData->hashtype & 0x7FFF);
        Print (L"Hash type           = ");
        if (HashType == EEPROM_SIGNATURE_TYPE_NONE) {
          Print (L"None\n");
          HashSize = 0;
        } else
        if (HashType == EEPROM_SIGNATURE_TYPE_MD5) {
          Print (L"MD5\n");
          HashSize = MD5DigestSize;
        } else
        if (HashType == EEPROM_SIGNATURE_TYPE_SHA1) {
          Print (L"SHA1\n");
          HashSize = SHA1DigestSize;
        } else
        if (HashType == EEPROM_SIGNATURE_TYPE_SHA256) {
          Print (L"SHA256\n");
          HashSize = SHA256DigestSize;
        } else
        if (HashType == EEPROM_SIGNATURE_TYPE_SHA384) {
          Print (L"SHA384\n");
          HashSize = SHA384DigestSize;
        } else
        if (HashType == EEPROM_SIGNATURE_TYPE_SHA512) {
          Print (L"SHA512\n");
          HashSize = SHA512DigestSize;
        } else
        {
          Print (L"[%04x] Unknown\n", HashType);
        }
        Print (L"Hash signed         = ");
        if (HashType != SignatureData->hashtype) {
          Print (L"True\n");
          Signature     = Hash + HashSize;
          SignatureSize = 256;
        } else {
          Print (L"False\n");
          Signature     = NULL;
          SignatureSize = 0;
        }
        // Dump hash
        Print (L"Hash                = ");
        for (count = 0; count < HashSize; count++) {
          if (((count % 0x10) == 0) && (count > 0)) {
            Print (L"\n                      ");
          }
          Print (L"%02X", Hash[count]);
        }
        Print (L"\n");
        // Dump signed hash
        if (HashType != SignatureData->hashtype) {
          Print (L"Signed Hash         = ");
          for (count = 0; count < SignatureSize; count++) {
            if (((count % 0x10) == 0) && (count > 0)) {
              Print (L"\n                      ");
            }
            Print (L"%02X", Signature[count]);
          }
          Print (L"\n");
        }
      } else
      //
      // Check for $uCode$
      //
      if (AsciiStrnCmp (GenericHeader->signature, EEPROM_MICROCODE_SIGNATURE, 7) == 0) {
        MICROCODE   *MicrocodeData;
        MicrocodeData = (MICROCODE *) Buffer;
        Ptr      = (UINT8 *) Buffer + sizeof (MICROCODE);
        if (ProgramInfo->VerboseFlag) DumpParagraph (Ptr, (MicrocodeData->length - (Ptr - (UINT8 *) Buffer)));
      } else
      //
      // Check for $Video$
      //
      if (AsciiStrnCmp (GenericHeader->signature, EEPROM_VIDEO_DATA_SIGNATURE, 7) == 0) {
        VIDEO_DATA   *VideoData;
        VideoData = (VIDEO_DATA *) Buffer;
        Ptr       = (UINT8 *) Buffer + sizeof (VIDEO_DATA);
        if (ProgramInfo->VerboseFlag) DumpParagraph (Ptr, (VideoData->length - (Ptr - (UINT8 *) Buffer)));
      } else
      //
      // Unknown structure
      //
      {
        Ptr = (UINT8 *) Buffer + sizeof (GENERIC_HEADER);
        if (ProgramInfo->VerboseFlag) DumpParagraph (Ptr, (GenericHeader->length - (Ptr - (UINT8 *) Buffer)));
      }
    }
    Print (L"\n");
  }

  if (ProgramInfo->CheckFileName != NULL) {
    //
    // Restore EEPROM_MEMORY from temp buffer
    //
    if (ImageBuffer != NULL) {
      //
      // Restore memory image
      //
      EfiStatus = WriteEeprom (EEPROM_MEMORY, 0, &ImageSize, ImageBuffer, NULL);
      if (EFI_ERROR (EfiStatus)) {
        Print (L"ERROR: [#%d] Failed to restore original memory image (%r)!\n", __LINE__, EfiStatus);
        Status = SHELL_DEVICE_ERROR;
        goto Exit;
      }

      //
      // Re-validate the memory image
      //
      EfiStatus = ValidateEeprom (EEPROM_MEMORY);
      if (EFI_ERROR (EfiStatus)) {
        Print (L"ERROR: [#%d] Failed to validate original memory image (%r)!\n", __LINE__, EfiStatus);
        Status = SHELL_DEVICE_ERROR;
        goto Exit;
      }
    }
  }

Exit:
  //
  // Free resources
  //
  if (Buffer != NULL) {
    FreePool (Buffer);
    Buffer = NULL;
  }
  if (ImageBuffer != NULL) {
    FreePool (ImageBuffer);
    ImageBuffer = NULL;
  }
  return Status;
}

VOID
DumpParagraph (
  IN   VOID   *Ptr,
  IN   UINTN   Count
  )
{
  CHAR8     AsciiBuffer[17];
  UINT8    *Data;
  UINTN     Index;
  UINTN     Paragraphs;
  UINTN     PlaceHolder;
  UINT8     PlaceHolders;

  if (gDebugFlag) Print (L"%a(#%d) - Starting...\n", __FUNCTION__, __LINE__);

  // Check for invalid conditions
  if (Ptr == NULL) {
    if (gDebugFlag) Print (L"ERROR: [#%d] Internal coding error!\n", __LINE__);
    goto Exit;
  }

  // Start dumping the buffer
  if (Count > 0 ) {
    // Initialize variables
    Data = (UINT8 *) Ptr;
    Paragraphs = Count / 16;
    if ((Paragraphs * 16) < Count) {
      Paragraphs++;
    }
    PlaceHolder  = Paragraphs;
    PlaceHolders = 0;
    while (PlaceHolder > 0) {
      PlaceHolders++;
      PlaceHolder >>= 4;
    }

    // Print Header
    PrintChar (PlaceHolders + 5, L" ");
    Print (DIVIDING_LINE);
    PrintChar (PlaceHolders + 5, L" ");
    Print (L"| x0 x1 x2 x3  x4 x5 x6 x7  x8 x9 xA xB  xC xD xE xF |      String      |\n");
    Print (L" +");
    PrintChar (PlaceHolders + 3, L"-");
    Print (DIVIDING_LINE);

    // Print data
    for (Index = 0; Index < (Paragraphs * 16); Index++) {
      if (Index % 0x10 == 0x00) {
        if ((Index > 0) && ((Index / 0x10) % 0x04 == 0x00) && (Paragraphs > 6)) {
          Print (L" +");
          PrintChar (PlaceHolders + 3, L"-");
          Print (DIVIDING_LINE);
        }
        Print (L" | %0*xx | ", PlaceHolders, (Index / 0x10));
      }

      // Print the data or a filler
      if (Index < Count) {
        Print (L"%02x ", Data[Index]);
        if ((Data[Index] < 32) || (Data[Index] > 126)) {
          // Not printable
          AsciiBuffer[(Index % 0x10)] = '.';
        } else {
          // Printable
          AsciiBuffer[(Index % 0x10)] = Data[Index];
        }
      } else {
        Print (L"   ");
        AsciiBuffer[(Index % 0x10)] = ' ';
      }

      // Print break or line end if needed
      if (Index % 0x10 == 0x0F) {
        AsciiBuffer[16] = 0x00;
        Print (L"| %a |\n", AsciiBuffer);
      } else if (Index % 0x04 == 0x03) {
        Print (L" ");
      }
    }

    // Print footer
    Print (L" +");
    PrintChar (PlaceHolders + 3, L"-");
    Print (DIVIDING_LINE);
  }

Exit:
  return;
}

VOID
DumpProgramInfo (
  IN PROGRAM_INFO    *ProgramInfo
  )
{
  if (gDebugFlag) Print (L"%a(#%d) - Starting...\n", __FUNCTION__, __LINE__);

  Print (L"ProgramInfo                @ %08x\n", ProgramInfo);
  Print (L" - HelpFlag                = %02x\n", ProgramInfo->HelpFlag);
  Print (L" - AddressFlag             = %02x\n", ProgramInfo->AddressFlag);
  Print (L"   - AddressValue          = %02x\n", ProgramInfo->AddressValue);
  Print (L" - BusFlag                 = %02x\n", ProgramInfo->BusFlag);
  Print (L"   - BusValue              = %02x\n", ProgramInfo->BusValue);
  Print (L" - CheckFlag               = %02x\n", ProgramInfo->CheckFlag);
  Print (L"   - CheckLibraryIndex     = %02x\n", ProgramInfo->CheckLibraryIndex);
  Print (L"   - CheckFile             = %08x\n", ProgramInfo->CheckFile);
  Print (L"   - CheckFileName         = %s\n",   ProgramInfo->CheckFileName);
  Print (L"   - CheckFileSize         = %08x\n", ProgramInfo->CheckFileSize);
  Print (L" - DumpHumanFlag           = %02x\n", ProgramInfo->DumpHumanFlag);
  Print (L"   - DumpHumanLibraryIndex = %02x\n", ProgramInfo->DumpHumanLibraryIndex);
  Print (L"   - DumpHumanFile         = %08x\n", ProgramInfo->DumpHumanFile);
  Print (L"   - DumpHumanFileName     = %s\n",   ProgramInfo->DumpHumanFileName);
  Print (L"   - DumpHumanFileSize     = %08x\n", ProgramInfo->DumpHumanFileSize);
  Print (L" - DumpRawFlag             = %02x\n", ProgramInfo->DumpRawFlag);
  Print (L"   - DumpRawLibraryIndex   = %02x\n", ProgramInfo->DumpRawLibraryIndex);
  Print (L"   - DumpRawFile           = %08x\n", ProgramInfo->DumpRawFile);
  Print (L"   - DumpRawFileName       = %s\n",   ProgramInfo->DumpRawFileName);
  Print (L"   - DumpRawFileSize       = %08x\n", ProgramInfo->DumpRawFileSize);
  Print (L" - EraseFlag               = %02x\n", ProgramInfo->EraseFlag);
  Print (L"   - EraseLibraryIndex     = %02x\n", ProgramInfo->EraseLibraryIndex);
  Print (L" - ReadFlag                = %02x\n", ProgramInfo->ReadFlag);
  Print (L"   - ReadLibraryIndex      = %02x\n", ProgramInfo->ReadLibraryIndex);
  Print (L"   - ReadFile              = %08x\n", ProgramInfo->ReadFile);
  Print (L"   - ReadFileName          = %s\n",   ProgramInfo->ReadFileName);
  Print (L"   - ReadFileSize          = %08x\n", ProgramInfo->ReadFileSize);
  Print (L" - ScanFlag                = %02x\n", ProgramInfo->ScanFlag);
  Print (L" - VerifyFlag              = %02x\n", ProgramInfo->VerifyFlag);
  Print (L"   - VerifyLibraryIndex    = %02x\n", ProgramInfo->VerifyLibraryIndex);
  Print (L"   - VerifyFile            = %08x\n", ProgramInfo->VerifyFile);
  Print (L"   - VerifyFileName        = %s\n",   ProgramInfo->VerifyFileName);
  Print (L"   - VerifyFileSize        = %08x\n", ProgramInfo->VerifyFileSize);
  Print (L" - WriteFlag               = %02x\n", ProgramInfo->WriteFlag);
  Print (L"   - WriteLibraryIndex     = %02x\n", ProgramInfo->WriteLibraryIndex);
  Print (L"   - WriteFile             = %08x\n", ProgramInfo->WriteFile);
  Print (L"   - WriteFileName         = %s\n",   ProgramInfo->WriteFileName);
  Print (L"   - WriteFileSize         = %08x\n", ProgramInfo->WriteFileSize);
}

SHELL_STATUS
DumpRawOption (
  IN PROGRAM_INFO    *ProgramInfo
  )
{
  EEPROM_FUNCTION_INFO    EepromInfo;
  EFI_STATUS              EfiStatus;
  VOID                   *FunctionInfo;
  SHELL_STATUS            Status;

  if (gDebugFlag) Print (L"%a(#%d) - Starting...\n", __FUNCTION__, __LINE__);

  //
  // Initialize variables
  //
  ZeroMem (&EepromInfo, sizeof (EEPROM_FUNCTION_INFO));
  EepromInfo.Bus          = ProgramInfo->BusValue;
  EepromInfo.Address      = ProgramInfo->AddressValue;
  EepromInfo.LibraryIndex = EEPROM_EEPROM;
  FunctionInfo            = NULL;
  Status                  = SHELL_SUCCESS;

  //
  // Sanity checks
  //
  if (ProgramInfo == NULL) {
    if (gDebugFlag) Print (L"ERROR: [#%d] Internal coding error!\n", __LINE__);
    Status = SHELL_INVALID_PARAMETER;
    goto Exit;
  }
  if (ProgramInfo->DumpRawFileName != NULL) {
    if ((ProgramInfo->DumpRawFile == NULL) || (ProgramInfo->DumpRawFileSize == 0)) {
      if (gDebugFlag) Print (L"ERROR: [#%d] Image file is empty!\n", __LINE__);
      Status = SHELL_INVALID_PARAMETER;
      goto Exit;
    }
  }

  //
  // Read in the image file
  //
  if (ProgramInfo->DumpRawFileName == NULL) {
    //
    // Find out the size of the image
    //
    ProgramInfo->DumpRawFileSize = GetImageSize (ProgramInfo->DumpRawLibraryIndex);
    ProgramInfo->DumpRawFile     = AllocateZeroPool (ProgramInfo->DumpRawFileSize);
    if (ProgramInfo->DumpRawFile == NULL) {
      Print (L"ERROR: [#%d] Failed to allocate buffer!\n", __LINE__);
      Status = SHELL_OUT_OF_RESOURCES;
      goto Exit;
    }

    //
    // Read in the image
    //
    if (ProgramInfo->DumpRawLibraryIndex == EEPROM_EEPROM) {
      FunctionInfo = (VOID *) &EepromInfo;
    }
    EfiStatus = ReadEeprom (ProgramInfo->DumpRawLibraryIndex, 0, &ProgramInfo->DumpRawFileSize, ProgramInfo->DumpRawFile, FunctionInfo);
    if (EFI_ERROR (EfiStatus)) {
      Print (L"ERROR: [#%d] Failed to read image file (%r)!\n", __LINE__, EfiStatus);
      Status = SHELL_DEVICE_ERROR;
      goto Exit;
    }
  }

  //
  // Dump the image
  //
  DumpParagraph (ProgramInfo->DumpRawFile, ProgramInfo->DumpRawFileSize);

  //
  // Set exit value
  //
  Status = SHELL_SUCCESS;

Exit:
  return Status;
}

SHELL_STATUS
EraseOption (
  IN PROGRAM_INFO    *ProgramInfo
  )
{
  EFI_STATUS              EfiStatus;
  SHELL_STATUS            Status;

  if (gDebugFlag) Print (L"%a(#%d) - Starting...\n", __FUNCTION__, __LINE__);

  //
  // Initialize variables
  //
  Status = SHELL_SUCCESS;

  EfiStatus = EraseEeprom (ProgramInfo->EraseLibraryIndex);
  if (EFI_ERROR (EfiStatus)) {
    Print (L"ERROR: [#%d] Failed to erase image file (%r)!\n", __LINE__, EfiStatus);
    Status = SHELL_DEVICE_ERROR;
  }
  return Status;
}

UINTN
GetHexOrDecFromString (
  CHAR16 *Arg
  )
{
  CHAR16 Str[2];

  Str[0] = Arg[0];
  Str[1] = Arg[1];

  if (StrCmp (Str, L"0x") == 0 ||
    StrCmp (Str, L"0X") == 0) {
    return StrHexToUintn (Arg);
  } else{
    return StrDecimalToUintn (Arg);
  }
}

VOID
ParseParameters (
  IN UINTN            Argc,
  IN CHAR16         **Argv,
  IN PROGRAM_INFO    *ProgramInfo
  )
{
  EFI_STATUS   EfiStatus;
  UINTN        Index;

  if (gDebugFlag) Print (L"%a(#%d) - Starting...\n", __FUNCTION__, __LINE__);

  // Sanity checks
  if ((Argv == NULL) || (ProgramInfo == NULL)) {
    if (gDebugFlag) Print (L"ERROR: [#%d] Internal coding error!\n", __LINE__);
    goto Exit;
  }

  // Clear ProgramInfo
  ZeroMem (ProgramInfo, sizeof (ProgramInfo));

  for (Index = 1; Index < Argc; Index++) {
    // Convert to lowercase for easier parsing
    ToLower (Argv[Index]);

    // Check for Debug option
    if (StrCmp (Argv[Index], L"--debug") == 0) {
      // Debug option detected
      gDebugFlag = TRUE;
      Print (L"Enabling debug mode...\n");
    } else

    // Check for Help option
    if ((StrCmp (Argv[Index], L"-h") == 0)     ||
        (StrCmp (Argv[Index], L"--help") == 0) ||
        (StrCmp (Argv[Index], L"-?") == 0)) {
      // Help option detected
      ProgramInfo->HelpFlag = TRUE;
    } else

    // Check for Address option
    if ((StrCmp (Argv[Index], L"-a") == 0) || (StrCmp (Argv[Index], L"--address") == 0)) {
      // Address option detected
      ProgramInfo->AddressFlag = TRUE;
      if (Index + 1 < Argc) {
        // Get Slave address
        if (Argv[Index + 1][0] != L'-') {
          ProgramInfo->AddressValue = (UINT8) (GetHexOrDecFromString (Argv[Index + 1]) & 0xFF);
          Index++;
        }
      }
    } else

    // Check for Bus option
    if ((StrCmp (Argv[Index], L"-b") == 0) || (StrCmp (Argv[Index], L"--bus") == 0)) {
      // Bus option detected
      ProgramInfo->BusFlag = TRUE;
      if (Index + 1 < Argc) {
        // Get bus number
        if (Argv[Index + 1][0] != L'-') {
          ProgramInfo->BusValue = (UINT8) (GetHexOrDecFromString (Argv[Index + 1]) & 0xFF);
          Index++;
        }
      }
    } else

    // Check for Check security option
    if ((StrCmp (Argv[Index], L"-c") == 0) || (StrCmp (Argv[Index], L"--check") == 0)) {
      // Check security option detected
      ProgramInfo->CheckFlag = TRUE;
      if (Index + 1 < Argc) {
        // Get Library Index
        if (Argv[Index + 1][0] != L'-') {
          ProgramInfo->CheckLibraryIndex = (UINT8) (GetHexOrDecFromString (Argv[Index + 1]) & 0xFF);
          Index++;
        }
      }
      if (Index + 1 < Argc) {
        // Get Filename
        if (Argv[Index + 1][0] != L'-') {
          ProgramInfo->CheckFileName = Argv[Index + 1];
          Index++;
          // Read in file
          ProgramInfo->CheckFile = NULL;
          EfiStatus = ReadFileData (ProgramInfo->CheckFileName, &ProgramInfo->CheckFile, &ProgramInfo->CheckFileSize);
          if (EFI_ERROR (EfiStatus)) {
            Print (L"ERROR: [#%d] Failed to read image file (%r)!\n", __LINE__, EfiStatus);
          }
        }
      }
    } else

    // Check for Dump raw option
    if ((StrCmp (Argv[Index], L"-d") == 0) || (StrCmp (Argv[Index], L"--dump") == 0)) {
      // Dump raw option detected
      ProgramInfo->DumpRawFlag = TRUE;
      if (Index + 1 < Argc) {
        // Get Library Index
        if (Argv[Index + 1][0] != L'-') {
          ProgramInfo->DumpRawLibraryIndex = (UINT8) (GetHexOrDecFromString (Argv[Index + 1]) & 0xFF);
          Index++;
        }
      }
      if (Index + 1 < Argc) {
        // Get Filename
        if (Argv[Index + 1][0] != L'-') {
          ProgramInfo->DumpRawFileName = Argv[Index + 1];
          Index++;
          // Read in file
          ProgramInfo->DumpRawFile = NULL;
          EfiStatus = ReadFileData (ProgramInfo->DumpRawFileName, &ProgramInfo->DumpRawFile, &ProgramInfo->DumpRawFileSize);
          if (EFI_ERROR (EfiStatus)) {
            Print (L"ERROR: [#%d] Failed to read image file (%r)!\n", __LINE__, EfiStatus);
          }
        }
      }
    } else

    // Check for Dump human option
    if ((StrCmp (Argv[Index], L"-dh") == 0) || (StrCmp (Argv[Index], L"--dumphuman") == 0)) {
      // Dump human option detected
      ProgramInfo->DumpHumanFlag = TRUE;
      if (Index + 1 < Argc) {
        // Get Library Index
        if (Argv[Index + 1][0] != L'-') {
          ProgramInfo->DumpHumanLibraryIndex = (UINT8) (GetHexOrDecFromString (Argv[Index + 1]) & 0xFF);
          Index++;
        }
      }
      if (Index + 1 < Argc) {
        // Get Filename
        if (Argv[Index + 1][0] != L'-') {
          ProgramInfo->DumpHumanFileName = Argv[Index + 1];
          Index++;
          // Read in file
          ProgramInfo->DumpHumanFile = NULL;
          EfiStatus = ReadFileData (ProgramInfo->DumpHumanFileName, &ProgramInfo->DumpHumanFile, &ProgramInfo->DumpHumanFileSize);
          if (EFI_ERROR (EfiStatus)) {
            Print (L"ERROR: [#%d] Failed to read image file (%r)!\n", __LINE__, EfiStatus);
          }
        }
      }
    } else

    // Check for erase option
    if ((StrCmp (Argv[Index], L"-e") == 0) || (StrCmp (Argv[Index], L"--erase") == 0)) {
      // Check security option detected
      ProgramInfo->EraseFlag = TRUE;
      if (Index + 1 < Argc) {
        // Get Library Index
        if (Argv[Index + 1][0] != L'-') {
          ProgramInfo->EraseLibraryIndex = (UINT8) (GetHexOrDecFromString (Argv[Index + 1]) & 0xFF);
          Index++;
        }
      }
    } else

    // Check for Read option
    if ((StrCmp (Argv[Index], L"-r") == 0) || (StrCmp (Argv[Index], L"--read") == 0)) {
      // Read option detected
      ProgramInfo->ReadFlag = TRUE;
      if (Index + 1 < Argc) {
        // Get Library Index
        if (Argv[Index + 1][0] != L'-') {
          ProgramInfo->ReadLibraryIndex = (UINT8) (GetHexOrDecFromString (Argv[Index + 1]) & 0xFF);
          Index++;
        }
      }
      if (Index + 1 < Argc) {
        // Get Filename
        if (Argv[Index + 1][0] != L'-') {
          ProgramInfo->ReadFileName = Argv[Index + 1];
          Index++;
        }
      }
    } else

    // Check for Scan option
    if ((StrCmp (Argv[Index], L"-s") == 0) || (StrCmp (Argv[Index], L"--scan") == 0)) {
      // Scan option detected
      ProgramInfo->ScanFlag = TRUE;
    } else

    // Check for Verbose option
    if ((StrCmp (Argv[Index], L"--verbose") == 0)) {
      // Verbose option detected
      ProgramInfo->VerboseFlag = TRUE;
    } else

    // Check for Verify option
    if ((StrCmp (Argv[Index], L"-v") == 0) || (StrCmp (Argv[Index], L"--verify") == 0)) {
      // Verify option detected
      ProgramInfo->VerifyFlag = TRUE;
      if (Index + 1 < Argc) {
        // Get Library Index
        if (Argv[Index + 1][0] != L'-') {
          ProgramInfo->VerifyLibraryIndex = (UINT8) (GetHexOrDecFromString (Argv[Index + 1]) & 0xFF);
          Index++;
        }
      }
      if (Index + 1 < Argc) {
        // Get Filename
        if (Argv[Index + 1][0] != L'-') {
          ProgramInfo->VerifyFileName = Argv[Index + 1];
          Index++;
          // Read in file
          ProgramInfo->VerifyFile = NULL;
          EfiStatus = ReadFileData (ProgramInfo->VerifyFileName, &ProgramInfo->VerifyFile, &ProgramInfo->VerifyFileSize);
          if (EFI_ERROR (EfiStatus)) {
            Print (L"ERROR: [#%d] Failed to read image file (%r)!\n", __LINE__, EfiStatus);
          }
        }
      }
    } else

    // Check for Write option
    if ((StrCmp (Argv[Index], L"-w") == 0) || (StrCmp (Argv[Index], L"--write") == 0)) {
      // Write option detected
      ProgramInfo->WriteFlag = TRUE;
      if (Index + 1 < Argc) {
        // Get Library Index
        if (Argv[Index + 1][0] != L'-') {
          ProgramInfo->WriteLibraryIndex = (UINT8) (GetHexOrDecFromString (Argv[Index + 1]) & 0xFF);
          Index++;
        }
      }
      if (Index + 1 < Argc) {
        // Get Filename
        if (Argv[Index + 1][0] != L'-') {
          ProgramInfo->WriteFileName = Argv[Index + 1];
          Index++;
          // Read in file
          ProgramInfo->WriteFile = NULL;
          EfiStatus = ReadFileData (ProgramInfo->WriteFileName, &ProgramInfo->WriteFile, &ProgramInfo->WriteFileSize);
          if (EFI_ERROR (EfiStatus)) {
            Print (L"ERROR: [#%d] Failed to read image file (%r)!\n", __LINE__, EfiStatus);
          }
        }
      }
    } else

    // All that's left is Unknown
    {
      // Unkown option
      Print (L"Unknown! - Argv[%d]: \"%s\"\n", Index, Argv[Index]);
    }
  }

Exit:
  return;
}

VOID
PrintChar (
  IN UINT8    Count,
  IN CHAR16   *Char
)
{
  UINTN Index;

  // Check for invalid conditions
  if (Char == NULL) {
    Print (L"ERROR: [#%d] Internal coding error!\n", __LINE__);
    goto Exit;
  }

  for (Index = 0; Index < Count; Index++) {
    Print (L"%s", Char);
  }

Exit:
  return;
}

VOID
PrintHelp (VOID)
{
  Print (L" EepromApp.efi is a tool to read/write/dump the EERPOM data.\n");
  Print (L"\n");
  Print (L" Switches:\n");
  Print (L" -a  #            Sets the I2C slave address (7-bit).\n");
  Print (L" -b  #            Sets the I2C bus number.\n");
  Print (L" -c  # filename   Check security of the image.\n");
  Print (L" -d  # filename   Dumps the image in raw format.\n");
  Print (L" -dh # filename   Dumps the image in human readable format.\n");
  Print (L" -e  #            Erase that image.\n");
  Print (L" -h               Dumps this help info.\n");
  Print (L" -r  # filename   Reads the image from the library index into the file.\n");
  Print (L"                  - This reads the entire EEPROM part, not just the image.\n");
  Print (L" -s               Scans all of the buses looking for images.\n");
  Print (L" -v  # filename   Verifies the image in library index with the file.\n");
  Print (L" --verbose        Turns on verbosity.\n");
  Print (L" -w  # filename   Writes the image from the file to the library index.\n");
  Print (L"\n");
  Print (L" Library:\n");
  Print (L"  0 - Null library\n");
  Print (L"  1 - EEPROM library\n");
  Print (L"  2 - FV library\n");
  Print (L"  3 - Memory library\n");
  Print (L"\n");
  Print (L" Examples:\n");
  Print (L"   EepromApp.efi -b  6 -a 0x53 -d 1            - Dump the EEPROM image in raw\n");
  Print (L"                                                 format from I2C #6, 0x53.\n");
  Print (L"   EepromApp.efi -r  3 Test.bin                - Write the memory image to\n");
  Print (L"                                                 Test.bin.\n");
  Print (L"   EepromApp.efi -s -w  1 Test.bin             - Write Test.bin to the EERPOM\n");
  Print (L"                                                 last found during the scan.\n");
  Print (L"   EepromApp.efi -b  4 -a 0x5F -w 1 Test.bin   - Write Test.bin to the EERPOM\n");
  Print (L"                                                 on I2C #4, 0x5F.\n");
  Print (L"   EepromApp.efi -dh 0 Test.bin                - Dump Test.bin in human\n");
  Print (L"                                                 readable form.\n");
  Print (L"   EepromApp.efi -s -c 1                       - Scan the EEPROM buses for\n");
  Print (L"                                                 images and check the security\n");
  Print (L"                                                 of the last one found.\n");
  Print (L"\n");
}

//
// Read data from file
//
EFI_STATUS
ReadFileData (
  IN  CHAR16    *FileName,
  OUT UINT8    **Buffer,
  OUT UINT32    *BufferSize
  )
{
  EFI_STATUS             Status;
  SHELL_FILE_HANDLE      FileHandle;
  UINT64                 Size;
  VOID                   *NewBuffer;
  UINTN                  ReadSize;

  if (gDebugFlag) Print (L"%a(#%d) - Starting...\n", __FUNCTION__, __LINE__);

  //
  // Initialize variables
  //
  FileHandle = NULL;
  NewBuffer  = NULL;
  Size       = 0;

  //
  // Sanity checks
  //
  if (FileName == NULL) {
    if (gDebugFlag) Print (L"ERROR: [#%d] FileName not set!\n", __LINE__);
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  if (Buffer == NULL) {
    if (gDebugFlag) Print (L"ERROR: [#%d] Buffer pointer is NULL!\n", __LINE__);
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  if (*Buffer != NULL) {
    if (gDebugFlag) Print (L"ERROR: [#%d] *Buffer pointer is not NULL!\n", __LINE__);
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  //
  // Open file
  //
  Status = ShellOpenFileByName (FileName, &FileHandle, EFI_FILE_MODE_READ, 0);
  if (EFI_ERROR (Status)) {
    Print (L"ERROR: [#%d] Failed to open file %s (%r)\n", __LINE__, FileName, Status);
    goto Exit;
  }

  //
  // Sanity check to make sure it isn't a directory
  //
  Status = FileHandleIsDirectory (FileHandle);
  if (!EFI_ERROR (Status)) {
    Print (L"ERROR: [#%d] Failed to determine if %s was a directory (%r)\n", __LINE__, FileName, Status);
    Status = EFI_NOT_FOUND;
    goto Exit;
  }

  //
  // Get file size
  //
  Status = FileHandleGetSize (FileHandle, &Size);
  if (EFI_ERROR (Status)) {
    Print (L"ERROR: [#%d] Failed to get size of %s (%r)\n", __LINE__, FileName, Status);
    goto Exit;
  }

  //
  // Allocate buffer
  //
  NewBuffer = AllocatePool ((UINTN) Size);

  //
  // Read in file
  //
  ReadSize = (UINTN) Size;
  Status = FileHandleRead (FileHandle, &ReadSize, NewBuffer);
  if (EFI_ERROR (Status)) {
    Print (L"ERROR: [#%d] Failed to read %s (%r)\n", __LINE__, FileName, Status);
    goto Exit;
  } else if (ReadSize != (UINTN) Size) {
    Print (L"ERROR: [#%d] Failed to read the right number of bytes. Should have been %08x and read %08x\n", __LINE__, Size, ReadSize);
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

Exit:
  //
  // Free resources
  //
  if (FileHandle != NULL) {
    ShellCloseFile (&FileHandle);
  }

  //
  // Set output contents
  //
  if (!EFI_ERROR (Status)) {
    *BufferSize = (UINT32) Size;
    *Buffer     = NewBuffer;
  } else {
    *BufferSize = 0;
    *Buffer     = NULL;
  }

  return Status;
}

SHELL_STATUS
ReadOption (
  IN PROGRAM_INFO    *ProgramInfo
  )
{
  EEPROM_FUNCTION_INFO    EepromInfo;
  EFI_STATUS              EfiStatus;
  VOID                   *FunctionInfo;
  SHELL_STATUS            Status;

  if (gDebugFlag) Print (L"%a(#%d) - Starting...\n", __FUNCTION__, __LINE__);

  //
  // Initialize variables
  //
  ZeroMem (&EepromInfo, sizeof (EEPROM_FUNCTION_INFO));
  EepromInfo.Bus          = ProgramInfo->BusValue;
  EepromInfo.Address      = ProgramInfo->AddressValue;
  EepromInfo.LibraryIndex = EEPROM_EEPROM;
  FunctionInfo            = NULL;
  Status                  = SHELL_SUCCESS;

  //
  // Sanity checks
  //
  if (ProgramInfo == NULL) {
    if (gDebugFlag) Print (L"ERROR: [#%d] Internal coding error!\n", __LINE__);
    Status = SHELL_INVALID_PARAMETER;
    goto Exit;
  }

  //
  // Find out the size of the image
  //
  Print (L"  - Reading %a to %s", mEepromLibraryString[ProgramInfo->ReadLibraryIndex], ProgramInfo->ReadFileName);
  if (ProgramInfo->ReadLibraryIndex == EEPROM_EEPROM) {
    Print (L" via bus #%d @ 0x%02x", ProgramInfo->BusValue, ProgramInfo->AddressValue);
    FunctionInfo = (VOID *) &EepromInfo;
  }
  Print (L"...\n");
  ProgramInfo->ReadFileSize = GetImageSize (ProgramInfo->ReadLibraryIndex);
  ProgramInfo->ReadFile     = AllocateZeroPool (ProgramInfo->ReadFileSize);
  if (ProgramInfo->ReadFile == NULL) {
    Print (L"ERROR: [#%d] Failed to allocate buffer!\n", __LINE__);
    Status = SHELL_OUT_OF_RESOURCES;
    goto Exit;
  }

  //
  // Fill buffer from library
  //
  EfiStatus = ReadEeprom (ProgramInfo->ReadLibraryIndex, 0, &ProgramInfo->ReadFileSize, ProgramInfo->ReadFile, FunctionInfo);
  if (EFI_ERROR (EfiStatus)) {
    Print (L"ERROR: [#%d] Failed to read image file (%r)!\n", __LINE__, EfiStatus);
    Status = SHELL_DEVICE_ERROR;
    goto Exit;
  }

  //
  // Write out the image file
  //
  EfiStatus = WriteFileData (ProgramInfo->ReadFileName, ProgramInfo->ReadFile, ProgramInfo->ReadFileSize);
  if (EFI_ERROR (EfiStatus)) {
    Print (L"ERROR: [#%d] Failed to write image file (%r)!\n", __LINE__, EfiStatus);
    Status = SHELL_DEVICE_ERROR;
    goto Exit;
  }

  Print (L"  - Image %a written to %s\n", mEepromLibraryString[ProgramInfo->ReadLibraryIndex], ProgramInfo->ReadFileName);

Exit:
  //
  // Free resources
  //
  return Status;
}

SHELL_STATUS
ScanOption (
  IN PROGRAM_INFO    *ProgramInfo
  )
{
  UINT8                   Buffer[2];
  UINT8                   Bus;
  EEPROM_FUNCTION_INFO    EepromInfo;
  EFI_STATUS              EfiStatus;
  UINT8                   index;
  UINT8                   ScanSize;
  UINT32                  Size;
  SHELL_STATUS            Status;

  if (gDebugFlag) Print (L"%a(#%d) - Starting...\n", __FUNCTION__, __LINE__);

  //
  // Initialize variables
  //
  ZeroMem (&EepromInfo, sizeof (EEPROM_FUNCTION_INFO));
  EepromInfo.LibraryIndex = EEPROM_EEPROM;
  ScanSize                = 0;
  Size                    = 0;
  Status                  = SHELL_NOT_FOUND;

  //
  // Loop thru I2C buses for images
  //
  EepromInfo.ScanSize = &ScanSize;
  for (Bus = 0; Bus < 8; Bus++) {
    if (gDebugFlag) Print (L" - Scanning bus #%d\n", Bus);
    EepromInfo.Bus = Bus;
    ScanSize       = 0;
    EfiStatus = ReadEeprom (EEPROM_EEPROM, 0, &Size, Buffer, &EepromInfo);
    if (!EFI_ERROR (EfiStatus)) {
      //
      // Dump any images found
      //
      Status = SHELL_SUCCESS;
      for (index = 0; index < ScanSize; index++) {
        Print (L"  - Found image on I2C bus #%d, address 0x%02x\n", Bus, EepromInfo.ScanBuffer[index]);
        ProgramInfo->BusFlag      = TRUE;
        ProgramInfo->BusValue     = Bus;
        ProgramInfo->AddressFlag  = TRUE;
        ProgramInfo->AddressValue = EepromInfo.ScanBuffer[index];
      }
    }
  }

  //
  // Free resources
  //
  if (EepromInfo.ScanBuffer != NULL) {
    FreePool (EepromInfo.ScanBuffer);
    EepromInfo.ScanBuffer = NULL;
  }

  return Status;
}

/**
  Convert a null-terminated unicode string, in-place, to all lowercase.
  Then return it.

  @param  Str    The null-terminated string to be converted to all lowercase.

  @return        The null-terminated string converted into all lowercase.
**/
CHAR16 *
ToLower (
  IN OUT  CHAR16 *Str
  )
{
  UINTN Index;

  // Check for invalid conditions
  if (Str == NULL) {
    if (gDebugFlag) Print (L"ERROR: [#%d] Internal coding error!\n", __LINE__);
    goto Exit;
  }

  for (Index = 0; Str[Index] != L'\0'; Index++) {
    if ((Str[Index] >= L'A') && (Str[Index] <= L'Z')) {
      Str[Index] -= (CHAR16)(L'A' - L'a');
    }
  }

Exit:
  return Str;
}

SHELL_STATUS
VerifyOption (
  IN PROGRAM_INFO    *ProgramInfo
  )
{
  EEPROM_FUNCTION_INFO    EepromInfo;
  EFI_STATUS              EfiStatus;
  VOID                   *FunctionInfo;
  UINT8                  *ImageBuffer;
  UINT32                  ImageSize;
  UINT32                  index;
  SHELL_STATUS            Status;

  if (gDebugFlag) Print (L"%a(#%d) - Starting...\n", __FUNCTION__, __LINE__);

  //
  // Initialize variables
  //
  ZeroMem (&EepromInfo, sizeof (EEPROM_FUNCTION_INFO));
  EepromInfo.Bus          = ProgramInfo->BusValue;
  EepromInfo.Address      = ProgramInfo->AddressValue;
  EepromInfo.LibraryIndex = EEPROM_EEPROM;
  FunctionInfo            = NULL;
  ImageBuffer             = NULL;
  Status                  = SHELL_SUCCESS;

  //
  // Sanity checks
  //
  if (ProgramInfo == NULL) {
    if (gDebugFlag) Print (L"ERROR: [#%d] Internal coding error!\n", __LINE__);
    Status = SHELL_INVALID_PARAMETER;
    goto Exit;
  }
  if ((ProgramInfo->VerifyFile == NULL) || (ProgramInfo->VerifyFileSize == 0)) {
    if (gDebugFlag) Print (L"ERROR: [#%d] Image file is empty!\n", __LINE__);
    Status = SHELL_INVALID_PARAMETER;
    goto Exit;
  }

  //
  // Find out the size of the image
  //
  Print (L"  - Comparing %s to %a", ProgramInfo->VerifyFileName, mEepromLibraryString[ProgramInfo->VerifyLibraryIndex]);
  if (ProgramInfo->VerifyLibraryIndex == EEPROM_EEPROM) {
    Print (L" via bus #%d @ 0x%02x", ProgramInfo->BusValue, ProgramInfo->AddressValue);
    FunctionInfo = (VOID *) &EepromInfo;
  }
  Print (L"...\n");
  ImageSize   = GetImageSize (ProgramInfo->VerifyLibraryIndex);
  ImageBuffer = AllocateZeroPool (ImageSize);
  if (ImageBuffer == NULL) {
    Print (L"ERROR: [#%d] Failed to allocate buffer!\n", __LINE__);
    Status = SHELL_OUT_OF_RESOURCES;
    goto Exit;
  }

  //
  // Read in the memory image
  //
  EfiStatus = ReadEeprom (ProgramInfo->VerifyLibraryIndex, 0, &ImageSize, ImageBuffer, FunctionInfo);
  if (EFI_ERROR (EfiStatus)) {
    Print (L"ERROR: [#%d] Failed to read image file (%r)!\n", __LINE__, EfiStatus);
    Status = SHELL_DEVICE_ERROR;
    goto Exit;
  }

  //
  // Compare images
  //
  if ((ImageSize != ProgramInfo->VerifyFileSize) && (ProgramInfo->VerifyLibraryIndex != EEPROM_EEPROM)) {
    Print (L"ERROR: [#%d] Image size does not match file size!\n", __LINE__);
    Status = SHELL_DEVICE_ERROR;
    goto Exit;
  }
  for (index = 0; index < ImageSize; index++) {
    if (ProgramInfo->VerifyFile[index] != ImageBuffer[index]) {
      if (((index + 1) > ProgramInfo->VerifyFileSize) && (ProgramInfo->VerifyLibraryIndex == EEPROM_EEPROM)) {
        //
        // Getting image size from ReadEeprom with a size of 0 for EEPROM_EEPROM will return the size of the part, not the image.
        //
        break;
      } else {
        Print (L"ERROR: [#%d] Failed to verify file & image @ %08x [%02x:%02x]\n", __LINE__, index, ProgramInfo->VerifyFile[index], ImageBuffer[index]);
        Status = SHELL_DEVICE_ERROR;
        goto Exit;
      }
    }
  }
  Print (L"  - Image %s compared to %a successfully\n", ProgramInfo->VerifyFileName, mEepromLibraryString[ProgramInfo->VerifyLibraryIndex]);

Exit:
  //
  // Free resources
  //
  if (ImageBuffer != NULL) {
    FreePool (ImageBuffer);
    ImageBuffer = NULL;
  }
  return Status;
}

EFI_STATUS
WriteFileData (
  IN  CHAR16   *FileName,
  IN  UINT8    *Buffer,
  IN  UINTN     BufferSize
  )
{
  SHELL_FILE_HANDLE               FileHandle;
  EFI_STATUS                      Status;

  if (gDebugFlag) Print (L"%a(#%d) - Starting...\n", __FUNCTION__, __LINE__);

  //
  // Sanity checks
  //
  if (FileName == NULL) {
    if (gDebugFlag) Print (L"ERROR: [#%d] FileName not set!\n", __LINE__);
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  if (Buffer == NULL) {
    if (gDebugFlag) Print (L"ERROR: [#%d] Buffer pointer is NULL!\n", __LINE__);
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  //
  // Is the requested file a directory?
  //
  if (ShellIsDirectory (FileName) == EFI_SUCCESS) {
    Print (L"ERROR: [#%d] File is a directory! Can not be saved.\n", __LINE__);
    Status = EFI_LOAD_ERROR;
    goto Exit;
  }

  //
  // If file exists, delete it
  //
  Status = ShellOpenFileByName (FileName, &FileHandle, EFI_FILE_MODE_READ|EFI_FILE_MODE_WRITE, 0);
  if (!EFI_ERROR (Status)) {
    //
    // the file exits, delete it
    //
    Status = ShellDeleteFile (&FileHandle);
    if (EFI_ERROR (Status) || Status == EFI_WARN_DELETE_FAILURE) {
      Print (L"ERROR: [#%d] Deleteing existing file failed!\n", __LINE__);
      Status = EFI_LOAD_ERROR;
      goto Exit;
    }
  }

  //
  // Create file
  //
  Status = ShellOpenFileByName (FileName, &FileHandle, EFI_FILE_MODE_READ|EFI_FILE_MODE_WRITE|EFI_FILE_MODE_CREATE, 0);
  if (EFI_ERROR (Status)) {
    Print (L"ERROR: [#%d] Creating file failed!\n", __LINE__);
    Status = EFI_LOAD_ERROR;
    goto Exit;
  }

  //
  // Write buffer to file
  //
  Status = ShellWriteFile (FileHandle, &BufferSize, Buffer);
  if (EFI_ERROR (Status)) {
    ShellDeleteFile (&FileHandle);
    Status = EFI_LOAD_ERROR;
    goto Exit;
  }

  Status = EFI_SUCCESS;

Exit:
  //
  // Free resources
  //
  if (FileHandle != NULL) {
    ShellCloseFile (&FileHandle);
  }

  return Status;
}

SHELL_STATUS
WriteOption (
  IN PROGRAM_INFO    *ProgramInfo
  )
{
  EEPROM_FUNCTION_INFO    EepromInfo;
  EFI_STATUS              EfiStatus;
  VOID                   *FunctionInfo;
  SHELL_STATUS            Status;

  if (gDebugFlag) Print (L"%a(#%d) - Starting...\n", __FUNCTION__, __LINE__);

  //
  // Initialize variables
  //
  ZeroMem (&EepromInfo, sizeof (EEPROM_FUNCTION_INFO));
  EepromInfo.Bus          = ProgramInfo->BusValue;
  EepromInfo.Address      = ProgramInfo->AddressValue;
  EepromInfo.LibraryIndex = EEPROM_EEPROM;
  FunctionInfo            = NULL;
  Status                  = SHELL_SUCCESS;

  //
  // Sanity checks
  //
  if (ProgramInfo == NULL) {
    if (gDebugFlag) Print (L"ERROR: [#%d] Internal coding error!\n", __LINE__);
    Status = SHELL_INVALID_PARAMETER;
    goto Exit;
  }
  if ((ProgramInfo->WriteFile == NULL) || (ProgramInfo->WriteFileSize == 0)) {
    if (gDebugFlag) Print (L"ERROR: [#%d] Image file is empty!\n", __LINE__);
    Status = SHELL_INVALID_PARAMETER;
    goto Exit;
  }

  //
  // Write image out
  //
  Print (L"  - Writing %s to %a", ProgramInfo->WriteFileName, mEepromLibraryString[ProgramInfo->WriteLibraryIndex]);
  if (ProgramInfo->WriteLibraryIndex == EEPROM_EEPROM) {
    Print (L" via bus #%d @ 0x%02x", ProgramInfo->BusValue, ProgramInfo->AddressValue);
    FunctionInfo = (VOID *) &EepromInfo;
  }
  Print (L"...\n");
  EfiStatus = WriteEeprom (ProgramInfo->WriteLibraryIndex, 0, &ProgramInfo->WriteFileSize, ProgramInfo->WriteFile, FunctionInfo);
  if (EFI_ERROR (EfiStatus)) {
    Print (L"ERROR: [#%d] Failed to write image file (%r)!\n", __LINE__, EfiStatus);
    Status = SHELL_DEVICE_ERROR;
    goto Exit;
  }
  Print (L"  - Image %s written to %a\n", ProgramInfo->WriteFileName, mEepromLibraryString[ProgramInfo->WriteLibraryIndex]);

  //
  // Validate image
  //
  Print (L"  - Validating image...\n");
  EfiStatus = ValidateEeprom (ProgramInfo->WriteLibraryIndex);
  Print (L"  - Image validation status = %r\n", EfiStatus);

Exit:
  return Status;
}

