#include "argparse.h"
#include "ctar.h"
#include <stdio.h>

int main(int argc, char **argv)
{
  ctar_args args = CTAR_ARGS_INIT;
  parse_args(argc, argv, &args);

  if (args.list && ctar_list(&args) == -1)
  {
    return EXIT_FAILURE;
  }

  // printf("PARSED ARGS:\n");
  // printf("archive: %s\n", args.archive);
  // printf("dir: %s\n", args.dir);
  // printf("list: %d\n", args.list);
  // printf("extract: %d\n", args.extract);
  // printf("create: %d\n", args.create);
  // printf("compress: %d\n", args.compress);
  // printf("verbose: %d\n", args.verbose);
  // printf("files: ");
  // for (int i = 0; args.files != NULL && args.files[i] != NULL; i++)
  // {
  //   printf("%s ", args.files[i]);
  // }
  // printf("\n");
  
  return EXIT_SUCCESS;
}
