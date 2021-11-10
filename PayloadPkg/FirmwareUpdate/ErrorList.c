/** @file

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <ErrorList.h>

typedef struct _ErrData
{
    const char * ErrStr;
    unsigned char Category;
} ErrData;

static const char* gErrorCategories[ERROR_CATEGORIES_COUNT + 1] =
{
#ifdef CATEGORY_DEFINE_HELPER
#error "Rename CATEGORY_DEFINE_HELPER to name that not in use"
#endif
#define CATEGORY_DEFINE_HELPER(eenum, sstring)    sstring,
    CATEGORY_LIST_HELPER1
#undef CATEGORY_DEFINE_HELPER
};

static ErrData gErrorData[ERROR_CODES_COUNT+1] =
{
#ifdef ERROR_DEFINE_HELPER
#error "Rename ERROR_DEFINE_HELPER to name that not in use"
#endif
#define ERROR_DEFINE_HELPER(eenum, ccategory, sstring)    { sstring, ccategory },
    ERROR_LIST_HELPER1
    ERROR_LIST_HELPER2
    ERROR_LIST_HELPER3
    ERROR_LIST_HELPER4
    ERROR_LIST_HELPER5
    ERROR_LIST_HELPER6
    ERROR_LIST_HELPER7
#undef ERROR_DEFINE_HELPER
};

const char* GetErrorString(const ErrorCodes errorId)
{
    ErrData errDataSturct;
    if (ERROR_CODES_COUNT < errorId)
    {
        // unexpected
        errDataSturct = gErrorData[INTERNAL_ERROR];
    }
    else
    {
        errDataSturct = gErrorData[errorId];
    }
    return errDataSturct.ErrStr;
}

unsigned char GetErrorCategory(const ErrorCodes errorId)
{
    ErrData errDataStruct;
    if (ERROR_CODES_COUNT < errorId)
    {
        // unexpected
        errDataStruct = gErrorData[INTERNAL_ERROR];
    }
    else
    {
        errDataStruct = gErrorData[errorId];
    }
    return errDataStruct.Category;
}

const char* GetErrorCategoryString(const ErrorCategories errorCategory)
{
    return ERROR_CATEGORIES_COUNT < errorCategory ? gErrorData[INTERNAL_ERROR].ErrStr : gErrorCategories[errorCategory];
}
