#ifndef _CTAR_H
#define _CTAR_H

#include "typedef.h"

/**
 * @brief List the content of the archive.
 * 
 * @param args The arguments of the program.
 * @return int 0 if successful, -1 otherwise. 
 */
int ctar_list(ctar_args *args);

/**
 * @brief Print a ctar entry.
 * 
 * @param header The header of the entry.
 * @param verbose Whether to print verbose information.
 * @return int 0 if successful, -1 otherwise.
 */
int ctar_list_entry(ctar_header *header, bool verbose);

/**
 * Compress archive using zlib compression algorithm.
 *
 * @param input A pointer to the archive to be compressed.
 * @param output A pointer to the buffer where the compressed archive will be stored.
 * @param output_size The size of the output buffer.
 * @return A z_stream struct containing information about the compression process.
 */
z_stream compress_archive(char *input, char *output, size_t output_size);

/**
 * Decompress archive using zlib decompression algorithm.
 *
 * @param input A pointer to the compressed archive.
 * @param output A pointer to the buffer where the decompressed archive will be stored.
 * @param output_size The size of the output buffer.
 * @param defstream A z_stream struct containing the state of the previous compression.
 * @return A z_stream struct containing information about the decompression process.
 */
z_stream decompress_archive(char *input, char *output, size_t output_size, z_stream defstream);

/**
 * Test compression and decompression using zlib.
 * 
 * @return 0 if successful, 1 otherwise.
 */
int test_compression();

/**
 * @brief Convert an octal string to a decimal integer.
 *
 * @param oct The octal string.
 * @param size The size of the string,
 * @return int The decimal integer.
 */
int oct2dec(char *oct, int size);

/**
 * @brief Check if a header is blank.
 * 
 * @param header The header to check.
 * @return true If the header is blank.
 * @return false If the header is not blank.
 */
bool is_header_blank(ctar_header *header);

#endif // _CTAR_H