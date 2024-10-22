
/* @file
*
*  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
*
*/

#include <Library/CryptoLib.h>
#include <Library/ShellLib.h>
#include <Library/TimeStampLib.h>
#include <Library/BlMemoryAllocationLib.h>
#include <Library/SecureBootLib.h>
#include <Base.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/LoaderPerformanceLib.h>

#define SIZE_10MB (SIZE_8MB + SIZE_2MB)
#define MAX_ITER 4
#define MAX_DIGEST_BUFFER_SIZE SHA384_DIGEST_SIZE
#define CRYPTO_MEASURE_POINT_ID 0x5000
struct IterationLength {
    UINT32 Length;
    UINT8 Desc[7];
};

const struct IterationLength Iter_premem[] = {{ SIZE_1KB,   "1 KB"   },
                                       { SIZE_32KB,  "32 KB"  },
                                       { SIZE_64KB,  "64 KB"  },
                                       { SIZE_128KB, "128 KB" }};

const struct IterationLength Iter_postmem[] = {{ SIZE_64KB,  "64 KB"  },
                                       { SIZE_256KB, "256 KB" },
                                       { SIZE_1MB,   "1 MB"   },
                                       { SIZE_10MB,  "10 MB"  }};

enum Algorithm {
    SHA256,
    SHA384,
    SM3,
    MAX_ALGO
};

static const char* AlgorithmName (enum Algorithm Alg) {
    if (Alg == SHA256)
        return "SHA256";
    else if (Alg == SHA384)
        return "SHA384";
    else if (Alg == SM3)
        return "SM3";
    else
        return NULL;
}

VOID EFIAPI CryptoPerfTest (
)
{
    UINT16 MeasurePointId;
    UINT8 DigestBuffer[MAX_DIGEST_BUFFER_SIZE];
    UINT8 *Data = NULL;
    UINT8 Index;
    UINT16 Stage;
    UINT16 Alg;
    const struct IterationLength *Iter;

    if (GetLoaderStage() < LOADER_STAGE_2) {
        Iter = Iter_premem;
        Data = (UINT8*)(UINTN)FixedPcdGet32(PcdStage1ABase);
    } else {
        Iter = Iter_postmem;
        Data = (UINT8*) AllocateTemporaryMemory (Iter[MAX_ITER - 1].Length);
        if (Data == NULL)
            return;

        GenerateRandomNumbers((CHAR8 *)Data, Iter[MAX_ITER - 1].Length);
    }

    Stage = GetLoaderStage() < 2 ? 0 : 1;
    MeasurePointId = (UINT16)(CRYPTO_MEASURE_POINT_ID | (Stage << 8));
    AddMeasurePoint (MeasurePointId);
    for (Alg = 0; Alg < MAX_ALGO; Alg++) {
        MeasurePointId = (UINT16)((MeasurePointId & 0xFF00) | (Alg << 4));
        for (Index = 0; Index < MAX_ITER; Index++) {
            MeasurePointId++;
            if (Alg == SHA256) {
                Sha256 (Data, Iter[Index].Length, DigestBuffer);
                AddMeasurePoint (MeasurePointId);
            } else if (Alg == SHA384) {
                Sha384 (Data, Iter[Index].Length, DigestBuffer);
                AddMeasurePoint (MeasurePointId);
            } else if (Alg == SM3){
                Sm3 (Data, Iter[Index].Length, DigestBuffer);
                AddMeasurePoint (MeasurePointId);
            }
        }
    }
    if (Data != NULL)
        FreeTemporaryMemory(Data);
}

VOID EFIAPI CryptoPerfTestPrintResult (
)
{
    UINT8 Index;
    UINT64 TsTicks, TimeStamp1, TimeStamp2;
    BL_PERF_DATA *PerfData = GetPerfDataPtr();
    UINT8 Stage, Algorithm, Iter;
    UINT16 Id;

    ShellPrint (L"!!! IPP CRYPTO PERFORMANCE MESUREMENT !!!  \n");
    ShellPrint (L" Usecase                          | Delta (µs)      \n");
    ShellPrint (L"----------------------------------+----------------+\n");

    TimeStamp1 = 0;
    for (Index = 0; Index < PerfData->PerfIndex; Index++) {
        Id = ((UINT16 *)&(PerfData->TimeStamp[Index]))[3];
        if ((Id & 0xF000) == CRYPTO_MEASURE_POINT_ID) {
            if (!(Id & 0x00FF)) {
                TimeStamp1 = PerfData->TimeStamp[Index];
                ((UINT16 *)&TimeStamp1)[3] = 0;
                continue;
            }
            Stage = (Id >> 8) & 0x0F;
            Algorithm = (Id >> 4) & 0x0F;
            Iter = (Id & 0x0F) - 1;
            TimeStamp2 = PerfData->TimeStamp[Index];
            ((UINT16 *)&TimeStamp2)[3] = 0;
            TsTicks = TimeStamp2 - TimeStamp1;
            ShellPrint (L" %6a for %-8a - %8a   | %7d µs \n",
                        AlgorithmName(Algorithm),
                        Stage == 0? Iter_premem[Iter].Desc : Iter_postmem[Iter].Desc,
                        Stage == 0? "PREMEM" : "POSTMEM",
                        TimeStampTickToMicroSecond(TsTicks));
            TimeStamp1 = TimeStamp2;
        }
    }
    ShellPrint (L"----------------------------------+----------------+\n");
}
