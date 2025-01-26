#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc,char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo dncInfo;
    if(argc<3)
    {
	printf("Encoding :./a.out -e <.bmp file> <.txt file>[output file]\n Decoding:./a.out -d <.bmp file> <.txt file>[output file]\n");
	return -1;
    }
    if(check_operation_type(argv) == e_encode)
    {
	if(argc <4)
	{
	    printf("Encoding:./a.out -e <.bmp file> <.txt file>[output file]\n");
	    return -1;
	}
	if(read_and_validate_encode_args(argv, &encInfo)== e_success)
	{
	    if(do_encoding(&encInfo) == e_success)
	    {
		printf("Encoding done successfully\n");
	    }
	    else
	    {
		printf("Encoding is failure\n");
	    }
	}
	else
	{
	    printf("Read and validate is not success\n");
	    return 1;
	}
    }
    else if(check_operation_type(argv) == d_decode)
    {
	if (argc <3)
	{
	    printf("Decoding: ./a.out -d <.bmp file> <.txt file>[output file]\n");
	    return -1;
	}
	if (read_and_validate_decode_argv(argv,&dncInfo) == d_success)
	{
	    if(do_decoding(&dncInfo) == d_success)
	    {
		printf("Decoding done successfully\n");
	    }
	    else
	    {
		printf("Decoding failure\n");
	    }
	}
	else
	{
	    printf("Read and validate was failure\n");
	    return 1;
	}
    }
    return 0;

}
OperationType check_operation_type(char *argv[])
{

    if(strcmp(argv[1],"-e") == 0)
    {
	return e_encode;
    }
    else if(strcmp(argv[1],"-d") == 0)
    {
	return d_decode;
    }
    else
    {
	return e_unsupported;
    }
}
