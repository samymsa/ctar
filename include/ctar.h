#ifndef _CTAR_H
#define _CTAR_H

#include "typedef.h"

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
int ctar_close(int fd);

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

#endif // _CTAR_H