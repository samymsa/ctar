#ifndef _TYPEDEF_H
#define _TYPEDEF_H

#include <stdbool.h>
#include <stdlib.h>

/** @brief Default values for @ref ctar_args */
#define CTAR_ARGS_INIT \
  (ctar_args)          \
  {                    \
    .archive = NULL,   \
    .files = NULL,     \
    .dir = NULL,       \
    .list = false,     \
    .extract = false,  \
    .create = false,   \
    .compress = false, \
    .verbose = false,  \
  }

/** @brief Structure to store binary options */
typedef struct ctar_args
{
  char *archive;
  char **files;
  char *dir;
  bool list;
  bool extract;
  bool create;
  bool compress;
  bool verbose;
} ctar_args;

#endif // _TYPEDEF_H