#ifndef _UTILS_H
#define _UTILS_H

#include "typedef.h"
#include <sys/types.h>

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

/**
 * @brief Recursively create a directory.
 *
 * @param path The path of the directory to create.
 * @param mode The mode of the directory to create.
 * @return int 0 on success, -1 on failure.
 */
int mkdir_recursive(char *path, mode_t mode);

/**
 * @brief Skip the data blocks of a header.
 *
 * @param fd The file descriptor of the archive.
 * @param header The header to skip.
 * @return int 0 on success, -1 on failure.
 */
int skip_data_blocks(int fd, ctar_header *header);

/**
 * @brief Get the number of data blocks of a header.
 *
 * @param header The header to get the number of data blocks of.
 * @return int The number of data blocks.
 */
int get_nblocks(ctar_header *header);

#endif // _UTILS_H