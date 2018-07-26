#  @file
#  Converts Private PEM file into public modulus binary file.
#
#  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
#
#  This program and the accompanying materials
#  are licensed and made available under the terms and conditions of the BSD License
#  which accompanies this distribution.  The full text of the license may be found at
#  http://opensource.org/licenses/bsd-license.php.
#
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#

import binascii
import subprocess
import sys

# Version defines
__version__ = '0.1.0.0'
VerString   = 'Private PEM to public modulus binary converter, Version #{0}'.format(__version__)

def Main ():
  ReturnCode = 1
  try:
    # Setup variables
    OpenSslPath = sys.argv[1]
    PrivateKeyFileName = sys.argv[2]
    PublicKeyFileName = sys.argv[3]

    # Convert private PEM into public modulus hex-string
    PublicModulus = subprocess.check_output ([OpenSslPath, 'rsa', '-modulus', '-noout', '-inform', 'pem', '-in', PrivateKeyFileName])
    PublicModulus = PublicModulus[PublicModulus.find ('Modulus=') + 8:-1]

    # Convert hex-string into binary file
    BinaryFile = open (PublicKeyFileName, 'w+b')
    BinaryFile.write (binascii.unhexlify (PublicModulus))

    # Clear error code
    ReturnCode = 0

  finally:
    # Close file
    BinaryFile.close ()
    return ReturnCode

if __name__ == "__main__":
  print ''
  print (VerString)
  sys.exit (Main ())
