#ifndef _ARGPARSE_H
#define _ARGPARSE_H

#include "typedef.h"

void parse_args(int argc, char **argv, ctar_args *args);
void print_usage(char *bin_name);

#endif // _ARGPARSE_H