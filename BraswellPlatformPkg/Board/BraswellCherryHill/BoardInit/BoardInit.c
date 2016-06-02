/** @file
  Board Init driver.

  Copyright (c) 2010 - 2016, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include <Library/PeiServicesLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Guid/PlatformInfo.h>
#include <Ppi/BoardDetection.h>
#include <Ppi/ChvPlatformPolicyPpi.h>
#include <Protocol/PlatformGopPolicy.h>
#include <Library/GpioLib.h>
#include <Library/PchPlatformLib.h>
#include <PlatformGpioTable.h>
//
// TBD: Need update to CH version
//
extern UINT8  mBSW_CR_SpdDataMemorySolderDown[];

//
// TBD: Need update to CH version
//
CHV_GPIO_PAD_INIT  mBswCherryHillGpioInitData[] =
{
  //
  // North Community
  //              Group Pin#: PAD Name           Mode Select    Mode#   GPIO Config   GPIO TX State      GPIO Light       INT Type         INT Select    Term H/L   Open Drain   Current Source    Mask/Un-Mask        Glitch Cfg           InvertRX_TX        WAKE Cap   Wake_Mask_Bit     GPE        MMIO Addr     Community             Remarks
  CHV_GPIO_PAD_CONF  (L"N00: GPIO_DFX0       ",    Native  ,    M5   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    0     ,    NA    ,    0x4400    ,    NORTH    )  ,  //   C0_BPM0_TX
  CHV_GPIO_PAD_CONF  (L"N01: GPIO_DFX3       ",    Native  ,    M5   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   Inv_TX_Enable   ,      NA       ,    1     ,    NA    ,    0x4408    ,    NORTH    )  ,  //   C0_BPM3_TX
  CHV_GPIO_PAD_CONF  (L"N02: GPIO_DFX7       ",    GPIO    ,    M1   ,       GPO   ,         LOW   ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    2     ,    NA    ,    0x4410    ,    NORTH    )  ,  //
  CHV_GPIO_PAD_CONF  (L"N03: GPIO_DFX1       ",    Native  ,    M5   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    3     ,    NA    ,    0x4418    ,    NORTH    )  ,  //   C0_BPM1_TX
  CHV_GPIO_PAD_CONF  (L"N04: GPIO_DFX5       ",    GPIO    ,    M1   ,       GPO   ,         HIGH  ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    4     ,    NA    ,    0x4420    ,    NORTH    )  ,  //
  CHV_GPIO_PAD_CONF  (L"N05: GPIO_DFX4       ",    GPIO    ,    M1   ,       GPO   ,         HIGH  ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    5     ,    NA    ,    0x4428    ,    NORTH    )  ,  //
  CHV_GPIO_PAD_CONF  (L"N06: GPIO_DFX8       ",    Native    ,  M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    6     ,    NA    ,    0x4430    ,    NORTH    )  ,  //   JACK DETECT?
  CHV_GPIO_PAD_CONF  (L"N07: GPIO_DFX2       ",    Native  ,    M5   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    7     ,    NA    ,    0x4438    ,    NORTH    )  ,  //   C0_BPM2_TX
  CHV_GPIO_PAD_CONF  (L"N08: GPIO_DFX6       ",    Native  ,    M8   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    8     ,    NA    ,    0x4440    ,    NORTH    )  ,  //
  CHV_GPIO_PAD_CONF  (L"N15: GPIO_SUS0       ",    GPIO    ,    M1   ,       GPI   ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    9     ,    NA    ,    0x4800    ,    NORTH    )  ,  //   GPIO0_SUS0
  CHV_GPIO_PAD_CONF  (L"N16: SEC_GPIO_SUS10  ",    GPIO    ,    M1   ,       GPI   ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    10    ,    NA    ,    0x4808    ,    NORTH    )  ,  //   GPIO_SUS10
  CHV_GPIO_PAD_CONF  (L"N17: GPIO_SUS3       ",    GPIO    ,    M1   ,       GPI   ,         NA    ,     NA         ,   Trig_Level      ,     Line1   ,    NA    ,        NA  ,         NA     ,    NonMaskable  ,   En_Edge_RX_Data  ,   No_Inversion    ,      NA       ,    11    ,    NA    ,    0x4810    ,    NORTH    )  ,  //   NGFF_SDIO_WAKE_N
  CHV_GPIO_PAD_CONF  (L"N18: GPIO_SUS7       ",    GPIO    ,    M1   ,       GPI   ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    12    ,    SMI   ,    0x4818    ,    NORTH    )  ,  //   GPIO_SUS7
  CHV_GPIO_PAD_CONF  (L"N19: GPIO_SUS1       ",    GPIO    ,    M1   ,       GPI   ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    13    ,    NA    ,    0x4820    ,    NORTH    )  ,  //   GPIO_SUS1
  CHV_GPIO_PAD_CONF  (L"N20: GPIO_SUS5       ",    GPIO    ,    M1   ,       GPO   ,         HIGH  ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    14    ,    NA    ,    0x4828    ,    NORTH    )  ,  //   NGFF_KILL_WIFI_N
  CHV_GPIO_PAD_CONF  (L"N21: SEC_GPIO_SUS11  ",    GPIO    ,    M1   ,       GPI   ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    15    ,    NA    ,    0x4830    ,    NORTH    )  ,  //   GPIO_SUS11
  CHV_GPIO_PAD_CONF  (L"N22: GPIO_SUS4       ",    GPIO    ,    M1   ,       GPO   ,         HIGH  ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    16    ,    NA    ,    0x4838    ,    NORTH    )  ,  //   NGFF_KILL_BT_N
  CHV_GPIO_PAD_CONF  (L"N23: SEC_GPIO_SUS8   ",    GPIO    ,    M1   ,       GPI   ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    17    ,    NA    ,    0x4840    ,    NORTH    )  ,  //   GPIO_SUS8
  CHV_GPIO_PAD_CONF  (L"N24: GPIO_SUS2       ",    GPIO    ,    M1   ,       GPI   ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    18    ,    NA    ,    0x4848    ,    NORTH    )  ,  //   NGFF_PWR_EN
  CHV_GPIO_PAD_CONF  (L"N25: GPIO_SUS6       ",    GPIO    ,    M1   ,       GPI   ,         NA    ,     NA         ,   Trig_Edge_Low   ,     Line9   ,    NA    ,        NA  ,         NA     ,    NonMaskable  ,   En_Edge_RX_Data  ,   No_Inversion    ,      NA       ,    19    ,    SCI   ,    0x4850    ,    NORTH    )  ,  //   XDP_HLT_BOOT
  CHV_GPIO_PAD_CONF  (L"N26: CX_PREQ_B       ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    20    ,    NA    ,    0x4858    ,    NORTH    )  ,  //   PREQ_B
  CHV_GPIO_PAD_CONF  (L"N27: GPIO_SUS9       ",    GPIO    ,    M1   ,       GPI   ,         LOW   ,     NA         ,   Trig_Level      ,     Line15  ,    NA    ,        NA  ,         NA     ,    NonMaskable  ,   En_RX_Data       ,   Inv_RX_Data     ,      NA       ,    21    ,    SCI   ,    0x4860    ,    NORTH    )  ,  //   GPIO_SUS9
  CHV_GPIO_PAD_CONF  (L"N30: TRST_B          ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    22    ,    NA    ,    0x4C00    ,    NORTH    )  ,  //   TRST_B
  CHV_GPIO_PAD_CONF  (L"N31: TCK             ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    23    ,    NA    ,    0x4C08    ,    NORTH    )  ,  //   XDP_TCK
  CHV_GPIO_PAD_CONF  (L"N32: PROCHOT_B       ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    24    ,    NA    ,    0x4C10    ,    NORTH    )  ,  //   VR_HOT_N
  CHV_GPIO_PAD_CONF  (L"N33: SVID0_DATA      ",    Native  ,    M0   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    25    ,    NA    ,    0x4C18    ,    NORTH    )  ,  //   SVID_DATA
  CHV_GPIO_PAD_CONF  (L"N34: TMS             ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    26    ,    NA    ,    0x4C20    ,    NORTH    )  ,  //   XDP_TMS
  CHV_GPIO_PAD_CONF  (L"N35: CX_PRDY_B2      ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    27    ,    NA    ,    0x4C28    ,    NORTH    )  ,  //   PRDY_B2
  CHV_GPIO_PAD_CONF  (L"N36: TDO_2           ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    28    ,    NA    ,    0x4C30    ,    NORTH    )  ,  //
  CHV_GPIO_PAD_CONF  (L"N37: CX_PRDY_B       ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    29    ,    NA    ,    0x4C38    ,    NORTH    )  ,  //   CX_PRDY_B
  CHV_GPIO_PAD_CONF  (L"N38: SVID0_ALERT_B   ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   Inv_RX_Data     ,      NA       ,    30    ,    NA    ,    0x4C40    ,    NORTH    )  ,  //   SVID0_ALERT_B
  CHV_GPIO_PAD_CONF  (L"N39: TDO             ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    31    ,    NA    ,    0x4C48    ,    NORTH    )  ,  //   XDP_TDO
  CHV_GPIO_PAD_CONF  (L"N40: SVID0_CLK       ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    32    ,    NA    ,    0x4C50    ,    NORTH    )  ,  //   SVD_CLK
  CHV_GPIO_PAD_CONF  (L"N41: TDI             ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    33    ,    NA    ,    0x4C58    ,    NORTH    )  ,  //   XDP_TDI
  CHV_GPIO_PAD_CONF  (L"N45: GP_CAMERASB05   ",    GPIO    ,    M1   ,       GPO   ,         LOW   ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    34    ,    NA    ,    0x5000    ,    NORTH    )  ,  //   FLASH_TORCH
  CHV_GPIO_PAD_CONF  (L"N46: GP_CAMERASB02   ",    GPIO    ,    M1   ,       GPO   ,         LOW   ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    35    ,    NA    ,    0x5008    ,    NORTH    )  ,  //   GPIO_FLHD_N
  CHV_GPIO_PAD_CONF  (L"N47: GP_CAMERASB08   ",    GPIO    ,    M1   ,       GPO   ,         LOW   ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    36    ,    NA    ,    0x5010    ,    NORTH    )  ,  //   XDP_GP_CAMERASB08
  CHV_GPIO_PAD_CONF  (L"N48: GP_CAMERASB00   ",    GPIO    ,    M1   ,       GPO   ,         LOW   ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    37    ,    NA    ,    0x5018    ,    NORTH    )  ,  //   CAM_ACT_LED
  CHV_GPIO_PAD_CONF  (L"N49: GP_CAMERASB06   ",    GPIO    ,    M1   ,       GPO   ,         LOW   ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    38    ,    NA    ,    0x5020    ,    NORTH    )  ,  //   CAM1_PWRDWN
  CHV_GPIO_PAD_CONF  (L"N50: GP_CAMERASB10   ",    GPIO    ,    M1   ,       GPO   ,         LOW   ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    39    ,    NA    ,    0x5028    ,    NORTH    )  ,  //   CAM_2_RST_N
  CHV_GPIO_PAD_CONF  (L"N51: GP_CAMERASB03   ",    GPIO    ,    M1   ,       GPO   ,         LOW   ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    40    ,    NA    ,    0x5030    ,    NORTH    )  ,  //   XDP_GP_CAMERASB03
  CHV_GPIO_PAD_CONF  (L"N52: GP_CAMERASB09   ",    GPIO    ,    M1   ,       GPO   ,         LOW   ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    41    ,    NA    ,    0x5038    ,    NORTH    )  ,  //   CAM_1_RST_N
  CHV_GPIO_PAD_CONF  (L"N53: GP_CAMERASB01   ",    GPIO    ,    M1   ,       GPO   ,         LOW   ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    42    ,    NA    ,    0x5040    ,    NORTH    )  ,  //   FLASH_RESET_N
  CHV_GPIO_PAD_CONF  (L"N54: GP_CAMERASB07   ",    GPIO    ,    M1   ,       GPO   ,         LOW   ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    43    ,    NA    ,    0x5048    ,    NORTH    )  ,  //   CAM2_PWRDWN
  CHV_GPIO_PAD_CONF  (L"N55: GP_CAMERASB11   ",    GPIO    ,    M1   ,       GPO   ,         LOW   ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    44    ,    NA    ,    0x5050    ,    NORTH    )  ,  //   CAM_3_RST_N
  CHV_GPIO_PAD_CONF  (L"N56: GP_CAMERASB04   ",    GPIO    ,    M1   ,       GPO   ,         LOW   ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    45    ,    NA    ,    0x5058    ,    NORTH    )  ,  //   FLASH_TRIG
  CHV_GPIO_PAD_CONF  (L"N60: PANEL0_BKLTEN   ",    GPIO    ,    M1   ,       GPI   ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    46    ,    NA    ,    0x5400    ,    NORTH    )  ,  //   BRD_ID_BIT_1
  CHV_GPIO_PAD_CONF  (L"N61: HV_DDI0_HPD     ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,  Inv_TX_Enable    ,      NA       ,    47    ,    NA    ,    0x5408    ,    NORTH    )  ,  //   SOC_HDMI_HPD
  CHV_GPIO_PAD_CONF  (L"N62: HV_DDI2_DDC_SDA ",    Native  ,    M3   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    48    ,    NA    ,    0x5410    ,    NORTH    )  ,  //   UART0_RXD (Default) || DDI1_DDC_SCA
  CHV_GPIO_PAD_CONF  (L"N63: PANEL1_BKLTCTL  ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    49    ,    NA    ,    0x5418    ,    NORTH    )  ,  //
  CHV_GPIO_PAD_CONF  (L"N64: HV_DDI1_HPD     ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,  Inv_TX_Enable    ,      NA       ,    50    ,    NA    ,    0x5420    ,    NORTH    )  ,  //   SOC_DP1_HPD
  CHV_GPIO_PAD_CONF  (L"N65: PANEL0_BKLTCTL  ",    GPIO    ,    M1   ,       GPI   ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    51    ,    NA    ,    0x5428    ,    NORTH    )  ,  //   BRD_ID_BIT_0
  CHV_GPIO_PAD_CONF  (L"N66: HV_DDI0_DDC_SDA ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    52    ,    NA    ,    0x5430    ,    NORTH    )  ,  //   HDMI_SMB_SOC_SDA
  CHV_GPIO_PAD_CONF  (L"N67: HV_DDI2_DDC_SCL ",    Native  ,    M3   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    53    ,    NA    ,    0x5438    ,    NORTH    )  ,  //   UART0_TXD (Default) || DDI1_DDC_SCL
  CHV_GPIO_PAD_CONF  (L"N68: HV_DDI2_HPD     ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,  Inv_TX_Enable    ,      NA       ,    54    ,    NA    ,    0x5440    ,    NORTH    )  ,  //   SOC_DP2_HPD
  CHV_GPIO_PAD_CONF  (L"N69: PANEL1_VDDEN    ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    55    ,    NA    ,    0x5448    ,    NORTH    )  ,  //
  CHV_GPIO_PAD_CONF  (L"N70: PANEL1_BKLTEN   ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    56    ,    NA    ,    0x5450    ,    NORTH    )  ,  //
  CHV_GPIO_PAD_CONF  (L"N71: HV_DDI0_DDC_SCL ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    57    ,    NA    ,    0x5458    ,    NORTH    )  ,  //   HDMI_SMB_SOC_SCL
  CHV_GPIO_PAD_CONF  (L"N72: PANEL0_VDDEN    ",    GPIO    ,    M1   ,       GPI   ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA  ,         NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    58    ,    NA    ,    0x5460    ,    NORTH    )  ,  //   BRD_ID_BIT_2

  //
  // East Community
  //              Group Pin#: PAD Name           Mode Select    Mode#   GPIO Config   GPIO TX State      GPIO Light        INT Type         INT Select    Term H/L   Open Drain   Current Source    Mask/Un-Mask        Glitch Cfg           InvertRX_TX        WAKE Cap    Wake_Mask_Bit    GPE        MMIO Addr     Community             Remarks
  //
  CHV_GPIO_PAD_CONF  (L"E00: PMU_SLP_S3_B    ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    0     ,    NA    ,    0x4400    ,    EAST     )  ,  //   SLP_S3_3P3_N
  CHV_GPIO_PAD_CONF  (L"E01: PMU_BATLOW_B    ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    1     ,    NA    ,    0x4408    ,    EAST     )  ,  //   PMU_BATLOW_N
  CHV_GPIO_PAD_CONF  (L"E02: SUS_STAT_B      ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    2     ,    NA    ,    0x4410    ,    EAST     )  ,  //   SUS_STAT_N
  CHV_GPIO_PAD_CONF  (L"E03: PMU_SLP_S0IX_B  ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    3     ,    NA    ,    0x4418    ,    EAST     )  ,  //   SLP_S01X_3P3_N
  CHV_GPIO_PAD_CONF  (L"E04: PMU_AC_PRESENT  ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    4     ,    NA    ,    0x4420    ,    EAST     )  ,  //   PMU_AC_PRESENT
  CHV_GPIO_PAD_CONF  (L"E05: PMU_PLTRST_B    ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    5     ,    NA    ,    0x4428    ,    EAST     )  ,  //   PLTRST_3P3_N
  CHV_GPIO_PAD_CONF  (L"E06: PMU_SUSCLK      ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    6     ,    NA    ,    0x4430    ,    EAST     )  ,  //   SUSCLK_3P3
  CHV_GPIO_PAD_CONF  (L"E07: PMU_SLP_LAN_B   ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    7     ,    NA    ,    0x4438    ,    EAST     )  ,  //   Not Used, Available
  CHV_GPIO_PAD_CONF  (L"E08: PMU_PWRBTN_B    ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    8     ,    NA    ,    0x4440    ,    EAST     )  ,  //   SOC_PWRBTN_N special programming below
  CHV_GPIO_PAD_CONF  (L"E09: PMU_SLP_S4_B    ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    9     ,    NA    ,    0x4448    ,    EAST     )  ,  //   SLP_S4_3P3_N
  CHV_GPIO_PAD_CONF  (L"E10: PMU_WAKE_B      ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,  P_1K_H  ,        NA    ,       NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    10    ,    NA    ,    0x4450    ,    EAST     )  ,  //   PMU_3P3_WAKE_N
  CHV_GPIO_PAD_CONF  (L"E11: PMU_WAKE_LAN_B  ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    11    ,    NA    ,    0x4458    ,    EAST     )  ,  //   Not Used, Available
  CHV_GPIO_PAD_CONF  (L"E15: MF_ISH_GPIO_3   ",    GPIO    ,    M1   ,       GPI   ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    12    ,    NA    ,    0x4800    ,    EAST     )  ,  //   FAB ID bit 3
  CHV_GPIO_PAD_CONF  (L"E16: MF_ISH_GPIO_7   ",    GPIO    ,    M1   ,       GPO   ,         HIGH  ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    13    ,    NA    ,    0x4808    ,    EAST     )  ,  //   Not Used, Test points
  CHV_GPIO_PAD_CONF  (L"E17: MF_ISH_I2C1_SCL ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    14    ,    NA    ,    0x4810    ,    EAST     )  ,  //   Not Used, Test points
  CHV_GPIO_PAD_CONF  (L"E18: MF_ISH_GPIO_1   ",    GPIO    ,    M1   ,       GPI   ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    15    ,    NA    ,    0x4818    ,    EAST     )  ,  //   FAB ID bit 1
  CHV_GPIO_PAD_CONF  (L"E19: MF_ISH_GPIO_5   ",    GPIO    ,    M1   ,       GPO   ,         HIGH  ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    16    ,    NA    ,    0x4820    ,    EAST     )  ,  //   Not Used, Test points
  CHV_GPIO_PAD_CONF  (L"E20: MF_ISH_GPIO_9   ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    17    ,    NA    ,    0x4828    ,    EAST     )  ,  //   Not Used, Test points
  CHV_GPIO_PAD_CONF  (L"E21: MF_ISH_GPIO_0   ",    GPIO    ,    M1   ,       GPI   ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    18    ,    NA    ,    0x4830    ,    EAST     )  ,  //   FAB ID bit 0
  CHV_GPIO_PAD_CONF  (L"E22: MF_ISH_GPIO_4   ",    GPIO    ,    M1   ,       GPI   ,         NA    ,     NA         ,       NA          ,     Line0   ,    NA    ,        NA    ,       NA     ,    NonMaskable  ,        NA          ,   No_Inversion    ,      NA       ,    19    ,    NA    ,    0x4838    ,    EAST     )  ,  //   Not Used, Test points
  CHV_GPIO_PAD_CONF  (L"E23: MF_ISH_GPIO_8   ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    20    ,    NA    ,    0x4840    ,    EAST     )  ,  //   Not Used, Test points
  CHV_GPIO_PAD_CONF  (L"E24: MF_ISH_GPIO_2   ",    GPIO    ,    M1   ,       GPI   ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    21    ,    NA    ,    0x4848    ,    EAST     )  ,  //   FAB ID bit 2
  CHV_GPIO_PAD_CONF  (L"E25: MF_ISH_GPIO_6   ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    22    ,    NA    ,    0x4850    ,    EAST     )  ,  //   Not Used, Test points
  CHV_GPIO_PAD_CONF  (L"E26: MF_ISH_I2C1_SDA ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    23    ,    NA    ,    0x4858    ,    EAST     )  ,  //   Not Used, Test points
  //
  // South East Community
  //               Group Pin#: PAD Name          Mode Select    Mode#   GPIO Config   GPIO TX State      GPIO Light       INT Type         INT Select    Term H/L   Open Drain   Current Source    Mask/Un-Mask        Glitch Cfg           InvertRX_TX        WAKE Cap     Wake_Mask_Bit   GPE        MMIO Addr     Community             Remarks
  //
  CHV_GPIO_PAD_CONF  (L"SE00: MF_PLT_CLK0    ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    0     ,    NA    ,    0x4400    ,  SOUTHEAST  )  ,  //   CAM_1_MCLK Camera1_clock
  CHV_GPIO_PAD_CONF  (L"SE01: PWM1           ",    GPIO    ,    M1   ,       GPO   ,         HIGH  ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    1     ,    NA    ,    0x4408    ,  SOUTHEAST  )  ,  //   WIFI_PCIE_RST_N
  CHV_GPIO_PAD_CONF  (L"SE02: MF_PLT_CLK1    ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    1     ,    NA    ,    0x4410    ,  SOUTHEAST  )  ,  //   CAM_2_MCLK Camera2_clock
  CHV_GPIO_PAD_CONF  (L"SE03: MF_PLT_CLK4    ",    GPIO    ,    M1   ,       GPO   ,         LOW   ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    3     ,    NA    ,    0x4418    ,  SOUTHEAST  )  ,  //   Not Used
  CHV_GPIO_PAD_CONF  (L"SE04: MF_PLT_CLK3    ",    GPIO    ,    M1   ,       GPI   ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    4     ,    NA    ,    0x4420    ,  SOUTHEAST  )  ,  //   I2S_MCLK
  CHV_GPIO_PAD_CONF  (L"SE05: PWM0           ",    GPIO    ,    M1   ,       GPO   ,         LOW   ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    5     ,    NA    ,    0x4428    ,  SOUTHEAST  )  ,  //
  CHV_GPIO_PAD_CONF  (L"SE06: MF_PLT_CLK5    ",    GPIO    ,    M3   ,       GPO   ,         LOW   ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    6     ,    NA    ,    0x4430    ,  SOUTHEAST  )  ,  //   Not Used
  CHV_GPIO_PAD_CONF  (L"SE07: MF_PLT_CLK2    ",    GPIO    ,    M1   ,       GPI   ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    7     ,    NA    ,    0x4438    ,  SOUTHEAST  )  ,  //   Not Used
  CHV_GPIO_PAD_CONF  (L"SE15: SDMMC2_D3_CD_B ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    8     ,    NA    ,    0x4800    ,  SOUTHEAST  )  ,  //   NGFF_SDEMMC2_D3
  CHV_GPIO_PAD_CONF  (L"SE16: SDMMC1_CLK     ",    Native  ,    M1   ,       NA    ,         NA    ,     HIGH       ,       NA          ,     NA      , P_20K_L  ,        NA    ,       NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    9     ,    NA    ,    0x4808    ,  SOUTHEAST  )  ,  //   EMMC1_CLK
  CHV_GPIO_PAD_CONF  (L"SE17: SDMMC1_D0      ",    Native  ,    M1   ,       NA    ,         NA    ,     HIGH       ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    10    ,    NA    ,    0x4810    ,  SOUTHEAST  )  ,  //   EMMC1_D_0
  CHV_GPIO_PAD_CONF  (L"SE18: SDMMC2_D1      ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    11    ,    NA    ,    0x4818    ,  SOUTHEAST  )  ,  //   NGFF_SDEMMC2_D1
  CHV_GPIO_PAD_CONF  (L"SE19: SDMMC2_CLK     ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    12    ,    NA    ,    0x4820    ,  SOUTHEAST  )  ,  //   NGFF_SDEMMC2_CLK
  CHV_GPIO_PAD_CONF  (L"SE20: SDMMC1_D2      ",    Native  ,    M1   ,       NA    ,         NA    ,     HIGH       ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    13    ,    NA    ,    0x4828    ,  SOUTHEAST  )  ,  //   EMMC1_D_2
  CHV_GPIO_PAD_CONF  (L"SE21: SDMMC2_D2      ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    14    ,    NA    ,    0x4830    ,  SOUTHEAST  )  ,  //   NGFF_SDEMMC2_D2
  CHV_GPIO_PAD_CONF  (L"SE22: SDMMC2_CMD     ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    15    ,    NA    ,    0x4838    ,  SOUTHEAST  )  ,  //   NGFF_SDEMMC2_CM
  CHV_GPIO_PAD_CONF  (L"SE23: SDMMC1_CMD     ",    Native  ,    M1   ,       NA    ,         NA    ,     HIGH       ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    16    ,    NA    ,    0x4840    ,  SOUTHEAST  )  ,  //   EMMC1_CMD
  CHV_GPIO_PAD_CONF  (L"SE24: SDMMC1_D1      ",    Native  ,    M1   ,       NA    ,         NA    ,     HIGH       ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    17    ,    NA    ,    0x4848    ,  SOUTHEAST  )  ,  //   EMMC1_D_1
  CHV_GPIO_PAD_CONF  (L"SE25: SDMMC2_D0      ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    18    ,    NA    ,    0x4850    ,  SOUTHEAST  )  ,  //   NGFF_SDEMMC2_D0
  CHV_GPIO_PAD_CONF  (L"SE26: SDMMC1_D3_CD_B ",    Native  ,    M1   ,       NA    ,         NA    ,     HIGH       ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    19    ,    NA    ,    0x4858    ,  SOUTHEAST  )  ,  //   EMMC1_D_3
  CHV_GPIO_PAD_CONF  (L"SE30: SDMMC3_D1      ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    20    ,    NA    ,    0x4C00    ,  SOUTHEAST  )  ,  //   SD_CARD_D1
  CHV_GPIO_PAD_CONF  (L"SE31: SDMMC3_CLK     ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    21    ,    NA    ,    0x4C08    ,  SOUTHEAST  )  ,  //   SD_CARD_CLK
  CHV_GPIO_PAD_CONF  (L"SE32: SDMMC3_D3      ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    22    ,    NA    ,    0x4C10    ,  SOUTHEAST  )  ,  //   SD_CARD_D3
  CHV_GPIO_PAD_CONF  (L"SE33: SDMMC3_D2      ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    23    ,    NA    ,    0x4C18    ,  SOUTHEAST  )  ,  //   SD_CARD_D2
  CHV_GPIO_PAD_CONF  (L"SE34: SDMMC3_CMD     ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    24    ,    NA    ,    0x4C20    ,  SOUTHEAST  )  ,  //   SD_CARD_CMD
  CHV_GPIO_PAD_CONF  (L"SE35: SDMMC3_D0      ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    25    ,    NA    ,    0x4C28    ,  SOUTHEAST  )  ,  //   SD_CARD_D0
  CHV_GPIO_PAD_CONF  (L"SE45: MF_LPC_AD2     ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    26    ,    NA    ,    0x5000    ,  SOUTHEAST  )  ,  //   LPC_AD2
  CHV_GPIO_PAD_CONF  (L"SE46: LPC_CLKRUNB    ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    27    ,    NA    ,    0x5008    ,  SOUTHEAST  )  ,  //   L_CLKRUN_N
  CHV_GPIO_PAD_CONF  (L"SE47: MF_LPC_AD0     ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    28    ,    NA    ,    0x5010    ,  SOUTHEAST  )  ,  //   LPC_AD0
  CHV_GPIO_PAD_CONF  (L"SE48: LPC_FRAMEB     ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    29    ,    NA    ,    0x5018    ,  SOUTHEAST  )  ,  //   L_FRAME_N
  CHV_GPIO_PAD_CONF  (L"SE49: MF_LPC_CLKOUT1 ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    30    ,    NA    ,    0x5020    ,  SOUTHEAST  )  ,  //   L_CLKOUT1
  CHV_GPIO_PAD_CONF  (L"SE50: MF_LPC_AD3     ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    31    ,    NA    ,    0x5028    ,  SOUTHEAST  )  ,  //   LPC_AD3
  CHV_GPIO_PAD_CONF  (L"SE51: MF_LPC_CLKOUT0 ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      , P_NONE   ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    32    ,    NA    ,    0x5030    ,  SOUTHEAST  )  ,  //   L_CLKOUT0
  CHV_GPIO_PAD_CONF  (L"SE52: MF_LPC_AD1     ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    33    ,    NA    ,    0x5038    ,  SOUTHEAST  )  ,  //   LPC_AD1
  CHV_GPIO_PAD_CONF  (L"SE60: SPI1_MISO      ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    34    ,    NA    ,    0x5400    ,  SOUTHEAST  )  ,  //   SPI1_MISO
  CHV_GPIO_PAD_CONF  (L"SE61: SPI1_CS0_B     ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    35    ,    NA    ,    0x5408    ,  SOUTHEAST  )  ,  //   SPI1_CS0_N
  CHV_GPIO_PAD_CONF  (L"SE62: SPI1_CLK       ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    36    ,    NA    ,    0x5410    ,  SOUTHEAST  )  ,  //   SPI1_CLK
  CHV_GPIO_PAD_CONF  (L"SE63: MMC1_D6        ",    Native  ,    M1   ,       NA    ,         NA    ,     HIGH       ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    37    ,    NA    ,    0x5418    ,  SOUTHEAST  )  ,  //   EMMC1_D6
  CHV_GPIO_PAD_CONF  (L"SE64: SPI1_MOSI      ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    38    ,    NA    ,    0x5420    ,  SOUTHEAST  )  ,  //   SPI1_MOSI
  CHV_GPIO_PAD_CONF  (L"SE65: MMC1_D5        ",    Native  ,    M1   ,       NA    ,         NA    ,     HIGH       ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    39    ,    NA    ,    0x5428    ,  SOUTHEAST  )  ,  //   EMMC1_D5
  CHV_GPIO_PAD_CONF  (L"SE66: SPI1_CS1_B     ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    40    ,    NA    ,    0x5430    ,  SOUTHEAST  )  ,  //   SPI1_CS1_N
  CHV_GPIO_PAD_CONF  (L"SE67: MMC1_D4_SD_WE  ",    Native  ,    M1   ,       NA    ,         NA    ,     HIGH       ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    41    ,    NA    ,    0x5438    ,  SOUTHEAST  )  ,  //   EMMC1_D4
  CHV_GPIO_PAD_CONF  (L"SE68: MMC1_D7        ",    Native  ,    M1   ,       NA    ,         NA    ,     HIGH       ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    42    ,    NA    ,    0x5440    ,  SOUTHEAST  )  ,  //   EMMC1_D7
  CHV_GPIO_PAD_CONF  (L"SE69: MMC1_RCLK      ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,   No_Inversion    ,      NA       ,    43    ,    NA    ,    0x5448    ,  SOUTHEAST  )  ,  //   EMMC1_RCLK
  CHV_GPIO_PAD_CONF  (L"SE75: USB_OC1_B      ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    44    ,    NA    ,    0x5800    ,  SOUTHEAST  )  ,  //   USB3_OC1_N
  CHV_GPIO_PAD_CONF  (L"SE76: PMU_RSTBUTTON_B",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    45    ,    NA    ,    0x5808    ,  SOUTHEAST  )  ,  //   SOC_RESETBTN_N special programming below
  CHV_GPIO_PAD_CONF  (L"SE77: GPIO_ALERT     ",    GPIO    ,    M1   ,       GPI   ,         NA    ,     NA         ,    Trig_Level     ,     Line2   ,    NA    ,        NA    ,       NA     ,    NonMaskable  ,    En_RX_Data      ,    Inv_RX_Data    ,      NA       ,    46    ,    NA    ,    0x5810    ,  SOUTHEAST  )  ,  //   DCN : 2502579 - Programmed for TCH_PAD_INT_N
  CHV_GPIO_PAD_CONF  (L"SE78: SDMMC3_PWR_EN_B",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      , P_20K_L  ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    47    ,    NA    ,    0x5818    ,  SOUTHEAST  )  ,  //   SD_CARD_PWRDN_N
  CHV_GPIO_PAD_CONF  (L"SE79: ILB_SERIRQ     ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    48    ,    NA    ,    0x5820    ,  SOUTHEAST  )  ,  //   ILB_SERIRQ
  CHV_GPIO_PAD_CONF  (L"SE80: USB_OC0_B      ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    49    ,    NA    ,    0x5828    ,  SOUTHEAST  )  ,  //   USB3_OC0_N
  CHV_GPIO_PAD_CONF  (L"SE81: SDMMC3_CD_B    ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    50    ,    NA    ,    0x5830    ,  SOUTHEAST  )  ,  //   SD_CARD_DET_N
  CHV_GPIO_PAD_CONF  (L"SE82: SPKR           ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    51    ,    NA    ,    0x5838    ,  SOUTHEAST  )  ,  //   SPKR
  CHV_GPIO_PAD_CONF  (L"SE83: SUSPWRDNACK    ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    52    ,    NA    ,    0x5840    ,  SOUTHEAST  )  ,  //   SUSPWRDNACK_3P3
  CHV_GPIO_PAD_CONF  (L"SE84: SPARE_PIN      ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    53    ,    NA    ,    0x5848    ,  SOUTHEAST  )  ,  //   SPARE_PIN
  CHV_GPIO_PAD_CONF  (L"SE85: SDMMC3_1P8_EN  ",    Native  ,    M1   ,       NA    ,         NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA     ,       NA        ,        NA          ,    No_Inversion   ,      NA       ,    54    ,    NA    ,    0x5850    ,  SOUTHEAST  )  ,  //   SD_CARD_PWR_EN
  //
  //  South west Community
  //               Group Pin#: PAD Name          Mode Select    Mode#   GPIO Config   GPIO TX State      GPIO Light       INT Type         INT Select    Term H/L     Open Drain   Current Source    Mask/Un-Mask        Glitch Cfg           InvertRX_TX        WAKE Cap     Wake_Mask_Bit   GPE        MMIO Addr     Community             Remarks
  //
  CHV_GPIO_PAD_CONF  (L"SW00: FST_SPI_D2     ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     0     ,     NA    ,   0x4400    ,  SOUTHWEST  )  ,  //   FST_SPI_D2
  CHV_GPIO_PAD_CONF  (L"SW01: FST_SPI_D0     ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     1     ,     NA    ,   0x4408    ,  SOUTHWEST  )  ,  //   FST_SPI_D0
  CHV_GPIO_PAD_CONF  (L"SW02: FST_SPI_CLK    ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     2     ,     NA    ,   0x4410    ,  SOUTHWEST  )  ,  //   FAST_SPI_CLK
  CHV_GPIO_PAD_CONF  (L"SW03: FST_SPI_D3     ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     3     ,     NA    ,   0x4418    ,  SOUTHWEST  )  ,  //   FST_SPI_D3
  CHV_GPIO_PAD_CONF  (L"SW04: FST_SPI_CS1_B  ",    GPIO    ,    M1   ,        GPO   ,        HIGH  ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     4     ,     NA    ,   0x4420    ,  SOUTHWEST  )  ,  //   V3P3DX_TCH_EN
  CHV_GPIO_PAD_CONF  (L"SW05: FST_SPI_D1     ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     5     ,     NA    ,   0x4428    ,  SOUTHWEST  )  ,  //   FST_SPI_D1
  CHV_GPIO_PAD_CONF  (L"SW06: FST_SPI_CS0_B  ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     6     ,     NA    ,   0x4430    ,  SOUTHWEST  )  ,  //   FST_SPI_CS_N
  CHV_GPIO_PAD_CONF  (L"SW07: FST_SPI_CS2_B  ",    GPIO    ,    M1   ,        GPO   ,        LOW   ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     7     ,     NA    ,   0x4438    ,  SOUTHWEST  )  ,  //   FST_SPI_CS2_N (SPI TPM)
  CHV_GPIO_PAD_CONF  (L"SW15: UART1_RTS_B    ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     8     ,     NA    ,   0x4800    ,  SOUTHWEST  )  ,  //   UART_BT_RTS
  CHV_GPIO_PAD_CONF  (L"SW16: UART1_RXD      ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     9     ,     NA    ,   0x4808    ,  SOUTHWEST  )  ,  //   UART_BT_RXD
  CHV_GPIO_PAD_CONF  (L"SW17: UART2_RXD      ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      ,  P_NONE  ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     10    ,     NA    ,   0x4810    ,  SOUTHWEST  )  ,  //   UART_GPS_RXD
  CHV_GPIO_PAD_CONF  (L"SW18: UART1_CTS_B    ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     11    ,     NA    ,   0x4818    ,  SOUTHWEST  )  ,  //   UART_BT_CTS
  CHV_GPIO_PAD_CONF  (L"SW19: UART2_RTS_B    ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     12    ,     NA    ,   0x4820    ,  SOUTHWEST  )  ,  //   UART_GPS_RTS
  CHV_GPIO_PAD_CONF  (L"SW20: UART1_TXD      ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     13    ,     NA    ,   0x4828    ,  SOUTHWEST  )  ,  //   UART_BT_TXD
  CHV_GPIO_PAD_CONF  (L"SW21: UART2_TXD      ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     14    ,     NA    ,   0x4830    ,  SOUTHWEST  )  ,  //   UART_GPS_TXD
  CHV_GPIO_PAD_CONF  (L"SW22: UART2_CTS_B    ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      ,  P_NONE  ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     15    ,     NA    ,   0x4838    ,  SOUTHWEST  )  ,  //   UART_GPS_CTS
  CHV_GPIO_PAD_CONF  (L"SW30: MF_HDA_CLK     ",    Native  ,    M2   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     16    ,     NA    ,   0x4C00    ,  SOUTHWEST  )  ,  //   MF_HDA_CLK II GP_SSP_0_I2S_TXD
  CHV_GPIO_PAD_CONF  (L"SW31: MF_HDA_RSTB    ",    Native  ,    M2   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     17    ,     NA    ,   0x4C08    ,  SOUTHWEST  )  ,  //   AUD_LINK_RST_N
  CHV_GPIO_PAD_CONF  (L"SW32: MF_HDA_SDI0    ",    Native  ,    M2   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     18    ,     NA    ,   0x4C10    ,  SOUTHWEST  )  ,  //   AUD_LINK_SDI0_SOC
  CHV_GPIO_PAD_CONF  (L"SW33: MF_HDA_SDO     ",    Native  ,    M2   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     19    ,     NA    ,   0x4C18    ,  SOUTHWEST  )  ,  //   AUD_LINK_SDO
  CHV_GPIO_PAD_CONF  (L"SW34: MF_HDA_DOCKRSTB",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     20    ,     NA    ,   0x4C20    ,  SOUTHWEST  )  ,  //   NGFF_I2S_1_TXD_R_BT || I2S_2_TXD_R_AICO
  CHV_GPIO_PAD_CONF  (L"SW35: MF_HDA_SYNC    ",    Native  ,    M2   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     21    ,     NA    ,   0x4C28    ,  SOUTHWEST  )  ,  //   AUD_LINK_SYNC
  CHV_GPIO_PAD_CONF  (L"SW36: MF_HDA_SDI1    ",    Native  ,    M2   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     22    ,     NA    ,   0x4C30    ,  SOUTHWEST  )  ,  //   AUD_LINK_SDI1_SOC
  CHV_GPIO_PAD_CONF  (L"SW37: MF_HDA_DOCKENB ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     23    ,     NA    ,   0x4C38    ,  SOUTHWEST  )  ,  //   NGFF_I2S_1_RXD_R_BT || I2S_2_RXD_R_AICO
  CHV_GPIO_PAD_CONF  (L"SW45: I2C5_SDA       ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA    ,        NA        ,        NA          ,   Inv_TX_Enable   ,      NA       ,     24   ,     NA    ,   0x5000    ,  SOUTHWEST  )  ,  //   I2C5_SDA_3P3
  CHV_GPIO_PAD_CONF  (L"SW46: I2C4_SDA       ",    Native  ,    M3   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      ,   NA     ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     25    ,     NA    ,   0x5008    ,  SOUTHWEST  )  ,  //    HV_DDI2_DDC_SDA
  CHV_GPIO_PAD_CONF  (L"SW47: I2C6_SDA       ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA    ,        NA        ,        NA          ,   Inv_TX_Enable   ,      NA       ,     26   ,     NA    ,   0x5010    ,  SOUTHWEST  )  ,  //
  CHV_GPIO_PAD_CONF  (L"SW48: I2C5_SCL       ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA    ,        NA        ,        NA          ,   Inv_TX_Enable   ,      NA       ,     27   ,     NA    ,   0x5018    ,  SOUTHWEST  )  ,  //   I2C5_SDA_3P3
  CHV_GPIO_PAD_CONF  (L"SW49: I2C_NFC_SDA    ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA    ,        NA        ,        NA          ,   Inv_TX_Enable   ,      NA       ,     28   ,     NA    ,   0x5020    ,  SOUTHWEST  )  ,  //   I2C_NFC_SDA_3P3
  CHV_GPIO_PAD_CONF  (L"SW50: I2C4_SCL       ",    Native  ,    M3   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      ,   NA     ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     29    ,     NA    ,   0x5028    ,  SOUTHWEST  )  ,  //    HV_DDI2_DDC_SCL
  CHV_GPIO_PAD_CONF  (L"SW51: I2C6_SCL       ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA    ,        NA        ,        NA          ,   Inv_TX_Enable   ,      NA       ,     30   ,     NA    ,   0x5030    ,  SOUTHWEST  )  ,  //
  CHV_GPIO_PAD_CONF  (L"SW52: I2C_NFC_SCL    ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA    ,        NA        ,        NA          ,   Inv_TX_Enable   ,      NA       ,     31   ,     NA    ,   0x5038    ,  SOUTHWEST  )  ,  //   I2C_NFC_SCL_3P3
  CHV_GPIO_PAD_CONF  (L"SW60: I2C1_SDA       ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA    ,        NA        ,        NA          ,   Inv_TX_Enable   ,      NA       ,     32    ,     NA    ,   0x5400    ,  SOUTHWEST  )  ,  //   I2C1_SCL_AUD_CONN
  CHV_GPIO_PAD_CONF  (L"SW61: I2C0_SDA       ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA    ,        NA        ,        NA          ,   Inv_TX_Enable   ,      NA       ,     33    ,     NA    ,   0x5408    ,  SOUTHWEST  )  ,  //   I2C_3P3_NGFF_SCL
  CHV_GPIO_PAD_CONF  (L"SW62: I2C2_SDA       ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA    ,        NA        ,        NA          ,   Inv_TX_Enable   ,      NA       ,     34    ,     NA    ,   0x5410    ,  SOUTHWEST  )  ,  //   I2C2_SCL_CAM
  CHV_GPIO_PAD_CONF  (L"SW63: I2C1_SCL       ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA    ,        NA        ,        NA          ,   Inv_TX_Enable   ,      NA       ,     35    ,     NA    ,   0x5418    ,  SOUTHWEST  )  ,  //   I2C1_SDA_AUD_CONN
  CHV_GPIO_PAD_CONF  (L"SW64: I2C3_SDA       ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA    ,        NA        ,        NA          ,   Inv_TX_Enable   ,      NA       ,     36    ,     NA    ,   0x5420    ,  SOUTHWEST  )  ,  //   I2C3_CAM_SCL
  CHV_GPIO_PAD_CONF  (L"SW65: I2C0_SCL       ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA    ,        NA        ,        NA          ,   Inv_TX_Enable   ,      NA       ,     37    ,     NA    ,   0x5428    ,  SOUTHWEST  )  ,  //   I2C_3P3_NGFF_SDA
  CHV_GPIO_PAD_CONF  (L"SW66: I2C2_SCL       ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA    ,        NA        ,        NA          ,   Inv_TX_Enable   ,      NA       ,     38    ,     NA    ,   0x5430    ,  SOUTHWEST  )  ,  //   I2C2_SDA_CAM
  CHV_GPIO_PAD_CONF  (L"SW67: I2C3_SCL       ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA    ,        NA        ,        NA          ,   Inv_TX_Enable   ,      NA       ,     39    ,     NA    ,   0x5438    ,  SOUTHWEST  )  ,  //   I2C3_CAM_SDA
  CHV_GPIO_PAD_CONF  (L"SW75: SATA_GP0       ",    GPIO    ,    M1   ,        GPO   ,        HIGH  ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     40   ,     NA    ,   0x5800    ,  SOUTHWEST  )  ,  //   reserved for SATA (Interlock switch)
  CHV_GPIO_PAD_CONF  (L"SW76: SATA_GP1       ",    GPIO    ,    M1   ,        GPI   ,        HIGH  ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     41   ,     NA    ,   0x5808    ,  SOUTHWEST  )  ,  //   GPI to select UART0 | DDI1 mode
  CHV_GPIO_PAD_CONF  (L"SW77: SATA_LEDN      ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      ,    NA    ,     _ENABLE  ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     42   ,     NA    ,   0x5810    ,  SOUTHWEST  )  ,  //   SATA_LED_N
  CHV_GPIO_PAD_CONF  (L"SW78: SATA_GP2       ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      ,    NA    ,     _ENABLE  ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     43   ,     NA    ,   0x5818    ,  SOUTHWEST  )  ,  //   SATA_DEVSLP0 -need to check
  CHV_GPIO_PAD_CONF  (L"SW79: MF_SMB_ALERTB  ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     44   ,     NA    ,   0x5820    ,  SOUTHWEST  )  ,  //   SMB_ALERTB_3P3
  CHV_GPIO_PAD_CONF  (L"SW80: SATA_GP3       ",    GPIO    ,    M2   ,        GPI   ,        LOW   ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     45   ,     NA    ,   0x5828    ,  SOUTHWEST  )  ,  //   TOUCH_RST_N  -need to check
  CHV_GPIO_PAD_CONF  (L"SW81: MF_SMB_CLK     ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     46   ,     NA    ,   0x5830    ,  SOUTHWEST  )  ,  //   SMB_3P3_CLK
  CHV_GPIO_PAD_CONF  (L"SW82: MF_SMB_DATA    ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      , P_20K_H  ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     47   ,     NA    ,   0x5838    ,  SOUTHWEST  )  ,  //   SMB_3P3_DAT
  CHV_GPIO_PAD_CONF  (L"SW90: PCIE_CLKREQ0B  ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     48   ,     NA    ,   0x5C00    ,  SOUTHWEST  )  ,  //
  CHV_GPIO_PAD_CONF  (L"SW91: PCIE_CLKREQ1B  ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     49   ,     NA    ,   0x5C08    ,  SOUTHWEST  )  ,  //   PCIE_CLKREQ1_N
  CHV_GPIO_PAD_CONF  (L"SW92: GP_SSP_2_CLK   ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      , P_20K_L  ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     50   ,     NA    ,   0x5C10    ,  SOUTHWEST  )  ,  //   PCIE_CLKREQ1_N
  CHV_GPIO_PAD_CONF  (L"SW93: PCIE_CLKREQ2B  ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     51   ,     NA    ,   0x5C18    ,  SOUTHWEST  )  ,  //   PCIE_CLKREQ2_N
  CHV_GPIO_PAD_CONF  (L"SW94: GP_SSP_2_RXD   ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      , P_20K_L  ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     52   ,     NA    ,   0x5C20    ,  SOUTHWEST  )  ,  //
  CHV_GPIO_PAD_CONF  (L"SW95: PCIE_CLKREQ3B  ",    Native  ,    M2   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      ,    NA    ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     53   ,     NA    ,   0x5C28    ,  SOUTHWEST  )  ,  //   SDMMC3_WP (Def)
  CHV_GPIO_PAD_CONF  (L"SW96: GP_SSP_2_FS    ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      , P_20K_L  ,        NA    ,       NA    ,        NA        ,        NA          ,   No_Inversion    ,      NA       ,     54   ,     NA    ,   0x5C30    ,  SOUTHWEST  )  ,  //   I2S_2_FS
  CHV_GPIO_PAD_CONF  (L"SW97: GP_SSP_2_TXD   ",    Native  ,    M1   ,        NA    ,        NA    ,     NA         ,       NA          ,     NA      , P_20K_L  ,        NA    ,       NA    ,        NA        ,        NA          ,   Inv_TX_Enable   ,      NA       ,     55   ,     NA    ,   0x5C38    ,  SOUTHWEST  )  ,  //   I2S_2_TXD
};

CHAR16* GPIOPadNames[GPIO_TABLE_ROW_NUM] = {
  //
  // North Community
  //
  L"N00: GPIO_DFX0       ",
  L"N01: GPIO_DFX3       ",
  L"N02: GPIO_DFX7       ",
  L"N03: GPIO_DFX1       ",
  L"N04: GPIO_DFX5       ",
  L"N05: GPIO_DFX4       ",
  L"N06: GPIO_DFX8       ",
  L"N07: GPIO_DFX2       ",
  L"N08: GPIO_DFX6       ",
  L"N15: GPIO_SUS0       ",
  L"N16: SEC_GPIO_SUS10  ",
  L"N17: GPIO_SUS3       ",
  L"N18: GPIO_SUS7       ",
  L"N19: GPIO_SUS1       ",
  L"N20: GPIO_SUS5       ",
  L"N21: SEC_GPIO_SUS11  ",
  L"N22: GPIO_SUS4       ",
  L"N23: SEC_GPIO_SUS8   ",
  L"N24: GPIO_SUS2       ",
  L"N25: GPIO_SUS6       ",
  L"N26: CX_PREQ_B       ",
  L"N27: GPIO_SUS9       ",
  L"N30: TRST_B          ",
  L"N31: TCK             ",
  L"N32: PROCHOT_B       ",
  L"N33: SVID0_DATA      ",
  L"N34: TMS             ",
  L"N35: CX_PRDY_B2      ",
  L"N36: TDO_2           ",
  L"N37: CX_PRDY_B       ",
  L"N38: SVID0_ALERT_B   ",
  L"N39: TDO             ",
  L"N40: SVID0_CLK       ",
  L"N41: TDI             ",
  L"N45: GP_CAMERASB05   ",
  L"N46: GP_CAMERASB02   ",
  L"N47: GP_CAMERASB08   ",
  L"N48: GP_CAMERASB00   ",
  L"N49: GP_CAMERASB06   ",
  L"N50: GP_CAMERASB10   ",
  L"N51: GP_CAMERASB03   ",
  L"N52: GP_CAMERASB09   ",
  L"N53: GP_CAMERASB01   ",
  L"N54: GP_CAMERASB07   ",
  L"N55: GP_CAMERASB11   ",
  L"N56: GP_CAMERASB04   ",
  L"N60: PANEL0_BKLTEN   ",
  L"N61: HV_DDI0_HPD     ",
  L"N62: HV_DDI2_DDC_SDA ",
  L"N63: PANEL1_BKLTCTL  ",
  L"N64: HV_DDI1_HPD     ",
  L"N65: PANEL0_BKLTCTL  ",
  L"N66: HV_DDI0_DDC_SDA ",
  L"N67: HV_DDI2_DDC_SCL ",
  L"N68: HV_DDI2_HPD     ",
  L"N69: PANEL1_VDDEN    ",
  L"N70: PANEL1_BKLTEN   ",
  L"N71: HV_DDI0_DDC_SCL ",
  L"N72: PANEL0_VDDEN    ",
  //
  // East Community
  //
  L"E00: PMU_SLP_S3_B    ",
  L"E01: PMU_BATLOW_B    ",
  L"E02: SUS_STAT_B      ",
  L"E03: PMU_SLP_S0IX_B  ",
  L"E04: PMU_AC_PRESENT  ",
  L"E05: PMU_PLTRST_B    ",
  L"E06: PMU_SUSCLK      ",
  L"E07: PMU_SLP_LAN_B   ",
  L"E08: PMU_PWRBTN_B    ",
  L"E09: PMU_SLP_S4_B    ",
  L"E10: PMU_WAKE_B      ",
  L"E11: PMU_WAKE_LAN_B  ",
  L"E15: MF_ISH_GPIO_3   ",
  L"E16: MF_ISH_GPIO_7   ",
  L"E17: MF_ISH_I2C1_SCL ",
  L"E18: MF_ISH_GPIO_1   ",
  L"E19: MF_ISH_GPIO_5   ",
  L"E20: MF_ISH_GPIO_9   ",
  L"E21: MF_ISH_GPIO_0   ",
  L"E22: MF_ISH_GPIO_4   ",
  L"E23: MF_ISH_GPIO_8   ",
  L"E24: MF_ISH_GPIO_2   ",
  L"E25: MF_ISH_GPIO_6   ",
  L"E26: MF_ISH_I2C1_SDA ",
  //
  // South East Community
  //
  L"SE00: MF_PLT_CLK0    ",
  L"SE01: PWM1           ",
  L"SE02: MF_PLT_CLK1    ",
  L"SE03: MF_PLT_CLK4    ",
  L"SE04: MF_PLT_CLK3    ",
  L"SE05: PWM0           ",
  L"SE06: MF_PLT_CLK5    ",
  L"SE07: MF_PLT_CLK2    ",
  L"SE15: SDMMC2_D3_CD_B ",
  L"SE16: SDMMC1_CLK     ",
  L"SE17: SDMMC1_D0      ",
  L"SE18: SDMMC2_D1      ",
  L"SE19: SDMMC2_CLK     ",
  L"SE20: SDMMC1_D2      ",
  L"SE21: SDMMC2_D2      ",
  L"SE22: SDMMC2_CMD     ",
  L"SE23: SDMMC1_CMD     ",
  L"SE24: SDMMC1_D1      ",
  L"SE25: SDMMC2_D0      ",
  L"SE26: SDMMC1_D3_CD_B ",
  L"SE30: SDMMC3_D1      ",
  L"SE31: SDMMC3_CLK     ",
  L"SE32: SDMMC3_D3      ",
  L"SE33: SDMMC3_D2      ",
  L"SE34: SDMMC3_CMD     ",
  L"SE35: SDMMC3_D0      ",
  L"SE45: MF_LPC_AD2     ",
  L"SE46: LPC_CLKRUNB    ",
  L"SE47: MF_LPC_AD0     ",
  L"SE48: LPC_FRAMEB     ",
  L"SE49: MF_LPC_CLKOUT1 ",
  L"SE50: MF_LPC_AD3     ",
  L"SE51: MF_LPC_CLKOUT0 ",
  L"SE52: MF_LPC_AD1     ",
  L"SE60: SPI1_MISO      ",
  L"SE61: SPI1_CS0_B     ",
  L"SE62: SPI1_CLK       ",
  L"SE63: MMC1_D6        ",
  L"SE64: SPI1_MOSI      ",
  L"SE65: MMC1_D5        ",
  L"SE66: SPI1_CS1_B     ",
  L"SE67: MMC1_D4_SD_WE  ",
  L"SE68: MMC1_D7        ",
  L"SE69: MMC1_RCLK      ",
  L"SE75: USB_OC1_B      ",
  L"SE76: PMU_RSTBUTTON_B",
  L"SE77: GPIO_ALERT     ",
  L"SE78: SDMMC3_PWR_EN_B",
  L"SE79: ILB_SERIRQ     ",
  L"SE80: USB_OC0_B      ",
  L"SE81: SDMMC3_CD_B    ",
  L"SE82: SPKR           ",
  L"SE83: SUSPWRDNACK    ",
  L"SE84: SPARE_PIN      ",
  L"SE85: SDMMC3_1P8_EN  ",
  //
  //  South west Community
  //
  L"SW00: FST_SPI_D2     ",
  L"SW01: FST_SPI_D0     ",
  L"SW02: FST_SPI_CLK    ",
  L"SW03: FST_SPI_D3     ",
  L"SW04: FST_SPI_CS1_B  ",
  L"SW05: FST_SPI_D1     ",
  L"SW06: FST_SPI_CS0_B  ",
  L"SW07: FST_SPI_CS2_B  ",
  L"SW15: UART1_RTS_B    ",
  L"SW16: UART1_RXD      ",
  L"SW17: UART2_RXD      ",
  L"SW18: UART1_CTS_B    ",
  L"SW19: UART2_RTS_B    ",
  L"SW20: UART1_TXD      ",
  L"SW21: UART2_TXD      ",
  L"SW22: UART2_CTS_B    ",
  L"SW30: MF_HDA_CLK     ",
  L"SW31: MF_HDA_RSTB    ",
  L"SW32: MF_HDA_SDI0    ",
  L"SW33: MF_HDA_SDO     ",
  L"SW34: MF_HDA_DOCKRSTB",
  L"SW35: MF_HDA_SYNC    ",
  L"SW36: MF_HDA_SDI1    ",
  L"SW37: MF_HDA_DOCKENB ",
  L"SW45: I2C5_SDA       ",
  L"SW46: I2C4_SDA       ",
  L"SW47: I2C6_SDA       ",
  L"SW48: I2C5_SCL       ",
  L"SW49: I2C_NFC_SDA    ",
  L"SW50: I2C4_SCL       ",
  L"SW51: I2C6_SCL       ",
  L"SW52: I2C_NFC_SCL    ",
  L"SW60: I2C1_SDA       ",
  L"SW61: I2C0_SDA       ",
  L"SW62: I2C2_SDA       ",
  L"SW63: I2C1_SCL       ",
  L"SW64: I2C3_SDA       ",
  L"SW65: I2C0_SCL       ",
  L"SW66: I2C2_SCL       ",
  L"SW67: I2C3_SCL       ",
  L"SW75: SATA_GP0       ",
  L"SW76: SATA_GP1       ",
  L"SW77: SATA_LEDN      ",
  L"SW78: SATA_GP2       ",
  L"SW79: MF_SMB_ALERTB  ",
  L"SW80: SATA_GP3       ",
  L"SW81: MF_SMB_CLK     ",
  L"SW82: MF_SMB_DATA    ",
  L"SW90: PCIE_CLKREQ0B  ",
  L"SW91: PCIE_CLKREQ1B  ",
  L"SW92: GP_SSP_2_CLK   ",
  L"SW93: PCIE_CLKREQ2B  ",
  L"SW94: GP_SSP_2_RXD   ",
  L"SW95: PCIE_CLKREQ3B  ",
  L"SW96: GP_SSP_2_FS    ",
  L"SW97: GP_SSP_2_TXD   "
};

UINT16 GPIOMmioOffset[GPIO_TABLE_ROW_NUM] = {
  //
  // North Community
  //
  0x4400    ,
  0x4408    ,
  0x4410    ,
  0x4418    ,
  0x4420    ,
  0x4428    ,
  0x4430    ,
  0x4438    ,
  0x4440    ,
  0x4800    ,
  0x4808    ,
  0x4810    ,
  0x4818    ,
  0x4820    ,
  0x4828    ,
  0x4830    ,
  0x4838    ,
  0x4840    ,
  0x4848    ,
  0x4850    ,
  0x4858    ,
  0x4860    ,
  0x4C00    ,
  0x4C08    ,
  0x4C10    ,
  0x4C18    ,
  0x4C20    ,
  0x4C28    ,
  0x4C30    ,
  0x4C38    ,
  0x4C40    ,
  0x4C48    ,
  0x4C50    ,
  0x4C58    ,
  0x5000    ,
  0x5008    ,
  0x5010    ,
  0x5018    ,
  0x5020    ,
  0x5028    ,
  0x5030    ,
  0x5038    ,
  0x5040    ,
  0x5048    ,
  0x5050    ,
  0x5058    ,
  0x5400    ,
  0x5408    ,
  0x5410    ,
  0x5418    ,
  0x5420    ,
  0x5428    ,
  0x5430    ,
  0x5438    ,
  0x5440    ,
  0x5448    ,
  0x5450    ,
  0x5458    ,
  0x5460    ,
  //
  // East Community
  //
  0x4400    ,
  0x4408    ,
  0x4410    ,
  0x4418    ,
  0x4420    ,
  0x4428    ,
  0x4430    ,
  0x4438    ,
  0x4440    ,
  0x4448    ,
  0x4450    ,
  0x4458    ,
  0x4800    ,
  0x4808    ,
  0x4810    ,
  0x4818    ,
  0x4820    ,
  0x4828    ,
  0x4830    ,
  0x4838    ,
  0x4840    ,
  0x4848    ,
  0x4850    ,
  0x4858    ,
  //
  // South East Community
  //
  0x4400    ,
  0x4408    ,
  0x4410    ,
  0x4418    ,
  0x4420    ,
  0x4428    ,
  0x4430    ,
  0x4438    ,
  0x4800    ,
  0x4808    ,
  0x4810    ,
  0x4818    ,
  0x4820    ,
  0x4828    ,
  0x4830    ,
  0x4838    ,
  0x4840    ,
  0x4848    ,
  0x4850    ,
  0x4858    ,
  0x4C00    ,
  0x4C08    ,
  0x4C10    ,
  0x4C18    ,
  0x4C20    ,
  0x4C28    ,
  0x5000    ,
  0x5008    ,
  0x5010    ,
  0x5018    ,
  0x5020    ,
  0x5028    ,
  0x5030    ,
  0x5038    ,
  0x5400    ,
  0x5408    ,
  0x5410    ,
  0x5418    ,
  0x5420    ,
  0x5428    ,
  0x5430    ,
  0x5438    ,
  0x5440    ,
  0x5448    ,
  0x5800    ,
  0x5808    ,
  0x5810    ,
  0x5818    ,
  0x5820    ,
  0x5828    ,
  0x5830    ,
  0x5838    ,
  0x5840    ,
  0x5848    ,
  0x5850    ,
  //
  //  South west Community
  //
  0x4400    ,
  0x4408    ,
  0x4410    ,
  0x4418    ,
  0x4420    ,
  0x4428    ,
  0x4430    ,
  0x4438    ,
  0x4800    ,
  0x4808    ,
  0x4810    ,
  0x4818    ,
  0x4820    ,
  0x4828    ,
  0x4830    ,
  0x4838    ,
  0x4C00    ,
  0x4C08    ,
  0x4C10    ,
  0x4C18    ,
  0x4C20    ,
  0x4C28    ,
  0x4C30    ,
  0x4C38    ,
  0x5000    ,
  0x5008    ,
  0x5010    ,
  0x5018    ,
  0x5020    ,
  0x5028    ,
  0x5030    ,
  0x5038    ,
  0x5400    ,
  0x5408    ,
  0x5410    ,
  0x5418    ,
  0x5420    ,
  0x5428    ,
  0x5430    ,
  0x5438    ,
  0x5800    ,
  0x5808    ,
  0x5810    ,
  0x5818    ,
  0x5820    ,
  0x5828    ,
  0x5830    ,
  0x5838    ,
  0x5C00    ,
  0x5C08    ,
  0x5C10    ,
  0x5C18    ,
  0x5C20    ,
  0x5C28    ,
  0x5C30    ,
  0x5C38
};

UINT8 WakeMaskBit[GPIO_TABLE_ROW_NUM] = {
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
  13,
  14,
  15,
  16,
  17,
  18,
  19,
  20,
  21,
  22,
  23,
  24,
  25,
  26,
  27,
  28,
  29,
  30,
  31,
  32,
  33,
  34,
  35,
  36,
  37,
  38,
  39,
  40,
  41,
  42,
  43,
  44,
  45,
  46,
  47,
  48,
  49,
  50,
  51,
  52,
  53,
  54,
  55,
  56,
  57,
  58,
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
  13,
  14,
  15,
  16,
  17,
  18,
  19,
  20,
  21,
  22,
  23,
  0,
  1,
  1,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
  13,
  14,
  15,
  16,
  17,
  18,
  19,
  20,
  21,
  22,
  23,
  24,
  25,
  26,
  27,
  28,
  29,
  30,
  31,
  32,
  33,
  34,
  35,
  36,
  37,
  38,
  39,
  40,
  41,
  42,
  43,
  44,
  45,
  46,
  47,
  48,
  49,
  50,
  51,
  52,
  53,
  54,
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
  13,
  14,
  15,
  16,
  17,
  18,
  19,
  20,
  21,
  22,
  23,
  24,
  25,
  26,
  27,
  28,
  29,
  30,
  31,
  32,
  33,
  34,
  35,
  36,
  37,
  38,
  39,
  40,
  41,
  42,
  43,
  44,
  45,
  46,
  47,
  48,
  49,
  50,
  51,
  52,
  53,
  54,
  55
};


EFI_STATUS
GetBoardFabIdsKsc (
  OUT UINT16                    *BoardFabIds
  );

EFI_STATUS
GetBoradFabIdsGpio (
  OUT UINT8 *BoardId,
  OUT UINT8 *FabId
  );

EFI_STATUS
EFIAPI
BraswellCherryHillBoardDetectionCallback (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

static EFI_PEI_NOTIFY_DESCRIPTOR mBraswellCherryHillDetectionStartNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gBoardDetectionStartPpiGuid,
  BraswellCherryHillBoardDetectionCallback
};

static EFI_PEI_PPI_DESCRIPTOR mBraswellCherryHillDetectedPpi = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gBoardDetectedPpiGuid,
  NULL
};

VOID
ProgramGpiosWithPcd (
  IN UINT32                      Gpio_Mmio_Offset,
  IN UINT32                      Gpio_Pin_Num,
  IN PLATFORM_GPIO_TABLE_ROW*    GpioRowPtr
  )
{
  CHV_GPIO_PAD_INIT GpioConfTable[GPIO_TABLE_ROW_NUM];
  UINT8 RowIndex;
  UINT8 Mode_Select = 0;
  UINT8 Mode = 0;
  UINT8 GPIO_Config = NA;
  UINT8 GPIO_STATE = NA;
  UINT8 Gpio_Light_Mode = NA;
  UINT8 INT_Type = NA;
  UINT8 INT_S = NA;
  UINT8 Term_H_L = NA;
  UINT8 Open_Drain = NA;
  UINT8 Current_Source = NA;
  UINT8 Int_Mask = NA;
  UINT8 Glitch_Cfg = NA;
  UINT8 InvertRX_TX = NA;
  UINT8 WAKE_Mask = NA;
  UINT8 Wake_Mask_Bit = NA;
  UINT8 GPE = NA;
  UINT16 MMIO_Offset = 0;
  UINT32 Community = 0;

  RowIndex = 0;
  while (GpioRowPtr->Usage != USAGE_None) {
#ifdef EFI_DEBUG
    GpioConfTable[RowIndex].pad_name = GPIOPadNames[RowIndex];
#endif

    //
    // Mode select & GPIO_Config
    //
    switch (GpioRowPtr->Usage) {
      case USAGE_GPIO:
        Mode_Select = GPIO;
        GPIO_Config = GPIO;
        break;
      case USAGE_GPO:
        Mode_Select = GPIO;
        GPIO_Config = GPO;
        break;
      case USAGE_GPI:
        Mode_Select = GPIO;
        GPIO_Config = GPI;
        break;
      case USAGE_HIZ:
        Mode_Select = GPIO;
        GPIO_Config = HI_Z;
        break;
      case USAGE_Native:
        Mode_Select = Native;
        GPIO_Config = NA;
        break;
      default:
        ASSERT(FALSE);
      break;
    }
    //
    // Function Number
    //
    Mode = GpioRowPtr->FuncNum;
    //
    // GPIO state
    //
    switch (GpioRowPtr->InitLevel) {
      case GPIO_ATTRIBUTE_LOW:
        GPIO_STATE = LOW;
        break;
      case GPIO_ATTRIBUTE_HIGH:
        GPIO_STATE = HIGH;
        break;
      case NA:
        GPIO_STATE = NA;
        break;
      default:
        ASSERT(FALSE);
      break;
    }
    //
    // Gpio_Light_Mode
    //
    switch (GpioRowPtr->LightModeBar) {
      case GPIO_ATTRIBUTE_HIGH:
        Gpio_Light_Mode = HIGH;
        break;
      case GPIO_ATTRIBUTE_LOW:
        Gpio_Light_Mode = LOW;
        break;
      case NA:
        Gpio_Light_Mode = NA;
        break;
      default:
        ASSERT(FALSE);
      break;
    }
    //
    // INT_Type
    //
    switch (GpioRowPtr->TrigType) {
      case TRIGGER_Edge_High:
         INT_Type = Trig_Edge_High;
        break;
      case TRIGGER_Edge_Low:
         INT_Type = Trig_Edge_Low;
        break;
      case TRIGGER_Edge_Both:
         INT_Type = Trig_Edge_Both;
        break;
      case TRIGGER_Level_High:
         INT_Type = Trig_Level;
        break;
      case TRIGGER_Level_Low:
         INT_Type = Trig_Level;
        break;
      case TRIGGER_None:
        INT_Type = NA;
      default:
         ;
        break;
    }

    //
    // INT_S
    //
    INT_S = GpioRowPtr->IntLineNum;

    //
    // Term_H_L
    //
    if (GpioRowPtr->PullDirection == DIRECTION_Pull_Up) {
      if (GpioRowPtr->PullStrength == STRENGTH_20K)
        Term_H_L = P_20K_H;
      else if (GpioRowPtr->PullStrength == STRENGTH_5K)
        Term_H_L = P_5K_H;
      else if (GpioRowPtr->PullStrength == STRENGTH_1K)
        Term_H_L = P_1K_H;
      else
        Term_H_L = P_NONE;
    } else if (GpioRowPtr->PullDirection == DIRECTION_Pull_Down) {
      if (GpioRowPtr->PullStrength == STRENGTH_20K)
        Term_H_L = P_20K_L;
      else if (GpioRowPtr->PullStrength == STRENGTH_5K)
        Term_H_L = P_5K_L;
      else if (GpioRowPtr->PullStrength == STRENGTH_1K)
        Term_H_L = P_1K_L;
      else
        Term_H_L = P_NONE;
    } else {
        Term_H_L = NA;
    }

    //
    // Open_Drain
    //
    if (GpioRowPtr->DriveType == DRIVE_Open_Drain) {
        Open_Drain = _ENABLE;
    } else {
        Open_Drain = NA;
    }

    //
    // Int_Mask
    //
    if (GpioRowPtr->IntLineNum != NA) {
        Int_Mask = NonMaskable;
    } else {
        Int_Mask = NA;
    }

    //
    // Glitch_Cfg
    //
    switch (GpioRowPtr->GlitchFilterConfig) {
      case GLITCH_DISABLE:
         Glitch_Cfg = glitch_Disable;
        break;
      case GLITCH_EN_EdgeDetect:
         Glitch_Cfg = En_EdgeDetect;
        break;
      case GLITCH_EN_RX_Data:
         Glitch_Cfg = En_RX_Data;
        break;
      case GLITCH_EN_Edge_RX_Data:
         Glitch_Cfg = En_Edge_RX_Data;
        break;
      default:
         Glitch_Cfg = NA;
        break;
    }

    //
    // InvertRX_TX
    //
    switch (GpioRowPtr->InvertRxTx) {
      case GPIO_No_Inversion:
        InvertRX_TX = No_Inversion;
        break;
      case GPIO_Inv_RX_Enable:
        InvertRX_TX = Inv_RX_Enable;
        break;
      case GPIO_Inv_TX_Enable:
        InvertRX_TX = Inv_TX_Enable;
        break;
      case GPIO_Inv_RX_TX_Enable:
        InvertRX_TX = Inv_RX_TX_Enable;
        break;
      case GPIO_Inv_RX_Data:
        InvertRX_TX = Inv_RX_Data;
        break;
      case GPIO_Inv_TX_Data:
         InvertRX_TX = Inv_TX_Data;
        break;
      default:
         Glitch_Cfg = NA;
         break;
    }

    //
    // Community
    //
    if (RowIndex < GPIO_TABLE_NORTH_ROW_NUM) {
      Community = NORTH;
    } else if (RowIndex < ( GPIO_TABLE_NORTH_ROW_NUM + GPIO_TABLE_EAST_ROW_NUM )){
      Community = EAST;
    } else if (RowIndex < ( GPIO_TABLE_NORTH_ROW_NUM + GPIO_TABLE_EAST_ROW_NUM + GPIO_TABLE_SOUTHEAST_ROW_NUM)){
      Community = SOUTHEAST;
    } else if (RowIndex < GPIO_TABLE_ROW_NUM ){
      Community = SOUTHWEST;
    }

    //
    // Wake_Mask_Bit
    //
    Wake_Mask_Bit = WakeMaskBit[RowIndex];

    //
    // GPE
    //
    if ( GpioRowPtr->IntType == INT_SCI ) {
      GPE = SCI;
    } else if ( GpioRowPtr->IntType == INT_SMI ){
      GPE = SMI;
    } else {
      GPE = NA;
    }

    //
    // MMIO_Offset
    //
    MMIO_Offset = GPIOMmioOffset[RowIndex];

    GpioConfTable[RowIndex].padConfg0.padCnf0 = CHV_GPIO_PAD_CONFG0(Mode_Select, Mode, GPIO_Config, GPIO_STATE, Gpio_Light_Mode, INT_S, Term_H_L, Glitch_Cfg);
    GpioConfTable[RowIndex].padConfg0_changes.padCnf0= CHV_GPIO_PAD_CONFG0_CHANGE(GPIO_Config, GPIO_STATE, Gpio_Light_Mode, INT_S, Term_H_L, Glitch_Cfg);
    GpioConfTable[RowIndex].padConfg1.padCnf1 = CHV_GPIO_PAD_CONFG1(INT_Type, Open_Drain, Current_Source, InvertRX_TX);
    GpioConfTable[RowIndex].padConfg1_changes.padCnf1 = CHV_GPIO_PAD_CONFG1_CHANGE(INT_Type,  Open_Drain, Current_Source, InvertRX_TX);
    GpioConfTable[RowIndex].PAD_MISC.micsData = CHV_GPIO_PAD_MISC(Int_Mask, WAKE_Mask,Wake_Mask_Bit, GPE);
    GpioConfTable[RowIndex].Community = Community;
    GpioConfTable[RowIndex].MMIO_ADDRESS = CHV_GPIO_PAD_MMIO(MMIO_Offset, Community);

    GpioRowPtr++;
    RowIndex++;
    if (RowIndex >= Gpio_Pin_Num)
      break;
  }

  ASSERT(RowIndex == Gpio_Pin_Num);
  InternalGpioPADConfig (Gpio_Mmio_Offset, Gpio_Pin_Num, GpioConfTable);
}

EFI_STATUS
EFIAPI
BraswellCherryHillBoardDetectionCallback (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_PLATFORM_INFO_HOB            PlatformInfoHob;
  UINTN                            DataSize;
  UINT16                           BoardFabIds;
  UINT8                            BoardId;
  UINT8                            FabId;
  EFI_STATUS                       Status;
  VOID                             *Instance;
  PLATFORM_GPIO_TABLE_ROW*         GpioRowPtr;

  Status = PeiServicesLocatePpi (
             &gBoardDetectedPpiGuid,
             0,
             NULL,
             &Instance
             );
  if (!EFI_ERROR(Status)) {
    return EFI_SUCCESS;
  }

  DEBUG ((EFI_D_INFO, "BraswellCherryHillBoardDetectionCallback\n"));

  ZeroMem (&PlatformInfoHob, sizeof (PlatformInfoHob));

  Status = GetBoardFabIdsKsc (&BoardFabIds);
  if (Status != EFI_SUCCESS) {
    GetBoradFabIdsGpio (&BoardId, &FabId);
    if (BoardId == 1) {
      PlatformInfoHob.BoardId    = BOARD_ID_BSW_CH;
      PlatformInfoHob.BoardSvidConfig = BSW_SVID_CONFIG0;
      if (SocStepping() >= SocC0) {
        PlatformInfoHob.BoardSvidConfig = BSW_SVID_CONFIG1;
      }
      DEBUG ((EFI_D_INFO,  "I'm Braswell Cherry Hill \n\n"));
      DEBUG ((EFI_D_INFO,  "SoC Stepping = 0x%x \n", ((UINT32)SocStepping())));
      PlatformInfoHob.MemCfgID   = 0;
      DEBUG ((EFI_D_INFO,  "PlatformInfoHob->MemCfgID= 0x%x\n", PlatformInfoHob.MemCfgID));
      PlatformInfoHob.FABID = FabId;
      DEBUG ((EFI_D_INFO,  "PlatformInfoHob->FABID  = 0x%x\n", FabId ));
      PlatformInfoHob.PlatformFlavor = FlavorMobile;
      DEBUG ((EFI_D_INFO,  "PlatformInfoHob->PlatformFlavor  = 0x%x\n", PlatformInfoHob.PlatformFlavor ));
      DEBUG ((EFI_D_INFO,  "PlatformInfoHob->BoardSvidConfig  = 0x%x\n", PlatformInfoHob.BoardSvidConfig ));
      DEBUG ((EFI_D_INFO,  "PlatformInfoHob->BoardId  = 0x%x\n", PlatformInfoHob.BoardId ));

      PlatformInfoHob.ECSupport = FALSE;
      PlatformInfoHob.FanSupport = FALSE;
      PlatformInfoHob.BatterySupport = FALSE;

      DataSize = sizeof (EFI_PLATFORM_INFO_HOB);
      PcdSetPtr (PcdPlatformInfo, &DataSize, &PlatformInfoHob);

      DataSize = sizeof (EFI_GUID);
      PcdSetPtr (PcdBmpImageGuid, &DataSize, &gHdmiDpVbtGuid);

      PcdSet8 (PcdNfcConnection, 0);

      PcdSet8 (PcdOemMemeoryDimmType,DimmInstalled);

      //
      // Config SC/NC/SUS GPIO Pins
      //
      GpioRowPtr = (PLATFORM_GPIO_TABLE_ROW*)PcdGetPtr (PcdPlatformGpioTable);
      if (GpioRowPtr->Usage == USAGE_None) {
        // Program all the gpios from the default GPIO table
        InternalGpioPADConfig(0, sizeof(mBswCherryHillGpioInitData)/sizeof(mBswCherryHillGpioInitData[0]),   mBswCherryHillGpioInitData);
      } else {
        //
        // Use the GPIO table from pcd to initialize North/East/SouthWest/SouthEast GPIO Pins
        //
        DEBUG ((EFI_D_INFO, "Use GPIO table from pcd to config GPIO pins\n"));
        ProgramGpiosWithPcd (0, GPIO_TABLE_ROW_NUM, &GpioRowPtr[0]);
      }

      Status = PeiServicesInstallPpi (&mBraswellCherryHillDetectedPpi);
    }
  }

  return EFI_SUCCESS;
}

/**
  This function performs Board initialization in Pre-Memory.

  @retval     EFI_SUCCESS           The PPI is installed and initialized.
  @retval     EFI ERRORS            The PPI is not successfully installed.
  @retval     EFI_OUT_OF_RESOURCES  No enough resoruces (such as out of memory).
**/
EFI_STATUS
EFIAPI
BraswellCherryHillInitConstructor (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                        Status;

  Status = PeiServicesNotifyPpi (&mBraswellCherryHillDetectionStartNotifyList);

  return Status;
}
