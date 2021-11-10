/** @file
  Register names for CPU registers.

  <b>Conventions</b>
    Definitions beginning with "MSR_" are MSRs
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values of bits within the registers
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position

  Copyright (c) 2004 - 2016, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _CPU_REGS_H_
#define _CPU_REGS_H_

//
// Local APIC defines
//
#define APIC_REGISTER_LOCAL_ID_OFFSET         0x00000020
#define APIC_REGISTER_APIC_VERSION_OFFSET     0x00000030
#define APIC_REGISTER_SPURIOUS_VECTOR_OFFSET  0x000000F0
#define APIC_REGISTER_ICR_LOW_OFFSET          0x00000300
#define APIC_REGISTER_ICR_HIGH_OFFSET         0x00000310
#define APIC_REGISTER_LINT0_VECTOR_OFFSET     0x00000350
#define APIC_REGISTER_LINT1_VECTOR_OFFSET     0x00000360

#define BROADCAST_MODE_SPECIFY_CPU            0x00
#define BROADCAST_MODE_ALL_INCLUDING_SELF     0x01
#define BROADCAST_MODE_ALL_EXCLUDING_SELF     0x02

#ifndef DELIVERY_MODE_FIXED
#define DELIVERY_MODE_FIXED           0x0
#endif
#ifndef DELIVERY_MODE_LOWEST_PRIORITY
#define DELIVERY_MODE_LOWEST_PRIORITY 0x1
#endif
#ifndef DELIVERY_MODE_SMI
#define DELIVERY_MODE_SMI             0x2
#endif
#ifndef DELIVERY_MODE_REMOTE_READ
#define DELIVERY_MODE_REMOTE_READ     0x3
#endif
#ifndef DELIVERY_MODE_NMI
#define DELIVERY_MODE_NMI             0x4
#endif
#ifndef DELIVERY_MODE_INIT
#define DELIVERY_MODE_INIT            0x5
#endif
#ifndef DELIVERY_MODE_SIPI
#define DELIVERY_MODE_SIPI            0x6
#endif
#ifndef DELIVERY_MODE_MAX
#define DELIVERY_MODE_MAX             0x7
#endif

#ifndef TRIGGER_MODE_EDGE
#define TRIGGER_MODE_EDGE             0x00
#endif
#ifndef TRIGGER_MODE_LEVEL
#define TRIGGER_MODE_LEVEL            0x01
#endif

#ifndef CPU_FEATURE_DISABLE
#define CPU_FEATURE_DISABLE  0
#endif
#ifndef CPU_FEATURE_ENABLE
#define CPU_FEATURE_ENABLE   1
#endif

#ifndef CPU_FEATURE_ALL_CORES_ENABLE
#define CPU_FEATURE_ALL_CORES_ENABLE  4
#endif

#define CACHE_UNCACHEABLE               0
#define CACHE_WRITECOMBINING            1
#define CACHE_WRITETHROUGH              4
#define CACHE_WRITEPROTECTED            5
#define CACHE_WRITEBACK                 6

//
// MSR defines
//
#define MSR_IA32_PLATFORM_ID                                          0x00000017
#define N_PLATFORM_ID_SHIFT                                           50
#define B_PLATFORM_ID_MASK                                            0x7   ///< Bits 52:50
#define MSR_IA32_APIC_BASE                                            0x0000001B
#define B_MSR_IA32_APIC_BASE_G_XAPIC                                  BIT11
#define B_MSR_IA32_APIC_BASE_M_XAPIC                                  BIT10
#define B_MSR_IA32_APIC_BASE_BSP                                      BIT8
#define B_PIC_THREAD_CONTROL_TPR_DIS                                  BIT10
#define MSR_CORE_THREAD_COUNT                                         0x00000035
#define N_CORE_COUNT_OFFSET                                           16
#define B_THREAD_COUNT_MASK                                           0xFFFF
#define MSR_IA32_FEATURE_CONTROL                                      0x0000003A
#define B_MSR_IA32_FEATURE_CONTROL_LOCK                               BIT0
#define B_MSR_IA32_FEATURE_CONTROL_ELT                                BIT1
#define B_MSR_IA32_FEATURE_CONTROL_EVT                                BIT2
#define B_MSR_IA32_FEATURE_CONTROL_SLFE                               (BIT8 | BIT9 | BIT10 | BIT11 | BIT12 | BIT13 | BIT14)
#define B_MSR_IA32_FEATURE_CONTROL_SGE                                BIT15

#define MSR_IA32_MTRRCAP                                              0x000000FE

#define MSR_IA32_BIOS_UPDT_TRIG                                       0x00000079
#define MSR_IA32_BIOS_SIGN_ID                                         0x0000008B
#define MSR_IA32_SMM_MONITOR_CONTROL                                  0x0000009B
#define MSR_IA32_PMC0_MSR                                             0x000000C1
#define MSR_IA32_PMC1_MSR                                             0x000000C2
#define MSR_IA32_PMC2_MSR                                             0x000000C3
#define MSR_IA32_PMC3_MSR                                             0x000000C4
#define MSR_IA32_PMC4_MSR                                             0x000000C5
#define MSR_IA32_PMC5_MSR                                             0x000000C6
#define MSR_IA32_PMC6_MSR                                             0x000000C7
#define MSR_IA32_PMC7_MSR                                             0x000000C8
#define MSR_PLATFORM_INFO                                             0x000000CE
#define V_MSR_PRMRR_MASK                                              0x0000007FFFFFFFFF
#define N_PLATFORM_INFO_MIN_RATIO                                     40
#define B_PLATFORM_INFO_RATIO_MASK                                    0xFF
#define N_PLATFORM_INFO_MAX_RATIO                                     8
#define B_MSR_PLATFORM_INFO_BIOSGUARD_AVAIL                           BIT35
#define N_MSR_PLATFORM_INFO_CONFIG_TDP_NUM_LEVELS_OFFSET              33
#define V_CONFIG_TDP_NUM_LEVELS_MASK                                  (BIT34 | BIT33)
#define B_PLATFORM_INFO_TDC_TDP_LIMIT                                 BIT29
#define N_PLATFORM_INFO_RATIO_LIMIT                                   28
#define B_PLATFORM_INFO_RATIO_LIMIT                                   BIT28
#define B_FIVR_RFI_TUNING_AVAIL                                       BIT25
#define B_PLATFORM_INFO_SMM_SAVE_CONTROL                              BIT16
#define N_PLATFORM_INFO_PROG_TCC_ACTIVATION_OFFSET                    30
#define B_PLATFORM_INFO_PROG_TCC_ACTIVATION_OFFSET                    BIT30
#define B_PLATFORM_INFO_TIMED_MWAIT_SUPPORTED                         BIT37
#define B_PLATFORM_INFO_EDRAM_EN                                      BIT57
#define B_PLATFORM_INFO_PRODUCTION                                    BIT27
#define MSR_PMG_CST_CONFIG                                            0x000000E2
#define B_CST_CONTROL_LOCK                                            BIT15
#define B_IO_MWAIT_REDIRECTION_ENABLE                                 BIT10
#define B_TIMED_MWAIT_ENABLE                                          BIT31
#define B_PACKAGE_C_STATE_LIMIT                                       (BIT3 | BIT2 | BIT1 | BIT0)
#define V_PKG_CSTATE_LIMIT_C2                                         0x02
#define V_PKG_CSTATE_LIMIT_C1                                         0x01
#define V_PKG_CSTATE_LIMIT_C0                                         0x00
#define V_CSTATE_LIMIT_UNLIMIT                                        0x00
#define V_CSTATE_LIMIT_C1                                             0x10
#define V_CSTATE_LIMIT_C3                                             0x20
#define V_CSTATE_LIMIT_C6                                             0x30
#define V_CSTATE_LIMIT_C7                                             0x40
#define V_CSTATE_LIMIT_C8                                             0x50
#define V_CSTATE_LIMIT_C9                                             0x60
#define V_CSTATE_LIMIT_C10                                            0x70
#define V_CSTATE_LIMIT_MASK                                           0x000000F0
#define B_C3_AUTO_DEMOTION_ENABLE                                     BIT25
#define B_C1_AUTO_DEMOTION_ENABLE                                     BIT26
#define B_C3_AUTO_UNDEMOTION_ENABLE                                   BIT27
#define B_C1_AUTO_UNDEMOTION_ENABLE                                   BIT28
#define B_PKG_CSTATE_DEMOTION_ENABLE                                  BIT29
#define B_PKG_CSTATE_UNDEMOTION_ENABLE                                BIT30
#define V_FREQ_TUNING_MASK                                            0xFFFF
#define MSR_PMG_IO_CAPTURE_BASE                                       0x000000E4
#define B_MSR_PMG_CST_RANGE                                           (BIT18 | BIT17 | BIT16)
#define V_IO_CAPT_LVL2                                                (0x0 << 16)   ///< C3
#define V_IO_CAPT_LVL3                                                (0x1 << 16)   ///< C6
#define V_IO_CAPT_LVL4                                                (0x2 << 16)   ///< C7
#define V_IO_CAPT_LVL5                                                (0x3 << 16)   ///< C8
#define V_IO_CAPT_LVL6                                                (0x4 << 16)   ///< C9
#define V_IO_CAPT_LVL7                                                (0x5 << 16)   ///< C10
#define V_IO_CAPT_LVL2_BASE_ADDR_MASK                                 0xFFFF
#define V_IO_CAPT_LVL2_BASE_ADDR_C3                                   0x414
#define IA32_MTRR_CAP                                                 0x000000FE
#define B_IA32_MTRR_VARIABLE_SUPPORT                                  0xFF
#define B_IA32_MTRR_CAP_FIXED_SUPPORT                                 BIT8
#define B_IA32_MTRR_CAP_SMRR_SUPPORT                                  BIT11
#define B_IA32_MTRR_CAP_EMRR_SUPPORT                                  BIT12

#define IA32_MCG_CAP                                                  0x00000179
#define IA32_MCG_STATUS                                               0x0000017A
#define MSR_FLEX_RATIO                                                0x00000194
#define N_FLEX_RATIO                                                  8
#define B_FLEX_RATIO                                                  (0xFF << 8)
#define B_FLEX_EN                                                     BIT16
#define B_MAX_EXTRA_VOLTAGE                                           0xFF
#define N_OVERCLOCKING_BINS                                           17
#define B_OVERCLOCKING_BINS                                           (0x7 << 17)
#define B_OVERCLOCKING_LOCK                                           BIT20
#define RATIO_FLEX_CLEAR_MASK                                         0xFFFFFFFFFFFF00FFULL
#define MSR_IA32_PERF_STS                                             0x00000198
#define N_IA32_PERF_STSP_STATE_TARGET                                 8
#define B_IA32_PERF_STSP_STATE_MASK                                   0xFF
#define MSR_IA32_PERF_CTRL                                            0x00000199
#define N_IA32_PERF_CTRLP_STATE_TARGET                                8
#define B_IA32_PERF_CTRLP_STATE_TARGET                                (0x7F << 8)
#define B_IA32_PERF_CTRL_TURBO_DIS                                    BIT32
#define MSR_IA32_CLOCK_MODULATION                                     0x0000019A
#define IA32_THERM_INTERRUPT                                          0x0000019B
#define B_IA32_THERM_INTERRUPT_VIE                                    BIT4
#define MSR_IA32_THERM_STATUS                                         0x0000019C
#define MSR_IA32_MISC_ENABLE                                          0x000001A0
#define B_MSR_IA32_MISC_ENABLE_FSE                                    BIT0
#define B_MSR_IA32_MISC_ENABLE_TME                                    BIT3
#define N_MSR_IA32_MISC_ENABLE_EIST_OFFSET                            16
#define B_MSR_IA32_MISC_ENABLE_EIST                                   BIT16
#define B_MSR_IA32_MISC_ENABLE_MONITOR                                BIT18
#define B_MSR_IA32_MISC_ENABLE_CPUID_MAX                              BIT22
#define B_MSR_IA32_MISC_ENABLE_TPR_DIS                                BIT23
#define B_MSR_IA32_MISC_ENABLE_XD                                     BIT34
#define B_MSR_IA32_MISC_DISABLE_TURBO                                 BIT38
#define MSR_TEMPERATURE_TARGET                                        0x000001A2
#define N_MSR_TEMPERATURE_TARGET_TCC_OFFSET_LIMIT                     24
#define N_MSR_TEMPERATURE_TARGET_TCC_ACTIVATION_TEMPERATURE_OFFSET    (16)
#define B_MSR_TEMPERATURE_TARGET_TCC_ACTIVATION_TEMPERATURE_MASK      (0xFF << 16)
#define N_MSR_TEMPERATURE_TARGET_FAN_TEMP_TARGET_OFFSET               8
#define B_MSR_TEMPERATURE_TARGET_FAN_TEMP_TARGET_OFFSET               (0xFF << 8)
#define B_MSR_TEMPERATURE_TARGET_TCC_OFFSET_TIME_WINDOW               (0x7F)
#define B_MSR_TEMPERATURE_TARGET_TCC_OFFSET_MASK                      0xFF
#define B_MSR_TEMPERATURE_TARGET_TCC_OFFSET_CLAMP_ENABLE              BIT7
#define MISC_FEATURE_CONTROL                                          0x000001A4
#define B_MISC_FEATURE_CONTROL_MLC_STRP                               BIT0
#define B_MISC_FEATURE_CONTROL_MLC_SPAP                               BIT1
#define B_MISC_FEATURE_CONTROL_DCU_STRP                               BIT2
#define B_MISC_FEATURE_CONTROL_DCU_IPP                                BIT3
#define B_MISC_FEATURE_CONTROL_3_STRIKE_CNT                           BIT11
#define MSR_MISC_PWR_MGMT                                             0x000001AA
#define B_MISC_PWR_MGMT_SINGLE_PCTL_EN                                BIT0
#define B_MISC_PWR_MGMT_ENABLE_HWP                                    BIT6
#define B_MISC_PWR_MGMT_ENABLE_HWP_INTERRUPT                          BIT7
#define N_MISC_PWR_MGMT_ENABLE_HWP_INTERRUPT                          7
#define B_MISC_PWR_MGMT_ENABLE_OUT_OF_BAND_AUTONOMOUS                 BIT8
#define B_MISC_PWR_MGMT_ENABLE_SDC_OOB                                BIT9
#define N_MISC_PWR_MGMT_ENABLE_SDC_OOB                                9
#define B_MISC_PWR_MGMT_ENABLE_SDC                                    BIT10
#define N_MISC_PWR_MGMT_ENABLE_SDC                                    10
#define B_MISC_PWR_MGMT_SDC_OOB_CAPABLE                               BIT11
#define B_MISC_PWR_MGMT_ENABLE_HWP_EPP                                BIT12
#define N_MISC_PWR_MGMT_ENABLE_HWP_EPP                                12
#define B_MISC_PWR_MGMT_LOCK_TERM_INT                                 BIT22
#define MSR_TURBO_POWER_CURRENT_LIMIT                                 0x000001AC
#define B_MSR_TURBO_POWER_CURRENT_LIMIT_TDC_EN                        BIT31
#define N_MSR_TURBO_POWER_CURRENT_LIMIT_TDC_LIMIT                     16
#define B_MSR_TURBO_POWER_CURRENT_LIMIT_TDC_LIMIT                     (0x7F << 16)
#define B_MSR_TURBO_POWER_CURRENT_LIMIT_TDP_EN                        BIT15
#define N_MSR_TURBO_POWER_CURRENT_LIMIT_TDP_LIMIT                     0
#define B_MSR_TURBO_POWER_CURRENT_LIMIT_TDP_LIMIT                     (0x7F << 0)
#define MSR_TURBO_RATIO_LIMIT                                         0x000001AD
#define N_MSR_TURBO_RATIO_LIMIT_1C                                    0
#define B_MSR_TURBO_RATIO_LIMIT_1C                                    (0xFF << 0)
#define N_MSR_TURBO_RATIO_LIMIT_2C                                    8
#define B_MSR_TURBO_RATIO_LIMIT_2C                                    (0xFF << 8)
#define N_MSR_TURBO_RATIO_LIMIT_3C                                    16
#define B_MSR_TURBO_RATIO_LIMIT_3C                                    (0xFF << 16)
#define N_MSR_TURBO_RATIO_LIMIT_4C                                    24
#define B_MSR_TURBO_RATIO_LIMIT_4C                                    (0xFF << 24)
#define MSR_IA32_ENERGY_PERFORMANCE_BIAS                              0x1B0
#define B_ENERGY_POLICY_MASK                                          0xF
#define MSR_IA32_PLATFORM_DCA_CAP                                     0x000001F8
#define B_MSR_IA32_PLATFORM_DCA_CAP_TYPE0_EN                          BIT0
#define MSR_IA32_CPU_DCA_CAP                                          0x000001F9
#define B_MSR_IA32_CPU_DCA_CAP_TYPE0_SUP                              BIT0
#define MSR_IA32_DCA_0_CAP                                            0x000001FA
#define B_MSR_IA32_CPU_DCA_CAP_ENDID                                  BIT11
#define N_MSR_IA32_CPU_DCA_CAP_DELAY                                  13
#define B_MSR_IA32_CPU_DCA_CAP_DELAY                                  (BIT13 | BIT14 | BIT15 | BIT16)
#define B_MSR_IA32_CPU_DCA_CAP_SW_LOCK                                BIT24
#define B_MSR_IA32_CPU_DCA_CAP_SW_FLUSH                               BIT25
#define B_MSR_IA32_CPU_DCA_CAP_HW_LOCK                                BIT26
#define MSR_POWER_CTL                                                 0x000001FC
#define B_MSR_POWER_CTL_BI_PROCHOT                                    BIT0
#define B_MSR_POWER_CTL_C1E                                           BIT1
#define B_ENERGY_EFFICIENT_P_STATE_FEATURE_ENABLE                     BIT18
#define B_MSR_POWER_CTL_DISABLE_PROCHOT_OUT                           BIT21
#define B_MSR_POWER_CTL_PROCHOT_RESPONSE                              BIT22
#define B_MSR_POWER_CTL_PROCHOT_LOCK                                  BIT23
#define B_MSR_POWER_CTL_DISABLE_VR_THERMAL_ALERT                      BIT24
#define B_MSR_POWER_CTL_CSTATE_PRE_WAKE_DISABLE                       BIT30
#define B_MSR_FERR_ENABLE                                             BIT0

#define V_POWER_LIMIT_4_MASK                                          (0x1FFF)
#define B_CURRENT_LIMIT_LOCK                                          BIT31
#define B_CURRENT_LIMIT_MASK                                          0x1FFF

#define MSR_PACKAGE_POWER_SKU_UNIT                                    0x606
#define PACKAGE_POWER_UNIT_MASK                                       0xF
#define PACKAGE_TIME_UNIT_MASK                                        0xF0000

#define B_PKG_IRTL_VALID                                              BIT15
#define B_INTERRUPT_RESPONSE_TIME_LIMIT_MASK                          0x3FF
#define B_TIME_UNIT_MASK                                              (0x7 << 10)
#define N_TIME_UNIT_OFFSET                                            10
#define MSR_PACKAGE_POWER_LIMIT                                       0x610
#define MSR_PACKAGE_POWER_SKU                                         0x614
#define B_POWER_LIMIT_ENABLE                                          BIT15
#define B_CRITICAL_POWER_CLAMP_ENABLE                                 BIT16
#define B_POWER_LIMIT_LOCK                                            BIT31
#define POWER_SKU_MASK                                                (0x7FFF)
#define POWER_LIMIT_MASK                                              (0x7FFF)
#define POWER_LIMIT_1_TIME_MASK                                       (0xFE0000)
#define PACKAGE_TDP_POWER_MASK                                        (0x7FFF)
#define PACKAGE_MIN_POWER_MASK                                        (0x7FFF0000)
#define PACKAGE_MAX_POWER_MASK                                        (0x7FFF)
#define MSR_PL3_CONTROL                                               0x615
#define POWER_LIMIT_3_TIME_MASK                                       (0xFE0000)
#define POWER_LIMIT_3_DUTY_CYCLE_MASK                                 (0x7F000000)
#define MSR_DDR_RAPL_LIMIT                                            0x618
#define MSR_MAX_RING_RATIO_LIMIT_MASK                                 0x7F
#define MSR_CONFIG_TDP_NOMINAL                                        0x648
#define CONFIG_TDP_NOMINAL_RATIO_MASK                                 0xFF
#define MSR_CONFIG_TDP_LVL1                                           0x649
#define CONFIG_TDP_LVL1_RATIO_OFFSET                                  16
#define CONFIG_TDP_LVL1_RATIO_MASK                                    (0xFF << 16)
#define CONFIG_TDP_LVL1_PKG_TDP_MASK                                  (0x7FFF)
#define MSR_CONFIG_TDP_LVL2                                           0x64A
#define CONFIG_TDP_LVL2_RATIO_OFFSET                                  16
#define CONFIG_TDP_LVL2_RATIO_MASK                                    (0xFF << 16)
#define CONFIG_TDP_LVL2_PKG_TDP_MASK                                  (0x7FFF)
#define MSR_CONFIG_TDP_CONTROL                                        0x64B
#define CONFIG_TDP_CONTROL_LOCK                                       (1 << 31)
#define CONFIG_TDP_CONTROL_LVL_MASK                                   0x3
#define CONFIG_TDP_NOMINAL                                            0
#define CONFIG_TDP_LEVEL1                                             1
#define CONFIG_TDP_LEVEL2                                             2
#define MSR_TURBO_ACTIVATION_RATIO                                    0x64C
#define MSR_TURBO_ACTIVATION_RATIO_LOCK                               (1 << 31)
#define MSR_TURBO_ACTIVATION_RATIO_MASK                               0xFF
#define MSR_PKG_HDC_CONFIG_CTL                                        0x00000652
#define B_PKG_HDC_CONFIG_CTL_SDC_CX_MONITOR                           (BIT0 | BIT1 | BIT2)
#define N_PKG_HDC_CONFIG_CTL_SDC_CX_MONITOR                           0
#define B_PKG_HDC_CONFIG_CTL_SDC_MCNT_COUNT_METHOD                    BIT3
#define N_PKG_HDC_CONFIG_CTL_SDC_MCNT_COUNT_METHOD                    3
#define B_PKG_HDC_CONFIG_CTL_SDC_MAX_FORCE_IDLE_DURATION_TIME         (BIT5 | BIT6 | BIT7 | BIT8 | BIT9 | BIT10)
#define N_PKG_HDC_CONFIG_CTL_SDC_MAX_FORCE_IDLE_DURATION_TIME         5
#define B_PKG_HDC_CONFIG_CTL_SDC_DIRECT_CONTROL                       (BIT12 | BIT13 | BIT14 | BIT15 | BIT16 | BIT17 | BIT18)
#define N_PKG_HDC_CONFIG_CTL_SDC_DIRECT_CONTROL                       12
#define B_PKG_HDC_CONFIG_CTL_MIN_ACTIVE_TIME                          (BIT19 | BIT20 | BIT21 | BIT22 | BIT23 | BIT24 | BIT25 | BIT26)
#define N_PKG_HDC_CONFIG_CTL_MIN_ACTIVE_TIME                          19
#define MSR_IA32_HDC_PKG_CTL                                          0x00000DB0
#define B_HDC_PKG_CTL_SDC_PACKAGE_ENABLE                              BIT0
#define N_HDC_PKG_CTL_SDC_PACKAGE_ENABLE                              0
#define B_HDC_PKG_CTL_SDC_WAS_ONCE_ENABLED                            BIT1
#define N_HDC_PKG_CTL_SDC_WAS_ONCE_ENABLED                            1
#define MSR_IA32_PM_CTL1                                              0x00000DB1
#define B_PM_CTL1_SDC_ALLOWED                                         BIT0
#define N_PM_CTL1_SDC_ALLOWED                                         0
#define SMRR_PHYS_BASE                                                0x000001F2
#define SMRR_PHYS_MASK                                                0x000001F3
#define EMRR_PHYS_BASE                                                0x000001F4
#define EMRR_PHYS_MASK                                                0x000001F5
#define B_MSR_EMRR_PHYS_MASK_EN                                       BIT11
#define B_MSR_EMRR_PHYS_MASK_LOCK                                     BIT10
#define V_MAXIMUM_VARIABLE_MTRR_NUMBER                                10
#define CACHE_VARIABLE_MTRR_BASE                                      0x00000200
#define V_FIXED_MTRR_NUMBER                                           11
#define IA32_MTRR_FIX64K_00000                                        0x00000250
#define IA32_MTRR_FIX16K_80000                                        0x00000258
#define IA32_MTRR_FIX16K_A0000                                        0x00000259
#define IA32_MTRR_FIX4K_C0000                                         0x00000268
#define IA32_MTRR_FIX4K_C8000                                         0x00000269
#define IA32_MTRR_FIX4K_D0000                                         0x0000026A
#define IA32_MTRR_FIX4K_D8000                                         0x0000026B
#define IA32_MTRR_FIX4K_E0000                                         0x0000026C
#define IA32_MTRR_FIX4K_E8000                                         0x0000026D
#define IA32_MTRR_FIX4K_F0000                                         0x0000026E
#define IA32_MTRR_FIX4K_F8000                                         0x0000026F
#define MSR_IA32_CR_PAT                                               0x00000277
#define CACHE_IA32_MTRR_DEF_TYPE                                      0x000002FF
#define B_CACHE_MTRR_VALID                                            BIT11
#define B_CACHE_FIXED_MTRR_VALID                                      BIT10
#define NO_EVICT_MODE                                                 0x000002E0
#define B_NO_EVICT_MODE_SETUP                                         BIT0
#define B_NO_EVICT_MODE_RUN                                           BIT1
#define B_LOCK_MEM_CFG                                                BIT1
#define IA32_MC0_CTL                                                  0x00000400
#define IA32_MC0_STATUS                                               0x00000401
#define IA32_MC0_ADDR                                                 0x00000402
#define IA32_MC0_MISC                                                 0x00000403
#define IA32_MC4_CTL                                                  (IA32_MC0_CTL + (4 * 4))
#define IA32_MC8_CTL                                                  (IA32_MC0_CTL + (8 * 4))
#define IA32_MC4_STATUS                                               (IA32_MC0_STATUS + (4 * 4))
#define IA32_MC5_STATUS                                               (IA32_MC0_STATUS + (5 * 4))
#define IA32_MC6_STATUS                                               (IA32_MC0_STATUS + (6 * 4))
#define IA32_MC7_STATUS                                               (IA32_MC0_STATUS + (7 * 4))
#define IA32_MC8_STATUS                                               (IA32_MC0_STATUS + (8 * 4))
#define MSR_IA32_VMX_BASIC                                            0x00000480
#define MSR_IA32_VMX_MISC                                             0x00000485
#define APIC_GLOBAL_ENABLE                                            0x00000800
#define EXT_XAPIC_LOGICAL_APIC_ID                                     0x00000802
#define EXT_XAPIC_VERSION                                             0x00000803
#define EXT_XAPIC_SVR                                                 0x0000080F
#define EXT_XAPIC_ICR                                                 0x00000830
#define MSR_EXT_XAPIC_LVT_THERM                                       0x00000833
#define EXT_XAPIC_LVT_LINT0                                           0x00000835
#define EXT_XAPIC_LVT_LINT1                                           0x00000836
#define MSR_IA32_DEBUG_INTERFACE                                      0x00000C80
#define B_DEBUG_INTERFACE_ENABLE                                      BIT0
#define B_DEBUG_INTERFACE_LOCK                                        BIT30
#define B_DEBUG_INTERFACE_DEBUG_STATUS                                BIT31
#define NUM_TENTHS_TO_PERCENTAGE                                      1000
#define FIVR_NOTCH_MASK                                               0x0000FF00
#define FIVR_SSC_MASK                                                 0x000000FF
#define FIVR_SSC_LOCK_BIT                                             BIT31
#define MAX_FIVR_SSC_PERCENT                                          60
#define MAX_FIVR_NOTCH_PERCENT                                        50

//
// MSRs for SMM State Save Register
//
#define MSR_SMM_MCA_CAP                                               0x17D
#define B_TARGETED_SMI                                                BIT56
#define N_TARGETED_SMI                                                56
#define B_SMM_CPU_SVRSTR                                              BIT57
#define N_SMM_CPU_SVRSTR                                              57
#define B_SMM_CODE_ACCESS_CHK                                         BIT58
#define N_SMM_CODE_ACCESS_CHK                                         58
#define B_LONG_FLOW_INDICATION                                        BIT59
#define N_LONG_FLOW_INDICATION                                        59
#define MSR_SMM_FEATURE_CONTROL                                       0x4E0
#define B_SMM_FEATURE_CONTROL_LOCK                                    BIT0
#define B_SMM_CPU_SAVE_EN                                             BIT1
#define B_SMM_CODE_CHK_EN                                             BIT2
#define MSR_SMM_DELAYED                                               0x4E2
#define MSR_SMM_BLOCKED                                               0x4E3

#define TXT_PUBLIC_BASE                                               0xFED30000
#define R_CPU_BOOT_GUARD_ERRORCODE                                    0x30
#define R_CPU_BOOT_GUARD_BOOTSTATUS                                   0xA0
#define R_CPU_BOOT_GUARD_ACM_STATUS                                   0x328
#define V_CPU_BOOT_GUARD_LOAD_ACM_SUCCESS                             0x8000000000000000
#define B_BOOT_GUARD_ACM_ERRORCODE_MASK                               0x00007FF0

///
/// Local APIC definitions
///
#define MSR_XAPIC_BASE                                                0x1B
#define MMIO_LOCAL_APIC_THERMAL_DEF                                   0xFEE00330
#define B_INTERRUPT_MASK                                              (1 << 16)
#define B_DELIVERY_MODE                                               (0x07 << 8)
#define V_MODE_SMI                                                    (0x02 << 8)
#define B_VECTOR                                                      (0xFF << 0)
//
// Processor Definitions
//
#define CPUID_FULL_STEPPING                     0x0000000F
#define CPUID_FULL_FAMILY_MODEL                 0x0FFF0FF0
#define CPUID_FULL_FAMILY_MODEL_STEPPING        0x0FFF0FFF
#define CPUID_FULL_FAMILY_MODEL_BROXTON         0x000506C0

#define CPUID_PROCESSOR_TOPOLOGY                0xB

#ifndef SLE_FLAG
#ifndef STALL_ONE_MICRO_SECOND
#define STALL_ONE_MICRO_SECOND 1
#endif
#ifndef STALL_ONE_MILLI_SECOND
#define STALL_ONE_MILLI_SECOND 1000
#endif
#else // SLE FLAG
#ifndef STALL_ONE_MICRO_SECOND
#define STALL_ONE_MICRO_SECOND 0
#endif
#ifndef STALL_ONE_MILLI_SECOND
#define STALL_ONE_MILLI_SECOND 0
#endif
#endif // SLE_FLAG

#define BITS(x) (1 << (x))

/**
Notes :
  1.  Bit position always starts at 0.
  2.  Following macros are applicable only for Word aligned integers.
**/
#define BIT(Pos, Value)               (1 << (Pos) & (Value))
#define BITRANGE(From, Width, Value)  (((Value) >> (From)) & ((1 << (Width)) - 1))

typedef enum {
  EnumCpuBroxton    = CPUID_FULL_FAMILY_MODEL_BROXTON,
  EnumCpuMax        = CPUID_FULL_FAMILY_MODEL
} CPU_FAMILY;

///
/// Enums for CPU Stepping IDs
///
typedef enum {
  ///
  /// Broxton Steppings
  ///
  EnumBxtA0         = 0,
  ///
  /// Max Stepping
  ///
  EnumCpuSteppingMax  = CPUID_FULL_STEPPING
} CPU_STEPPING;



#define EFI_CACHE_UNCACHEABLE                  0
#define EFI_CACHE_WRITECOMBINING               1
#define EFI_CACHE_WRITETHROUGH                 4
#define EFI_CACHE_WRITEPROTECTED               5
#define EFI_CACHE_WRITEBACK                    6

//
// CPUID defines
//
#define   EFI_CPUID_SIGNATURE                    0x0

#define   EFI_CPUID_VERSION_INFO                 0x1
#define   B_EFI_CPUID_VERSION_INFO_EAX_MASK                            0x0FFF0FFF
#define   B_EFI_CPUID_VERSION_INFO_EAX_FULL_FAMILY_MODEL_MASK          0x0FFF0FF0
#define   B_EFI_CPUID_VERSION_INFO_EAX_EXT_FAMILY_ID_MASK              0x0FF00000
#define   B_EFI_CPUID_VERSION_INFO_EAX_EXT_MODEL_ID_MASK               0x000F0000
#define   N_EFI_CPUID_VERSION_INFO_EAX_EXT_FAMILY_ID                   20
#define   N_EFI_CPUID_VERSION_INFO_EAX_EXT_MODEL_ID                    16
#define   N_EFI_CPUID_VERSION_INFO_EAX_TYPE                            12
#define   N_EFI_CPUID_VERSION_INFO_EAX_FAMILY_ID                       8
#define   N_EFI_CPUID_VERSION_INFO_EAX_MODEL                           4
#define   N_EFI_CPUID_VERSION_INFO_EAX_STEPPING_ID                     0
#define   B_EFI_CPUID_VERSION_INFO_EBX_DEFAULT_APIC_ID                 (BIT31 | BIT30 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24)
#define   B_EFI_CPUID_VERSION_INFO_EBX_LOGICAL_CPU_PER_PACKAGE         (BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16)
#define   B_EFI_CPUID_VERSION_INFO_EBX_CLFLUSH_CHUNK_COUNT             (BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8)
#define   B_EFI_CPUID_VERSION_INFO_ECX_AES                             BIT25
#define   B_EFI_CPUID_VERSION_INFO_ECX_XAPIC                           BIT21
#define   B_EFI_CPUID_VERSION_INFO_ECX_SSE4_2                          BIT20
#define   B_EFI_CPUID_VERSION_INFO_ECX_SSE4_1                          BIT19
#define   B_EFI_CPUID_VERSION_INFO_ECX_DCA                             BIT18
#define   B_EFI_CPUID_VERSION_INFO_ECX_XTPR_UPDATE                     BIT14
#define   B_EFI_CPUID_VERSION_INFO_ECX_CMPXCHG16B                      BIT13
#define   B_EFI_CPUID_VERSION_INFO_ECX_L1_CONTEXT_ID                   BIT10
#define   B_EFI_CPUID_VERSION_INFO_ECX_SUP_SSE3                        BIT9
#define   B_EFI_CPUID_VERSION_INFO_ECX_TM2                             BIT8
#define   B_EFI_CPUID_VERSION_INFO_ECX_EIST                            BIT7
#define   B_EFI_CPUID_VERSION_INFO_ECX_SME                             BIT6
#define   B_EFI_CPUID_VERSION_INFO_ECX_VME                             BIT5
#define   B_EFI_CPUID_VERSION_INFO_ECX_QPL                             BIT4
#define   B_EFI_CPUID_VERSION_INFO_ECX_MWAIT                           BIT3
#define   B_EFI_CPUID_VERSION_INFO_ECX_SSE3                            BIT0
#define   B_EFI_CPUID_VERSION_INFO_EDX_PBE                             BIT31
#define   B_EFI_CPUID_VERSION_INFO_EDX_THERMAL_CLOCK_CONTROL           BIT29
#define   B_EFI_CPUID_VERSION_INFO_EDX_HT                              BIT28
#define   B_EFI_CPUID_VERSION_INFO_EDX_SELF_SNOOP                      BIT27
#define   B_EFI_CPUID_VERSION_INFO_EDX_SSE2                            BIT26
#define   B_EFI_CPUID_VERSION_INFO_EDX_SSE                             BIT25
#define   B_EFI_CPUID_VERSION_INFO_EDX_FAST_SAVE_RESTORE               BIT24
#define   B_EFI_CPUID_VERSION_INFO_EDX_MMX                             BIT23
#define   B_EFI_CPUID_VERSION_INFO_EDX_ACPI_SUPPORT                    BIT22
#define   B_EFI_CPUID_VERSION_INFO_EDX_DEBUG_TRACE_STORE               BIT21
#define   B_EFI_CPUID_VERSION_INFO_EDX_CLFLUSH_INTR                    BIT19
#define   B_EFI_CPUID_VERSION_INFO_EDX_CPU_SERIAL_NUMBER               BIT18
#define   B_EFI_CPUID_VERSION_INFO_EDX_PSE                             BIT17
#define   B_EFI_CPUID_VERSION_INFO_EDX_PAT                             BIT16
#define   B_EFI_CPUID_VERSION_INFO_EDX_CON_MOVE_INTR                   BIT15
#define   B_EFI_CPUID_VERSION_INFO_EDX_MCA                             BIT14
#define   B_EFI_CPUID_VERSION_INFO_EDX_PGE                             BIT13
#define   B_EFI_CPUID_VERSION_INFO_EDX_MTRR                            BIT12
#define   B_EFI_CPUID_VERSION_INFO_EDX_SEP                             BIT11
#define   B_EFI_CPUID_VERSION_INFO_EDX_ON_CHIP_APIC                    BIT9
#define   B_EFI_CPUID_VERSION_INFO_EDX_CMPXCHG8                        BIT8
#define   B_EFI_CPUID_VERSION_INFO_EDX_MCE                             BIT7
#define   B_EFI_CPUID_VERSION_INFO_EDX_PAE                             BIT6
#define   B_EFI_CPUID_VERSION_INFO_EDX_MSR                             BIT5
#define   B_EFI_CPUID_VERSION_INFO_EDX_TIME_STAMP_COUNTER              BIT4
#define   B_EFI_CPUID_VERSION_INFO_EDX_PAGE_SIZE_EXT                   BIT3
#define   B_EFI_CPUID_VERSION_INFO_EDX_DEBUG_EXT                       BIT2
#define   B_EFI_CPUID_VERSION_INFO_EDX_VME_8086                        BIT1
#define   B_EFI_CPUID_VERSION_INFO_EDX_FP_386                          BIT0

#define EFI_CPUID_CACHE_INFO                   0x2
#define EFI_CPUID_SERIAL_NUMBER                0x3

#define EFI_CPUID_CACHE_PARAMS                 0x4
#define   B_EFI_CPUID_CACHE_PARAMS_EAX_MAX_CORES_IN_PACKAGE            (BIT31 | BIT30 | BIT29 | BIT28 | BIT27 | BIT26)
#define   B_EFI_CPUID_CACHE_PARAMS_EAX_TOTAL_THREADS_SHARE_CACHE       (BIT25 | BIT24 | BIT23 | BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16 | BIT15 | BIT14)
#define   B_EFI_CPUID_CACHE_PARAMS_EAX_FULLY_ASSOCIATIVE_CACHE         BIT9
#define   B_EFI_CPUID_CACHE_PARAMS_EAX_SELF_INITIALIZING               BIT8
#define   B_EFI_CPUID_CACHE_PARAMS_EAX_CACHE_LEVEL                     (BIT7 | BIT6 | BIT5)
#define   B_EFI_CPUID_CACHE_PARAMS_EAX_CACHE_TYPE                      (BIT4 | BIT3 | BIT2 | BIT1 | BIT0)
#define   B_EFI_CPUID_CACHE_PARAMS_EBX_WAYS_OF_ASSOCIATIVITY           (BIT31 | BIT30 | BIT29 | BIT28 | BIT27 | BIT26 | BIT25 | BIT24 | BIT23 | BIT22)
#define   B_EFI_CPUID_CACHE_PARAMS_EBX_PHYSICAL_LINE_PARTITIONS        (BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16 | BIT15 | BIT14 | BIT13 | BIT12)
#define   B_EFI_CPUID_CACHE_PARAMS_EBX_SYSTEM_COHERENCY_LINE_SIZE      (BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)
#define   B_EFI_CPUID_CACHE_PARAMS_EDX_PREFETCH_STRIDE                 (BIT9 | BIT8 | BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)
#define   B_EFI_CPUID_CACHE_PARAMS_EDX_CACHE_INCLUSIVE_IN_LOWER_CACHE  BIT1
#define   B_EFI_CPUID_CACHE_PARAMS_EDX_WBINVD_INVD_ON_LOWER_CACHE      BIT0

#define EFI_CPUID_MONITOR_MWAIT_PARAMS         0x5
#define   B_EFI_CPUID_MONITOR_MWAIT_ECX_INTERRUPTS_BREAK_MWAIT         BIT1
#define   B_EFI_CPUID_MONITOR_MWAIT_ECX_MWAIT_SUPPORT                  BIT0
#define   B_EFI_CPUID_MONITOR_MWAIT_EDX_PARAMS_C7                      (BIT31 | BIT30 | BIT29 | BIT28)
#define   N_EFI_CPUID_MONITOR_MWAIT_EDX_PARAMS_C7                      28
#define   B_EFI_CPUID_MONITOR_MWAIT_EDX_PARAMS_C6                      (BIT27 | BIT26 | BIT25 | BIT24)
#define   N_EFI_CPUID_MONITOR_MWAIT_EDX_PARAMS_C6                      24
#define   B_EFI_CPUID_MONITOR_MWAIT_EDX_PARAMS_C5                      (BIT23 | BIT22 | BIT21 | BIT20)
#define   N_EFI_CPUID_MONITOR_MWAIT_EDX_PARAMS_C5                      20
#define   B_EFI_CPUID_MONITOR_MWAIT_EDX_PARAMS_C4                      (BIT19 | BIT18 | BIT17 | BIT16)
#define   N_EFI_CPUID_MONITOR_MWAIT_EDX_PARAMS_C4                      16
#define   B_EFI_CPUID_MONITOR_MWAIT_EDX_PARAMS_C3                      (BIT15 | BIT14 | BIT13 | BIT12)
#define   N_EFI_CPUID_MONITOR_MWAIT_EDX_PARAMS_C3                      12
#define   B_EFI_CPUID_MONITOR_MWAIT_EDX_PARAMS_C2                      (BIT11 | BIT10 | BIT9 | BIT8)
#define   N_EFI_CPUID_MONITOR_MWAIT_EDX_PARAMS_C2                      8
#define   B_EFI_CPUID_MONITOR_MWAIT_EDX_PARAMS_C1                      (BIT7 | BIT6 | BIT5 | BIT4)
#define   N_EFI_CPUID_MONITOR_MWAIT_EDX_PARAMS_C1                      4
#define   B_EFI_CPUID_MONITOR_MWAIT_EDX_PARAMS_C0                      (BIT3 | BIT2 | BIT1 | BIT0)
#define   N_EFI_CPUID_MONITOR_MWAIT_EDX_PARAMS_C0                      0

#define EFI_CPUID_POWER_MANAGEMENT_PARAMS      0x6
#define   EFI_CPUID_POWER_MANAGEMENT_EAX_PECI                          BIT0
#define   EFI_CPUID_POWER_MANAGEMENT_EBX_NUM_INT_THRESHOLDS            (BIT3 | BIT2 | BIT1 | BIT0)
#define   EFI_CPUID_POWER_MANAGEMENT_ECX_HW_COORDINATION_FEEDBACK      BIT0

#define EFI_CPUID_REV7                         0x7
#define EFI_CPUID_REV8                         0x8
#define EFI_CPUID_DCA_PARAMS                   0x9
#define EFI_CPUID_ARCH_PERF_MON                0xA
#define EFI_CPUID_CORE_TOPOLOGY                0xB

#define EFI_CPUID_EXTENDED_FUNCTION            0x80000000

#define EFI_CPUID_EXTENDED_FEATURE_BITS        0x80000001
#define EFI_CPUID_EXTENDED_FEATURE_BITS_ECX_LAHF_SAHF                  BIT0
#define EFI_CPUID_EXTENDED_FEATURE_BITS_EDX_XD                         BIT20
#define EFI_CPUID_EXTENDED_FEATURE_BITS_EDX_SYSCALL                    BIT11

//
// This constant defines the maximum length of the CPU brand string. According to the
// IA manual, the brand string is in EAX through EDX (thus 16 bytes) after executing
// the CPUID instructions with EAX as 80000002, 80000003, 80000004.
//
#define MAXIMUM_CPU_BRAND_STRING_LENGTH        48

#define EFI_CPUID_BRAND_STRING1                0x80000002
#define EFI_CPUID_BRAND_STRING2                0x80000003
#define EFI_CPUID_BRAND_STRING3                0x80000004

#define EFI_CPUID_ADVANCED_POWER_MANAGEMENT    0x80000007
#define   EFI_CPUID_ADVANCED_POWER_MANAGEMENT_EDX_TSC_INVARIANCE         BIT8

#define EFI_CPUID_VIRT_PHYS_ADDRESS_SIZE       0x80000008
#define   B_EFI_CPUID_VIRTUAL_ADDRESS_BITS                             (BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8)
#define   B_EFI_CPUID_PHYSICAL_ADDRESS_BITS                            (BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)

//
// Common MSR
//
#ifndef EFI_MSR_IA32_PLATFORM_ID
#define EFI_MSR_IA32_PLATFORM_ID               0x00000017
#endif // EFI_MSR_IA32_PLATFORM_ID
#define   N_EFI_MSR_IA32_PLATFORM_ID_PLATFORM_ID_BITS                  50
#define   B_EFI_MSR_IA32_PLATFORM_ID_PLATFORM_ID_BITS_MASK             (BIT52 | BIT51 | BIT50)
#define   N_EFI_MSR_IA32_PLATFORM_ID_PLATFORM_ID_BITS_MASK_START       50
#define   N_EFI_MSR_IA32_PLATFORM_ID_PLATFORM_ID_BITS_MASK_END         52

#ifndef EFI_MSR_IA32_APIC_BASE
#define EFI_MSR_IA32_APIC_BASE                 0x0000001B


#define   B_EFI_MSR_IA32_APIC_BASE_APIC_BASE_ADDRESS                   0xFFFFFF000  //For Nehalem, base address can be up to 43 bits but not cover here yet
#define   B_EFI_MSR_IA32_APIC_BASE_APIC_GLOBAL_ENABLE                  BIT11
#define   B_EFI_MSR_IA32_APIC_BASE_M_XAPIC                             BIT10
#define   B_EFI_MSR_IA32_APIC_BASE_BSP                                 BIT8
#endif // EFI_MSR_IA32_APIC_BASE
//
// Local APIC defines, offset from APIC base address
//
#define APIC_REGISTER_LOCAL_ID_OFFSET          0x00000020
#define   N_APIC_REGISTER_LOCAL_ID_OFFSET_XAPIC_ID_MASK                24
#define   B_APIC_REGISTER_LOCAL_ID_OFFSET_XAPIC_ID_MASK                0xFF000000

#define APIC_REGISTER_APIC_VERSION_OFFSET      0x00000030
#define   B_APIC_REGISTER_APIC_VERSION_OFFSET_VERSION_MASK             0xFF

#define APIC_REGISTER_SPURIOUS_VECTOR_OFFSET   0x000000F0
#define APIC_REGISTER_ICR_LOW_OFFSET           0x00000300
#define APIC_REGISTER_ICR_HIGH_OFFSET          0x00000310
#define APIC_REGISTER_LINT0_VECTOR_OFFSET      0x00000350
#define APIC_REGISTER_LINT1_VECTOR_OFFSET      0x00000360

#define EFI_MSR_IA32_FEATURE_CONTROL           0x0000003A
#define   B_EFI_MSR_IA32_FEATURE_CONTROL_SGE                           BIT15
#define   B_EFI_MSR_IA32_FEATURE_CONTROL_SLFE                          (BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8)
#define   B_EFI_MSR_IA32_FEATURE_CONTROL_SMRR                          BIT3
#define   B_EFI_MSR_IA32_FEATURE_CONTROL_EVT                           BIT2
#define   B_EFI_MSR_IA32_FEATURE_CONTROL_ELT                           BIT1
#define   B_EFI_MSR_IA32_FEATURE_CONTROL_LOCK                          BIT0
#define   B_EFI_MSR_IA32_FEATURE_CONTROL_VT_SECURE                     0x0000FF02

#ifndef EFI_MSR_IA32_BIOS_UPDT_TRIG
#define EFI_MSR_IA32_BIOS_UPDT_TRIG            0x00000079
#endif
#ifndef EFI_MSR_IA32_BIOS_SIGN_ID
#define EFI_MSR_IA32_BIOS_SIGN_ID              0x0000008B
#endif

#define EFI_MSR_IA32_MCG_CAP                   0x00000179
#define EFI_MSR_IA32_MCG_STATUS                0x0000017A

#define EFI_MSR_PMG_CST_CONFIG                 0x000000E2
#define   B_EFI_MSR_PMG_CST_CONFIG_CST_CONTROL_LOCK                    BIT15
#define   B_EFI_MSR_PMG_CST_CONFIG_IO_MWAIT_REDIRECTION_ENABLE         BIT10
#define   B_EFI_MSR_PMG_CST_CONFIG_PACKAGE_C_STATE_LIMIT               (BIT2 | BIT1 | BIT0)

#define EFI_MSR_PMG_IO_CAPTURE_ADDR            0x000000E4   //For Nehalem Spec: EFI_IA32_PMG_IO_CAPTURE_BASE
#define   N_EFI_MSR_PMG_IO_CAPTURE_ADDR_CST_RANGE                      16
#define   B_EFI_MSR_PMG_IO_CAPTURE_ADDR_LVL_2_BASE_ADDRESS_MASK        0xFFFF

#define EFI_MSR_IA32_MPERF                     0x000000E7
#define EFI_MSR_IA32_APERF                     0x000000E8

#define EFI_MSR_IA32_MTRR_CAP                  0x000000FE
#define   B_EFI_MSR_IA32_MTRR_CAP_EMRR_SUPPORT                         BIT12
#define   B_EFI_MSR_IA32_MTRR_CAP_SMRR_SUPPORT                         BIT11
#define   B_EFI_MSR_IA32_MTRR_CAP_WC_SUPPORT                           BIT10
#define   B_EFI_MSR_IA32_MTRR_CAP_FIXED_SUPPORT                        BIT8
#define   B_EFI_MSR_IA32_MTRR_CAP_VARIABLE_SUPPORT                     (BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)

#define EFI_MSR_CLOCK_FLEX_MAX                 0x00000194
#define   B_EFI_MSR_CLOCK_FLEX_MAX_FLEX_EN                             BIT16
#define   B_EFI_MSR_CLOCK_FLEX_MAX_FLEX_RATIO_MASK                     0x1F
#define   N_EFI_MSR_CLOCK_FLEX_MAX_FLEX_RATIO                          8

#define EFI_MSR_IA32_PERF_STS                  0x00000198
#define EFI_MSR_IA32_PERF_CTRL                 0x00000199

#ifndef EFI_MSR_IA32_THERM_INTERRUPT
#define EFI_MSR_IA32_THERM_INTERRUPT           0x0000019B
#endif

#define   B_EFI_MSR_IA32_THERM_INTERRUPT_VIE                           BIT4

#ifndef EFI_MSR_IA32_THERM_STATUS
#define EFI_MSR_IA32_THERM_STATUS              0x0000019C
#endif

#ifndef EFI_MSR_IA32_MISC_ENABLE
#define EFI_MSR_IA32_MISC_ENABLE               0x000001A0
#endif

#ifndef EFI_MSR_POWER_MISC
#define EFI_MSR_POWER_MISC                      0x00000120
  #define B_EFI_MSR_POWER_MISC_ENABLE_ULFM_AUTOCM_MASK               (1 << 2)
  #define B_EFI_MSR_POWER_MISC_ENABLE_INDP_AUTOCM_MASK               (1 << 3)
  #define B_EFI_MSR_POWER_MISC_ENABLE_IA_UNTRUSTED_MODE              (1 << 6)
#endif

#define   B_EFI_MSR_IA32_MISC_ENABLE_XD                                BIT34
#define   B_EFI_MSR_IA32_MISC_ENABLE_CPUID_MAX                         BIT22
#define   B_EFI_MSR_IA32_MISC_ENABLE_MONITOR                           BIT18
#define   B_EFI_MSR_IA32_MISC_ENABLE_EIST                              BIT16
#define   B_EFI_MSR_IA32_MISC_ENABLE_TM1_EN                            BIT3

#define EFI_MSR_SMRR_PHYS_BASE                 0x000001F2
#define EFI_MSR_SMRR_PHYS_MASK                 0x000001F3

#define EFI_MSR_CACHE_VARIABLE_MTRR_BASE       0x00000200
#define EFI_MSR_CACHE_VARIABLE_MTRR_END        0x0000020F
#define   V_EFI_FIXED_MTRR_NUMBER                                      11

#define EFI_MSR_IA32_MTRR_FIX64K_00000         0x00000250
#define EFI_MSR_IA32_MTRR_FIX16K_80000         0x00000258
#define EFI_MSR_IA32_MTRR_FIX16K_A0000         0x00000259
#define EFI_MSR_IA32_MTRR_FIX4K_C0000          0x00000268
#define EFI_MSR_IA32_MTRR_FIX4K_C8000          0x00000269
#define EFI_MSR_IA32_MTRR_FIX4K_D0000          0x0000026A
#define EFI_MSR_IA32_MTRR_FIX4K_D8000          0x0000026B
#define EFI_MSR_IA32_MTRR_FIX4K_E0000          0x0000026C
#define EFI_MSR_IA32_MTRR_FIX4K_E8000          0x0000026D
#define EFI_MSR_IA32_MTRR_FIX4K_F0000          0x0000026E
#define EFI_MSR_IA32_MTRR_FIX4K_F8000          0x0000026F
#define EFI_MSR_CACHE_IA32_MTRR_DEF_TYPE       0x000002FF
#define   B_EFI_MSR_CACHE_MTRR_VALID                                   BIT11
#define   B_EFI_MSR_GLOBAL_MTRR_ENABLE                                 BIT11
#define   B_EFI_MSR_FIXED_MTRR_ENABLE                                  BIT10
#define   B_EFI_MSR_CACHE_MEMORY_TYPE                                  (BIT2 | BIT1 | BIT0)

#define EFI_MSR_VALID_MASK                     0xFFFFFFFFF
#define EFI_CACHE_VALID_ADDRESS                0xFFFFFF000
#define EFI_SMRR_CACHE_VALID_ADDRESS           0xFFFFF000
#define EFI_CACHE_VALID_EXTENDED_ADDRESS       0xFFFFFFFFFF000

// Leave one MTRR pairs for OS use
#define EFI_CACHE_NUM_VAR_MTRR_PAIRS_FOR_OS   1
#define EFI_CACHE_LAST_VARIABLE_MTRR_FOR_BIOS (EFI_MSR_CACHE_VARIABLE_MTRR_END) - \
        (EFI_CACHE_NUM_VAR_MTRR_PAIRS_FOR_OS * 2)

#define EFI_MSR_NO_EVICT_MODE                  0x000002E0
#define   B_EFI_MSR_NO_EVICT_MODE_RUN                                  BIT1
#define   B_EFI_MSR_NO_EVICT_MODE_SETUP                                BIT0

#define EFI_MSR_IA32_MC0_CTL                   0x00000400
#define EFI_MSR_IA32_MC0_STATUS                0x00000401
#define EFI_MSR_IA32_MC0_ADDR                  0x00000402
#define EFI_MSR_IA32_MC0_MISC                  0x00000403
#define EFI_MSR_IA32_MC8_CTL                   (EFI_IA32_MC0_CTL + (8*4))
#define EFI_MSR_IA32_MC8_STATUS                (EFI_IA32_MC0_STATUS + (8*4))

#define EFI_MSR_EXT_XAPIC_LOGICAL_APIC_ID      0x00000802
#define EFI_MSR_EXT_XAPIC_VERSION              0x00000803
#define   B_EFI_MSR_EXT_XAPIC_VERSION_VERSION                          0xFF
#define EFI_MSR_EXT_XAPIC_ICR                  0x00000830
#define EFI_MSR_EXT_XAPIC_LVT_LINT0            0x00000835
#define EFI_MSR_EXT_XAPIC_LVT_LINT1            0x00000836

#define EFI_MSR_EBL_CR_POWERON                 0x0000002A
#define   B_EFI_MSR_EBL_CR_POWERON_EXECUTEBIST                         BIT9

#ifndef EFI_MSR_IA32_FEATURE_CONTROL
#define EFI_MSR_IA32_FEATURE_CONTROL           0x0000003A
#endif
#define   B_EFI_MSR_IA32_FEATURE_CONTROL_C_STATE_SMI                   BIT16

#ifndef EFI_MSR_PMG_CST_CONFIG
#define EFI_MSR_PMG_CST_CONFIG                 0x000000E2
#endif
#define   B_EFI_MSR_PMG_CST_CONFIG_L2_SHRINK_THRESHOLD_MASK            (BIT20 | BIT19 | BIT18 | BIT17 | BIT16)
#define   B_EFI_MSR_PMG_CST_CONFIG_EIST_HARWARE_COORDINATION           BIT11
#define   B_EFI_MSR_PMG_CST_CONFIG_STPGNT_ISSUE                        BIT9
#define   B_EFI_MSR_PMG_CST_CONFIG_C4VID_DISABLE                       BIT7
#define   B_EFI_MSR_PMG_CST_CONFIG_CSM_TRIGGER_MASK                    (BIT6 | BIT5 | BIT4)

#define EFI_MSR_PMG_IO_BASE_ADDR               0x000000E3
#define   B_EFI_MSR_PMG_IO_BASE_ADDR_PMB1_MASK                         0xFFFF0000
#define   B_EFI_MSR_PMG_IO_BASE_ADDR_PMB0_MASK                         0x0000FFFF

#define   B_EFI_MSR_PMG_IO_CAPTURE_ADDR_CST_RANGE                      (BIT22 | BIT21 | BIT20 | BIT19 | BIT18 | BIT17 | BIT16)

#define EFI_BBL_CR_CTL3                        0x0000011E
#define   B_EFI_BBL_CR_CTL3_L2_WAY_SHRINK_MIN                          (BIT31 | BIT30)
#define   B_EFI_BBL_CR_CTL3_L2_WAY_SHRINK_RATE                         (BIT29 | BIT28)
#define   B_EFI_BBL_CR_CTL3_L2_WAY_CHUNK_SIZE                          (BIT27 | BIT26 | BIT25)
#define   B_EFI_BBL_CR_CTL3_L2_REDUCTION_CONF_LOCK                     BIT24
#define   B_EFI_BBL_CR_CTL3_L2_NOT_PRESENT                             BIT23
#define   B_EFI_BBL_CR_CTL3_SIZE_OF_WAY                                (BIT17 | BIT16 | BIT15 | BIT14 | BIT13)
#define   B_EFI_BBL_CR_CTL3_L2_ENABLED                                 BIT8
#define   B_EFI_BBL_CR_CTL3_NUMBER_OF_WAYS                             (BIT4 | BIT3 | BIT2 | BIT1)
#define   B_EFI_BBL_CR_CTL3_L2_HW_ENABLED                              BIT0

#define   B_EFI_MSR_CLOCK_FLEX_MAX_FLEX_RATIO_PINEVIEW                 (BIT12 | BIT11 | BIT10 | BIT9 | BIT8)
#define   B_EFI_MSR_CLOCK_FLEX_MAX_FLEX_VID                            (BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)

#ifndef EFI_MSR_IA32_PERF_STS
#define EFI_MSR_IA32_PERF_STS                  0x00000198
#endif

#define   B_EFI_MSR_IA32_PERF_STS_BUS_RATIO_BOOT                       (BIT60 | BIT59 | BIT58 | BIT57 | BIT56)
#define   B_EFI_MSR_IA32_PERF_STS_VID_BOOT                             (BIT53 | BIT52 | BIT51 | BIT50 | BIT49 | BIT48)
#define   B_EFI_MSR_IA32_PERF_STS_BUS_RATIO_MAX                        (BIT44 | BIT43 | BIT42 | BIT41 | BIT40)
#define   B_EFI_MSR_IA32_PERF_STS_VID_MAX                              (BIT37 | BIT36 | BIT35 | BIT34 | BIT33 | BIT32)
#define   N_EFI_MSR_IA32_PERF_STS_BUT_RATIO_MIN                        24
#define   B_EFI_MSR_IA32_PERF_STS_BUT_RATIO_MIN_MASK                   (BIT28 | BIT27 | BIT26 | BIT25 | BIT24)
#define   B_EFI_MSR_IA32_PERF_STS_TS                                   BIT21
#define   B_EFI_MSR_IA32_PERF_STS_CMD_SEEK                             BIT20
#define   B_EFI_MSR_IA32_PERF_STS_THERM_THROT                          BIT19
#define   B_EFI_MSR_IA32_PERF_STS_TT                                   BIT18
#define   B_EFI_MSR_IA32_PERF_STS_VIP                                  BIT17
#define   B_EFI_MSR_IA32_PERF_STS_FIP                                  BIT16
#define   B_EFI_MSR_IA32_PERF_STS_BUS_RATIO_STS                        (BIT12 | BIT11 | BIT10 | BIT9 | BIT8)
#define   N_EFI_MSR_IA32_PERF_STS_BUS_RATIO_STS                        8
#define   B_EFI_MSR_IA32_PERF_STS_VID_STS                              (BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)

#ifndef EFI_MSR_IA32_PERF_CTRL
#define EFI_MSR_IA32_PERF_CTRL                 0x00000199
#endif


#define   B_EFI_MSR_IA32_PERF_CTRL_BUS_RATIO_SEL                       (BIT12 | BIT11 | BIT10 | BIT9 | BIT8)
#define   B_EFI_MSR_IA32_PERF_CTRL_VID_SEL                             (BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)

#define EFI_MSR_THERM2_CTL                     0x0000019D
#define   B_EFI_MSR_THERM2_CTL_BUS_RATIO_THROT                         (BIT12 | BIT11 | BIT10 | BIT9 | BIT8)
#define   B_EFI_MSR_THERM2_CTL_VID_THROT                               (BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)

#ifndef EFI_MSR_IA32_MISC_ENABLE
#define EFI_MSR_IA32_MISC_ENABLE               0x000001A0
#endif
#define   B_EFI_MSR_IA32_MISC_ENABLE_HARD_C4E_EN                       BIT33
#define   B_EFI_MSR_IA32_MISC_ENABLE_C4E_EN                            BIT32
#define   B_EFI_MSR_IA32_MISC_ENABLE_C2E_EN                            BIT26
#define   B_EFI_MSR_IA32_MISC_ENABLE_C1E_EN                            BIT25
#define   B_EFI_MSR_IA32_MISC_ENABLE_FORCEPR_INPUT_EN                  BIT21
#define   B_EFI_MSR_IA32_MISC_ENABLE_EIST_LOCK                         BIT20
#define   B_EFI_MSR_IA32_MISC_ENABLE_BI_DIRECTIONAL_PROCHOT_EN         BIT17
#define   B_EFI_MSR_IA32_MISC_ENABLE_TM2_EN                            BIT13
#define   B_EFI_MSR_IA32_MISC_ENABLE_FERR_MULTIPLEXING_EN              BIT10

#ifndef EFI_MSR_PIC_SENS_CFG
#define EFI_MSR_PIC_SENS_CFG                   0x000001AA
#endif
#define   B_EFI_MSR_PIC_SENS_CFG_LOCK_THERMAL_INT                      BIT22
#define   B_EFI_MSR_PIC_SENS_CFG_LOCK_TM1                              BIT21
#define   B_EFI_MSR_PIC_SENS_CFG_TM1_ON_TM2                            BIT20
#define   B_EFI_MSR_PIC_SENS_CFG_CONVERSION_TIME                       (BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8)
#define   B_EFI_MSR_PIC_SENS_CFG_BYPASS_FILTER                         BIT4

#ifndef MSR_PLATFORM_INFO
#define MSR_PLATFORM_INFO                       0xCE
#endif
#define   B_PLATFORM_INFO_TIMED_MWAIT_SUPPORTED                        BIT37
#define   B_PLATFORM_INFO_MAX_NON_TURBO_LIM_RATIO                      (BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8)
#define   N_PLATFORM_INFO_MAX_NON_TURBO_LIM_RATIO                      8
#define   B_PLATFORM_INFO_MAX_EFFICIENCY_RATIO                         (BIT47 | BIT46 | BIT45 | BIT44 | BIT43 | BIT42 | BIT41 | BIT40)
#define   N_PLATFORM_INFO_MAX_EFFICIENCY_RATIO                         40

#ifndef MSR_IACORE_RATIOS
#define MAX_RATIO_4C_OFFSET                   24
#define MAX_RATIO_3C_OFFSET                   16
#define MAX_RATIO_2C_OFFSET                   8
#define MAX_RATIO_1C_OFFSET                   0
#define MAX_RATIO_1C_MASK                     0x000000ff
#define MAX_RATIO_2C_MASK                     0x0000ff00
#define MAX_RATIO_3C_MASK                     0x00ff0000
#define MAX_RATIO_4C_MASK                     0xff000000
#define MAX_VID_4C_OFFSET                     24
#define MAX_VID_3C_OFFSET                     16
#define MAX_VID_2C_OFFSET                     8
#define MAX_VID_1C_OFFSET                     0
#define MAX_RATIO_1C_MASK                     0x000000ff
#define MAX_RATIO_2C_MASK                     0x0000ff00
#define MAX_RATIO_3C_MASK                     0x00ff0000
#define MAX_RATIO_4C_MASK                     0xff000000

#define MSR_PM_CFG_CTRL                       0xE2
#define C0_SUB_STATES_MASK                    0x0000000f
#define C1_SUB_STATES_MASK                    0x000000f0
#define C2_SUB_STATES_MASK                    0x00000f00
#define C3_SUB_STATES_MASK                    0x0000f000
#define C4_SUB_STATES_MASK                    0x000f0000
#define C5_SUB_STATES_MASK                    0x00f00000
#define C6_SUB_STATES_MASK                    0x0f000000
#define C7_SUB_STATES_MASK                    0xf0000000
#define CSTATE_LIMIT_MASK                     0xF
#define CSTATE_LIMIT_NO_LIMIT                 0x0
#define CSTATE_LIMIT_C1                       0x0
#define CSTATE_LIMIT_C3                       0x0
#define CSTATE_LIMIT_C6                       0x1
#define CSTATE_LIMIT_C7                       0x1
#define CSTATE_LIMIT_C7S                      0x1
#define CSTATE_LIMIT_C8                       0x2
#define CSTATE_LIMIT_C9                       0x2
#define CSTATE_LIMIT_C10                      0x2
#define B_TIMED_MWAIT_ENABLE                  BIT31
#define B_SINGLE_PCTL                         (1 << 11)
//
// Turbo
//
#define MAX_RATIO_LIMIT_4C_OFFSET    24
#define MAX_RATIO_LIMIT_3C_OFFSET    16
#define MAX_RATIO_LIMIT_2C_OFFSET    8
#define MAX_RATIO_LIMIT_1C_OFFSET    0
#define MAX_RATIO_LIMIT_MASK1      0x000000ff
#define MAX_RATIO_LIMIT_MASK2      0x0000ff00
#define MAX_RATIO_LIMIT_MASK3      0x00ff0000
#define MAX_RATIO_LIMIT_MASK4      0xff000000
#define TURBO_DISABLE_MASK            ((UINT64)1 << 38)
#define TURBO_MODE_DISABLE_BIT        38
//
// P-State
//
#define MSR_CLOCK_FLEX_MAX              MSR_FLEX_RATIO
#define ENABLE_FLEX                   (1 << 16)
#define RATIO_FLEX_BYTE_MASK          0xFF                  // Bits 15:8
#define RATIO_FLEX_EN_MASK              0x0000000000010000      // Clear all but bit 16
#define MAX_NON_TURBO_MASK              0x000000000000FF00
#define MAX_EFFICIENCY_MASK             0x0000FF0000000000
//
// ACPI P-State Coordination Type
//
#define PSD_SW_ALL                      0xfc
#define PSD_SW_ANY                      0xfd
#define PSD_HW_ALL                      0xfe
#endif
//-----------------------------------------------------------------------------
// Thermal Management Registers
//-----------------------------------------------------------------------------
#define EFI_MSR_IA32_CR_THERM_INTERRUPT             0x19b
#define EFI_MSR_IA32_CR_THERM_STATUS                0x19c
#define EFI_MSR_CPU_THERM_TEMPERATURE               0x1a2


#define EFI_MSR_PKGC6_IRTL                          0x60B
#define EFI_MSR_PKGC7_IRTL                          0x60C
#define EFI_MSR_TURBO_POWER_LIMIT                   0x610
#define EFI_MSR_PACKAGE_POWER_SKU                   0x614
#define STOP_ENABLE                                (1 << 19)
#define CURRENT_LIMIT_MASK                          0x1FFF      // Bits 12:0

#define EFI_MSR_PRIMARY_PLANE_TURBO_POWER_LIMIT     0x638
#define EFI_MSR_SECONDARY_PLANE_TURBO_POWER_LIMIT   0x640
#define PLANE_POWER_LIMIT_MASK                      (0x7FFF)
#define PLANE_POWER_LIMIT_ENABLE                    (1 << 15)
#define PLANE_POWER_LIMIT_TIME_MASK                 (0x7F)
#define PLANE_POWER_LIMIT_TIME_OFFSET               17
#define PLANE_POWER_LIMIT_LOCK                      (1 << 31)


#pragma pack(1)
///
/// MSR_REGISTER definition as a Union of QWORDS, DWORDS and BYTES
///
typedef union _MSR_REGISTER {
  UINT64  Qword;       ///< MSR value in 64 bit QWORD.

  ///
  /// MSR value represented in two DWORDS
  ///
  struct _DWORDS {
    UINT32  Low;       ///< Lower DWORD of the 64 bit MSR value.
    UINT32  High;      ///< Higher DWORD of the 64 bit MSR value.
  } Dwords;

  ///
  /// MSR value represented in eight bytes.
  ///
  struct _BYTES {
    UINT8 FirstByte;   ///< First byte of the 64 bit MSR value.
    UINT8 SecondByte;  ///< Second byte of the 64 bit MSR value.
    UINT8 ThirdByte;   ///< Third byte of the 64 bit MSR value.
    UINT8 FouthByte;   ///< Fourth byte of the 64 bit MSR value.
    UINT8 FifthByte;   ///< Fifth byte of the 64 bit MSR value.
    UINT8 SixthByte;   ///< Sixth byte of the 64 bit MSR value.
    UINT8 SeventhByte; ///< Seventh byte of the 64 bit MSR value.
    UINT8 EighthByte;  ///< Eigth byte of the 64 bit MSR value.
  } Bytes;
} MSR_REGISTER;
#pragma pack()

#endif

