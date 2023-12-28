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

#endif // _CTAR_H