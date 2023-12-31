#ifndef _CTAR_H
#define _CTAR_H

#include <zlib.h>
#include "typedef.h"

/**
 * Compress archive using zlib compression algorithm.
 *
 * @param src_buf A pointer to the archive to be compressed.
 * @param src_fsize The size of the source buffer.
 * @param dest_buf A pointer to the buffer where the compressed archive will be stored.
 * @param dest_fsize The size of the output buffer.
 * @return A z_stream struct containing information about the compression process.
 */
z_stream ctar_compress(char *src_buf, size_t src_fsize, char *dest_buf, size_t dest_fsize);

/**
 * Decompress archive using zlib decompression algorithm.
 *
 * @param src_buf A pointer to the archive to be decompressed.
 * @param src_fsize The size of the source buffer.
 * @param dest_buf A pointer to the buffer where the decompressed archive will be stored.
 * @param dest_fsize The size of the output buffer.
 * @return A z_stream struct containing information about the decompression process.
 */
z_stream ctar_decompress(char *src_buf, size_t src_fsize, char *dest_buf, size_t dest_fsize);

/**
 * @brief Get the archive size.
 *
 * @param fd The file descriptor of the archive.
 * @return The archive size.
 */
size_t get_archive_size(int fd);

/**
 * @brief Removes a substring from a given string.
 *
 * @param string The string from which the substring will be removed.
 * @param substring The substring to be removed from the string.
 */
void remove_substring(char *string, const char *substring);

/**
 * @brief Open an archive in the correct mode.
 * 
 * @param args The arguments of the program.
 * @return int file descriptor of the archive if successful, -1 otherwise.
 */
int ctar_open(ctar_args *args);

/**
 * @brief Close an archive.
 * 
 * @param fd The file descriptor of the archive.
 * @return int 0 if successful, -1 otherwise.
 */
int ctar_close(int fd, ctar_args *args);

/**
 * @brief List the content of the archive.
 * 
 * @param args The arguments of the program.
 * @param fd The file descriptor of the archive.
 * @return int 0 if successful, -1 otherwise. 
 */
int ctar_list(ctar_args *args, int fd);

/**
 * @brief Print a ctar entry.
 *
 * @param header The header of the entry.
 * @param verbose Whether to print verbose information.
 * @return int 0 if successful, -1 otherwise.
 */
int ctar_list_entry(ctar_header *header, bool verbose);

/**
 * @brief Extract the content of the archive.
 * 
 * @param args The arguments of the program.
 * @param fd The file descriptor of the archive.
 * @return int 0 if successful, -1 otherwise.
 */
int ctar_extract(ctar_args *args, int fd);

/**
 * @brief Extract a ctar entry.
 *
 * @param header The header of the entry.
 * @param verbose Whether to print verbose information.
 * @param fd The file descriptor of the archive.
 * @return int 0 if successful, -1 otherwise.
 */
int ctar_extract_entry(ctar_header *header, bool verbose, int fd);

/**
 * @brief Extract a regular file.
 *
 * @param header The header of the entry.
 * @param fd The file descriptor of the archive, pointing to the beginning of the data.
 * @return int 0 if successful, -1 otherwise.
 */
int ctar_extract_regular(ctar_header *header, int fd);

/**
 * @brief Extract a symbolic link.
 * 
 * @param header The header of the entry.
 * @param fd The file descriptor of the archive, pointing to the beginning of the data.
 * @return int 0 if successful, -1 otherwise.
 */
int ctar_extract_symlink(ctar_header *header, int fd);

/**
 * @brief Extract a directory.
 * 
 * @param header The header of the entry.
 * @param fd The file descriptor of the archive, pointing to the beginning of the data.
 * @return int 0 if successful, -1 otherwise.
 */
int ctar_extract_directory(ctar_header *header, int fd);

#endif // _CTAR_H