#ifndef DECODE_H
#define DECODE_H
#include "types.h" // Contains user defined types
#include "encode.h"

typedef struct _Decodeinfo
{
    /* output image info*/
    char *out_img_fname;
    FILE *fptr_out_img; 

    /*secret file info*/
    char sec_fname[100];
    FILE *fptr_sec_file;
    long sec_file_length;

    /*Magic string info*/
    long magic_string_size;
    char magic_string[100];

    /*Extension info*/
    long extn_size;
    char extn[10];  

}DecInfo;

/* Decoding function prototype */

/* Read and validate decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecInfo *decInfo);

/*Decode integer data*/
Status decode_integer_data(char *image_buffer,long *size,DecInfo *decInfo);

/*Decoding magic string size*/
Status decode_magic_string_size(DecInfo *decInfo);

/*Decoding magic string*/
Status decode_magic_string(long magic_string_size, DecInfo *decInfo);

/*Decode string data*/
Status decode_string_data(long size,char *data ,DecInfo *decInfo);

/*Decoding secret file extension size*/
Status decode_extn_size(DecInfo *decInfo);

/*Decoding secret file extension*/
Status decode_secret_file_extn( DecInfo *decInfo);

/*Decoding secret message size*/
Status decode_secret_msg_size(DecInfo *decInfo);

/*Decoding secret file data*/
Status decode_secret_file(DecInfo *decInfo);

#endif