/** @file
  EEPROM raw data library instance.

  Copyright (c) 2015 - 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "EepromDataEepromLib.h"

LIST_ENTRY         *gEepromPartsHeadLink = NULL;
EEPROM_PART_INFO   *gEepromParts         = NULL;

EFI_STATUS
EFIAPI
CreateBlockList (
  IN       UINT32        Offset,
  IN       UINT32       *Size,
  IN       VOID         *FunctionInfo,
  IN OUT   BLOCK_LIST   *BlockListHead
  )
{
  UINT8                   Address;
  EEPROM_FUNCTION_INFO   *EepromInfo;
  BLOCK_LIST             *BlockList;
  UINT16                  BlockOffset;
  UINT32                  ReadCount;
  UINT32                  ReadOffset;
  EFI_STATUS              Status;

  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting\n", __FUNCTION__, __LINE__));

  //
  // Initialize variables
  //
  EepromInfo = (EEPROM_FUNCTION_INFO *) FunctionInfo;
  Status     = EFI_SUCCESS;

  //
  // Sanity checks
  //
  if ((Size == NULL) || (FunctionInfo == NULL) || (BlockListHead == NULL)) {
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  //
  // Debug info
  //
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Bus = 0x%02x   Address = 0x%02x\n", __FUNCTION__, __LINE__, EepromInfo->Bus, EepromInfo->Address));

  //
  // Initialize first linked entry
  //
  BlockList = EepromAllocatePool (sizeof (BLOCK_LIST));
  if (BlockList == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }
  BlockList->Address   = EepromInfo->Address;
  BlockList->Signature = BLOCK_LIST_SIGNATURE;
  BlockList->Size      = 0;
  ReadCount            = *Size;
  ReadOffset           = Offset;
  while (ReadCount > 0) {
    Address = GetMapAddress (
                ReadOffset,
                &BlockOffset,
                FunctionInfo
                );
    if (ReadCount == *Size) {
      //
      // Beginning of list search.
      //
      BlockList->Offset = BlockOffset;
    }
    if (Address == BlockList->Address) {
      //
      // Same address range, increment size.
      //
      BlockList->Size++;
    } else if (Address != INVALID_I2C_ADDRESS) {
      //
      // Dump this block of info
      //
      if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Address = 0x%08x\n", __FUNCTION__, __LINE__, BlockList->Address));
      if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Offset  = 0x%08x\n", __FUNCTION__, __LINE__, BlockList->Offset));
      if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Size    = 0x%08x\n", __FUNCTION__, __LINE__, BlockList->Size));
      //
      // New address range, create new PageList.
      //
      InsertTailList (&BlockListHead->Link, &BlockList->Link);
      BlockList = EepromAllocatePool (sizeof (BLOCK_LIST));
      if (BlockList == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto Exit;
      }
      BlockList->Address   = Address;
      BlockList->Offset    = BlockOffset;
      BlockList->Signature = BLOCK_LIST_SIGNATURE;
      BlockList->Size      = 1;
    } else if (BlockList->Size == 0) {
      //
      // GetMapAddress failed to find an address for this offset
      //
      Status = EFI_NO_MAPPING;
      goto Exit;
    }
    //
    // Decrement read counter
    //
    ReadCount--;
    //
    // Increment read offset
    //
    ReadOffset++;
  }
  //
  // Dump this block of info
  //
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Address = 0x%08x\n", __FUNCTION__, __LINE__, BlockList->Address));
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Offset  = 0x%08x\n", __FUNCTION__, __LINE__, BlockList->Offset));
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Size    = 0x%08x\n", __FUNCTION__, __LINE__, BlockList->Size));
  //
  // Insert last record
  //
  InsertTailList (&BlockListHead->Link, &BlockList->Link);

Exit:
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - Ending with %r\n", __FUNCTION__, __LINE__, Status));
  }
  return Status;
}

VOID
EFIAPI
DestroyBlockList (
  IN  BLOCK_LIST  *BlockListHead
  )
{
  BLOCK_LIST    *BlockList;
  LIST_ENTRY    *Node;

  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting\n", __FUNCTION__, __LINE__));

  //
  // Sanity checks
  //
  if (BlockListHead == NULL) {
    goto Exit;
  }
  if (IsListEmpty (&BlockListHead->Link)) {
    goto Exit;
  }

  //
  // Walk block list and free memory
  //
  Node = GetFirstNode (&BlockListHead->Link);
  do {
    BlockList = BLOCK_LIST_FROM_THIS (Node);
    Node      = GetNextNode (&BlockListHead->Link, Node);
    RemoveEntryList (&BlockList->Link);
    BlockList = EepromFreePool (BlockList);
  } while (!IsNull (&BlockListHead->Link, Node));

Exit:
  //
  // Free block list head
  //
  BlockListHead = EepromFreePool (BlockListHead);
  return;
}

VOID
EFIAPI
DumpEepromMap (VOID)
{
  EEPROM_MAP_INFO    *CurrentEepromMap;
  EEPROM_PART_INFO   *CurrentEepromParts;
  LIST_ENTRY         *MapNode;
  LIST_ENTRY         *PartNode;

  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting\n", __FUNCTION__, __LINE__));
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - gEepromPartsHeadLink = 0x%08x\n", __FUNCTION__, __LINE__, gEepromPartsHeadLink));
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d)   - HeadLink->ForwardLink = 0x%08x\n", __FUNCTION__, __LINE__, gEepromPartsHeadLink->ForwardLink));
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d)   - HeadLink->BackLink    = 0x%08x\n", __FUNCTION__, __LINE__, gEepromPartsHeadLink->BackLink));
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - gEepromParts         = 0x%08x\n", __FUNCTION__, __LINE__, gEepromParts));

  if ((gEepromParts != NULL) && (gEepromPartsHeadLink != NULL)) {
    PartNode = GetFirstNode (gEepromPartsHeadLink);
    do {
      CurrentEepromParts = EEPROM_PART_INFO_FROM_THIS (PartNode);
      if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d)   - PartNode            = %08x   HeadLink = %08x\n", __FUNCTION__, __LINE__, PartNode, gEepromPartsHeadLink));
      DEBUG ((DEBUG_INFO, "%a (#%4d)   - EEPROM bus number   = 0x%08x\n", __FUNCTION__, __LINE__, CurrentEepromParts->BusNumber));
      DEBUG ((DEBUG_INFO, "%a (#%4d)   - EEPROM address      = 0x%08x\n", __FUNCTION__, __LINE__, CurrentEepromParts->Address));
      DEBUG ((DEBUG_INFO, "%a (#%4d)   - EEPROM speed        = 0x%08x\n", __FUNCTION__, __LINE__, CurrentEepromParts->PartSpeed));
      DEBUG ((DEBUG_INFO, "%a (#%4d)   - EEPROM part size    = 0x%08x\n", __FUNCTION__, __LINE__, CurrentEepromParts->PartSize));
      DEBUG ((DEBUG_INFO, "%a (#%4d)   - EEPROM block length = 0x%08x\n", __FUNCTION__, __LINE__, CurrentEepromParts->BlockLength));
      DEBUG ((DEBUG_INFO, "%a (#%4d)   - EEPROM page size    = 0x%08x\n", __FUNCTION__, __LINE__, CurrentEepromParts->PageSize));
      DEBUG ((DEBUG_INFO, "%a (#%4d)   - EEPROM master map   = 0x%08x\n", __FUNCTION__, __LINE__, CurrentEepromParts->Master));
      //
      // Dump map records
      //
      MapNode = GetFirstNode (&CurrentEepromParts->MapHeadLink);
      do {
        CurrentEepromMap = EEPROM_MAP_INFO_FROM_THIS (MapNode);
        if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d)     - MapNode = %08x   HeadLink = %08x\n", __FUNCTION__, __LINE__, MapNode, &CurrentEepromParts->MapHeadLink));
        DEBUG ((DEBUG_INFO, "%a (#%4d)     - Address = 0x%08x\n", __FUNCTION__, __LINE__, CurrentEepromMap->Address));
        DEBUG ((DEBUG_INFO, "%a (#%4d)     - Offset  = 0x%08x\n", __FUNCTION__, __LINE__, CurrentEepromMap->Offset));
        //
        // Get next record
        //
        MapNode = GetNextNode (&CurrentEepromParts->MapHeadLink, MapNode);
      } while (!IsNull (&CurrentEepromParts->MapHeadLink, MapNode));
      //
      // Get next record
      //
      PartNode = GetNextNode (gEepromPartsHeadLink, PartNode);
    } while (!IsNull (gEepromPartsHeadLink, PartNode));
  } else {
    DEBUG ((DEBUG_INFO, "%a (#%4d) - WARNING: gEepromParts or gEepromPartsHeadLink is bad!\n", __FUNCTION__, __LINE__));
  }
}

EFI_STATUS
EFIAPI
FillEepromMap (
  IN OUT   VOID      *FunctionInfo
  )
{
  CHAR8                   AsciiBuffer[32];
  UINT8                  *Buffer;
  BOOLEAN                 CheckBuffer;
  EEPROM_MAP_INFO        *CurrentEepromMap;
  EEPROM_PART_INFO       *CurrentEepromParts;
  EEPROM_MAP             *EepromHeader;
  EEPROM_FUNCTION_INFO   *EepromInfo;
  UINT32                  index;
  EEPROM_MAP_RECORD      *MapRecord;
  UINT32                  MaxRecord;
  UINT8                  *Ptr;
  UINT32                  Size;
  EFI_STATUS              Status;

  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting\n", __FUNCTION__, __LINE__));

  //
  // Initialize variables
  //
  Buffer             = NULL;
  CurrentEepromParts = NULL;
  EepromInfo         = NULL;

  //
  // Sanity checks
  //
  if (FunctionInfo == NULL) {
    //
    // EEPROM function info is outside of range. Bail.
    //
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: FunctionInfo == NULL!\n", __FUNCTION__, __LINE__));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  } else {
    EepromInfo = (EEPROM_FUNCTION_INFO *) FunctionInfo;
    if (EepromInfo->Buffer != NULL) {
      CheckBuffer = TRUE;
    } else {
      CheckBuffer = FALSE;
    }
  }
  if (CheckBuffer) {
    //
    // We'll be checking the passed in buffer for the map.
    //
    if ((EepromInfo->Buffer == NULL) || (EepromInfo->Size == 0)) {
      //
      // EEPROM function info is outside of range. Bail.
      //
      DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: FunctionInfo->Buffer == NULL or FunctionInfo->Size == 0!\n", __FUNCTION__, __LINE__));
      Status = EFI_INVALID_PARAMETER;
      goto Exit;
    }
  }
  if ((EepromInfo->Bus > MAX_I2C_BUS) || (EepromInfo->Address > MAX_I2C_ADDRESS)) {
    //
    // EEPROM function info is outside of range. Bail.
    //
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: FunctionInfo->Bus > 7 or FunctionInfo->Address >= 0x80!\n", __FUNCTION__, __LINE__));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  //
  // Debug info
  //
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - gEepromPartsHeadLink = 0x%08x   gEepromParts = 0x%08x\n", __FUNCTION__, __LINE__, gEepromPartsHeadLink, gEepromParts));
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Bus                  = 0x%02x         Address      = 0x%02x\n", __FUNCTION__, __LINE__, EepromInfo->Bus, EepromInfo->Address));

  //
  // Is our map valid?
  //
  if ((gEepromParts != NULL) && (gEepromPartsHeadLink != NULL)) {
    //
    // Parse list of part records to see if we already have this map.
    //
    if (SearchEepromPartList (EepromInfo->Bus, EepromInfo->Address) != NULL) {
      //
      // Yep. Bail.
      //
      if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Already have valid EEPROM map.\n", __FUNCTION__, __LINE__));
      Status = EFI_SUCCESS;
      goto Exit;
    } else {
      if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Failed to find EEPROM map. Trying to add it.\n", __FUNCTION__, __LINE__));
    }
  } else {
    //
    // Make sure our parts buffer is pointing to NULL.
    //
    gEepromPartsHeadLink = EepromFreePool (gEepromPartsHeadLink);
    gEepromParts         = EepromFreePool (gEepromParts);
  }

  //
  // Create read buffer
  //
  if (CheckBuffer) {
    //
    // Use the passed in buffer.
    //
    Buffer = EepromInfo->Buffer;
  } else {
    if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Allocating buffer to hold first 4KB.\n", __FUNCTION__, __LINE__));
    Size   = 4 * 1024;
    Buffer = EepromAllocatePool (Size);
    if (Buffer == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Exit;
    }
  }

  //
  // Fill in map
  //   1. Read in 4KB from PcdEepromBus|PcdEepromAddress.
  //   2. Jump the structures looking for $EeprMap structure.
  //   3. Load EEPROM map records from this structure.
  //
  if (CheckBuffer) {
    Size = EepromInfo->Size;
  } else {
    //
    // Step #1: Load buffer with first 4KB of data from the EEPROM
    //
    if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Reading first 4KB of EEPROM data.\n", __FUNCTION__, __LINE__));
    //
    // Read EEPROM
    //
    Ptr  = Buffer;
    Size = 4 * 1024;
    //
    // Get first 8 bytes
    //
    Status = I2cReadPages (EepromInfo->Bus, EepromInfo->Address, 0, 8, Ptr);
    if (EFI_ERROR (Status)) {
      goto Exit;
    }
    //
    // Sanity check EEPROM contents
    //
    AsciiSPrint (AsciiBuffer, 32, "%8a", (CHAR8 *) Ptr);
    if (AsciiStrnCmp (AsciiBuffer, EEPROM_HEADER_SIGNATURE, 0x08) != 0) {
      //
      // Not a vallid EEPROM image. Bail.
      //
      Status = EFI_NO_MAPPING;
      goto Exit;
    }
    //
    // Read in 4KB
    //
    Ptr    = Buffer;
    Status = I2cReadPages (EepromInfo->Bus, EepromInfo->Address, 0, Size, Ptr);
    if (EFI_ERROR (Status)) {
      goto Exit;
    }
  }

  //
  // Step #2: Search for $EeprMap
  //
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Looking for $EeprMap structure.\n", __FUNCTION__, __LINE__));
  Status       = EFI_NO_MAPPING;
  EepromHeader = (EEPROM_MAP *) Buffer;
  Ptr          = Buffer;
  while (Ptr[0] == '$') {
    AsciiSPrint (AsciiBuffer, 32, "%8a", EepromHeader->signature);
    AsciiBuffer[8] = 0;
    if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Structure = %a @ 0x%08x\n", __FUNCTION__, __LINE__, AsciiBuffer, EepromHeader));
    if (AsciiStrnCmp (EepromHeader->signature, EEPROM_MAP_SIGNATURE, 0x08) != 0) {
      //
      // This is not our structure. Skip to next structure.
      //
      EepromHeader = (EEPROM_MAP *) ((UINT8 *) EepromHeader + EepromHeader->length + (EepromHeader->length % 0x10));
      if ((UINT8 *) EepromHeader > (Buffer + Size)) {
        //
        // Didn't find $EeprMap structure
        //
        Status = EFI_NO_MAPPING;
        break;
      } else {
        continue;
      }
    } else {
      //
      // Found $EeprMap. Create memory buffer.
      //
      if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Allocating buffer to hold new EEPROM map.\n", __FUNCTION__, __LINE__));
      CurrentEepromParts = EepromAllocatePool (sizeof (EEPROM_PART_INFO));
      if (CurrentEepromParts == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto Exit;
      }
      //
      // Copy data into new EEPROM map
      //
      CurrentEepromParts->Signature   = EEPROM_PART_INFO_SIGNATURE;
      CurrentEepromParts->Address     = EepromInfo->Address;
      CurrentEepromParts->BusNumber   = EepromInfo->Bus;
      CurrentEepromParts->PartSpeed   = EepromHeader->speed;
      CurrentEepromParts->PartSize    = EepromHeader->partsize;
      CurrentEepromParts->BlockLength = EepromHeader->blklength;
      CurrentEepromParts->PageSize    = EepromHeader->pagesize;
      CurrentEepromParts->Master      = EepromHeader->master;
      if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - EEPROM bus number   = 0x%08x\n", __FUNCTION__, __LINE__, CurrentEepromParts->BusNumber));
      if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - EEPROM address      = 0x%08x\n", __FUNCTION__, __LINE__, CurrentEepromParts->Address));
      if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - EEPROM speed        = 0x%08x\n", __FUNCTION__, __LINE__, CurrentEepromParts->PartSpeed));
      if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - EEPROM part size    = 0x%08x\n", __FUNCTION__, __LINE__, CurrentEepromParts->PartSize));
      if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - EEPROM block length = 0x%08x\n", __FUNCTION__, __LINE__, CurrentEepromParts->BlockLength));
      if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - EEPROM master map   = 0x%08x\n", __FUNCTION__, __LINE__, CurrentEepromParts->Master));
      Status = EFI_SUCCESS;
      break;
    }
  };
  //
  // Check for error condition
  //
  if (EFI_ERROR (Status)) {
    //
    // Failed to find $EeprMap
    //
    Status = EFI_NO_MAPPING;
    goto Exit;
  }
  //
  // Insert record
  //
  if (gEepromParts == NULL) {
    // First record, setup head link
    gEepromParts         = CurrentEepromParts;
    gEepromPartsHeadLink = EepromAllocatePool (sizeof (LIST_ENTRY));
    if (gEepromPartsHeadLink == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Exit;
    }
    InitializeListHead (gEepromPartsHeadLink);
  }
  InsertTailList (gEepromPartsHeadLink, &CurrentEepromParts->Link);

  //
  // Step #3: Load in EEPROM map records
  //
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Loading EEPROM map.\n", __FUNCTION__, __LINE__));
  MapRecord = (EEPROM_MAP_RECORD *) ((UINT8 *) EepromHeader + sizeof (EEPROM_MAP));
  MaxRecord = (UINT32) (((UINT8 *) EepromHeader + EepromHeader->length) - (UINT8 *) MapRecord) / sizeof (EEPROM_MAP_RECORD);
  index     = 0;
  while (index < MaxRecord) {
    if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d)   - 0x%08x of 0x%08x @ 0x%08x\n", __FUNCTION__, __LINE__, index, MaxRecord, MapRecord));
    //
    // Need a new link record
    //
    CurrentEepromMap = EepromAllocatePool (sizeof (EEPROM_MAP_INFO));
    if (CurrentEepromMap == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Exit;
    }
    //
    // Update memory EEPROM map
    //
    CurrentEepromMap->Signature = EEPROM_MAP_INFO_SIGNATURE;
    CurrentEepromMap->Address   = MapRecord->address;
    CurrentEepromMap->Offset    = MapRecord->offset;
    if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d)   -       Map[%04x] = %08x\n", __FUNCTION__, __LINE__, index, CurrentEepromMap));
    if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d)   -   Address[%04x] = %08x\n", __FUNCTION__, __LINE__, index, CurrentEepromMap->Address));
    if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d)   -    Offset[%04x] = %08x\n", __FUNCTION__, __LINE__, index, CurrentEepromMap->Offset));
    //
    // Insert record
    //
    if (index == 0) {
      // First record, setup head link
      InitializeListHead (&CurrentEepromParts->MapHeadLink);
    }
    InsertTailList (&CurrentEepromParts->MapHeadLink, &CurrentEepromMap->Link);
    //
    // Point to next record
    //
    MapRecord++;
    index++;
  }

  //
  // Set flag
  //
  Status = EFI_SUCCESS;

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

UINT8
EFIAPI
GetMapAddress (
   IN      UINT32    ReadOffset,
   OUT     UINT16   *BlockOffset,
   IN      VOID     *FunctionInfo
   )
{
  EEPROM_FUNCTION_INFO   *EepromInfo;
  EEPROM_MAP_INFO        *EepromMapInfo;
  EEPROM_PART_INFO       *EepromPartInfo;

  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting\n", __FUNCTION__, __LINE__));

  //
  // Initialize variables
  //
  EepromInfo = (EEPROM_FUNCTION_INFO *) FunctionInfo;

  //
  // Sanity Checks
  //
  if ((BlockOffset == NULL) || (FunctionInfo == NULL)){
    goto Exit;
  }

  //
  // Get part info
  //
  EepromPartInfo = SearchEepromPartList (EepromInfo->Bus, EepromInfo->Address);
  if (EepromPartInfo != NULL) {
    //
    // Found the right EEPROM part record.
    //
    EepromMapInfo = SearchEepromMapList (EepromPartInfo, INVALID_I2C_ADDRESS, ReadOffset);
    if (EepromMapInfo != NULL) {
      //
      // Found the right EEPROM part/map record. Return block offset and I2C address.
      //
      *BlockOffset = (UINT16) ((ReadOffset % EepromPartInfo->BlockLength) & 0xFFFF);
      return EepromMapInfo->Address;
    }
  }

Exit:
  return INVALID_I2C_ADDRESS;
}

EFI_STATUS
EFIAPI
I2cReadPages (
  IN       UINT8     Bus,
  IN       UINT8     Address,
  IN       UINT16    Offset,
  IN       UINT32    Size,
  IN OUT   UINT8    *Buffer
  )
{
  UINT32              CurrentCount;
  UINT16              CurrentOffset;
  EEPROM_PART_INFO   *EepromPartInfo;
  UINT32              I2cBaseAddress;
  UINT16              index;
  UINT8               PageOffset[2];
  UINT16              PageSize;
  UINT8               PartSpeed;
  UINT8              *Ptr;
  UINT32              ReadCount;
  UINT16              RetryCount;
  EFI_STATUS          Status;

  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting\n", __FUNCTION__, __LINE__));

  //
  // Initialize variables
  //
  CurrentCount   = 0;
  EepromPartInfo = SearchEepromPartList (Bus, Address);
  I2cBaseAddress = 0;
  Ptr            = Buffer;
  RetryCount     = 3;

  //
  // Sanity checks
  //
  if (Ptr == NULL) {
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  if ((Bus > MAX_I2C_BUS) || (Address > MAX_I2C_ADDRESS)) {
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  //
  // Set page size & part speed
  //
  if (EepromPartInfo == NULL) {
    //
    // No page size available. Use default size.
    //
    PageSize  = DEFAULT_PAGE_SIZE;
    PartSpeed = Standard_Speed; // default to 100KHz (Standard speed)
  } else {
    PageSize  = EepromPartInfo->PageSize;
    PartSpeed = EepromPartInfo->PartSpeed;
  }
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Bus          = %02x         Address    = %02x\n", __FUNCTION__, __LINE__, Bus, Address));
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Offset       = %08x   Size       = %08x\n", __FUNCTION__, __LINE__, Offset, Size));
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - PageSize     = %02x         PartSpeed  = %02x\n", __FUNCTION__, __LINE__, PageSize, PartSpeed));
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Buffer       = %08x\n", __FUNCTION__, __LINE__, Buffer));
  //
  // Init I2C controller
  //
  Status = I2cInit (Bus, Address, PartSpeed, &I2cBaseAddress);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - I2cInit() = %r\n", __FUNCTION__, __LINE__, Status));
    goto Exit;
  }
  //
  // Read data from part
  //
  while (CurrentCount < Size) {
    //
    // Set offset
    //
    CurrentOffset = (UINT16) (Offset + CurrentCount);
    //
    // Calculate ReadCount
    //
    ReadCount = PageSize;
    if ((Size - CurrentCount) < PageSize) {
      ReadCount = Size - CurrentCount;
    }
    //
    // Adjust for page boundaries
    //
    if (((CurrentOffset % PageSize) + ReadCount) > PageSize) {
      //
      // Read to page boundary
      //
      ReadCount = PageSize - (CurrentOffset % PageSize);
    }
    if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Size         = %08x\n", __FUNCTION__, __LINE__, Size));
    if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - CurrentCount = %08x\n", __FUNCTION__, __LINE__, CurrentCount));
    if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - PageSize     = %08x\n", __FUNCTION__, __LINE__, PageSize));
    if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - ReadCount    = %08x\n", __FUNCTION__, __LINE__, ReadCount));
    //
    // Set offset
    //
    PageOffset[0] = (CurrentOffset >> 8) & 0xFF;   // MSB offset
    PageOffset[1] = (CurrentOffset >> 0) & 0xFF;   // LSB offset
    Status = I2cSetOffset (I2cBaseAddress, PageOffset, 2);
    if (EFI_ERROR (Status)) {
      if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_ERROR, "%a (#%4d) - I2cSetOffset() = %r [%02x:%02x:%04x]\n", __FUNCTION__, __LINE__, Status, Bus, Address, (Offset + CurrentCount)));
      goto Exit;
    }
    //
    // Read a page
    //
    for (index = 0; index < ReadCount; index++) {
      if ((index == 0) && (ReadCount - 1 != index)) {
        // First byte, but not only byte
        Status = I2cRead (I2cBaseAddress, Ptr, TRUE, FALSE);
      } else if ((index == 0) && (ReadCount - 1 == index)) {
        // First and only byte
        Status = I2cRead (I2cBaseAddress, Ptr, TRUE, TRUE);
      } else if (ReadCount - 1 == index) {
        // Last byte of page
        Status = I2cRead (I2cBaseAddress, Ptr, FALSE, TRUE);
      } else {
        // Everything else
        Status = I2cRead (I2cBaseAddress, Ptr, FALSE, FALSE);
      }
      if (EFI_ERROR (Status)) {
        //
        // Something went wrong. Bail from this for loop.
        //
        DEBUG ((DEBUG_ERROR, "%a (#%4d) - I2cRead() = %r\n", __FUNCTION__, __LINE__, Status));
        break;
      }
      Ptr++;
    }
    //
    // Check for error condition and try it again
    //
    if (Status == EFI_NO_RESPONSE) {
      //
      // Nobody home at this slave address
      //
      goto Exit;
    }
    if (EFI_ERROR (Status)) {
      if (RetryCount-- > 0) {
        //
        // Try it again.
        //
        Ptr = &Buffer[CurrentCount];
      } else {
        DEBUG ((DEBUG_ERROR, "%a (#%4d) - Offset = %08x\n", __FUNCTION__, __LINE__, (Offset + CurrentCount + index)));
        goto Exit;
      }
    } else {
      //
      // Update variables
      //
      CurrentCount += ReadCount;
    }
  }

Exit:
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - Ending with %r\n", __FUNCTION__, __LINE__, Status));
  }
  return Status;
}

EFI_STATUS
EFIAPI
I2cWritePages (
  IN       UINT8      Bus,
  IN       UINT8      Address,
  IN       UINT16     Offset,
  IN       UINT32     Size,
  IN OUT   UINT8     *Buffer
  )
{
  UINT32              CurrentCount;
  UINT16              CurrentOffset;
  EEPROM_PART_INFO   *EepromPartInfo;
  UINT32              I2cBaseAddress;
  UINT16              index;
  UINT8               PageOffset[2];
  UINT16              PageSize;
  UINT8               PartSpeed;
  UINT8              *Ptr;
  UINT16              RetryCount;
  EFI_STATUS          Status;
  UINT32              WriteCount;

  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting\n", __FUNCTION__, __LINE__));

  //
  // Initialize variables
  //
  CurrentCount   = 0;
  EepromPartInfo = SearchEepromPartList (Bus, Address);
  I2cBaseAddress = 0;
  Ptr            = Buffer;
  RetryCount     = 3;

  //
  // Sanity checks
  //
  if (Ptr == NULL) {
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  if ((Bus > MAX_I2C_BUS) || (Address > MAX_I2C_ADDRESS)) {
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  //
  // Set page size & part speed
  //
  if (EepromPartInfo == NULL) {
    //
    // No page size available. Assume 32 bytes per page.
    //
    PageSize  = 32;
    PartSpeed = 1; // default to 100KHz (Standard speed)
  } else {
    PageSize  = EepromPartInfo->PageSize;
    PartSpeed = EepromPartInfo->PartSpeed;
  }
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Bus          = %02x         Address    = %02x\n", __FUNCTION__, __LINE__, Bus, Address));
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Offset       = %08x   Size       = %08x\n", __FUNCTION__, __LINE__, Offset, Size));
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - PageSize     = %02x         PartSpeed  = %02x\n", __FUNCTION__, __LINE__, PageSize, PartSpeed));
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Buffer       = %08x\n", __FUNCTION__, __LINE__, Buffer));
  //
  // Init I2C controller
  //
  Status = I2cInit (Bus, Address, PartSpeed, &I2cBaseAddress);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - I2cInit() = %r\n", __FUNCTION__, __LINE__, Status));
    goto Exit;
  }
  //
  // Write data to part
  //
  while (CurrentCount < Size) {
    //
    // Set pointer
    //
    Ptr = &Buffer[CurrentCount];
    //
    // Set offset
    //
    CurrentOffset = (UINT16) (Offset + CurrentCount);
    //
    // Calculate WriteCount
    //
    WriteCount = PageSize;
    if ((Size - CurrentCount) < PageSize) {
      WriteCount = Size - CurrentCount;
    }
    //
    // Adjust for page boundaries
    //
    if (((CurrentOffset % PageSize) + WriteCount) > PageSize) {
      //
      // Write to page boundary
      //
      WriteCount = PageSize - (CurrentOffset % PageSize);
    }
    if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Size         = %08x\n", __FUNCTION__, __LINE__, Size));
    if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - CurrentCount = %08x\n", __FUNCTION__, __LINE__, CurrentCount));
    if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - PageSize     = %08x\n", __FUNCTION__, __LINE__, PageSize));
    if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - WriteCount   = %08x\n", __FUNCTION__, __LINE__, WriteCount));
    //
    // Set offset
    //
    PageOffset[0] = (CurrentOffset >> 8) & 0xFF;   // MSB offset
    PageOffset[1] = (CurrentOffset >> 0) & 0xFF;   // LSB offset
    Status = I2cSetOffset (I2cBaseAddress, PageOffset, 2);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a (#%4d) - I2cSetOffset() = %r\n", __FUNCTION__, __LINE__, Status));
      goto Exit;
    }
    //
    // Write a page
    //
    for (index = 0; index < WriteCount; index++) {
      if (WriteCount - 1 == index) {
        // Last byte of the page
        Status = I2cWrite (I2cBaseAddress, *Ptr, FALSE, TRUE);
      } else {
        // Everything else
        Status = I2cWrite (I2cBaseAddress, *Ptr, FALSE, FALSE);
      }
      if (EFI_ERROR (Status)) {
        //
        // Something went wrong. Bail from this for loop.
        //
        DEBUG ((DEBUG_ERROR, "%a (#%4d) - I2cWrite() = %r\n", __FUNCTION__, __LINE__, Status));
        break;
      }
      Ptr++;
    }
    if (Status == EFI_NO_RESPONSE) {
      //
      // Nobody home at this slave address
      //
      goto Exit;
    }
    //
    // Check for error condition and try it again
    //
    if (EFI_ERROR (Status)) {
      if (RetryCount-- == 0) {
        DEBUG ((DEBUG_ERROR, "%a (#%4d) - Offset = %08x\n", __FUNCTION__, __LINE__, (Offset + CurrentCount + index)));
        goto Exit;
      }
    } else {
      //
      // Update variables
      //
      CurrentCount += WriteCount;
      if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - CurrentCount = %08x   WriteCount = %08x\n", __FUNCTION__, __LINE__, CurrentCount, WriteCount));
    }
    //
    // Make sure write is done
    //
    Status = I2cPoll (I2cBaseAddress);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a (#%4d) - I2cPoll() = %r\n", __FUNCTION__, __LINE__, Status));
      goto Exit;
    }
  }

Exit:
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - Ending with %r\n", __FUNCTION__, __LINE__, Status));
  }
  return Status;
}

EFI_STATUS
EFIAPI
LoadEepromMap (VOID)
{
//KES:    //
//KES:    // Initialize variables
//KES:    //
//KES:    gEepromParts         = (EEPROM_PART_INFO *) (UINTN) PcdGet64 (PcdEepromParts);
//KES:    gEepromPartsHeadLink = (LIST_ENTRY *) (UINTN) PcdGet64 (PcdEepromPartsHeadLink);
//KES:
//KES:    //
//KES:    // Load from HOB if present
//KES:    //
//KES:    if (PcdGetBool (PcdEepromMapHobValid)) {
//KES:      //
//KES:      // HOB is valid, load it into memory.
//KES:      //
//KES:    }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
ReadEepromBlockList (
  IN OUT   UINT8        *Buffer,
  IN       VOID         *FunctionInfo,
  IN       BLOCK_LIST   *BlockListHead
  )
{
  BLOCK_LIST             *CurrentBlockList;
  EEPROM_FUNCTION_INFO   *EepromInfo;
  LIST_ENTRY             *Node;
  UINT8                  *Ptr;
  UINT32                  ReadCount;
  EFI_STATUS              Status;

  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting\n", __FUNCTION__, __LINE__));

  //
  // Initialize variables
  //
  EepromInfo = (EEPROM_FUNCTION_INFO *) FunctionInfo;
  Ptr        = Buffer;
  ReadCount  = 0;
  Status     = EFI_SUCCESS;

  //
  // Sanity checks
  //
  if ((Buffer == NULL) || (FunctionInfo == NULL) || (BlockListHead == NULL)) {
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  //
  // Read data into buffer
  //
  DEBUG ((DEBUG_INFO, "%a (#%4d) - Reading from EEPROM...\n", __FUNCTION__, __LINE__));
  Node = GetFirstNode (&BlockListHead->Link);
  do {
    //
    // Read in a block
    //
    CurrentBlockList = BLOCK_LIST_FROM_THIS (Node);
    if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) -               buffer @ %08x\n", __FUNCTION__, __LINE__, Buffer));
    if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) -                  bus = %08x\n", __FUNCTION__, __LINE__, EepromInfo->Bus));
    if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) -              address = %08x\n", __FUNCTION__, __LINE__, CurrentBlockList->Address));
    if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) -               offset = %08x\n", __FUNCTION__, __LINE__, CurrentBlockList->Offset));
    if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) -                  ptr @ %08x\n", __FUNCTION__, __LINE__, Ptr));
    Status = I2cReadPages (EepromInfo->Bus, CurrentBlockList->Address, CurrentBlockList->Offset, CurrentBlockList->Size, Ptr);
    if (EFI_ERROR (Status)) {
      goto Exit;
    }
    //
    // Get next record
    //
    Ptr += CurrentBlockList->Size;
    Node = GetNextNode (&BlockListHead->Link, Node);
  } while (!IsNull (&BlockListHead->Link, Node));

Exit:
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - Ending with %r\n", __FUNCTION__, __LINE__, Status));
  }
  return Status;
}

//
// Desc:        Reads from the EEPROM and copies to the passed in buffer.
// Variables:   Offset         Start copying from the offset
//              Size           Size of the buffer and the number of bytes to copy
//                             - If size = 0, then return image size
//              Buffer         Storage buffer for the copied data from the EEPROM
//              FunctionInfo   Pointer to function specific data
// Return:      EFI_SUCCESS             Data copied successfully
//              EFI_UNSUPPORTED         This function is not supported
//              EFI_INVALID_PARAMETER   One of the parameters is invalid
//              EFI_NOT_READY           Called before all necessary protocols available
//              EFI_DEVICE_ERROR        Communication error with device
//
EFI_STATUS
EFIAPI
ReadEepromEeprom (
  IN       UINT32    Offset,
  IN OUT   UINT32   *Size,
  IN OUT   UINT8    *Buffer,
  IN OUT   VOID     *FunctionInfo
  )
{
  BLOCK_LIST             *BlockList;
  EEPROM_FUNCTION_INFO   *EepromInfo;
  EEPROM_PART_INFO       *EepromPartInfo;
  EFI_STATUS              Status;

  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting\n", __FUNCTION__, __LINE__));

  //
  // Initialize variables
  //
  BlockList            = NULL;
  EepromInfo           = (EEPROM_FUNCTION_INFO *) FunctionInfo;
  EepromPartInfo       = NULL;
  Status               = EFI_SUCCESS;
  LoadEepromMap ();

  //
  // Sanity checks
  //
  // NOTE: Input parameters already checked in wrapper function.
  //
  if (FunctionInfo == NULL) {
    //
    // This needs to be pointing to something. Bail.
    //
    if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_ERROR, "%a (#%4d) - FunctionInfo == NULL!\n", __FUNCTION__, __LINE__));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  if (EepromInfo->LibraryIndex != EEPROM_EEPROM) {
    if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_ERROR, "%a (#%4d) - LibraryIndex != EEPROM_EEPROM!\n", __FUNCTION__, __LINE__));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  //
  // Scan I2C bus for images?
  //
  if (EepromInfo->ScanSize != NULL) {
    Status = ScanI2cBusForImages (EepromInfo->Bus, &(EepromInfo->ScanBuffer), EepromInfo->ScanSize);
    goto Exit;
  }

  //
  // Make sure we have the EEPROM map for this part
  //
  Status = FillEepromMap (FunctionInfo);
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  //
  // Dump current mappings
  //
  DumpEepromMap ();

  //
  // Do they just want the size of the EEPROM part?
  //
  if (*Size == 0) {
    //
    // Return EEPROM part size
    //
    EepromPartInfo = SearchEepromPartList (EepromInfo->Bus, EepromInfo->Address);
    if (EepromPartInfo == NULL) {
      //
      // Failed to find part info. Bail.
      //
      Status = EFI_NO_MAPPING;
    } else {
      //
      // Return EEPROM part size.
      //
      *Size = EepromPartInfo->PartSize;
      Status = EFI_BUFFER_TOO_SMALL;
    }
    goto Exit;
  }

  //
  // Initialize block list head
  //
  BlockList = EepromAllocatePool (sizeof (BLOCK_LIST));
  InitializeListHead (&BlockList->Link);

  //
  // Create list of blocks to read
  //
  Status = CreateBlockList (
             Offset,         // Offset in EEPROM to start reading data from
             Size,           // Number of bytes to read
             FunctionInfo,   // Pointer to function specific data
             BlockList       // Head block list
             );
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - CreateBlockList() = %r\n", __FUNCTION__, __LINE__, Status));
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  //
  // Read the list into the buffer
  //
  Status = ReadEepromBlockList (
             Buffer,
             FunctionInfo,
             BlockList
             );
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - ReadEepromBlockList() = %r\n", __FUNCTION__, __LINE__, Status));
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

Exit:
  //
  // Free resources
  //
  DestroyBlockList (BlockList);

  if (EFI_ERROR (Status) && (Status != EFI_BUFFER_TOO_SMALL)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - Ending with %r\n", __FUNCTION__, __LINE__, Status));
  }
  //
  // Save off pointers for use across LIB instances
  //
  SaveEepromMap ();

  return Status;
}

EFI_STATUS
EFIAPI
SaveEepromMap (VOID)
{
  //
  // Initialize variables
  //
  PcdSet64S (PcdEepromParts,         (UINTN) gEepromParts);
  PcdSet64S (PcdEepromPartsHeadLink, (UINTN) gEepromPartsHeadLink);

  //
  // Save to HOB if needed
  //
  if (!PcdGetBool (PcdEepromMapHobValid)) {
    //
    // HOB is not valid, load it into HOB.
    //

    //
    // Toggle flag
    //
    PcdSetBoolS (PcdEepromMapHobValid, TRUE);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
ScanI2cBusForImages (
  IN       UINT8     I2cBus,
  IN OUT   UINT8   **Addresses,
  IN OUT   UINT8    *AddressesSize
  )
{
  BOOLEAN         AddressArray[MAX_I2C_ADDRESS];
  UINT8           AddressCount;
  EEPROM_HEADER   EepromHeader;
  UINT8           index;
  EFI_STATUS      Status;

  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting\n", __FUNCTION__, __LINE__));

  //
  // Initialize variables
  //
  ZeroMem (AddressArray, MAX_I2C_ADDRESS);
  ZeroMem (&EepromHeader, sizeof (EEPROM_HEADER));
  AddressCount = 0;
  Status       = EFI_SUCCESS;

  //
  // Sanity checks
  //
  if (Addresses == NULL) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Addresses buffer is NULL!\n", __FUNCTION__, __LINE__));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  if (AddressesSize == NULL) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: AddressesSize buffer is NULL!\n", __FUNCTION__, __LINE__));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  //
  // Make sure Addresses is empty
  //
  *Addresses     = EepromFreePool (*Addresses);
  *AddressesSize = 0;

  //
  // Scan bus for possible images
  //
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Scanning bus %02x.\n", __FUNCTION__, __LINE__, I2cBus));
  for (index = 0; index < MAX_I2C_ADDRESS; index++) {
    //
    // Check for somebody home
    //
    if (mEepromDataLibDebugFlag &&((index % 0x10) == 0)) DEBUG ((DEBUG_INFO, "."));
    Status = I2cReadPages (I2cBus, index, 0, sizeof (EEPROM_HEADER), (UINT8 *) &EepromHeader);
    if (!EFI_ERROR (Status) && (AsciiStrnCmp (EepromHeader.signature, EEPROM_HEADER_SIGNATURE, 0x08) == 0)) {
      //
      // Update array and count, since this devices starts with $Eeprom$
      //
      AddressArray[index] = TRUE;
      AddressCount++;
    }
  }
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, " [%d]\n", AddressCount));

  //
  // Give return values
  //
  *AddressesSize = AddressCount;

  //
  // Scan array for possible images
  //
  if (AddressCount > 0) {
    //
    // Allocate return buffer
    //
    *Addresses = EepromAllocatePool (AddressCount);
    if (*Addresses == NULL) {
      DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Unable to allocate buffer for Addresses!\n", __FUNCTION__, __LINE__));
      Status = EFI_OUT_OF_RESOURCES;
      *AddressesSize = 0;
      goto Exit;
    }
    //
    // Fill return buffer
    //
    for (index = MAX_I2C_ADDRESS; index > 0; index--) {
      if (AddressArray[index - 1]) {
        //
        // Here's one
        //
        if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Possible image on bus %02x, address %02x.\n", __FUNCTION__, __LINE__, I2cBus, index - 1));
        (*Addresses)[--AddressCount] = index - 1;
        if (AddressCount == 0) {
          //
          // We've accounted for them all. Bail.
          //
          Status = EFI_SUCCESS;
          goto Exit;
        }
      }
    }
  }

Exit:
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - Ending with %r\n", __FUNCTION__, __LINE__, Status));
  }
  return Status;
}

EEPROM_MAP_INFO *
EFIAPI
SearchEepromMapList (
  IN  EEPROM_PART_INFO   *EepromPartInfo,
  IN  UINT8               Address,
  IN  UINT32              Offset
  )
{
  EEPROM_MAP_INFO   *EepromMapInfo;
  LIST_ENTRY        *Node;

  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting\n", __FUNCTION__, __LINE__));

  //
  // Sanity checks
  //
  if (EepromPartInfo == NULL) {
    goto Exit;
  }
  if ((Address > MAX_I2C_ADDRESS) && (Address != INVALID_I2C_ADDRESS)) {
    goto Exit;
  }

  Node = GetFirstNode (&EepromPartInfo->MapHeadLink);
  do {
    EepromMapInfo = EEPROM_MAP_INFO_FROM_THIS (Node);
    if (EepromMapInfo->Address == Address) {
      //
      // Found the right Address. Bail.
      //
      return EepromMapInfo;
    }
    if (Address == INVALID_I2C_ADDRESS) {
      //
      // Looking for an offset match.
      //
      if ((Offset >= EepromMapInfo->Offset) && (Offset < (EepromMapInfo->Offset + EepromPartInfo->BlockLength))) {
        //
        // Found the right offset. Bail.
        //
        return EepromMapInfo;
      }
    }
    //
    // Get next record
    //
    Node = GetNextNode (&EepromPartInfo->MapHeadLink, Node);
  } while (!IsNull (&EepromPartInfo->MapHeadLink, Node));

Exit:
  return NULL;
}

EEPROM_PART_INFO *
EFIAPI
SearchEepromPartList (
  IN  UINT8    Bus,
  IN  UINT8    Address
  )
{
  EEPROM_MAP_INFO    *EepromMapInfo;
  EEPROM_PART_INFO   *EepromPartInfo;
  LIST_ENTRY         *Node;

  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting\n", __FUNCTION__, __LINE__));
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - gEepromParts         = %08x\n", __FUNCTION__, __LINE__, gEepromParts));
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - gEepromPartsHeadLink = %08x\n", __FUNCTION__, __LINE__, gEepromPartsHeadLink));

  //
  // Sanity checks
  //
  if ((gEepromParts == NULL) || (gEepromPartsHeadLink == NULL)) {
    return NULL;
  }
  if ((Bus > MAX_I2C_BUS) || (Address > MAX_I2C_ADDRESS)) {
    return NULL;
  }

  Node = GetFirstNode (gEepromPartsHeadLink);
  do {
    if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d)   - Node               = %08x\n", __FUNCTION__, __LINE__, Node));
    EepromPartInfo = EEPROM_PART_INFO_FROM_THIS (Node);
    if (EepromPartInfo->BusNumber == Bus) {
      //
      // Is this our root address?
      //
      if (EepromPartInfo->Address == Address) {
        //
        // Yep.
        //
        return EepromPartInfo;
      }
      //
      // Found the right Bus, is it the right address?
      //
      EepromMapInfo = SearchEepromMapList (
                        EepromPartInfo,
                        Address,
                        0x00000000
                        );
      if (EepromMapInfo == NULL) {
        //
        // Not our record
        //
      } else {
        //
        // Found the right record. Bail.
        //
        return EepromPartInfo;
      }
    }
    //
    // Get next record
    //
    Node = GetNextNode (gEepromPartsHeadLink, Node);
  } while (!IsNull (gEepromPartsHeadLink, Node));
  return NULL;
}

EFI_STATUS
EFIAPI
WriteEepromBlockList (
  IN   UINT8        *Buffer,
  IN   VOID         *FunctionInfo,
  IN   BLOCK_LIST   *BlockListHead
  )
{
  BLOCK_LIST             *CurrentBlockList;
  EEPROM_FUNCTION_INFO   *EepromInfo;
  LIST_ENTRY             *Node;
  UINT8                  *Ptr;
  UINT32                  ReadCount;
  EFI_STATUS              Status;

  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting\n", __FUNCTION__, __LINE__));

  //
  // Initialize variables
  //
  EepromInfo = (EEPROM_FUNCTION_INFO *) FunctionInfo;
  ReadCount  = 0;
  Status     = EFI_SUCCESS;

  //
  // Sanity checks
  //
  if ((Buffer == NULL) || (FunctionInfo == NULL) || (BlockListHead == NULL)) {
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  //
  // Write data from buffer
  //
  DEBUG ((DEBUG_INFO, "%a (#%4d) - Writing to EEPROM...\n", __FUNCTION__, __LINE__));
  Node = GetFirstNode (&BlockListHead->Link);
  do {
    //
    // Write out a block
    //
    CurrentBlockList = BLOCK_LIST_FROM_THIS (Node);
    Ptr              = Buffer + CurrentBlockList->Offset;
    Status = I2cWritePages (EepromInfo->Bus, CurrentBlockList->Address, CurrentBlockList->Offset, CurrentBlockList->Size, Ptr);
    if (EFI_ERROR (Status)) {
      goto Exit;
    }
    //
    // Get next record
    //
    Node = GetNextNode (&BlockListHead->Link, Node);
  } while (!IsNull (&BlockListHead->Link, Node));

Exit:
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - Ending with %r\n", __FUNCTION__, __LINE__, Status));
  }
  return Status;
}

//
// Desc:        Writes to the EEPROM and copies to the passed in buffer.
// Variables:   Offset         Start copying from the offset
//              Size           Size of the buffer and the number of bytes to copy
//              Buffer         Storage buffer for the copied data to the EEPROM
//              FunctionInfo   Pointer to function specific data
// Return:      EFI_SUCCESS             Data copied successfully
//              EFI_UNSUPPORTED         This function is not supported
//              EFI_INVALID_PARAMETER   One of the parameters is invalid
//              EFI_NOT_READY           Called before all necessary protocols available
//              EFI_DEVICE_ERROR        Communication error with device
//
EFI_STATUS
EFIAPI
WriteEepromEeprom (
  IN       UINT32    Offset,
  IN OUT   UINT32   *Size,
  IN OUT   UINT8    *Buffer,
  IN OUT   VOID     *FunctionInfo
  )
{
  BLOCK_LIST             *BlockList;
  EEPROM_FUNCTION_INFO   *EepromInfo;
  EEPROM_PART_INFO       *EepromPartInfo;
  EFI_STATUS              Status;

  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting\n", __FUNCTION__, __LINE__));

  //
  // Initialize variables
  //
  BlockList            = NULL;
  EepromInfo           = (EEPROM_FUNCTION_INFO *) FunctionInfo;
  EepromPartInfo       = NULL;
  Status               = EFI_SUCCESS;
  LoadEepromMap ();

  //
  // Sanity checks
  //
  // NOTE: Input parameters already checked in wrapper function.
  //
  if (FunctionInfo == NULL) {
    //
    // This needs to be pointing to something. Bail.
    //
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  if (EepromInfo->LibraryIndex != EEPROM_EEPROM) {
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  //
  // Grab the EEPROM map from the passed in buffer
  //
  EepromInfo->Buffer = Buffer;
  EepromInfo->Size   = *Size;
  Status = FillEepromMap (FunctionInfo);
  if (EFI_ERROR (Status)) {
    if (Status == EFI_NO_RESPONSE) {
      //
      // Nobody home, bail.
      //
      goto Exit;
    }
    //
    // We don't already have the EEPROM map in the buffer. See if there's one already in the part.
    //
    if (*Size == 0) {
      //
      // Asking for EEPROM image size. We don't know so bail.
      //
      Status = EFI_NOT_READY;
      goto Exit;
    } else {
      EepromInfo->Buffer = NULL;
      EepromInfo->Size   = 0;
      Status = FillEepromMap (FunctionInfo);
      if (EFI_ERROR (Status)) {
        goto Exit;
      }
    }
  }

  //
  // Dump current mappings
  //
  DumpEepromMap ();

  //
  // Do they just want the size of the EEPROM part?
  //
  if (*Size == 0) {
    //
    // Return EEPROM part size
    //
    EepromPartInfo = SearchEepromPartList (EepromInfo->Bus, EepromInfo->Address);
    if (EepromPartInfo == NULL) {
      //
      // Failed to find part info. Bail.
      //
      Status = EFI_NO_MAPPING;
    } else {
      //
      // Return EEPROM part size.
      //
      *Size = EepromPartInfo->PartSize;
      Status = EFI_BUFFER_TOO_SMALL;
    }
    goto Exit;
  }

  //
  // Initialize block list head
  //
  BlockList = EepromAllocatePool (sizeof (BLOCK_LIST));
  InitializeListHead (&BlockList->Link);

  //
  // Create list of blocks to write
  //
  Status = CreateBlockList (
             Offset,         // Offset in EEPROM to start writing data to
             Size,           // Number of bytes to write
             FunctionInfo,   // Pointer to function specific data
             BlockList       // Head block list
             );
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - CreateBlockList() = %r\n", __FUNCTION__, __LINE__, Status));
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  //
  // Write the list from the buffer
  //
  Status = WriteEepromBlockList (
             Buffer,
             FunctionInfo,
             BlockList
             );
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - WriteEepromBlockList() = %r\n", __FUNCTION__, __LINE__, Status));
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

Exit:
  //
  // Free resources
  //
  DestroyBlockList (BlockList);

  if (EFI_ERROR (Status) && (Status != EFI_BUFFER_TOO_SMALL)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - Ending with %r\n", __FUNCTION__, __LINE__, Status));
  }
  //
  // Save off pointers for use across LIB instances
  //
  SaveEepromMap ();

  return Status;
}

