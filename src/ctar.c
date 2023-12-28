#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include "ctar.h"
#include "utils.h"

int ctar_list(ctar_args *args)
{
  int fd = open(args->archive, O_RDONLY);
  if (fd == -1)
  {
    perror("Unable to open archive");
    return -1;
  }

  ctar_header header;
  int nbytes;
  int blank_header_count = 0;

  while ((nbytes = read(fd, &header, sizeof(ctar_header))) > 0 && blank_header_count < 2)
  {
    if (is_header_blank(&header))
    {
      blank_header_count++;
      continue;
    }

    if (ctar_list_entry(&header, args->verbose) == -1)
    {
      return -1;
    }

    // Go to the next header, skipping the data blocks
    int size = oct2dec(header.size, CTAR_SIZE_SIZE);
    int nblocks = size / CTAR_BLOCK_SIZE + (size % CTAR_BLOCK_SIZE == 0 ? 0 : 1);
    int data_size = nblocks * CTAR_BLOCK_SIZE;
    if (lseek(fd, data_size, SEEK_CUR) == -1)
    {
      perror("Unable to seek archive");
      return -1;
    }
  }

  if (nbytes == -1)
  {
    perror("Unable to read archive");
    return -1;
  }

  if (close(fd) == -1)
  {
    perror("Unable to close archive");
    return -1;
  }

  return 0;
}

/**
 * If verbose is true, the following information is printed:
 * - file mode
 * - owner and group
 * - file size
 * - last modification time
 * - file name
 *
 * Otherwise, only the file name is printed.
 *
 * The following file types are supported:
 * - regular file (REGTYPE): '-'
 * - hard link (LNKTYPE): 'h'
 * - symbolic link (SYMTYPE): 'l'
 * - character special (CHRTYPE): 'c'
 * - block special (BLKTYPE): 'b'
 * - directory (DIRTYPE): 'd'
 * - FIFO special (FIFOTYPE): 'p'
 */
int ctar_list_entry(ctar_header *header, bool verbose)
{
  if (verbose)
  {
    // File mode
    mode_t mode = (mode_t)oct2dec(header->mode, CTAR_MODE_SIZE);
    char types[] = "-hlcbdp-";
    char mode_str[10] = {
        types[header->typeflag[0] ? header->typeflag[0] - '0' : 0],
        mode & S_IRUSR ? 'r' : '-',
        mode & S_IWUSR ? 'w' : '-',
        mode & S_ISUID ? 'S' : (mode & S_IXUSR ? 'x' : '-'),
        mode & S_IRGRP ? 'r' : '-',
        mode & S_IWGRP ? 'w' : '-',
        mode & S_ISGID ? 'S' : (mode & S_IXGRP ? 'x' : '-'),
        mode & S_IROTH ? 'r' : '-',
        mode & S_IWOTH ? 'w' : '-',
        mode & S_IXOTH ? 'x' : '-',
    };
    printf("%.10s ", mode_str);

    // Owner and group
    printf("%.*s/%.*s ", CTAR_UNAME_SIZE, header->uname, CTAR_GNAME_SIZE, header->gname);

    // File size
    printf("%7d ", oct2dec(header->size, CTAR_SIZE_SIZE));

    // Last modification time
    time_t mtime = (time_t)oct2dec(header->mtime, CTAR_MTIME_SIZE);
    struct tm *tm = localtime(&mtime);
    if (tm == NULL)
    {
      perror("Unable to get modification time");
      return -1;
    }

    char mtime_str[20];
    strftime(mtime_str, sizeof(mtime_str), "%Y-%m-%d %H:%M", tm);
    printf("%s ", mtime_str);
  }

  // File name
  printf("%.*s\n", CTAR_NAME_SIZE, header->name);

  return 0;
}
