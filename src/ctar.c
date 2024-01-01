#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <libgen.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <linux/limits.h>
#include "ctar.h"
#include "ctar_zlib.h"
#include "utils.h"

/**
 * If args->list or args->extract is true, the archive is opened in read-only mode.
 * Otherwise, the archive is opened in write-only mode.
 */
int ctar_open(ctar_args *args)
{
  if (args->compress && args->create)
  {
    // Create a tmp file to work on
    return ctar_mkstemp();
  }

  if (args->compress && (args->list || args->extract))
  {
    // Create a tmp file to work on
    int tmp_fd = ctar_mkstemp();
    if (tmp_fd == -1)
    {
      return -1;
    }

    // Decompress archive into the tmp file
    if (ctar_decompress(args->archive, tmp_fd) == -1)
    {
      return -1;
    }

    return tmp_fd;
  }

  int flags = args->list || args->extract ? O_RDONLY : O_WRONLY | O_CREAT | O_TRUNC;
  int fd = open(args->archive, flags, 0644);
  if (fd == -1)
  {
    perror("Unable to open archive");
    return -1;
  }

  return fd;
}

int ctar_close(ctar_args *args, int fd)
{
  if (args->compress && args->create)
  {
    // Compress the tmp file into the original archive path
    if (ctar_compress(args->archive, fd) == -1)
    {
      return -1;
    }
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
  if (!is_checksum_valid(header))
  {
    fprintf(stderr, "Warning: checksum mismatch, skipping entry\n");
    return 0;
  }

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
    // Get user and group from header->uid and header->gid
    struct passwd *pw = getpwuid(oct2dec(header->uid, CTAR_UID_SIZE));
    if (pw == NULL)
    {
      perror("Unable to get user");
      return -1;
    }

    struct group *gr = getgrgid(oct2dec(header->gid, CTAR_GID_SIZE));
    if (gr == NULL)
    {
      perror("Unable to get group");
      return -1;
    }

    printf("%.*s/%.*s ", CTAR_UNAME_SIZE, pw->pw_name, CTAR_GNAME_SIZE, gr->gr_name);

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

/**
 * @note This will change the chdir to args->dir.
 * The old working directory is stored in args->dir.
 * Call ctar_chdir() again to reset the current working directory.
 */
int ctar_chdir(ctar_args *args)
{
  // Save the (old) current working directory
  char old_cwd[CTAR_ARGS_DIR_SIZE];
  if (getcwd(old_cwd, sizeof(old_cwd)) == NULL)
  {
    perror("Unable to get current working directory");
    return -1;
  }
  
  if (chdir(args->dir) == -1)
  {
    perror("Unable to change directory");
    return -1;
  }

  // Store old_cwd in args->dir
  if (strncpy(args->dir, old_cwd, CTAR_ARGS_DIR_SIZE) == NULL)
  {
    perror("Unable to store old current working directory");
    return -1;
  }

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
  if (!is_checksum_valid(header))
  {
    fprintf(stderr, "Warning: checksum mismatch, skipping entry\n");
    return 0;
  }
  
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

int ctar_create(ctar_args *args, int fd)
{
  for (int i = 0; args->files != NULL && args->files[i] != NULL; i++)
  {
    if (ctar_create_entry(args->files[i], args->verbose, fd) == -1)
    {
      return -1;
    }
  }
  return ctar_create_end_of_archive(fd);
}

/**
 * The end of archive is marked by two consecutive blank headers.
 */
int ctar_create_end_of_archive(int fd)
{
  char buf[2 * CTAR_BLOCK_SIZE];
  memset(buf, 0, sizeof(buf));
  if (write(fd, buf, sizeof(buf)) == -1)
  {
    perror("Unable to write end of archive");
    return -1;
  }

  return 0;
}

/**
 * The following file types are supported:
 * - regular file (REGTYPE): '-'
 * - symbolic link (SYMTYPE): 'l'
 * - directory (DIRTYPE): 'd'
 *
 * @note The following USTAR fields are not used:
 * - uname
 * - gname
 * - devmajor
 * - devminor
 * - prefix
 */
int ctar_create_entry(char *path, bool verbose, int fd)
{
  // Check if path is not the same as the archive
  struct stat st_archive;
  if (fstat(fd, &st_archive) == -1)
  {
    perror("Unable to stat archive");
    return -1;
  }

  struct stat st;
  if (lstat(path, &st) == -1)
  {
    perror("Unable to stat file");
    return -1;
  }

  if (st_archive.st_dev == st.st_dev && st_archive.st_ino == st.st_ino)
  {
    fprintf(stderr, "Warning: archive and file '%s' are the same, skipping entry\n", path);
    return 0;
  }

  ctar_header header = CTAR_HEADER_INIT;
  strncpy(header.name, path, CTAR_NAME_SIZE);
  dec2oct(st.st_mode, header.mode, CTAR_MODE_SIZE);
  dec2oct(st.st_uid, header.uid, CTAR_UID_SIZE);
  dec2oct(st.st_gid, header.gid, CTAR_GID_SIZE);
  dec2oct(st.st_size, header.size, CTAR_SIZE_SIZE);
  dec2oct(st.st_mtime, header.mtime, CTAR_MTIME_SIZE);
  strncpy(header.uname, getpwuid(st.st_uid)->pw_name, CTAR_UNAME_SIZE);
  strncpy(header.gname, getgrgid(st.st_gid)->gr_name, CTAR_GNAME_SIZE);

  if (verbose)
  {
    printf("%.*s\n", CTAR_NAME_SIZE, header.name);
  }

  if (S_ISREG(st.st_mode))
  {
    return ctar_create_regular(&header, fd);
  }

  if (S_ISLNK(st.st_mode))
  {
    return ctar_create_symlink(&header, fd);
  }

  if (S_ISDIR(st.st_mode))
  {
    return ctar_create_directory(&header, verbose, fd);
  }

  fprintf(stderr, "Warning: unsupported file type '%c', skipping entry\n", header.typeflag[0]);
  return 0;
}

int ctar_create_regular(ctar_header *header, int fd)
{
  // Write header
  header->typeflag[0] = REGTYPE;
  compute_checksum(header);
  if (write(fd, header, sizeof(ctar_header)) == -1)
  {
    perror("Unable to write header");
    return -1;
  }

  // Write data blocks
  int in_fd = open(header->name, O_RDONLY);
  if (in_fd == -1)
  {
    perror("Unable to open input file");
    return -1;
  }

  // Copy data blocks
  int nbytes;
  char buf[CTAR_BLOCK_SIZE];
  while ((nbytes = read(in_fd, buf, CTAR_BLOCK_SIZE)) > 0)
  {
    if (nbytes < CTAR_BLOCK_SIZE)
    {
      // Pad last block with zeros
      memset(buf + nbytes, 0, CTAR_BLOCK_SIZE - nbytes);
    }

    if (write(fd, buf, CTAR_BLOCK_SIZE) == -1)
    {
      perror("Unable to write to archive");
      return -1;
    }
  }

  if (nbytes == -1)
  {
    perror("Unable to read input file");
    return -1;
  }

  // Close input file
  if (close(in_fd) == -1)
  {
    perror("Unable to close input file");
    return -1;
  }

  return 0;
}

int ctar_create_symlink(ctar_header *header, int fd)
{
  // Read link name
  if (readlink(header->name, header->linkname, CTAR_LINKNAME_SIZE) == -1)
  {
    perror("Unable to read link name");
    return -1;
  }

  // Write header
  header->typeflag[0] = SYMTYPE;
  dec2oct(0, header->size, CTAR_SIZE_SIZE); // Size is always 0 for symbolic links
  compute_checksum(header);

  if (write(fd, header, sizeof(ctar_header)) == -1)
  {
    perror("Unable to write header");
    return -1;
  }

  return 0;
}

/**
 * Adding a directory to the archive will recursively add all files and directories inside it.
 */
int ctar_create_directory(ctar_header *header, bool verbose, int fd)
{
  // Write header
  header->typeflag[0] = DIRTYPE;
  dec2oct(0, header->size, CTAR_SIZE_SIZE); // Size is always 0 for directories
  compute_checksum(header);

  if (write(fd, header, sizeof(ctar_header)) == -1)
  {
    perror("Unable to write header");
    return -1;
  }

  // Add files and directories inside the directory
  DIR *dir = opendir(header->name);
  if (dir == NULL)
  {
    perror("Unable to open directory");
    return -1;
  }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL)
  {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
    {
      continue;
    }

    // If header->name ends with a slash, we need to remove it
    if (header->name[strlen(header->name) - 1] == '/')
    {
      header->name[strlen(header->name) - 1] = '\0';
    }

    char path[CTAR_NAME_SIZE];
    int snprint_result = snprintf(path, sizeof(path), "%s/%s", header->name, entry->d_name);
    if (snprint_result >= sizeof(path))
    {
      fprintf(stderr, "Warning: path '%s/%s' is too long, skipping entry\n", header->name, entry->d_name);
      continue;
    }

    if (snprint_result < 0)
    {
      fprintf(stderr, "Warning: unable to create path '%s/%s', skipping entry\n", header->name, entry->d_name);
      continue;
    }

    if (ctar_create_entry(path, verbose, fd) == -1)
    {
      return -1;
    }
  }

  if (closedir(dir) == -1)
  {
    perror("Unable to close directory");
    return -1;
  }

  return 0;
}
