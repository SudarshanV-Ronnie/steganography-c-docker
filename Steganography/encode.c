#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* 
   Function: get_image_size_for_bmp
   Purpose : Calculate image capacity
*/

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

    // Return image capacity
    return width * height * 3;
}

/*
   Function: open_files
   Purpose : Open all required files
*/

Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");

    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        printf("Unable to open source image\n");
        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");

    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        printf("Unable to open secret file\n");
        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");

    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        printf("Unable to open stego image\n");
        return e_failure;
    }

    // No failure return e_success
    printf("All files opened successfully\n");
    return e_success;
}

/* 
   Function: read_and_validate_encode_args
   Purpose : Check input arguments and store them in structure
*/

Status read_and_validate_encode_args(int argc,char *argv[], EncodeInfo *encInfo)
{
    if(argc<2)
    {
        printf("Please pass valid arguments\n");
        return 1;
    }

    int len1 = strlen(argv[2]);
    if (argv[2][len1 - 4] == '.' && argv[2][len1 - 3] == 'b' && argv[2][len1 - 2] == 'm' && argv[2][len1 - 1] == 'p')
    {

        encInfo->src_image_fname= argv[2];
    }
    else
    {
        return e_failure;
    }

    char *dot = strrchr(argv[3], '.');  //if we use strchr then it will look for first dot in the file 
    if (dot != NULL &&  (strcmp(dot, ".txt") == 0 || strcmp(dot, ".csv") == 0 || strcmp(dot, ".py") == 0))
    {
        encInfo->secret_fname = argv[3];
        strcpy(encInfo->extn_secret_file, dot);
    }
    else
    {
        return e_failure;
    }
    if (argc > 4)
    {
        int len3 = strlen(argv[4]);
        if (argv[4][len3 - 4] == '.' && argv[4][len3 - 3] == 'b' && argv[4][len3 - 2] == 'm' && argv[4][len3 - 1] == 'p')
        {

            encInfo->stego_image_fname = argv[4];
        }
        else
        {
            return e_failure;
        }
    }
    else
    {
    encInfo->stego_image_fname = "stego_img.bmp";
    }
    return e_success;
}

/* Check if image has enough space */

Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);


    if (encInfo->image_capacity > 54 + strlen(MAGIC_STRING) * 8 + 32 + strlen(encInfo->extn_secret_file) * 8 + 32 + encInfo->size_secret_file * 8)
    {
        printf("Image has enough capacity\n");
        return e_success;
    }
    else
    {
        printf("Image does not have enough capacity\n");
        return e_failure;
    }
}

/* Get size of secret file */

uint get_file_size(FILE *fptr_secret)
{
    fseek(fptr_secret, 0, SEEK_END);
    long size = ftell(fptr_secret);
    rewind(fptr_secret);
    printf("Secret file size : %lu bytes\n", size);
    return size;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char image_buffer[54];
    rewind(fptr_src_image);

    if (fread(image_buffer, 54, 1, fptr_src_image) != 1)
    {
        printf("Failed to read BMP header\n");
        return e_failure;
    }
    if (fwrite(image_buffer, 54, 1, fptr_dest_image) != 1)
    {
        printf("Failed to write BMP header\n");
        return e_failure;
    }

    printf("BMP header copied successfully\n");
    return e_success;
}

/* Encode one character into 8 bytes */

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i = 0; i < 8; i++)
    {
        char bit=(data>>(7-i)) & 1;                      // Get bit
        image_buffer[i]=image_buffer[i]&0xFE;           // Clear LSB
        image_buffer[i]=image_buffer[i] | bit;          // Set LSB
    }
    return e_success;
}

/* Encode integer into 32 bytes */
Status encode_size_to_lsb(int data, char *image_buffer)
{
    for (int i = 0; i < 32; i++)
    {
        int bit = (data>>(31-i))&1;
        image_buffer[i] = image_buffer[i] & 0xFE;
        image_buffer[i]= image_buffer[i] | bit;  
    }
    return e_success;
}

/* Encode any data into image */

Status encode_magic_string(const  char *magic_str, EncodeInfo *encInfo)
{
    if(encode_data_to_image(magic_str, strlen(magic_str), encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_success)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

/* Encode magic string */

Status encode_data_to_image(const char *data, int size, FILE *src, FILE *dest)
{
     char image_buffer[8];
    for (int i = 0; i < size; i++)
    {
        if(fread(image_buffer,1,8,src)!=8)
        {
            return e_failure;
           
        }
        if( encode_byte_to_lsb(data[i],image_buffer)!=e_success)
        {
            return e_failure;
        }
        if(fwrite(image_buffer,1,8,dest)!=8)
        {
            return e_failure;
        }
    }
     return e_success;
}

/* Encode secret file extension size */

Status encode_secret_file_extn_size(long extn_size, EncodeInfo *encInfo)
{
    char image_buffer[32];
    
    if(fread(image_buffer , 1,32,encInfo->fptr_src_image)!=32)
    {
        return e_failure;
    }
    if(encode_size_to_lsb(extn_size,image_buffer)==e_failure)
    {
        return e_failure;
    }
    if(fwrite(image_buffer , 1,32,encInfo->fptr_stego_image)!=32)
    {
        return e_failure;
    }
    return e_success;
}

/* Encode secret file extension */

Status encode_secret_file_extn(const char *extn, EncodeInfo *encInfo) // call generic function
{
    if(encode_data_to_image(extn,strlen(extn),encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
    {
        return e_failure;
    }
    else
    {
        return e_success;
    }
}

/* Encode secret file size */

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char image_buffer[32];
    
    if(fread(image_buffer,1,32,encInfo->fptr_src_image)!=32)
    {
        return e_failure;
    }
    if(encode_size_to_lsb(file_size,image_buffer)==e_failure)
    {
        return e_failure;
    }
    if(fwrite(image_buffer,1,32,encInfo->fptr_stego_image)!=32)
    {
        return e_failure;
    }
    return e_success;
}

/* Encode secret file content */

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char data[encInfo->size_secret_file];
    rewind(encInfo->fptr_secret);
    // read the contents from the file
    if (fread(data, 1, encInfo->size_secret_file, encInfo->fptr_secret) != encInfo->size_secret_file)
    {
       printf("secret file contents are not read correctly\n");
        return e_failure;
    }
    if(encode_data_to_image(data, encInfo->size_secret_file, encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
    {
    return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status copy_remaining_img_data(FILE *src, FILE *dest)
{
    // char ch;
    // while(//read byte by byte upto eof)
    //     {
    //     // write byte  by byte to the dest_img
    //     }

    int ch;
    while ((ch = fgetc(src)) != EOF)  // read one byte
    {
        if (fputc(ch, dest) == EOF)   // write one byte
        {
            return e_failure;         // write error
        }
    }
    return e_success;
}


Status do_encoding(EncodeInfo *encInfo)
{
    if (open_files(encInfo) == e_failure)
    {
        printf("error\n");
        return e_failure;
    }
    else
    {
        printf("Files are opened successfully\n");
    }

    if (check_capacity(encInfo) == e_failure)
    {
        printf("checking file capacity is failed\n");
        return e_failure;
    }
    else
    {
        printf("Checking file capacity successfully completed\n");
    }

    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
         printf("Header is not copied correctly\n");
         return e_failure;
    }
    else
    {
        printf("Copy of bmp header is successful\n");
    }
    if(encode_magic_string(MAGIC_STRING,encInfo)==e_failure)
    {
        printf("Magic string is not encoded correctly\n");
        return e_failure;
    }
    else 
    {
        printf("Magic string encoded successfully\n");
    }

    if (encode_secret_file_extn_size(strlen(encInfo -> extn_secret_file), encInfo) == e_failure)
    { 
        printf("secret file extension size is not encoded correctly\n");
        return e_failure;
    }
    else
    {
       printf("Secret file extension size is Encoded successfully\n");
    }
    if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_failure)
    {
        printf("Secret file extention is not encoded correctly\n");
        return e_failure;
    }
    else
    {
       printf("Secret file extenstion Encoded successfully\n");
    }

    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_failure)
    {
        printf("Secret file size is not encoded correctly\n");
        return e_failure;
    }
    else
    {
        printf("Secret file size Encoded successfully\n");
    }
    if (encode_secret_file_data(encInfo) == e_failure)
    {
         printf("Secret file data is not encoded correctly\n");
        return e_failure;
    }
    else
    {
        printf("Secret file data Encoded successfully\n");
    }
    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        printf( "The remaining data is not copied successfully\n");
        return e_failure;
    }
    else
    {
        printf("Remaining data successfully copied\n");
    }
    return e_success;
    
 }