#include <stdio.h>
# include <string.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

int main(int agrc, char *argv[])
{
    // check the opration type

     if(check_operation_type(argv)== e_encode)
     {
        EncodeInfo encInfo;
        printf("Selected Encoding\n");

        if(read_and_validate_encode_args(argv , &encInfo)== e_success)
        {
         printf("Read and validate input agrument sucessfully\n");
         if(do_encoding(&encInfo) == e_success)
         {
            printf("Complate Encoding\n");
         }
         else
         {
            printf("Failed to do the Encoding\n");
         }
        }
        else
         {
            printf("fail to validate input agrument \n");
         }
     }

else if (check_operation_type(argv) == e_decode)
{
    DecodeInfo dncInfo;
    printf("Selected Decoding\n");

    // Step 1: Validate decode arguments
    if (read_and_validate_decode_args(argv, &dncInfo) == e_success)
    {
        printf("Read and validated decode arguments successfully\n");

        // Step 2: Open required files
        if (open_decode_files(&dncInfo) == e_success)
        {
            // Step 3: Perform decoding
            if (do_decoding(&dncInfo) == e_success)
            {
                printf("Complete Decoding\n");
            }
            else
            {
                printf("Failed to do Decoding\n");
            }
        }
        else
        {
            printf("Failed to open files for decoding\n");
        }
    }
    else
    {
        printf("Failed to validate decode arguments\n");
    }
}


     else
     {
        printf("Invalid Option\n");
        printf("************** Usage*************\n");
        printf("Encoding : ./a.out -e beautiful.bmp secret.txt stego.bmp\n");
        printf("Decoding : ./a.out -d stego.bmp decode.txt\n");
     }

     return 0;

   }

     OperationType check_operation_type(char *argv[])
       {
            if(strcmp(argv[1], "-e")== 0)
                return e_encode;
            
            else if(strcmp(argv[1], "-d")==0)
               return e_decode;

            else
                return e_unsupported;
       }
