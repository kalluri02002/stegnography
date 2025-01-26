#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "decode.h"
#include "types.h"
#include "common.h"


/* function defination */

Status read_and_validate_decode_argv(char *argv[], DecodeInfo *decInfo)
{
    // Checking for .bmp file
    if(strstr(argv[2], ".bmp"))
    {
        decInfo -> src_image_fname = argv[2];
    }
    else
    {
        return d_failure;
    }

    //for secret file name allocating the memory
    decInfo -> secret_fname = malloc(12);

    if(argv[3])
    {
        //if user passes the secret file name through CLA, store that file name
        strcpy(decInfo -> secret_fname, argv[3]);
    }
    else
    {
        //if user not passes the secret file name
        strcpy(decInfo -> secret_fname, "output");
    }
    return d_success;
}
/* Start decoding */

Status do_decoding(DecodeInfo *decInfo)
{
    printf("Decoding Procedure Started\n");

    printf("Opening required files\n");
    if(open_img_file(decInfo) == d_success)
    {
        printf("Opened %s\n", decInfo -> src_image_fname);
        if(decode_magic_string(decInfo -> fptr_src_image) == d_success)
        {
            printf("Done\n");
            if(decode_secret_file_extn_size(decInfo) == d_success)
            {
                printf("Done\n");
                if(decode_secret_file_extn(decInfo) == d_success)
                {
                    printf("Done\n");
                    if(decode_file_data_size(decInfo) == d_success)
                    {
                        printf("Done\n");
                        if(decode_file_data(decInfo) == d_success)
                        {
                            printf("Done\n");
                        }
                        else
                        {
                            printf("Decoding secret file data was a failure\n");
                            return d_failure;
                        }
                    }
                    else
                    {
                        printf("Decoding secret file data size was a failure\n");
                        return d_failure;
                    }
                }
                else
                {
                    printf("Decoding secret file extn was a failure\n");
                    return d_failure;
                }
            }
            else
            {
                printf("Decoding secret file extn size was a failure\n");
                return d_failure;
            }
        }
        else
        {
            printf("Decoding magic string was a failure\n");
        }
    }
    else
    {
        printf("open file was failure\n");
        return d_failure;
    }
    return d_success;
}

Status open_img_file(DecodeInfo *decInfo)
{
    //open the file in read mode and store address in stego image file
    decInfo -> fptr_src_image = fopen(decInfo -> src_image_fname, "r");
    //if it not returns the address print the error message(do error handling)
    if (decInfo -> fptr_src_image == NULL )
    {
        perror("fopen");
        fprintf(stderr, "ERROR : Unable to open file %s\n", decInfo -> src_image_fname);
        return d_failure;
    }
    //if there is no error return d_success
    return d_success;
}

Status decode_magic_string(FILE *fptr_image)
{
    printf("Decoding Magic String Signature\n");
    //declaring one character array with size
    char buffer[8];
    //array to hold the magic string
    char *ch = malloc(strlen(MAGIC_STRING));

    //moving the image pointer to 54 bytes
    fseek(fptr_image, 54, SEEK_SET);

    //running loop 3 times to geting the magic string
    for ( int i = 0 ; i < strlen(MAGIC_STRING) ; i++ )
    {
        fread(buffer, 8, 1, fptr_image);
        ch[i] = decode_byte_from_lsb(buffer);
    }
    //checking magic string
    if (strcmp(ch, MAGIC_STRING) == 0)
    {
	    return d_success;
    }
    else
    {
	    return d_failure;
    }
}

char decode_byte_from_lsb(char *buffer)
{
    char ch = 0;
    //running loop 8 time
    for( int i = 0 ; i < 8 ; i++ )
    {
	//retrieving the lsb bit,moving and storing in ch
	    ch = (buffer[i] & 1) | (ch << 1);
    }
    return ch;
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    printf("Decoding secret File Extenstion Size\n");
    char buffer[32];
    //reading 32 bytes from source image and storing into buffer
    fread(buffer, 32, 1, decInfo -> fptr_src_image);
    //getting size and storing in the structure member
    decInfo -> size_extn = decode_size_from_lsb(buffer);

    return d_success;
}

long decode_size_from_lsb(char *buffer)
{
    long size = 0;

    for(int i = 0 ; i < 32 ; i++)
    {
	    size = (buffer[i] & 1) | (size << 1);
    }
    return size;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char buffer[8];

    for(int i = 0 ; i < decInfo -> size_extn ; i++)
    {
	    fread(buffer, 8, 1, decInfo -> fptr_src_image);

	    decInfo -> extn_secret_file[i] = decode_byte_from_lsb(buffer);
    }
    printf("Output File not mentioned. Creating %s%s as default\n", decInfo -> secret_fname, decInfo -> extn_secret_file);
    decInfo -> fptr_secret = fopen(strcat(decInfo -> secret_fname, (char *)decInfo -> extn_secret_file), "w");
    printf("Opened %s\n", decInfo -> secret_fname);
    if(decInfo -> fptr_secret == NULL )
    {
	    perror("fopen");
	    fprintf(stderr,"ERROR : Unable to open file %s\n",decInfo -> secret_fname);
	    return d_failure;
    }
    return d_success;
}

Status decode_file_data_size(DecodeInfo *decInfo)
{
    printf("Decoding %s File Size\n", decInfo -> secret_fname);
    char buffer[32];

    fread(buffer, 32, 1, decInfo -> fptr_src_image);

    decInfo -> size_secret_file = decode_size_from_lsb(buffer);

    return d_success;
}

Status decode_file_data(DecodeInfo *decInfo)
{
    printf("Decoding %s File Data\n", decInfo -> secret_fname);
    char buffer[8], ch;

    for (int i = 0 ; i < decInfo -> size_secret_file - 1 ; i++)
    {
     	fread(buffer, 8, 1, decInfo -> fptr_src_image);

	    ch = decode_byte_from_lsb(buffer);

    	fwrite(&ch, 1, 1, decInfo -> fptr_secret);
    }
    ch = '\n';
    fwrite(&ch, 1, 1, decInfo -> fptr_secret);
    return d_success;
}

