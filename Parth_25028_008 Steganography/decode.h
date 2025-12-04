#ifndef DECODE_H
#define DECODE_H

#include "types.h"
#include <stdio.h>

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct DecodeInfo 
{
    char *stego_image_fname;
    FILE *fptr_stego_image;

    char output_fname[50];
    FILE *fptr_output;

    char extn_secret_file[4];   
    uint size_secret_file;

    char image_data[8];

} DecodeInfo;

/* Function Prototypes */

Status read_and_validate_decode_args(char *argv[], DecodeInfo *dncInfo);

Status open_decode_files(DecodeInfo *dncInfo);

Status skip_bmp_header(FILE *fptr_stego_image);

Status decode_magic_string(DecodeInfo *dncInfo);

uint decode_size_from_lsb(FILE *fptr_stego_image);

Status decode_secret_file_extn(DecodeInfo *dncInfo, uint extn_size); 

Status decode_secret_file_size(DecodeInfo *dncInfo); 

Status decode_secret_file_data(DecodeInfo *dncInfo);

char decode_byte_from_lsb(char *buffer); 

Status do_decoding(DecodeInfo *dncInfo);

#endif
