#ifndef _CTAR_ZLIB_H_
#define _CTAR_ZLIB_H_

#include "typedef.h"
#include <zlib.h>

#define CTAR_ZLIB_CHUNK 16384 // Represents the size of the buffer used to read/write data

/**
 * @brief Compress from fd_in into path.
 * 
 * @param path The path of the compressed file to create.
 * @param fd_in The file descriptor of the uncompressed file.
 * @return int 0 if successful, -1 otherwise.
 */
int ctar_compress(char *path, int fd_in);

/**
 * @brief Decompress from path into fd_out.
 * 
 * @param path The path of the compressed file to decompress.
 * @param fd_out The file descriptor of the uncompressed file.
 * @return int 0 if successful, -1 otherwise.
 */
int ctar_decompress(char *path, int fd_out);

#endif // _CTAR_ZLIB_H_