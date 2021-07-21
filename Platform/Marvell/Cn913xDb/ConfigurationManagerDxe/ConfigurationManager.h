/** @file

  Copyright (c) 2017 - 2021, Arm Limited. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

  @par Glossary:
    - Cm or CM   - Configuration Manager
    - Obj or OBJ - Object
**/

#ifndef CONFIGURATION_MANAGER_H__
#define CONFIGURATION_MANAGER_H__
#include <Library/PcdLib.h>

/** C array containing the compiled AML template.
    These symbols are defined in the auto generated C file
    containing the AML bytecode array.
*/
extern CHAR8  dsdt_aml_code[];
extern CHAR8  ssdt_aml_code[];

/** The configuration manager version
*/
#define CONFIGURATION_MANAGER_REVISION CREATE_REVISION (1, 0)

/** The OEM ID
*/
#define CFG_MGR_OEM_ID    { 'M', 'V', 'E', 'B', 'U' }

#if FixedPcdGet32(PcdMaxCpCount) == 1
#define CFG_MGR_OEM_TABLE_ID    SIGNATURE_64 ('C', 'N', '9', '1', '3', '0', 0, 0)
#elif FixedPcdGet32(PcdMaxCpCount) == 2
#define CFG_MGR_OEM_TABLE_ID    SIGNATURE_64 ('C', 'N', '9', '1', '3', '1', 0, 0)
#else
#define CFG_MGR_OEM_TABLE_ID    SIGNATURE_64 ('C', 'N', '9', '1', '3', '2', 0, 0)
#endif

/** A helper macro for mapping a reference token
*/
#define REFERENCE_TOKEN(Field)                                           \
  (CM_OBJECT_TOKEN)((UINT8*)&Cn913xDbPlatformRepositoryInfo +            \
    OFFSET_OF (EDKII_PLATFORM_REPOSITORY_INFO, Field))

/** A helper macro for populating the GIC CPU information
*/
#define GICC_ENTRY(                                                      \
          CPUInterfaceNumber,                                            \
          Mpidr,                                                         \
          PmuIrq,                                                        \
          VGicIrq,                                                       \
          EnergyEfficiency                                               \
          ) {                                                            \
    CPUInterfaceNumber,       /* UINT32  CPUInterfaceNumber           */ \
    Mpidr,                    /* UINT32  AcpiProcessorUid             */ \
    EFI_ACPI_6_2_GIC_ENABLED | \
    EFI_ACPI_6_2_PERFORMANCE_INTERRUPT_MODEL, /* UINT32  Flags        */ \
    0,                        /* UINT32  ParkingProtocolVersion       */ \
    PmuIrq,                   /* UINT32  PerformanceInterruptGsiv     */ \
    0,                        /* UINT64  ParkedAddress                */ \
    FixedPcdGet64 (                                                      \
      PcdGicInterruptInterfaceBase                                       \
      ),                      /* UINT64  PhysicalBaseAddress          */ \
    0xF0260000,               /* UINT64  GICV                         */ \
    0xF0240000,               /* UINT64  GICH                         */ \
    VGicIrq,                  /* UINT32  VGICMaintenanceInterrupt     */ \
    0,                        /* UINT64  GICRBaseAddress              */ \
    Mpidr,                    /* UINT64  MPIDR                        */ \
    EnergyEfficiency          /* UINT8   ProcessorPowerEfficiencyClass*/ \
    }

/** A helper macro for populating the Processor Hierarchy Node flags
*/
#define PROC_NODE_FLAGS(                                                \
          PhysicalPackage,                                              \
          AcpiProcessorIdValid,                                         \
          ProcessorIsThread,                                            \
          NodeIsLeaf,                                                   \
          IdenticalImplementation                                       \
          )                                                             \
  (                                                                     \
    PhysicalPackage |                                                   \
    (AcpiProcessorIdValid << 1) |                                       \
    (ProcessorIsThread << 2) |                                          \
    (NodeIsLeaf << 3) |                                                 \
    (IdenticalImplementation << 4)                                      \
  )

/** A helper macro for populating the Cache Type Structure's attributes
*/
#define CACHE_ATTRIBUTES(                                               \
          AllocationType,                                               \
          CacheType,                                                    \
          WritePolicy                                                   \
          )                                                             \
  (                                                                     \
    AllocationType |                                                    \
    (CacheType << 2) |                                                  \
    (WritePolicy << 4)                                                  \
  )

/** A function that prepares Configuration Manager Objects for returning.

  @param [in]  This        Pointer to the Configuration Manager Protocol.
  @param [in]  CmObjectId  The Configuration Manager Object ID.
  @param [in]  Token       A token for identifying the object.
  @param [out] CmObject    Pointer to the Configuration Manager Object
                           descriptor describing the requested Object.

  @retval EFI_SUCCESS           Success.
  @retval EFI_INVALID_PARAMETER A parameter is invalid.
  @retval EFI_NOT_FOUND         The required object information is not found.
**/
typedef EFI_STATUS (*CM_OBJECT_HANDLER_PROC) (
  IN  CONST EDKII_CONFIGURATION_MANAGER_PROTOCOL  * CONST This,
  IN  CONST CM_OBJECT_ID                                  CmObjectId,
  IN  CONST CM_OBJECT_TOKEN                               Token,
  IN  OUT   CM_OBJ_DESCRIPTOR                     * CONST CmObject
  );

/** The number of CPUs
*/
#define PLAT_CPU_COUNT          4

/** The number of ACPI tables to install
*/
#if FixedPcdGet32(PcdMaxCpCount) == 2
#define PLAT_ACPI_TABLE_COUNT   9
#else
#define PLAT_ACPI_TABLE_COUNT   8
#endif

/** The number of Processor Hierarchy Nodes
    - one package node
    - two cluster nodes
    - two cores per cluster
*/
#define PLAT_PROC_HIERARCHY_NODE_COUNT  7

/** The number of unique cache structures:
    - package L3 cache
    - cluster L2 cache
    - core L1 instruction cache
    - core L1 data cache
*/
#define PLAT_CACHE_COUNT                4

/** The number of resources private to core instance
    - L1 data cache
    - L1 instruction cache
*/
#define CORE_RESOURCE_COUNT  2

// active low, level triggered
#define GTDT_GTIMER_FLAGS         EFI_ACPI_6_0_GTDT_TIMER_FLAG_ALWAYS_ON_CAPABILITY
#define GTDT_WDG_FLAGS            0x0

#define CN913X_DBG2_UART_REG_BASE 0xF2702000

#define GIC_MSI_FRAME0            0xF0280000
#define GIC_MSI_FRAME1            0xF0290000
#define GIC_MSI_FRAME2            0xF02A0000
#define GIC_MSI_FRAME3            0xF02B0000

/** A structure describing the platform configuration
    manager repository information
*/
typedef struct PlatformRepositoryInfo {
  /// Configuration Manager Information
  CM_STD_OBJ_CONFIGURATION_MANAGER_INFO CmInfo;

  /// List of ACPI tables
  CM_STD_OBJ_ACPI_TABLE_INFO            CmAcpiTableList[PLAT_ACPI_TABLE_COUNT];

  /// Boot architecture information
  CM_ARM_BOOT_ARCH_INFO                 BootArchInfo;

  /// Power management profile information
  CM_ARM_POWER_MANAGEMENT_PROFILE_INFO  PmProfileInfo;

  //Fixed feature flags for the Platform
  CM_ARM_FIXED_FEATURE_FLAGS            FixedFeatureFlags;

  /// Generic timer information
  CM_ARM_GENERIC_TIMER_INFO             GenericTimerInfo;

  /// Watchdog information
  CM_ARM_GENERIC_WATCHDOG_INFO          Watchdog;

  /// GIC CPU interface information
  CM_ARM_GICC_INFO                      GicCInfo[PLAT_CPU_COUNT];

  /// GIC MSI Frame information
  CM_ARM_GIC_MSI_FRAME_INFO             GicMsiFrameInfo[PLAT_CPU_COUNT];

  /// GIC distributor information
  CM_ARM_GICD_INFO                      GicDInfo;

  /// Serial port information for the serial port console redirection port
  CM_ARM_SERIAL_PORT_INFO               SpcrSerialPort;

  /// Serial port information for the DBG2 UART port
  CM_ARM_SERIAL_PORT_INFO               DbgSerialPort;

  /// PCI configuration space information
  CM_ARM_PCI_CONFIG_SPACE_INFO          PciConfigInfo;

  // Processor topology information
  CM_ARM_PROC_HIERARCHY_INFO            ProcHierarchyInfo[PLAT_PROC_HIERARCHY_NODE_COUNT];

  // Cache information
  CM_ARM_CACHE_INFO                     CacheInfo[PLAT_CACHE_COUNT];

  // package private resources
  CM_ARM_OBJ_REF                        PackageResources;

  // cluster private resources
  CM_ARM_OBJ_REF                        ClusterResources;

  // core private resources
  CM_ARM_OBJ_REF                        CoreResources[CORE_RESOURCE_COUNT];

} EDKII_PLATFORM_REPOSITORY_INFO;

#endif // CONFIGURATION_MANAGER_H__
