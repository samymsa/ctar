#include <stdio.h>
#include <string.h>  // for strlen
#include <assert.h>
#include "zlib.h"    // for zlib functions

z_stream compress(char *input, char *output)
{
    // zlib struct
    z_stream defstream;
    defstream.zalloc = Z_NULL;
    defstream.zfree = Z_NULL;
    defstream.opaque = Z_NULL;
    // setup "a" as the input and "b" as the compressed output
    defstream.avail_in = (uInt)strlen(input)+1; // size of input, string + terminator
    defstream.next_in = (Bytef *)input; // input char array
    defstream.avail_out = (uInt)sizeof(output); // size of output
    defstream.next_out = (Bytef *)output; // output char array
    deflateInit(&defstream, Z_BEST_COMPRESSION);
    deflate(&defstream, Z_FINISH);
    deflateEnd(&defstream);
    return defstream;
}

zstream decompress(char *input, char *output)
{
    // zlib struct
    z_stream infstream;
    infstream.zalloc = Z_NULL;
    infstream.zfree = Z_NULL;
    infstream.opaque = Z_NULL;
    // setup "b" as the input and "c" as the compressed output
    infstream.avail_in = (uInt)((char*)defstream.next_out - input); // size of input
    infstream.next_in = (Bytef *)input; // input char array
    infstream.avail_out = (uInt)sizeof(output); // size of output
    infstream.next_out = (Bytef *)output; // output char array
     
    // the actual DE-compression work.
    inflateInit(&infstream);
    inflate(&infstream, Z_NO_FLUSH);
    inflateEnd(&infstream);
    return infstream;
}

// adapted from: http://stackoverflow.com/questions/7540259/deflate-and-inflate-zlib-h-in-c
int zlib_test(int argc, char* argv[])
{
    // original string len = 36
    char a[50] = "Hello Hello Hello Hello Hello Hello!"; 

    // placeholder for the compressed (deflated) version of "a" 
    char b[50];

    // placeholder for the UNcompressed (inflated) version of "b"
    char c[50];
     

    printf("Uncompressed size is: %lu\n", strlen(a));
    printf("Uncompressed string is: %s\n", a);


    printf("\n----------\n\n");

    // STEP 1.
    // compression
    z_stream defstream = compress(a, b);
     
    // This is one way of getting the size of the output
    printf("Compressed size is: %lu\n", strlen(b));
    printf("Compressed string is: %s\n", b);
    

    printf("\n----------\n\n");


    // STEP 2.
    // decompression
    z_stream infstream = decompress(b, c);
     
    printf("Uncompressed size is: %lu\n", strlen(c));
    printf("Uncompressed string is: %s\n", c);
    

    // make sure uncompressed is exactly equal to original.
    assert(strcmp(a,c)==0);

    return 0;
}