/*
Name        : V Priyanka
Date        : 28/08/2024
Description : LSB image Steganography
*/
#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
char *str_extn;
char *output_ext;
char *src_extn;
/*
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);
    rewind(fptr_image);

    // Return image capacity
    return width * height * 3;
}*/

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
/*Openig required files*/
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file

    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling

    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }
    else
    {
        printf("INFO: Opened %s\n", encInfo->src_image_fname);
    }

    // Secret file

    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling

    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }
    else
    {
        printf("INFO: Opened %s\n", encInfo->secret_fname);
    }

    // No failure return e_success
    return e_success;
}
/*validating encode arguments*/
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    src_extn = strstr(argv[2], ".");
    if (src_extn != NULL)
    {
        
        if (strcmp(src_extn, ".bmp") ==0) // checking .bmp extension is present or not
        {
            encInfo->src_image_fname = argv[2];
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

    if (argv[3] != NULL)
    {
        str_extn = strstr(argv[3], "."); // checking "." is present or not
        if (str_extn != NULL)
        {
            encInfo->secret_fname = argv[3];
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

    strcpy(encInfo->extn_secret_file, str_extn); // copying secret file extension

    encInfo->extn_size = strlen(encInfo->extn_secret_file); // storing extension size

    if (argv[4] != NULL)
    {
        output_ext = strstr(argv[4], ".");
        if (output_ext != NULL)
        {
            if (strcmp(output_ext, ".bmp") == 0)
            {
                if (strstr(argv[4], ".bmp") != NULL) // checking output file extension is .bmp or not
                {
                    encInfo->stego_image_fname = argv[4];

                    return e_success;
                }
                else
                {
                    printf("Please provide .bmp file.\n");
                    return e_failure;
                }
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
    else
    {
        encInfo->stego_image_fname = "stego.bmp"; // if output file is not given , creating output file as stego.bmp
    }
    return e_success;
}

/*Getting file size*/
uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);
    uint size = ftell(fptr);
    rewind(fptr);
    return size;
}

/*Doing encoding*/
Status do_encoding(EncodeInfo *encInfo)
{
    printf("INFO: Opening required files\n");
    if (open_files(encInfo) == e_success) // checking files are opened or not
    {
        printf("INFO: Done\n");
    }
    else
    {
        printf("Error in opening files\n");
        return e_failure;
    }
    printf("INFO: ## Encoding Procedure Started ##\n");

    printf("INFO: Checking for %s size\n", encInfo->secret_fname);

    if (get_file_size(encInfo->fptr_secret) > 0) // checking secret file is not empty
    {
        printf("INFO: Done. Not Empty\n");
        encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    }
    else
    {
        printf("ERROR: %s file is empty\n", encInfo->secret_fname);
        return e_failure;
    }

    // reading the magic string from user
    printf("REQ:Enter the magic string of length 10: ");
    scanf("%[^\n]", encInfo->magic_string);

    encInfo->magic_string_size = strlen(encInfo->magic_string);

    if (encInfo->magic_string_size > 10) // checking the magic string length is not more than 10
    {
        printf("Magic string length is more than 10\n");
        return e_failure;
    }

    // checking the capacity of image to handle encoding data
    printf("INFO: Checking for %s capacity to handle %s\n", encInfo->src_image_fname, encInfo->secret_fname);
    if (check_capacity(encInfo) == e_success)
    {
        printf("INFO: Done. Found OK\n");
    }
    else
    {
        printf("ERROR: Image capacity is low to encode\n");
        return e_failure;
    }
    // Stego Image file

    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling

    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }
    else
    {
        printf("INFO: Opened %s\n", encInfo->stego_image_fname);
    }

    // checking the output file is given by user or not
    if (strcmp(encInfo->stego_image_fname, "stego.bmp") == 0)
    {
        printf("INFO: Output File not mentioned. Creating %s as default\n", encInfo->stego_image_fname);
    }

    printf("INFO: Copying Image Header\n");
    if (copy_bmp_header(encInfo) == e_success)
    {
        printf("INFO: Done\n");
    }
    else
    {
        printf("Error in copying the bmp header\n");
        return e_failure;
    }

    printf("INFO: Encoding Magic String Size\n");
    if (encode_magic_string_size(encInfo->magic_string_size, encInfo) == e_success)
    {
        printf("INFO: Done\n");
    }
    else
    {
        printf("Error in encoding magic string size");
        return e_failure;
    }

    printf("INFO: Encoding Magic String Signature\n");
    if (encode_magic_string(encInfo->magic_string, encInfo) == e_success)
    {
        printf("INFO: Done\n");
    }
    else
    {
        printf("Error in encoding magic string\n");
        return e_failure;
    }
    printf("INFO: Encoding %s File Extension Size\n", encInfo->secret_fname);
    if (encode_extn_size(encInfo->extn_size, encInfo) == e_success)
    {
        printf("INFO: Done\n");
    }
    else
    {
        printf("Error in encoding extension size\n");
        return e_failure;
    }
    printf("INFO: Encoding %s File Extension\n", encInfo->secret_fname);
    if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
    {
        printf("INFO: Done\n");
    }
    else
    {
        printf("Error in encoding secret file extension\n");
    }
    printf("INFO: Encoding %s File Size\n", encInfo->secret_fname);
    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
    {
        printf("INFO: Done\n");
    }
    else
    {
        printf("Error in encoding secret file size\n");
    }
    printf("INFO: Encoding %s File Data\n", encInfo->secret_fname);
    if (encode_secret_file_data(encInfo) == e_success)
    {
        printf("INFO: Done\n");
    }
    else
    {
        printf("Error in encoding secret file data\n");
        return e_failure;
    }
    printf("INFO: Copying Left Over Data\n");
    if (copy_remaining_img_data(encInfo) == e_success)
    {
        printf("INFO: Done\n");
    }
    else
    {
        printf("Error in copying remaing data\n");
        return e_failure;
    }
    return e_success;
}

/*checking image capacity can handle encoding data or not*/
Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_file_size(encInfo->fptr_src_image);
    printf("Image capacity size is: %d bytes\n", encInfo->image_capacity);

    encInfo->encoding_data = 54 + (4 + strlen(encInfo->magic_string) + 4 + encInfo->extn_size + 4 + encInfo->size_secret_file) * 8;

    printf("Encoding data size is: %d bytes\n", encInfo->encoding_data);

    if (encInfo->image_capacity > encInfo->encoding_data)
    {

        return e_success;
    }
    else
    {
        return e_failure;
    }
}

/*copying bmp header*/
Status copy_bmp_header(EncodeInfo *encInfo)
{
    char header[54];

    if (fread(header, 1, 54, encInfo->fptr_src_image) == 54)
    {

        if (fwrite(header, 1, 54, encInfo->fptr_stego_image) == 54)
        {
            return e_success;
        }
        else
        {
            printf("Error: Unable to write BMP header to stego image file\n");
            return e_failure;
        }
    }
    else
    {
        printf("Error: Unable to read BMP header from source image file\n");
        return e_failure;
    }
}
/*Encoding magic string size to output file*/
Status encode_magic_string_size(long magic_string_size, EncodeInfo *encInfo)
{
    char temp_buff[32];
    fread(temp_buff, 1, 32, encInfo->fptr_src_image);
    encode_size_to_lsb(magic_string_size, temp_buff);
    fwrite(temp_buff, 1, 32, encInfo->fptr_stego_image);
    return e_success;
}

/*encoding size to lsb bit image*/
Status encode_size_to_lsb(long data, char *image_buffer)
{
    for (int i = 0; i < 32; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((data >> i) & 1);
    }
    return e_success;
}

/*Encoding magic string to image*/
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    if (encode_string_to_image(encInfo->magic_string, encInfo->magic_string_size, encInfo) == e_success)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

/*Encoding string to image*/
Status encode_string_to_image(char *data, int size, EncodeInfo *encInfo)
{
    char temp_buff[8];
    for (int i = 0; i < size; i++)
    {
        if (fread(temp_buff, 8, 1, encInfo->fptr_src_image) != 1)
        {

            return e_failure;
        }

        for (int j = 0; j < 8; j++)
        {
            temp_buff[j] = (temp_buff[j] & 0xFE) | ((data[i] >> j) & 1);
        }

        if (fwrite(temp_buff, 8, 1, encInfo->fptr_stego_image) != 1)
        {

            return e_failure;
        }
    }
    return e_success;
}

/*Encoding secret file extension size*/
Status encode_extn_size(long extn_size, EncodeInfo *encInfo)
{
    char temp_buff[32];
    fread(temp_buff, 1, 32, encInfo->fptr_src_image);
    encode_size_to_lsb(extn_size, temp_buff);
    fwrite(temp_buff, 1, 32, encInfo->fptr_stego_image);
    return e_success;
}

/*Encoding secret file extension*/
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    if (encode_string_to_image(encInfo->extn_secret_file, encInfo->extn_size, encInfo) == e_success)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

/*Encoding seceret file size*/
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char temp_buff[32];
    fread(temp_buff, 1, 32, encInfo->fptr_src_image);
    encode_size_to_lsb(file_size, temp_buff);
    fwrite(temp_buff, 1, 32, encInfo->fptr_stego_image);
    return e_success;
}

/*Encoding secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    int size = encInfo->size_secret_file;
    char sec_msg_data[size];
    char ch;
    int i = 0;
    while ((ch = fgetc(encInfo->fptr_secret)) != EOF) /*getting secret message data*/
    {
        sec_msg_data[i] = ch;
        i++;
    }

    if (encode_string_to_image(sec_msg_data, encInfo->size_secret_file, encInfo) == e_success)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

/*copying remaining data*/
Status copy_remaining_img_data(EncodeInfo *encInfo)
{
    uint size = encInfo->image_capacity - encInfo->encoding_data;
    char buffer[size];

    if (fread(buffer, 1, size, encInfo->fptr_src_image) == size)
    {

        if (fwrite(buffer, 1, size, encInfo->fptr_stego_image) == size)
        {
            return e_success;
        }
        else
        {
            printf("Error: Unable to write remaing data to stego image file\n");
            return e_failure;
        }
    }
    else
    {
        printf("Error: Unable to read remaing data from source image file\n");
        return e_failure;
    }
}