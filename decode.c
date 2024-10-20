/*
Name        : V Priyanka
Date        : 28/08/2024
Description : LSB image Steganography
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"
#include "decode.h"

char *in_ext;
/*Validating decode arguments of argv*/
Status read_and_validate_decode_args(char *argv[], DecInfo *decInfo)
{
    in_ext = strstr(argv[2], ".");
    {
        if (in_ext != NULL)
        {
            if (strcmp(in_ext, ".bmp") ==0)
            {
                decInfo->out_img_fname = argv[2];
            }
            else
            {
                
                return e_failure;
            }
        }
        else
        {
            return e_failure;
        }
    }

    if (argv[3] != NULL)
    {
        strcpy(decInfo->sec_fname, argv[3]);
    }
    else
    {
        strcpy(decInfo->sec_fname, "secret_output");
    }
    return e_success;
}

/* Doing decoding */
Status do_decoding(DecInfo *decInfo)
{
    printf("INFO: ## Decoding Procedure Started ##\n");

    printf("INFO: Opening required files\n");
    decInfo->fptr_out_img = fopen(decInfo->out_img_fname, "r");
    if (decInfo->fptr_out_img == NULL)
    {
        printf("Failed to open output file\n");
        return e_failure;
    }
    else
    {
        printf("INFO: Opened %s\n", decInfo->out_img_fname);
    }
    printf("INFO: Done\n");

    printf("INFO: Decoding magic string length\n");
    if (decode_magic_string_size(decInfo) == e_success)
    {
        if (decInfo->magic_string_size > 10)
        {
            printf("ERROR: Please provide valid output file\n");
            return e_failure;
        }
        printf("INFO: Size of magic string - %ld\n", decInfo->magic_string_size);
        printf("INFO: Done\n");
    }
    else
    {
        printf("ERROR: Failed to decode magic string length\n");
        return e_failure;
    }

    printf("INFO: Decoding magic string \n");
    if (decode_magic_string(decInfo->magic_string_size, decInfo) == e_success)
    {
        printf("INFO: Done\n");
    }
    else
    {
        printf("ERROR: Failed to decode magic string\n");
        return e_failure;
    }

    char str[100];
    printf("REQ: Enter the magic string: ");
    scanf("%[^\n]", str); // reading the magic string from user
    getchar();

    // comparing whether the user entered magic string and decoded magic string are same or not
    if (strcmp(decInfo->magic_string, str) == 0)
    {
        printf("INFO: User entered magic string and decoded magic string same\n");
    }
    else
    {
        printf("Invalid magic string!\n");
        return e_failure;
    }

    printf("INFO: Decoding Output File Extension size \n");
    if (decode_extn_size(decInfo) == e_success)
    {
        printf("INFO: Done\n");
    }
    else
    {
        printf("Error in decoding extension size\n");
        return e_failure;
    }

    printf("INFO: Decoding Output File Extension\n");
    if (decode_secret_file_extn(decInfo) == e_success)
    {
        printf("INFO: Done\n");
    }
    else
    {
        printf("Error in decoding secret file extension \n");
        return e_failure;
    }

    // checking whether the user given output file or not
    if (strcmp(decInfo->sec_fname, "secret_output") == 0)
    {
        printf("Output File not mentioned. Creating %s as default\n", decInfo->sec_fname);
    }

    // merging the output file name and extension
    if (strcat(decInfo->sec_fname, decInfo->extn) != NULL)
    {
        // printf("File and extension merged successfully\n");
    }
    else
    {
        printf("Error in merging file name and extenion\n");
        return e_failure;
    }

    // opening the ouput file in write mode
    decInfo->fptr_sec_file = fopen(decInfo->sec_fname, "w");
    if (decInfo->fptr_sec_file != NULL)
    {
        printf("INFO: Opened %s\n", decInfo->sec_fname);
        printf("INFO: Done. Opened all required files\n");
        printf("INFO: Done\n");
    }
    else
    {
        printf("Error in opening secret file\n");
        return e_failure;
    }

    printf("INFO: Decoding %s File Size\n", decInfo->sec_fname);
    if (decode_secret_msg_size(decInfo) == e_success)
    {
        printf("INFO: Done\n");
    }
    else
    {
        printf("Error in decoding secret file length\n");
        return e_failure;
    }

    printf("INFO: Decoding %s File Data\n", decInfo->sec_fname);
    if (decode_secret_file(decInfo) == e_success)
    {
        printf("INFO: Done\n");
    }
    else
    {
        printf("Error in decoding secret file\n");
        return e_failure;
    }
    return e_success;
}

/*Decoding magic string size*/
Status decode_magic_string_size(DecInfo *decInfo)
{
    fseek(decInfo->fptr_out_img, 54, SEEK_SET);
    char temp_buff[32];
    fread(temp_buff, 1, 32, decInfo->fptr_out_img);

    if (decode_integer_data(temp_buff, &decInfo->magic_string_size, decInfo) == e_success)
    {

        return e_success;
    }
    else
    {
        return e_failure;
    }
}

/*Decoding integer data*/
Status decode_integer_data(char *image_buffer, long *size, DecInfo *decInfo)
{
    long num = 0;
    for (int i = 0; i < 32; i++)
    {
        int get = image_buffer[i] & 0x01;
        num = num | (get << i);
    }
    *size = num;
    return e_success;
}

/*Decoding magic string*/
Status decode_magic_string(long magic_string_size, DecInfo *decInfo)
{
    if (decode_string_data(decInfo->magic_string_size, decInfo->magic_string, decInfo) == e_success)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

/*Decoding string data*/
Status decode_string_data(long size, char *data, DecInfo *decInfo)
{
    char temp_buff[8];
    
    for (int i = 0; i < size; i++)
    {
        char c = 0;
        fread(temp_buff, 1, 8, decInfo->fptr_out_img);
        for (int j = 0; j < 8; j++)
        {
            int get = temp_buff[j] & 0x01;
            c = c | (get << j);
        }
        data[i] = c;
    }
    data[size] = '\0';
    return e_success;
}

/*Decoding extension size*/
Status decode_extn_size(DecInfo *decInfo)
{
    char temp_buff[32];
    fread(temp_buff, 1, 32, decInfo->fptr_out_img);

    if (decode_integer_data(temp_buff, &decInfo->extn_size, decInfo) == e_success)
    {

        return e_success;
    }
    else
    {
        return e_failure;
    }
}

/*Decoding secret file extension*/
Status decode_secret_file_extn(DecInfo *decInfo)
{
    if (decode_string_data(decInfo->extn_size, decInfo->extn, decInfo) == e_success)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

/*Decoding secret message size*/
Status decode_secret_msg_size(DecInfo *decInfo)
{
    char temp_buff[32];
    fread(temp_buff, 1, 32, decInfo->fptr_out_img);

    if (decode_integer_data(temp_buff, &decInfo->sec_file_length, decInfo) == e_success)
    {

        return e_success;
    }
    else
    {
        return e_failure;
    }
}

/*Decoding secret file data*/
Status decode_secret_file(DecInfo *decInfo)
{
    char temp_buff[8];
    char data[decInfo->sec_file_length];
    for (int i = 0; i < decInfo->sec_file_length; i++)
    {
        char c = 0;
        fread(temp_buff, 1, 8, decInfo->fptr_out_img);
        for (int j = 0; j < 8; j++)
        {
            int get = temp_buff[j] & 0x01;
            c = c | (get << j);
        }
        data[i] = c;
    }
    data[decInfo->sec_file_length] = '\0';
    fprintf(decInfo->fptr_sec_file, "%s", data);

    return e_success;
}
