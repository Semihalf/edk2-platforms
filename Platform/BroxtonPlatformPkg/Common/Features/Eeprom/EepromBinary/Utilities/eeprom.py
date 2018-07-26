#  @file
#  Produces binary file to be placed in an EEPROM.  The format and contents of the
#  EEPROM are defined by cofiguration files.  The first configuration file defines the
#  available structures and layout rules.  The second configuration file defines the
#  contents of the EEPROM image.
#
#  Copyright (c) 2015 - 2017, Intel Corporation. All rights reserved.<BR>
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
import sys
import copy
import array
import argparse
import binascii
import subprocess
import ConfigParser
import re as regex
from collections import OrderedDict

__version__ = '0.6.0.1'
VerString = 'EEPROM Generator Ver. {0}'.format(__version__)
InVerboseMode = False

ReservedCfgSections = ['Defines', 'Patch', 'Structures']
ReservedCfgNames = ['StructureType'.lower()]

Types = {0x0:'UINT8',0x1:'UINT8',0x2:'UINT16', 0x4:'UINT32'}

Defines="#ifndef _EEPROM_LAYOUT_H_\n#define _EEPROM_LAYOUT_H_\n\n"
Pragmas="#pragma pack(1)\n"

PragmasEnd = "#pragma pack()\n"
DefinesEnd = "#endif\n"

FileHeader = Defines + Pragmas
FileTail = PragmasEnd + DefinesEnd

###############################################################################
class ProjData:
    def __init__(self):
        self.DataTypes = None
        self.ProjCfg = None
        self.AlignnmentMask = None
        self.ReqStructList = None
        self.MaxReqStructSize = None
        self.ReqOrderDict = None
        self.StructLengthPatch = None
        self.BinLengthPatch = None
        self.CrcPatch = None

###############################################################################

def GetValue(Str):
    if Str is None:
        return None

    #
    # Check to see if this should be processed as a string.
    #
    TmpStr = Str.strip()
    if TmpStr.startswith('"') and TmpStr.endswith('"'):
        return TmpStr.replace('"', '')

    #
    # Assume it is a number as a last resort and convert.
    #
    return StrToInt(Str)


def StrToInt(Str):
    Base = 10

    #
    # Check parameter
    #
    if Str is None:
        return None

    #
    # Verify that the value is a number
    #
    TmpStr = Str.strip().lower()
    if not TmpStr.isalnum():
        return None

    #
    # Determine the base of the number for conversion
    #
    if TmpStr.startswith('0x'):
        Base = 16

    return int(TmpStr, base=Base)


###############################################################################

def ErrorPrint(Str):
    print('ERROR: {0}'.format(Str))

def WarningPrint(Str):
    print('WARNING: {0}'.format(Str))

def DebugPrint(Str):
    if InVerboseMode:
        print(Str)

###############################################################################

def DumpBinData(BinData):
    if not InVerboseMode:
        return

    ByteCount = 0
    TmpLine = '{0:04X}:'.format(ByteCount)
    for Byte in BinData:
        TmpLine += ' {0:02X}'.format(Byte)
        ByteCount += 1
        if ByteCount % 16 == 0:
            print(TmpLine)
            TmpLine = '{0:04X}:'.format(ByteCount)
    if not TmpLine.endswith(':'):
        print(TmpLine)
    print('Data size: {0:X}'.format(ByteCount))

def CheckFile(FilePath):
    #
    # Validate input
    #
    if FilePath is None:
        return False

    #
    # Check to see if the file exists
    #
    if not os.path.isfile(FilePath):
        return False

    return True

def OpenCfg(FilePath):
    #
    # Validate input
    #
    if FilePath is None:
        return None

    #
    # Fix up path and verify that the file exists
    #
    TmpPath = os.path.abspath(os.path.normpath(FilePath))
    if not CheckFile(TmpPath):
        ErrorPrint('File does not exist: {0}'.format(TmpPath))
        return None

    #
    # Open the configuration file and return the config parser object.
    #
    CfgObj = ConfigParser.ConfigParser()
    try:
        CfgObj.read(TmpPath)
    except:
        ErrorPrint('Unable to parser configuration file: {0}'.format(TmpPath))
        return None

    return CfgObj

def SetStr(Offset, Width, Value, BinData):
    StrLen = len(Value)
    if StrLen > Width:
        ErrorPrint('Value too large for field at offset {0:02X}'.format(Offset))
        return

    Index = Offset
    StrIndex = 0
    while(Index < Offset + Width):
        if StrIndex >= StrLen:
            BinData[Index] = ord(' ')
        else:
            BinData[Index] = ord(Value[StrIndex])
        Index += 1
        StrIndex += 1

    return

def SetInt(Offset, Width, Value, BinData):
    TmpValue = Value
    Index = Offset
    while(Index < Offset + Width):
        BinData[Index] = (TmpValue & 0xFF)
        TmpValue = TmpValue >> 8
        Index += 1

    return

def SetBinArray(Offset, Width, Value, BinData):
    InputLength = len(Value)
    DataIndex = 0
    BinIndex = Offset
    while(BinIndex < Offset + Width):
        if DataIndex < InputLength:
            BinData[BinIndex] = Value[DataIndex]
        else:
            BinData[BinIndex] = 11

        DataIndex += 1
        BinIndex += 1

def SetValue(Offset, Width, Value, BinData):
    if isinstance(Value, str):
        SetStr(Offset, Width, Value, BinData)
    elif isinstance(Value, int) or isinstance(Value, long):
        SetInt(Offset, Width, Value, BinData)
    elif isinstance(Value, array.array):
        SetBinArray(Offset, Width, Value, BinData)
    else:
        ErrorPrint('Unable to set value at offset {0:02X}'.format(Offset))

    return

def SetDefValue(Type, FieldDict, BinData):
    if Type is None or FieldDict is None:
        return
    SetValue(FieldDict['Offset'], FieldDict['Width'], FieldDict['Default'], BinData)

    return

def CreateDefData(Type, ProjInfo):
    BinData = array.array('B')

    #
    # Determine the size of the entire structure if possible.  Some structures are variable
    # length and the data will be appended to the end.
    #
    StructLength = 0
    for Key in ProjInfo.DataTypes[Type].keys():
        if not isinstance(ProjInfo.DataTypes[Type][Key], dict):
            continue
        Offset = ProjInfo.DataTypes[Type][Key]['Offset']
        Width = ProjInfo.DataTypes[Type][Key]['Width']
        if Offset + Width > StructLength:
            StructLength = Offset + Width

    #
    # Zero fill the array
    #
    for item in range(StructLength):
        BinData.append(0)

    #
    # Set default values in structure
    #
    for Field in ProjInfo.DataTypes[Type].keys():
        if not isinstance(ProjInfo.DataTypes[Type][Field], dict):
            continue
        SetDefValue(Type, ProjInfo.DataTypes[Type][Field], BinData)

    return BinData

def CompileiAsl(IaslPath, InputFile):
    DebugPrint("\nCompiling {0}...".format(InputFile))
    if not os.path.isfile(InputFile):
        ErrorPrint("{0} doesn't exist".format(InputFile))
        return None
    if os.path.isdir(IaslPath):
       IaslPath = os.path.join(IaslPath,"iasl")
    try:
        OutputFile="EepromAsl.aml"
        prefixArg = "-p{0}".format(OutputFile)
        if InVerboseMode:
            subprocess.check_call([IaslPath, prefixArg, InputFile])
        else:
            NoOutputFile = open(os.devnull, 'w')
            subprocess.check_call([IaslPath,prefixArg,InputFile],stdout=NoOutputFile)
    except subprocess.CalledProcessError:
        ErrorPrint("Invalid ASL file ( {0} )".format(InputFile))
        OutputFile = None
    except OSError:
        IasFile = os.path.basename(IaslPath)
        Path = IaslPath.replace(IasFile,"")
        OutputFile = None
        ErrorPrint("Could not find {0} in {1}".format(IasFile, Path))
    DebugPrint("\n")
    return OutputFile

def OverrideData(Type, ProjInfo, StructSection, BinData):
    StructDefs = ProjInfo.DataTypes[Type]

    #
    # Process any overrides
    #
    NameVals = ProjInfo.ProjCfg.items(StructSection)
    for Name, Val in NameVals:
        TmpBinData = array.array('B')

        #
        # Check to see if the data should be from a file.
        #
        if Val.startswith('from_file'):
            FileName = Val.replace('from_file', '').strip().strip('()').strip()
            TmpBinData = array.array('B')
            try:
                f = open(FileName, 'rb')
                f.seek(0, os.SEEK_END)
                FileSize = f.tell()
                f.seek(0, os.SEEK_SET)
                TmpBinData.fromfile(f, FileSize)
                f.close()
            except Exception as e:
                ErrorPrint('{0}'.format(e))
                continue
        elif Val.startswith('from_asl'):
            params = Val.replace('from_asl', '').strip().strip('()').strip()
            (IaslPath, InputFile) = params.split(',')
            #compile ASL with iasl
            OutputFile = CompileiAsl(IaslPath, InputFile)
            if OutputFile is None:
                return None
            TmpBinData = array.array('B')
            try:
                f = open(OutputFile, 'rb')
                f.seek(0, os.SEEK_END)
                FileSize = f.tell()
                f.seek(0, os.SEEK_SET)
                TmpBinData.fromfile(f, FileSize)
                f.close()
            except Exception as e:
                ErrorPrint('{0}'.format(e))
                continue
        elif Val.startswith('from_struct_list'):
            Section = Val.replace('from_struct_list', '').strip().strip('()').strip()
            SectionData = BuildMultipleStructs(ProjInfo, Section)
            if SectionData is None or len(SectionData) == 0:
                return None
            for StructType, Bin in SectionData:
                TmpBinData.extend(Bin)
        elif Val.startswith('from_struct'):
            StructSection = Val.replace('from_struct', '').strip().strip('()').strip()
            TmpBinData = BuildSingleStruct(ProjInfo, StructSection)
            if TmpBinData is None or len(TmpBinData) == 0:
                return None
        elif Name in StructDefs.keys():
            Val = GetValue(Val)
            if Val is None:
                continue
            SetValue(StructDefs[Name]['Offset'], StructDefs[Name]['Width'], Val, BinData)
        elif Name in ReservedCfgNames:
            continue
        else:
            WarningPrint('Unable to set field: {0} in {1}'.format(Name, Type))

        #
        # Apply data to the correct field.
        #
        if len(TmpBinData) > 0:
            if StructDefs[Name]['Width'] == 0:
                #
                # Just extend the array
                #
                BinData.extend(TmpBinData)
            else:
                #
                # Actually set the value
                #
                SetValue(StructDefs[Name]['Offset'], StructDefs[Name]['Width'], TmpBinData, BinData)

    #
    # Make sure the structure is padded for alignment requirements
    #
    DataLength = len(BinData)
    if DataLength & ProjInfo.AlignmentMask != 0:
        NewStart = (DataLength + ProjInfo.AlignmentMask) & ~ProjInfo.AlignmentMask
        Index = DataLength
        while(Index < NewStart):
            BinData.append(0)
            Index += 1

    #
    # If a length field exists update it with the current size of the structure.
    #
    for Patch in ProjInfo.StructLengthPatch:
        Patch = Patch.lower()
        if Patch in StructDefs.keys():
            SetValue(StructDefs[Patch]['Offset'], StructDefs[Patch]['Width'], len(BinData), BinData)

    return BinData

def ParseArguments():
    Parser = argparse.ArgumentParser()
    Parser.add_argument('-d', '--deff', action='store', default=None,
            dest='Definitions', nargs=1,
            help='Provides a path to the definition file for the image.')
    Parser.add_argument('-p', '--project', action='store', default=None,
            dest='Project', nargs=1,
            help='Provides a path to the project configuration file.')
    Parser.add_argument('-b', '--binary', action='store', default=None,
            dest='Binary', nargs=1,
            help='Output binary file name.')
    Parser.add_argument('--header', action='store', default=None,
            dest='HeaderFile', nargs=1,
            help='Provides a file name for a C header file to be generated.')
    Parser.add_argument('--verbose', action='store_true', default=False,
            dest='Verbose', help='Enables verbose output for debugging.')
    Parser.add_argument('--version', action='store_true', default=False,
            dest='Version',
            help='Displays the version information and exits the program.')
    Args = Parser.parse_args()

    #
    # Display version information and exit
    #
    if Args.Version:
        print(VerString)
        sys.exit(0)

    #
    # Handle missing file information
    #
    if Args.Definitions is None:
        ErrorPrint('Missing structure definition file.')
        Parser.print_help()
        sys.exit(1)
    if Args.Project is None and Args.HeaderFile is None:
        ErrorPrint('Missing project file.')
        Parser.print_help()
        sys.exit(1)

    #
    # Set global verbose mode flag.
    #
    if Args.Verbose:
        global InVerboseMode
        InVerboseMode = True

    return Args

def GetCfgValue(CfgObj, Section, Option):
    RetVal = ''
    if CfgObj.has_section(Section) and CfgObj.has_option(Section, Option.lower()):
        RetVal = CfgObj.get(Section, Option.lower())

    return RetVal

def GetCfgValueList(CfgObj, Section, Option):
    RetVal = []
    OptVal = GetCfgValue(CfgObj, Section, Option)
    if len(OptVal) > 0:
        if OptVal.find('|') >= 0:
            Opts = OptVal.split('|')
        else:
            Opts = [OptVal]
        for OptVal in Opts:
            RetVal.append(OptVal.strip())

    return RetVal

def GetCfgDictList(CfgObj, Section, Option):
    RetVal = {}
    Opts = GetCfgValueList(CfgObj, Section, Option)
    if len(Opts) > 0:
        for Item in Opts:
            if Item.find(':') < 0:
                WarningPrint('Possible bad format in option {0}'.format(Option))
                continue
            Vals = Item.split(':')
            if len(Vals) != 2:
                WarningPrint('Possible bad format in option {0}'.format(Option))
                continue
            RetVal[Vals[0].strip()] = Vals[1].strip()

    return RetVal

def GetPatchLoc(TypeDict, Type, PatchList, BinLen):
    RetVal = []
    for PatchName in PatchList:
        PatchName = PatchName.lower()
        if PatchName in TypeDict[Type].keys():
            PatchLoc = copy.deepcopy(TypeDict[Type][PatchName])
            PatchLoc['Offset'] = PatchLoc['Offset'] + BinLen
            RetVal.append(PatchLoc)
    return RetVal

def BuildSingleStruct(ProjInfo, StructSection):
    BinData = None

    #
    # Validate information and find the structure to build
    #
    if not ProjInfo.ProjCfg.has_section(StructSection):
        ErrorPrint('Unable to find section [{0}].'.format(StructSection))
        return BinData
    StructType = GetCfgValue(ProjInfo.ProjCfg, StructSection, 'StructureType')
    if len(StructType) == 0:
        ErrorPrint('Missing required field "StructureType" in section {0}'.format(StructSection))
        return BinData

    #
    # Build the default structure
    #
    BinData = CreateDefData(StructType, ProjInfo)
    if BinData is None:
        return BinData

    #
    # Fill in override values for the structure
    #
    BinData = OverrideData(StructType, ProjInfo, StructSection, BinData)
    if BinData is None:
        ErrorPrint('Unable to create default binary data for {0}'.format(StructType))

    return BinData

def BuildMultipleStructs(ProjInfo, Section):
    BinList = []

    if not ProjInfo.ProjCfg.has_section(Section):
        ErrorPrint('Missing section [{0}].'.format(Section))
        return None

    for Field, StructSection in ProjInfo.ProjCfg.items(Section):
        #
        # Build the default structure
        #
        BinData = BuildSingleStruct(ProjInfo, StructSection)
        if BinData is None:
            return None
        DumpBinData(BinData)

        #
        # If the structure has to be the first structure insert it at the top of the list.
        #
        StructType = GetCfgValue(ProjInfo.ProjCfg, StructSection, 'StructureType')
        if ProjInfo.ReqOrderDict.has_key(StructType):
            Index = StrToInt(ProjInfo.ReqOrderDict[StructType])
            BinList.insert(Index, (StructType, BinData))
            for Key in ProjInfo.ReqOrderDict.keys():
                if StrToInt(ProjInfo.ReqOrderDict[Key]) == Index:
                    ProjInfo.ReqOrderDict.pop(Key)
        else:
            BinList.append((StructType, BinData))

    return BinList

def VerifyCfg(CfgObj, Tool, Type):
    if not CfgObj.has_section('Defines'):
        ErrorPrint('Unable to find Defines section.')
        return False
    Option = GetCfgValue(CfgObj, 'Defines', 'Tool')
    if len(Option) == 0:
        ErrorPrint('Unable to determine tool compatibility')
        return False
    elif Option != Tool:
        ErrorPrint('Configuration file not compatible with {0}'.format(Tool))
        return False
    Option = GetCfgValue(CfgObj, 'Defines', 'Type')
    if len(Option) == 0:
        ErrorPrint('Unable to verify file Type.')
        return False
    elif Option != Type:
        ErrorPrint('Not a EEPROM "{0}" file.'.format(Type))
        return False

    #
    # Display information about the configuration file being used.
    #
    Option = GetCfgValue(CfgObj, 'Defines', 'Platform')
    if len(Option) > 0:
        print('- Platform: {0}'.format(Option))
    Option = GetCfgValue(CfgObj, 'Defines', 'Version')
    if len(Option) > 0:
        print('- Version:  {0}'.format(Option))
    print('- Type:     {0}\n'.format(Type))

    return True

#
# Convert Section Identifier from CamelCase to snake case
#
def ConvertFromCamelCaseToSnakeCase(String):
    RegexPattern1 = "(.)([A-Z][a-z]+)"
    RegexPattern2 = "([a-z0-9])([A-Z])"
    tempSub = regex.sub(RegexPattern1, r'\1_\2', String)
    return regex.sub(RegexPattern2, r'\1_\2', tempSub)

#
# Generates a C header file from the definitions configuration data.
#
def GenerateHeaderFile(HeaderFileName, DataTypes):
    GeneratedStructures = FileHeader
    with open(HeaderFileName, "w") as HeaderFile:
        #
        # Generate data structure dictionary based off of configuration file.
        #
        for Section in DataTypes.keys():
            #
            # Skip sections that are not data structures
            #
            GeneratedStructures += ""
            StructureFields = ""
            if Section in ReservedCfgSections:
                continue
            GeneratedStructures += "\ntypedef struct {\n"

            for Field in DataTypes[Section].keys():
                #
                # Handle normal structure fields
                #
                StructureFieldsWidth = DataTypes[Section][Field]['Width']
                FieldType = Types.get(StructureFieldsWidth, None)
                FieldTypeAndIdentifier = ""

                #
                # Generate Type
                #
                if FieldType is None:
                    FieldTypeAndIdentifier = "    {0:8}{1}[{2}];\n".format('CHAR8', Field, StructureFieldsWidth)
                else:
                    if StructureFieldsWidth == 0:
                        FieldTypeAndIdentifier = "//  {0:8}{1}[0];\n".format(FieldType, Field)
                    else:
                        FieldTypeAndIdentifier = "    {0:8}{1};\n".format(FieldType, Field)

                #
                # Append Identifier to Type
                #
                StructureFields=StructureFields+FieldTypeAndIdentifier

            #
            # Close Structure with converted upper CamelCase Section name
            #
            StructureFields=StructureFields + '}' + "{0};\n\n".format(ConvertFromCamelCaseToSnakeCase(Section).upper())

            #
            # Append generated structure to the previously generated structures' string
            #
            GeneratedStructures+=StructureFields

        #
        # Write the generated structures to file
        #
        HeaderFile.write(GeneratedStructures + FileTail)
    HeaderFile.close()

def Main():
    ProjInfo = ProjData()

    #
    # Get command line arguments
    #
    Args = ParseArguments()
    print('{0}\n'.format(VerString))

    #
    # Load supported structures
    #
    DataTypeCfg = OpenCfg(Args.Definitions[0])
    if DataTypeCfg is None:
        return 1
    print('Processing {0}'.format(Args.Definitions[0]))
    if not VerifyCfg(DataTypeCfg, 'EEPROM', 'Definitions'):
        return 1

    #
    # Check to see if any alignment is required for the binary image.
    #
    ProjInfo.AlignmentMask = 0
    if DataTypeCfg.has_option('Defines', 'ReqAlignment'.lower()):
        ProjInfo.AlignmentMask = GetValue(DataTypeCfg.get('Defines', 'ReqAlignment'.lower())) - 1

    #
    # Determine the required structure types.
    #
    ProjInfo.ReqStructList = GetCfgValueList(DataTypeCfg, 'Defines', 'RequiredStructs')
    ProjInfo.MaxReqStructSize = StrToInt(GetCfgValue(DataTypeCfg, 'Defines', 'MaxReqStructSize'))
    ProjInfo.ReqOrderDict = GetCfgDictList(DataTypeCfg, 'Defines', 'RequiredIndex')

    #
    # Determine labels to patch.
    #
    ProjInfo.StructLengthPatch = GetCfgValueList(DataTypeCfg, 'Patch', 'StructSize')
    ProjInfo.BinLengthPatch = GetCfgValueList(DataTypeCfg, 'Patch', 'BinSize')
    ProjInfo.CrcPatch = GetCfgValueList(DataTypeCfg, 'Patch', 'Crc32')

    #
    # Generate data structure dictionary based off of configuration file.
    #
    ProjInfo.DataTypes = OrderedDict()
    for Section in DataTypeCfg.sections():
        #
        # Skip sections that are not data structures
        #
        if Section in ReservedCfgSections:
            continue
        FieldList = []
        for Field, Value in DataTypeCfg.items(Section):
            #
            # Get structure fields and place them in a list
            #
            Vals = Value.strip().split('|')
            FieldList.append((Field.strip(), Vals))
        Sorted = False
        while not Sorted:
            Swap = False
            for Index in range(1, len(FieldList)):
                if FieldList[Index][1][0] < FieldList[Index-1][1][0]:
                    FieldList.insert(Index - 1, FieldList.pop(Index))
                    Swap = True
            if not Swap:
                Sorted = True
        if Section not in ProjInfo.DataTypes.keys():
            ProjInfo.DataTypes[Section] = OrderedDict()
        for Field, Vals in FieldList:
            ProjInfo.DataTypes[Section][Field] = OrderedDict()
            ProjInfo.DataTypes[Section][Field]['Offset'] = GetValue(Vals[0])
            ProjInfo.DataTypes[Section][Field]['Width'] = GetValue(Vals[1])
            ProjInfo.DataTypes[Section][Field]['Default'] = GetValue(Vals[2])

    #
    # Dump the dictionary if verbose output requrested
    #
    if InVerboseMode:
        print('Data Types:')
        for SecKey in ProjInfo.DataTypes.keys():
            print('- Struct: {0}'.format(SecKey))
            for FieldKey in ProjInfo.DataTypes[SecKey].keys():
                print('    {0:15}: 0x{1:>04X}'.format(FieldKey, ProjInfo.DataTypes[SecKey][FieldKey]['Offset']))
        print('')

    #
    # Generate the header file here if one was requested.
    #
    if Args.HeaderFile is not None:
        print('Generating header file: {0}'.format(Args.HeaderFile[0]))
        GenerateHeaderFile(Args.HeaderFile[0], ProjInfo.DataTypes)
        print('')
    if Args.Project is None:
        return 0

    #
    # Load platform configuration data
    #
    ProjInfo.ProjCfg = OpenCfg(Args.Project[0])
    if ProjInfo.ProjCfg is None:
        return 1
    print('Processing {0}'.format(Args.Project[0]))
    if not VerifyCfg(ProjInfo.ProjCfg, 'EEPROM', 'Project'):
        return 1

    #
    # Process Structure of the platform data
    #
    print('Generating structures...')
    BinList = []
    Section = 'Structures'
    TmpList = BuildMultipleStructs(ProjInfo, Section)
    if TmpList is None:
        return 1
    BinList.extend(TmpList)

    #
    # Generate final image
    #
    print('Generating binary image...')
    VerifyReqSize = True
    FileSizePatchList = []
    CrcPatchList = []
    FileImage = array.array('B')
    for Type, Struct in BinList:
        #
        # Determine patch locations
        #
        FileSizePatchList.extend(GetPatchLoc(ProjInfo.DataTypes, Type, ProjInfo.BinLengthPatch, len(FileImage)))
        CrcPatchList.extend(GetPatchLoc(ProjInfo.DataTypes, Type, ProjInfo.CrcPatch, len(FileImage)))

        #
        # Add data blob to the file image
        #
        FileImage.extend(Struct)

        #
        # Structure added so see if it is a required structure and remove it from the list.
        #
        if Type in ProjInfo.ReqStructList:
            ProjInfo.ReqStructList.remove(Type)

        #
        # Check that all the required structures are found and that are within the required size.
        #
        if VerifyReqSize and len(ProjInfo.ReqStructList) == 0:
            VerifyReqSize = False
            if len(FileImage) > ProjInfo.MaxReqStructSize:
                ErrorPrint('Required structures to large to fit in the first {0} bytes.'.format(ProjInfo.MaxReqStructSize))
                return 1

    #
    # Check for any missing required structures.
    #
    if len(ProjInfo.ReqStructList) != 0:
        ErrorPrint('Missing required structures...')
        for ReqStruct in ProjInfo.ReqStructList:
            print ('* {0}'.format(ReqStruct))
        return 1

    #
    # Patch file size information.
    #
    BinSize = len(FileImage)
    DebugPrint('File Size: {0:X}'.format(BinSize))
    for PatchLoc in FileSizePatchList:
        DebugPrint('Updating File Size: {0:08X} @ {1:08X}'.format(BinSize, PatchLoc['Offset']))
        SetValue(PatchLoc['Offset'], PatchLoc['Width'], BinSize, FileImage)

    #
    # Compute CRC32 for the header if the field exists
    #
    Crc32Value = 0
    Crc32Value = binascii.crc32(FileImage) & 0xFFFFFFFF
    for CrcLoc in CrcPatchList:
        DebugPrint('Updating CRC32 value: {0:08X} @ {1:08X}'.format(Crc32Value, CrcLoc['Offset']))
        SetValue(CrcLoc['Offset'],
                 CrcLoc['Width'],
                 int(Crc32Value),
                 FileImage)

    #
    # Determine file name to use.
    #
    FileName = 'eeprom.bin'
    if ProjInfo.ProjCfg.has_section('Defines') and ProjInfo.ProjCfg.has_option('Defines', 'OutputFileName'.lower()):
        FileName = ProjInfo.ProjCfg.get('Defines', 'OutputFileName'.lower())
    if Args.Binary is not None:
        FileName = Args.Binary[0]

    #
    # Write final image to output file
    #
    print('Writing file: {0}'.format(FileName))
    try:
        f = open(FileName, 'wb')
        FileImage.tofile(f)
        f.close()
    except Exception as e:
        print('ERROR: {0}'.format(e))

    return 0

if __name__ == '__main__':
    sys.exit(Main())
