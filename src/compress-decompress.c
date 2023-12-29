#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "zlib.h"

z_stream compress_archive(char *input, size_t input_size, char *output, size_t output_size)
{
    // zlib struct
    z_stream defstream;
    defstream.zalloc = Z_NULL;
    defstream.zfree = Z_NULL;
    defstream.opaque = Z_NULL;

    // setup input and compressed output
    defstream.avail_in = input_size;      // size of input
    defstream.next_in = (Bytef *)input;   // input char array
    defstream.avail_out = output_size;    // size of output
    defstream.next_out = (Bytef *)output; // output char array

    // the actual compression work
    deflateInit(&defstream, Z_BEST_COMPRESSION);
    deflate(&defstream, Z_FINISH);
    deflateEnd(&defstream);

    return defstream;
}

z_stream decompress_archive(char *input, char *output, size_t output_size, z_stream defstream)
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
    // open the archive in binary mode
    FILE *input_archive = fopen("archive.tar", "rb");
    if (input_archive == NULL)
    {
        perror("Error during the opening of the archive");
        return 1;
    }

    // get the size of the archive
    fseek(input_archive, 0, SEEK_END);
    size_t input_size = ftell(input_archive);
    fseek(input_archive, 0, SEEK_SET);

    // allocate memory for the archive
    char *input_buffer = (char *)malloc(input_size);
    if (input_buffer == NULL)
    {
        perror("Erreur d'allocation mémoire");
        fclose(input_archive);
        return 1;
    }

    // print the size of the archive
    fread(input_buffer, 1, input_size, input_archive);
    printf("Taille de l'archive d'origine : %lu octets\n", input_size);

    // compression process
    char output_buffer[input_size];
    z_stream defstream = compress_archive(input_buffer, input_size, output_buffer, sizeof(output_buffer));

    // write compressed data to file
    FILE *output_archive = fopen("compressed-archive.tar.gz", "wb");
    fwrite(output_buffer, 1, defstream.total_out, output_archive);

    // print the size of the compressed archive
    printf("Taille de l'archive compressée : %lu octets\n", defstream.total_out);

    // close the files
    fclose(input_archive);
    fclose(output_archive);

    // read the entire compressed archive into a buffer
    input_archive = fopen("compressed-archive.tar.gz", "rb");
    fread(input_buffer, 1, input_size, input_archive);

    // decompression process
    char decompressed_buffer[input_size];
    z_stream infstream = decompress_archive(input_buffer, decompressed_buffer, sizeof(decompressed_buffer), defstream);

    // write decompressed data to file
    FILE *decompressed_archive = fopen("decompressed-archive.tar", "wb");
    fwrite(decompressed_buffer, 1, sizeof(decompressed_buffer), decompressed_archive);

    // print the size of the decompressed archive
    printf("Taille de l'archive décompressée : %lu octets\n", infstream.total_out);

    // free the memory allocated for the archive
    free(input_buffer);

    // close the files
    fclose(input_archive);
    fclose(decompressed_archive);

    return 0;
}