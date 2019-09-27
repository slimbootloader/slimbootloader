/** @file

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


#ifndef _ERRORLIST_H
#define _ERRORLIST_H

/*Categories definitions   Category type                            Description*/
#define CATEGORY_LIST_HELPER1\
    CATEGORY_DEFINE_HELPER(CATEGORY_SUCCESS = 0,                     "")\
    CATEGORY_DEFINE_HELPER(CATEGORY_INTERNAL_ERROR,                  "Internal tool execution error.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_WARNING,                         "Passed with warnings.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_UNSUPPORTED_OS,                  "The tool was executed on an unsupported OS.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_UNSUPPORTED_HW,                  "Tool isn't compatible with current HW or current HW environment isn't supported.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_MEMORY_ALLOCATION,               "Memory allocation error.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_SYSTEM_API_CALL_ERROR,           "System API returned unexpected error code.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_SYSTEM_API_NOT_FOUND,            "System library or API does not exist.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_IO_ERROR,                        "Failed to open, read, write or close used for program execution or user file.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_PERIPHERALS_VALIDATION_FAILURE,  "Failed to validate tool peripherals files (file does not exist, file signature validation failed).")\
    CATEGORY_DEFINE_HELPER(CATEGORY_ME_COMMUNICATION_FAILURE,        "Failed to communicate with ME device or invalid ME HW response.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_HECI_COMMUNICATION_FAILURE,      "HECI device API failure (possible open, IO, read, write, or close APIs).")\
    CATEGORY_DEFINE_HELPER(CATEGORY_INVALID_CLI_OPTIONS,             "Invalid CLI parameters.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_CSE_FILE_INVALID_INPUT_VALUE,    "Invalid update value provided for NVAR or FPF.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_CSE_FILE_READ_ERROR,             "NVAR or FPF read error.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_CSE_FILE_UPDATE_ERROR,           "NVAR or FPF write or update error.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_CSE_FILE_COMPARE_GENERAL_ERROR,  "NVAR or FPF compare error.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_PCI_ACCESS_ERROR,                "Unable to access PCI device with read/write error.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_OS_DEVICE_COMMUNICATION_FAILURE, "OS device not found or failed to perform program request.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_USER_INPUT_FILE_ERROR,           "User input file (xml, cfg or text) format or content error.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_SPI_ACCESS_ERROR,                "Unable to access SPI device with read/write error.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_SMBUS_ACCESS_ERROR,              "Unable to access SMBUS device with read/write error.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_SMBIOS_ACCESS_ERROR,             "Unable to access SMBIOS device with read/write error.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_TOOL_INITIALIZATION_ERROR,       "Failed to initialize the tool.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_XML_USER_FILE_ERROR,             "Error in parsing user XML file.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_KEYBOX_INVALID_RESPONSE,         "Keybox response header values are not as expected.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_KEYBOX_INVALID_STATUS,           "Invalid Keybox response status.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_MEMANUF_TEST_RUN_ERROR,          "MEManuf test failed to run.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_MEMANUF_TEST_RESULT_READ_ERROR,  "Cannot read MEManuf test result.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_MEMANUF_BIST_TEST_FAILED,        "MEManuf BIST test failed.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_MEMANUF_EOL_TEST_FAILED,         "MEManuf EOL test failed.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_MEMANUF_OPERATION_FAILED,        "MEManuf operation failed.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_MEINFO_FEATURE_NOT_FOUND,        "Cannot find MEInfo feature or feature is not supported.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_MEINFO_FEATURE_COMPARE_ERROR,    "MEInfo feature not equals to user value.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_MEINFO_FEATURE_EXECUTION_ERROR,  "MEInfo failed to execute specific feature.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_FPT_INVALID_SPI_CONF,            "Invalid FPT SPI devices configuration file.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_INVALID_USER_INPUT,              "Invalid user parameter for specified tool feature.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_FWU_FW_ERROR,                    "FW returned an error during the FWU process.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_FWU_RESTORE_POINT_ERROR,         "FWU Restore Point Image Failure.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_INVALID_PARTITION,               "Partition does not exist in the flash image.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_FWU_OPERATION_FAILED,            "FWU tool failed to execute the requested command.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_TOOL_FEATURE_EXECUTION_ERROR,    "Tool feature execution failed with general error.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_GENERAL_ERROR,                   "FW or general environment error.")\
    CATEGORY_DEFINE_HELPER(CATEGORY_ICC_FW_ERROR,                    "FW returned an error on ICC command.")\
    CATEGORY_DEFINE_HELPER(RESERVED_44,                              "")\
    CATEGORY_DEFINE_HELPER(ERROR_CATEGORIES_COUNT,                   "")

typedef enum _ErrorCategories
{
#ifdef CATEGORY_DEFINE_HELPER
#error "Rename CATEGORY_DEFINE_HELPER to name that not in use"
#endif
#define CATEGORY_DEFINE_HELPER(eenum, sstring)    eenum,
    CATEGORY_LIST_HELPER1
#undef CATEGORY_DEFINE_HELPER
} ErrorCategories;


/**
@file errorlist.h
@enum ErrorCodes
@brief This is a list of every error that can occur in any tool or common code. SUCCESS is at 0 WARNING is 1.
*/

/*
There are few ERROR_LIST_HELPERX macros
This is due to limitation of Watcom C compiler which isn't able to handle long macros
*/

// Enum _ErrorCodes value should not pass the value: (MAX_UINT32 - MAX_UINT8),
// since it's shifts left in BITS_PER_BYTE bits before displayed.
#define ERROR_LIST_HELPER1 /* 1-100 */\
    ERROR_DEFINE_HELPER (SUCCESS = 0,                                           CATEGORY_SUCCESS,                            "\n") \
    ERROR_DEFINE_HELPER (TOOL_COMMON_ERROR,                                     CATEGORY_GENERAL_ERROR,                      "") \
    ERROR_DEFINE_HELPER (PASSED_WITH_WARNING,                                   CATEGORY_WARNING,                            "") /* Passed With Warning ErrorLevel */ \
    ERROR_DEFINE_HELPER (INTERNAL_ERROR,                                        CATEGORY_INTERNAL_ERROR,                     "Internal Error. Unexpected error occurred.") \
    ERROR_DEFINE_HELPER (ERROR_UNSUPPORTED_OS,                                  CATEGORY_UNSUPPORTED_OS,                     "Unsupported OS.") \
    ERROR_DEFINE_HELPER (ERROR_MEMORY_ALLOC,                                    CATEGORY_MEMORY_ALLOCATION,                  "Memory allocation error occurred.") \
    ERROR_DEFINE_HELPER (SM_BIOS_ERROR_PROCADDR,                                CATEGORY_SMBIOS_ACCESS_ERROR,                "Error accessing the function \"GetSystemFirmwareTable\" from \"kernel32.dll\".") \
    ERROR_DEFINE_HELPER (SM_BIOS_ERROR_GENERAL,                                 CATEGORY_SMBIOS_ACCESS_ERROR,                "The function \"GetSystemFirmwareTable\" failed with Windows Error Code: %d.") \
    ERROR_DEFINE_HELPER (SM_BIOS_ERROR_PERMISSION,                              CATEGORY_SMBIOS_ACCESS_ERROR,                "Error accessing the \"kernel32.dll\".") \
    ERROR_DEFINE_HELPER (COMMIT_SVN_FAILED,                                     CATEGORY_TOOL_FEATURE_EXECUTION_ERROR,       "Commit Anti Rollback SVN failed.") \
    ERROR_DEFINE_HELPER (FILE_READ_ERROR,                                       CATEGORY_IO_ERROR,                           "Error occurred while reading the file.") \
    ERROR_DEFINE_HELPER (FILEIO_ERROR_GETTING_CWD,                              CATEGORY_IO_ERROR,                           "Error getting current working directory path.") \
    ERROR_DEFINE_HELPER (FILEIO_ERROR_GETTING_CWD_PERMISSIONS,                  CATEGORY_IO_ERROR,                           "Error getting current working directory permission:") \
    ERROR_DEFINE_HELPER (FILEIO_ERROR_OPEN_FILE,                                CATEGORY_IO_ERROR,                           "An unknown error occurred while opening the file.") \
    ERROR_DEFINE_HELPER (FILEIO_ERROR_SEEK,                                     CATEGORY_IO_ERROR,                           "An unknown error occurred while working with the file.") \
    ERROR_DEFINE_HELPER (FILEIO_ERROR_WRITE,                                    CATEGORY_IO_ERROR,                           "Error occurred while writing to the file.") \
    ERROR_DEFINE_HELPER (ERROR_FILE_READ_SIGNATURE,                             CATEGORY_PERIPHERALS_VALIDATION_FAILURE,     "Error while trying to read the signature of the file \"%S\".") \
    ERROR_DEFINE_HELPER (ERROR_FILE_NOT_INTEL_SIGNED,                           CATEGORY_PERIPHERALS_VALIDATION_FAILURE,     "The file \"%s\", is not signed by Intel(R) Embedded Subsystems and IP Blocks Group.") \
    ERROR_DEFINE_HELPER (ERROR_FILE_CERT_READ,                                  CATEGORY_PERIPHERALS_VALIDATION_FAILURE,     "Invalid certificate information residing in file \"%s\".") \
    ERROR_DEFINE_HELPER (FAILED_TO_WRITE_TO_IO_PORT,                            CATEGORY_OS_DEVICE_COMMUNICATION_FAILURE,    "Failed to write 0x%02X to IO Port 0x%04X.") \
    ERROR_DEFINE_HELPER (HECI_STATUS_LOCATE_DEVICE_ERROR,                       CATEGORY_ME_COMMUNICATION_FAILURE,           "Cannot locate ME device.") \
    ERROR_DEFINE_HELPER (HECI_STATUS_WRITE_REGISTER_ERROR,                      CATEGORY_HECI_COMMUNICATION_FAILURE,         "Write register failure.") \
    ERROR_DEFINE_HELPER (HECI_STATUS_BUFFER_OVEREFLOW_ERROR,                    CATEGORY_HECI_COMMUNICATION_FAILURE,         "Circular buffer overflow.") \
    ERROR_DEFINE_HELPER (HECI_STATUS_MSG_TRANSMISSION_ERROR,                    CATEGORY_HECI_COMMUNICATION_FAILURE,         "Communication error between application and Intel(R) ME module.") \
    ERROR_DEFINE_HELPER (HECI_STATUS_VERSION_MISMATCH,                          CATEGORY_HECI_COMMUNICATION_FAILURE,         "Unsupported HECI bus message protocol version.") \
    ERROR_DEFINE_HELPER (HECI_STATUS_TIMEOUT_ERROR,                             CATEGORY_HECI_COMMUNICATION_FAILURE,         "HECI Timeout.") \
    ERROR_DEFINE_HELPER (HECI_STATUS_UNEXPECTED_RESPONSE,                       CATEGORY_HECI_COMMUNICATION_FAILURE,         "Unexpected result in command response.") \
    ERROR_DEFINE_HELPER (HECI_STATUS_CANNOT_FOUND_HOST_CLIENT,                  CATEGORY_HECI_COMMUNICATION_FAILURE,         "Cannot find host client.") \
    ERROR_DEFINE_HELPER (HECI_STATUS_CANNOT_FOUND_ME_CLIENT,                    CATEGORY_HECI_COMMUNICATION_FAILURE,         "Cannot find ME client.") \
    ERROR_DEFINE_HELPER (HECI_STATUS_CANNOT_DISCONNECT,                         CATEGORY_HECI_COMMUNICATION_FAILURE,         "Failure occurred during ME disconnect.") \
    ERROR_DEFINE_HELPER (HECI_STATUS_CLIENT_ALREADY_CONNECTED,                  CATEGORY_HECI_COMMUNICATION_FAILURE,         "Client already connected.") \
    ERROR_DEFINE_HELPER (HECI_STATUS_NO_FREE_CONNECTION,                        CATEGORY_HECI_COMMUNICATION_FAILURE,         "No free connection available.") \
    ERROR_DEFINE_HELPER (HECI_STATUS_FLOW_CONTROL_ERROR,                        CATEGORY_HECI_COMMUNICATION_FAILURE,         "Flow control error.") \
    ERROR_DEFINE_HELPER (HECI_STATUS_NO_MESSAGE,                                CATEGORY_HECI_COMMUNICATION_FAILURE,         "No message.") \
    ERROR_DEFINE_HELPER (HECI_STATUS_BUFFER_TOO_LARGE,                          CATEGORY_HECI_COMMUNICATION_FAILURE,         "Buffer size is too large.") \
    ERROR_DEFINE_HELPER (HECI_STATUS_BUFFER_TOO_SMALL,                          CATEGORY_HECI_COMMUNICATION_FAILURE,         "Buffer is too small.") \
    ERROR_DEFINE_HELPER (CERTIFICATE_NAME_TOO_LONG,                             CATEGORY_CSE_FILE_INVALID_INPUT_VALUE,       "%s is too long.") \
    ERROR_DEFINE_HELPER (CLI_ERROR_INVALID_COMMAND_ARGUMENT,                    CATEGORY_INVALID_CLI_OPTIONS,                "Invalid command line option(s).") \
    ERROR_DEFINE_HELPER (CLI_ERROR_INVALID_OPTION,                              CATEGORY_INVALID_CLI_OPTIONS,                "The following Parameter is not a valid option: %s.") \
    ERROR_DEFINE_HELPER (LPC_DID_ERROR_PCH_NOT_SUPPORTED,                       CATEGORY_UNSUPPORTED_HW,                     "PCH is not supported.") \
    ERROR_DEFINE_HELPER (SAFE_FUNC_ERROR_INVALID_SIZE,                          CATEGORY_INTERNAL_ERROR,                     "Internal Error (Safe function wrapper error: Invalid size).") \
    ERROR_DEFINE_HELPER (SAFE_FUNC_ERROR_COMPOSE_STR_FROM_LIST,                 CATEGORY_INTERNAL_ERROR,                     "Internal Error (Safe function wrapper error: compose string from list).") \
    ERROR_DEFINE_HELPER (SAFE_FUNC_ERROR_COMPOSE_STR,                           CATEGORY_INTERNAL_ERROR,                     "Internal Error (Safe function wrapper error: compose string).") \
    ERROR_DEFINE_HELPER (SAFE_FUNC_ERROR_MEMNCPY,                               CATEGORY_INTERNAL_ERROR,                     "Internal Error (Safe function wrapper error: memncpy).") \
    ERROR_DEFINE_HELPER (SAFE_FUNC_ERROR_STRNCPY,                               CATEGORY_INTERNAL_ERROR,                     "Internal Error (Safe function wrapper error: strncpy).") \
    ERROR_DEFINE_HELPER (SAFE_FUNC_ERROR_STRNCAT,                               CATEGORY_INTERNAL_ERROR,                     "Internal Error (Safe function wrapper error: strncat).") \
    ERROR_DEFINE_HELPER (SAFE_FUNC_ERROR_TOKEN_STR,                             CATEGORY_INTERNAL_ERROR,                     "Internal Error (Safe function wrapper error: strtok).") \
    ERROR_DEFINE_HELPER (DISPLAY_ERROR_PRINTF,                                  CATEGORY_INTERNAL_ERROR,                     "Printf function failed.") \
    ERROR_DEFINE_HELPER (NVAR_SUPPORT_FAILED_VAR,                               CATEGORY_CSE_FILE_READ_ERROR,                "Failed getting variable \"%s\" value.") \
    ERROR_DEFINE_HELPER (NVAR_SKU_DEP_SUPPORT_FAILED_VAR,                       CATEGORY_CSE_FILE_READ_ERROR,                "The variable \"%s\" is supported on Corporate SKU only.") \
    ERROR_DEFINE_HELPER (PLAT_INFO_NO_MATCHED_LOCL,                             CATEGORY_INTERNAL_ERROR,                     "Unable to find matching LOCL.") \
    ERROR_DEFINE_HELPER (CANNOT_READ_PCI,                                       CATEGORY_PCI_ACCESS_ERROR,                   "Could not access PCI device.") \
    ERROR_DEFINE_HELPER (OS_STATUS_LOAD_LIB_FAILURE,                            CATEGORY_SYSTEM_API_NOT_FOUND,               "Unable to load library.") \
    ERROR_DEFINE_HELPER (OS_STATUS_NOT_PERMITTED,                               CATEGORY_SYSTEM_API_CALL_ERROR,              "Unable to change permission.") \
    ERROR_DEFINE_HELPER (OS_STATUS_PERMISSION_FAILURE,                          CATEGORY_SYSTEM_API_CALL_ERROR,              "Unable to perform request due to permission failure.") \
    ERROR_DEFINE_HELPER (DEVICE_NOT_FOUND,                                      CATEGORY_OS_DEVICE_COMMUNICATION_FAILURE,    "Cannot find requested device.") \
    ERROR_DEFINE_HELPER (OS_STATUS_INTEGRITY_CHECK_FAILED,                      CATEGORY_OS_DEVICE_COMMUNICATION_FAILURE,    "Unable to perform CreateFile.") \
    ERROR_DEFINE_HELPER (FPF_COMPARE_FAILURE,                                   CATEGORY_CSE_FILE_COMPARE_GENERAL_ERROR,     "The FPF compare failed.") \
    ERROR_DEFINE_HELPER (INVALID_CSE_FILE_NAME,                                 CATEGORY_CSE_FILE_INVALID_INPUT_VALUE,       "The CSE File Component requested, \"%s\", is not valid for this operation.") \
    ERROR_DEFINE_HELPER (INVALID_CSE_FILE_ID,                                   CATEGORY_CSE_FILE_INVALID_INPUT_VALUE,       "The CSE File Component requested, ID is not valid\n" \
                                                                                                                             "for this operation.") \
    ERROR_DEFINE_HELPER (FPT_INVALID_CONFIG_FORMAT,                             CATEGORY_USER_INPUT_FILE_ERROR,              "Failed to read FPT NVARs config file. %s.") \
    ERROR_DEFINE_HELPER (FW_REGISTER_READ_ERROR,                                CATEGORY_ME_COMMUNICATION_FAILURE,           "Fail to read FW Status Register value.") \
    ERROR_DEFINE_HELPER (TOOL_ERROR_CREATE_LOG_FILE,                            CATEGORY_IO_ERROR,                           "Fail to create verbose log file.") \
    ERROR_DEFINE_HELPER (ERROR_UNKNOWN_UNSUPPORTED_HW,                          CATEGORY_UNSUPPORTED_HW,                     "Unknown or unsupported hardware platform. %s") \
    ERROR_DEFINE_HELPER (FAILED_TO_INIT_SPI,                                    CATEGORY_SPI_ACCESS_ERROR,                   "Failed to initialize SPI interface.") \
    ERROR_DEFINE_HELPER (FPT_VAR_UPDATE_FAILED,                                 CATEGORY_CSE_FILE_UPDATE_ERROR,              "Could not update [%s].") \
    ERROR_DEFINE_HELPER (CANNOT_UPDATE_FPT_VAR,                                 CATEGORY_CSE_FILE_UPDATE_ERROR,              "Cannot update %s. Invalid data length.") \
    ERROR_DEFINE_HELPER (MEINFO_ERROR_PARAM_NOT_FOUND,                          CATEGORY_MEINFO_FEATURE_NOT_FOUND,           "Feature not found.") \
    ERROR_DEFINE_HELPER (MEINFO_ERROR_PARAM_INVALID,                            CATEGORY_MEINFO_FEATURE_NOT_FOUND,           "Feature not available.") \
    ERROR_DEFINE_HELPER (MEINFO_ERROR_PARAM_NOT_EQUAL,                          CATEGORY_MEINFO_FEATURE_COMPARE_ERROR,       "%s actual value is - %s.") \
    ERROR_DEFINE_HELPER (ERROR_FW_STATUS_FAILURE,                               CATEGORY_MEMANUF_EOL_TEST_FAILED,            "FW status test failed.") \
    ERROR_DEFINE_HELPER (ERROR_BG_FW_STATUS_FAILURE,                            CATEGORY_INTERNAL_ERROR,                     "Boot Guard status test failed.") \
    ERROR_DEFINE_HELPER (FPT_CERT_INVALID_FILE,                                 CATEGORY_USER_INPUT_FILE_ERROR,              "Parameter \"%s\" - %s.") \
    ERROR_DEFINE_HELPER (FPT_CERT_UPDATE_MISSING_COMPONENTS,                    CATEGORY_USER_INPUT_FILE_ERROR,              "The value of \'%s\' is missing.") \
    ERROR_DEFINE_HELPER (ERROR_TOOL_INIT,                                       CATEGORY_TOOL_INITIALIZATION_ERROR,          "Failed to communicate with CSME.\n" \
                                                                                                                             "This tool must be run from a privileged account (administrator/root).") \
    ERROR_DEFINE_HELPER (FPT_MASTER_ACCESS_VALUE_INVALID,                       CATEGORY_USER_INPUT_FILE_ERROR,              "Master Access config file value for \"%s\" format is invalid.") \
    ERROR_DEFINE_HELPER (MEINFO_ERROR_PARAM_RETRIEVE,                           CATEGORY_MEINFO_FEATURE_NOT_FOUND,           "Failed to retrieve feature.") \
    ERROR_DEFINE_HELPER (FPT_MASTER_ACCESS_VALUE_EXCEED,                        CATEGORY_USER_INPUT_FILE_ERROR,              "Master Access config file value for \"%s\" exceed maximum allowed value.") \
    ERROR_DEFINE_HELPER (MEINFO_ERROR_FIT_VER,                                  CATEGORY_MEINFO_FEATURE_EXECUTION_ERROR,     "Failed to retrieve Intel (R) FIT version.") \
    ERROR_DEFINE_HELPER (MEINFO_ERROR_INT_VER,                                  CATEGORY_MEINFO_FEATURE_EXECUTION_ERROR,     "Failed to retrieve Intel (R) Internal Build Version.") \
    ERROR_DEFINE_HELPER (FPT_MASTER_DUPLICATE_REGION,                           CATEGORY_USER_INPUT_FILE_ERROR,              "Ambiguous Master Access value. Master Access config file region \"%s\"\n" \
                                                                                                                             "defined more than once.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_OPT_FAILED,                              CATEGORY_MEMANUF_OPERATION_FAILED,           "MEManuf Operation Failed.") \
    ERROR_DEFINE_HELPER (FPT_MASTER_ACCESS_INVALID_ACCESS_NODE,                 CATEGORY_USER_INPUT_FILE_ERROR,              "Invalid Access node name in Master Access configuration file.") \
    ERROR_DEFINE_HELPER (FPT_MASTER_ACCESS_INVALID_REQUIRED_NODE,               CATEGORY_USER_INPUT_FILE_ERROR,              "Invalid RequiredValue node name in Master Access configuration file.") \
    ERROR_DEFINE_HELPER (NVAR_MAC_WRONG_SERVER_ADDRESS,                         CATEGORY_CSE_FILE_UPDATE_ERROR,              "Invalid server address.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_BIST_NOTSTARTED,                         CATEGORY_MEMANUF_TEST_RUN_ERROR,             "Intel(R) test failed to start, error 0x%X returned.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_BIST_NOTFINISHED,                        CATEGORY_MEMANUF_TEST_RUN_ERROR,             "Intel(R) test timeout (exceeded 30 seconds).") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_BIST_RUNNING_RETRY,                      CATEGORY_MEMANUF_TEST_RUN_ERROR,             "Intel(R) ME test is currently running, try again later.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_CFGGEN,                                  CATEGORY_TOOL_FEATURE_EXECUTION_ERROR,       "MEManuf EOL & BIST config file generation failed.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_NOM3BIST_ON_FLASH,                       CATEGORY_MEMANUF_TEST_RESULT_READ_ERROR,     "M3 results are not available from SPI. Please run -test option to perform the BIST test.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_M3_BIST_CONFIGURATION,                   CATEGORY_MEMANUF_TEST_RESULT_READ_ERROR,     "Could not read M3 results from SPI.") \
    ERROR_DEFINE_HELPER (ERROR_SMBUS_READ_BYTE_STATUS_NOT_READY,                CATEGORY_SMBUS_ACCESS_ERROR,                 "SMBus hardware is not ready.") \
    ERROR_DEFINE_HELPER (ERROR_SMBUS_READ_BYTE_STATUS_FAILURE,                  CATEGORY_SMBUS_ACCESS_ERROR,                 "Internal error - SMBus Read Byte PEC failure.") \
    ERROR_DEFINE_HELPER (ERROR_SMBUS_READ_BYTE_STATUS_TIMED_OUT,                CATEGORY_SMBUS_ACCESS_ERROR,                 "SMBus encountered time-out.") \
    ERROR_DEFINE_HELPER (SPI_SIGN_INVALID,                                      CATEGORY_TOOL_FEATURE_EXECUTION_ERROR,       "    Signature: invalid! No more information can be displayed.") \
    ERROR_DEFINE_HELPER (ERROR_M3_LIVE_HEAP_TEST_DO_NOT_MATCH,                  CATEGORY_INTERNAL_ERROR,                     "Internal error - Failed to match.") \
    ERROR_DEFINE_HELPER (ERROR_M3_LIVE_HEAP_TEST_OUT_OF_MEMORY,                 CATEGORY_INTERNAL_ERROR,                     "Internal error - Out of memory.") \
    ERROR_DEFINE_HELPER (M3_LIVE_HEAP_TEST_FAILED_GET_POWER_PACKAGE,            CATEGORY_INTERNAL_ERROR,                     "Internal error - Unable to get current PP.") \
    ERROR_DEFINE_HELPER (WLAN_POWER_WELL_FAILED_GET_TEST_RESULT,                CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Failed to retrieve test result from SPI.") \
    ERROR_DEFINE_HELPER (ERROR_WLAN_POWER_WELL_FAILED_GET_PACKAGE,              CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Failed to retrieve power package setting.") \

#define ERROR_LIST_HELPER2 /* 101-200 */\
    ERROR_DEFINE_HELPER (ERROR_WLAN_POWER_WELL_TEST_FAILED_GET_RULE,            CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Failed to retrieve power rule from SPI.") \
    ERROR_DEFINE_HELPER (ERROR_WLAN_POWER_WELL_TEST_CFG_INCORRECT,              CATEGORY_MEMANUF_BIST_TEST_FAILED,           "WLAN power well setting is set incorrectly.") \
    ERROR_DEFINE_HELPER (ERROR_CPU_MISSING_TEST_FAILED_GET_RESULT,              CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Failed to retrieve test result from SPI.") \
    ERROR_DEFINE_HELPER (ERROR_CPU_MISSING_TEST_FAILED_GET_PLAT_ATTR,           CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Internal error - Failed to retrieve Platform Attribute.") \
    ERROR_DEFINE_HELPER (ERROR_CPU_MISSING_TEST_GET_LOGIC_RULE,                 CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Failed to retrieve PROC_MISSING NVAR setting.") \
    ERROR_DEFINE_HELPER (ERROR_CPU_MISSING_TEST_LOGIC_INCORRECT,                CATEGORY_MEMANUF_BIST_TEST_FAILED,           "PROC_MISSING NVAR setting is set incorrectly.") \
    ERROR_DEFINE_HELPER (ERROR_GET_PASSWORD_FAILED,                             CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Failed to retrieve password from SPI.") \
    ERROR_DEFINE_HELPER (ERROR_PASSWORD_LENGTH_IS_INCORRECT,                    CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Internal error - Password length is incorrect.") \
    ERROR_DEFINE_HELPER (ERROR_CALL_IsLocalPwdModified_FAILED,                  CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Internal error - Modified local password.") \
    ERROR_DEFINE_HELPER (ERROR_PASSWORD_INVALID,                                CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Internal error - Invalid password.") \
    ERROR_DEFINE_HELPER (ERROR_SB_SELF_TEST_FAILED,                             CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Boot Guard Self Test Failed.") \
    ERROR_DEFINE_HELPER (ERROR_LAN_HW_NOT_READY,                                CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Intel integrated LAN setting is set incorrectly.") \
    ERROR_DEFINE_HELPER (ERROR_LAN_NIC_FAILURE,                                 CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Intel LAN Connected Device (PHY) physical connectivity error with ME.") \
    ERROR_DEFINE_HELPER (ERROR_ILLEGAL_LENGTH,                                  CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Internal error - Illegal data length.") \
    ERROR_DEFINE_HELPER (ERROR_ILLEGAL_VALUE,                                   CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Internal error - Illegal data value.") \
    ERROR_DEFINE_HELPER (ERROR_EHBC_STATE_FLASH_READ_ERROR,                     CATEGORY_MEMANUF_BIST_TEST_FAILED,           "EHBC State Test Failed - Error while reading data from flash.") \
    ERROR_DEFINE_HELPER (ERROR_EHBC_STATE_PRIV_LEVEL_MISMATCH,                  CATEGORY_MEMANUF_BIST_TEST_FAILED,           "EHBC State Test Failed - Contradiction with current Privacy Level.") \
    ERROR_DEFINE_HELPER (ERROR_WLAN_CODE_INVALID,                               CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Current WLAN does not match micro-code, please update WLAN micro-code in FW.") \
    ERROR_DEFINE_HELPER (ERROR_WLAN_NIC_FAILURE,                                CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Communication with WLAN device failed.") \
    ERROR_DEFINE_HELPER (ERROR_INVALID_HASH_LENGTH,                             CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Length of OEM Customizable Certificate Friendly Name setting is set incorrectly.") \
    ERROR_DEFINE_HELPER (ERROR_INVALID_HASH_VALUE,                              CATEGORY_MEMANUF_BIST_TEST_FAILED,           "OEM Customizable Certificate Stream setting is set incorrectly.") \
    ERROR_DEFINE_HELPER (ERROR_INVALID_HASH_ALGORITHM,                          CATEGORY_MEMANUF_BIST_TEST_FAILED,           "OEM Customizable Certificate Hash Algorithm setting is set incorrectly.") \
    ERROR_DEFINE_HELPER (ERROR_INVALID_NAME_LENGTH,                             CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Length of OEM Customizable Certificate Stream is set incorrectly.") \
    ERROR_DEFINE_HELPER (ERROR_UNABLE_TO_COMPRESS,                              CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Internal error - Unable to compress.") \
    ERROR_DEFINE_HELPER (ERROR_INVALID_COMPRESSION_VAL,                         CATEGORY_MEMANUF_BIST_TEST_FAILED,           "The compressed data is incorrect.") \
    ERROR_DEFINE_HELPER (ERROR_USBR_NVAR_WRONG_CONFIG,                          CATEGORY_MEMANUF_BIST_TEST_FAILED,           "USBr EHCI 1 Enabled and/or USBr EHCI 2 Enabled setting is set incorrectly.") \
    ERROR_DEFINE_HELPER (ERROR_USBR_IN_USE,                                     CATEGORY_MEMANUF_BIST_TEST_FAILED,           "KVM device is already in use by other components.") \
    ERROR_DEFINE_HELPER (ERROR_EC_UNABLE_TO_GET_SOURCE,                         CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Failed to retrieve power source.") \
    ERROR_DEFINE_HELPER (ERROR_EC_POWER_NOT_AC,                                 CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Power source is not AC.") \
    ERROR_DEFINE_HELPER (ERROR_POWER_WELL_LAN_INCORRECT,                        CATEGORY_MEMANUF_BIST_TEST_FAILED,           "LAN power well setting is set incorrectly.") \
    ERROR_DEFINE_HELPER (ERROR_POWER_WELL_WLAN_INCORRECT,                       CATEGORY_MEMANUF_BIST_TEST_FAILED,           "WLAN power well setting is set incorrectly.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_EOL_UUIDCHECK_ALLZERO,                   CATEGORY_MEMANUF_EOL_TEST_FAILED,            "System UUID actual value is all 0x00.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_EOL_UUIDCHECK_ALLF,                      CATEGORY_MEMANUF_EOL_TEST_FAILED,            "System UUID actual value is all 0xFF.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_EOL_MANUFJMPCHECK,                       CATEGORY_MEMANUF_EOL_TEST_FAILED,            "Security Descriptor Override Strap (SDO) is enabled.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_EOL_EOPCHECK,                            CATEGORY_MEMANUF_EOL_TEST_FAILED,            "End-Of-Post message is not sent.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_EOL_FOVLOCKCHECK_UNKNOWN,                CATEGORY_MEMANUF_EOL_TEST_FAILED,            "Unable to determine Intel(R) ME Manufacturing Mode status.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_EOL_FOVLOCKCHECK_NOT_EOM,                CATEGORY_MEMANUF_EOL_TEST_FAILED,            "Intel(R) ME is still in Manufacturing Mode.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_CHECK_FRACC_ACCESSLOCK,                  CATEGORY_MEMANUF_EOL_TEST_FAILED,            "BIOS has granted Intel(R) Gbe and/or ME access to its region.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_EOL_MISMATCH,                            CATEGORY_MEMANUF_EOL_TEST_FAILED,            "%s mismatch, actual value is - %s.") \
    ERROR_DEFINE_HELPER (RESERVED_139,                                          CATEGORY_INTERNAL_ERROR,                     "") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_NO_AMT,                                  CATEGORY_MEMANUF_TEST_RUN_ERROR,             "Cannot run the command since Intel(R) AMT is not available.") \
    ERROR_DEFINE_HELPER (MEMANUF_MFS_CORRUPTED_ERROR,                           CATEGORY_MEMANUF_TEST_RUN_ERROR,             "MFS is corrupted.") \
    ERROR_DEFINE_HELPER (MEMANUF_PCH_ID_MISMATCH_ERROR,                         CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Using wrong PCH SKU Emulation via Intel (R) FIT vs whats the actual HW Type.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_FAILED_HIBERNATION,                      CATEGORY_MEMANUF_TEST_RUN_ERROR,             "Cannot perform hibernation. Please manually reboot the system.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_TEST_FAILED,                             CATEGORY_MEMANUF_OPERATION_FAILED,           "MEManuf Test Failed.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_TEST_NOT_RUN,                            CATEGORY_MEMANUF_TEST_RUN_ERROR,             "Test is enabled by the user but is unknown by the platform - %s.") \
    ERROR_DEFINE_HELPER (ROOT_NODE_ADDING_SIBLING_ERROR,                        CATEGORY_XML_USER_FILE_ERROR,                "Attempting to add sibling to XML root node.") \
    ERROR_DEFINE_HELPER (EMPTY_FILE_ERROR,                                      CATEGORY_CSE_FILE_READ_ERROR,                "File size is zero.") \
    ERROR_DEFINE_HELPER (XML_PARSING_FAILURE,                                   CATEGORY_XML_USER_FILE_ERROR,                "XML parsing failed.") \
    ERROR_DEFINE_HELPER (XML_PARSING_OVERFLOW_FAILURE,                          CATEGORY_XML_USER_FILE_ERROR,                "XML parsing encountered data overflow.") \
    ERROR_DEFINE_HELPER (XML_INVALID_ERROR_CODE_CONVERSION,                     CATEGORY_XML_USER_FILE_ERROR,                "Invalid XML error code conversion.") \
    ERROR_DEFINE_HELPER (XML_PARSER_ERROR_NO_MEMORY,                            CATEGORY_XML_USER_FILE_ERROR,                "XML parser - out of memory error.") \
    ERROR_DEFINE_HELPER (XML_MISSING_REQUIRED_NODE,                             CATEGORY_USER_INPUT_FILE_ERROR,              "Missing RequiredValue xml node in Master Access configuration file.") \
    ERROR_DEFINE_HELPER (XML_INCORRECT_REGION_NAME,                             CATEGORY_USER_INPUT_FILE_ERROR,              "Incorrect region name in Master Access configuration file.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_FAILED_QUERY_TEST,                       CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Failed to retrieve list of BIST tests to run from FW.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_FAILED_BIST_BISTRESULT_CMD,              CATEGORY_MEMANUF_TEST_RESULT_READ_ERROR,     "Unexpected failure when retrieving BIST results.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_RETRIEVE_EOL_CFG,                        CATEGORY_USER_INPUT_FILE_ERROR,              "Retrieving the EOL Config list of tests failed.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_RETRIEVE_EOL_VAR,                        CATEGORY_USER_INPUT_FILE_ERROR,              "Retrieving the EOL Var list of tests failed.") \
    ERROR_DEFINE_HELPER (MEMANUF_CONFIG_FILE_NO_NAME_FOR_TEST,                  CATEGORY_USER_INPUT_FILE_ERROR,              "No name attribute specified for test: %s.") \
    ERROR_DEFINE_HELPER (MEMANUF_CONFIG_FILE_PARSING_ERROR,                     CATEGORY_USER_INPUT_FILE_ERROR,              "Failed to parse configuration file provided.") \
    ERROR_DEFINE_HELPER (MEMANUF_CONFIG_FILE_WRITE_NO_FILE_PATH,                CATEGORY_USER_INPUT_FILE_ERROR,              "No output file path specified to write configuration file.") \
    ERROR_DEFINE_HELPER (MEMANUF_CONFIG_FILE_WRITE_NO_DATA,                     CATEGORY_USER_INPUT_FILE_ERROR,              "No data to write to configuration file.") \
    ERROR_DEFINE_HELPER (MEMANUF_CONFIG_FILE_INVALID_ERRACTION_DATA,            CATEGORY_USER_INPUT_FILE_ERROR,              "Invalid ErrAction specified") \
    ERROR_DEFINE_HELPER (SPI_INIT_INCOMPATIBLE_DEVICES,                         CATEGORY_SPI_ACCESS_ERROR,                   "The 2 SPI flash devices do not have compatible command sets.") \
    ERROR_DEFINE_HELPER (SPI_INIT_NO_DEVICES_FOUND,                             CATEGORY_SPI_ACCESS_ERROR,                   "No SPI flash device could be identified. Please verify if Fparts.txt has support.") \
    ERROR_DEFINE_HELPER (SPI_LOADDEVFILE_NO_MEMORY,                             CATEGORY_SPI_ACCESS_ERROR,                   "Failed to allocate memory for the flash part definition file \"%s\".") \
    ERROR_DEFINE_HELPER (SPI_LOADFILE_PARSE_FAILED,                             CATEGORY_SPI_ACCESS_ERROR,                   "Parsing file failed.") \
    ERROR_DEFINE_HELPER (SPI_PROTECTED_RANGE_ACCESS_DENIED,                     CATEGORY_FPT_INVALID_SPI_CONF,               "Protected Range Registers are currently set by BIOS, preventing flash access.\n" \
                                                                                                                             "Please contact the target system BIOS vendor for an option to disable\n" \
                                                                                                                             "Protected Range Registers.") \
    ERROR_DEFINE_HELPER (SPI_HWSEQ_FAILED,                                      CATEGORY_SPI_ACCESS_ERROR,                   "Hardware sequencing failed. Make sure that you have access to target flash area.") \
    ERROR_DEFINE_HELPER (SPI_READ_ACCESS_DENIED,                                CATEGORY_SPI_ACCESS_ERROR,                   "The host CPU does not have read access to the target flash area.\n" \
                                                                                                                             "To enable read access for this operation you must modify the descriptor" \
                                                                                                                             "settings to give host access to this region.") \
    ERROR_DEFINE_HELPER (SPI_READ_OUT_OF_RANGE,                                 CATEGORY_SPI_ACCESS_ERROR,                   "An attempt was made to read beyond the end of flash memory.") \
    ERROR_DEFINE_HELPER (SPI_SWSEQ_FAILED,                                      CATEGORY_SPI_ACCESS_ERROR,                   "Software sequencing failed. Make sure that you have access to target flash area.") \
    ERROR_DEFINE_HELPER (FPT_INVALID_BLOCK_ERASE_SIZE,                          CATEGORY_FPT_INVALID_SPI_CONF,               "Invalid Block Erase Size.") \
    ERROR_DEFINE_HELPER (FPT_INVALID_WRITE_GRANULARITY,                         CATEGORY_FPT_INVALID_SPI_CONF,               "Invalid Write Granularity value.") \
    ERROR_DEFINE_HELPER (FPT_INVALID_WRITE_STATUS_REG_CMD,                      CATEGORY_FPT_INVALID_SPI_CONF,               "Invalid Enable Write Status Register Command value.") \
    ERROR_DEFINE_HELPER (SPI_DEV_INFO_INVALID_INDEX,                            CATEGORY_INTERNAL_ERROR,                     "The supplied zero-based index of the SPI Device is out of range.") \
    ERROR_DEFINE_HELPER (SPI_REGION_INVALID_DESC,                               CATEGORY_SPI_ACCESS_ERROR,                   "Invalid descriptor region.") \
    ERROR_DEFINE_HELPER (SPI_REGION_NO_EXIST,                                   CATEGORY_SPI_ACCESS_ERROR,                   "Region does not exist.") \
    ERROR_DEFINE_HELPER (SPI_WRITE_OUT_OF_RANGE,                                CATEGORY_SPI_ACCESS_ERROR,                   "An attempt was made to write beyond the end of flash memory.") \
    ERROR_DEFINE_HELPER (SPI_ERASE_OUT_OF_RANGE,                                CATEGORY_SPI_ACCESS_ERROR,                   "An attempt was made to erase beyond the end of flash memory.") \
    ERROR_DEFINE_HELPER (SPI_ERASE_ALIGNMENT_ERROR,                             CATEGORY_SPI_ACCESS_ERROR,                   "The address 0x%08X of the block to erase is not aligned correctly.") \
    ERROR_DEFINE_HELPER (SPI_HARDWARE_TIMEOUT_OCCURRED,                         CATEGORY_SPI_ACCESS_ERROR,                   "Hardware timeout occurred in SPI device.") \
    ERROR_DEFINE_HELPER (SPI_INIT_DEVICE_NOT_SUPPORTED,                         CATEGORY_SPI_ACCESS_ERROR,                   "There are no supported SPI flash devices installed. Please check connectivity.") \
    ERROR_DEFINE_HELPER (SPI_UNRECOGNISED_HSFSTS_VALUE,                         CATEGORY_SPI_ACCESS_ERROR,                   "Unrecognized value in the HSFSTS register.") \
    ERROR_DEFINE_HELPER (SPI_STATUS_AEL,                                        CATEGORY_SPI_ACCESS_ERROR,                   "AEL is not equal to zero.") \
    ERROR_DEFINE_HELPER (SPI_STATUS_FCERR,                                      CATEGORY_SPI_ACCESS_ERROR,                   "FCERR is not equal to zero.") \
    ERROR_DEFINE_HELPER (MEMANUF_EOL_CHECKING_VAR_FAILED,                       CATEGORY_CSE_FILE_COMPARE_GENERAL_ERROR,     "Checking variable \"%s\" failed.") \
    ERROR_DEFINE_HELPER (FPROG_FOV_INVALID_VAR_VALUE,                           CATEGORY_CSE_FILE_UPDATE_ERROR,              "Invalid value for \"%s\" CVAR.") \
    ERROR_DEFINE_HELPER (FPROG_FOV_INVALID_VAR_NAME,                            CATEGORY_CSE_FILE_READ_ERROR,                "Invalid Manufacturing Line Configurable variable name \"%s\".") \
    ERROR_DEFINE_HELPER (FPROG_FILE_DOESNOT_EXISTS,                             CATEGORY_IO_ERROR,                           "File does not exist.") \
    ERROR_DEFINE_HELPER (FPROG_VERIFY_LOCK_SETTINGS_FAILED,                     CATEGORY_TOOL_FEATURE_EXECUTION_ERROR,       "End Of Manufacturing Operation failure - Verification failure on Descriptor Lock settings.") \
    ERROR_DEFINE_HELPER (FPROG_ERROR_GET_MASTER_BASE_ADDR,                      CATEGORY_SPI_ACCESS_ERROR,                   "Unable to get master base address from the descriptor.") \
    ERROR_DEFINE_HELPER (FPROG_INVALID_PASSWORD,                                CATEGORY_INVALID_USER_INPUT,                 "Password does not match the criteria.") \
    ERROR_DEFINE_HELPER (FPROG_INVALID_VALUE_LENGTH,                            CATEGORY_CSE_FILE_INVALID_INPUT_VALUE,       "Invalid length of Manufacturing Line Configurable value.\n" \
                                                                                                                             "Check configuration file for correct length.") \
    ERROR_DEFINE_HELPER (FPROG_INVALID_HASH_FILE,                               CATEGORY_CSE_FILE_INVALID_INPUT_VALUE,       "Invalid hash certificate file.") \
    ERROR_DEFINE_HELPER (FPROG_VERIFY_GLOBAL_LOCKED_FAILED,                     CATEGORY_CSE_FILE_UPDATE_ERROR,              "End Of Manufacturing Operation failure - Verification failure on ME Manufacturing\n" \
                                                                                                                             "Mode Done settings.") \
    ERROR_DEFINE_HELPER (NVAR_MCA_POLICY_CFG_RULES_WRONG_MANUF_STATE,           CATEGORY_CSE_FILE_UPDATE_ERROR,              "cfg_rules: the requested rule change is not supported after end of manufacturing.") \
    ERROR_DEFINE_HELPER (FPROG_VALIDATION_PARAM_FAILED,                         CATEGORY_INVALID_CLI_OPTIONS,                "Invalid parameter value specified by user. Use -? option to see help.") \
    ERROR_DEFINE_HELPER (ME_DISABLED,                                           CATEGORY_INTERNAL_ERROR,                     "ME disabled.") \
    ERROR_DEFINE_HELPER (FPROG_FAILED_GET_DEVICE_INFO,                          CATEGORY_SPI_ACCESS_ERROR,                   "Failed to get information about the installed flash devices.") \

#define ERROR_LIST_HELPER3 /* 201-300 */\
    ERROR_DEFINE_HELPER (FPROG_ERROR_READ_FLASH_DESC,                           CATEGORY_SPI_ACCESS_ERROR,                   "An error occurred reading the flash descriptor signature.") \
    ERROR_DEFINE_HELPER (FPROG_DESC_INVALID_SIGNATURE,                          CATEGORY_SPI_ACCESS_ERROR,                   "Flash descriptor does not have correct signature.") \
    ERROR_DEFINE_HELPER (FPT_FOV_COMMIT_FAILED,                                 CATEGORY_CSE_FILE_UPDATE_ERROR,              "The attempt to commit the Manufacturing Line Configurables has failed.") \
    ERROR_DEFINE_HELPER (FPROG_ACCESS_DENIED_FILE_OPEN,                         CATEGORY_FPT_INVALID_SPI_CONF,               "Access was denied opening file.") \
    ERROR_DEFINE_HELPER (FPROG_FAILED_FILE_READ_ENTIRE,                         CATEGORY_IO_ERROR,                           "Failed to read the entire file into memory. File: %s.") \
    ERROR_DEFINE_HELPER (FPROG_READ_ADDR_OUTSIDE_BOUNDARY,                      CATEGORY_USER_INPUT_FILE_ERROR,              "The address is outside the boundaries of the flash area.") \
    ERROR_DEFINE_HELPER (FPROG_CANNOT_WRITE_FLASH,                              CATEGORY_SPI_ACCESS_ERROR,                   "Unable to write data to flash. Address 0x%x.") \
    ERROR_DEFINE_HELPER (FPROG_VERIFY_MISMATCH_FOUND,                           CATEGORY_SPI_ACCESS_ERROR,                   "Data verify mismatch found.") \
    ERROR_DEFINE_HELPER (FPROG_FAILED_WRITE_FLASH_TO_FILE,                      CATEGORY_IO_ERROR,                           "Failed to write the entire flash contents to file.") \
    ERROR_DEFINE_HELPER (FPROG_ERROR_READ_MAPPING_DATA,                         CATEGORY_SPI_ACCESS_ERROR,                   "An error occurred reading the flash mapping data.") \
    ERROR_DEFINE_HELPER (FPROG_ERROR_VALID_SIGN_NON_DESC_MODE,                  CATEGORY_SPI_ACCESS_ERROR,                   "System booted in Non-Descriptor mode, but the flash appears to contain a valid signature.") \
    ERROR_DEFINE_HELPER (FPROG_ERROR_READ_COMPONENT_DATA,                       CATEGORY_SPI_ACCESS_ERROR,                   "An error occurred reading the flash components data.") \
    ERROR_DEFINE_HELPER (FPROG_ERROR_READ_BASE_LIMIT,                           CATEGORY_SPI_ACCESS_ERROR,                   "An error occurred reading the flash region base/limit data.") \
    ERROR_DEFINE_HELPER (FPROG_ERROR_READ_MASTER_ACCESS_DATA,                   CATEGORY_SPI_ACCESS_ERROR,                   "An error occurred reading the flash master access data.") \
    ERROR_DEFINE_HELPER (FPROG_FLASH_NOT_BLANK,                                 CATEGORY_SPI_ACCESS_ERROR,                   "Flash is not blank.") \
    ERROR_DEFINE_HELPER (NVAR_MCA_WRONG_PAVP_EDP_CONFIG_VALUE,                  CATEGORY_CSE_FILE_UPDATE_ERROR,              "PAVP oem config data: invalid edp port valu.e") \
    ERROR_DEFINE_HELPER (FPT_SETTING_GLOBAL_RESET_FAILED,                       CATEGORY_TOOL_FEATURE_EXECUTION_ERROR,       "Setting Global Reset Failed.") \
    ERROR_DEFINE_HELPER (ME_DISABLE_NOT_NEEDED,                                 CATEGORY_INTERNAL_ERROR,                     "ME disable not needed.") \
    ERROR_DEFINE_HELPER (ME_ALREADY_DISABLED,                                   CATEGORY_INTERNAL_ERROR,                     "ME already disabled.") \
    ERROR_DEFINE_HELPER (FPT_ME_DISABLE_FAILURE,                                CATEGORY_TOOL_FEATURE_EXECUTION_ERROR,       "The request to disable the ME failed.") \
    ERROR_DEFINE_HELPER (FPT_GBE_ERROR_SAVING_GBE_DATA,                         CATEGORY_SPI_ACCESS_ERROR,                   "There is a problem with the GbE binary which prevents saving the data.") \
    ERROR_DEFINE_HELPER (FPT_MISSING_REQUIRED_PARAMETER,                        CATEGORY_TOOL_FEATURE_EXECUTION_ERROR,       "A required parameter is missing.") \
    ERROR_DEFINE_HELPER (FPF_COMMIT_NOT_ALLOWED,                                CATEGORY_CSE_FILE_UPDATE_ERROR,              "Committing the FPF is not allowed at this time.") \
    ERROR_DEFINE_HELPER (FPF_ALREADY_COMMITTED,                                 CATEGORY_CSE_FILE_UPDATE_ERROR,              "The FPF has already been committed.") \
    ERROR_DEFINE_HELPER (NVAR_MCA_WRONG_PAVP_LSPCON_CONFIG_VALUE,               CATEGORY_CSE_FILE_UPDATE_ERROR,              "PAVP oem config data: invalid lspcon port value.") \
    ERROR_DEFINE_HELPER (FPF_COMMIT_SPECIFIC_NOT_SUPPORTED,                     CATEGORY_CSE_FILE_UPDATE_ERROR,              "Committing a specific FPF is not supported. Consider committing all the FPFs.") \
    ERROR_DEFINE_HELPER (KEYBOX_FILE_SIZE_ERROR,                                CATEGORY_USER_INPUT_FILE_ERROR,              "Keybox file size invalid.") \
    ERROR_DEFINE_HELPER (NVAR_MCA_WRONG_ALL_HASHES_STATE,                       CATEGORY_CSE_FILE_UPDATE_ERROR,              "Invalid all hashes state file.")\
    ERROR_DEFINE_HELPER (NVAR_MCA_WRONG_IDLE_TIMEOUT,                           CATEGORY_CSE_FILE_UPDATE_ERROR,              "Invalid idle timeout file.")\
    ERROR_DEFINE_HELPER (NVAR_MCA_WRONG_PROV_STATE,                             CATEGORY_CSE_FILE_UPDATE_ERROR,              "Invalid provisioning state file.")\
    ERROR_DEFINE_HELPER (KEYBOX_PROVISION_CEK_INVALID,                          CATEGORY_KEYBOX_INVALID_STATUS,              "CEK is invalid.") \
    ERROR_DEFINE_HELPER (KEYBOX_PROVISION_CEK_NOT_AVAILABLE,                    CATEGORY_KEYBOX_INVALID_STATUS,              "CEK is not available.") \
    ERROR_DEFINE_HELPER (KEYBOX_PROVISION_AFTER_EOM,                            CATEGORY_KEYBOX_INVALID_STATUS,              "Cannot provision after EOM.") \
    ERROR_DEFINE_HELPER (NVAR_MCA_WRONG_REDIR_STATE,                            CATEGORY_CSE_FILE_UPDATE_ERROR,              "Invalid redirection state file.")\
    ERROR_DEFINE_HELPER (KEYBOX_PROVISION_BAD_CRC,                              CATEGORY_KEYBOX_INVALID_STATUS,              "Bad CRC.") \
    ERROR_DEFINE_HELPER (KEYBOX_PROVISION_BAD_MAGIC,                            CATEGORY_KEYBOX_INVALID_STATUS,              "Bad Magic.") \
    ERROR_DEFINE_HELPER (NVAR_MCA_WRONG_EHBC_STATE,                             CATEGORY_CSE_FILE_UPDATE_ERROR,              "Invalid EHBC state file.")\
    ERROR_DEFINE_HELPER (KEYBOX_NOT_PROVISIONED,                                CATEGORY_KEYBOX_INVALID_STATUS,              "Keybox is not provisioned.") \
    ERROR_DEFINE_HELPER (SPI_WRITE_ACCESS_DENIED,                               CATEGORY_SPI_ACCESS_ERROR,                   "The host CPU does not have write access to the target flash area.\n" \
                                                                                                                             "To enable write access for this operation you must modify the descriptor\n" \
                                                                                                                             "settings to give host access to this region.") \
    ERROR_DEFINE_HELPER (USER_CANCELLED_OPERATIION,                             CATEGORY_INVALID_USER_INPUT,                 "User selected to cancel the operation.") \
    ERROR_DEFINE_HELPER (HECI_RESP_LEN_INVALID,                                 CATEGORY_INTERNAL_ERROR,                     "Internal error - Invalid Heci response length.") \
    ERROR_DEFINE_HELPER (SYS_STATE_ERROR,                                       CATEGORY_SYSTEM_API_CALL_ERROR,              "Error determining possible system states.") \
    ERROR_DEFINE_HELPER (HECI_STATUS_CANNOT_LOCATE_DRIVER,                      CATEGORY_HECI_COMMUNICATION_FAILURE,         "Cannot locate MEI driver.") \
    ERROR_DEFINE_HELPER (FWU_FW_NOT_FOUND,                                      CATEGORY_FWU_FW_ERROR,                       "Unexpected internal FW error occurred. Object was not found.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_CONFIG_FILE_INVALID_STATE,               CATEGORY_USER_INPUT_FILE_ERROR,              "Invalid \"State\" found for test - %s.") \
    ERROR_DEFINE_HELPER (ERROR_ISH_SRV_BIST_TEST_ERROR,                         CATEGORY_MEMANUF_BIST_TEST_FAILED,           "ISH Internal Error.") \
    ERROR_DEFINE_HELPER (ERROR_ISH_SRV_BIST_IUP_NOT_FOUND_ERROR,                CATEGORY_MEMANUF_BIST_TEST_FAILED,           "IUP Not Found.") \
    ERROR_DEFINE_HELPER (HID_STATUS_LOCATE_DEVICE_ERROR,                        CATEGORY_OS_DEVICE_COMMUNICATION_FAILURE,    "Cannot locate HID device.") \
    ERROR_DEFINE_HELPER (HID_STATUS_GET_FEATURE_REPORT_FAILED,                  CATEGORY_OS_DEVICE_COMMUNICATION_FAILURE,    "Incorrect Report ID received.") \
    ERROR_DEFINE_HELPER (ERROR_BIST_TEST_MCTP_SMBUS3_FAILED,                    CATEGORY_MEMANUF_BIST_TEST_FAILED,           "MCTP SMBUS test failed.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_CONFIG_FILE_MISSING_STATE,               CATEGORY_USER_INPUT_FILE_ERROR,              "Invalid config file. \"State\" was not found for test - %s.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_CONFIG_FILE_MISSING_REQ_VALUE,           CATEGORY_USER_INPUT_FILE_ERROR,              "Invalid config file. \"RequiredValue\" was not found for test - %s.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_CFG_FILE_MISSING_ERR_ACTION,             CATEGORY_USER_INPUT_FILE_ERROR,              "Invalid config file. \"ErrAction\" was not found for test - %s.") \
    ERROR_DEFINE_HELPER (ERROR_TOUCH_STATUS_ACCESS_DENIED,                      CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Unable to validate address range.") \
    ERROR_DEFINE_HELPER (ERROR_TOUCH_STATUS_NOT_READY,                          CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Memory window not set or device is not armed for operation.") \
    ERROR_DEFINE_HELPER (ERROR_TOUCH_STATUS_NO_SENSOR_FOUND,                    CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Sensor could not be found. Either no sensor is connected, the sensor has not yet initialized,\n" \
                                                                                                                              "or the system is improperly configured.") \
    ERROR_DEFINE_HELPER (ERROR_TOUCH_STATUS_OUT_OF_MEMORY,                      CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Not enough memory/storage for requested operation.") \
    ERROR_DEFINE_HELPER (ERROR_TOUCH_STATUS_SENSOR_DISABLED,                    CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Used in TOUCH_SENSOR_HID_READY_FOR_DATA_RSP to indicate sensor has been disabled or reset and\n" \
                                                                                                                             "must be reinitialized.") \
    ERROR_DEFINE_HELPER (ERROR_TOUCH_STATUS_COMPAT_CHECK_FAIL,                  CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Used to indicate compatibility revision check between sensor and ME failed,\n" \
                                                                                                                             "or protocol ver between ME/HID/Kernels failed.") \
    ERROR_DEFINE_HELPER (ERROR_TOUCH_STATUS_SENSOR_UNEXPECTED_RESET,            CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Indicates sensor went through an unexpected reset.") \
    ERROR_DEFINE_HELPER (ERROR_TOUCH_STATUS_RESET_FAILED,                       CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Requested sensor reset failed to complete.") \
    ERROR_DEFINE_HELPER (ERROR_TOUCH_STATUS_TIMEOUT,                            CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Operation timed out.") \
    ERROR_DEFINE_HELPER (ERROR_TOUCH_STATUS_TEST_MODE_FAIL,                     CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Test mode pattern did not match expected values.") \
    ERROR_DEFINE_HELPER (ERROR_TOUCH_STATUS_SENSOR_FAIL_FATAL,                  CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Indicates sensor reported fatal error during reset sequence. Further progress is not possible.") \
    ERROR_DEFINE_HELPER (ERROR_TOUCH_STATUS_SENSOR_FAIL_NONFATAL,               CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Indicates sensor reported non-fatal error during reset sequence.\n" \
                                                                                                                             "HID/BIOS logs error and attempts to continue.") \
    ERROR_DEFINE_HELPER (ERROR_TOUCH_STATUS_INVALID_DEVICE_CAPS,                CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Indicates sensor reported invalid capabilities, such as not supporting required minimum frequency or I/O mode.") \
    ERROR_DEFINE_HELPER (ERROR_TOUCH_STATUS_QUIESCE_IO_IN_PROGRESS,             CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Indicates that command cannot be complete until ongoing Quiesce I/O flow has completed.") \
    ERROR_DEFINE_HELPER (NVAR_MCA_NO_FILE,                                      CATEGORY_CSE_FILE_READ_ERROR,                "Cannot find the NVAR file; the system maybe in EOM.") \
    ERROR_DEFINE_HELPER (NVAR_MCA_INVALID_CFG_RULE,                             CATEGORY_CSE_FILE_UPDATE_ERROR,              "Invalid cfg rule data.") \
    ERROR_DEFINE_HELPER (NVAR_MCA_NO_FILE_ATTRIBUTE,                            CATEGORY_CSE_FILE_READ_ERROR,                "Cannot access the NVAR file attributes.") \
    ERROR_DEFINE_HELPER (CSE_FILE_HASH_ERROR,                                   CATEGORY_INTERNAL_ERROR,                     "Failed to hash CSE file data.") \
    ERROR_DEFINE_HELPER (NVAR_MCA_AFTER_LOCK,                                   CATEGORY_CSE_FILE_UPDATE_ERROR,              "Operation is not allowed after EOM.") \
    ERROR_DEFINE_HELPER (NVAR_MCA_INVALID_INPUT,                                CATEGORY_CSE_FILE_INVALID_INPUT_VALUE,       "Used an invalid input parameter to access the NVAR file.") \
    ERROR_DEFINE_HELPER (FPF_MCA_FILE_NOT_WRITTEN,                              CATEGORY_CSE_FILE_READ_ERROR,                "FPF is not written.") \
    ERROR_DEFINE_HELPER (NVAR_MCA_WRONG_PRIVACY_LEVEL,                          CATEGORY_CSE_FILE_UPDATE_ERROR,              "Invalid privacy level file.") \
    ERROR_DEFINE_HELPER (FILE_INVALID,                                          CATEGORY_USER_INPUT_FILE_ERROR,              "File is invalid.") \
    ERROR_DEFINE_HELPER (HDCP_RX_PROVISION_AFTER_EOM,                           CATEGORY_TOOL_FEATURE_EXECUTION_ERROR,       "Can not provision after EOM.") \
    ERROR_DEFINE_HELPER (HDCP_RX_PROVISION_CERT_VERIFICATION_FAILED,            CATEGORY_TOOL_FEATURE_EXECUTION_ERROR,       "Certificate verification failed.") \
    ERROR_DEFINE_HELPER (HDCP_RX_NOT_PROVISIONED,                               CATEGORY_TOOL_FEATURE_EXECUTION_ERROR,       "HDCP Rx is not provisioned.") \
    ERROR_DEFINE_HELPER (FPROG_INVALID_STRING,                                  CATEGORY_CSE_FILE_INVALID_INPUT_VALUE,       "Invalid string value entered for the Manufacturing Line Configurable.") \
    ERROR_DEFINE_HELPER (ME_RECOVERY_MODE,                                      CATEGORY_ME_COMMUNICATION_FAILURE,           "Detected ME in recovery mode.") \
    ERROR_DEFINE_HELPER (FPT_DELETE_TOKEN_FAILED,                               CATEGORY_TOOL_FEATURE_EXECUTION_ERROR,       "FW returned status: Erase token failure.") \
    ERROR_DEFINE_HELPER (SIZE_FAILURE,                                          CATEGORY_INVALID_USER_INPUT,                 "Detected invalid data size.") \
    ERROR_DEFINE_HELPER (INVALID_HEX_FAILURE,                                   CATEGORY_INVALID_USER_INPUT,                 "Detected invalid hex value.") \
    ERROR_DEFINE_HELPER (ERROR_BIST_TEST_PAVP_SET_EDP_FAILED,                   CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Failed to retrieve 5K port setting.") \
    ERROR_DEFINE_HELPER (ERROR_BIST_TEST_PAVP_SET_LSPCON_FAILED,                CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Failed to retrieve LSPCON Port setting.") \
    ERROR_DEFINE_HELPER (ERROR_PAVP_CHECK_EDP_LSPCON_CONFIG_FAILED,             CATEGORY_MEMANUF_BIST_TEST_FAILED,           "Display port settings are not correct.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_CHECK_EC_REGION_WRITE,                   CATEGORY_MEMANUF_EOL_TEST_FAILED,            "EC Region write access permissions don't match Intel recommended values.") \
    ERROR_DEFINE_HELPER (NVAR_SUPPORT_FILE_SIZE_MISMATCH,                       CATEGORY_CSE_FILE_UPDATE_ERROR,              "Unexpected size found in the file \"%s\". Expected: 0x%X. Received: 0x%X.") \
    ERROR_DEFINE_HELPER (FW_PHASE_IN_BUP,                                       CATEGORY_ME_COMMUNICATION_FAILURE,           "Unable to execute command in this Firmware State. Please reboot.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_CHECK_GBE_REGION_WRITE,                  CATEGORY_MEMANUF_EOL_TEST_FAILED,            "GBE Region write access permissions don't match Intel recommended values.") \
    ERROR_DEFINE_HELPER (FPROG_GPIO_DATA_ERROR,                                 CATEGORY_CSE_FILE_INVALID_INPUT_VALUE,       "GPIO file contains GPIO pin assignments that are not multiples\n" \
                                                                                                                             "of the GPIO pin data structure.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_CHECK_ME_REGION_WRITE,                   CATEGORY_MEMANUF_EOL_TEST_FAILED,            "ME Region write access permissions don't match Intel recommended values.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_EOL_MISMATCH_UEP_VS_HW,                  CATEGORY_MEMANUF_EOL_TEST_FAILED,            "Mismatch on FPF file %s - UEP: %s, FPF HW: %s.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_EOL_FPF_NOT_COMMITTED,                   CATEGORY_MEMANUF_EOL_TEST_FAILED,            "FPFs are not committed to HW.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_CHECK_BIOS_REGION_WRITE,                 CATEGORY_MEMANUF_EOL_TEST_FAILED,            "BIOS Region write access permissions don't match Intel recommended values.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_READING_FPF_HW,                          CATEGORY_MEMANUF_EOL_TEST_FAILED,            "Failed to read FPF HW.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_FPF_CONFIG_NOT_SET,                      CATEGORY_MEMANUF_EOL_TEST_FAILED,            "SOC Config Lock is not set.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_EOL_FPF_LOCK_BIT_NOT_SET,                CATEGORY_MEMANUF_EOL_TEST_FAILED,            "Lock bit FPF is not set on file.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_READING_FPF_UEP,                         CATEGORY_MEMANUF_EOL_TEST_FAILED,            "Failed to read FPF in UEP.") \

#define ERROR_LIST_HELPER4 /* 301-400 */\
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_EOL_FWUPDATE_OEM_ID,                     CATEGORY_MEMANUF_EOL_TEST_FAILED,            "FW Update OEM ID incorrectly set to 00 or FF.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_EOL_FWUPDATE_OEM_ID_UNKNOWN,             CATEGORY_MEMANUF_EOL_TEST_FAILED,            "Unable to determine FW Update OEM ID status.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_CHECK_BIOS_REGION_READ,                  CATEGORY_MEMANUF_EOL_TEST_FAILED,            "BIOS Region read access permissions don't match Intel recommended values.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_CHECK_ME_REGION_READ,                    CATEGORY_MEMANUF_EOL_TEST_FAILED,            "ME Region read access permissions don't match Intel recommended values.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_CHECK_GBE_REGION_READ,                   CATEGORY_MEMANUF_EOL_TEST_FAILED,            "GBE Region read access permissions don't match Intel recommended values.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_CHECK_EC_REGION_READ,                    CATEGORY_MEMANUF_EOL_TEST_FAILED,            "EC Region read access permissions don't match Intel recommended values.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_EOL_RPMC_NOT_ENABLED,                    CATEGORY_MEMANUF_EOL_TEST_FAILED,            "RPMC SPI device did not initialize RPMC support correctly,\n" \
                                                                                                                             "RPMC SPI device needs replacement/ refurbishment.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_EOL_RPMC_NOT_BOUND,                      CATEGORY_MEMANUF_EOL_TEST_FAILED,            "RPMC SPI device has not been bound to the platform yet,\n" \
                                                                                                                             "RPMC manufacturing process is not complete.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_HW_BINDING_CHECK,                        CATEGORY_MEMANUF_EOL_TEST_FAILED,            "\"HW Binding\" state is not enabled.") \
    ERROR_DEFINE_HELPER (FPROG_VAR_NOT_UPDATABLE_ERROR,                         CATEGORY_CSE_FILE_UPDATE_ERROR,              "The \"%s\" var is not updatable.") \
    ERROR_DEFINE_HELPER (NVAR_DEP_SUPPORT_FAILED_VAR,                           CATEGORY_CSE_FILE_READ_ERROR,                "The variable \"%s\" is not supported on this platform.") \
    ERROR_DEFINE_HELPER (ERROR_PCH_UNLOCK_STATE,                                CATEGORY_MEMANUF_EOL_TEST_FAILED,            "PCH is unlocked. Disable Delayed Authentication Mode and retry.") \
    ERROR_DEFINE_HELPER (MEMANUF_INVALID_REQUIRED_VALUE_FORMAT,                 CATEGORY_CSE_FILE_COMPARE_GENERAL_ERROR,     "Test required value format is not valid.") \
    ERROR_DEFINE_HELPER (ERROR_BTG_CONFIGURATION,                               CATEGORY_CSE_FILE_INVALID_INPUT_VALUE,       "Invalid BootGuard configuration.") \
    ERROR_DEFINE_HELPER (ERROR_CONFIRM_ARB_SVN,                                 CATEGORY_MEMANUF_EOL_TEST_FAILED,            "Minimum ARB SVN value set on current platform does not match corresponding ARB SVN in FW image.") \
    ERROR_DEFINE_HELPER (FWU_FW_INVALID,                                        CATEGORY_FWU_FW_ERROR,                       "Unexpected internal FW error occurred. Invalid parameter.") \
    ERROR_DEFINE_HELPER (ERROR_PLATFORM_NAME_FOUND,                             CATEGORY_UNSUPPORTED_HW,                     "Platform name for this PCH type not found or not exists.") \
    ERROR_DEFINE_HELPER (FPROG_CLEAR_FPF_ERROR,                                 CATEGORY_CSE_FILE_INVALID_INPUT_VALUE,       "Clear option is not supported for FPFs.") \
    ERROR_DEFINE_HELPER (STORAGE_TYPE_NOT_SUPPORTED,                            CATEGORY_UNSUPPORTED_HW,                     "This command cannot be processed on platforms using %s as the storage type.") \
    ERROR_DEFINE_HELPER (REGION_NOT_SUPPORTED,                                  CATEGORY_UNSUPPORTED_HW,                     "This command cannot be processed. Region is not supported on this platform.") \
    ERROR_DEFINE_HELPER (FPROG_SIZE_EXCEED_LIMIT,                               CATEGORY_CSE_FILE_UPDATE_ERROR,              "The maximum number of updated NVARs has been reached.") \
    ERROR_DEFINE_HELPER (INVALID_VALUE_FOR_CVAR,                                CATEGORY_CSE_FILE_INVALID_INPUT_VALUE,       "Invalid value for this CVAR.") \
    ERROR_DEFINE_HELPER (VAR_COMPARE_FAILURE,                                   CATEGORY_CSE_FILE_COMPARE_GENERAL_ERROR,     "The VAR compare failed.") \
    ERROR_DEFINE_HELPER (ERROR_FLASH_LOGS_EXIST,                                CATEGORY_MEMANUF_EOL_TEST_FAILED,            "Fatal flash logs exist in NVM.") \
    ERROR_DEFINE_HELPER (HECI_REQUEST_REPLY_SIZE_MISMATCH,                      CATEGORY_HECI_COMMUNICATION_FAILURE,         "Request and Reply messages' size mismatch.") \
    ERROR_DEFINE_HELPER (HECI_UNSUPPORTED_MSG_TYPE,                             CATEGORY_HECI_COMMUNICATION_FAILURE,         "Intel (R) ME Interface : Unsupported message type.") \
    ERROR_DEFINE_HELPER (PART_NOT_PRESENT,                                      CATEGORY_INVALID_PARTITION,                  "Specified partition was not found in the Update Image.") \
    ERROR_DEFINE_HELPER (FWU_NO_FPT_IN_IMAGE,                                   CATEGORY_USER_INPUT_FILE_ERROR,              "FPT is not found in the image.") \
    ERROR_DEFINE_HELPER (FWU_ALLOWSV_MISSING,                                   CATEGORY_INVALID_CLI_OPTIONS,                "Full FW Update using same version is not allowed. Include -allowsv in command line to allow it.") \
    ERROR_DEFINE_HELPER (FWU_RESTORE_POINT_FAILURE,                             CATEGORY_FWU_RESTORE_POINT_ERROR,            "Restore Point Image Failure. Reboot may be required.") \
    ERROR_DEFINE_HELPER (FWU_INVALID_PARTID,                                    CATEGORY_INVALID_CLI_OPTIONS,                "Invalid Partition ID. Use a Partition ID which is possible to do Partial FW Update on.") \
    ERROR_DEFINE_HELPER (FWU_PID_NOT_EXPECTED,                                  CATEGORY_INVALID_PARTITION,                  "The partition provided is not supported by the platform.") \
    ERROR_DEFINE_HELPER (FWU_INVALID_IMG_LENGTH,                                CATEGORY_FWU_FW_ERROR,                       "The requested size of partition to read/write/erase exceeds the actual partition size.") \
    ERROR_DEFINE_HELPER (FWU_UPD_PROCESS,                                       CATEGORY_FWU_FW_ERROR,                       "Firmware Update operation not initiated because a firmware update is already in progress.") \
    ERROR_DEFINE_HELPER (FWU_SKU_MISMATCH,                                      CATEGORY_FWU_FW_ERROR,                       "Sku capabilities bits are different between the Update Image and the Flash Image.") \
    ERROR_DEFINE_HELPER (FWU_VER_MISMATCH,                                      CATEGORY_FWU_FW_ERROR,                       "Major version number of Update Image is not the same as major version number of Flash Image.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_CREATING_FT,                                 CATEGORY_FWU_FW_ERROR,                       "Firmware update failed due to an internal error\n" \
                                                                                                                             "The total size of the backup partitions is bigger than NFTP size.") \
    ERROR_DEFINE_HELPER (FWU_SAL_NOTIFICATION,                                  CATEGORY_FWU_FW_ERROR,                       "Firmware update failed due to an internal error caused by a failure in event publishing.") \
    ERROR_DEFINE_HELPER (FWU_FW_DEVICE_ERROR,                                   CATEGORY_FWU_FW_ERROR,                       "FW Flash read/write/erase operation failed.") \
    ERROR_DEFINE_HELPER (FWU_UPDATE_TIMEOUT,                                    CATEGORY_ME_COMMUNICATION_FAILURE,           "Update operation timed-out; cannot determine if the operation succeeded.") \
    ERROR_DEFINE_HELPER (FWU_LOCAL_DIS,                                         CATEGORY_FWU_FW_ERROR,                       "FW Update is disabled. MEBX has options to disable / enable FW Update.") \
    ERROR_DEFINE_HELPER (FWU_INVALID_OEM_ID,                                    CATEGORY_FWU_FW_ERROR,                       "Firmware update cannot be initiated because the OEM ID given for FW Update did not match the OEM ID in the FW.") \
    ERROR_DEFINE_HELPER (FWU_DISPLAY_FW_VERSION,                                CATEGORY_FWU_OPERATION_FAILED,               "Display FW Version failed.") \
    ERROR_DEFINE_HELPER (FWU_DOWNGRADE_VETOED,                                  CATEGORY_FWU_FW_ERROR,                       "Update was blocked by one of the FW modules.") \
    ERROR_DEFINE_HELPER (FWU_FW_WRITE_FILE_FAIL,                                CATEGORY_FWU_FW_ERROR,                       "Firmware update failed due to an internal error\n" \
                                                                                                                             "Write file failed: error occurred in write() or number of bytes written is not the same as file length.") \
    ERROR_DEFINE_HELPER (FWU_PARTITION_LAYOUT_NOT_COMP,                         CATEGORY_FWU_FW_ERROR,                       "Sanity check in erase/write of partitions. Error might have happened when size of partition is not 4K aligned.") \
    ERROR_DEFINE_HELPER (FWU_FLASH_CODE_PARTITION_INVALID,                      CATEGORY_FWU_FW_ERROR,                       "Firmware update failed due to an internal error\n" \
                                                                                                                             "Firmware returns invalid flash code partition.") \
    ERROR_DEFINE_HELPER (FWU_FLASH_NFT_PARTITION_INVALID,                       CATEGORY_FWU_FW_ERROR,                       "Firmware update failed due to an internal error\n" \
                                                                                                                             "NFTP is corrupted, CSE is in Recovery Mode.") \
    ERROR_DEFINE_HELPER (FWU_HOST_RESET_REQUIRED,                               CATEGORY_FWU_FW_ERROR,                       "Host reset is required after the last FW Update operation.") \
    ERROR_DEFINE_HELPER (FWU_LOWER_TCB_SVN,                                     CATEGORY_FWU_FW_ERROR,                       "Update to Image with lower TCB SVN is not allowed.") \
    ERROR_DEFINE_HELPER (FWU_INSTID_IS_NOT_EXPECTED_ID,                         CATEGORY_FWU_FW_ERROR,                       "Partial update is allowed only to the expected instance ID of an IUP.\n" \
                                                                                                                             "The Update Image contains IUP with instance ID that is not the currently expected one by the FW.\n" \
                                                                                                                             "To update LOCL, please use The Intel Management and Security Status (IMSS) tool.") \
    ERROR_DEFINE_HELPER (FWU_REJ_IPU_FULL_UPDATE_NEEDED,                        CATEGORY_FWU_FW_ERROR,                       "Partial Update is not allowed, because CSE is in Recovery Mode.") \
    ERROR_DEFINE_HELPER (FWU_IPU_NAMEID_NOT_FOUND,                              CATEGORY_FWU_FW_ERROR,                       "Partial Update of an IUP was requested, but this IUP doesn't exist in the Flash Image.") \
    ERROR_DEFINE_HELPER (FWU_RESTORE_POINT_OPERATION_NOT_ALLOWED,               CATEGORY_FWU_FW_ERROR,                       "Get Restore Point Image is not allowed, because FW Update is in progress. (The regular FW Update will continue).") \
    ERROR_DEFINE_HELPER (FWU_LOWER_VCN,                                         CATEGORY_FWU_FW_ERROR,                       "Update to Image with lower VCN is not allowed.") \
    ERROR_DEFINE_HELPER (FWU_INVALID_SVN,                                       CATEGORY_FWU_FW_ERROR,                       "SVN invalid: SVN larger than 254 is not allowed.") \
    ERROR_DEFINE_HELPER (FWU_OUT_OF_SVN_RESOURCES,                              CATEGORY_FWU_FW_ERROR,                       "PSVN partition is full, so cannot update to higher SVN.") \
    ERROR_DEFINE_HELPER (FWU_RESTORE_POINT_REQUEST_FLASH_IN_RECOVERY,           CATEGORY_FWU_FW_ERROR,                       "Restore Point Image was requested, but it is not allowed because CSE is in Recovery Mode.") \
    ERROR_DEFINE_HELPER (FWU_DISPLAY_PART_VERSION,                              CATEGORY_FWU_OPERATION_FAILED,               "Display Partition Version failed.") \
    ERROR_DEFINE_HELPER (FWU_RESTORE_POINT_REQUEST_RESTART_NEEDED,              CATEGORY_FWU_FW_ERROR,                       "Restore Point Image was requested, but there was Full/Partial FW Update before without Restart after it.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_PMC_INSTANCE,                                CATEGORY_FWU_FW_ERROR,                       "Update to incompatible PMC: The PMC instance ID is different, which may be due to H/LP SKU incompatibility.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_H_LP_MISMATCH,                               CATEGORY_FWU_FW_ERROR,                       "Update to incompatible H/LP SKU image.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_UPD_IMG_TOO_BIG,                             CATEGORY_FWU_FW_ERROR,                       "Update Image length is bigger than the expected size of the image according to its size in the flash.\n" \
                                                                                                                             "For example: Error on updating from Consumer to Corporate.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_INVALID_MANIFEST_SIZE,                       CATEGORY_FWU_FW_ERROR,                       "Manifest size in Update Image is bigger than 8KB, or exceeds the Update Image size.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_OPEN_LDR_VER_MAN_FAILED,                     CATEGORY_FWU_FW_ERROR,                       "Failed to open loader to verify manifest.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_OPEN_LDR_KEYS_FAILED,                        CATEGORY_FWU_FW_ERROR,                       "Failed to open loader to install / uninstall keys.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_INSTALL_KEYS_FAILED,                         CATEGORY_FWU_FW_ERROR,                       "Failed to verify signature of OEM or RoT key manifests. For example: Error on update from Production to Pre-Production.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_UNINSTALL_KEYS_FAILED,                       CATEGORY_FWU_FW_ERROR,                       "loader uninstall keys failed.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_SKUMGR_FAILED,                               CATEGORY_FWU_FW_ERROR,                       "Call to sku mgr functions failed.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_CFGMGR_FAILED,                               CATEGORY_FWU_FW_ERROR,                       "Call to cfgmgr functions failed.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_MAN_NOT_FOUND,                               CATEGORY_FWU_FW_ERROR,                       "Manifest not found in partition (in Update or Flash Image).") \
    ERROR_DEFINE_HELPER (FWU_ERROR_CRYPTO_FAILED,                               CATEGORY_FWU_FW_ERROR,                       "Crypto operation (calculating hash of partition) failed.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_VER_MAN_FAILED_FTPR,                         CATEGORY_FWU_FW_ERROR,                       "Loader failed to verify manifest signature of FTPR. Production vs. Pre-Production.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_VER_MAN_FAILED_NFTP,                         CATEGORY_FWU_FW_ERROR,                       "Loader failed to verify manifest signature of NFTP.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_VER_MAN_FAILED_DLMP,                         CATEGORY_FWU_FW_ERROR,                       "Loader failed to verify manifest signature of IDLM.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_VER_MAN_FAILED_RBEP,                         CATEGORY_FWU_FW_ERROR,                       "Loader failed to verify manifest signature of RBE.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_VER_MAN_FAILED_PMCP,                         CATEGORY_FWU_FW_ERROR,                       "Loader failed to verify manifest signature of PMC.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_VER_MAN_FAILED_OEMP,                         CATEGORY_FWU_FW_ERROR,                       "Loader failed to verify manifest signature of OEM KM.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_VER_MAN_FAILED_WCOD,                         CATEGORY_FWU_FW_ERROR,                       "Loader failed to verify manifest signature of WCOD.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_VER_MAN_FAILED_LOCL,                         CATEGORY_FWU_FW_ERROR,                       "Loader failed to verify manifest signature of LOCL.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_VER_MAN_FAILED_PCHC,                         CATEGORY_FWU_FW_ERROR,                       "Loader failed to verify manifest signature of PCHC.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_VER_MAN_FAILED_IOMP,                         CATEGORY_FWU_FW_ERROR,                       "Loader failed to verify manifest signature of IOMP.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_VER_MAN_FAILED_MGPP,                         CATEGORY_FWU_FW_ERROR,                       "Loader failed to verify manifest signature of MGPP.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_VER_MAN_FAILED_TBTP,                         CATEGORY_FWU_FW_ERROR,                       "Loader failed to verify manifest signature of TBTP.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_VER_MAN_FAILED_ISHC,                         CATEGORY_FWU_FW_ERROR,                       "Loader failed to verify manifest signature of ISHC.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_VER_MAN_FAILED_IUNP,                         CATEGORY_FWU_FW_ERROR,                       "Loader failed to verify manifest signature of IUNIT.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_GET_EXT_FAILED_FTPR,                         CATEGORY_FWU_FW_ERROR,                       "Some manifest extension is missing in FTPR.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_GET_EXT_FAILED_NFTP,                         CATEGORY_FWU_FW_ERROR,                       "Some manifest extension is missing in NFTP.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_GET_EXT_FAILED_DLMP,                         CATEGORY_FWU_FW_ERROR,                       "Some manifest extension is missing in IDLM.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_GET_EXT_FAILED_RBEP,                         CATEGORY_FWU_FW_ERROR,                       "Some manifest extension is missing in RBE.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_GET_EXT_FAILED_PMCP,                         CATEGORY_FWU_FW_ERROR,                       "Some manifest extension is missing in PMC. Wrong MEU Tool was used to create the partition.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_GET_EXT_FAILED_OEMP,                         CATEGORY_FWU_FW_ERROR,                       "Some manifest extension is missing in OEM KM. Wrong MEU Tool was used to create the partition.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_GET_EXT_FAILED_WCOD,                         CATEGORY_FWU_FW_ERROR,                       "Some manifest extension is missing in WCOD.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_GET_EXT_FAILED_LOCL,                         CATEGORY_FWU_FW_ERROR,                       "Some manifest extension is missing in LOCL.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_GET_EXT_FAILED_PCHC,                         CATEGORY_FWU_FW_ERROR,                       "Some manifest extension is missing in PCHC. Wrong MEU Tool was used to create the partition.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_GET_EXT_FAILED_IOMP,                         CATEGORY_FWU_FW_ERROR,                       "Some manifest extension is missing in IOMP. Wrong MEU Tool was used to create the partition.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_GET_EXT_FAILED_MGPP,                         CATEGORY_FWU_FW_ERROR,                       "Some manifest extension is missing in MGPP. Wrong MEU Tool was used to create the partition.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_GET_EXT_FAILED_TBTP,                         CATEGORY_FWU_FW_ERROR,                       "Some manifest extension is missing in TBTP. Wrong MEU Tool was used to create the partition.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_GET_EXT_FAILED_ISHC,                         CATEGORY_FWU_FW_ERROR,                       "Some manifest extension is missing in ISHC. Wrong MEU Tool was used to create the partition.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_GET_EXT_FAILED_IUNP,                         CATEGORY_FWU_FW_ERROR,                       "Some manifest extension is missing in IUNIT. Wrong MEU Tool was used to create the partition.") \

#define ERROR_LIST_HELPER5 /* 401-500 */\
    ERROR_DEFINE_HELPER (FWU_ERROR_INTEGRITY_FAILED_FTPR,                       CATEGORY_FWU_FW_ERROR,                       "FTPR partition hash and calculated hash are not the same.\n" \
                                                                                                                             "If partition hash is zero - wrong MEU Tool was used to create the partition.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_INTEGRITY_FAILED_NFTP,                       CATEGORY_FWU_FW_ERROR,                       "NFTP partition hash and calculated hash are not the same.\n" \
                                                                                                                             "If partition hash is zero - wrong MEU Tool was used to create the partition.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_INTEGRITY_FAILED_DLMP,                       CATEGORY_FWU_FW_ERROR,                       "DLMP partition hash and calculated hash are not the same.\n" \
                                                                                                                             "If partition hash is zero - wrong MEU Tool was used to create the partition.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_INTEGRITY_FAILED_RBEP,                       CATEGORY_FWU_FW_ERROR,                       "RBEP partition hash and calculated hash are not the same.\n" \
                                                                                                                             "If partition hash is zero - wrong MEU Tool was used to create the partition.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_INTEGRITY_FAILED_PMCP,                       CATEGORY_FWU_FW_ERROR,                       "PMCP partition hash and calculated hash are not the same.\n" \
                                                                                                                             "If partition hash is zero - wrong MEU Tool was used to create the partition.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_INTEGRITY_FAILED_OEMP,                       CATEGORY_FWU_FW_ERROR,                       "OEMP partition hash and calculated hash are not the same.\n" \
                                                                                                                             "If partition hash is zero - wrong MEU Tool was used to create the partition.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_INTEGRITY_FAILED_WCOD,                       CATEGORY_FWU_FW_ERROR,                       "WCOD partition hash and calculated hash are not the same.\n" \
                                                                                                                             "If partition hash is zero - wrong MEU Tool was used to create the partition.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_INTEGRITY_FAILED_LOCL,                       CATEGORY_FWU_FW_ERROR,                       "LOCL partition hash and calculated hash are not the same.\n" \
                                                                                                                             "If partition hash is zero - wrong MEU Tool was used to create the partition.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_INTEGRITY_FAILED_PCHC,                       CATEGORY_FWU_FW_ERROR,                       "PCHC partition hash and calculated hash are not the same.\n" \
                                                                                                                             "If partition hash is zero - wrong MEU Tool was used to create the partition.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_INTEGRITY_FAILED_IOMP,                       CATEGORY_FWU_FW_ERROR,                       "IOMP partition hash and calculated hash are not the same.\n" \
                                                                                                                             "If partition hash is zero - wrong MEU Tool was used to create the partition.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_INTEGRITY_FAILED_MGPP,                       CATEGORY_FWU_FW_ERROR,                       "MGPP partition hash and calculated hash are not the same.\n" \
                                                                                                                             "If partition hash is zero - wrong MEU Tool was used to create the partition.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_INTEGRITY_FAILED_TBTP,                       CATEGORY_FWU_FW_ERROR,                       "TBTP partition hash and calculated hash are not the same.\n" \
                                                                                                                             "If partition hash is zero - wrong MEU Tool was used to create the partition.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_INTEGRITY_FAILED_ISHC,                       CATEGORY_FWU_FW_ERROR,                       "ISHC partition hash and calculated hash are not the same.\n" \
                                                                                                                             "If partition hash is zero - wrong MEU Tool was used to create the partition.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_INTEGRITY_FAILED_IUNP,                       CATEGORY_FWU_FW_ERROR,                       "IUNP partition hash and calculated hash are not the same.\n" \
                                                                                                                             "If partition hash is zero - wrong MEU Tool was used to create the partition.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_API_VER_MAJOR_FTPR,                          CATEGORY_FWU_FW_ERROR,                       "Place holder. This error code will not be returned by the FW.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_API_VER_MAJOR_NFTP,                          CATEGORY_FWU_FW_ERROR,                       "Place holder. This error code will not be returned by the FW.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_API_VER_MAJOR_DLMP,                          CATEGORY_FWU_FW_ERROR,                       "Place holder. This error code will not be returned by the FW.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_API_VER_MAJOR_RBEP,                          CATEGORY_FWU_FW_ERROR,                       "Place holder. This error code will not be returned by the FW.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_API_VER_MAJOR_PMCP,                          CATEGORY_FWU_FW_ERROR,                       "PMCP must have the same major API version as the version inside the list in FTPR,\n" \
                                                                                                                             "in the Update Image for Full Update.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_API_VER_MAJOR_OEMP,                          CATEGORY_FWU_FW_ERROR,                       "OEMP must have the same major API version as the version inside the list in FTPR,\n" \
                                                                                                                             "in the Update Image for Full Update.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_API_VER_MAJOR_WCOD,                          CATEGORY_FWU_FW_ERROR,                       "WCOD must have the same major API version as the version inside the list in FTPR,\n" \
                                                                                                                             "in the Update Image for Full Update, in the Flash Image for Partial Update.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_API_VER_MAJOR_LOCL,                          CATEGORY_FWU_FW_ERROR,                       "LOCL must have the same major API version as the version inside the list in FTPR,\n" \
                                                                                                                             "in the Update Image for Full Update, in the Flash Image for Partial Update.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_API_VER_MAJOR_PCHC,                          CATEGORY_FWU_FW_ERROR,                       "PCHC must have the same major API version as the version inside the list in FTPR,\n" \
                                                                                                                             "in the Update Image for Full Update, in the Flash Image for Partial Update.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_API_VER_MAJOR_IOMP,                          CATEGORY_FWU_FW_ERROR,                       "IOMP must have the same major API version as the version inside the list in FTPR,\n" \
                                                                                                                             "in the Update Image for Full Update, in the Flash Image for Partial Update.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_API_VER_MAJOR_MGPP,                          CATEGORY_FWU_FW_ERROR,                       "MGPP must have the same major API version as the version inside the list in FTPR,\n" \
                                                                                                                             "in the Update Image for Full Update, in the Flash Image for Partial Update.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_API_VER_MAJOR_TBTP,                          CATEGORY_FWU_FW_ERROR,                       "TBTP must have the same major API version as the version inside the list in FTPR,\n" \
                                                                                                                             "in the Update Image for Full Update, in the Flash Image for Partial Update.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_API_VER_MAJOR_ISHC,                          CATEGORY_FWU_FW_ERROR,                       "ISHC must have the same major API version as the version inside the list in FTPR,\n" \
                                                                                                                             "in the Update Image for Full Update, in the Flash Image for Partial Update.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_API_VER_MAJOR_IUNP,                          CATEGORY_FWU_FW_ERROR,                       "IUNP must have the same major API version as the version inside the list in FTPR,\n" \
                                                                                                                             "in the Update Image for Full Update, in the Flash Image for Partial Update.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_PART_SIZE,                                   CATEGORY_FWU_FW_ERROR,                       "The size of an Update partition is bigger than the size of the Flash partition.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_BACKUP_OUTSIDE_NFTP,                         CATEGORY_FWU_FW_ERROR,                       "Location of partition to backup is not inside NFTP.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_MAX_IUPS,                                    CATEGORY_FWU_FW_ERROR,                       "The number of IUPs in the Update/Flash Image is bigger than MAX_IUPS.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_NOT_IUP,                                     CATEGORY_FWU_FW_ERROR,                       "Partition name inside IUPs list (in FTPR manifest extension) is not IUP.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_IUP_MISSING_UPDATE,                          CATEGORY_FWU_FW_ERROR,                       "Non-optional IUP (like LOCL, WCOD) inside IUPs list (in FTPR manifest extension) is not in the Update Image.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_PMC_MISSING_UPDATE,                          CATEGORY_FWU_FW_ERROR,                       "PMC partition is not in the Update Image.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_NOT_PARTIAL_IUP,                             CATEGORY_FWU_FW_ERROR,                       "It is not allowed to do Partial Update on this partition.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_PARTIAL_TCSS,                                CATEGORY_FWU_FW_ERROR,                       "It is not allowed to do Partial Update on Type-C partitions, according to NVAR.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_FTPR_VER,                                    CATEGORY_FWU_FW_ERROR,                       "RBEP and NFTP must have the same version as FTPR, in the Update Image.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_FTPR_SVN,                                    CATEGORY_FWU_FW_ERROR,                       "RBEP and NFTP must have the same SVN as FTPR, in the Update Image.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_FTPR_VCN,                                    CATEGORY_FWU_FW_ERROR,                       "RBEP and NFTP must have the same VCN as FTPR, in the Update Image.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_FTPR_VER_MAJOR,                              CATEGORY_FWU_FW_ERROR,                       "Non-optional IUPs (like LOCL, WCOD) must have the same major build version as FTPR,\n" \
                                                                                                                             "in the Update Image for Full Update, in the Flash Image for Partial Update.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_IUP_SVN,                                     CATEGORY_FWU_FW_ERROR,                       "Update IUP must not have SVN smaller than SVN of Flash IUP.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_IMAGE_LEN,                                   CATEGORY_FWU_FW_ERROR,                       "Update Image length is not the same as Flash Image length.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_IUP_VCN,                                     CATEGORY_FWU_FW_ERROR,                       "Update IUP must not have VCN smaller than VCN of Flash IUP.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_PV_BIT,                                      CATEGORY_FWU_FW_ERROR,                       "Update from PV bit ON to PV bit OFF is not allowed.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_REVENUE,                                     CATEGORY_FWU_FW_ERROR,                       "Update to PV bit OFF on Revenue platform is not allowed.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_SVN_UPGRADE,                                 CATEGORY_FWU_FW_ERROR,                       "Update to higher SVN must be an upgrade - to higher build version.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_SVN_HOTFIX,                                  CATEGORY_FWU_FW_ERROR,                       "Update to higher SVN must be to a higher Hot Fix number (the third number in the build version).") \
    ERROR_DEFINE_HELPER (FWU_ERROR_IUP_MISSING_FLASH,                           CATEGORY_FWU_FW_ERROR,                       "Non-optional IUP (like LOCL, WCOD) inside IUPs list (in FTPR manifest extension) is not in the Flash Image.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_PARTITION_NOT_FOUND,                         CATEGORY_FWU_FW_ERROR,                       "A partition that was searched in the Update Image is not in it.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_ENGINEERING_MISMATCH,                        CATEGORY_FWU_FW_ERROR,                       "Update between engineering build vs regular build is not allowed.\n" \
                                                                                                                             "Both builds have to be the same type: regular or engineering build.\n" \
                                                                                                                             "Engineering build is 7000 and above. Regular build is below 7000.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_OEMP_MISSING,                                CATEGORY_FWU_FW_ERROR,                       "OEM KM partition is not in the Update Image, but ISHC/IUNP is in the Update Image, which is not allowed.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_IUPS_NOT_COMPATIBLE,                         CATEGORY_FWU_FW_ERROR,                       "ISHC/IUNP do not exist in the same way in the Update Image and in the Flash Image.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_OEMP_IN_UPDATE,                              CATEGORY_FWU_FW_ERROR,                       "OEM KM partition is not in the Flash Image, but it is in the Update Image, which is not allowed.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_WRONG_IUP,                                   CATEGORY_FWU_FW_ERROR,                       "Partial FW Update: the Update Image contains IUP that is different than the one that was requested to be updated in the Partial Update command.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_IMAGE_IUP_SIZE,                              CATEGORY_FWU_FW_ERROR,                       "The Partial Update Image size is different than the size of the IUP in it (as it is in the manifest).\n" \
                                                                                                                             "This means that the Update Image contains more (or less) than the IUP partition.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_OPEN_IUP,                                    CATEGORY_FWU_FW_ERROR,                       "Bug: Open of IUP path failed. Need to add the path in Storage, or add permissions to FW Update process.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_SPI_IUP,                                     CATEGORY_FWU_FW_ERROR,                       "Bug: spi_flash_partition_updated() failed. This updates the files (in the file system) of the newly updated IUP, after Partial Update (without reset).") \
    ERROR_DEFINE_HELPER (FWU_ERROR_RULE_FILE_INVALID,                           CATEGORY_FWU_FW_ERROR,                       "Update Rule file contains invalid value. (This file holds the MEBX option for FW Update: values: disable / enable).") \
    ERROR_DEFINE_HELPER (FWU_ERROR_PWR_FAILED,                                  CATEGORY_FWU_FW_ERROR,                       "Call to pwr function failed.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_SPI_FAILED,                                  CATEGORY_FWU_FW_ERROR,                       "Call to spi function failed.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_RESTORE_POINT_ALREADY_STARTED,               CATEGORY_FWU_FW_ERROR,                       "Get Restore Point Image is not allowed, because a previous Get Restore Point operation already started.\n" \
                                                                                                                             "Both operations will be aborted. (Get Restore Point can be started again after this).") \
    ERROR_DEFINE_HELPER (FWU_ERROR_RESTORE_POINT_OFFSET_INVALID,                CATEGORY_FWU_FW_ERROR,                       "Bug: Get Restore Point Image Data: The offset of Restore Point Image is bigger than the Image length.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_WRONG_HECI_MSG_LENGTH,                       CATEGORY_FWU_FW_ERROR,                       "Heci message length is not as expected.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_ENV_INVALID,                                 CATEGORY_FWU_FW_ERROR,                       "FWU_START_MSG Heci message contains invalid value in UpdateEnvironment.\n" \
                                                                                                                             "Value should be FWU_ENV_MANUFACTURING. (Other possible value: FWU_ENV_IFU is obsolete).") \
    ERROR_DEFINE_HELPER (FWU_ERROR_WRONG_DATA_OPERATION,                        CATEGORY_FWU_FW_ERROR,                       "FWU_DATA Heci command was sent, but the FW Update wasn't started with FWU_START Heci command before it.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_NVM_FAILED,                                  CATEGORY_FWU_FW_ERROR,                       "Call to storage_nvm function failed.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_UFS_EOP,                                     CATEGORY_FWU_FW_ERROR,                       "FW Update is not possible on UFS Flash after End Of Post (after the OS is running).\n" \
                                                                                                                             "It is possible only before the OS is running using Bios Capsule Update.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_API_VER_MAJOR_DPHY,                          CATEGORY_FWU_FW_ERROR,                       "DPHY must have the same major API version as the version inside the list in FTPR,\n" \
                                                                                                                             "in the Update Image for Full Update, in the Flash Image for Partial Update.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_INTEGRITY_FAILED_DPHY,                       CATEGORY_FWU_FW_ERROR,                       "DPHY partition hash and calculated hash are not the same. If partition hash is zero - wrong MEU Tool was used to create the partition.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_GET_EXT_FAILED_DPHY,                         CATEGORY_FWU_FW_ERROR,                       "Some manifest extension is missing in DPHY. Wrong MEU Tool was used to create the partition.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_VER_MAN_FAILED_DPHY,                         CATEGORY_FWU_FW_ERROR,                       "Loader failed to verify manifest signature of DPHY.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_SVN_TCB_ARB,                                 CATEGORY_FWU_FW_ERROR,                       "Update to higher TCB SVN must be also to higher ARB SVN.") \
    ERROR_DEFINE_HELPER (FWU_INVALID_PARTID_ALL,                                CATEGORY_INVALID_CLI_OPTIONS,                "Invalid Partition ID. Use a Partition ID which is on the Flash Image.") \
    ERROR_DEFINE_HELPER (FWU_DISPLAY_PART_VENDOR_ID,                            CATEGORY_FWU_OPERATION_FAILED,               "Display Partition Vendor ID failed.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_INVALID_UPDATE_IMAGE,                        CATEGORY_FWU_FW_ERROR,                       "Wrong structure of Update Image.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_INVALID_FLASH_IMAGE,                         CATEGORY_FWU_FW_ERROR,                       "Flash Image content is invalid.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_PG_FAILURE,                                  CATEGORY_FWU_FW_ERROR,                       "FW Update process called to PG entry override at the start of the update, and it returned error.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_IPK_FAILURE,                                 CATEGORY_FWU_FW_ERROR,                       "Clear ipk valid bit failed. This prevents CSE from entering M3 after FW Update, and instead CSE will go into MOff.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_FLUSH_NVM_ERR,                               CATEGORY_FWU_FW_ERROR,                       "Error when flushing NVM to UMA space (before rewriting flash).") \
    ERROR_DEFINE_HELPER (FWU_ERROR_WRONG_END_OPERATION,                         CATEGORY_FWU_FW_ERROR,                       "FWU_END Heci command was sent, but there was no FWU_DATA command before it.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_DATA_LENGTH_INVALID,                         CATEGORY_FWU_FW_ERROR,                       "FWU_DATA Heci command has invalid data length (too big).") \
    ERROR_DEFINE_HELPER (FWU_INVALID_HECI_CMD,                                  CATEGORY_FWU_FW_ERROR,                       "FW Update process received Heci command message with unknown command type.") \
    ERROR_DEFINE_HELPER (HECI_STATUS_CANNOT_DETECT_ME_STATE,                    CATEGORY_ME_COMMUNICATION_FAILURE,           "Cannot obtain ME Mode.") \
    ERROR_DEFINE_HELPER (FWU_ME_STATE_NOT_NORMAL,                               CATEGORY_ME_COMMUNICATION_FAILURE,           "Local FW Update only supported when ME Mode=Normal.") \
    ERROR_DEFINE_HELPER (ERROR_SROP_SHIP_NOT_SUPPORTED,                         CATEGORY_MEMANUF_BIST_TEST_FAILED,           "BIOS does not support boot measurements.") \
    ERROR_DEFINE_HELPER (ERROR_SETUP_LOCK_NOT_SUPPORTED,                        CATEGORY_MEMANUF_BIST_TEST_FAILED,           "BIOS does not support Trusted Device Setup boot.") \
    ERROR_DEFINE_HELPER (SLIM_SKU_UNSUPPORTED_COMMAND,                          CATEGORY_INVALID_CLI_OPTIONS,                "This command is not supported on Slim SKU.") \
    ERROR_DEFINE_HELPER (UNIQUE_VARS_ALREADY_SET,                               CATEGORY_CSE_FILE_UPDATE_ERROR,              "ODM ID \\ System Integrator ID \\ Reserved ID: value already set.") \
    ERROR_DEFINE_HELPER (ERROR_FILE_ALREADY_EXIST,                              CATEGORY_IO_ERROR,                           "File already exists.") \
    \
    /* TODO Change to use MEMANUF_ERROR_EOL_MISMATCH */ \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_EOL_FW_VERSIONCHECK,                     CATEGORY_MEMANUF_EOL_TEST_FAILED,            "ME FW version mismatch, actual value is - %s.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_EOL_GBE_VERSIONCHECK,                    CATEGORY_MEMANUF_EOL_TEST_FAILED,            "Intel(R) Gbe version mismatch, actual value is - %s.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_EOL_BIOS_VERSIONCHECK,                   CATEGORY_MEMANUF_EOL_TEST_FAILED,            "BIOS version mismatch, actual value is - %s.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_EOL_UUIDCHECK_MISMATCH,                  CATEGORY_MEMANUF_EOL_TEST_FAILED,            "System UUID mismatch, actual value is - %s.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_EOL_MACCHECK_MISMATCH,                   CATEGORY_MEMANUF_EOL_TEST_FAILED,            "Intel(R) Wired LAN MAC address mismatch, actual value is - %s.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_EOL_WLAN_MACCHECK_MISMATCH,              CATEGORY_MEMANUF_EOL_TEST_FAILED,            "Intel(R) Wireless LAN MAC address mismatch, actual value is - %s.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_EOL_WLAN_UCODE_MISMATCH,                 CATEGORY_MEMANUF_EOL_TEST_FAILED,            "Wireless LAN micro-code mismatch, actual value is - %s.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_EOL_FWU_OEM_ID_MISMATCH,                 CATEGORY_MEMANUF_EOL_TEST_FAILED,            "Firmware Update OEM ID mismatch, actual value is - %s.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_EOL_TOUCH_VENDORID_CHECK,                CATEGORY_MEMANUF_EOL_TEST_FAILED,            "Touch - Vendor ID mismatch, actual value is - %s.") \
    \
    ERROR_DEFINE_HELPER (NVAR_MCA_WRONG_PKI_SUFFIX,                             CATEGORY_INTERNAL_ERROR,                     "Invalid PKI suffix file.") \
    ERROR_DEFINE_HELPER (FWU_LOWER_ARB_SVN,                                     CATEGORY_FWU_FW_ERROR,                       "Update to Image with lower ARB SVN is not allowed.") \

#define ERROR_LIST_HELPER6 /* 501-600 */\
    ERROR_DEFINE_HELPER (FWU_ERROR_FTPR_BUILD,                                  CATEGORY_FWU_FW_ERROR,                       "RBEP and NFTP must have the same unique build as FTPR, in the Update Image.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_FIPS,                                        CATEGORY_FWU_FW_ERROR,                       "Disable FIPS mode failed.") \
    ERROR_DEFINE_HELPER (FPT_COMMITFPF_RPMB_FUSE_SET,                           CATEGORY_CSE_FILE_UPDATE_ERROR,              "RPMB fuse is set. Cannot commit FPFs.") \
    ERROR_DEFINE_HELPER (FWU_ERROR_PCHC_MISSING_UPDATE,                         CATEGORY_FWU_FW_ERROR,                       "PCHC partition is not in the Update Image.") \
    ERROR_DEFINE_HELPER (MEMANUF_ERROR_EOL_MISMATCH_UEP_HW,                     CATEGORY_MEMANUF_EOL_TEST_FAILED,            "Mismatch between FPF UEP and HW values.") \
    ERROR_DEFINE_HELPER (FWU_SMALL_BUFFER,                                      CATEGORY_USER_INPUT_FILE_ERROR,              "Invalid Update Image length, size is smaller than required.") \
    ERROR_DEFINE_HELPER (FWU_CORRUPTED,                                         CATEGORY_USER_INPUT_FILE_ERROR,              "The internal structure of the Update Image is corrupted.") \
    ERROR_DEFINE_HELPER (FWU_FULL_INVALID,                                      CATEGORY_USER_INPUT_FILE_ERROR,              "Update Image has wrong structure for Full Update operation.") \
    ERROR_DEFINE_HELPER (FWU_PARTIAL_INVALID,                                   CATEGORY_USER_INPUT_FILE_ERROR,              "Update Image has wrong structure for Partial Update operation.") \
    ERROR_DEFINE_HELPER (FWU_MANDATORY_INVALID,                                 CATEGORY_USER_INPUT_FILE_ERROR,              "Mandatory partitions (FTPR / NFTP / RBEP) were not found in the Update Image.") \
    ERROR_DEFINE_HELPER (FWU_MAX_IUP_ERROR,                                     CATEGORY_INTERNAL_ERROR,                     "Number of IUPs in FW exceeds allowed maximum.") \
    ERROR_DEFINE_HELPER (RESERVED_512,                                          CATEGORY_INTERNAL_ERROR,                     "") \
    ERROR_DEFINE_HELPER (FWU_MAN_NOT_FOUND,                                     CATEGORY_USER_INPUT_FILE_ERROR,              "Missing a required partition manifest in the Update Image.") \
    ERROR_DEFINE_HELPER (FWU_EXT_NOT_FOUND,                                     CATEGORY_USER_INPUT_FILE_ERROR,              "Missing a required partition manifest extension in the Update Image.") \
    ERROR_DEFINE_HELPER (VAR_INVALID_DATA_SIZE,                                 CATEGORY_CSE_FILE_READ_ERROR,                "The VAR invalid data size.") \
    ERROR_DEFINE_HELPER (FWU_ALLOCATED_BUFFER_SMALL,                            CATEGORY_FWU_FW_ERROR,                       "Update Image size exceeds allocated buffer.") \
    ERROR_DEFINE_HELPER (FWU_FWSTS_REG,                                         CATEGORY_FWU_FW_ERROR,                       "FW failed to read FWSTS register.") \
    ERROR_DEFINE_HELPER (FWU_FW_READ_FILE_FAIL,                                 CATEGORY_FWU_FW_ERROR,                       "Firmware update failed due to an internal error\n" \
                                                                                                                             "Read file failed: error occurred in read() or number of bytes read is not the same as file length.") \
    ERROR_DEFINE_HELPER (FWU_PG_IN_PROGRESS,                                    CATEGORY_FWU_FW_ERROR,                       "PG in progress, no override is allowed during such state.") \
    ERROR_DEFINE_HELPER (FWU_ALLOWSV_RS_MISSING,                                CATEGORY_INVALID_CLI_OPTIONS,                "Full FW Update using same version is not allowed. Include /s in command line to allow it.") \
    ERROR_DEFINE_HELPER (RESERVED_521,                                          CATEGORY_INTERNAL_ERROR,                     "") \
    ERROR_DEFINE_HELPER (FWU_FW_ISH_CFG,                                        CATEGORY_FWU_FW_ERROR,                       "FW failed to set ISH configuration file.") \
    ERROR_DEFINE_HELPER (ERROR_LAN_PCIE_NOT_READY,                              CATEGORY_MEMANUF_BIST_TEST_FAILED,           "PCIe connectivity failure. Unable to connect to vPro NIC through designated bus.")\
    ERROR_DEFINE_HELPER (ERROR_LAN_SMBUS_NOT_READY,                             CATEGORY_MEMANUF_BIST_TEST_FAILED,           "SMBUS connectivity failure. Unable to connect to vPro NIC through designated bus.")\
    ERROR_DEFINE_HELPER (ERROR_EDP_LSPCON_VALUES_OVERLAP,                       CATEGORY_CSE_FILE_UPDATE_ERROR,              "Conflict in OEM Data: Overlapping values of LSPCON Port Config and eDP Port Config found.")\
    ERROR_DEFINE_HELPER (ERROR_SRV_FQDN_INVALID_VALUE,                          CATEGORY_CSE_FILE_UPDATE_ERROR,              "Invalid configuration server FQDN value.")\
    ERROR_DEFINE_HELPER (NVAR_MCA_WRONG_HOST_FQDN,                              CATEGORY_CSE_FILE_UPDATE_ERROR,              "Invalid host FQDN file.")\
    ERROR_DEFINE_HELPER (NVAR_MCA_WRONG_GPIO_OWN_MODE,                          CATEGORY_CSE_FILE_UPDATE_ERROR,              "One or more GPIO pads provided in file have invalid ownership mode set.")\
    ERROR_DEFINE_HELPER (NVAR_MCA_WRONG_GPIO_PAD_MODE,                          CATEGORY_CSE_FILE_UPDATE_ERROR,              "One or more GPIO pads provided in file have invalid pad mode set.")\
    ERROR_DEFINE_HELPER (NVAR_MCA_WRONG_GPIO_FEATURE_IN_USE,                    CATEGORY_CSE_FILE_UPDATE_ERROR,              "Two or more GPIO pads provided in file have same feature field value set.")\
    ERROR_DEFINE_HELPER (NVAR_MCA_WRONG_GPIO_FEATURE,                           CATEGORY_CSE_FILE_UPDATE_ERROR,              "One or more GPIO pads provided in file have invalid feature field value set.")\
    ERROR_DEFINE_HELPER (NVAR_MCA_WRONG_CERT_HASH_VALUE,                        CATEGORY_CSE_FILE_UPDATE_ERROR,              "Invalid cert hash file.")\
    ERROR_DEFINE_HELPER (NVAR_MCA_WRONG_HOST_FQDN_DOMAIN,                       CATEGORY_CSE_FILE_UPDATE_ERROR,              "Invalid host FQDN domain name.")\
    ERROR_DEFINE_HELPER (NVAR_MCA_WRONG_HOST_FQDN_HOSTNAME,                     CATEGORY_CSE_FILE_UPDATE_ERROR,              "Invalid host FQDN hostname.")\
    ERROR_DEFINE_HELPER (NVAR_MCA_POLICY_SYSINTID_WRONG_SIZE,                   CATEGORY_CSE_FILE_UPDATE_ERROR,              "ODM ID \\ System Integrator ID \\ Reserved ID: invalid size.")\
    ERROR_DEFINE_HELPER (NVAR_MCA_POLICY_SYSINTID_WRONG_DATA,                   CATEGORY_CSE_FILE_UPDATE_ERROR,              "ODM ID \\ System Integrator ID \\ Reserved ID: invalid value.")\
    ERROR_DEFINE_HELPER (NVAR_MCA_WRONG_GPIO_PAD_ADDR_PARAM,                    CATEGORY_CSE_FILE_UPDATE_ERROR,              "One or more GPIO pads provided in file have invalid pad address set (group / pad number).")\
    ERROR_DEFINE_HELPER (NVAR_MCA_WRONG_GPIO_PAD_ADDR_IN_USE,                   CATEGORY_CSE_FILE_UPDATE_ERROR,              "Two or more GPIO pads provided in file have same pad address set.")\
    ERROR_DEFINE_HELPER (NVAR_MCA_PRE_PROV_INVALID_STATE,                       CATEGORY_CSE_FILE_UPDATE_ERROR,              "Update this var is not supported if AMT is provisioned.")\
    ERROR_DEFINE_HELPER (NVAR_MCA_EHBC_STATE_PRIV_LEVEL_MISMATC,                CATEGORY_CSE_FILE_UPDATE_ERROR,              "Unsupported combination of EHBC state and privacy level files.")\
    ERROR_DEFINE_HELPER (FWU_ERROR_FWSTS_INVALID,                               CATEGORY_FWU_FW_ERROR,                       "CSE is in Recovery Mode but FWSTS registers report Normal Mode.")\
    ERROR_DEFINE_HELPER (FWU_ERROR_BURN_INVALID,                                CATEGORY_FWU_FW_ERROR,                       "The Flash Image that was burned on the platform was corrupted. CSE is in Recovery Mode at first boot.")\
    ERROR_DEFINE_HELPER (FPROG_CLEAR_HASH_ERROR,                                CATEGORY_CSE_FILE_INVALID_INPUT_VALUE,       "Clear option is not supported for Hashed vars.") \
    ERROR_DEFINE_HELPER (RESERVED_544,                                          CATEGORY_INTERNAL_ERROR,                     "")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_INCORRECT_API_VERSION,                    CATEGORY_ICC_FW_ERROR,                       "Incorrect API version.")\
    ERROR_DEFINE_HELPER (RESERVED_546,                                          CATEGORY_INTERNAL_ERROR,                     "")\
    ERROR_DEFINE_HELPER (RESERVED_547,                                          CATEGORY_INTERNAL_ERROR,                     "")\
    ERROR_DEFINE_HELPER (RESERVED_548,                                          CATEGORY_INTERNAL_ERROR,                     "")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_FLASH_WEAR_OUT_VIOLATION,                 CATEGORY_ICC_FW_ERROR,                       "Flash wear out violation.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_FLASH_CORRUPTION,                         CATEGORY_ICC_FW_ERROR,                       "Flash corruption.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_PROFILE_NOT_SELECTABLE_BY_BIOS,           CATEGORY_ICC_FW_ERROR,                       "Profile is not selectable in BIOS.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_TOO_LARGE_PROFILE_INDEX,                  CATEGORY_ICC_FW_ERROR,                       "Profile index is too large.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_NO_SUCH_PROFILE_IN_FLASH,                 CATEGORY_ICC_FW_ERROR,                       "No such profile in flash.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_CMD_NOT_SUPPORTED_AFTER_END_OF_POST,      CATEGORY_ICC_FW_ERROR,                       "Command is not supported after EOP.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_NO_SUCH_RECORD,                           CATEGORY_ICC_FW_ERROR,                       "No such record.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_FILE_NOT_FOUND,                           CATEGORY_ICC_FW_ERROR,                       "File not found.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_INVALID_RECORD_FORMAT,                    CATEGORY_ICC_FW_ERROR,                       "Invalid record format.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_TOO_LARGE_UOB_RECORD,                     CATEGORY_ICC_FW_ERROR,                       "UOB record is too large.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_CLOCK_NOT_CONFIGURABLE,                   CATEGORY_ICC_FW_ERROR,                       "Clock is not configurable")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_REGISTER_IS_LOCKED,                       CATEGORY_ICC_FW_ERROR,                       "Register is locked.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_NO_VALID_PRE_UOB,                         CATEGORY_ICC_FW_ERROR,                       "No valid PRE UOB.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_NO_VALID_PERM_UOB,                        CATEGORY_ICC_FW_ERROR,                       "No valid PERM UOB.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_NO_DATA_FOR_THIS_CLOCK,                   CATEGORY_ICC_FW_ERROR,                       "No data for this clock.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_PROFILE_INDEX_IS_CURRENT,                 CATEGORY_ICC_FW_ERROR,                       "Profile index is current.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_NO_BCLK_ADJUSTMENT_FOUND,                 CATEGORY_ICC_FW_ERROR,                       "No bclk adjustment found.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_WARM_RESET_RAMP_NOT_SUPPORTED,            CATEGORY_ICC_FW_ERROR,                       "Warm reset ramp is not supported.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_UOB_RECORD_IS_ALREADY_INVALID,            CATEGORY_ICC_FW_ERROR,                       "UOB record is already invalid.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_NO_PROFILES_EXIST,                        CATEGORY_ICC_FW_ERROR,                       "No profile exists.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_AUTH_FAILURE,                             CATEGORY_ICC_FW_ERROR,                       "Authentication failure.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_ERROR_READING_FILE,                       CATEGORY_ICC_FW_ERROR,                       "Pending file.")\
    ERROR_DEFINE_HELPER (RESERVED_571,                                          CATEGORY_INTERNAL_ERROR,                     "")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_FREQ_TOO_HIGH,                            CATEGORY_ICC_FW_ERROR,                       "Frequency is too high.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_PENDING_REVERT_TO_DEFAULT,                CATEGORY_ICC_FW_ERROR,                       "Pending to revert to default.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_PENDING_SET_PROFILE,                      CATEGORY_ICC_FW_ERROR,                       "Pending to set profile.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_UNVALID_PROFILE,                          CATEGORY_ICC_FW_ERROR,                       "Invalid profile")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_UNVALID_OEM_DATA,                         CATEGORY_ICC_FW_ERROR,                       "Invalid OEM data.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_ERROR_READING_DYNAMIC_RECORD,             CATEGORY_ICC_FW_ERROR,                       "Failed to read dynamic record.")\
    ERROR_DEFINE_HELPER (RESERVED_578,                                          CATEGORY_INTERNAL_ERROR,                     "")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_FREQ_TOO_LOW,                             CATEGORY_ICC_FW_ERROR,                       "Frequency is too low.")\
    ERROR_DEFINE_HELPER (RESERVED_580,                                          CATEGORY_INTERNAL_ERROR,                     "")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_SSC_MODE_CHANGE_NOT_SUPPORTED,            CATEGORY_ICC_FW_ERROR,                       "SSC mode change is not supported.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_RANGE_VIOLATION_SSC_TOO_HIGH,             CATEGORY_ICC_FW_ERROR,                       "Range Violation: SSC is too high.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_SURVIVABILITY_SYNC_DISABLED,              CATEGORY_ICC_FW_ERROR,                       "Survivability sync disabled.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_WARM_RESET_FREQ_TOO_LOW,                  CATEGORY_ICC_FW_ERROR,                       "Warm reset required is too low.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_NO_SUCH_TARGET_ID,                        CATEGORY_ICC_FW_ERROR,                       "Specified target ID does not exist.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_NO_SUCH_REGISTER,                         CATEGORY_ICC_FW_ERROR,                       "Specified register does not exist.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_INVALIDATE_SUCCESSFUL,                    CATEGORY_ICC_FW_ERROR,                       "Invalidate successful.")\
    ERROR_DEFINE_HELPER (RESERVED_588,                                          CATEGORY_INTERNAL_ERROR,                     "")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_VALID_UOB_ALREADY_PRESENT,                CATEGORY_ICC_FW_ERROR,                       "Valid UOB already present.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_WAITING_FOR_POWER_CYCLE,                  CATEGORY_ICC_FW_ERROR,                       "Waiting for power cycle.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_SURVIVABILITY_TABLE_ACCESS_VIOLATION,     CATEGORY_ICC_FW_ERROR,                       "Survivability table access violation.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_SURVIVABILITY_TABLE_TOO_LARGE,            CATEGORY_ICC_FW_ERROR,                       "Survivability table is too large.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_NO_SUCH_EID,                              CATEGORY_ICC_FW_ERROR,                       "EID does not exist.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_SUCCESS_TRANSLATE_ONLY,                   CATEGORY_ICC_FW_ERROR,                       "Success translate only.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_PCIE_FAIL_READING_DATA,                   CATEGORY_ICC_FW_ERROR,                       "Failure in reading PCIe Data.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_PCIE_FAIL_WRITING_DATA,                   CATEGORY_ICC_FW_ERROR,                       "Failure in writing PCIe Data.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_PCIE_CONFIG_INVALID,                      CATEGORY_ICC_FW_ERROR,                       "Invalid PCIe configuration data.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_CMD_NOT_SUPPORTED_BEFORE_DID,             CATEGORY_ICC_FW_ERROR,                       "CMD not supported before DID.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_FIA_MUX_CONFIG_SKU_MISMATCH,              CATEGORY_ICC_FW_ERROR,                       "FIA MUX error - max config sku mismatch.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_FIA_MUX_NO_CONFIG_FOUND,                  CATEGORY_ICC_FW_ERROR,                       "FIA MUX error - no config found.")\

#define ERROR_LIST_HELPER7 /* 601-700 */\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_FIA_MUX_ERROR_GETTING_LANES_LIMIT,        CATEGORY_ICC_FW_ERROR,                       "FIA MUX error - getting laned limit.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_FIA_MUX_ERROR_READING_CONF_FROM_FILE,     CATEGORY_ICC_FW_ERROR,                       "FIA MUX error - reading configuration from file.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_FIA_MUX_ERROR_PROMPTING_TO_GLOBAL_RESET,  CATEGORY_ICC_FW_ERROR,                       "FIA MUX error - prompting to global reset.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_FIA_MUX_INVALID_FIA_MUX_CONFIG,           CATEGORY_ICC_FW_ERROR,                       "Invalid FIA MUX configuration.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_FIA_MUX_ERROR_WRITING_CONF_TO_FILE,       CATEGORY_ICC_FW_ERROR,                       "FIA MUX error - reading configuration to file.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_FIA_MUX_ERROR_READING_CONF_FROM_STRAPS,   CATEGORY_ICC_FW_ERROR,                       "FIA MUX error - reading configuration from straps.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_MAX_BUNDLES_PER_RECORD_REACHED,           CATEGORY_ICC_FW_ERROR,                       "Max bundles record reached.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_PLL_UNSUPPORTED,                          CATEGORY_ICC_FW_ERROR,                       "Specified PLL is not supported")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_DATA_ITEM_UNSUPPORTED,                    CATEGORY_ICC_FW_ERROR,                       "Data item unsupported.")\
    ERROR_DEFINE_HELPER (ICC_FW_ERROR_OEM_PROFILE_CRDR_VIOLATION,               CATEGORY_ICC_FW_ERROR,                       "Oem profile card violation.")\
    ERROR_DEFINE_HELPER (RESERVED_611,                                          CATEGORY_INTERNAL_ERROR,                     "")\
    ERROR_DEFINE_HELPER (RESERVED_612,                                          CATEGORY_INTERNAL_ERROR,                     "")\
    ERROR_DEFINE_HELPER (ICC_SW_INVALID_ARG,                                    CATEGORY_INVALID_USER_INPUT,                 "Invalid argument.")\
    ERROR_DEFINE_HELPER (AMT_IPV6_DISABLED,                                     CATEGORY_TOOL_FEATURE_EXECUTION_ERROR,       "AMT Ipv4 Interface is disabled.")\
    ERROR_DEFINE_HELPER (AMT_INTERFACE_DOES_NOT_EXIST,                          CATEGORY_TOOL_FEATURE_EXECUTION_ERROR,       "Interface does not exists.")\
    ERROR_DEFINE_HELPER (NVAR_MCA_WRONG_UC_POLICY,                              CATEGORY_CSE_FILE_UPDATE_ERROR,              "Invalid user consent policy file.")\
    ERROR_DEFINE_HELPER (FEAT_ARB_SVN_DISABLED,                                 CATEGORY_MEINFO_FEATURE_NOT_FOUND,           "Anti Rollback SVN feature is disabled.") \
    ERROR_DEFINE_HELPER (INPUT_FILE_TOO_LARGE,                                  CATEGORY_CSE_FILE_INVALID_INPUT_VALUE,       "Input file is too big.")\
\
    ERROR_DEFINE_HELPER (ERROR_CODES_COUNT,                                     CATEGORY_INTERNAL_ERROR,                     "")



typedef enum _ErrorCodes
{
#ifdef ERROR_DEFINE_HELPER
#error "Rename ERROR_DEFINE_HELPER to name that not in use"
#endif
#define ERROR_DEFINE_HELPER(eenum, ccategory ,sstring)    eenum,
    ERROR_LIST_HELPER1
    ERROR_LIST_HELPER2
    ERROR_LIST_HELPER3
    ERROR_LIST_HELPER4
    ERROR_LIST_HELPER5
    ERROR_LIST_HELPER6
    ERROR_LIST_HELPER7
#undef ERROR_DEFINE_HELPER
} ErrorCodes;

/**
@brief  Return the error string matching the error id
@param[in]  errorId - Error Id enum
@return Error string
**/
const char* GetErrorString(const ErrorCodes errorId);

/**
@brief  Return the error category number matching the error id
@param[in]  errorId - Error Id enum
@return Error category number
**/
unsigned char GetErrorCategory(const ErrorCodes errorId);

/**
 * \brief Returns description string associated with specific category
 * \param errorCategory
 * \return
 */
const char* GetErrorCategoryString(const ErrorCategories errorCategory);

#endif // _ERRORLIST_H
