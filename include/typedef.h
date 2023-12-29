#ifndef _TYPEDEF_H
#define _TYPEDEF_H

#include <stdbool.h>
#include <stdlib.h>

#define CTAR_NAME_SIZE 100
#define CTAR_MODE_SIZE 8
#define CTAR_UID_SIZE 8
#define CTAR_GID_SIZE 8
#define CTAR_SIZE_SIZE 12
#define CTAR_MTIME_SIZE 12
#define CTAR_CHKSUM_SIZE 8
#define CTAR_TYPEFLAG_SIZE 1
#define CTAR_LINKNAME_SIZE 100
#define CTAR_MAGIC_SIZE 6
#define CTAR_VERSION_SIZE 2
#define CTAR_UNAME_SIZE 32
#define CTAR_GNAME_SIZE 32
#define CTAR_DEVMAJOR_SIZE 8
#define CTAR_DEVMINOR_SIZE 8
#define CTAR_PREFIX_SIZE 155
#define CTAR_PAD_SIZE 12
#define CTAR_BLOCK_SIZE 512

#define REGTYPE  '0'            /* regular file */
#define AREGTYPE '\0'           /* regular file */
#define LNKTYPE  '1'            /* link */
#define SYMTYPE  '2'            /* reserved */
#define CHRTYPE  '3'            /* character special */
#define BLKTYPE  '4'            /* block special */
#define DIRTYPE  '5'            /* directory */
#define FIFOTYPE '6'            /* FIFO special */
#define CONTTYPE '7'            /* reserved */

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

/** @brief Binary options structure */
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

#define CTAR_HEADER_INIT \
  (ctar_header)         \
  {                     \
    .name = "",         \
    .mode = "",         \
    .uid = "",          \
    .gid = "",          \
    .size = "",         \
    .mtime = "",        \
    .chksum = "",       \
    .typeflag = "",     \
    .linkname = "",     \
    .magic = "ustar",   \
    .version = "00",    \
    .uname = "",        \
    .gname = "",        \
    .devmajor = "",     \
    .devminor = "",     \
    .prefix = "",       \
    .pad = "",          \
  }

/** @brief USTAR format header structure */
typedef struct ctar_header
{
  char name[CTAR_NAME_SIZE];
  char mode[CTAR_MODE_SIZE];
  char uid[CTAR_UID_SIZE];
  char gid[CTAR_GID_SIZE];
  char size[CTAR_SIZE_SIZE];
  char mtime[CTAR_MTIME_SIZE];
  char chksum[CTAR_CHKSUM_SIZE];
  char typeflag[CTAR_TYPEFLAG_SIZE];
  char linkname[CTAR_LINKNAME_SIZE];
  char magic[CTAR_MAGIC_SIZE]; // "ustar"
  char version[CTAR_VERSION_SIZE];
  char uname[CTAR_UNAME_SIZE];
  char gname[CTAR_GNAME_SIZE];
  char devmajor[CTAR_DEVMAJOR_SIZE];
  char devminor[CTAR_DEVMINOR_SIZE];
  char prefix[CTAR_PREFIX_SIZE];
  char pad[CTAR_PAD_SIZE];
} ctar_header;

#endif // _TYPEDEF_H