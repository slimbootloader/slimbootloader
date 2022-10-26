/** @file
  This file Multiboot-2 specification (implementation).

  Copyright (c) 2014 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/MultibootLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Guid/MemoryMapInfoGuid.h>
#include <Guid/GraphicsInfoHob.h>
#include "MultibootLibInternal.h"
#include "multiboot2.h"

VOID DumpMb2Header (CONST struct multiboot2_header *Mh);


/**
  Get multiboot header address

  Get multiboot header following Multboot spec 2.0.

  @param[in]  ImageAddr    Memory address of an image

  @retval MultibootHeader  Pointer to Multiboot header if found.
  @retval NULL             No valid Multiboot header found or requesting
                           features not support.
**/
CONST struct multiboot2_header *
EFIAPI
GetMultiboot2Header (
  IN  VOID                   *ImageAddr
  )
{
  UINT32                     AlignedAddress;
  struct multiboot2_header  *MbHeader;
  UINT32                     Offset;

  // Multiboot header must be 64-bit aligned.
  AlignedAddress = ALIGN_UP((UINT32)(UINTN)ImageAddr, MULTIBOOT2_HEADER_ALIGN);

  // Multiboot header must completely within the first 32K bytes of the image.
  for (Offset = 0; Offset < MULTIBOOT2_SEARCH - sizeof (struct multiboot2_header); Offset += MULTIBOOT2_HEADER_ALIGN) {
    MbHeader = (struct multiboot2_header *)(UINTN)(AlignedAddress + Offset);
    if ((MbHeader->magic == MULTIBOOT2_HEADER_MAGIC) &&
        (MbHeader->architecture == MULTIBOOT2_ARCHITECTURE_I386) &&
        (MbHeader->magic + MbHeader->architecture + MbHeader->header_length + MbHeader->checksum == 0)) {
      return (CONST struct multiboot2_header *) MbHeader;
    }
  }

  return NULL;
}

/**
  Check if it is Multiboot image

  @param[in]  ImageAddr    Memory address of an image

  @retval TRUE             Image is Multiboot 0.6.98 compliant image
  @retval FALSE            Not multiboot image
**/
BOOLEAN
EFIAPI
IsMultiboot2 (
  IN  VOID                   *ImageAddr
  )
{
  CONST struct multiboot2_header *MbHeader;

  MbHeader = GetMultiboot2Header (ImageAddr);
  if (MbHeader != NULL) {
    DumpMb2Header (MbHeader);
    return TRUE;
  }
  return FALSE;
}


/**
  Get memory map information.

  @retval   Memory Map Info pointer
**/
STATIC
MEMORY_MAP_INFO *
EFIAPI
GetMemoryMapInfo (
  VOID
  )
{
  EFI_HOB_GUID_TYPE          *GuidHob;

  GuidHob = GetNextGuidHob (&gLoaderMemoryMapInfoGuid, GetHobList());
  if (GuidHob == NULL) {
    ASSERT (GuidHob);
    return NULL;
  }

  return (MEMORY_MAP_INFO *)GET_GUID_HOB_DATA (GuidHob);
}


/**
  Init Multiboot-2 memory map.

  @param[out]  MbMmap         Multiboot memmap buffer
  @param[in]   MemoryMapInfo  Memmap buffer from boot loader
**/
VOID
InitMultiboot2Mmap (
  OUT     struct multiboot2_mmap_entry *MbMmap,
  IN      MEMORY_MAP_INFO              *MemoryMapInfo
  )
{
  UINT32                     Index;
  MEMORY_MAP_ENTRY           *MmapEntry;

  MmapEntry = &MemoryMapInfo->Entry[0];
  for (Index = 0; Index < MemoryMapInfo->Count; Index++) {
    MbMmap[Index].addr = MmapEntry[Index].Base;
    MbMmap[Index].len  = MmapEntry[Index].Size;
    MbMmap[Index].type = MmapEntry[Index].Type;
    MbMmap[Index].zero = 0;
  }
}


/**
  Setup the Multiboot info for boot usage.

  @param[in,out]   MultiBoot  Point to loaded Multiboot image structure
**/
VOID
EFIAPI
SetupMultiboot2Info (
  IN OUT MULTIBOOT_IMAGE     *MultiBoot
  )
{
  MULTIBOOT2_INFO *Mb2Info = &MultiBoot->Mb2Info;

  unsigned mb2_info_size_max = 0x1000;
  void *mb2_info = AllocatePool (mb2_info_size_max);
  if (mb2_info == NULL) {
    DEBUG ((DEBUG_INFO, "Multiboot-2 info buffer allocation Error\n"));
    ASSERT (mb2_info);
  }

  struct multiboot2_start_tag *mbi_start = (struct multiboot2_start_tag *)mb2_info;
  Mb2Info->StartTag = mbi_start;
  unsigned info_idx = sizeof(*mbi_start);

  /*
  ** Add tags:
  */
#define DECLARE_TAG(STRUCT, TAG, BASE, P) \
  STRUCT *TAG = (STRUCT *) ((char *)(BASE) + (P))
#define APPEND_TAG(P, TAG, TYPE, SIZE) do { \
  (TAG)->type = (TYPE); \
  (TAG)->size = (SIZE); \
  (P) += ALIGN_UP((TAG)->size, MULTIBOOT2_TAG_ALIGN); \
} while (0)

  // command line tag (1)
  if (MultiBoot->CmdFile.Size != 0)
  {
    DECLARE_TAG (struct multiboot2_tag_string, tag, mb2_info, info_idx);

    unsigned len = MultiBoot->CmdFile.Size;
    CopyMem(tag->string, MultiBoot->CmdFile.Addr, len);
    tag->string[len] = '\0';

    APPEND_TAG (info_idx, tag, MULTIBOOT2_TAG_TYPE_CMDLINE, sizeof(struct multiboot2_tag_string) + len + 1);
  }

  // bootloader name tag (2)
  {
    DECLARE_TAG (struct multiboot2_tag_string, tag, mb2_info, info_idx);

    UINT32 len = (UINT32) AsciiStrSize((CHAR8 *) mLoaderName);
    CopyMem(tag->string, mLoaderName, len);

    APPEND_TAG (info_idx, tag, MULTIBOOT2_TAG_TYPE_BOOT_LOADER_NAME, sizeof(struct multiboot2_tag_string) + len);
  }

  // module tags (3)
  for (unsigned idx = 0; idx < MultiBoot->MbModuleNumber; idx += 1)
  {
    DECLARE_TAG (struct multiboot2_tag_module, module_tag, mb2_info, info_idx);

    const MULTIBOOT_MODULE *mod = &MultiBoot->MbModule[idx];
    UINT32 len = (UINT32) AsciiStrSize((CHAR8 *) mod->String);

    module_tag->mod_start   = mod->Start;
    module_tag->mod_end     = mod->End;
    CopyMem(module_tag->cmdline, mod->String, len);

    APPEND_TAG (info_idx, module_tag, MULTIBOOT2_TAG_TYPE_MODULE, sizeof(struct multiboot2_tag_module) + len);
  }

  // basic memory info tag (4)
  {
    DECLARE_TAG (struct multiboot2_tag_basic_meminfo, tag, mb2_info, info_idx);

    // The amount of lower and upper memory size (in KB) will be updated
    // later since current memmap is not the final memmap.
    tag->mem_upper = 0;
    tag->mem_lower = 0;

    APPEND_TAG (info_idx, tag, MULTIBOOT2_TAG_TYPE_BASIC_MEMINFO, sizeof(struct multiboot2_tag_basic_meminfo));
  }

  // normal memory map tag (6)
  {
    DECLARE_TAG (struct multiboot2_tag_mmap, mmap_tag, mb2_info, info_idx);

    MEMORY_MAP_INFO *mmap_info = GetMemoryMapInfo();
    unsigned mb_mmap_count = mmap_info->Count;

    mmap_tag->entry_size = sizeof(struct multiboot2_mmap_entry);
    mmap_tag->entry_version = 0;
    InitMultiboot2Mmap (&mmap_tag->entries[0], mmap_info);

    APPEND_TAG (info_idx, mmap_tag, MULTIBOOT2_TAG_TYPE_MMAP,
                sizeof(struct multiboot2_tag_mmap) + sizeof(struct multiboot2_mmap_entry) * mb_mmap_count);
  }

  // framebuffer tag (8)
  EFI_HOB_GUID_TYPE *GuidHob = GetFirstGuidHob (&gEfiGraphicsInfoHobGuid);
  if (GuidHob != NULL)
  {
    DECLARE_TAG (struct multiboot2_tag_framebuffer, tag, mb2_info, info_idx);

    EFI_PEI_GRAPHICS_INFO_HOB  *GfxInfoHob = (EFI_PEI_GRAPHICS_INFO_HOB *)GET_GUID_HOB_DATA (GuidHob);
    tag->common.framebuffer_addr   = (UINT64)(UINTN)GfxInfoHob->FrameBufferBase;
    tag->common.framebuffer_pitch  = GfxInfoHob->GraphicsMode.PixelsPerScanLine * 4;
    tag->common.framebuffer_width  = GfxInfoHob->GraphicsMode.HorizontalResolution;
    tag->common.framebuffer_height = GfxInfoHob->GraphicsMode.VerticalResolution;
    tag->common.framebuffer_bpp    = 32;
    tag->common.framebuffer_type   = 1;
    tag->u.type1.framebuffer_red_field_position   = 0x10;
    tag->u.type1.framebuffer_red_mask_size        = 8;
    tag->u.type1.framebuffer_green_field_position = 0x08;
    tag->u.type1.framebuffer_green_mask_size      = 8;
    tag->u.type1.framebuffer_blue_field_position  = 0x00;
    tag->u.type1.framebuffer_blue_mask_size       = 8;

    APPEND_TAG (info_idx, &tag->common, MULTIBOOT2_TAG_TYPE_FRAMEBUFFER, sizeof(struct multiboot2_tag_framebuffer));
  }

  // end tag (0)
  {
    DECLARE_TAG (struct multiboot2_tag, end_tag, mb2_info, info_idx);
    APPEND_TAG (info_idx, end_tag, MULTIBOOT2_TAG_TYPE_END, sizeof(struct multiboot2_tag));
  }

#undef DECLARE_TAG
#undef APPEND_TAG

  ASSERT (info_idx <= mb2_info_size_max);

  mbi_start->size = info_idx;
  mbi_start->reserved = 0x00;

  /*
  ** Arrange for passing this data to the image.
  */
  MultiBoot->BootState.Eax = MULTIBOOT2_BOOTLOADER_MAGIC;
  MultiBoot->BootState.Ebx = (UINT32)(UINTN)mbi_start;
}


/**
  Update the memory info inside the Multiboot-2 info.

  @param[in,out]   MultiBoot     Point to loaded Multiboot-2 image structure
  @param[in]       RsvdMemBase   Reserved memory base address
  @param[in]       RsvdMemSize   Reserved memory size
  @param[in]       RsvdMemExtra  Extra space to add to the reserved memory region.
**/
VOID
EFIAPI
UpdateMultiboot2MemInfo (
  IN OUT MULTIBOOT_IMAGE     *MultiBoot,
  IN UINT64                   RsvdMemBase,
  IN UINT64                   RsvdMemSize,
  IN UINT32                   RsvdMemExtra
  )
{
  struct multiboot2_start_tag         *Mi = MultiBoot->Mb2Info.StartTag;
  struct multiboot2_tag_basic_meminfo *minfo_tag = NULL;
  struct multiboot2_tag_mmap          *mmap_tag = NULL;

  //
  //  Find the BASIC_MEMINFO and MMAP info tags; punt if not found (we created them).
  //
  for (unsigned off = sizeof(*Mi); off < Mi->size; ) {
    struct multiboot2_tag *tag = (void *) ((char *) Mi + off);

    if (tag->type == MULTIBOOT2_TAG_TYPE_BASIC_MEMINFO)
      minfo_tag = (struct multiboot2_tag_basic_meminfo *) tag;
    if (tag->type == MULTIBOOT2_TAG_TYPE_MMAP)
      mmap_tag = (struct multiboot2_tag_mmap *) tag;
    if (tag->type == MULTIBOOT2_TAG_TYPE_END)
      break;

    off += ALIGN_UP (MAX (tag->size, sizeof(*tag)), MULTIBOOT2_TAG_ALIGN);
  }
  if (minfo_tag == NULL || mmap_tag == NULL)
    ASSERT (minfo_tag && mmap_tag);

  //
  //  Iterate over the memory map entries and look for:
  //  - the given reserved region => extend it by the reqested extra size (from the beginning);
  //  - the region that is immediately below the reserved space => shrink it by the extra space;
  //  - usable memory regions starting at address 0x0 or 1M => record size in the basic mem info tag.
  //
  unsigned n_entries = (mmap_tag->size - sizeof(*mmap_tag)) / mmap_tag->entry_size;
  for (unsigned idx = 0; idx < n_entries; idx += 1) {
    struct multiboot2_mmap_entry *entry = &mmap_tag->entries[idx];

    DEBUG ((DEBUG_INFO, "\n\t%12llx %12llx %4d",
        mmap_tag->entries[idx].addr,
        mmap_tag->entries[idx].len,
        mmap_tag->entries[idx].type));

    if (entry->addr == RsvdMemBase && entry->len == RsvdMemSize) {
      entry->addr -= RsvdMemExtra;
      entry->len  += RsvdMemExtra;
    } else if (entry->addr + entry->len == RsvdMemBase) {
      entry->len  -= RsvdMemExtra;
    }

    if (entry->type == MEM_MAP_TYPE_RAM) {
      if (entry->addr == 0)
        minfo_tag->mem_lower = (UINT32) (RShiftU64 (entry->len, 10));
      else if (entry->addr == MB_ (1))
        minfo_tag->mem_upper = (UINT32) (RShiftU64 (entry->len, 10));
    }
  }
}


/**
  Parse Multiboot-2 image header tags and extract data required for setting up the image.

  @param[in] Mh   The Multiboot header to be parsed.

  @retval  RETURN_SUCCESS    All required information was found.
  @retval  Others            Some required information is not present.
**/
EFI_STATUS
EFIAPI
ParseMultiboot2Header (
  IN CONST struct multiboot2_header *Mh,
  OUT BOOLEAN  *AlignModules,
  OUT UINT8    **HeaderAddr,
  OUT UINT8    **LoadAddr,
  OUT UINT8    **LoadEnd,
  OUT UINT8    **BssEnd,
  OUT UINT32   *EntryPoint
  )
{
  *AlignModules = FALSE;

  UINT32 tags_needed = (1U << MULTIBOOT2_HEADER_TAG_ADDRESS)
                     | (1U << MULTIBOOT2_HEADER_TAG_ENTRY_ADDRESS);

  for (unsigned off = sizeof(*Mh); off < Mh->header_length ; )
  {
    const struct multiboot2_header_tag *htag = (void *) ((char *) Mh + off);

    switch (htag->type)
    {
    case MULTIBOOT2_HEADER_TAG_ADDRESS:               // 2
    {
      const struct multiboot2_header_tag_address *tag = (void *) ((char *) Mh + off);
      *HeaderAddr = (UINT8 *)(UINTN) tag->header_addr;
      *LoadAddr   = (UINT8 *)(UINTN) tag->load_addr;
      *LoadEnd    = (UINT8 *)(UINTN) tag->load_end_addr;
      *BssEnd     = (UINT8 *)(UINTN) tag->bss_end_addr;
    }
    break;
    case MULTIBOOT2_HEADER_TAG_ENTRY_ADDRESS:         // 3
    {
      const struct multiboot2_header_tag_entry_address *tag = (void *) ((char *) Mh + off);
      *EntryPoint = tag->entry_addr;
    }
    break;
    case MULTIBOOT2_HEADER_TAG_MODULE_ALIGN:          // 6
      *AlignModules = TRUE;
      break;

    case MULTIBOOT2_HEADER_TAG_END:                   // 0
    case MULTIBOOT2_HEADER_TAG_FRAMEBUFFER:           // 5
      // noted, ignore the tag.
      break;

//  case MULTIBOOT2_HEADER_TAG_INFORMATION_REQUEST:   // 1
//  case MULTIBOOT2_HEADER_TAG_CONSOLE_FLAGS:         // 4
//  case MULTIBOOT2_HEADER_TAG_EFI_BS:                // 7
//  case MULTIBOOT2_HEADER_TAG_ENTRY_ADDRESS_EFI32:   // 8
//  case MULTIBOOT2_HEADER_TAG_ENTRY_ADDRESS_EFI64:   // 9
//  case MULTIBOOT2_HEADER_TAG_RELOCATABLE:           // 10
    default:
      // unsupported or unknown tag, failure!
      return RETURN_UNSUPPORTED;
    }

    off += ALIGN_UP (MAX (htag->size, sizeof(*htag)), MULTIBOOT2_TAG_ALIGN);
    if (htag->type == MULTIBOOT2_TAG_TYPE_END)
      break;
    tags_needed &= ~(1U << htag->type);
  }

  return (tags_needed ? RETURN_UNSUPPORTED : RETURN_SUCCESS);
}

/**
  Setup Multiboot image and its boot info.

  @param[in,out] MultiBoot   Point to loaded Multiboot image structure

  @retval  RETURN_SUCCESS    Setup Multiboot image successfully
  @retval  Others            There is error when setup image
**/
EFI_STATUS
EFIAPI
SetupMultiboot2Image (
  IN OUT MULTIBOOT_IMAGE     *MultiBoot
  )
{
  EFI_STATUS                 Status;
  BOOLEAN                    AlignModules;
  UINT8                      *HeaderAddr;
  UINT8                      *LoadAddr;
  UINT8                      *LoadEnd;
  UINT8                      *BssEnd;
  UINT32                     EntryPoint;
  CONST struct multiboot2_header     *MbHeader;
  UINT32                      ImgOffset;
  UINT32                      ImgLength;
  UINT8                      *CopyStart;

  if (MultiBoot == NULL) {
    return RETURN_INVALID_PARAMETER;
  }

  MbHeader = GetMultiboot2Header (MultiBoot->BootFile.Addr);
  if (MbHeader == NULL) {
    return RETURN_LOAD_ERROR;
  }

  Status = ParseMultiboot2Header (MbHeader, &AlignModules, &HeaderAddr, &LoadAddr, &LoadEnd, &BssEnd, &EntryPoint);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (AlignModules) {
    // Other modules should be page (4KB) aligned
    Status = AlignMultibootModules (MultiBoot);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  ImgOffset  = (UINT32)((UINT8 *)MbHeader - (UINT8 *)MultiBoot->BootFile.Addr - (HeaderAddr - LoadAddr));
  ImgLength  = (UINT32)((LoadEnd == NULL) ? MultiBoot->BootFile.Size - ImgOffset : LoadEnd - LoadAddr);
  if ((ImgOffset >= MultiBoot->BootFile.Size) || (ImgOffset + ImgLength > MultiBoot->BootFile.Size)) {
    return RETURN_LOAD_ERROR;
  }

  DEBUG ((DEBUG_INFO, "Mb: LoadAddr=0x%p, LoadEnd=0x%p , BssEnd=0x%p, Size=0x%x\n", LoadAddr, LoadEnd, BssEnd, ImgLength));
  CopyStart = (UINT8 *)MultiBoot->BootFile.Addr + ImgOffset;
  CopyMem (LoadAddr, CopyStart, ImgLength);
  if ((BssEnd != NULL) && (LoadEnd != NULL)) {
    if (BssEnd > LoadEnd) {
      ZeroMem ((VOID *) LoadEnd, BssEnd - LoadEnd);
    }
  }

  SetupMultiboot2Info (MultiBoot);
  MultiBoot->BootState.EntryPoint = EntryPoint;
  return EFI_SUCCESS;
}


/**
  Print out the Multiboot header.

  @param[in]  Mh  The Multiboot header to be printed.
**/
VOID
DumpMb2Header (CONST struct multiboot2_header *Mh)
{
  DEBUG ((DEBUG_INFO, "\nDump MB2 header @%p:\n", Mh));

  DEBUG ((DEBUG_INFO, "- magic:             %8x\n", Mh->magic));
  DEBUG ((DEBUG_INFO, "- architecture:      %8x\n", Mh->architecture));
  DEBUG ((DEBUG_INFO, "- header_length:     %8x\n", Mh->header_length));
  DEBUG ((DEBUG_INFO, "- checksum:          %8x\n", Mh->checksum));

  for (unsigned off = sizeof(*Mh); off < Mh->header_length ; )
  {
    const struct multiboot2_header_tag *htag = (void *) ((char *) Mh + off);

    DEBUG ((DEBUG_INFO, "- tag @%02x [T=%2d F=%04x S=%2d]: ", off, htag->type, htag->flags, htag->size));
    switch (htag->type)
    {
    case MULTIBOOT2_HEADER_TAG_END:                   // 0
      DEBUG ((DEBUG_INFO, "<END>"));
      break;
    case MULTIBOOT2_HEADER_TAG_ADDRESS:               // 2
    {
      const struct multiboot2_header_tag_address *tag = (void *) ((char *) Mh + off);
      DEBUG ((DEBUG_INFO, "header @%#x, load to %#x-%#x, bss-end @%#x",
              tag->header_addr,
              tag->load_addr,
              tag->load_end_addr,
              tag->bss_end_addr));
    }
    break;
    case MULTIBOOT2_HEADER_TAG_ENTRY_ADDRESS:         // 3
    {
      const struct multiboot2_header_tag_entry_address *tag = (void *) ((char *) Mh + off);
      DEBUG ((DEBUG_INFO, "entry point: %#x", tag->entry_addr));
    }
    break;
    case MULTIBOOT2_HEADER_TAG_MODULE_ALIGN:          // 6
      DEBUG ((DEBUG_INFO, "align modules."));
      break;
    case MULTIBOOT2_HEADER_TAG_FRAMEBUFFER:           // 5
    {
      const struct multiboot2_header_tag_framebuffer *t5 = (void *) ((char *) Mh + off);
      DEBUG ((DEBUG_INFO, "(frame buffer) preferred graphics mode: width=%d, height=%d, depth=%d",
              t5->width, t5->height, t5->depth));
    }
    break;

//  case MULTIBOOT2_HEADER_TAG_INFORMATION_REQUEST:   // 1
//  case MULTIBOOT2_HEADER_TAG_CONSOLE_FLAGS:         // 4
//  case MULTIBOOT2_HEADER_TAG_EFI_BS:                // 7
//  case MULTIBOOT2_HEADER_TAG_ENTRY_ADDRESS_EFI32:   // 8
//  case MULTIBOOT2_HEADER_TAG_ENTRY_ADDRESS_EFI64:   // 9
//  case MULTIBOOT2_HEADER_TAG_RELOCATABLE:           // 10

    default:
      DEBUG ((DEBUG_INFO, "??"));
      break;
    }
    DEBUG ((DEBUG_INFO, "\n"));

    off += ALIGN_UP (MAX (htag->size, sizeof(*htag)), MULTIBOOT2_TAG_ALIGN);
    if (htag->type == MULTIBOOT2_TAG_TYPE_END)
      break;
  }
}

/**
  Print out the Multiboot-2 information block.

  @param[in]  Mi  The Multiboot-2 information block to be printed.
**/
VOID
DumpMb2Info (
  IN  CONST struct multiboot2_start_tag *Mi
  )
{
  DEBUG ((DEBUG_INFO, "\nDump MB2 info @%p (size=%d, reserved=%x):\n", Mi, Mi->size, Mi->reserved));
  if (Mi == NULL) {
    return;
  }

  for (unsigned off = sizeof(*Mi); off < Mi->size; )
  {
      const struct multiboot2_tag *tag = (void *) ((char *) Mi + off);

    DEBUG ((DEBUG_INFO, "- tag @%03x [T=%2d S=%2d]: ", off, tag->type, tag->size));
    switch (tag->type)
    {
    case MULTIBOOT2_TAG_TYPE_END:               // 0
      DEBUG ((DEBUG_INFO, "<END>"));
      break;
    case MULTIBOOT2_TAG_TYPE_CMDLINE:           // 1
    {
        struct multiboot2_tag_string *string_tag = (void *) ((char *) Mi + off);
      DEBUG ((DEBUG_INFO, "command line: '%a'",
              string_tag->string));
    }
    break;
    case MULTIBOOT2_TAG_TYPE_BOOT_LOADER_NAME:  // 2
    {
        struct multiboot2_tag_string *string_tag = (void *) ((char *) Mi + off);
      DEBUG ((DEBUG_INFO, "boot loader name: '%a'",
              string_tag->string));
    }
    break;
    case MULTIBOOT2_TAG_TYPE_MODULE:            // 3
    {
        struct multiboot2_tag_module *module_tag = (void *) ((char *) Mi + off);
      DEBUG ((DEBUG_INFO, "module: start %#x, end %#x, cmd line '%a'",
              module_tag->mod_start, module_tag->mod_end, module_tag->cmdline));
    }
    break;
    case MULTIBOOT2_TAG_TYPE_BASIC_MEMINFO:     // 4
    {
        struct multiboot2_tag_basic_meminfo *meminfo_tag = (void *) ((char *) Mi + off);
      DEBUG ((DEBUG_INFO, "basic memory info: %dK lower, %dK upper memory",
              meminfo_tag->mem_lower, meminfo_tag->mem_upper));
    }
    break;
    case MULTIBOOT2_TAG_TYPE_MMAP:              // 6
    {
        struct multiboot2_tag_mmap *mmap_tag = (void *) ((char *) Mi + off);
      unsigned n_entries = (mmap_tag->size - sizeof(*mmap_tag)) / mmap_tag->entry_size;
      DEBUG ((DEBUG_INFO, "memory map: %d entries (version=%d, size=%d):",
              n_entries, mmap_tag->entry_version, mmap_tag->entry_size));
      DEBUG ((DEBUG_INFO, "\n\t     address       length type\n\t------------ ------------ ----"));
      for (unsigned idx = 0; idx < n_entries; idx += 1)
      {
        DEBUG ((DEBUG_INFO, "\n\t%12llx %12llx %4d",
                mmap_tag->entries[idx].addr,
                mmap_tag->entries[idx].len,
                mmap_tag->entries[idx].type));
      }
    }
    break;
    case MULTIBOOT2_TAG_TYPE_FRAMEBUFFER:       // 8
    {
        struct multiboot2_tag_framebuffer *fb_tag = (void *) ((char *) Mi + off);
      DEBUG ((DEBUG_INFO, "frame buffer @%llx: pitch %#x, width %d, height %d, bpp %d\n\ttype %d",
              fb_tag->common.framebuffer_addr,
              fb_tag->common.framebuffer_pitch,
              fb_tag->common.framebuffer_width,
              fb_tag->common.framebuffer_height,
              fb_tag->common.framebuffer_bpp,
              fb_tag->common.framebuffer_type));
      if (fb_tag->common.framebuffer_type == MULTIBOOT2_FRAMEBUFFER_TYPE_INDEXED)
        DEBUG ((DEBUG_INFO, " (indexed, %d colors): ...",
                fb_tag->u.type0.framebuffer_palette_num_colors));
      else if (fb_tag->common.framebuffer_type == MULTIBOOT2_FRAMEBUFFER_TYPE_RGB)
        DEBUG ((DEBUG_INFO, " (RGB): R=(%d,%d) G=(%d,%d) B=(%d,%d)",
                fb_tag->u.type1.framebuffer_red_field_position,   fb_tag->u.type1.framebuffer_red_mask_size,
                fb_tag->u.type1.framebuffer_green_field_position, fb_tag->u.type1.framebuffer_green_mask_size,
                fb_tag->u.type1.framebuffer_blue_field_position,  fb_tag->u.type1.framebuffer_blue_mask_size));
      else if (fb_tag->common.framebuffer_type == MULTIBOOT2_FRAMEBUFFER_TYPE_EGA_TEXT)
        DEBUG ((DEBUG_INFO, ": EGA TEXT"));
      else
        DEBUG ((DEBUG_INFO, " ???"));
    }
    break;

//  case MULTIBOOT2_TAG_TYPE_BOOTDEV:           // 5
//  case MULTIBOOT2_TAG_TYPE_VBE:               // 7
//  case MULTIBOOT2_TAG_TYPE_ELF_SECTIONS:      // 9
//  case MULTIBOOT2_TAG_TYPE_APM:               // 10
//  case MULTIBOOT2_TAG_TYPE_EFI32:             // 11
//  case MULTIBOOT2_TAG_TYPE_EFI64:             // 12
//  case MULTIBOOT2_TAG_TYPE_SMBIOS:            // 13
//  case MULTIBOOT2_TAG_TYPE_ACPI_OLD:          // 14
//  case MULTIBOOT2_TAG_TYPE_ACPI_NEW:          // 15
//  case MULTIBOOT2_TAG_TYPE_NETWORK:           // 16
//  case MULTIBOOT2_TAG_TYPE_EFI_MMAP:          // 17
//  case MULTIBOOT2_TAG_TYPE_EFI_BS:            // 18
//  case MULTIBOOT2_TAG_TYPE_EFI32_IH:          // 19
//  case MULTIBOOT2_TAG_TYPE_EFI64_IH:          // 20
//  case MULTIBOOT2_TAG_TYPE_LOAD_BASE_ADDR:    // 21

    default:
      DEBUG ((DEBUG_INFO, "??"));
      break;
    }
    DEBUG ((DEBUG_INFO, "\n"));

    off += ALIGN_UP (MAX (tag->size, sizeof(*tag)), MULTIBOOT2_TAG_ALIGN);
    if (tag->type == MULTIBOOT2_TAG_TYPE_END)
      break;
  }
}
