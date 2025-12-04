#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

/* ---------- 1. read_and_validate_decode_args ---------- */

Status read_and_validate_decode_args(char *argv[], DecodeInfo *dncInfo)
{
    // Validate stego .bmp file
    if (argv[2] != NULL && strcmp(strstr(argv[2], "."), ".bmp") == 0)
    {
        dncInfo->stego_image_fname = argv[2];
    }
    else
    {
        printf("Error: Please provide a valid .bmp stego image\n");
        return e_failure;
    }

    // Validate output file name
    if (argv[3] != NULL)
    {
        strcpy(dncInfo->output_fname, argv[3]);
    }
    else
    {
        printf("Error : Please provide a valid output file\n");
        return e_failure;
    }

    return e_success;
}

/* ---------- 2. open_decode_files ---------- */

Status open_decode_files(DecodeInfo *dncInfo)
{
    // Open stego image 
    dncInfo->fptr_stego_image = fopen(dncInfo->stego_image_fname, "r");
    if (dncInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "Error : Unable to open file %s\n", dncInfo->stego_image_fname);
        return e_failure;
    }

    // Open output file
    dncInfo->fptr_output = fopen(dncInfo->output_fname, "w");
    if (dncInfo->fptr_output == NULL)
    {
        perror("fopen");
        fprintf(stderr, "Error : Unable to open File %s\n", dncInfo->output_fname);
        return e_failure;
    }

    return e_success;
}

/* ---------- 3. skip_bmp_header ---------- */

Status skip_bmp_header(FILE *fptr_stego_image)
{
    // Move pointer to 54 bytes
    fseek(fptr_stego_image, 54, SEEK_SET);
    return e_success;
}

/* ---------- 4. decode_size_from_lsb ---------- */

uint decode_size_from_lsb(FILE *fptr_stego_image)
{
    uint size = 0;
    unsigned char ch;

    for (int i = 0; i < 32; i++)
    {
        fread(&ch, 1, 1, fptr_stego_image);

        uint bit = ch & 1;
        size = (size << 1) | bit;
    }
    return size;
}

/* ---------- 5. decode_magic_string ---------- */

Status decode_magic_string(DecodeInfo *dncInfo)
{
    char decoded_str[3];
    int i;

    for (i = 0; i < strlen(MAGIC_STRING); i++)
    {
        fread(dncInfo->image_data, 8, sizeof(char), dncInfo->fptr_stego_image);
        decoded_str[i] = decode_byte_from_lsb(dncInfo->image_data);
    }

    decoded_str[i] = '\0';

    if (strcmp(decoded_str, MAGIC_STRING) == 0)
        return e_success;
    else
        return e_failure;
}

   // decode serect file extension
 Status decode_secret_file_extn(DecodeInfo *dncInfo, uint extn_size)
 {
      for(int i=0; i < extn_size; i++)
      {
         fread(dncInfo->image_data, 8, sizeof(char), dncInfo->fptr_stego_image);

         dncInfo->extn_secret_file[i] = decode_byte_from_lsb(dncInfo->image_data);

      }  
     dncInfo->extn_secret_file[extn_size] = '\0';
     
     return e_success;
 }

Status decode_secret_file_size(DecodeInfo *dncInfo)
{
    dncInfo->size_secret_file = decode_size_from_lsb(dncInfo->fptr_stego_image);

    if(dncInfo->size_secret_file > 0)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
} 

Status decode_secret_file_data(DecodeInfo *dncInfo)
{
    char ch;

    for (int i = 0; i < dncInfo->size_secret_file; i++)
    {
        // Read 8 bytes from stego image
        fread(dncInfo->image_data, 8, sizeof(char), dncInfo->fptr_stego_image);

        // Decode 1 byte from LSBs
        ch = decode_byte_from_lsb(dncInfo->image_data);

        // Write the decoded byte to output file
        fwrite(&ch, 1, 1, dncInfo->fptr_output);
    }

    return e_success;
}

char decode_byte_from_lsb(char *buffer)
{
    unsigned char decoded = 0;

    for (int i = 0; i < 8; i++)
    {
        decoded = (decoded << 1) | (buffer[i] & 1);
    }

    return decoded;
}



Status do_decoding(DecodeInfo *dncInfo)
{
    // Skip header
    if (skip_bmp_header(dncInfo->fptr_stego_image) == e_success)
    {
        printf("Successfully skipped BMP header\n");
    }   
    else
    {
        return e_failure;
    }

    // Decode magic string
    if (decode_magic_string(dncInfo) == e_success)
     {
        printf("Successfully decoded magic string\n");
     }
     else
     {
        return e_failure;
     }  

    // Decode extension size
    uint extn_size = decode_size_from_lsb(dncInfo->fptr_stego_image);
    printf("Extension size: %u\n", extn_size);

    // Decode extension
    if (decode_secret_file_extn(dncInfo, extn_size) == e_success)
    {
        printf("Secret file extension decoded: %s\n", dncInfo->extn_secret_file);
    }   
    else
    {
        return e_failure;
    }

    // Decode secret file size
    if (decode_secret_file_size(dncInfo) == e_success)
    {
        printf("Secret file size: %u bytes\n", dncInfo->size_secret_file);
    }

    else
    {
        return e_failure;
    }
    // Decode secret file data
    if (decode_secret_file_data(dncInfo) == e_success)
     {
        printf("Secret file data decoded successfully\n");
     }  
    else
     {
        return e_failure;
     }
    return e_success;
}
