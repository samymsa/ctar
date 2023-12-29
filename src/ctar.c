#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include "ctar.h"
#include "zlib.h"

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

z_stream ctar_compress(char *input, size_t input_size, char *output, size_t output_size)
{
  // zlib struct
  z_stream defstream;
  defstream.zalloc = Z_NULL;
  defstream.zfree = Z_NULL;
  defstream.opaque = Z_NULL;

  // setup input and compressed output
  defstream.avail_in = input_size;      // size of input
  defstream.next_in = (Bytef *)input;   // input char array
  defstream.avail_out = output_size;    // size of output
  defstream.next_out = (Bytef *)output; // output char array

  // the actual compression work
  deflateInit(&defstream, Z_BEST_COMPRESSION);
  deflate(&defstream, Z_FINISH);
  deflateEnd(&defstream);

  return defstream;
}

z_stream ctar_decompress(char *input, char *output, size_t output_size, z_stream defstream)
{
  // zlib struct
  z_stream infstream;
  infstream.zalloc = Z_NULL;
  infstream.zfree = Z_NULL;
  infstream.opaque = Z_NULL;

  // setup input and decompressed output
  infstream.avail_in = (uInt)((char *)defstream.next_out - input); // size of input
  infstream.next_in = (Bytef *)input;                              // input char array
  infstream.avail_out = output_size;                               // size of output
  infstream.next_out = (Bytef *)output;                            // output char array

  // the actual decompression work
  inflateInit(&infstream);
  inflate(&infstream, Z_NO_FLUSH);
  inflateEnd(&infstream);

  return infstream;
}

int test_compression()
{
  // open the archive in binary mode
  FILE *input_archive = fopen("archive.tar", "rb");
  if (input_archive == NULL)
  {
    perror("Error during the opening of the archive");
    return 1;
  }

  // get the size of the archive
  fseek(input_archive, 0, SEEK_END);
  size_t input_size = ftell(input_archive);
  fseek(input_archive, 0, SEEK_SET);

  // allocate memory for the archive
  char *input_buffer = (char *)malloc(input_size);
  if (input_buffer == NULL)
  {
    perror("Erreur d'allocation mémoire");
    fclose(input_archive);
    return 1;
  }

  // print the size of the archive
  fread(input_buffer, 1, input_size, input_archive);
  printf("Taille de l'archive d'origine : %lu octets\n", input_size);

  // compression process
  char output_buffer[input_size];
  z_stream defstream = ctar_compress(input_buffer, input_size, output_buffer, sizeof(output_buffer));

  // write compressed data to file
  FILE *output_archive = fopen("compressed-archive.tar.gz", "wb");
  fwrite(output_buffer, 1, defstream.total_out, output_archive);

  // print the size of the compressed archive
  printf("Taille de l'archive compressée : %lu octets\n", defstream.total_out);

  // close the files
  fclose(input_archive);
  fclose(output_archive);

  // read the entire compressed archive into a buffer
  input_archive = fopen("compressed-archive.tar.gz", "rb");
  fread(input_buffer, 1, input_size, input_archive);

  // decompression process
  char decompressed_buffer[input_size];
  z_stream infstream = ctar_decompress(input_buffer, decompressed_buffer, sizeof(decompressed_buffer), defstream);

  // write decompressed data to file
  FILE *decompressed_archive = fopen("decompressed-archive.tar", "wb");
  fwrite(decompressed_buffer, 1, sizeof(decompressed_buffer), decompressed_archive);

  // print the size of the decompressed archive
  printf("Taille de l'archive décompressée : %lu octets\n", infstream.total_out);

  // free the memory allocated for the archive
  free(input_buffer);

  // close the files
  fclose(input_archive);
  fclose(decompressed_archive);

  return 0;
}

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
