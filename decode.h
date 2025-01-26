#ifndef DECODE_H
#define DECODE_H


#include "types.h"

#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    char *src_image_fname;
    FILE *fptr_src_image;

    char *secret_fname;
    long size_extn;
    char extn_secret_file[MAX_FILE_SUFFIX];
    FILE *fptr_secret;
    long size_secret_file;

}DecodeInfo;

Status read_and_validate_decode_argv(char *argv[], DecodeInfo *decInfo);

Status do_decoding(DecodeInfo *decInfo);

Status open_img_file(DecodeInfo *decInfo);

Status decode_magic_string(FILE *fptr_img);

char decode_byte_from_lsb(char *buffer);

Status decode_secret_file_extn_size(DecodeInfo *decInfo);

long decode_size_from_lsb(char *buffer);

Status decode_secret_file_extn(DecodeInfo *decInfo);

Status decode_file_data_size(DecodeInfo *decInfo);

Status decode_file_data(DecodeInfo *decInfo);

#endif
