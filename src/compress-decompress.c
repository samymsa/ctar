#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "zlib.h"

z_stream compress_file(char *input, char *output, size_t output_size)
{
    // zlib struct
    z_stream defstream;
    defstream.zalloc = Z_NULL;
    defstream.zfree = Z_NULL;
    defstream.opaque = Z_NULL;

    // setup input and compressed output
    defstream.avail_in = (uInt)strlen(input) + 1; // size of input, string + terminator
    defstream.next_in = (Bytef *)input;           // input char array
    defstream.avail_out = output_size;            // size of output
    defstream.next_out = (Bytef *)output;         // output char array

    // the actual compression work
    deflateInit(&defstream, Z_BEST_COMPRESSION);
    deflate(&defstream, Z_FINISH);
    deflateEnd(&defstream);

    return defstream;
}

z_stream decompress_file(char *input, char *output, size_t output_size, z_stream defstream)
{
    // zlib struct
    z_stream infstream;
    infstream.zalloc = Z_NULL;
    infstream.zfree = Z_NULL;
    infstream.opaque = Z_NULL;

    // setup input and decompressed output
    infstream.avail_in = (uInt)((char *)defstream.next_out - input); // size of input
    infstream.next_in = (Bytef *)input;                              // input char array
    infstream.avail_out = output_size;                               // size of output
    infstream.next_out = (Bytef *)output;                            // output char array

    // the actual decompression work
    inflateInit(&infstream);
    inflate(&infstream, Z_NO_FLUSH);
    inflateEnd(&infstream);

    return infstream;
}

int main(int argc, char *argv[])
{
    // open the file in binary mode
    FILE *input_file = fopen("lorem.txt", "rb");

    // read the entire file into a buffer
    char input_buffer[BUFSIZ];
    size_t input_size = fread(input_buffer, 1, sizeof(input_buffer), input_file);

    // print the size of the file
    printf("Taille du fichier d'origine : %lu octets\n", input_size);

    // compression process
    char output_buffer[BUFSIZ];
    z_stream defstream = compress_file(input_buffer, output_buffer, sizeof(output_buffer));

    // write compressed data to file
    FILE *output_file = fopen("compressed-lorem", "wb");
    fwrite(output_buffer, 1, defstream.total_out, output_file);

    // print the size of the compressed file
    printf("Taille du fichier compressé : %lu octets\n", defstream.total_out);

    fclose(input_file);
    fclose(output_file);

    // read the entire file compressed into a buffer
    input_file = fopen("compressed-lorem", "rb");
    input_size = fread(input_buffer, 1, sizeof(input_buffer), input_file);

    // decompression process
    char decompressed_buffer[BUFSIZ];
    z_stream infstream = decompress_file(input_buffer, decompressed_buffer, sizeof(decompressed_buffer), defstream);

    // write decompressed data to file
    FILE *decompressed_file = fopen("decompressed-lorem.txt", "wb");
    fwrite(decompressed_buffer, 1, strlen(decompressed_buffer), decompressed_file);

    // print the size of the decompressed file
    printf("Taille du fichier décompressé : %lu octets\n", strlen(decompressed_buffer));

    fclose(input_file);
    fclose(decompressed_file);

    return 0;
}