#  @file
#  Creates the RSA private/public key-pairs.
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

import os
import subprocess
import sys

# Version defines
__version__ = '0.1.0.0'
VerString   = 'Create RSA Private/Public key pair, Version #{0}'.format(__version__)

def Main ():
  ReturnCode = 1
  try:
    # Setup variables
    OpenSslPath           = sys.argv[1]
    DestinationPath       = sys.argv[2]
    if len(sys.argv) > 3:
      FilePrefix          = sys.argv[3]
      PrivateKeyFile      = DestinationPath + '\\' + FilePrefix + '-private.pem'
      PublicKeyFile       = DestinationPath + '\\' + FilePrefix + '-public.pem'
      PublicKeyBinaryFile = DestinationPath + '\\' + FilePrefix + '-public.bin'
    else:
      PrivateKeyFile      = DestinationPath + '\\private-key.pem'
      PublicKeyFile       = DestinationPath + '\\public-key.pem'
      PublicKeyBinaryFile = DestinationPath + '\\public-key.bin'
    PemToBinaryConverter  = os.path.dirname(os.path.dirname(OpenSslPath)) + '\\Utilities\\ConvertPemToBin.py'

    # Debug prints
    if False:
      print('OpenSslPath          = {0}'.format(OpenSslPath))
      print('DestinationPath      = {0}'.format(DestinationPath))
      print('FilePrefix           = {0}'.format(FilePrefix))
      print('PemToBinaryConverter = {0}'.format(PemToBinaryConverter))
      print('PrivateKeyFile       = {0}'.format(PrivateKeyFile))
      print('PublicKeyFile        = {0}'.format(PublicKeyFile))
      print('PublicKeyBinaryFile  = {0}'.format(PublicKeyBinaryFile))

    # Create private key
    #   OpenSSL genrsa -out private-key.pem 2048
    #    - Generates the private key in PEM form
    subprocess.call([OpenSslPath, 'genrsa', '-out', PrivateKeyFile, '2048'])

    # Create public key
    #   OpenSSL rsa -in private-key.pem -outform pem -pubout -out public-key.pem
    #    - Generates the public key in PEM form from the private key
    subprocess.call([OpenSslPath, 'rsa', '-in', PrivateKeyFile, '-outform', 'pem', '-pubout', '-out', PublicKeyFile])

    # Convert private PEM into public modulus hex-string
    #   ConvertPemToBin.py OpenSslPath PrivateKeyPath PublicKeyBinaryFile
    subprocess.call(['python.exe', PemToBinaryConverter, OpenSslPath, PrivateKeyFile, PublicKeyBinaryFile])

    # Clear error code
    ReturnCode = 0

  finally:
    # Close file
    return ReturnCode

if __name__ == "__main__":
  print ''
  print (VerString)
  sys.exit (Main ())
