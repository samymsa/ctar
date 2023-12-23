#ifndef _COMPRESS_DECOMPRESS_H
#define _COMPRESS_DECOMPRESS_H

#include "zlib.h"

/**
 * Compress file using zlib compression algorithm.
 *
 * @param input A pointer to the file to be compressed.
 * @param output A pointer to the buffer where the compressed file will be stored.
 * @param output_size The size of the output buffer.
 * @return A z_stream struct containing information about the compression process.
 */
z_stream compress_file(char *input, char *output, size_t output_size);

/**
 * Decompress file using zlib decompression algorithm.
 *
 * @param input A pointer to the compressed file.
 * @param output A pointer to the buffer where the decompressed file will be stored.
 * @param output_size The size of the output buffer.
 * @param defstream A z_stream struct containing the state of the previous compression.
 * @return A z_stream struct containing information about the decompression process.
 */
z_stream decompress_file(char *input, char *output, size_t output_size, z_stream defstream);

#endif // _COMPRESS_DECOMPRESS_H
