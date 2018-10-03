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
__version__   = '0.1.0.0'
VerString     = 'EEPROM DramPolicyPpi binary generation tool, Version #{0}'.format(__version__)
PythonVersion = 'Python version = {0:X}'.format(sys.hexversion)

def Main():
  # Set return code to error
  ReturnCode = 1

  try:
    # Get paths
    ThisUtilityPath      = os.path.dirname(os.path.abspath(sys.argv[0]))
    # Backup until we get to BroxtonPlatformPkg
    TempPath             = ThisUtilityPath
    while os.path.basename(TempPath) != 'BroxtonPlatformPkg':
      TempPath           = os.path.dirname(TempPath)
    UtilityPath          = TempPath        + '\\Common\\Features\\Eeprom\\EepromBinary\\Utilities'
    ScrapDramPolicyPath  = UtilityPath     + '\\ScrapDramPolicyPpi.py'
    OutputFilePath       = ThisUtilityPath + '\\DramPolicyPpi.bin'
    if len(sys.argv) > 1:
      LogFilePath        = sys.argv[1]

    # Debug prints
    if False:
      print('sys.argv[0]          = {0}'.format(sys.argv[0]))
      print('ThisUtilityPath      = {0}'.format(ThisUtilityPath))
      print('UtilityPath          = {0}'.format(UtilityPath))
      print('ScrapDramPolicyPath  = {0}'.format(ScrapDramPolicyPath))
      print('LogFilePath          = {0}'.format(LogFilePath))

    # Generate DramPolicyPpi binary
    #   ScrapDramPolicyPpi.py LogFilePath OutputFilePath
    subprocess.call(['python.exe', ScrapDramPolicyPath, LogFilePath, OutputFilePath])

    # Clear error code
    ReturnCode = 0

  finally:
    return ReturnCode

if __name__ == "__main__":
  print('')
  print(VerString)
  print(PythonVersion)
  sys.exit(Main())
