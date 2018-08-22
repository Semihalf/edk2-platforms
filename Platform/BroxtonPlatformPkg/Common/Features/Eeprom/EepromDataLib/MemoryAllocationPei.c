/** @file
  Common EEPROM memory allocation code. This is necessary in PEI
  becuase PEI doesn't have working FreePool code and this code
  uses alot of pool allocation for temporary storage.

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "EepromDataLib.h"

#include <PiPei.h>
#include <Library/EepromLib.h>
#include <Library/HobLib.h>

//
// Desc:        Frees an existing EEPROM memory allocation from the EEPROM heap.
// Variables:   HobPointer     Pointer to the EEPROM memory allocation HOB
//              Offset         Pointer to the beginning of the pool to free
// Return:      Status         EFI_SUCCESS           - Successfully freed pool
//                             EFI_INVALID_PARAMETER - Invalid input parameter
//
EFI_STATUS
EFIAPI
ClearEepromAllocation (
  IN EEPROM_ALLOCATION_STRUCT   *Header,
  IN VOID                       *Offset
  )
{
  UINT8         bit;
  UINT16        ByteCount;
  UINTN         HeapPointer;
  UINTN         index;
  UINT16        Size;
  UINTN         StartIndex;
  EFI_STATUS    Status;

  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting...\n", __FUNCTION__, __LINE__));

  //
  // Initialize variables
  //
  HeapPointer  = (UINTN) Header + sizeof (EEPROM_ALLOCATION_STRUCT);
  HeapPointer += 0x10 - (HeapPointer % 0x10);
  Size         = 0;
  StartIndex   = ((UINTN) Offset - HeapPointer) / 0x10;
  Status       = EFI_SUCCESS;

  //
  // Sanity checks
  //
  if ((Header == NULL) || (Offset == NULL)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Header or Offset is NULL!\n", __FUNCTION__, __LINE__));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  if (HeapPointer > (UINTN) Offset) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: HeapPointer > Offset!\n", __FUNCTION__, __LINE__));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  if (((UINTN) Offset % 0x10) != 0) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Offset not paragraph aligned [%08x]!\n", __FUNCTION__, __LINE__, Offset));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  //
  // Clear flags
  //
  for (index = StartIndex; index < (MAX_EEPROM_ALLOCATION_FLAG * 8); index++) {
    ByteCount = (UINT16) (index / 8);
    bit       = (1 << (index % 8));
    // Check usage flag
    if (Header->UsageFlag[ByteCount] & bit) {
      // Used. Reset flag
      Header->UsageFlag[ByteCount] &= ~bit;
      Size++;
    } else {
      // Available. End of this allocation, bail.
      break;
    }
  }

Exit:
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Header = %08x\n", __FUNCTION__, __LINE__, Header));
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Size   = %08x\n", __FUNCTION__, __LINE__, Size * 0x10));
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Heap   = %08x\n", __FUNCTION__, __LINE__, HeapPointer));
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Start  = %08x\n", __FUNCTION__, __LINE__, StartIndex));
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Offset = %08x\n", __FUNCTION__, __LINE__, Offset));
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - Ending with %r\n", __FUNCTION__, __LINE__, Status));
  }
  return Status;
}

UINTN
EFIAPI
DisplayStackPointer (
  IN   CHAR8    *Function,
  IN   UINTN     LineNumber
  )
{
  EFI_HOB_HANDOFF_INFO_TABLE   *Hob;
  UINTN                         Temp;

  Hob  = GetHobList ();
  Temp = 0;
  if ((Hob != NULL) & (mEepromDataLibDebugFlag)) {
    DEBUG ((DEBUG_INFO, "%a (#%4d) - INFO: FreeTop    = %08x\n", __FUNCTION__, __LINE__, Hob->EfiFreeMemoryTop));
    DEBUG ((DEBUG_INFO, "%a (#%4d) - INFO: FreeBottom = %08x\n", __FUNCTION__, __LINE__, Hob->EfiFreeMemoryBottom));
    Temp = (UINTN) Hob->EfiFreeMemoryBottom;
  }

  return Temp;
}

//
// Desc:        Dumps the EEPROM memory allocation status.
// Variables:   HobPointer     Pointer to the EEPROM memory allocation HOB
// Return:      NA
//
VOID
EFIAPI
DumpAllocationTable (
  IN EEPROM_ALLOCATION_STRUCT   *Header
  )
{
  UINT8     bit;
  UINT16    ByteCount;
  UINTN     count;
  UINTN     index;

  if (mEepromDataLibDebugFlag) {
    //
    // Loop thru and display blocks
    //
    count = 0;
    for (index = 0; index < (MAX_EEPROM_ALLOCATION_FLAG * 8); index++) {
      ByteCount = (UINT16) (index / 8);
      bit       = (1 << (index % 8));
      if (count == 0) {
        DEBUG ((DEBUG_INFO, "%04x - ", index));
      }
      // Check usage flag
      if (Header->UsageFlag[ByteCount] & bit) {
        // Used.
        DEBUG ((DEBUG_INFO, "*"));
      } else {
        // Available
        DEBUG ((DEBUG_INFO, " "));
      }
      count++;
      if (count == (8 * 8)) {
        DEBUG ((DEBUG_INFO, "\n"));
        count = 0;
      }
    }
  }
}

//
// Desc:        Finds the first fit for the requested pool size.
// Variables:   HobPointer     Pointer to the EEPROM memory allocation HOB
//              Size           Size of the requested pool
// Return:      Pointer        Pointer to the pool or NULL if no room
//
VOID*
EFIAPI
FindEepromAllocationOffset (
  IN EEPROM_ALLOCATION_STRUCT   *Header,
  IN UINT16                      Size
  )
{
  UINT8     bit;
  UINTN     beginingIndex;
  UINT16    ByteCount;
  UINT16    CurrentSize;
  UINTN     HeapPointer;
  UINTN     index;
  VOID     *ReturnPointer;
  UINTN     StartIndex;

  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting...\n", __FUNCTION__, __LINE__));

  //
  // Initialize variables
  //
  CurrentSize    = 0;
  HeapPointer    = (UINTN) Header + sizeof (EEPROM_ALLOCATION_STRUCT);
  HeapPointer   += 0x10 - (HeapPointer % 0x10);
  ReturnPointer  = NULL;
  StartIndex     = 0;

  //
  // Sanity checks
  //
  if (Header == NULL) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Header is NULL!\n", __FUNCTION__, __LINE__));
    goto Exit;
  }
  if ((Size == 0) || (Size > MAX_EEPROM_ALLOCATION_SIZE)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Size is either 0 or > %08x!\n", __FUNCTION__, __LINE__, MAX_EEPROM_ALLOCATION_SIZE));
    goto Exit;
  }

  //
  // Loop thru flags looking for enough to make up size
  // - Size is in flag units already
  //
  if (Size < 0x20) {
    // For small allocations
    beginingIndex = 0;
  } else {
    // For larger allocations
    beginingIndex = 0x20;
  }
  for (index = beginingIndex; index < (MAX_EEPROM_ALLOCATION_FLAG * 8); index++) {
    ByteCount = (UINT16) (index / 8);
    bit       = (1 << (index % 8));
    // Check usage flag
    if (Header->UsageFlag[ByteCount] & bit) {
      // Used. Reset count.
      CurrentSize = 0;
    } else {
      // Available
      if ((index == 0) || ((CurrentSize == 1) && (index != 1))) {
        // Block after empty block, except first block
        StartIndex = index;
      }
      CurrentSize++;
    }
    // Do we have enough room?
    if (CurrentSize > Size) {
      // Yep. Set usage flags, leaving the last one cleared.
      for (index = StartIndex; index < StartIndex + Size; index++) {
        ByteCount = (UINT16) (index / 8);
        bit       = (1 << (index % 8));
        Header->UsageFlag[ByteCount] |= bit;
      }
      // Set pointer
      ReturnPointer = (VOID *) (HeapPointer + (StartIndex * 0x10));
      // Bail
      break;
    }
  }

  //
  // Display error if no room
  //
  if (ReturnPointer == NULL) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: No room on heap!\n", __FUNCTION__, __LINE__));
  }

Exit:
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Header = %08x\n", __FUNCTION__, __LINE__, Header));
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Size   = %08x\n", __FUNCTION__, __LINE__, Size * 0x10));
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Heap   = %08x\n", __FUNCTION__, __LINE__, HeapPointer));
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Start  = %08x\n", __FUNCTION__, __LINE__, StartIndex));
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Offset = %08x\n", __FUNCTION__, __LINE__, ReturnPointer));
  return ReturnPointer;
}

//
// Desc:        Finds or allocates a memory buffer for the PEI EEPROM code to use as a heap.
// Variables:   NA
// Return:      Pointer        Eeprom Memory HOB
//
EEPROM_ALLOCATION_STRUCT*
EFIAPI
GetEepromMemoryHob (VOID)
{
  EEPROM_ALLOCATION_STRUCT      AllocationHeader;
  EEPROM_ALLOCATION_STRUCT     *HeaderPointer;
  EFI_HOB_GUID_TYPE            *GuidHobPtr;

  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting...\n", __FUNCTION__, __LINE__));

  //
  // If HOB already there, just return pointer to it
  //
  GuidHobPtr = GetFirstGuidHob (&gEepromVariableGuid);
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - GuidHobPtr = %08x\n", __FUNCTION__, __LINE__, GuidHobPtr));
  while (GuidHobPtr != NULL) {
    //
    // Get Memory HOB header pointer
    //
    HeaderPointer = GET_GUID_HOB_DATA (GuidHobPtr);
    if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - HeaderPointer = %08x\n", __FUNCTION__, __LINE__, HeaderPointer));
    //
    // Is this our block?
    //
    if (HeaderPointer->Signature == ALLOCATION_HOB_HEADER_SIGNATURE) {
      //
      // Yep
      //
      goto Exit;
    }
    //
    // Skip past present HOB to get next HOB
    //
    GuidHobPtr = GET_NEXT_HOB (GuidHobPtr);
    GuidHobPtr = GetNextGuidHob (&gEepromVariableGuid, GuidHobPtr);
    if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - GuidHobPtr = %08x\n", __FUNCTION__, __LINE__, GuidHobPtr));
  }

  //
  // Create HOB
  //
  ZeroMem (&AllocationHeader, sizeof (EEPROM_ALLOCATION_STRUCT));
  AllocationHeader.Signature = ALLOCATION_HOB_HEADER_SIGNATURE;
  HeaderPointer = BuildGuidDataHob (&gEepromVariableGuid, &AllocationHeader, MAX_EEPROM_HOB_SIZE);
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - HeaderPointer = %08x\n", __FUNCTION__, __LINE__, HeaderPointer));

Exit:
  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - AllocationHob @ %08x\n", __FUNCTION__, __LINE__, HeaderPointer));
  return HeaderPointer;
}

//
// Desc:        Copies the contents of an existing memory pool into a new memory pool of equal or greater size.
// Variables:   Size           Size of the pool to copy existing pool into
//              SourcePointer  Pointer to the source buffer to copy
// Return:      Pointer        Pointer to your copy of the pool
//
VOID*
EFIAPI
EepromAllocateCopyPool (
  IN  UINTN    Size,
  IN  VOID    *SourcePointer
  )
{
  VOID    *PoolPointer;

  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting...\n", __FUNCTION__, __LINE__));

  //
  // Sanity checks
  //
  if (SourcePointer == NULL) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: SourcePointer is NULL!\n", __FUNCTION__, __LINE__));
    PoolPointer = NULL;
    goto Exit;
  }
  if ((Size == 0) || (Size > MAX_EEPROM_ALLOCATION_SIZE)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Size is either 0 or > %08x!\n", __FUNCTION__, __LINE__, MAX_EEPROM_ALLOCATION_SIZE));
    PoolPointer = NULL;
    goto Exit;
  }

  //
  // Get pool for copy
  //
  PoolPointer = EepromAllocatePool (Size);
  if (PoolPointer == NULL) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: PoolPointer is NULL!\n", __FUNCTION__, __LINE__));
    goto Exit;
  }

  //
  // Copy source into new pool
  //
  CopyMem (PoolPointer, SourcePointer, Size);

Exit:
  return PoolPointer;
}

//
// Desc:        Creates a new memory pool.
// Variables:   Size           Size of the pool requested
// Return:      Pointer        Pointer the new pool
//
VOID*
EFIAPI
EepromAllocatePool (
  IN  UINTN   Size
  )
{
  EEPROM_ALLOCATION_STRUCT   *AllocationHobHeader;
  UINT16                      ConvertedSize;
  UINT16                      PoolSize;
  VOID                       *ReturnPointer;

  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting...\n", __FUNCTION__, __LINE__));

  //
  // Sanity checking
  //
  if ((Size == 0) || (Size > MAX_EEPROM_ALLOCATION_SIZE)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Size is either 0 or > %08x!\n", __FUNCTION__, __LINE__, MAX_EEPROM_ALLOCATION_SIZE));
    ReturnPointer = NULL;
    goto Exit;
  }

  //
  // Convert size to paragraphs and add one for the guard
  //
  ConvertedSize  = (UINT16) (Size & 0xFFFF);
  PoolSize       = ConvertedSize / 0x10;
  if ((ConvertedSize % 0x10) > 0) {
    //
    // Size it up to the next paragraph
    //
    PoolSize++;
  }

  //
  // Get or create EEPROM allocation pool HOB
  //
  AllocationHobHeader = GetEepromMemoryHob ();
  if (AllocationHobHeader == NULL) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: AllocationHobHeader is NULL!\n", __FUNCTION__, __LINE__));
    ReturnPointer = NULL;
    goto Exit;
  }

  //
  // Find first available fit and set usage flags
  //
  ReturnPointer = FindEepromAllocationOffset (AllocationHobHeader, PoolSize);
  if (ReturnPointer == NULL) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: ReturnPointer is NULL!\n", __FUNCTION__, __LINE__));
    goto Exit;
  }

  //
  // Zero pool
  //
  ZeroMem (ReturnPointer, Size);

  //
  // Dump pool
  //
  DumpAllocationTable (AllocationHobHeader);

Exit:
  return ReturnPointer;
}

//
// Desc:        Frees a memory pool.
// Variables:   Pointer        Pointer to the beginning of the pool to be freed
// Return:      Pointer        NULL
//
VOID*
EFIAPI
EepromFreePool (
  IN  VOID  *Pointer
  )
{
  EEPROM_ALLOCATION_STRUCT   *AllocationHobHeader;
  EFI_STATUS                  Status;

  if (mEepromDataLibDebugFlag) DEBUG ((DEBUG_INFO, "%a (#%4d) - Starting...\n", __FUNCTION__, __LINE__));

  //
  // Get EEPROM allocation pool HOB
  //
  AllocationHobHeader = GetEepromMemoryHob ();
  if (AllocationHobHeader == NULL) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: AllocationHobHeader is NULL!\n", __FUNCTION__, __LINE__));
    goto Exit;
  }

  //
  // Sanity checks
  //
  if (Pointer == NULL) {
    // Nothing to do. Bail.
    goto Exit;
  }
  if ((UINTN) Pointer > ((UINTN) AllocationHobHeader + MAX_EEPROM_ALLOCATION_SIZE)) {
    // Past our heap. Bail.
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: This is not our pool [%08x]!\n", __FUNCTION__, __LINE__, Pointer));
    goto Exit;
  }

  //
  // Find allocation and clear usage flags
  //
  Status = ClearEepromAllocation (AllocationHobHeader, Pointer);

  //
  // Dump pool
  //
  DumpAllocationTable (AllocationHobHeader);

Exit:
  return NULL;
}

