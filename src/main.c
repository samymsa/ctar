#include "argparse.h"
#include "ctar.h"
#include <stdio.h>

int main(int argc, char **argv)
{
  ctar_args args = CTAR_ARGS_INIT;
  parse_args(argc, argv, &args);

  int fd = ctar_open(&args);
  if (fd == -1)
  {
    return EXIT_FAILURE;
  }

  // Change the current working directory to the one specified by the user.
  if (args.dir[0] && ctar_chdir(&args) == -1)
  {
    return EXIT_FAILURE;
  }

  if ((args.list && ctar_list(&args, fd) == -1) ||
      (args.extract && ctar_extract(&args, fd) == -1) ||
      (args.create && ctar_create(&args, fd) == -1))
  {
    return EXIT_FAILURE;
  }

  // Reset the current working directory to the original one.
  if (args.dir[0] && ctar_chdir(&args) == -1)
  {
    return EXIT_FAILURE;
  }

  if (ctar_close(&args, fd) == -1)
  {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
