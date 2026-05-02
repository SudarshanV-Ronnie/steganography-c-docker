/* Name: Sudarshan V
Start Date: 15-02-26
End Date: 24-02-26
Description: Steganography
Output:
For Encoding:- 
Encoding process started...
Input validation successful.
All files opened successfully
Files are opened successfully
width = 1024
height = 768
Secret file size : 24 bytes
Image has enough capacity
Checking file capacity successfully completed
BMP header copied successfully
Copy of bmp header is successful
Magic string encoded successfully
Secret file extension size is Encoded successfully
Secret file extenstion Encoded successfully
Secret file size Encoded successfully
Secret file data Encoded successfully
Remaining data successfully copied
Encoding completed successfully.

For Decoding:- 
Decoding process started...
Input validation successful.
File is opened successfully
Decoded magic string: #*
Magic string decoded successfully
Decode file extension size is completed
Decoded extn is : .txt
Decode file extension is completed
Output file: output.txt
Decode size of the file is completed
file  size is 24
Decode file data is completed
Decoding completed successfully.
*/
#include<stdio.h>
#include<string.h>
#include"encode.h"
#include"decode.h"
#include"types.h"
#include"common.h"

/* 
   Function: check_operation_type
   Purpose : Check whether user selected encoding or decoding
*/

OperationType check_operation_type(char*argv)
{

    if(strcmp(argv,"-e")==0)
    {
        return e_encode;            // User selected encoding
    }
    else if(strcmp(argv,"-d")==0)
    {
        return e_decode;            // User selected decoding
    }
    else
    {
        return e_unsupported;       // Invalid option
    }
}
int main(int argc ,  char*argv[])
{ 
    EncodeInfo encoInfo;
    DecodeInfo decInfo;
    if(argc<2)
    {
        printf("Please provide proper arguments.\n");
        printf("For Encoding minimum 4 arguments.\n");
        printf("For Decoding minimum 3 arguments.\n"); 
        return 1;
    }
    
    /* Identify operation type */

   OperationType op = check_operation_type(argv[1]);

    /* -------------------- ENCODING -------------------- */

   if(op==e_encode)
   {
        if(argc < 4)   
        {
            printf("Not enough arguments for encoding.\n");
            printf("Usage : %s -e <source.bmp> <secret.txt> <output.bmp>\n", argv[0]);
            return 1;
        }

        printf("Encoding process started...\n");
        
        /* Validate encoding inputs */
        if(read_and_validate_encode_args(argc,argv,&encoInfo)==e_success)
        {
            printf("Input validation successful.\n");
            if(do_encoding (&encoInfo)==e_success)
            {
                printf("Encoding completed successfully.\n");                
            }
            else
            {
                printf("Encoding failed.\n");
                return 1;
            }
        }
    }
       
    /* -------------------- DECODING -------------------- */

    else if(op==e_decode)
    {
        if(argc < 3)   
        {
            printf("Not enough arguments for decoding.\n");
            printf("Usage : %s -d <stego.bmp> [output_file]\n", argv[0]);
            return 1;
        }

        printf("Decoding process started...\n");

        /* Validate decoding inputs */
        if(read_and_validate_decode_args(argc,argv,&decInfo)==d_success)
        {
            printf("Input validation successful.\n");
            if(do_decoding (&decInfo)==d_success)
            {
                printf("Decoding completed successfully.\n");    
            }
            else
            {
                printf("Decoding failed.\n");
                return 1;
            }
        }
    }

   /* -------------------- INVALID OPTION -------------------- */

    else
    {
        printf("Unsupported option.\n");
        printf("Use -e for encoding or -d for decoding.\n");
        return 1;
    }

    return 0;

}