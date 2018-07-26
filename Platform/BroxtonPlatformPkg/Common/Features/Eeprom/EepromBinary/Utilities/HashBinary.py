#  @file
#  Hashes the EEPROM binary.
#
#  Copyright (c) 2015 - 2018, Intel Corporation. All rights reserved.<BR>
#
#  This program and the accompanying materials
#  are licensed and made available under the terms and conditions of the BSD License
#  which accompanies this distribution.  The full text of the license may be found at
#  http://opensource.org/licenses/bsd-license.php.
#
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#

import os
import struct
import subprocess
import sys
import zlib

# Version defines
__version__ = '0.1.0.1'
VerString   = 'EEPROM hash generator, Version #{0}'.format(__version__)

# The following defines are based off of EEPROM layout version #1.00
# We could parse the definition file for these values
BinaryLengthOffset    = 0x10
Crc32Offset           = 0x14
Crc32LengthOffset     = 0x18
HashTypeOffset        = 0x10
PromSigStructSize     = 0x20
StructureLengthOffset = 0x0C

# The following defines are based off of EEPROM layout version #1.00
# We cannot parse the definition file for these values
HashTypeNone          = 0x0000
HashTypeMD5           = 0x0001
HashTypeSHA1          = 0x0002
HashTypeSHA256        = 0x0003
HashTypeSHA384        = 0x0004
HashTypeSHA512        = 0x0005
HashTypeSigningFlag   = 0x8000

# Hash digest size defines
MD5DigestSize         = 0x10
SHA1DigestSize        = 0x14
SHA256DigestSize      = 0x20
SHA384DigestSize      = 0x30
SHA512DigestSize      = 0x40

def CopyFile(Destination, Source, Size):
  count = 0
  while count < Size:
    Destination.write(Source.read(1))
    count += 1
  return

def Crc32File(File):
  CurrentLocation = File.tell()
  File.seek (0)
  returnValue = (zlib.crc32(File.read()) & 0xFFFFFFFF)
  File.seek (CurrentLocation)
  return returnValue

def FindStructure(File, Structure):
  CurrentLocation = File.tell()
  offset   = 0
  fileSize = GetFileLength(File)
  while offset < fileSize:
    File.seek(offset)
    Signature = struct.unpack('8s', File.read(8))[0]
    if Signature == Structure:
      return offset
    offset += 0x10
  File.seek(CurrentLocation)
  return fileSize

def GetFileLength(File):
  CurrentLocation = File.tell()
  File.seek(0, os.SEEK_END)
  FileLength = File.tell()
  File.seek(CurrentLocation)
  return FileLength

def Main():
  # Set error return code
  ReturnCode = 1

  try:
    # Set OpenSSL path
    OpenSslPath = sys.argv[1]

    # Open file for reading, modifying, and in binary mode
    BinaryFileName = sys.argv[2]
    BinaryFile = open(BinaryFileName, 'r+b')

    # Get file length
    BinaryFileLength = GetFileLength(BinaryFile)

    # Find $PromSig
    SignatureLocation = FindStructure(BinaryFile, "$PromSig")
    if SignatureLocation == BinaryFileLength:
      # Failed to find structure. Bail after printing warning.
      print ('WARNING: Failed to find $PromSig structure in {0}'.format(BinaryFileName))
      return ReturnCode

    # Determine which hashing algorithm to use
    BinaryFile.seek(SignatureLocation + HashTypeOffset)
    FileHashSize = 0
    FileSigSize  = 0
    HashType     = struct.unpack('H', BinaryFile.read(2))[0]
    HashingType  = ''
    SigningFlag  = HashType & HashTypeSigningFlag
    HashType    &= 0x7FFF
    if HashType == HashTypeNone:
      FileHashSize = 0x00
      print('- Nothing to hash. Bailing...')
      return ReturnCode
    elif HashType == HashTypeMD5:
      # MD5 hashing
      FileHashSize = MD5DigestSize
      HashingType  = '-md5'
      print('- MD5 hash requested.')
    elif HashType == HashTypeSHA1:
      # SHA1 hashing
      FileHashSize = SHA1DigestSize
      HashingType  = '-sha1'
      print('- SHA1 hash requested.')
    elif HashType == HashTypeSHA256:
      # SHA256 hashing
      FileHashSize = SHA256DigestSize
      HashingType  = '-sha256'
      print('- SHA256 hash requested.')
    elif HashType == HashTypeSHA384:
      # SHA384 hashing
      FileHashSize = SHA384DigestSize
      HashingType  = '-sha384'
      print('- SHA384 hash requested.')
    elif HashType == HashTypeSHA512:
      # SHA512 hashing
      FileHashSize = SHA512DigestSize
      HashingType  = '-sha512'
      print('- SHA512 hash requested.')
    else:
      print('ERROR: Invalid hash type requested!')
      return 1

    if SigningFlag == HashTypeSigningFlag:
      print('- signing requested.')
      if HashType == HashTypeSHA384:
        print('ERROR: OpenSSL does not currently support signing SHA384!')
        return 1
      if HashType == HashTypeSHA512:
        print('ERROR: OpenSSL does not currently support signing SHA512!')
        return 1
      if len (sys.argv) != 4:
        print('ERROR: Missing OpenSSL generated private key PEM file!')
        return 1
      PrivateKeyFileName = sys.argv[3]
      FileSigSize = 256

    # Update $PromSig.Length
    PromSigLength = PromSigStructSize + FileHashSize + FileSigSize
    BinaryFile.seek(SignatureLocation + StructureLengthOffset)
    WriteBinary(BinaryFile, PromSigLength, 0x04)

    # Update $Eeprom$.CrcLength
    HeaderLocation = FindStructure(BinaryFile, "$Eeprom$")
    if HeaderLocation != 0x00:
      print('ERROR: $Eeprom$ structure is not at the beginning of the file!')
      return 1
    BinaryFile.seek(HeaderLocation + BinaryLengthOffset)
    Crc32Length = BinaryFile.read(4)
    BinaryFile.seek(HeaderLocation + Crc32LengthOffset)
    BinaryFile.write(Crc32Length)

    # Update $Eeprom$.Length
    BinaryFile.seek(HeaderLocation + BinaryLengthOffset)
    WriteBinary(BinaryFile, BinaryFileLength + FileHashSize + FileSigSize, 0x04)

    # Update EEPROM header CRC32
    HeaderLocation = FindStructure(BinaryFile, "$Eeprom$")
    if HeaderLocation != 0x00:
      print('ERROR: $Eeprom$ structure is not at the beginning of the file!')
      return 1
    BinaryFile.seek(HeaderLocation + Crc32Offset)
    WriteBinary(BinaryFile, 0x0000, 0x04)
    Crc32 = Crc32File(BinaryFile)
    print('- CRC32 = {0:08X}'.format (Crc32))
    BinaryFile.seek(HeaderLocation + Crc32Offset)
    WriteBinary(BinaryFile, Crc32, 0x04)

    # Close the file so OpenSSL can get to it
    BinaryFile.close()

    # Hash the file
    subprocess.call([OpenSslPath, 'dgst', HashingType, '-binary', '-out', 'hash.tmp', BinaryFileName])

    # Sign the file
    if SigningFlag == HashTypeSigningFlag:
      subprocess.call([OpenSslPath, 'dgst', HashingType, '-sign', PrivateKeyFileName, '-out', 'sign.tmp', BinaryFileName])

    # Add hash to end of the file
    HashFile = open('hash.tmp', 'r+b')
    BinaryFile = open(BinaryFileName, 'r+b')
    BinaryFile.seek(GetFileLength (BinaryFile))
    CopyFile(BinaryFile, HashFile, FileHashSize)
    BinaryFile.close()
    HashFile.close()
    os.remove('hash.tmp')

    # Add signed hash to end of the file
    if SigningFlag == HashTypeSigningFlag:
      SignatureFile = open('sign.tmp', 'r+b')
      BinaryFile = open(BinaryFileName, 'r+b')
      BinaryFile.seek(GetFileLength (BinaryFile))
      CopyFile(BinaryFile, SignatureFile, FileSigSize)
      BinaryFile.close()
      SignatureFile.close()
      os.remove('sign.tmp')

    # Align file size to paragraphs
    BinaryFile = open(BinaryFileName, 'r+b')
    BinaryFileLength = GetFileLength(BinaryFile)
    AlignmentSize = 0x10 - (BinaryFileLength % 0x10)
    if AlignmentSize < 0x10:
      BinaryFile.seek(BinaryFileLength)
      WriteBinary(BinaryFile, 0x0000, AlignmentSize)

    # Clear return code
    ReturnCode = 0

  finally:
    # Close file
    BinaryFile.close()
    return ReturnCode

def WriteBinary(File, binary, Size):
  count = 0
  value = binary
  while count < Size:
    count += 1
    # Just want the low byte
    temp = value & 0xFF
    # Shift right a byte
    value >>= 8
    # Write the low byte out
    File.write(str(chr (temp)))

if __name__ == "__main__":
  print('')
  print(VerString)
  sys.exit(Main())
