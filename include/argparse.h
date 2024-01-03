#ifndef _ARGPARSE_H
#define _ARGPARSE_H

#include "typedef.h"

/**
 * @brief Print usage of the binary
 *
 * @param bin_name
 */
void print_usage(char *bin_name);

/**
 * @brief Parse binary options
 *
 * @param argc number of arguments
 * @param argv arguments
 * @param args @ref ctar_args output structure
 * @return int 0 if success, -1 otherwise
 */
int parse_args(int argc, char **argv, ctar_args *args);

#endif // _ARGPARSE_H