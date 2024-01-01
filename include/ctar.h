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


/**
 * Changes the current working directory of the ctar process.
 *
 * @param args The ctar_args structure containing the command-line arguments.
 * @param fd The file descriptor of the directory to change to.
 * @return Returns 0 on success, or -1 on failure.
 */
int ctar_chdir(ctar_args *args);

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

/**
 * @brief Create an archive.
 *
 * @param args The arguments of the program.
 * @param fd The file descriptor of the archive.
 * @return int 0 if successful, -1 otherwise.
 */
int ctar_create(ctar_args *args, int fd);

/**
 * @brief Create the end of the archive.
 *
 * @param fd The file descriptor of the archive.
 * @return int 0 if successful, -1 otherwise.
 */
int ctar_create_end_of_archive(int fd);

/**
 * @brief Create a ctar entry.
 *
 * @param path The path of the entry.
 * @param verbose Whether to print verbose information.
 * @param fd The file descriptor of the archive.
 * @return int 0 if successful, -1 otherwise.
 */
int ctar_create_entry(char *path, bool verbose, int fd);

/**
 * @brief Create a regular file.
 *
 * @param header The header of the entry.
 * @param fd The file descriptor of the archive.
 * @return int 0 if successful, -1 otherwise.
 */
int ctar_create_regular(ctar_header *header, int fd);

/**
 * @brief Create a symbolic link.
 *
 * @param header The header of the entry.
 * @param fd The file descriptor of the archive.
 * @return int 0 if successful, -1 otherwise.
 */
int ctar_create_symlink(ctar_header *header, int fd);

/**
 * @brief Create a directory.
 *
 * @param header The header of the entry.
 * @param fd The file descriptor of the archive.
 * @return int 0 if successful, -1 otherwise.
 */
int ctar_create_directory(ctar_header *header, int fd);

#endif // _CTAR_H