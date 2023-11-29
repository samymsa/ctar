#include <getopt.h>
#include <stdio.h>
#include <errno.h>
#include "argparse.h"

/**
 * @brief Binary options declaration
 * (must end with {NULL, 0, NULL, 0})
 *
 * @see man 3 getopt_long or getopt
 * @see struct option definition
 */
static const struct option options[] =
    {
        {"list", required_argument, NULL, 'l'},
        {"extract", required_argument, NULL, 'e'},
        {"create", required_argument, NULL, 'c'},
        {"directory", required_argument, NULL, 'd'},
        {"compress", no_argument, NULL, 'z'},
        {"verbose", no_argument, NULL, 'v'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}};

/**
 * @brief Binary options string
 * (related to options declaration)
 *
 * @see man 3 getopt_long or getopt
 */
static const char *optstr = "l:e:c:d:zvh";

/**
 * @brief Print usage of the binary
 *
 * @param bin_name
 */
void print_usage(char *bin_name)
{
  char *syntax = "{-l|-e|-c} ARCHIVE [-d DIR] [-zvh] [FILES...]";
  char *params = "  -l, --list: List files in archive\n"
                 "  -e, --extract: Extract files from archive\n"
                 "  -c, --create: Create archive\n"
                 "  -d, --directory DIR: Change to DIR before performing any operations. This option is order-sensitive, i.e. it affects all options that follow.\n"
                 "  -z, --compress: Compress or decompress the archive using gzip\n"
                 "  -v, --verbose: enable verbose mode\n"
                 "  -h, --help: display this help\n"
                 "  ARCHIVE: archive file\n"
                 "  FILES: files to be added to the archive\n";
  printf("USAGE: %s %s\n%s", bin_name, syntax, params);
}

/**
 * @brief Parse binary options
 *
 * @param argc number of arguments
 * @param argv arguments
 * @param args @ref ctar_args output structure
 */
void parse_args(int argc, char **argv, ctar_args *args)
{
  int opt;
  int opt_index = 0;
  while ((opt = getopt_long(argc, argv, optstr, options, &opt_index)) != -1)
  {
    switch (opt)
    {
    case 'l':
      args->list = true;
      args->archive = optarg;
      break;
    case 'e':
      args->extract = true;
      args->archive = optarg;
      break;
    case 'c':
      args->create = true;
      args->archive = optarg;
      break;
    case 'd':
      args->dir = optarg;
      break;
    case 'z':
      args->compress = true;
      break;
    case 'v':
      args->verbose = true;
      break;
    case 'h':
      print_usage(argv[0]);
      exit(EXIT_SUCCESS);
    default:
      fprintf(stderr, "Try '%s -h' or '%s --help' for more information.\n", argv[0], argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  if (args->list + args->extract + args->create != 1)
  {
    fprintf(stderr, "You must specify exactly one of -l, -e, -c.\n");
    exit(EXIT_FAILURE);
  }

  if (argc > optind)
  {
    args->files = argv + optind;
  }
}