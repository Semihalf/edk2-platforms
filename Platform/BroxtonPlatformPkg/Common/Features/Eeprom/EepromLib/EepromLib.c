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

UINT32       mCrcTableEeprom[256];
BOOLEAN      mCrcInitFlag        = FALSE;
BOOLEAN      mEepromLibDebugFlag = FALSE;

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

  if (!mCrcInitFlag) {
    for (TableEntry = 0; TableEntry < 256; TableEntry++) {
      Value = ReverseBits ((UINT32) TableEntry);
      for (Index = 0; Index < 8; Index++) {
        if ((Value & 0x80000000) != 0) {
          Value = (Value << 1) ^ 0x04C11DB7;
        } else {
          Value = Value << 1;
        }
      }
      mCrcTableEeprom[TableEntry] = ReverseBits (Value);
    }
    mCrcInitFlag = TRUE;
  }
}

UINT32
EFIAPI
StartCrc32 (VOID)
{
  // Table initialized?
  InitializeCrc32Table ();

  return 0xFFFFFFFF;
}

UINT32
EFIAPI
AddToCrc32 (
  IN   VOID     *Data,
  IN   UINTN     DataSize,
  IN   UINT32    Crc
  )
{
  UINT32   Crc32;
  UINTN    Index;
  UINT8   *Ptr;

  Crc32 = Crc;
  Ptr   = (UINT8 *) Data;
  for (Index = 0; Index < DataSize; Index++) {
    Crc32 = (Crc32 >> 8) ^ mCrcTableEeprom[(UINT8) Crc32 ^ Ptr[Index]];
  }
  return Crc32;
}

UINT32
EFIAPI
FinishCrc32 (
  IN   UINT32   Crc32
  )
{
  return (Crc32 ^ 0xFFFFFFFF);
}

/*++

Routine Description:

  The CalculateCrc32 routine.

Arguments:

  Data        - The buffer contaning the data to be processed
  DataSize    - The size of data to be processed
  CrcOut      - A pointer to the caller allocated UINT32 that on
                exits, contains the CRC32 checksum of Data

Returns:

  EFI_SUCCESS               - Calculation is successful.
  EFI_INVALID_PARAMETER     - Data / CrcOut = NULL, or DataSize = 0

--*/
EFI_STATUS
EFIAPI
EepromCalculateCrc32 (
  IN     UINT8    *Data,
  IN     UINTN     DataSize,
  IN OUT UINT32   *CrcOut
  )
{
  UINT32    Crc32;

  if (mEepromLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting...\n", __FUNCTION__, __LINE__));

  // Sanity checks
  if ((DataSize == 0) || (Data == NULL) || (CrcOut == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Crc32   = StartCrc32 ();
  Crc32   = AddToCrc32 (Data, DataSize, Crc32);
  *CrcOut = FinishCrc32 (Crc32);

  return EFI_SUCCESS;
}

VOID
EepromPrintChar (
  IN UINTN     DebugMask,
  IN UINTN     Count,
  IN CHAR16   *Char
)
{
  UINTN Index;

  for (Index = 0; Index < Count; Index++) {
    DEBUG ((DebugMask, "%s", Char));
  }
}

#define DIVIDING_LINE "+----------------------------------------------------+------------------+\n"

VOID
EepromDumpParagraph (
  IN   UINTN   DebugMask,
  IN   VOID   *Ptr,
  IN   UINTN   Count
  )
{
  CHAR8     AsciiBuffer[17];
  UINT8    *Data;
  UINTN     Index;
  UINTN     Paragraphs;
  UINTN     PlaceHolder;
  UINTN     PlaceHolders;

  //
  // Use a different pointer so that the one passed in doesn't change
  //
  Data = (UINT8 *) Ptr;
  //
  // Calcualte the number of paragraphs
  //
  Paragraphs = Count / 16;
  if ((Paragraphs * 16) < Count) {
    Paragraphs++;
  }
  //
  // Calculate the number of columns
  //
  PlaceHolder  = Paragraphs;
  PlaceHolders = 0;
  while (PlaceHolder > 0) {
    PlaceHolders++;
    PlaceHolder >>= 4;
  }

  //
  // Dump the buffer
  //
  if (Count > 0 ) {
    //
    // Print header
    //
    EepromPrintChar (DebugMask, PlaceHolders + 5, L" ");
    DEBUG ((DebugMask, DIVIDING_LINE));
    EepromPrintChar (DebugMask, PlaceHolders + 5, L" ");
    DEBUG ((DebugMask, "| x0 x1 x2 x3  x4 x5 x6 x7  x8 x9 xA xB  xC xD xE xF |      String      |\n"));
    DEBUG ((DebugMask, " +"));
    EepromPrintChar (DebugMask, PlaceHolders + 3, L"-");
    DEBUG ((DebugMask, DIVIDING_LINE));
    //
    // Print data
    //
    for (Index = 0; Index < (Paragraphs * 16); Index++) {
      //
      // Print divider
      //
      if (Index % 0x10 == 0x00) {
        if ((Index > 0) && ((Index / 0x10) % 0x04 == 0x00) && (Paragraphs > 6)) {
          DEBUG ((DebugMask, " +"));
          EepromPrintChar (DebugMask, PlaceHolders + 3, L"-");
          DEBUG ((DebugMask, DIVIDING_LINE));
        }
        DEBUG ((DebugMask, " | %0*xx | ", PlaceHolders, (Index / 0x10)));
      }
      //
      // Print the data or a filler
      //
      if (Index < Count) {
        DEBUG ((DebugMask, "%02x ", Data[Index]));
        if ((Data[Index] < 32) || (Data[Index] > 126)) {
          //
          // Not printable
          //
          AsciiBuffer[(Index % 0x10)] = '.';
        } else {
          //
          // Printable
          //
          AsciiBuffer[(Index % 0x10)] = Data[Index];
        }
      } else {
        DEBUG ((DebugMask, "   "));
        AsciiBuffer[(Index % 0x10)] = ' ';
      }
      //
      // Print break or line end if needed
      //
      if (Index % 0x10 == 0x0F) {
        AsciiBuffer[16] = 0x00;
        DEBUG ((DebugMask, "| %a |\n", AsciiBuffer));
      } else if (Index % 0x04 == 0x03) {
        DEBUG ((DebugMask, " "));
      }
    }
    //
    // Print footer
    //
    DEBUG ((DebugMask, " +"));
    EepromPrintChar (DebugMask, PlaceHolders + 3, L"-");
    DEBUG ((DebugMask, DIVIDING_LINE));
  }
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
  BOOLEAN                *ImageValidFlags;
  UINTN                   PcdPtrSize;
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
  ImageValidFlags         = (BOOLEAN *) PcdGetPtr (PcdEepromLibraryValid);
  PcdPtrSize              = PcdGetSize (PcdEepromLibraryValid);

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
  ImageValidFlags[LibraryIndex] = FALSE;

Exit:
  //
  // Free resources
  //
  Buffer = EepromFreePool (Buffer);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - Ending with %r\n", __FUNCTION__, __LINE__, Status));
  }
  PcdSetPtr (PcdEepromLibraryValid, &PcdPtrSize, ImageValidFlags);
  return Status;
}

/**
  Returns the EEPROM structure data

  @param[in]   LibraryIndex            Which library to use to get the structure
  @param[in]   Signature               Signature of the structure we are looking for
  @param[out]  StructureIndex          Index of where to start looking for the next structure
  @param[out]  Buffer                  Buffer containing the structure data
                                       NOTE: Caller is responsible for freeing this memory.
  @param[out]  Size                    Size of the StructureData buffer

  @retval      EFI_SUCCESS             Structure data found
  @retval      EFI_NOT_FOUND           Structure data not found
  @retval      EFI_INVALID_PARAMETER   Invalid parameter passed in
**/
EFI_STATUS
EFIAPI
GetEepromStructure (
  IN       UINT8      LibraryIndex,
  IN OUT   CHAR8      Signature[EEPROM_SIGNATURE_SIZE],
  IN OUT   UINT32    *StructureIndex,
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
  if (StructureIndex == NULL) {
    Index = 0;
  } else {
    Index = *StructureIndex;
  }
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
        if (StructureIndex != NULL) *StructureIndex = Index;
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

/**
  Returns the EEPROM structure data

  @param[in]   LibraryIndex            Which library to use to get the structure
  @param[in]   Signature               Signature of the structure we are looking for
  @param[out]  StructureIndex          Index of where to start looking for the next structure
  @param[in]   HeaderSize              Size of the structure header
  @param[out]  StructureHeader         Header of the structure we're looking for
                                       NOTE: Caller is responsible for freeing this memory.
  @param[out]  StructureData           Buffer containing the structure data
  @param[out]  DataSize                Size of the StructureData buffer

  @retval      EFI_SUCCESS             Structure data found
  @retval      EFI_NOT_FOUND           Structure data not found
  @retval      EFI_INVALID_PARAMETER   Invalid parameter passed in
**/
EFI_STATUS
EFIAPI
GetEepromStructureData (
  IN       UINT8     *LibraryIndex,
  IN       CHAR8      Signature[EEPROM_SIGNATURE_SIZE],
  IN OUT   UINT32    *StructureIndex,
  IN       UINT32     HeaderSize,
  OUT      UINT8    **StructureHeader,
  OUT      UINT8    **StructureData,
  OUT      UINT32    *DataSize
  )
{
  CHAR8               AsciiData[32];
  UINT8               EepromLibrary;
  GENERIC_HEADER     *Header;
  UINT32              Index;
  UINT32              Size;
  EFI_STATUS          Status;

  if (mEepromLibDebugFlag) DEBUG ((DEBUG_INFO, "%a(#%4d) - Starting...\n", __FUNCTION__, __LINE__));

  //
  // Return the structure data
  // 1. Get valid EEPROM library index
  // 2. Find structure
  // 3. Return structure data
  //
  if (StructureIndex == NULL) {
    Index = 0;
  } else {
    Index = *StructureIndex;
  }
  if (LibraryIndex == NULL) {
    EepromLibrary = GetValidEepromLibrary (FALSE);
  } else {
    EepromLibrary = *LibraryIndex;
  }
  if (EepromLibrary == EEPROM_NULL) {
    DEBUG ((DEBUG_ERROR, "%a(#%4d) - ERROR: Didn't find a valid EEPROM binary!\n", __FUNCTION__, __LINE__));
    Status = EFI_NOT_FOUND;
  } else {
    Header = NULL;
    Size   = 0;
    Status = GetEepromStructure (EepromLibrary, Signature, &Index, (UINT8 **) &Header, &Size);
    if (EFI_ERROR (Status) || (Size == 0)) {
      DEBUG ((DEBUG_ERROR, "%a(#%4d) - ERROR: Didn't find the %a structure in the EERPOM binary!\n", __FUNCTION__, __LINE__, Signature));
      Status = EFI_NOT_FOUND;
    } else {
      Size = Header->length - HeaderSize;
      if (StructureHeader != NULL) *StructureHeader = (UINT8 *) Header;
      if (StructureData != NULL) {
        if (Size == 0) {
          *StructureData = NULL;
        } else {
          *StructureData = (UINT8 *) Header + HeaderSize;
        }
      }
      if (DataSize != NULL) *DataSize = Size;
      if (StructureIndex != NULL) *StructureIndex = Index;
      if (mEepromLibDebugFlag) {
        ZeroMem (AsciiData, 32);
        CopyMem (AsciiData, Header->signature, 8);
        DEBUG ((DEBUG_INFO, "%a(#%4d) - Signature    = %a\n",        __FUNCTION__, __LINE__, AsciiData));
        DEBUG ((DEBUG_INFO, "%a(#%4d) - Version      = %04x:%04x\n", __FUNCTION__, __LINE__, Header->vermajor, Header->verminor));
        DEBUG ((DEBUG_INFO, "%a(#%4d) - Length       = 0x%08x\n",    __FUNCTION__, __LINE__, Header->length));
      }
      Status = EFI_SUCCESS;
    }
  }
  if (mEepromLibDebugFlag) DEBUG ((DEBUG_INFO, "%a(#%4d) - Returning %r...\n", __FUNCTION__, __LINE__, Status));
  return Status;
}

/**
  Returns the EEPROM image size

  @param[in]   LibraryIndex         Which library to use to get the structure

  @retval      UINT32               Size of that image
**/
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
  if (EFI_ERROR (Status) || (AsciiStrnCmp (EepromHeader.signature, EEPROM_HEADER_SIGNATURE, 8) != 0)) {
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
      Size   = 0;
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
  IN       BOOLEAN   CopyToMemory
  )
{
  BOOLEAN                 DxeEepromSecurityFlag;
  UINT8                  *EepromAutoList;
  EEPROM_FUNCTION_INFO    EepromInfo;
  UINT8                   FirstPassImage;
  UINT8                   FirstValidImage;
  UINT8                  *ImageBuffer;
  UINT32                  ImageSize;
  BOOLEAN                *ImageValidFlags;
  UINT8                   index;
  UINT8                   Library;
  UINTN                   PcdPtrSize;
  UINT32                  Size;
  EFI_STATUS              Status;

  if (mEepromLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting[%d]...\n", __FUNCTION__, __LINE__, PcdGetBool (PcdEepromDxeVerificationRequired)));

  //
  // Initialize variables
  //
  ZeroMem (&EepromInfo, sizeof (EEPROM_FUNCTION_INFO));
  DxeEepromSecurityFlag   = PcdGetBool (PcdEepromDxeVerificationRequired);
  EepromAutoList          = (UINT8 *) PcdGetPtr (PcdEepromAutoPriority);
  EepromInfo.Bus          = PcdGet8 (PcdEepromBus);
  EepromInfo.Address      = PcdGet8 (PcdEepromAddress);
  EepromInfo.LibraryIndex = EEPROM_EEPROM;
  FirstPassImage          = EEPROM_NULL;
  FirstValidImage         = EEPROM_NULL;
  ImageBuffer             = NULL;
  ImageValidFlags         = (BOOLEAN *) PcdGetPtr (PcdEepromLibraryValid);
  PcdPtrSize              = PcdGetSize (PcdEepromLibraryValid);

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
  if (mEepromLibDebugFlag) DisplayStackPointer (__FUNCTION__, __LINE__);
  //
  // Loop thru PcdEepromAutoPriority looking for a previously validated image.
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
    if (!InPeiPhase () && DxeEepromSecurityFlag) {
      //
      // DXE security verifiation required. Clear all the valid flags.
      //
      if (mEepromLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Clearing validation flags.\n", __FUNCTION__, __LINE__));
      ImageValidFlags[Library] = FALSE;
    }
    if (ImageValidFlags[Library]) {
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
  if (mEepromLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - FirstValidImage = %a\n", __FUNCTION__, __LINE__, mEepromLibraryString[FirstValidImage]));

  //
  // Display current stack pointer
  //
  if (mEepromLibDebugFlag) DisplayStackPointer (__FUNCTION__, __LINE__);
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
      if (!EFI_ERROR (Status) && (FirstValidImage == EEPROM_NULL)) {
        //
        // This one is valid. Bail.
        //
        FirstValidImage = Library;
      }
      if ((Status == EFI_MEDIA_CHANGED) && (FirstPassImage == EEPROM_NULL)) {
        FirstPassImage = Library;
      }
      //
      // Point to next library
      //
      index++;
    }
  }

  if (FirstValidImage == EEPROM_NULL) {
    if (mEepromLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - No validated images found.\n", __FUNCTION__, __LINE__));
    if (InPeiPhase ()) {
      //
      // We're in PEI and no valid images found. Set flag for DXE security action.
      //
      if (mEepromLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Set flag for DXE security action.\n", __FUNCTION__, __LINE__));
      PcdSetBool (PcdEepromDxeVerificationRequired, TRUE);
    } else {
      if (DxeEepromSecurityFlag) {
        //
        // We didn't find a valid image, we're in DXE, and security action is required. Inform user and perform policy action.
        //
        DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: No valid images found!\n", __FUNCTION__, __LINE__));
        switch (PcdGet8 (PcdEepromSecurityViolationPolicy)) {
          case EEPROM_DEADLOOP:
          default:
            CpuDeadLoop ();
            break;
        }
      }
    }
    if (FirstPassImage == EEPROM_NULL) {
      //
      // We got nothing, so bail
      //
      Library = EEPROM_NULL;
      goto Exit;
    } else {
      Library = FirstPassImage;
      goto Exit;
    }
  }

  //
  // Something got validated.
  //
  if (mEepromLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Validated EEPROM library. Clear flag.\n", __FUNCTION__, __LINE__));
  PcdSetBool (PcdEepromDxeVerificationRequired, FALSE);
  if (!InPeiPhase () && DxeEepromSecurityFlag) {
    //
    // We verified at least one image. Reset the system.
    //
    if (mEepromLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - We're in DXE and verification security flag was set, so do a system reset.\n", __FUNCTION__, __LINE__));
    EepromResetSystem (EfiResetWarm);
    //
    // We shouldn't get here, but have the deadloop to catch it if it does
    //
    CpuDeadLoop ();
  }

  //
  // Display current stack pointer
  //
  if (mEepromLibDebugFlag) DisplayStackPointer (__FUNCTION__, __LINE__);
  //
  // Determine which image to copy to memory
  //
  if (ImageValidFlags[EEPROM_MEMORY]) {
    //
    // Yep. Bail.
    //
    Library = EEPROM_MEMORY;
    goto Exit;
  }
  if (ImageValidFlags[EEPROM_EEPROM]) {
    Library = EEPROM_EEPROM;
  }
  if (ImageValidFlags[EEPROM_FV]) {
    Library = EEPROM_FV;
  }
  if (ImageValidFlags[EEPROM_EEPROM] && ImageValidFlags[EEPROM_FV]) {
    EEPROM_HEADER      *EepromEepromHeader;
    EEPROM_HEADER      *FvEepromHeader;
    //
    // Initialize variables
    //
    EepromEepromHeader = NULL;
    FvEepromHeader     = NULL;
    Library            = EEPROM_NULL;
    //
    // Get EepromHeader records
    //
    Size = 0;
    Status = GetEepromStructure (EEPROM_EEPROM, EEPROM_HEADER_SIGNATURE, NULL, (UINT8 **) &EepromEepromHeader, &Size);
    if (EFI_ERROR (Status) || (Size == 0) || (EepromEepromHeader == NULL)) {
      DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Failed to get EEPROM header structure! (%r)\n", __FUNCTION__, __LINE__, Status));
      Library = EEPROM_FV;
    } else {
      Size = 0;
      Status = GetEepromStructure (EEPROM_FV, EEPROM_HEADER_SIGNATURE, NULL, (UINT8 **) &FvEepromHeader, &Size);
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
    //
    // Free resources
    //
    EepromEepromHeader = EepromFreePool (EepromEepromHeader);
    FvEepromHeader     = EepromFreePool (FvEepromHeader);
  }
  //
  // Display current stack pointer
  //
  if (mEepromLibDebugFlag) DisplayStackPointer (__FUNCTION__, __LINE__);

  //
  // Check to see if we need to copy into memory and not in PEI
  //
  if (!CopyToMemory || InPeiPhase ()) {
    //
    // Nope. Bail.
    //
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
  if ((FirstValidImage == EEPROM_NULL) && (Library == EEPROM_NULL)) {
    //
    // Nothing is valid. Return default NULL.
    //
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Failed to find a valid image in list PcdEepromAutoPriority!\n", __FUNCTION__, __LINE__));
  }

  //
  // Display current stack pointer
  //
  if (mEepromLibDebugFlag) DisplayStackPointer (__FUNCTION__, __LINE__);

  if (mEepromLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Returning library %a\n", __FUNCTION__, __LINE__, mEepromLibraryString[Library]));
  PcdSetPtr (PcdEepromLibraryValid, &PcdPtrSize, ImageValidFlags);
  return Library;
}

EFI_STATUS
EFIAPI
ValidateEeprom (
  IN       UINT8    LibraryIndex
  )
{
  UINT32                  BufferSize;
  UINT32                  Count;
  UINT32                  Crc32;
  UINT32                  Crc32Size;
  EEPROM_HEADER          *EepromHeader;
  EEPROM_FUNCTION_INFO    EepromInfo;
  UINT32                  HashSize;
  UINT8                  *ImageBuffer;
  UINT32                  ImageSize;
  BOOLEAN                *ImageValidFlags;
  UINT32                  Offset;
  UINT32                  OriginalCrc32;
  UINTN                   PcdPtrSize;
  UINT32                  Size;
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
  ImageBuffer             = NULL;
  Status                  = EFI_SUCCESS;
  Structure               = NULL;
  ImageValidFlags         = (BOOLEAN *) PcdGetPtr (PcdEepromLibraryValid);
  PcdPtrSize              = PcdGetSize (PcdEepromLibraryValid);

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
  // Verify structure order
  //
  Count  = 0;
  Offset = 0;
  Status = EFI_SUCCESS;
  while (!EFI_ERROR (Status) && (Offset < ImageSize)) {
    Status = GetNextEepromStructure (LibraryIndex, &Offset, (UINT8 **) &ImageBuffer, &BufferSize);
    if (!EFI_ERROR (Status)) {
      //
      // Increment count
      //
      Count++;
      Structure = (GENERIC_HEADER *) ImageBuffer;
      //
      // Sanity check header
      //
      if (Count == 1) {
        //
        // First structure must be $Eeprom$
        //
        Test = AsciiStrnCmp (Structure->signature, EEPROM_HEADER_SIGNATURE, 8);
      } else if (Count == 2) {
        //
        // Second structure must be $EeprMap
        //
        Test = AsciiStrnCmp (Structure->signature, EEPROM_MAP_SIGNATURE, 8);
      } else if (Count == 3) {
        //
        // Third structure must be $BrdInfo
        //
        Test = AsciiStrnCmp (Structure->signature, EEPROM_BOARD_INFO_SIGNATURE, 8);
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
      if (AsciiStrnCmp (Structure->signature, EEPROM_SIGNATURE_SIGNATURE, 8) == 0) {
        //
        // Check if this is the last structure
        //
        HashSize = Structure->length - sizeof (SIGNATURE_DATA);
        if (Offset < ImageSize) {
          //
          // Oops! $PromSig is not the last structure.
          //
          DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: $PromSig structure is not the last structure!\n", __FUNCTION__, __LINE__));
          Status = EFI_VOLUME_CORRUPTED;
          goto Exit;
        }
      }
    }
  }
  ImageBuffer = EepromFreePool (ImageBuffer);

  //
  // Get $Eeprom$ structure
  //
  Size   = 0;
  Status = GetEepromStructure (LibraryIndex, EEPROM_HEADER_SIGNATURE, NULL, (UINT8 **) &EepromHeader, &Size);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Image is corrupted!\n", __FUNCTION__, __LINE__));
    Status = EFI_VOLUME_CORRUPTED;
    goto Exit;
  }
  //
  // Start CRC32 calculations with $Eeprom$
  //
  OriginalCrc32 = EepromHeader->crc32;
  Crc32Size     = EepromHeader->crclength;
  EepromHeader->crc32 = 0;
  Crc32  = StartCrc32 ();
  Crc32  = AddToCrc32 (EepromHeader, Size, Crc32);
  Offset = Size;
  EepromHeader = EepromFreePool (EepromHeader);
  //
  // Add the rest of the binary
  //
  Size = 1024;
  ImageBuffer = EepromAllocatePool (Size);
  if (ImageBuffer == NULL) {
    //
    // Failed to allocate pool
    //
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Failed to allocate pool for Buffer!\n", __FUNCTION__, __LINE__));
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }
  Status = EFI_SUCCESS;
  while (!EFI_ERROR (Status) && (Offset < Crc32Size)) {
    if ((Offset + Size) > Crc32Size) {
      Size = Crc32Size - Offset;
    }
    Status  = ReadEeprom (LibraryIndex, Offset, &Size, ImageBuffer, &EepromInfo);
    Offset += Size;
    if (!EFI_ERROR (Status)) {
      Crc32 = AddToCrc32 (ImageBuffer, Size, Crc32);
    }
  }
  ImageBuffer = EepromFreePool (ImageBuffer);
  Crc32 = FinishCrc32 (Crc32);
  if (OriginalCrc32 != Crc32) {
    //
    // Oops!
    //
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: CRC32 check failed! [%08x:%08x]\n", __FUNCTION__, __LINE__, OriginalCrc32, Crc32));
    Status = EFI_SECURITY_VIOLATION;
    goto Exit;
  }

  //
  // Verify hash
  //
  if (HashSize > 0) {
    //
    // Check hash
    //
    Status = SignedHashCheck (LibraryIndex);
    if (EFI_ERROR (Status) && (Status != EFI_MEDIA_CHANGED)) {
      DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Failed to verify the hash!\n", __FUNCTION__, __LINE__));
      goto Exit;
    }
  }

Exit:
  //
  // Free resources
  //
  EepromHeader = EepromFreePool (EepromHeader);
  ImageBuffer  = EepromFreePool (ImageBuffer);
  if (EFI_ERROR (Status)) {
    ImageValidFlags[LibraryIndex] = FALSE;
    if (Status != EFI_MEDIA_CHANGED) DEBUG ((DEBUG_ERROR, "%a (#%4d) - Ending with %r\n", __FUNCTION__, __LINE__, Status));
  } else {
    //
    // Might need to go to a bit flag here to indicate CRC32, hash, and signed hash pass. First round in PEI will only be
    // able to get CRC32 pass since hash is supported by OpenSSL library and it is HUGE.
    //
    ImageValidFlags[LibraryIndex] = TRUE;
  }
  PcdSetPtr (PcdEepromLibraryValid, &PcdPtrSize, ImageValidFlags);
  return Status;
}

