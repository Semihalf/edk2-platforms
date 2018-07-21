/** @file
  Common EEPROM library instance.

  Copyright (c) 2015 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "EepromLib.h"

BOOLEAN     *gImageValidFlag;
UINT32       mCrcTable[256];
BOOLEAN      mCrcInitFlag        = FALSE;
BOOLEAN      mEepromLibDebugFlag = TRUE;

/**
  This internal function reverses bits for 32bit data.

  @param  Value                 The data to be reversed.

  @return                       Data reversed.

**/
UINT32
EFIAPI
ReverseBits (
  UINT32  Value
  )
{
  UINTN   Index;
  UINT32  NewValue;

  NewValue = 0;
  for (Index = 0; Index < 32; Index++) {
    if ((Value & (1 << Index)) != 0) {
      NewValue = NewValue | (1 << (31 - Index));
    }
  }

  return NewValue;
}

/**
  Initialize CRC32 table.

**/
VOID
EFIAPI
InitializeCrc32Table (
  VOID
  )
{
  UINTN   TableEntry;
  UINTN   Index;
  UINT32  Value;

  for (TableEntry = 0; TableEntry < 256; TableEntry++) {
    Value = ReverseBits ((UINT32) TableEntry);
    for (Index = 0; Index < 8; Index++) {
      if ((Value & 0x80000000) != 0) {
        Value = (Value << 1) ^ 0x04C11DB7;
      } else {
        Value = Value << 1;
      }
    }

    mCrcTable[TableEntry] = ReverseBits (Value);
  }
  mCrcInitFlag = TRUE;
}

/*++

Routine Description:

  The CalculateCrc32 routine.

Arguments:

  Data        - The buffer contaning the data to be processed
  DataSize    - The size of data to be processed
  CrcOut      - A pointer to the caller allocated UINT32 that on
                contains the CRC32 checksum of Data

Returns:

  EFI_SUCCESS               - Calculation is successful.
  EFI_INVALID_PARAMETER     - Data / CrcOut = NULL, or DataSize = 0

--*/
EFI_STATUS
EFIAPI
CalculateCrc32 (
  IN     UINT8    *Data,
  IN     UINTN     DataSize,
  IN OUT UINT32   *CrcOut
  )
{
  UINT32  Crc;
  UINTN   Index;
  UINT8   *Ptr;

  if (mEepromLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting...\n", __FUNCTION__, __LINE__));

  // Sanity checks
  if ((DataSize == 0) || (Data == NULL) || (CrcOut == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  // Table initialized?
  if (!mCrcInitFlag) InitializeCrc32Table ();

  Crc = 0xFFFFFFFF;
  for (Index = 0, Ptr = Data; Index < DataSize; Index++, Ptr++) {
    Crc = (Crc >> 8) ^ mCrcTable[(UINT8) Crc ^ *Ptr];
  }

  *CrcOut = Crc ^ 0xFFFFFFFF;

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
EepromLibNemToMemory (VOID)
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
EraseEeprom (
  IN       UINT8    LibraryIndex
  )
{
  UINT8                  *Buffer;
  EEPROM_FUNCTION_INFO    EepromInfo;
  UINT32                  Size;
  EFI_STATUS              Status;

  if (mEepromLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting...\n", __FUNCTION__, __LINE__));

  //
  // Initialize variables
  //
  ZeroMem (&EepromInfo, sizeof (EEPROM_FUNCTION_INFO));
  EepromInfo.Bus          = PcdGet8 (PcdEepromBus);
  EepromInfo.Address      = PcdGet8 (PcdEepromAddress);
  EepromInfo.LibraryIndex = EEPROM_EEPROM;
  Size                    = sizeof (GENERIC_HEADER);
  Buffer                  = EepromAllocatePool (Size);

  //
  // Sanity checks
  //
  if (LibraryIndex >= EEPROM_DATA_LIBRARY_INDEX_MAX) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Library index is larger than %d!\n", __FUNCTION__, __LINE__, EEPROM_DATA_LIBRARY_INDEX_MAX - 1));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  if (Buffer == NULL) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Buffer pointer is NULL!\n", __FUNCTION__, __LINE__));
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }

  //
  // Try to erase the first header, thereby invalidating the image.
  //
  Status = WriteEeprom (LibraryIndex, 0, &Size, Buffer, &EepromInfo);
  if (EFI_ERROR (Status)) {
    //
    // Failed, so bail.
    //
    goto Exit;
  }

  //
  // Clear the valid status for this image.
  //
  gImageValidFlag[LibraryIndex] = FALSE;

Exit:
  //
  // Free resources
  //
  Buffer = EepromFreePool (Buffer);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - Ending with %r\n", __FUNCTION__, __LINE__, Status));
  }
  return Status;
}

EFI_STATUS
EFIAPI
GetEepromStructure (
  IN       UINT8      LibraryIndex,
  IN OUT   CHAR8      Signature[EEPROM_SIGNATURE_SIZE],
  IN OUT   UINT8    **Buffer,
  IN OUT   UINT32    *Size
  )
{
  GENERIC_HEADER   *EepromHeader;
  UINT32            Index;
  EFI_STATUS        Status;

  if (mEepromLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting...\n", __FUNCTION__, __LINE__));

  //
  // Initialize variables
  //
  EepromHeader = NULL;
  Status       = EFI_SUCCESS;

  //
  // Sanity checks
  //
  if (LibraryIndex >= EEPROM_DATA_LIBRARY_INDEX_MAX) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Library index is larger than %d!\n", __FUNCTION__, __LINE__, EEPROM_DATA_LIBRARY_INDEX_MAX - 1));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  if (Signature == NULL) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Signature pointer is NULL!\n", __FUNCTION__, __LINE__));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  if (Buffer == NULL) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Buffer pointer is NULL!\n", __FUNCTION__, __LINE__));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  if (Size == NULL) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Size pointer is NULL!\n", __FUNCTION__, __LINE__));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  //
  // Start at the beginning structure and loop thru the image looking for the requested structure
  //
  Index = 0;
  while (!EFI_ERROR (Status)) {
    //
    // Make sure buffer is empty
    //
    *Buffer = EepromFreePool (*Buffer);
    Status = GetNextEepromStructure (LibraryIndex, &Index, Buffer, Size);
    if (*Size != 0) {
      //
      // The buffer isn't empty
      //
      EepromHeader = (GENERIC_HEADER *) *Buffer;
      if (AsciiStrnCmp (EepromHeader->signature, Signature, AsciiStrLen (Signature)) == 0) {
        //
        // This is our structure. Bail.
        //
        goto Exit;
      }
    }
    *Buffer = EepromFreePool (*Buffer);
  }

  //
  // Didn't find it. Error out.
  //
  DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Failed to find %a structure!\n", __FUNCTION__, __LINE__, Signature));
  Status = EFI_NOT_FOUND;

Exit:
  if (EFI_ERROR (Status)) {
    *Buffer = EepromFreePool (*Buffer);
    *Size   = 0;
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - Ending with %r\n", __FUNCTION__, __LINE__, Status));
  }
  return Status;
}

UINT32
EFIAPI
GetImageSize (
  IN       UINT8      LibraryIndex
  )
{
  EEPROM_HEADER           EepromHeader;
  EEPROM_FUNCTION_INFO    EepromInfo;
  UINT32                  Size;
  EFI_STATUS              Status;

  if (mEepromLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting...\n", __FUNCTION__, __LINE__));

  //
  // Initialize variables
  //
  ZeroMem (&EepromInfo, sizeof (EEPROM_FUNCTION_INFO));
  EepromInfo.Bus          = PcdGet8 (PcdEepromBus);
  EepromInfo.Address      = PcdGet8 (PcdEepromAddress);
  EepromInfo.LibraryIndex = EEPROM_EEPROM;

  //
  // Sanity checks
  //
  if (LibraryIndex >= EEPROM_DATA_LIBRARY_INDEX_MAX) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Library index is larger than %d!\n", __FUNCTION__, __LINE__, EEPROM_DATA_LIBRARY_INDEX_MAX - 1));
    Size = 0;
    goto Exit;
  }

  //
  // Grab $Eeprom$ structure which should be the first structure at offset 0
  //
  Size = sizeof (EEPROM_HEADER);
  ZeroMem (&EepromHeader, Size);
  Status = ReadEeprom (LibraryIndex, 0, &Size, (UINT8 *) &EepromHeader, &EepromInfo);
  if (EFI_ERROR (Status) || (AsciiStrnCmp (EepromHeader.signature, "$Eeprom$", 8) != 0)) {
    //
    // Oops!
    //
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Failed to find $Eeprom$ structure!\n", __FUNCTION__, __LINE__));
    //
    // Try ReadEeprom() method
    //
    Size   = 0;
    Status = ReadEeprom (LibraryIndex, 0, &Size, (UINT8 *) &EepromHeader, &EepromInfo);
    if ((Status != EFI_BUFFER_TOO_SMALL) || (Status != EFI_SUCCESS)) {
      //
      // Didn't find size.
      //
      Status = EFI_NOT_FOUND;
      goto Exit;
    }
  }

  //
  // Return size
  //
  Size = EepromHeader.structlength;
  if (mEepromLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Image size = %08x\n", __FUNCTION__, __LINE__, Size));

Exit:
  return Size;
}

EFI_STATUS
EFIAPI
GetNextEepromStructure (
  IN       UINT8      LibraryIndex,
  IN OUT   UINT32    *Index,
  IN OUT   UINT8    **Buffer,
  IN OUT   UINT32    *Size
  )
{
  CHAR8                  *AsciiString;
  EEPROM_FUNCTION_INFO    EepromInfo;
  UINT32                  ImageSize;
  UINT32                  Offset;
  EFI_STATUS              Status;
  GENERIC_HEADER          Structure;
  UINT8                  *TempBuffer;
  UINT32                  TempSize;

  if (mEepromLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting...\n", __FUNCTION__, __LINE__));

  //
  // Initialize variables
  //
  ZeroMem (&EepromInfo, sizeof (EEPROM_FUNCTION_INFO));
  EepromInfo.Bus          = PcdGet8 (PcdEepromBus);
  EepromInfo.Address      = PcdGet8 (PcdEepromAddress);
  EepromInfo.LibraryIndex = EEPROM_EEPROM;
  Status                  = EFI_SUCCESS;
  TempBuffer              = NULL;
  TempSize                = 0;

  //
  // Sanity checks
  //
  if (LibraryIndex >= EEPROM_DATA_LIBRARY_INDEX_MAX) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Library index is larger than %d!\n", __FUNCTION__, __LINE__, EEPROM_DATA_LIBRARY_INDEX_MAX - 1));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  if (Buffer == NULL) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Buffer pointer is NULL!\n", __FUNCTION__, __LINE__));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  if (Size == NULL) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Size pointer is NULL!\n", __FUNCTION__, __LINE__));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  //
  // Make sure the incoming buffer is empty
  //
  *Buffer = EepromFreePool (*Buffer);
  *Size   = 0;

  //
  // Grab the image size
  //
  ImageSize = GetImageSize (LibraryIndex);
  if (ImageSize == 0) {
    //
    // Oops!
    //
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Failed to get image size!\n", __FUNCTION__, __LINE__));
    Status = EFI_NOT_FOUND;
    goto Exit;
  }

  //
  // Paragraph align offset
  //
  Offset = *Index + (*Index % 0x10);

  //
  // Sanity check to make sure we are still in the image
  //
  if (Offset >= ImageSize) {
    if (mEepromLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - WARNING: Went past the end of the image!\n", __FUNCTION__, __LINE__));
    Status = EFI_END_OF_FILE;
    goto Exit;
  }
  
  //
  // Read in the next header
  //
  TempSize = sizeof (GENERIC_HEADER);
  Status = ReadEeprom (LibraryIndex, Offset, &TempSize, (UINT8 *) &Structure, &EepromInfo);
  if (EFI_ERROR (Status)) {
    //
    // Got an error!
    //
    goto Exit;
  }
  //
  // Check to see if this is a valid structure
  //
  if (Structure.signature[0] != '$') {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Failed to find structure signature starting with $!\n", __FUNCTION__, __LINE__));
    Status = EFI_VOLUME_CORRUPTED;
    goto Exit;
  }

  //
  // This is our structure!
  //
  TempSize   = Structure.length;
  TempBuffer = EepromAllocatePool (TempSize);
  if (TempBuffer == NULL) {
    //
    // Failed to allocate pool
    //
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Failed to allocate pool for Buffer!\n", __FUNCTION__, __LINE__));
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }

  //
  // Some debug info
  //
  AsciiString = Structure.signature;
  AsciiString[sizeof (Structure.signature)] = 0;
  if (mEepromLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Structure[%04x] = %a\n", __FUNCTION__, __LINE__, Offset, AsciiString));

  //
  // Grab the structure
  //
  Status = ReadEeprom (LibraryIndex, Offset, &TempSize, TempBuffer, &EepromInfo);
  if (mEepromLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - ReadEeprom() -> %r\n", __FUNCTION__, __LINE__, Status));
  if (!EFI_ERROR (Status)) {
    //
    // Got a valid structure. Point Index to end of the structure..
    //
    *Index = Offset + TempSize;
    //
    // Pass out data
    //
    *Buffer = TempBuffer;
    *Size   = TempSize;
  }

Exit:
  if (EFI_ERROR (Status)) {
    TempBuffer = EepromFreePool (TempBuffer);
    if (Status != EFI_END_OF_FILE) {
      DEBUG ((DEBUG_ERROR, "%a (#%4d) - Ending with %r\n", __FUNCTION__, __LINE__, Status));
    }
  }
  return Status;
}

UINT8
EFIAPI
GetValidEepromLibrary (
  IN       BOOLEAN   CopyToMemory,
  IN       BOOLEAN   MemoryInitialized
  )
{
  UINT8                  *EepromAutoList;
  EEPROM_FUNCTION_INFO    EepromInfo;
  UINT8                   FirstValidImage;
  UINT8                  *ImageBuffer;
  UINT32                  ImageSize;
  UINT8                   index;
  UINT8                   Library;
  UINT32                  Size;
  EFI_STATUS              Status;

  if (mEepromLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting...\n", __FUNCTION__, __LINE__));

  //
  // Initialize variables
  //
  ZeroMem (&EepromInfo, sizeof (EEPROM_FUNCTION_INFO));
  EepromAutoList          = (UINT8 *) PcdGetPtr (PcdEepromAutoPriority);
  EepromInfo.Bus          = PcdGet8 (PcdEepromBus);
  EepromInfo.Address      = PcdGet8 (PcdEepromAddress);
  EepromInfo.LibraryIndex = EEPROM_EEPROM;
  FirstValidImage         = EEPROM_NULL;
  ImageBuffer             = NULL;

  //
  // Sanity checks
  //
  if (EepromAutoList == NULL) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: PCD PcdEepromAutoPriority is empty!\n", __FUNCTION__, __LINE__));
    Library = EEPROM_NULL;
    goto Exit;
  }

  //
  // Display current stack pointer
  //
  DisplayStackPointer (__FUNCTION__, __LINE__);

  //
  // Loop thru PcdEepromAutoPriority looking for a validated image.
  //
  index = 0;
  while (EepromAutoList[index] != 0xFF) {
    Library = EepromAutoList[index];
    if (Library >= EEPROM_DATA_LIBRARY_INDEX_MAX) {
      //
      // We're in the weeds. Bail.
      //
      DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: PCD PcdEepromAutoPriority contains invalid value!\n", __FUNCTION__, __LINE__));
      Library = EEPROM_NULL;
      goto Exit;
    }
    if (gImageValidFlag[Library]) {
      //
      // This library is valid, bail.
      //
      FirstValidImage = Library;
      break;
    }
    //
    // Point to next library
    //
    index++;
  }
  //
  // Check to see if we need to validate and copy into memory
  //
  if (!CopyToMemory) {
    //
    // Nope. Bail.
    //
    goto Exit;
  }
  //
  // Display current stack pointer
  //
  DisplayStackPointer (__FUNCTION__, __LINE__);
  //
  // If nothing is valid, try validating them all
  //
  if (FirstValidImage == EEPROM_NULL) {
    //
    // Couldn't find a validated image. Try validating them all.
    //
    index = 0;
    while (EepromAutoList[index] != 0xFF) {
      Library = EepromAutoList[index];
      if (mEepromLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Trying to validate library %a...\n", __FUNCTION__, __LINE__, mEepromLibraryString[Library]));
      Status = ValidateEeprom (Library);
      if (mEepromLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Validating %a -> %r\n", __FUNCTION__, __LINE__, mEepromLibraryString[Library], Status));
      if (!EFI_ERROR (Status) || (Status == EFI_MEDIA_CHANGED)) {
        //
        // This one is valid. Bail.
        //
        if (FirstValidImage == EEPROM_NULL) {
          FirstValidImage = Library;
        }
      }
      //
      // Point to next library
      //
      index++;
    }
  }
  //
  // Display current stack pointer
  //
  DisplayStackPointer (__FUNCTION__, __LINE__);
  //
  // Determine which image to copy to memory
  //
  if (gImageValidFlag[EEPROM_MEMORY]) {
    //
    // Yep. Bail.
    //
    Library = EEPROM_MEMORY;
    goto Exit;
  }
  if (gImageValidFlag[EEPROM_EEPROM]) {
    Library = EEPROM_EEPROM;
  }
  if (gImageValidFlag[EEPROM_FV]) {
    Library = EEPROM_FV;
  }
  if (gImageValidFlag[EEPROM_EEPROM] && gImageValidFlag[EEPROM_FV]) {
    BOARD_INFO_TABLE   *EepromBoardInfo;
    BOARD_INFO_TABLE   *FvBoardInfo;
    EEPROM_HEADER      *EepromEepromHeader;
    EEPROM_HEADER      *FvEepromHeader;
    //
    // Initialize variables
    //
    EepromBoardInfo    = NULL;
    EepromEepromHeader = NULL;
    FvBoardInfo        = NULL;
    FvEepromHeader     = NULL;
    Library            = EEPROM_NULL;
    //
    // Get BoardInfo records
    //
    Size = 0;
    Status = GetEepromStructure (EEPROM_EEPROM, "$BrdInfo", (UINT8 **) &EepromBoardInfo, &Size);
    if (EFI_ERROR (Status) || (Size == 0) || (EepromBoardInfo == NULL)) {
      DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Failed to get EEPROM Board Info structure! (%r)\n", __FUNCTION__, __LINE__, Status));
      Library = EEPROM_FV;
    } else {
      Size = 0;
      Status = GetEepromStructure (EEPROM_FV, "$BrdInfo", (UINT8 **) &FvBoardInfo, &Size);
      if (EFI_ERROR (Status) || (Size == 0) || (FvBoardInfo == NULL)) {
        DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Failed to get FV Board Info structure! (%r)\n", __FUNCTION__, __LINE__, Status));
        Library = EEPROM_EEPROM;
      } else {
        //
        // Compare BoardInfo records
        //
        if ((CompareMem (EepromBoardInfo->manuname, FvBoardInfo->manuname, 16) == 0) &&
            (CompareMem (EepromBoardInfo->brdname,  FvBoardInfo->brdname, 16)  == 0) &&
            (EepromBoardInfo->boardid   == FvBoardInfo->boardid)                     &&
            (EepromBoardInfo->fabid     == FvBoardInfo->fabid)                       &&
            (EepromBoardInfo->ecid      == FvBoardInfo->ecid)                        &&
            (EepromBoardInfo->boardtype == FvBoardInfo->boardtype)) {
          //
          // Get EepromHeader records
          //
          Size = 0;
          Status = GetEepromStructure (EEPROM_EEPROM, "$Eeprom$", (UINT8 **) &EepromEepromHeader, &Size);
          if (EFI_ERROR (Status) || (Size == 0) || (EepromEepromHeader == NULL)) {
            DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Failed to get EEPROM header structure! (%r)\n", __FUNCTION__, __LINE__, Status));
            Library = EEPROM_FV;
          } else {
            Size = 0;
            Status = GetEepromStructure (EEPROM_FV, "$Eeprom$", (UINT8 **) &FvEepromHeader, &Size);
            if (EFI_ERROR (Status) || (Size == 0) || (FvEepromHeader == NULL)) {
              DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Failed to get FV header structure! (%r)\n", __FUNCTION__, __LINE__, Status));
              Library = EEPROM_EEPROM;
            } else {
              //
              // Compare image versions
              //
              if (EepromEepromHeader->version > FvEepromHeader->version) {
                Library = EEPROM_EEPROM;
              } else {
                Library = EEPROM_FV;
              }
            }
          }
        } else {
          //
          // FV gets priority since BoardInfo data doesn't match
          //
          Library = EEPROM_FV;
        }
      }
    }
    //
    // Free resources
    //
    EepromBoardInfo    = EepromFreePool (EepromBoardInfo);
    EepromEepromHeader = EepromFreePool (EepromEepromHeader);
    FvBoardInfo        = EepromFreePool (FvBoardInfo);
    FvEepromHeader     = EepromFreePool (FvEepromHeader);
  }
  //
  // Display current stack pointer
  //
  DisplayStackPointer (__FUNCTION__, __LINE__);
  //
  // If we don't have memory, then bail so we don't take up all of the stack space in NEM.
  //
  if (!MemoryInitialized) {
    goto Exit;
  }
  //
  // We have a validated image and it is not in memory. Copy it in.
  // 1. Ask for the image size so we can allocate a large enough buffer.
  // 2. Read the image into the buffer.
  // 3. Write it to memory.
  // 4. Validate the memory copy.
  //
  DEBUG ((DEBUG_INFO, "%a (#%4d) - Copying %a to EEPROM_MEMORY\n", __FUNCTION__, __LINE__, mEepromLibraryString[Library]));
  ImageSize = GetImageSize (Library);
  if (ImageSize != 0) {
    //
    // Something to copy
    //
    ImageBuffer = EepromAllocatePool (ImageSize);
    if (ImageBuffer == NULL) {
      DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Failed to allocate pool!\n", __FUNCTION__, __LINE__));
    } else {
      //
      // Read in entire image
      //
      Status = ReadEeprom (Library, 0, &ImageSize, ImageBuffer, &EepromInfo);
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Reading image from %a returned %r!\n", __FUNCTION__, __LINE__, mEepromLibraryString[Library], Status));
      } else {
        //
        // Got the image, write to memory
        //
        Status = WriteEeprom (EEPROM_MEMORY, 0, &ImageSize, ImageBuffer, NULL);
        if (EFI_ERROR (Status)) {
          DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Writing image to %a returned %r!\n", __FUNCTION__, __LINE__, mEepromLibraryString[EEPROM_MEMORY], Status));
        } else {
          //
          // Validate memory image
          //
          Status = ValidateEeprom (EEPROM_MEMORY);
          if (EFI_ERROR (Status) && (Status != EFI_MEDIA_CHANGED)) {
            DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Validating image returned %r!\n", __FUNCTION__, __LINE__, Status));
          } else {
            //
            // Return EEPROM_MEMORY
            //
            Library = EEPROM_MEMORY;
          }
        }
      }
    }
  }

Exit:
  //
  // Free resources
  //
  ImageBuffer = EepromFreePool (ImageBuffer);
  if (FirstValidImage == EEPROM_NULL) {
    //
    // Nothing is valid. Return default NULL.
    //
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Failed to find a valid image in list PcdEepromAutoPriority!\n", __FUNCTION__, __LINE__));
    Library = EEPROM_NULL;
  }

  //
  // Display current stack pointer
  //
  DisplayStackPointer (__FUNCTION__, __LINE__);

  if (mEepromLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Returning library %a\n", __FUNCTION__, __LINE__, mEepromLibraryString[Library]));
  return Library;
}

EFI_STATUS
EFIAPI
ValidateEeprom (
  IN       UINT8    LibraryIndex
  )
{
  UINT32                  Count;
  UINT32                  Crc32;
  UINT32                  Crc32Size;
  EEPROM_HEADER          *EepromHeader;
  EEPROM_FUNCTION_INFO    EepromInfo;
  UINT8                  *Hash;
  UINT32                  HashSize;
  UINT16                  HashType;
  UINT8                  *ImageBuffer;
  UINT32                  ImageSize;
  SIGNATURE_DATA         *Signature;
  UINT8                  *SignedHash;
  UINT32                  SignedHashSize;
  EFI_STATUS              Status;
  GENERIC_HEADER         *Structure;
  INTN                    Test;

  if (mEepromLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting...\n", __FUNCTION__, __LINE__));

  //
  // Initialize variables
  //
  ZeroMem (&EepromInfo, sizeof (EEPROM_FUNCTION_INFO));
  Crc32Size               = 0;
  EepromHeader            = NULL;
  EepromInfo.Bus          = PcdGet8 (PcdEepromBus);
  EepromInfo.Address      = PcdGet8 (PcdEepromAddress);
  EepromInfo.LibraryIndex = EEPROM_EEPROM;
  Hash                    = NULL;
  ImageBuffer             = NULL;
  SignedHash              = NULL;
  SignedHashSize          = 0;
  Status                  = EFI_SUCCESS;
  Structure               = NULL;

  //
  // Sanity checks
  //
  if (LibraryIndex >= EEPROM_DATA_LIBRARY_INDEX_MAX) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Library index is larger than %d!\n", __FUNCTION__, __LINE__, EEPROM_DATA_LIBRARY_INDEX_MAX - 1));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  //
  // Get image size
  //
  ImageSize = GetImageSize (LibraryIndex);
  if (ImageSize == 0) {
    //
    // Oops!
    //
    Status = EFI_NOT_FOUND;
    goto Exit;
  }
  //
  // Get a buffer to hold the image
  //
  ImageBuffer = EepromAllocatePool (ImageSize);
  if (ImageBuffer == NULL) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Unable to allocate 0x%08x bytes for the image buffer!\n", __FUNCTION__, __LINE__, ImageSize));
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }
  //
  // Read in the image
  //
  Status = ReadEeprom (LibraryIndex, 0, &ImageSize, ImageBuffer, &EepromInfo);
  if (EFI_ERROR (Status)) {
    //
    // Oops!!!
    //
    goto Exit;
  }
  EepromHeader = (EEPROM_HEADER *) ImageBuffer;
  //
  // Verify structure order
  //
  Count     = 0;
  Structure = (GENERIC_HEADER *) ImageBuffer;
  while ((UINT8 *) Structure < (UINT8 *) (ImageBuffer + ImageSize)) {
    //
    // Increment count
    //
    Count++;
    //
    // Sanity check header
    //
    if (Count == 1) {
      //
      // First structure must be $Eeprom$
      //
      Test = AsciiStrnCmp (Structure->signature, "$Eeprom$", 8);
      //
      // Set CRC32 size
      //
      Crc32Size = EepromHeader->crclength;
    } else if (Count == 2) {
      //
      // Second structure must be $EeprMap
      //
      Test = AsciiStrnCmp (Structure->signature, "$EeprMap", 8);
    } else if (Count == 3) {
      //
      // Third structure must be $BrdInfo
      //
      Test = AsciiStrnCmp (Structure->signature, "$BrdInfo", 8);
    } else {
      //
      // All header signatures begin with $
      //
      Test = AsciiStrnCmp (Structure->signature, "$", 0x01);
    }
    if (Test != 0) {
      //
      // Sanity check failed! Bail.
      //
      DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Image is corrupted!\n", __FUNCTION__, __LINE__));
      Status = EFI_VOLUME_CORRUPTED;
      goto Exit;
    }
    if (AsciiStrnCmp (Structure->signature, "$PromSig", 8) == 0) {
      //
      // Check if this is the last structure
      //
      Signature = (SIGNATURE_DATA *) Structure;
      HashSize  = Signature->length - sizeof (SIGNATURE_DATA);
      Hash      = ((UINT8 *) Signature) + sizeof (SIGNATURE_DATA);
      HashType  = Signature->hashtype;
      if (((UINT8 *) Signature - ImageBuffer) + Signature->length + (Structure->length % 0x10) != ImageSize) {
        //
        // Oops! $PromSig is not the last structure.
        //
        DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: $PromSig structure is not the last structure!\n", __FUNCTION__, __LINE__));
        Status = EFI_VOLUME_CORRUPTED;
        goto Exit;
      }
    }
    //
    // Get next structure on paragraph boundary
    //
    Structure = (GENERIC_HEADER *) ((UINT8 *) Structure + Structure->length + (Structure->length % 0x10));
  }
  //
  // Verify CRC32
  //
  Crc32 = EepromHeader->crc32;
  EepromHeader->crc32 = 0;
  CalculateCrc32 (ImageBuffer, Crc32Size, &EepromHeader->crc32);
  if (EepromHeader->crc32 != Crc32) {
    //
    // Oops!
    //
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: CRC32 check failed! [%08x:%08x]\n", __FUNCTION__, __LINE__, EepromHeader->crc32, Crc32));
    Status = EFI_SECURITY_VIOLATION;
    goto Exit;
  }
  //
  // Verify hash
  //
  Status = EFI_SUCCESS;
  if (HashSize > 0) {
    //
    // Check hash
    //
    Status = SignedHashCheck (LibraryIndex, ImageBuffer, Crc32Size, Signature);
    if (EFI_ERROR (Status) && (Status != EFI_MEDIA_CHANGED)) {
      DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Failed to verify the hash!\n", __FUNCTION__, __LINE__));
      goto Exit;
    }
  }

Exit:
  //
  // Free resources
  //
  ImageBuffer = EepromFreePool (ImageBuffer);
  if (EFI_ERROR (Status) && (Status != EFI_MEDIA_CHANGED)) {
    gImageValidFlag[LibraryIndex] = FALSE;
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - Ending with %r\n", __FUNCTION__, __LINE__, Status));
  } else {
    //
    // Might need to go to a bit flag here to indicate CRC32, hash, and signed hash pass. First round in PEI will only be
    // able to get CRC32 pass since hash is supported by OpenSSL library and it is HUGE.
    //
    gImageValidFlag[LibraryIndex] = TRUE;
  }
  return Status;
}

//
// Desc:        Registers the raw data libraries
// Variables:   None
// Return:      EFI_SUCCESS, anything else will cause an ASSERT
//
EFI_STATUS
EFIAPI
EepromInitConstructor (VOID)
{
  if (mEepromLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting...\n", __FUNCTION__, __LINE__));
  //
  // Initiliaze CRC32 tables
  //
  if (!mCrcInitFlag) InitializeCrc32Table ();

  //
  // Initiliaze Library valid flags pointer from PCD. This uses the fact that the PCD library currently just passes out
  // a pointer to it's internal DB. There is no need to update the PCD, since the pointer already points to the internal
  // PCD copy. If the PCD library changes to include a CRC check of it's data, then we'll have to start using the PcdSetPtr()
  // function to set the internal PCD value.
  //
  gImageValidFlag = PcdGetPtr (PcdEepromLibraryValid);

  return EFI_SUCCESS;
}

