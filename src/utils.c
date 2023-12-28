#include "utils.h"

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
