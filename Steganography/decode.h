#include<stdio.h>
#include"types.h"
#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * decoding  the encoded image 
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* Source Image info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
    
   
   //secret data storing file info
    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
    long size_secret_file;

    //magic string information 
    char magic_string[50];
    int magic_string_len;

} DecodeInfo;


/* decoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv);

/* Read and validate decode args from argv */
Status read_and_validate_decode_args(int argc,char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_decode_files(DecodeInfo *decInfo);

Status decode_magic_string(DecodeInfo *decInfo);

/* decode secret file extenstion */
Status decode_secret_file_extn(int extn_size, DecodeInfo *decInfo);

/* decode secret file size */
Status decode_secret_file_size(long *secret_file_size, DecodeInfo *decInfo);
Status decode_secret_file_extn_size(int *extn_size, DecodeInfo *decInfo);

/* decode secret file data*/
Status decode_secret_file_data( long secret_file_size,DecodeInfo *decInfo );

/* decode a byte into LSB of image data array */
Status decode_byte_to_lsb(char*data, char *image_buffer);
Status decode_size_to_lsb(int * data, char *image_buffer);


#endif