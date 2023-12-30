#include "ctar_zlib.h"
#include <stdio.h>

/**
 * @note This function will not close fd_in.
 * @note This function will reset fd_in to the beginning of the file.
 */
int ctar_compress(char *path, int fd_in)
{
  if (lseek(fd_in, 0, SEEK_SET) == -1)
  {
    perror("Unable to seek uncompressed file");
    return -1;
  }

  gzFile file_out = gzopen(path, "wb");
  if (file_out == NULL)
  {
    perror("Unable to open compressed file");
    return -1;
  }

  char buffer[CTAR_ZLIB_CHUNK];
  int nbytes;

  while ((nbytes = read(fd_in, buffer, CTAR_ZLIB_CHUNK)) > 0)
  {
    if (gzwrite(file_out, buffer, nbytes) == 0)
    {
      perror("Unable to write compressed file");  
      return -1;
    }
  }

  if (nbytes == -1)
  {
    perror("Unable to read uncompressed file");
    return -1;
  }

  if (gzclose(file_out) != Z_OK)
  {
    perror("Unable to close compressed file");
    return -1;
  }

  if (lseek(fd_in, 0, SEEK_SET) == -1)
  {
    perror("Unable to seek uncompressed file");
    return -1;
  }

  return 0;
}

/**
 * @note This function will not close fd_out.
 * @note This function will reset fd_out to the beginning of the file.
 */
int ctar_decompress(char *path, int fd_out)
{
  if (lseek(fd_out, 0, SEEK_SET) == -1)
  {
    perror("Unable to seek decompressed file");
    return -1;
  }

  gzFile file_in = gzopen(path, "rb");
  if (file_in == NULL)
  {
    perror("Unable to open compressed file");
    return -1;
  }

  char buffer[CTAR_ZLIB_CHUNK];
  int nbytes;

  while ((nbytes = gzread(file_in, buffer, CTAR_ZLIB_CHUNK)) > 0)
  {
    if (write(fd_out, buffer, nbytes) == -1)
    {
      perror("Unable to write decompressed file");
      return -1;
    }
  }

  if (nbytes == -1)
  {
    perror("Unable to read compressed file");
    return -1;
  }

  if (gzclose(file_in) != Z_OK)
  {
    perror("Unable to close compressed file");
    return -1;
  }

  if (lseek(fd_out, 0, SEEK_SET) == -1)
  {
    perror("Unable to seek decompressed file");
    return -1;
  }

  return 0;
}