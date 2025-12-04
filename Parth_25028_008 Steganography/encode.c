#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;

    fseek(fptr_image, 18, SEEK_SET);

    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    return width * height * 3;
}

/* Get File pointers for i/p and o/p files */
Status open_files(EncodeInfo *encInfo)
{
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);
        return e_failure;
    }

    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);
        return e_failure;
    }

    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);
        return e_failure;
    }

    return e_success;
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if (argv[2] != NULL && strcmp(strstr(argv[2], "."), ".bmp") == 0)
    {
        encInfo->src_image_fname = argv[2];
    }
    else
    {
        return e_failure;
    }

    if (argv[3] != NULL && strcmp(strstr(argv[3], "."), ".txt") == 0)
    {
        encInfo->secret_fname = argv[3];
    }
    else
    {
        return e_failure;
    }

    if (argv[4] != NULL)
        encInfo->stego_image_fname = argv[4];
    else
        encInfo->stego_image_fname = "Stego.bmp";

    return e_success;
}

uint get_file_size(FILE *fptr_secret)
{
    fseek(fptr_secret, 0, SEEK_END);
    uint size = ftell(fptr_secret);
    rewind(fptr_secret);

    printf("Size : %u\n", size);
    return size;
}


Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    long required = (54 + strlen(MAGIC_STRING) + 4 + 4 + encInfo->size_secret_file) * 8;

    if (encInfo->image_capacity > required)
        return e_success;

    return e_failure;
}

Status copy_bmp_header(FILE *fptr_src, FILE *fptr_stego)
{
    char header[54];
    fseek(fptr_src, 0, SEEK_SET);
    fread(header, 1, 54, fptr_src);
    fwrite(header, 1, 54, fptr_stego);
    return e_success;
}

/* Encode 1 byte into LSBs */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i = 0; i < 8; i++)
    {
        unsigned char bit = (data >> (7 - i)) & 1;
        image_buffer[i] = (image_buffer[i] & 0xFE) | bit;
    }
    return e_success;
}

/* Encode generic data */
Status encode_data_to_image(const char *data, int size, FILE *fptr_src, FILE *fptr_stego, EncodeInfo *encInfo)
{
    for (int i = 0; i < size; i++)
    {
        fread(encInfo->image_data, 1, 8, fptr_src);
        encode_byte_to_lsb(data[i], encInfo->image_data);
        fwrite(encInfo->image_data, 1, 8, fptr_stego);
    }
    return e_success;
}

/* Encode magic string */
Status encode_magic_string(const char *magic_str, EncodeInfo *encInfo)
{
    encode_data_to_image(magic_str, strlen(magic_str),
                         encInfo->fptr_src_image,
                         encInfo->fptr_stego_image,
                         encInfo);
    return e_success;
}

/* Encode size into 32 pixels */
Status encode_size_to_lsb(char *image_buffer, int size)
{
    for (int i = 0; i < 32; i++)
    {
        unsigned int bit = (size >> (31 - i)) & 1;
        image_buffer[i] = (image_buffer[i] & 0xFE) | bit;
    }
    return e_success;
}

Status encode_size(uint size, FILE *fptr_src, FILE *fptr_stego)
{
    char rgb[32];
    fread(rgb, 1, 32, fptr_src);
    encode_size_to_lsb(rgb, size);
    fwrite(rgb, 1, 32, fptr_stego);
    return e_success;
}

Status encode_secret_file_size(long size, EncodeInfo *encInfo)
{
    char rgb[32];
    fread(rgb, 1, 32, encInfo->fptr_src_image);
    encode_size_to_lsb(rgb, size);
    fwrite(rgb, 1, 32, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_extn(const char *ext, EncodeInfo *encInfo)
{
    encode_data_to_image(ext, strlen(ext),
                         encInfo->fptr_src_image,
                         encInfo->fptr_stego_image,
                         encInfo);
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char ch;

    for (int i = 0; i < encInfo->size_secret_file; i++)
    {
        fread(&ch, 1, 1, encInfo->fptr_secret);
        fread(encInfo->image_data, 1, 8, encInfo->fptr_src_image);

        encode_byte_to_lsb(ch, encInfo->image_data);

        fwrite(encInfo->image_data, 1, 8, encInfo->fptr_stego_image);
    }
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    while (fread(&ch, 1, 1, fptr_src) == 1)
    {
        fwrite(&ch, 1, 1, fptr_dest);
    }
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    if (open_files(encInfo) == e_success)
    {
        printf("Opened All file successfully\n");

        if (check_capacity(encInfo) == e_success)
        {
            printf("Image have enough space to encode\n");

            if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
            {
                printf("successfully copied Header File\n");

                if (encode_magic_string(MAGIC_STRING, encInfo) == e_success)
                {
                    printf("Successfully encoded the magic string\n");

                    if (encode_size(strlen(".txt"), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
                    {
                        printf("Encoded secret file extension size successfully\n");

                        if (encode_secret_file_extn(".txt", encInfo) == e_success)
                        {
                            printf("Successfully encoded the secret file extension\n");

                            if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
                            {
                                printf("Successfully encoded the secret file size\n");

                                if (encode_secret_file_data(encInfo) == e_success)
                                {
                                    printf("Successfully encoded the secret Data\n");

                                    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
                                    {
                                        printf("successfully copied the remaining RGB Data\n");
                                    }
                                    else
                                    {
                                        printf("Failed to copy remaining RGB Data\n");
                                        return e_failure;
                                    }
                                }
                                else
                                {
                                    printf("Failed to encode the secret Data\n");
                                    return e_failure;
                                }
                            }
                            else
                            {
                                printf("Failed to encode the secret file size\n");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("Failed to encode the secret file extension\n");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("Failed to encode secret file extension size\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("Failed to encode magic string\n");
                    return e_failure;
                }
            }
            else
            {
                printf("Failed to copy Header File\n");
                return e_failure;
            }
        }
        else
        {
            printf("Failure!! image does not have enough space\n");
            return e_failure;
        }
    }
    else
    {
        printf("Fail to open files\n");
        return e_failure;
    }

    return e_success;
}
