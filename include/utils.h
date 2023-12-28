#ifndef _UTILS_H
#define _UTILS_H

#include "typedef.h"

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

#endif // _UTILS_H