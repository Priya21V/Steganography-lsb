/*
Name        : V Priyanka
Date        : 28/08/2024
Description : LSB image Steganography
*/
#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "decode.h"
#define _GNU_SOURCE

int main(int argc, char *argv[])
{
    EncodeInfo encInfo;
    DecInfo decInfo;
    // checking operation-encoding or decoding
    if (argc < 3)
    {
        if (argc == 1)
        {
            printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp_file> <.text_file> [output file]\n");
            printf("./lsb_steg: Decoding: ./lsb_steg -d <.bmp_file> [output file]\n");
        }
        else if (strcmp(argv[1], "-e") == 0)
        {
            printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp_file> <.text_file> [output file]\n");
        }
        else if (strcmp(argv[1], "-d") == 0)
        {
            printf("./lsb_steg: Decoding: ./lsb_steg -d <.bmp_file> [output file]\n");
        }
        else
        {
            printf("Error-------->Unsupported format.\n");
        }
    }
    else if (argc == 4 || (argc == 5  && strcmp(argv[1],"-e")==0)|| argc == 3)
    {

        //function call of check_operation_type()
        int res = check_operation_type(argv);

        //checking the res is e_encode or not ,if yes doing encoding
        if (res == e_encode)
        {
            //function call of read_and_validate_encode_args()
            if (read_and_validate_encode_args(argv, &encInfo) == e_success)
            {
                //function call of do_encoding()
                if (do_encoding(&encInfo) == e_success)
                {
                    printf("INFO: ## Encoding Done Successfully ##\n");

                    //closing the opened files
                    fclose(encInfo.fptr_secret);
                    fclose(encInfo.fptr_src_image);
                    fclose(encInfo.fptr_stego_image);
                    //fcloseall();
                }
                else
                    printf("INFO: ## Encoding not Done Successfully ##\n");
            }
            else
            {
                printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp_file> <.text_file> [output file]\n");
                return 0;
            }
        }
        else if (res == e_decode)   //checking the res is e_decode or not ,if yes doing decoding
        {
            //function call of read_and_validate_decode_args()
            if (read_and_validate_decode_args(argv, &decInfo) == e_success)
            {
                //function call of do_decoding()
                if (do_decoding(&decInfo) == e_success)
                {
                    printf("INFO: ## Decoding Done Successfully ##\n");

                    //closing opened files
                    fclose(decInfo.fptr_out_img);
                    fclose(decInfo.fptr_sec_file);
                    
                }
                else
                {
                    printf("INFO: ## Decoding not Done Successfully ##\n");
                }  
            }
            else
            {
                printf("./lsb_steg: Decoding: ./lsb_steg -d <.bmp_file> [output file]\n");
            }
        }
        else if (res == e_unsupported)   //if the res is e_unsupported printing error
        {
            printf("Error-------->Unsupported format.\n");
        }
    }
    else if(argc>=5)
    {
        if(strcmp(argv[1],"-e")==0)
        {
            printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp_file> <.text_file> [output file]\n");
        }
        else if(strcmp(argv[1],"-d")==0)
        {
            printf("./lsb_steg: Decoding: ./lsb_steg -d <.bmp_file> [output file]\n");   
        }
        else
        {
            printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp_file> <.text_file> [output file]\n");
            printf("./lsb_steg: Decoding: ./lsb_steg -d <.bmp_file> [output file]\n");

        }
        
    }

    return 0;
}
 
 //checking operation type decoding or encoding
OperationType check_operation_type(char *argv[])
{
    if ((strcmp(argv[1], "-e")) == 0)
    {
        return e_encode;  
    }
    else if ((strcmp(argv[1], "-d")) == 0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}
