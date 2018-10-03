#  @file
#  Scraps the debug log to generate the DRAM_POLICY_PPI data file for the EEPROM image.
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

import sys

# Version defines
__version__ = '0.1.0.0'
VerString   = 'Scrap the debug log for the DRAM_POLICY_PPI data, Version #{0}'.format(__version__)

# Global defines
DebugFlag      = False
StartingMarker = 'DRAM_POLICY_PPI dump'

def Main():
  # Assume failure
  ReturnCode = 1

  if DebugFlag:
    index = 1
    while len(sys.argv) > index - 1:
      print('sys.argv[{0}] = {1}'.format(index - 1, sys.argv[index - 1]))
      index += 1

  if len(sys.argv) < 3:
    print('Useage: ScrapDramPolicyPpi DebugLog OutputFile')
    return 1

  DebugLogPath   = sys.argv[1]
  OutputFilePath = sys.argv[2]
  if DebugFlag:
    print('DebugLogPath   = {0}'.format(DebugLogPath))
    print('OutputFilePath = {0}'.format(OutputFilePath))

  # Assume failure
  LogFile    = None
  OutputFile = None

  try:
    # Open the files
    LogFile    = open(DebugLogPath,   "r")
    OutputFile = open(OutputFilePath, "wb")

    # Search the Log file for the marker and then scrap the data
    ByteCount = 0
    MarkerFoundFlag = False
    for LogLine in LogFile:
      if StartingMarker in LogLine:
        # Beginning of data found
        MarkerFoundFlag = True
      if len(LogLine) == 1:
        # End of data found
        MarkerFoundFlag = False
      if MarkerFoundFlag:
        # Parse data to write out to the binary file
        if LogLine.startswith(' | '):
          # This is valid data line. Parse it.
          LogLine = LogLine.split(' ')
          PipeCount = 0
          for element in LogLine:
            if element == '|':
              PipeCount += 1
            elif (PipeCount == 2) and (len(element) > 0):
              # Print out what we are writing to the file
              print('0x{0:02X} - 0x{1}'.format(ByteCount, element))
              # Write it to the file
              OutputFile.write(str(chr(int(element, 16))))
              # Increment the byte counter
              ByteCount += 1

    # Clear the return flag
    ReturnCode = 0

  finally:
    # Close files
    if LogFile is not None:
      LogFile.close()
    if OutputFile is not None:
      OutputFile.close()
    return ReturnCode

if __name__ == "__main__":
  print ''
  print(VerString)
  sys.exit (Main())
