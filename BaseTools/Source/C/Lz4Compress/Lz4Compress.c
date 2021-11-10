/** @file
LZ4 compress and decompress utility

Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CommonLib.h"
#include "Sdk/lz4.h"
#include "Sdk/lz4hc.h"

#define UTILITY_NAME "Lz4Compress"

#define INTEL_COPYRIGHT \
  "Copyright (c) 2017, Intel Corporation. All rights reserved."

void PrintHelp (void)
{
  printf (   "\n" UTILITY_NAME " - " INTEL_COPYRIGHT "\n"
             "\nUsage:  Lz4Compress -e|-d  -o <outputFile>  <inputFile>\n"
             "  -e: encode file\n"
             "  -d: decode file\n"
             "  -o FileName, --output FileName: specify the output filename\n"
             );
}


int
main (
  int    argc,
  char  *argv[]
  )
/*++

Routine Description:

  Compress or Decompress the image using LZ4 algrithsm

Arguments:

  Argc            - standard C main() argument count

  Argv            - standard C main() argument list

Returns:

  0             success
  non-zero      otherwise

--*/
{
  FILE  *fp;
  int    i;
  int    sz;
  int    bufsz;
  int    res;
  int    decompress;
  int    inpsz;
  char   *bufi;
  char   *bufo;
  char   *input;
  char   *output;

  bufi  = NULL;
  bufo  = NULL;
  decompress = 1;

  output = NULL;
  input  = NULL;
  decompress = -1;

  if (argc < 5) {
    PrintHelp ();
    return -1;
  }

  for (i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      if (!strcmp(argv[i], "-d")) {
        decompress = 1;
      } else if (!strcmp(argv[i], "-e")) {
        decompress = 0;
      } else if (!strcmp(argv[i], "-o")) {
        if (i+1 < argc) {
          output =  argv[i+1];
          i++;
        }
      } else {
        printf("Unknown option '%s' !\n", argv[i]);
        return -1;
      }
    } else {
      if (input == NULL) {
        input =  argv[i];
      }
    }
  }

  if (input == NULL) {
    printf("Input file name is required!\n");
    return -1;
  }

  if (output == NULL) {
    printf("Output file name is required!\n");
    return -2;
  }

  if (decompress < 0) {
    printf("Operation is required!\n");
    return -3;
  }

  fp = fopen (input, "rb");
  if (!fp) {
    printf("Cannot open file '%s' !\n", input);
    return -4;
  }

  fseek(fp, 0L, SEEK_END);
  inpsz = ftell(fp);
  fseek(fp, 0L, SEEK_SET);

  bufi = (char *)malloc(inpsz);
  if (bufi) {
    fread(bufi, inpsz, 1, fp);
  }
  fclose(fp);

  if (decompress == 1) {
    sz = *(int *)bufi;
    if ((sz < 0) || (inpsz < sizeof(int))) {
      res = -1;
    } else {
      bufo = (char *)malloc(sz);
      res = LZ4_decompress_safe((const char *)bufi + sizeof(int), (char *)bufo, inpsz - sizeof(int), sz);
    }
  } else {
    bufsz = LZ4_compressBound(inpsz);
    bufo = (char *)malloc(bufsz);
    if (bufo) {
      res = LZ4_compress_HC((const char *)bufi, (char *)bufo, inpsz, bufsz, 0);
    } else {
      res = -1;
    }
  }

  if (res > 0) {
    fp = fopen (output, "wb");
    if (!fp) {
      printf("Cannot create file '%s' !\n", output);
    } else {
      if (!decompress) {
        fwrite(&inpsz, sizeof(int), 1, fp);
      }
      fwrite(bufo, res, 1, fp);
      fclose(fp);
    }
  }

  if (bufi) {
    free(bufi);
  }

  if (bufo) {
    free(bufo);
  }

  if (res <= 0) {
    printf("Failed!\n");
    res = -1;
  } else {
    printf("OK!\n");
    res = 0;
  }

  return res;
}