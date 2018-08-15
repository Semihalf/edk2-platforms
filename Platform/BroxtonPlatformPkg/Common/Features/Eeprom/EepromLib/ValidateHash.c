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

#include <Library/BaseCryptLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

CONST UINT8  mRsaE[] = {0x01, 0x00, 0x01};
CONST UINT8  mHashSizeLookup[] = {
               0x00,
               MD5_DIGEST_SIZE,
               SHA1_DIGEST_SIZE,
               SHA256_DIGEST_SIZE,
               SHA384_DIGEST_SIZE,
               SHA512_DIGEST_SIZE
               };

BOOLEAN
EFIAPI
InPeiPhase (VOID)
{
  return FALSE;
}

EFI_STATUS
EFIAPI
SetEepromVariable (
  IN       UINT8             LibraryIndex,
  IN       UINT8            *Buffer,
  IN       UINT32            BufferSize
  )
{
  UINT32       Attributes;
  EFI_STATUS   Status;
  CHAR16       VariableName[32];

  //
  // Initialize variables
  //
  Attributes = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS;
  Status     = EFI_SUCCESS;
  UnicodeSPrint (VariableName, 32, L"PromSig-%d", LibraryIndex);

  //
  // Sanity checks
  //
  if (LibraryIndex >= EEPROM_DATA_LIBRARY_INDEX_MAX) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Library index is larger than %d!\n", __FUNCTION__, __LINE__, EEPROM_DATA_LIBRARY_INDEX_MAX - 1));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  if ((Buffer == NULL) && (BufferSize > 0)) {
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  //
  // Set variable
  //
  Status = gRT->SetVariable (
                  VariableName,           // Variable name in UniCode
                  &gEepromVariableGuid,   // Variable GUID
                  Attributes,             // Attributes
                  BufferSize,             // Data size
                  Buffer                  // Data
                  );

Exit:
  return Status;
}

EFI_STATUS
EFIAPI
SignedHashCheck (
  IN       UINT8             LibraryIndex
  )
{
  UINT32                  Crc32Size;
  UINT8                   Digest[MAX_DIGEST_SIZE];
  EEPROM_HEADER          *EepromHeader;
  EEPROM_FUNCTION_INFO    EepromInfo;
  FV_FUNCTION_INFO        FvInfo;
  EFI_GUID                FvPublicKeyFile;
  UINT8                  *Hash;
  UINT32                  HashSize;
  BOOLEAN                 HashStatus;
  UINT16                  HashType;
  UINT8                  *ImageBuffer;
  UINT32                  ImageSize;
  UINT8                  *PublicKey;
  UINT32                  PublicKeySize;
  VOID                   *Rsa;
  BOOLEAN                 RsaStatus;
  SIGNATURE_DATA         *Signature;
  UINT8                  *SignedHash;
  UINT32                  SignedHashSize;
  UINT32                  Size;
  EFI_STATUS              Status;

  //
  // Initialize variables
  //
  CopyMem (&FvPublicKeyFile, PcdGetPtr (PcdEepromPublicKeyFile), sizeof (EFI_GUID));
  ZeroMem (&FvInfo, sizeof (FV_FUNCTION_INFO));
  EepromInfo.Bus          = PcdGet8 (PcdEepromBus);
  EepromInfo.Address      = PcdGet8 (PcdEepromAddress);
  EepromInfo.LibraryIndex = EEPROM_EEPROM;
  FvInfo.LibraryIndex = EEPROM_FV;
  FvInfo.FvFileGuid   = &FvPublicKeyFile;
  PublicKey           = NULL;
  PublicKeySize       = 0;
  Rsa                 = NULL;
  SignedHash          = NULL;
  SignedHashSize      = 0;
  Status              = EFI_UNSUPPORTED;

  //
  // Sanity checks
  //
  if (LibraryIndex >= EEPROM_DATA_LIBRARY_INDEX_MAX) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Library index is larger than %d!\n", __FUNCTION__, __LINE__, EEPROM_DATA_LIBRARY_INDEX_MAX - 1));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  //
  // Get binary image size
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
  // Get $Eeprom$ structure
  //
  Size   = 0;
  Status = GetEepromStructure (LibraryIndex, EEPROM_HEADER_SIGNATURE, (UINT8 **) &EepromHeader, &Size);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Image is corrupted!\n", __FUNCTION__, __LINE__));
    Status = EFI_VOLUME_CORRUPTED;
    goto Exit;
  }
  Crc32Size = EepromHeader->crclength;
  if (Crc32Size == 0) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Buffer size is 0!\n", __FUNCTION__, __LINE__));
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  //
  // Get $PromSig structure
  //
  Size   = 0;
  Status = GetEepromStructure (LibraryIndex, EEPROM_SIGNATURE_SIGNATURE, (UINT8 **) &Signature, &Size);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Image is corrupted!\n", __FUNCTION__, __LINE__));
    Status = EFI_VOLUME_CORRUPTED;
    goto Exit;
  }
  Hash     = ((UINT8 *) Signature) + sizeof (SIGNATURE_DATA);
  HashType = Signature->hashtype;
  HashSize = mHashSizeLookup[HashType & HASH_TYPE_MASK];
  if (HashType & EEPROM_SIGNATURE_TYPE_SIGNED) {
    SignedHash     = ((UINT8 *) Signature) + sizeof (SIGNATURE_DATA) + HashSize;
    SignedHashSize = Signature->length - sizeof (SIGNATURE_DATA) - HashSize;
  }
  if (HashType == 0) {
    //
    // Nothing to do. Bail.
    //
    Status = EFI_SUCCESS;
    goto Exit;
  }
  if ((HashType & HASH_TYPE_MASK) > MAX_HASH_TYPE) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: HashType out of bounds! [%04x]\n", __FUNCTION__, __LINE__, HashType));
    Status = EFI_COMPROMISED_DATA;
    goto Exit;
  }
  if ((Hash == NULL) || (HashSize == 0)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Buffer or buffer size is NULL!\n", __FUNCTION__, __LINE__));
    Status = EFI_COMPROMISED_DATA;
    goto Exit;
  }
  if ((SignedHashSize != 256) && (SignedHashSize != 0)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: SignedHashSize failed! [%02x]\n", __FUNCTION__, __LINE__, SignedHashSize));
    Status = EFI_COMPROMISED_DATA;
    goto Exit;
  }

  //
  // Get binary image
  //
  ImageBuffer = EepromAllocatePool (ImageSize);
  if (ImageBuffer == NULL) {
    //
    // Failed to allocate pool
    //
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Failed to allocate pool for Buffer!\n", __FUNCTION__, __LINE__));
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }
  Status = ReadEeprom (LibraryIndex, 0, &ImageSize, ImageBuffer, &EepromInfo);
  if (EFI_ERROR (Status)) {
    //
    // Oops!!!
    //
    goto Exit;
  }

  //
  // Clear existing NvStorage variable if it exists. gEepromVariableGuid:L"PromSig-#"
  //
  Status = SetEepromVariable (LibraryIndex, NULL, 0);
  if (EFI_ERROR (Status) && (Status != EFI_NOT_FOUND)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Failed to delete variable PromSig-%d! [%r]\n", __FUNCTION__, __LINE__, LibraryIndex, Status));
    goto Exit;
  }

  //
  // Figure out what hash is being used
  //
  switch (HashType & HASH_TYPE_MASK) {
    case EEPROM_SIGNATURE_TYPE_NONE:
      //
      // Nothing to do. Bail.
      //
      break;
    case EEPROM_SIGNATURE_TYPE_MD5:
      //
      // MD5 hashing
      //
      ZeroMem (Digest, MAX_DIGEST_SIZE);
      HashStatus = Md5HashAll (ImageBuffer, Crc32Size, Digest);
      if (!HashStatus) {
        DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Image failed MD5 hash check!\n", __FUNCTION__, __LINE__));
        Status = EFI_SECURITY_VIOLATION;
        goto Exit;
      }
      if (CompareMem (Digest, Hash, MD5_DIGEST_SIZE) != 0) {
        DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Image failed MD5 hash check!\n", __FUNCTION__, __LINE__));
        Status = EFI_SECURITY_VIOLATION;
        goto Exit;
      }
      HashSize = MD5_DIGEST_SIZE;
      break;
    case EEPROM_SIGNATURE_TYPE_SHA1:
      //
      // SHA1 hashing
      //
      ZeroMem (Digest, MAX_DIGEST_SIZE);
      HashStatus = Sha1HashAll (ImageBuffer, Crc32Size, Digest);
      if (!HashStatus) {
        DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Image failed SHA1 hash check!\n", __FUNCTION__, __LINE__));
        Status = EFI_SECURITY_VIOLATION;
        goto Exit;
      }
      if (CompareMem (Digest, Hash, SHA1_DIGEST_SIZE) != 0) {
        DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Image failed SHA1 hash check!\n", __FUNCTION__, __LINE__));
        Status = EFI_SECURITY_VIOLATION;
        goto Exit;
      }
      HashSize = SHA1_DIGEST_SIZE;
      break;
    case EEPROM_SIGNATURE_TYPE_SHA256:
      //
      // SHA256 hashing
      //
      ZeroMem (Digest, MAX_DIGEST_SIZE);
      HashStatus = Sha256HashAll (ImageBuffer, Crc32Size, Digest);
      if (!HashStatus) {
        DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Image failed SHA256 hash check!\n", __FUNCTION__, __LINE__));
        Status = EFI_SECURITY_VIOLATION;
        goto Exit;
      }
      if (CompareMem (Digest, Hash, SHA256_DIGEST_SIZE) != 0) {
        DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Image failed SHA256 hash check!\n", __FUNCTION__, __LINE__));
        Status = EFI_SECURITY_VIOLATION;
        goto Exit;
      }
      HashSize = SHA256_DIGEST_SIZE;
      break;
    case EEPROM_SIGNATURE_TYPE_SHA384:
      //
      // SHA384 hashing
      //
      ZeroMem (Digest, MAX_DIGEST_SIZE);
      HashStatus = Sha384HashAll (ImageBuffer, Crc32Size, Digest);
      if (!HashStatus) {
        DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Image failed SHA384 hash check!\n", __FUNCTION__, __LINE__));
        Status = EFI_SECURITY_VIOLATION;
        goto Exit;
      }
      if (CompareMem (Digest, Hash, SHA384_DIGEST_SIZE) != 0) {
        DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Image failed SHA384 hash check!\n", __FUNCTION__, __LINE__));
        Status = EFI_SECURITY_VIOLATION;
        goto Exit;
      }
      HashSize = SHA384_DIGEST_SIZE;
      break;
    case EEPROM_SIGNATURE_TYPE_SHA512:
      //
      // SHA512 hashing
      //
      ZeroMem (Digest, MAX_DIGEST_SIZE);
      HashStatus = Sha512HashAll (ImageBuffer, Crc32Size, Digest);
      if (!HashStatus) {
        DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Image failed SHA512 hash check!\n", __FUNCTION__, __LINE__));
        Status = EFI_SECURITY_VIOLATION;
        goto Exit;
      }
      if (CompareMem (Digest, Hash, SHA512_DIGEST_SIZE) != 0) {
        DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Image failed SHA512 hash check!\n", __FUNCTION__, __LINE__));
        Status = EFI_SECURITY_VIOLATION;
        goto Exit;
      }
      HashSize = SHA512_DIGEST_SIZE;
      break;
    default:
      //
      // Oops!
      //
      DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Unknown hash type [%04x]!\n", __FUNCTION__, __LINE__, HashType));
      Status = EFI_VOLUME_CORRUPTED;
      goto Exit;
      break;
  }

  //
  // Does this have a signed hash?
  //
  if ((HashType & EEPROM_SIGNATURE_TYPE_SIGNED) != EEPROM_SIGNATURE_TYPE_SIGNED) {
    //
    // Nope. Bail.
    //
    Status = EFI_SUCCESS;
    gotoExit;
  }

  //
  // Get size of public key file in FV
  //
  PublicKey = EepromAllocatePool (2);
  Status = ReadEeprom (EEPROM_FV, 0, &PublicKeySize, PublicKey, &FvInfo);
  if ((Status != EFI_BUFFER_TOO_SMALL) || (PublicKeySize == 0)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - Finding Public key file size returned with %r and a size of %08x\n", __FUNCTION__, __LINE__, Status, PublicKeySize));
    Status = EFI_COMPROMISED_DATA;
    goto Exit;
  }

  //
  // Allocate buffer for public key
  //
  PublicKey = EepromFreePool (PublicKey);
  PublicKey = EepromAllocatePool (PublicKeySize);
  if (PublicKey == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }

  //
  // Read in Public key from FV
  //
  Status = ReadEeprom (EEPROM_FV, 0, &PublicKeySize, PublicKey, &FvInfo);
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  //
  // Verify signature
  //
  Rsa = RsaNew ();
  if (Rsa == NULL) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - RsaNew() returned NULL buffer!\n", __FUNCTION__, __LINE__));
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }

  //
  // Set RSA Key Components.
  // NOTE: Only N and E are needed to be set as RSA public key for signature verification.
  //
  RsaStatus = RsaSetKey (Rsa, RsaKeyN, PublicKey, EFI_CERT_TYPE_RSA2048_SIZE);
  if (!RsaStatus) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - RsaSetKey(RsaKeyN) failed!\n", __FUNCTION__, __LINE__));
    Status = EFI_ABORTED;
    goto Exit;
  }

  RsaStatus = RsaSetKey (Rsa, RsaKeyE, mRsaE, sizeof (mRsaE));
  if (!RsaStatus) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - RsaSetKey(RsaKeyE) failed!\n", __FUNCTION__, __LINE__));
    Status = EFI_ABORTED;
    goto Exit;
  }

  RsaStatus = RsaPkcs1Verify (Rsa, Hash, HashSize, SignedHash, SignedHashSize);
  if (!RsaStatus) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - RsaPkcs1Verify() failed!\n", __FUNCTION__, __LINE__));
    Status = EFI_SECURITY_VIOLATION;
    goto Exit;
  }

Exit:
  if (Rsa != NULL) {
    RsaFree (Rsa);
  }
  EepromHeader = EepromFreePool (EepromHeader);
  ImageBuffer  = EepromFreePool (ImageBuffer);
  PublicKey    = EepromFreePool (PublicKey);
  Signature    = EepromFreePool (Signature);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a (#%4d) - Ending with %r\n", __FUNCTION__, __LINE__, Status));
  } else if (HashType != 0) {
    //
    // This is where a copy of the $PromSig data is stored in NvStorage. gEepromVariableGuid:L"PromSig-#"
    //
    Status = SetEepromVariable (LibraryIndex, Hash, (HashSize + SignedHashSize));
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a (#%4d) - ERROR: Failed to delete variable PromSig-%d! [%r]\n", __FUNCTION__, __LINE__, LibraryIndex, Status));
    }
  }
  return Status;
}

