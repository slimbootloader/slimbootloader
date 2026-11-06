/** @file
  FAT file system access routines for FAT recovery PEIM

Copyright (c) 2006 - 2019, Intel Corporation. All rights reserved.<BR>

SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "FatLitePeim.h"

#define FAT_CASE_NAME_LOWER     0x08
#define FAT_CASE_EXT_LOWER      0x10


/*
 * Check if the boot sector's media field is valid
 *
 * Based on fat_valid_media() from Linux kernel's include/linux/msdos_fs.h
 */
STATIC inline
BOOLEAN
FatValidMedia (
  IN UINT8  Media
  )
{
  return (Media >= 0xf8 || Media == 0xf0);
}

/**
  Converts unicode characters to lower case characters.

  @param  Str    A pointer to a Null-terminated string.

**/
STATIC
VOID
UnicodeStrLwr (
  IN OUT CHAR16                       *Str
  )
{
  while (*Str != 0) {
    if ('A' <= *Str && *Str <= 'Z') {
      *Str = (CHAR16)(*Str + 0x20);
    }
    Str += 1;
  }
}

/**
  Computes the standard FAT LFN alias checksum over an 11-byte 8.3
  short-name entry (FileName+Extension), matching the algorithm every
  FAT-compliant OS uses to bind a chain of long-file-name directory
  entries to the short entry that terminates the chain. Each LFN slot
  carries this checksum computed over the *short* entry it belongs to,
  precisely so the parser can detect a long-name chain that has been
  desynchronized, truncated, spliced, or otherwise tampered with even
  though every individual entry still looks structurally valid.

  @param  ShortName   Pointer to the 11-byte FileName[11] field of the
                       short directory entry.

  @retval             The 8-bit checksum.

**/
STATIC
UINT8
FatLfnChecksum (
  IN  CONST CHAR8  *ShortName
  )
{
  UINT8  Checksum;
  UINTN  Index;

  Checksum = 0;
  for (Index = 0; Index < 11; Index++) {
    Checksum = (UINT8) (((Checksum & 1) ? 0x80 : 0) + (Checksum >> 1) + (UINT8) ShortName[Index]);
  }

  return Checksum;
}

/**
  Check if there is a valid FAT in the corresponding Block device
  of the volume and if yes, fill in the relevant fields for the
  volume structure. Note there should be a valid Block device number
  already set.

  @param  PrivateData            Global memory map for accessing global
                                 variables.
  @param  Volume                 On input, the BlockDeviceNumber field of the
                                 Volume  should be a valid value. On successful
                                 output, all  fields except the VolumeNumber
                                 field is initialized.

  @retval EFI_SUCCESS            A FAT is found and the volume structure is
                                 initialized.
  @retval EFI_NOT_FOUND          There is no FAT on the corresponding device.
  @retval EFI_DEVICE_ERROR       There is something error while accessing device.

**/
EFI_STATUS
FatGetBpbInfo (
  IN      PEI_FAT_PRIVATE_DATA  *PrivateData,
  IN OUT  PEI_FAT_VOLUME        *Volume
  )
{
  EFI_STATUS              Status;
  PEI_FAT_BOOT_SECTOR     Bpb;
  PEI_FAT_BOOT_SECTOR_EX  BpbEx;
  UINT32                  Sectors;
  UINT32                  SectorsPerFat;
  UINT32                  RootDirSectors;
  UINT64                  FatLba;
  UINT64                  RootLba;
  UINT64                  FirstClusterLba;
  UINT8                   BootSignature[2];

  //
  // Read in the BPB
  //
  Status = FatReadDisk (
             PrivateData,
             Volume->BlockDeviceNo,
             0,
             sizeof (PEI_FAT_BOOT_SECTOR_EX),
             &BpbEx
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Fail fast: every valid boot sector, regardless of FAT12/16/32, must
  // end with the 0x55AA boot sector signature at byte offset 510-511 of
  // the first physical sector. Checking this cheap, well-known marker up
  // front rejects non-FAT/garbage sectors before any further attacker-
  // controlled BPB field is trusted, mirroring the very first check
  // U-Boot's fat_set_blk_dev() performs.
  //
  Status = FatReadDisk (
             PrivateData,
             Volume->BlockDeviceNo,
             510,
             sizeof (BootSignature),
             BootSignature
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (BootSignature[0] != 0x55 || BootSignature[1] != 0xAA) {
    return EFI_NOT_FOUND;
  }

  CopyMem (
    (UINT8 *) (&Bpb),
    (UINT8 *) (&BpbEx),
    sizeof (PEI_FAT_BOOT_SECTOR)
    );

  Volume->FatType = FatUnknown;

  Sectors         = Bpb.Sectors;
  if (Sectors == 0) {
    Sectors = Bpb.LargeSectors;
  }

  SectorsPerFat = Bpb.SectorsPerFat;
  if (SectorsPerFat == 0) {
    SectorsPerFat   = BpbEx.LargeSectorsPerFat;
    Volume->FatType = Fat32;
  }
  //
  // Filter out those not a FAT
  //
  if (Bpb.Ia32Jump[0] != 0xe9 && Bpb.Ia32Jump[0] != 0xeb && Bpb.Ia32Jump[0] != 0x49) {
    return EFI_NOT_FOUND;
  }

  if (Bpb.ReservedSectors == 0 || Bpb.NoFats == 0 || Sectors == 0 || Bpb.SectorSize == 0) {
    return EFI_NOT_FOUND;
  }

  //
  // SectorSize must be a power of 2 within the range the FAT spec and
  // real hardware actually use (512-4096 bytes). Bpb.SectorSize was only
  // checked for being non-zero above; left unchecked it is an arbitrary
  // attacker-controlled UINT16 (e.g. 3 or 65535) that feeds directly into
  // Volume->ClusterSize and every subsequent offset/size calculation.
  // Mirrors U-Boot's is_bootsector_valid() sector_size check.
  //
  if (Bpb.SectorSize < 512 || Bpb.SectorSize > 4096 ||
      (Bpb.SectorSize & (Bpb.SectorSize - 1)) != 0) {
    return EFI_NOT_FOUND;
  }

  if (Bpb.SectorsPerCluster != 1 &&
      Bpb.SectorsPerCluster != 2 &&
      Bpb.SectorsPerCluster != 4 &&
      Bpb.SectorsPerCluster != 8 &&
      Bpb.SectorsPerCluster != 16 &&
      Bpb.SectorsPerCluster != 32 &&
      Bpb.SectorsPerCluster != 64 &&
      Bpb.SectorsPerCluster != 128
      ) {
    return EFI_NOT_FOUND;
  }

  if (Volume->FatType == Fat32 && (SectorsPerFat == 0 || BpbEx.FsVersion != 0)) {
    return EFI_NOT_FOUND;
  }

  if (!FatValidMedia (Bpb.Media)) {
    return EFI_NOT_FOUND;
  }

  if (Volume->FatType != Fat32 && Bpb.RootEntries == 0) {
    return EFI_NOT_FOUND;
  }

  //
  // For FAT12/16, the root directory has a fixed number of 32-byte
  // entries and must occupy whole sectors: RootEntries * 32 must divide
  // evenly by SectorSize. An unaligned RootEntries value is a potential
  // sign of a corrupted/malicious BPB and would otherwise leave the computed
  // RootDirSectors later on silently rounded, shifting FirstClusterLba/data-area
  // math away from what the volume actually describes. Mirrors U-Boot's
  // is_bootsector_valid() dir_entries alignment check.
  //
  if (Volume->FatType != Fat32 &&
      (Bpb.RootEntries % (Bpb.SectorSize / sizeof (FAT_DIRECTORY_ENTRY))) != 0) {
    return EFI_NOT_FOUND;
  }

  //
  // If this is fat32, refuse to mount mirror-disabled volumes
  //
  if (Volume->FatType == Fat32 && ((BpbEx.ExtendedFlags & 0x80) != 0)) {
    return EFI_NOT_FOUND;
  }
  //
  // Fill in the volume structure fields
  // (Sectors & SectorsPerFat is computed earlier already)
  //
  Volume->ClusterSize = Bpb.SectorSize * Bpb.SectorsPerCluster;
  Volume->RootEntries = Bpb.RootEntries;
  Volume->SectorSize  = Bpb.SectorSize;

  //
  // Ceiling on ClusterSize (see defination of PEI_FAT_MAX_CLUSTER_SIZE_BYTES for further reasoning)
  //
  if (Volume->ClusterSize > PEI_FAT_MAX_CLUSTER_SIZE_BYTES) {
    return EFI_NOT_FOUND;
  }

  RootDirSectors = ((Volume->RootEntries * sizeof (FAT_DIRECTORY_ENTRY)) + (Volume->SectorSize - 1)) / Volume->SectorSize;

  FatLba                  = Bpb.ReservedSectors;

  // Cast to UINT64 to prevent 32-bit overflow before addition
  RootLba                 = (UINT64)Bpb.NoFats * SectorsPerFat + FatLba;
  FirstClusterLba         = RootLba + RootDirSectors;

  Volume->VolumeSize      = MultU64x32 (Sectors, Volume->SectorSize);
  Volume->FatPos          = MultU64x32 (FatLba, Volume->SectorSize);
  Volume->RootDirPos      = MultU64x32 (RootLba, Volume->SectorSize);
  Volume->FirstClusterPos = MultU64x32 (FirstClusterLba, Volume->SectorSize);
  if (FirstClusterLba >= Sectors) {
    return EFI_NOT_FOUND;
  }

  //
  // Reject any BPB that claims a volume larger than the block device.
  //
  // The BPB's Sectors/LargeSectors field, and everything derived from it
  // (including MaxCluster), comes from the boot sector and is therefore
  // media-controlled. A crafted BPB could claim billions of sectors on a small
  // device. That would inflate MaxCluster and let a malicious FAT table keep
  // the chain-walking loops in FatGetNextCluster()/FatSetFilePos() running
  // for a very long time, stalling boot (denial of service).
  //
  // Cross-check the claimed size against the capacity the block device itself
  // reports.
  //
  if (Volume->VolumeSize > MultU64x32 (PrivateData->BlockDevice[Volume->BlockDeviceNo].LastBlock + 1,
                                        PrivateData->BlockDevice[Volume->BlockDeviceNo].BlockSize)) {
    return EFI_NOT_FOUND;
  }

  Volume->MaxCluster      = (UINT32) (Sectors - FirstClusterLba) / Bpb.SectorsPerCluster;
  Volume->RootDirCluster  = BpbEx.RootDirFirstCluster;

  //
  // If this is not a fat32, determine if it's a fat16 or fat12
  //
  if (Volume->FatType != Fat32) {

    if (Volume->MaxCluster >= 65525) {
      return EFI_NOT_FOUND;
    }

    Volume->FatType = Volume->MaxCluster < 4085 ? Fat12 : Fat16;
  }

  return EFI_SUCCESS;
}


/**
  Gets the next cluster in the cluster chain

  @param  PrivateData            Global memory map for accessing global variables
  @param  Volume                 The volume
  @param  Cluster                The cluster
  @param  NextCluster            The cluster number of the next cluster

  @retval EFI_SUCCESS            The address is got
  @retval EFI_INVALID_PARAMETER  ClusterNo exceeds the MaxCluster of the volume.
  @retval EFI_DEVICE_ERROR       Read disk error

**/
EFI_STATUS
FatGetNextCluster (
  IN  PEI_FAT_PRIVATE_DATA  *PrivateData,
  IN  PEI_FAT_VOLUME        *Volume,
  IN  UINT32                Cluster,
  OUT UINT32                *NextCluster
  )
{
  EFI_STATUS  Status;
  UINT64      FatEntryPos;
  UINT32      Dummy;

  *NextCluster = 0;

  if (Volume->FatType == Fat32) {
    if (Cluster >= Volume->MaxCluster + 2) {
      return EFI_INVALID_PARAMETER;
    }

    FatEntryPos = Volume->FatPos + MultU64x32 (4, Cluster);

    Status      = FatReadDisk (PrivateData, Volume->BlockDeviceNo, FatEntryPos, 4, NextCluster);
    *NextCluster &= 0x0fffffff;

    //
    // Pad high bits for our FAT_CLUSTER_... macro definitions to work
    //
    if ((*NextCluster) >= 0x0ffffff7) {
      *NextCluster |= (-1 & ~0xf);
    }

  } else if (Volume->FatType == Fat16) {
    //
    // Make sure we have enough space for FAT16 entry (2 bytes)
    //
    if (Cluster >= Volume->MaxCluster + 2) {
      return EFI_INVALID_PARAMETER;
    }

    FatEntryPos = Volume->FatPos + MultU64x32 (2, Cluster);

    Status      = FatReadDisk (PrivateData, Volume->BlockDeviceNo, FatEntryPos, 2, NextCluster);

    //
    // Pad high bits for our FAT_CLUSTER_... macro definitions to work
    //
    if ((*NextCluster) >= 0xfff7) {
      *NextCluster |= (-1 & ~0xf);
    }

  } else {
    //
    // Make sure we have enough space for FAT12 entry (1.5 bytes)
    //
    if (Cluster >= Volume->MaxCluster + 2) {
      return EFI_INVALID_PARAMETER;
    }

    FatEntryPos = Volume->FatPos + DivU64x32Remainder (MultU64x32 (3, Cluster), 2, &Dummy);

    Status      = FatReadDisk (PrivateData, Volume->BlockDeviceNo, FatEntryPos, 2, NextCluster);

    if ((Cluster & 0x01) != 0) {
      *NextCluster = (*NextCluster) >> 4;
    } else {
      *NextCluster = (*NextCluster) & 0x0fff;
    }
    //
    // Pad high bits for our FAT_CLUSTER_... macro definitions to work
    //
    if ((*NextCluster) >= 0x0ff7) {
      *NextCluster |= (-1 & ~0xf);
    }
  }

  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;

}


/**
  Charge cluster-chain hops against the file's lifetime hop budget.

  A valid, acyclic chain holds at most MaxCluster distinct clusters
  (numbered 2 .. MaxCluster + 1). The budget allows MaxCluster + 3 hops in
  total; exceeding it means a cluster was revisited (cycle) or the chain is
  corrupt. Charging Hops at once is equivalent to charging one hop Hops
  times.

  @param  File     The file whose chain is being walked.
  @param  Hops     Number of hops to charge.

  @retval TRUE     The hops were charged.
  @retval FALSE    The budget would be exceeded; nothing was charged.

**/
STATIC
BOOLEAN
FatChargeClusterHops (
  IN  PEI_FAT_FILE  *File,
  IN  UINT32        Hops
  )
{
  if ((UINT64) File->TotalClusterHops + Hops > (UINT64) File->Volume->MaxCluster + 3) {
    DEBUG ((DEBUG_ERROR, "FatSetFilePos: cluster chain hop limit exceeded, possible cyclic/malicious FAT\n"));
    return FALSE;
  }

  File->TotalClusterHops += Hops;
  return TRUE;
}


/**
  Set a file's CurrentPos and CurrentCluster, then compute StraightReadAmount.

  @param  PrivateData            the global memory map
  @param  File                   the file
  @param  Pos                    the Position which is offset from the file's
                                 CurrentPos

  @retval EFI_SUCCESS            Success.
  @retval EFI_INVALID_PARAMETER  Pos is beyond file's size.
  @retval EFI_DEVICE_ERROR       Something error while accessing media.

**/
EFI_STATUS
FatSetFilePos (
  IN  PEI_FAT_PRIVATE_DATA  *PrivateData,
  IN  PEI_FAT_FILE          *File,
  IN  UINT32                Pos
  )
{
  EFI_STATUS  Status;
  UINT32      AlignedPos;
  UINT32      Offset;
  UINT32      Cluster;
  UINT32      PrevCluster;
  UINT32      HopCount;
  UINT32      Hops;
  UINT32      TargetPos;

  if (File->IsFixedRootDir) {

    if (Pos >= MultU64x32 (File->Volume->RootEntries, 32) - File->CurrentPos) {
      return EFI_INVALID_PARAMETER;
    }

    File->CurrentPos += Pos;
    File->StraightReadAmount = (UINT32) (MultU64x32 (File->Volume->RootEntries, 32) - File->CurrentPos);

  } else {

    if (Pos > MAX_UINT32 - File->CurrentPos) {
      return EFI_INVALID_PARAMETER;
    }

    DivU64x32Remainder (File->CurrentPos, File->Volume->ClusterSize, &Offset);

    //
    // Fast path: the target lies inside the current straight-run window.
    //
    // While StraightRunValid is TRUE the window covers the bytes
    // [CurrentPos, CurrentPos + StraightReadAmount). They lie in the clusters
    // CurrentCluster, CurrentCluster + 1, ..., each verified by the look-ahead
    // below to be the FAT successor of the one before it and below
    // MaxCluster + 2. StraightReadAmount is never 0 while the window is valid.
    //
    // Because Pos < StraightReadAmount, the target cluster is
    // CurrentCluster + Hops, so it can be reached without re-reading the FAT.
    // The hops are still charged against the lifetime hop budget, so cycle
    // detection sees exactly the same count as the chain walk below. The
    // window end does not move, so the invariant still holds afterwards.
    //
    // Without this, every cluster boundary crossed by small reads (for
    // example 32-byte directory entries) would rebuild the window from
    // scratch, costing up to PEI_FAT_MAX_STRAIGHT_RUN_CLUSTERS FAT lookups per
    // cluster instead of one rebuild per window.
    //
    // Pos == StraightReadAmount lands on the cluster after the window, which
    // has not been verified, so it takes the slow path.
    //
    if (File->StraightRunValid && (Pos < File->StraightReadAmount)) {
      Hops = (Offset + Pos) / File->Volume->ClusterSize;
      if (Hops != 0) {
        //
        // Defense in depth: unreachable while the invariant above holds.
        //
        if ((File->CurrentCluster > MAX_UINT32 - Hops) ||
            (File->CurrentCluster + Hops >= File->Volume->MaxCluster + 2)) {
          File->StraightRunValid = FALSE;
          return EFI_DEVICE_ERROR;
        }

        if (!FatChargeClusterHops (File, Hops)) {
          return EFI_DEVICE_ERROR;
        }

        File->CurrentCluster += Hops;
      }

      File->CurrentPos         += Pos;
      File->StraightReadAmount -= Pos;
      return EFI_SUCCESS;
    }

    //
    // Slow path: walk the chain to the target, then rebuild the window. The
    // window is invalid until the rebuild completes, so an error part-way
    // through can never leave a stale window behind for the fast path.
    //
    File->StraightRunValid = FALSE;
    AlignedPos             = (UINT32) File->CurrentPos - (UINT32) Offset;

    //
    // Cycle detection and defense-in-depth against corrupt or malicious FAT chains.
    //
    // Every hop, here and in the fast path above, is charged against
    // File->TotalClusterHops (see FatChargeClusterHops()). The budget covers
    // the whole lifetime of the File object rather than a single call. A
    // per-call limit can be bypassed. For example, a directory scan such as
    // FatReadNextDirectoryEntry reads 32 bytes at a time, so each call takes
    // at most one hop (only when it crosses a cluster boundary), yet the same
    // File object is walked cluster by cluster across many calls.
    //
    // This loop advances only as far as the caller has actually consumed file
    // content. A legitimately large, non-cyclic file will therefore produce a
    // large hop count too, and that is expected. Other DoS vectors that depend
    // on volume are bounded separately:
    //   - Oversized directory scans:            PEI_FAT_MAX_DIR_SIZE, PEI_FAT_MAX_DIR_ENTRY_COUNT
    //   - Oversized single-shot read-ahead:     PEI_FAT_MAX_STRAIGHT_RUN_CLUSTERS
    //
    TargetPos = (UINT32) File->CurrentPos + Pos;
    Hops      = 0;

    while
    (
      !FAT_CLUSTER_FUNCTIONAL (File->CurrentCluster) &&
      TargetPos >= AlignedPos &&
      (TargetPos - AlignedPos) >= File->Volume->ClusterSize
      ) {
      if (!FatChargeClusterHops (File, 1)) {
        return EFI_DEVICE_ERROR;
      }
      Hops++;
      AlignedPos += File->Volume->ClusterSize;
      Status = FatGetNextCluster (
                 PrivateData,
                 File->Volume,
                 File->CurrentCluster,
                 &File->CurrentCluster
                 );
      if (EFI_ERROR (Status)) {
        return EFI_DEVICE_ERROR;
      }
    }

    if (FAT_CLUSTER_FUNCTIONAL (File->CurrentCluster)) {
      //
      // Reading the last byte of a file whose size is an exact multiple of
      // the cluster size advances the position onto the end-of-chain marker.
      // Accept this one case: the walk in this call stepped from a data
      // cluster onto a proper end-of-chain value and the target is exactly
      // the start of that (nonexistent) cluster. StraightRunValid stays
      // FALSE, and a window cannot be rebuilt from an end-of-chain cluster,
      // so any further read on this handle fails in FatReadFile().
      //
      // Seeking past the end of the chain, or onto a free, reserved or bad
      // cluster, is still an error.
      //
      if ((Hops != 0) &&
          FAT_CLUSTER_END_OF_CHAIN (File->CurrentCluster) &&
          (TargetPos == AlignedPos)) {
        File->CurrentPos         = TargetPos;
        File->StraightReadAmount = 0;
        return EFI_SUCCESS;
      }

      return EFI_INVALID_PARAMETER;
    }

    File->CurrentPos += Pos;

    //
    // Compute the size of the straight run: the number of consecutive clusters
    // the file occupies from the current cluster. FatReadFile() uses it to read
    // them in a single batch.
    //
    // The look-ahead stops at the first non-contiguous cluster
    // (Cluster != PrevCluster + 1) and never modifies File->CurrentCluster.
    // It therefore cannot walk a cycle. A cluster stays in the window only if
    // FatGetNextCluster() accepts it (so it is below MaxCluster + 2) and its
    // predecessor links to it; any failure aborts without validating the
    // window. The fast path above relies on this.
    //
    // A crafted but valid volume can still hold a very long sequential chain,
    // for example a million clusters behind a tiny file. Walking all of it
    // would cost an unbounded number of disk reads just to size a batch. To
    // prevent this, the look-ahead is capped at
    // min (MaxCluster + 2, PEI_FAT_MAX_STRAIGHT_RUN_CLUSTERS) and simply stops
    // (it does not fail) when it reaches the cap.
    //
    // The cap only shrinks the batch size. FatReadFile() still reads arbitrarily
    // large files correctly, using additional bounded reads.
    //
    File->StraightReadAmount  = 0;
    Cluster                   = File->CurrentCluster;
    HopCount                  = 0;
    while (!FAT_CLUSTER_FUNCTIONAL (Cluster)) {
      if (File->StraightReadAmount > MAX_UINT32 - File->Volume->ClusterSize) {
        break;
      }
      if (HopCount++ > ((File->Volume->MaxCluster + 2 < (UINT32) PEI_FAT_MAX_STRAIGHT_RUN_CLUSTERS) ?
                        File->Volume->MaxCluster + 2 : (UINT32) PEI_FAT_MAX_STRAIGHT_RUN_CLUSTERS)) {
        break;
      }
      File->StraightReadAmount += File->Volume->ClusterSize;
      PrevCluster = Cluster;
      Status      = FatGetNextCluster (PrivateData, File->Volume, Cluster, &Cluster);
      if (EFI_ERROR (Status)) {
        return EFI_DEVICE_ERROR;
      }

      if (Cluster != PrevCluster + 1) {
        break;
      }
    }
    File->StraightRunValid = TRUE;

    DivU64x32Remainder (File->CurrentPos, File->Volume->ClusterSize, &Offset);
    File->StraightReadAmount -= (UINT32) Offset;

  }

  return EFI_SUCCESS;
}


/**
  Reads file data. Updates the file's CurrentPos.

  @param  PrivateData            Global memory map for accessing global variables
  @param  File                   The file.
  @param  Size                   The amount of data to read.
  @param  Buffer                 The buffer storing the data.

  @retval EFI_SUCCESS            The data is read.
  @retval EFI_INVALID_PARAMETER  File is invalid.
  @retval EFI_DEVICE_ERROR       Something error while accessing media.

**/
EFI_STATUS
FatReadFile (
  IN  PEI_FAT_PRIVATE_DATA  *PrivateData,
  IN  PEI_FAT_FILE          *File,
  IN  UINTN                 Size,
  OUT VOID                  *Buffer
  )
{
  EFI_STATUS  Status;
  CHAR8       *BufferPtr;
  UINT32      Offset;
  UINT64      PhysicalAddr;
  UINTN       Amount;

  BufferPtr = Buffer;

  if (File->IsFixedRootDir) {
    //
    // This is the fixed root dir in FAT12 and FAT16
    //
    if (File->CurrentPos + Size > File->Volume->RootEntries * sizeof (FAT_DIRECTORY_ENTRY)) {
      return EFI_INVALID_PARAMETER;
    }

    Status = FatReadDisk (
               PrivateData,
               File->Volume->BlockDeviceNo,
               File->Volume->RootDirPos + File->CurrentPos,
               Size,
               Buffer
               );
    File->CurrentPos += (UINT32) Size;
    return Status;

  } else {

    if ((File->Attributes & FAT_ATTR_DIRECTORY) == 0) {
      Size = Size < (File->FileSize - File->CurrentPos) ? Size : (File->FileSize - File->CurrentPos);
    }
    //
    // This is a normal cluster based file
    //
    while (Size != 0) {
      DivU64x32Remainder (File->CurrentPos, File->Volume->ClusterSize, &Offset);

      //
      // Check for calculation CurrentCluster - 2 underflows in next step
      // if CurrentCluster is 0 or 1.
      //
      if (File->CurrentCluster < 2) {
        return EFI_DEVICE_ERROR;
      }

      //
      // A freshly opened sub-directory or file has no straight-run window
      // yet; build it before using StraightReadAmount. A zero-length seek
      // does not move CurrentPos or CurrentCluster. A handle positioned at
      // the end of its cluster chain (see FatSetFilePos()) has no window and
      // cannot rebuild one, so any further read fails here.
      //
      if (!File->StraightRunValid) {
        Status = FatSetFilePos (PrivateData, File, 0);
        if (EFI_ERROR (Status)) {
          return EFI_DEVICE_ERROR;
        }
      }

      Amount        = File->StraightReadAmount;
      Amount        = Size > Amount ? Amount : Size;
      //
      // A valid straight-run window is never empty. Fail instead of looping
      // forever without making progress if that invariant is ever broken.
      //
      if (Amount == 0) {
        return EFI_DEVICE_ERROR;
      }

      PhysicalAddr  = File->Volume->FirstClusterPos + MultU64x32 (File->Volume->ClusterSize, File->CurrentCluster - 2);

      Status = FatReadDisk (
                 PrivateData,
                 File->Volume->BlockDeviceNo,
                 PhysicalAddr + Offset,
                 Amount,
                 BufferPtr
                 );
      if (EFI_ERROR (Status)) {
        return EFI_DEVICE_ERROR;
      }
      //
      // Advance the file's current pos and current cluster
      //
      Status = FatSetFilePos (PrivateData, File, (UINT32) Amount);
      if (EFI_ERROR (Status)) {
        return EFI_DEVICE_ERROR;
      }

      BufferPtr += Amount;
      Size -= Amount;
    }

    return EFI_SUCCESS;
  }
}


/**
  Check whether a directory entry's declared FileSize can possibly be
  held by the volume.

  @param  Volume     The volume the entry belongs to.
  @param  FileSize   The FileSize field of the directory entry.

  @retval TRUE       FileSize is within the volume's physical capacity.
  @retval FALSE      FileSize cannot be represented on this volume.

**/
BOOLEAN
FatFileSizeValid (
  IN  PEI_FAT_VOLUME   *Volume,
  IN  UINT32           FileSize
  )
{
  return ((UINT64) FileSize <= MultU64x32 (Volume->MaxCluster, Volume->ClusterSize));
}

/**
  This function reads the next item in the parent directory and
  initializes the output parameter SubFile (CurrentPos is initialized to 0).
  The function updates the CurrentPos of the parent dir to after the item read.
  If no more items were found, the function returns EFI_NOT_FOUND.

  @param  PrivateData            Global memory map for accessing global variables
  @param  ParentDir              The parent directory.
  @param  Attributes             The file attribute (file or dir) to find.
  @param  SubFile                The File structure containing the sub file that
                                 is caught.

  @retval EFI_SUCCESS            The next sub file is obtained.
  @retval EFI_INVALID_PARAMETER  The ParentDir is not a directory.
  @retval EFI_NOT_FOUND          No more sub file exists.
  @retval EFI_DEVICE_ERROR       Something error while accessing media.

**/
EFI_STATUS
FatReadNextDirectoryEntry (
  IN  PEI_FAT_PRIVATE_DATA  *PrivateData,
  IN  PEI_FAT_FILE          *ParentDir,
  IN  UINT32                 Attributes,
  OUT PEI_FAT_FILE          *SubFile
  )
{
  EFI_STATUS          Status;
  FAT_DIRECTORY_ENTRY DirEntry;
  FAT_DIRECTORY_LFN   *LfnEntry;
  CHAR16              *Pos;
  CHAR16              BaseName[9];
  CHAR16              Ext[4];
  CHAR16             *LfnBufferPointer;
  UINT8               LfnOrdinal;
  UINT8               LfnChecksum;
  UINTN               LfnBufferLen;
  UINT32              DirEntryCount;
  UINT32              LfnEntryCount;
  BOOLEAN             LfnChainValid;
  BOOLEAN             UseLfn;

  LfnBufferLen  = 0;
  LfnChainValid = FALSE;
  LfnChecksum   = 0;
  ZeroMem ((UINT8 *) SubFile, sizeof (PEI_FAT_FILE));
  DirEntryCount = 0;

  //
  // Pick a valid directory entry
  //
  while (DirEntryCount < PEI_FAT_MAX_DIR_ENTRY_COUNT) {
    DirEntryCount++;
    //
    // Read one entry
    //
    LfnOrdinal    = 0;
    LfnEntryCount = 0;
    //
    // Capture whether the previous outer iteration completed a full,
    // ordinally-consistent LFN chain before it gets reset below -- this is
    // used after the short entry (found later in *this* iteration) is
    // accepted, to validate that the chain's alias checksum actually
    // matches this short entry rather than some unrelated/spliced one.
    //
    UseLfn = LfnChainValid;
    // Only a chain that completed validly last iteration may hand its name
    // to this entry; otherwise drop whatever LfnBufferLen/LongFileName held.
    if (!LfnChainValid) {
      LfnBufferLen = 0;
      SubFile->LongFileName[0] = 0;
    }
    LfnChainValid = FALSE;

    //
    // If it is LFN entry, read all of the following LFN entries.
    //
    do {
      LfnEntryCount++;
      if (LfnEntryCount > MAX_LFN_ENTRIES) {
        LfnOrdinal = 0;
        LfnBufferLen = 0;
        break;
      }

      //
      // Apply every raw slot against the per-directory budget.
      // The per-call DirEntryCount alone does not bound total
      // work across repeated calls or across LFN slots.
      //
      if (ParentDir->CurrentPos >= PEI_FAT_MAX_DIR_SIZE) {
        return EFI_NOT_FOUND;
      }

      Status = FatReadFile (PrivateData, ParentDir, 32, &DirEntry);
      if (EFI_ERROR (Status)) {
        return EFI_DEVICE_ERROR;
      }

      if (DirEntry.Attributes == FAT_ATTR_LFN) {
        LfnEntry = (FAT_DIRECTORY_LFN *)&DirEntry;
        //
        // Every LFN slot in a chain carries the alias checksum of the
        // short entry it belongs to. The first (LAST-flagged) slot sets the
        // expected checksum; any later slot that disagrees was spliced in
        // from another chain and invalidates it. The surviving checksum is
        // then verified against the short entry terminating this chain.
        //
        if ((LfnEntry->Ordinal & FAT_LFN_LAST) != 0) {
          LfnChecksum  = LfnEntry->Checksum;
          LfnOrdinal   = LfnEntry->Ordinal & (FAT_LFN_LAST - 1);
          if (LfnOrdinal > MAX_LFN_ENTRIES) {
            LfnOrdinal = 0;
          }
          LfnBufferLen = LfnOrdinal * LFN_CHAR_TOTAL;
        }
        if (((LfnEntry->Ordinal & (FAT_LFN_LAST - 1)) != LfnOrdinal) ||
            (LfnEntry->Checksum != LfnChecksum)) {
          // Unexpected LFN entry, skip it.
          LfnOrdinal   = 0;
          LfnBufferLen = 0;
        } else if (LfnOrdinal > 0) {
          LfnBufferPointer = SubFile->LongFileName + (LfnOrdinal - 1) * LFN_CHAR_TOTAL;
          CopyMem (LfnBufferPointer, LfnEntry->Name1, sizeof (CHAR16) * LFN_CHAR1_LEN);
          LfnBufferPointer += LFN_CHAR1_LEN;
          CopyMem (LfnBufferPointer, LfnEntry->Name2, sizeof (CHAR16) * LFN_CHAR2_LEN);
          LfnBufferPointer += LFN_CHAR2_LEN;
          CopyMem (LfnBufferPointer, LfnEntry->Name3, sizeof (CHAR16) * LFN_CHAR3_LEN);
          LfnBufferPointer += LFN_CHAR3_LEN;
          LfnOrdinal--;
          if (LfnOrdinal == 0) {
            LfnChainValid = TRUE;
          }
        }
      } else if (LfnOrdinal > 0) {
        LfnOrdinal = 0;
        LfnBufferLen = 0;
        break;
      }
    } while (LfnOrdinal > 0);

    if (DirEntry.Attributes == FAT_ATTR_LFN) {
      continue;
    }

    //
    // if this is a terminator dir entry, just return EFI_NOT_FOUND
    //
    if (DirEntry.FileName[0] == EMPTY_ENTRY_MARK) {
      return EFI_NOT_FOUND;
    }

    //
    // Search Directory only if Attributes has FAT_ATTR_DIRECTORY.
    // Otherwise, search files and directories
    //
    if ((Attributes & FAT_ATTR_DIRECTORY) && !(DirEntry.Attributes & FAT_ATTR_DIRECTORY)) {
      continue;
    }

    //
    // Skip Volume file
    //
    if (DirEntry.Attributes & FAT_ATTR_VOLUME_ID) {
      continue;
    }

    //
    // Skip entries whose declared size exceeds the volume's physical
    // capacity (see FatFileSizeValid()). Such entries are corrupt or
    // malicious; handing them out would let one 32-byte dir entry drive
    // an unbounded pre-auth memory allocation in consumers that size
    // buffers from FatFsGetFileSize().
    //
    if (!FatFileSizeValid (ParentDir->Volume, DirEntry.FileSize)) {
      continue;
    }

    //
    // If this not an invalid entry neither an empty entry, this is what we want.
    // otherwise we will start a new loop to continue to find something meaningful
    //
    if ((UINT8) DirEntry.FileName[0] != DELETE_ENTRY_MARK) {
      //
      // The long name accumulated in SubFile->LongFileName only belongs to
      // this short entry if the chain's alias checksum actually matches
      // it. A mismatch means the LFN chain was desynchronized, truncated,
      // or spliced (e.g. by a crafted/corrupted directory) -- fall back to
      // the short 8.3 name only rather than trusting an unrelated long
      // name, mirroring U-Boot's extract_vfat_name() checksum check.
      //
      if (UseLfn && SubFile->LongFileName[0] != 0) {
        if (FatLfnChecksum (DirEntry.FileName) != LfnChecksum) {
          SubFile->LongFileName[0] = 0;
        }
      }
      break;
    }
  }

  if (DirEntryCount >= PEI_FAT_MAX_DIR_ENTRY_COUNT) {
    return EFI_NOT_FOUND;
  }
  //
  // fill in the output parameter
  //
  EngFatToStr (8, DirEntry.FileName, BaseName);
  EngFatToStr (3, DirEntry.FileName + 8, Ext);

  //
  // Check CaseFlag
  //
  if (DirEntry.CaseFlag & FAT_CASE_NAME_LOWER) {
    UnicodeStrLwr (BaseName);
  }
  if (DirEntry.CaseFlag & FAT_CASE_EXT_LOWER) {
    UnicodeStrLwr (Ext);
  }

  Pos = (UINT16 *) SubFile->FileName;
  CopyMem ((UINT8 *) Pos, (UINT8 *) BaseName, 2 * (StrLen (BaseName) + 1));

  if (Ext[0] != 0) {
    Pos += StrLen (BaseName);
    *Pos = '.';
    Pos++;
    CopyMem ((UINT8 *) Pos, (UINT8 *) Ext, 2 * (StrLen (Ext) + 1));
  }

  SubFile->LongFileName[LfnBufferLen] = 0;

  SubFile->Attributes     = DirEntry.Attributes;
  SubFile->CurrentCluster = DirEntry.FileCluster;
  if (ParentDir->Volume->FatType == Fat32) {
    SubFile->CurrentCluster |= DirEntry.FileClusterHigh << 16;
  }

  SubFile->CurrentPos       = 0;
  SubFile->FileSize         = DirEntry.FileSize;
  SubFile->StartingCluster  = SubFile->CurrentCluster;
  SubFile->Volume           = ParentDir->Volume;

  //
  // in Pei phase, time parameters do not need to be filled for minimum use.
  //
  return Status;
}
