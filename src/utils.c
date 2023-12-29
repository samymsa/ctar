#include "utils.h"

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
 * @note The checksum is computed by taking the sum of the unsigned byte
 * values of the header block with the chksum field taken to be all spaces.
 */
void compute_checksum(ctar_header *header)
{
  for (int i = 0; i < CTAR_CHKSUM_SIZE; i++)
  {
    header->chksum[i] = ' ';
  }

  int sum = 0;
  for (int i = 0; i < sizeof(ctar_header); i++)
  {
    sum += ((char *)header)[i];
  }

  dec2oct(sum, header->chksum, CTAR_CHKSUM_SIZE);
}
