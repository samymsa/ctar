#include "utils.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int oct2dec(char *oct, int size)
{
  int dec = 0;
  for (int i = 0; i < size; i++)
  {
    if (oct[i] == '\0')
    {
      break;
    }
    dec = dec * 8 + oct[i] - '0';
  }
  return dec;
}

void dec2oct(int dec, char *oct, int size)
{
  int i = size - 1;
  oct[i--] = '\0';
  while (dec > 0 && i >= 0)
  {
    oct[i] = dec % 8 + '0';
    dec /= 8;
    i--;
  }
  while (i >= 0)
  {
    oct[i] = '0';
    i--;
  }
}

bool is_header_blank(ctar_header *header)
{
  for (int i = 0; i < sizeof(ctar_header); i++)
  {
    if (((char *)header)[i] != 0)
    {
      return false;
    }
  }
  return true;
}

int mkdir_recursive(char *path, mode_t mode)
{
  char *sep = strrchr(path, '/');
  if (sep != NULL)
  {
    *sep = '\0';
    if (mkdir_recursive(path, mode) == -1)
    {
      return -1;
    }
    *sep = '/';
  }

  if (mkdir(path, mode) == -1 && errno != EEXIST)
  {
    return -1;
  }

  return 0;
}

int skip_data_blocks(int fd, ctar_header *header)
{
  return lseek(fd, get_nblocks(header) * CTAR_BLOCK_SIZE, SEEK_CUR);
}

int get_nblocks(ctar_header *header)
{
  int size = oct2dec(header->size, CTAR_SIZE_SIZE);
  int nblocks = size / CTAR_BLOCK_SIZE + (size % CTAR_BLOCK_SIZE == 0 ? 0 : 1);
  return nblocks;
}

/**
 * @note The checksum is calculated by taking the sum of the
 * unsigned byte values of the header record with the eight
 * checksum bytes taken to be ASCII spaces (decimal value 32).
 */
void compute_checksum(ctar_header *header)
{
  memset(header->chksum, ' ', CTAR_CHKSUM_SIZE);

  int sum = 0;
  for (int i = 0; i < sizeof(ctar_header); i++)
  {
    sum += ((char *)header)[i];
  }

  dec2oct(sum, header->chksum, CTAR_CHKSUM_SIZE - 1);

  // The checksum is terminated by a null and a space.
  header->chksum[CTAR_CHKSUM_SIZE - 2] = '\0';
  header->chksum[CTAR_CHKSUM_SIZE - 1] = ' ';
}

bool is_checksum_valid(ctar_header *header)
{
  // Save the original checksum.
  char chksum[CTAR_CHKSUM_SIZE];
  memcpy(chksum, header->chksum, CTAR_CHKSUM_SIZE);
  
  // Compute the checksum and compare it with the original one.
  compute_checksum(header);
  bool is_valid = memcmp(chksum, header->chksum, CTAR_CHKSUM_SIZE) == 0;

  // Restore the original checksum.
  memcpy(header->chksum, chksum, CTAR_CHKSUM_SIZE);

  return is_valid;
}

int ctar_mkstemp()
{
  char tmp_archive_path[] = "/tmp/ctar-XXXXXX";
  int tmp_fd = mkstemp(tmp_archive_path);
  if (tmp_fd == -1 || unlink(tmp_archive_path) == -1)
  {
    perror("Unable to create temporary file");
    return -1;
  }
  
  return tmp_fd;
}
