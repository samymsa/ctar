#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <libgen.h>
#include "ctar.h"
#include "utils.h"

z_stream ctar_compress(char *src_buf, size_t src_fsize, char *dest_buf, size_t dest_fsize)
{
  // zlib struct
  z_stream defstream;
  defstream.zalloc = Z_NULL;
  defstream.zfree = Z_NULL;
  defstream.opaque = Z_NULL;

  // setup input and compressed output
  defstream.avail_in = src_fsize;         // size of input
  defstream.next_in = (Bytef *)src_buf;   // input char array
  defstream.avail_out = dest_fsize;       // size of output
  defstream.next_out = (Bytef *)dest_buf; // output char array

  // the actual compression work
  deflateInit(&defstream, Z_BEST_COMPRESSION);
  deflate(&defstream, Z_FINISH);
  deflateEnd(&defstream);

  return defstream;
}

z_stream ctar_decompress(char *src_buf, size_t src_fsize, char *dest_buf, size_t dest_fsize)
{
  // zlib struct
  z_stream infstream;
  infstream.zalloc = Z_NULL;
  infstream.zfree = Z_NULL;
  infstream.opaque = Z_NULL;

  // setup input and decompressed output
  infstream.avail_in = src_fsize;         // size of input
  infstream.next_in = (Bytef *)src_buf;   // input char array
  infstream.avail_out = dest_fsize; // size of output
  infstream.next_out = (Bytef *)dest_buf; // output char array

  // the actual decompression work
  inflateInit(&infstream);
  inflate(&infstream, Z_NO_FLUSH);
  inflateEnd(&infstream);

  return infstream;
}

size_t get_archive_size(int fd)
{
  size_t fsize;
  fsize = lseek(fd, 0, SEEK_END);
  return fsize;
}

void remove_substring(char *string, const char *substring)
{
  char *substring_pos = strstr(string, substring);
  if (substring_pos != NULL)
  {
    // Calculate the length of the substring
    size_t len = strlen(substring);
    // Shift the rest of the string to overwrite the substring
    memmove(substring_pos, substring_pos + len, strlen(substring_pos + len) + 1);
  }
}

/**
 * If args->list or args->extract is true, the archive is opened in read-only mode.
 * Otherwise, the archive is opened in write-only mode.
 */
int ctar_open(ctar_args *args)
{
  int flags = args->list || args->extract ? O_RDONLY : O_WRONLY | O_CREAT | O_TRUNC;
  int fd = open(args->archive, flags, 0644);

  if (fd == -1)
  {
    perror("Unable to open archive");
    return -1;
  }

  if ((args->compress && args->list) || (args->compress && args->extract))
  {
    // get the size of the archive
    size_t fsize = get_archive_size(fd);

    printf("archive.tar.gz : %lu octets\n", fsize);

    // allocate memory for the archive
    char *src_buf = (char *)malloc(fsize);
    if (src_buf == NULL)
    {
      perror("Memory allocation error");
      close(fd);
      return -1;
    }

    // read the entire compressed archive into a buffer
    size_t number_read = read(fd, src_buf, fsize);
    if (number_read == -1)
    {
      perror("Unable to read archive");
      return -1;
    }

    // decompression process
    char dest_buf[fsize];
    z_stream infstream = ctar_decompress(src_buf, fsize, dest_buf, sizeof(dest_buf));

    printf("infstream.avail_in : %x octets\n", infstream.avail_in);
    printf("infstream.avail_out : %x octets\n", infstream.avail_out);

    // write decompressed data to file
    remove_substring(args->archive, ".gz");
    int fd_dest = open(args->archive, O_CREAT | O_WRONLY, 0644);
    if (fd_dest == -1)
    {
      perror("Unable to open archive");
      return -1;
    }

    write(fd_dest, dest_buf, infstream.total_out);

    // print the size of the decompressed archive
    printf("archive.tar : %lu octets\n", infstream.total_out);

    // free the memory allocated and close the file
    free(src_buf);
    close(fd_dest);
  }

  return fd;
}

int ctar_close(int fd, ctar_args *args)
{
  if (args->compress && args->create)
  {
    // get the size of the archive
    off_t fsize = get_archive_size(fd);
    printf("Taille de l'archive d'origine : %lu octets\n", fsize);

    // allocate memory for the archive
    char *src_buf = (char *)malloc(fsize);
    if (src_buf == NULL)
    {
      perror("Memory allocation error");
      close(fd);
      return -1;
    }

    // read the entire archive into a buffer
    ssize_t number_read = read(fd, src_buf, fsize);
    if (number_read == -1)
    {
      perror("Unable to read archive");
      return -1;
    }

    // compression process
    char dest_buf[fsize];
    z_stream defstream = ctar_compress(src_buf, fsize, dest_buf, sizeof(dest_buf));

    // write compressed data to file
    args->archive = strcat(args->archive, ".gz");
    int fd_dest = open(args->archive, O_CREAT | O_WRONLY, 0644);
    write(fd_dest, dest_buf, defstream.total_out);

    // print the size of the compressed archive
    printf("Taille de l'archive compressée : %lu octets\n", defstream.total_out);

    // free the memory allocated and close the file
    free(src_buf);
    close(fd_dest);
  }

  if (close(fd) == -1)
  {
    perror("Unable to close archive");
    return -1;
  }

  return 0;
}

int ctar_list(ctar_args *args, int fd)
{
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

    if (skip_data_blocks(fd, &header) == -1)
    {
      perror("Unable to skip data blocks");
      return -1;
    }
  }

  if (nbytes == -1)
  {
    perror("Unable to read archive");
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
  printf("%.*s", CTAR_NAME_SIZE, header->name);

  // Link name (if applicable)
  if (verbose && header->typeflag[0] == SYMTYPE)
  {
    printf(" -> %.*s", CTAR_LINKNAME_SIZE, header->linkname);
  }

  printf("\n");

  return 0;
}

int ctar_extract(ctar_args *args, int fd)
{
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

    if (ctar_extract_entry(&header, args->verbose, fd) == -1)
    {
      return -1;
    }
  }

  if (nbytes == -1)
  {
    perror("Unable to read archive");
    return -1;
  }

  return 0;
}

/**
 * The following file types are supported:
 * - regular file (REGTYPE): '-'
 * - symbolic link (SYMTYPE): 'l'
 * - directory (DIRTYPE): 'd'
 */
int ctar_extract_entry(ctar_header *header, bool verbose, int fd)
{
  if (verbose)
  {
    printf("%.*s\n", CTAR_NAME_SIZE, header->name);
  }

  switch (header->typeflag[0])
  {
  case REGTYPE:
  case AREGTYPE:
  case CONTTYPE:
    return ctar_extract_regular(header, fd);
  case SYMTYPE:
    return ctar_extract_symlink(header, fd);
  case DIRTYPE:
    return ctar_extract_directory(header, fd);
  default:
    fprintf(stderr, "Warning: unsupported file type '%c', skipping entry\n", header->typeflag[0]);
    return 0;
  }
}

int ctar_extract_regular(ctar_header *header, int fd)
{
  // Prepare directory
  // Create a copy of header name because dirname() may modify it
  char *dir = strdup(header->name);
  if (mkdir_recursive(dirname(dir), 0755) == -1)
  {
    perror("Unable to create parent directory");
    return -1;
  }
  free(dir);

  // Open output file
  int mode = oct2dec(header->mode, CTAR_MODE_SIZE);
  int out_fd = open(header->name, O_WRONLY | O_CREAT | O_TRUNC, mode);
  if (out_fd == -1)
  {
    perror("Unable to open output file");
    return -1;
  }

  // Copy data blocks
  int remaining = oct2dec(header->size, CTAR_SIZE_SIZE);
  char buf[CTAR_BLOCK_SIZE];
  while (remaining > 0)
  {
    int nbytes = read(fd, buf, CTAR_BLOCK_SIZE);
    if (nbytes == -1)
    {
      perror("Unable to read archive");
      return -1;
    }

    int nbytes_to_write = nbytes < remaining ? nbytes : remaining;
    if (write(out_fd, buf, nbytes_to_write) == -1)
    {
      perror("Unable to write to output file");
      return -1;
    }
    remaining -= nbytes;
  }

  // Close output file
  if (close(out_fd) == -1)
  {
    perror("Unable to close output file");
    return -1;
  }

  return 0;
}

int ctar_extract_symlink(ctar_header *header, int fd)
{
  if (skip_data_blocks(fd, header) == -1)
  {
    perror("Unable to skip data blocks");
    return -1;
  }

  if (symlink(header->linkname, header->name) == -1)
  {
    perror("Unable to create symbolic link");
    return -1;
  }

  return 0;
}

int ctar_extract_directory(ctar_header *header, int fd)
{
  if (skip_data_blocks(fd, header) == -1)
  {
    perror("Unable to skip data blocks");
    return -1;
  }

  int mode = oct2dec(header->mode, CTAR_MODE_SIZE);
  if (mkdir_recursive(header->name, mode) == -1)
  {
    perror("Unable to create directory");
    return -1;
  }

  return 0;
}