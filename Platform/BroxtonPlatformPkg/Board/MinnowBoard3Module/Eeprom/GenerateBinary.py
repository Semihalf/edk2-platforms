#  @file
#  Script file to generate the EEPROM binary
#
#  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
#
#  This program and the accompanying materials
#  are licensed and made available under the terms and conditions of the BSD License
#  which accompanies this distribution.  The full text of the license may be found at
#  http://opensource.org/licenses/bsd-license.php.
#
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#

import glob
import os
import subprocess
import sys

# Version defines
__version__   = '0.1.0.2'
VerString     = 'EEPROM binary generation tool, Version #{0}'.format(__version__)
PythonVersion = 'Python version = {0:X}'.format(sys.hexversion)

def Main():
  # Set return code to error
  ReturnCode = 1

  try:
    # Get paths
    ThisUtilityPath   = os.path.dirname(os.path.abspath(sys.argv[0]))
    ParentUtilityPath = os.path.dirname(ThisUtilityPath)
    UtilityPath       = ParentUtilityPath + '\\Utilities'
    ProjectName       = 'MB3'

    # MinnowBoard 3 specific defines
    EepromBinaryUtility  = UtilityPath     + '\\eeprom.py'
    EepromHashUtility    = UtilityPath     + '\\HashBinary.py'
    EepromDefinitionFile = ThisUtilityPath + '\\' + ProjectName + '-Defs.cfg'
    EepromProjectFile    = ThisUtilityPath + '\\' + ProjectName + '-Prj.cfg'
    EepromBinaryFile     = ThisUtilityPath + '\\' + ProjectName + '-Eeprom.bin'
    EepromHeaderFile     = ThisUtilityPath + '\\EepromStruct.h'
    EepromPrivatePemFile = ThisUtilityPath + '\\RSA-keys\\' + ProjectName + '-private.pem'
    OpenSslPath          = UtilityPath     + '\\OpenSsl\\openssl.exe'

    # Debug prints
    if False:
      print('sys.argv[0]          = {0}'.format(sys.argv[0]))
      print('ThisUtilityPath      = {0}'.format(ThisUtilityPath))
      print('ParentUtilityPath    = {0}'.format(ParentUtilityPath))
      print('UtilityPath          = {0}'.format(UtilityPath))
      print('EepromBinaryFile     = {0}'.format(EepromBinaryFile))
      print('EepromBinaryUtility  = {0}'.format(EepromBinaryUtility))
      print('EepromDefinitionFile = {0}'.format(EepromDefinitionFile))
      print('EepromHashUtility    = {0}'.format(EepromHashUtility))
      print('EepromHeaderFile     = {0}'.format(EepromHeaderFile))
      print('EepromPrivatePemFile = {0}'.format(EepromPrivatePemFile))
      print('EepromProjectFile    = {0}'.format(EepromProjectFile))
      print('OpenSslPath          = {0}'.format(OpenSslPath))

    # Delete any previous files
    RemoveFile(EepromBinaryFile)
    RemoveFile(EepromHeaderFile)
    for file in glob.glob(ThisUtilityPath + '\\*.aml'):
      RemoveFile(file)

    # Generate unhashed binary
    #   eeprom.py -d EepromDefinitionFile -p EepromProjectFile -b EepromBinaryFile --header EepromHeaderFile
    subprocess.call(['python.exe', EepromBinaryUtility, '-d', EepromDefinitionFile, '-p', EepromProjectFile, '-b', EepromBinaryFile, '--header', EepromHeaderFile])

    # Add hashing/signature
    #   HashBinary.py OpenSSL EepromBinaryFile [PrivateKey]
    subprocess.call(['python.exe', EepromHashUtility, OpenSslPath, EepromBinaryFile, EepromPrivatePemFile])

    # Delete temporary files
    for file in glob.glob(ThisUtilityPath + '\\*.aml'):
      RemoveFile(file)

    # Clear error code
    ReturnCode = 0

  finally:
    return ReturnCode

def RemoveFile(FileName):
  if os.path.exists(FileName):
    os.remove(FileName)
  return

if __name__ == "__main__":
  print('')
  print(VerString)
  print(PythonVersion)
  sys.exit(Main())
