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
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
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

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

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

    // No failure return e_success
    return e_success;
}
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    //step1:check argv[2] is .bmp or not
    //step2:if yes store argv[2] into src_image_fname,go to step3
    //else-> print error return failure.
    //step3:check argv[3].txt or not
    //step4:if yer secret_fname go to step 5
    //else-> return failure
    //step5:check argv[4] passed or not
    //step6:if yes store into stego_image_fname
    //else-> store default name into stego_image_fname
    if(strcmp((strstr(argv[2],".")),".bmp") == 0)
    {
    
       encInfo->src_image_fname = argv[2];
    }
    else
    {
        return e_failure;
    }
    if(strcmp((strstr(argv[3],".")),".txt") == 0)
    {
       encInfo->secret_fname = argv[3];
       strcpy(encInfo ->extn_secret_file,".txt");
    }
    else
    {
        return e_failure;
    }
    if(argv[4]!=NULL)
    {
        encInfo->stego_image_fname = argv[4];
    }
    else
    {
        printf("Output file not mentioned.Creating stego_img.bmp as default\n");
        encInfo -> stego_image_fname = "stego_img.bmp";
    }
    return e_success;
}
//function definition for encoding
Status do_encoding(EncodeInfo *encInfo)
{
    printf("Encoding process started\n");
    if(open_files(encInfo) == e_success)
    {
        printf("Open files\n");
        printf("Done\n");
        if(check_capacity(encInfo) == e_success)
        {
            printf("Done\n");
            if(copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
            {
                printf("Done\n");
                if(encode_magic_string(MAGIC_STRING, encInfo) == e_success)
                {
                    printf("Done\n");
                    if(encode_secret_file_size(strlen(encInfo -> extn_secret_file), encInfo) == e_success)
                    {
                        printf("Done\n");
                        if(encode_secret_file_extn(encInfo -> extn_secret_file, encInfo) == e_success)
                        {
                            printf("Done\n");
                            if(encode_secret_file_size(encInfo -> size_secret_file, encInfo) == e_success)
                            {
                                printf("Done\n");
                                if(encode_secret_file_data(encInfo) == e_success)
                                {
                                    printf("Done\n");
                                    if(copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
                                    {
                                        printf("Done\n");
                                    }
                                    else
                                    {
                                        printf("Failed to copy remaining data\n");
                                        return e_failure;
                                    }
                                }
                                else
                                {
                                    printf("Failed to encode secret file data\n");
                                    return e_failure;
                                }
                            }
                               
                            else
                            {
                                printf("Encode secret file size is a failure\n");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("Encode secret file extenstion is a failure\n");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("Encode secret file extenstion size is a failure\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("Magic string was not encoded\n");
                    return e_failure;
                }
            }
            else
            {
                printf("Couldn't copy the bmp header\n");
                return e_failure;
            }
        }
        else
        {
            printf("Check capacity is a failure\n");
            return e_failure;
        }
    }
    else
    {
        printf("open file function is a failure\n");
        return e_failure;
    }
    return e_success;
}   
Status check_capacity(EncodeInfo *encInfo)
{
    //step1:find src_image_file size
    encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);
    //step2:find secret file size
    encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);
    //step3:caluculate the value of how much byte needed for encoding
    if(encInfo -> size_secret_file)
    {
        printf("Done\n");
        printf("Checking for bautiful.bmp capacity to handle secret.txt\n");


    // 16(magic string)+32(size of exten)+32(exten)+32(size_of_secret_file)+(secret_file_size*8
        if(encInfo -> image_capacity >(54+(2+4+4+4+encInfo -> size_secret_file)*8));
        {
            return e_success;
    //step4:compare image capacity > caluculation
    //step5:if true, return success
        }
    }
    else
    {
        return e_failure;
    }
    
    // else-> return failure
}
uint get_file_size(FILE *fptr)
{
    fseek(fptr,0,SEEK_END);
    return ftell(fptr);
}
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    printf("Copying image header\n");
    char str[54];
    //step1:rewind fptr_src_image
    fseek(fptr_src_image,0,SEEK_SET);
    //step2:read 54 bytes from fptr_src_image
    fread(str,54,1,fptr_src_image);
    //step3:write 54 bytes to fptr_stego_image,return success
    fwrite(str,54,1,fptr_dest_image);
    return e_success;
}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    printf("Encoding magic string signature\n");
    encode_data_to_image((char *)magic_string, strlen(magic_string), encInfo -> fptr_src_image, encInfo -> fptr_stego_image);
    return e_success;
}

Status encode_data_to_image(char *data, int size, FILE *fptr_src_image,FILE *fptr_stego_image)
{
    char buffer[8];
    //read 8 bytes from source image
    for(int i=0;i<size;i++)
    {
        fread(buffer,8,1,fptr_src_image);
       // call encode_byte_to_lsb();
        encode_byte_to_lsb(data[i], buffer);
       //step3:write 8 bytes from image_buffer to stego_image.
       fwrite(buffer,8,1,fptr_stego_image);
    }
    return e_success;
    //step4:repeat this process till size time.
}
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    //step1:get a bit from ch
    for(int i=0;i<8;i++)
    {
    //step:clear lsb form the image buffer[i]
    //step3:replace
        image_buffer[i]=(image_buffer[i] & 0xFE) | (((unsigned)data >> (7-i))&1);
        
    //step5:repeat this process in 8 times
    }
    return e_success;
}
Status encode_secret_file_extn_size(long file_size, EncodeInfo *encInfo)
{
    printf("Encoding secret.txt file extention size\n");
    char buffer[32];
    fread(buffer,32,1,encInfo -> fptr_src_image);
    //encode the size to stego image file
    encode_byte_to_lsb(file_size, buffer);
    // same like magic string
    fwrite(buffer,32,1,encInfo -> fptr_stego_image);
    return e_success;
}
Status encode_int_to_lsb(long data,char *image_buffer)
{
    for(int i=0;i<32;i++)
    {
        image_buffer[i]=(image_buffer[i] & 0xFE) | (((unsigned)data >> (31-i))&1);
    }
    return e_success;
}
       
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    printf("Encode secret file extenstion\n");
    //call encode_data_to_image()
    char buffer[8];
    int i;
    for(i=0;i<strlen(file_extn);i++)
    {
        fread(buffer,8,1,encInfo -> fptr_src_image);
        encode_byte_to_lsb(file_extn[i],buffer);
        fwrite(buffer,8,1,encInfo -> fptr_stego_image);
    }
    return e_success;
}
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    printf("Encode secret file size\n");
    char buffer[32];
    fread(buffer,32,1,encInfo -> fptr_src_image);
    encode_int_to_lsb(file_size,buffer);
    fwrite(buffer,32,1,encInfo -> fptr_stego_image);
    return e_success;
}
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    printf("Encode secret file data\n");
    char buffer[8];
    fseek(encInfo ->fptr_secret,0,SEEK_SET);
    for(int i=0;i<(encInfo -> size_secret_file-1);i++)
    {
        fread(buffer,8,1,encInfo ->fptr_src_image);
        fread(&(encInfo -> secret_data[0]),1,1,encInfo -> fptr_secret);
        encode_byte_to_lsb(encInfo -> secret_data[0],buffer);
        fwrite(buffer,8,1,encInfo -> fptr_stego_image);
    }
   return e_success;
}
Status copy_remaining_img_data(FILE *fptr_src_image,FILE *fptr_stego_image)
{
    printf("copying remaininig data\n");
    char ch;
    while(fread(&ch,1,1,fptr_src_image)>0)
    {
        fwrite(&ch,1,1,fptr_stego_image);
    }
    return e_success;
}
