# CTar

CTar is a simple tar-like program written in C. It supports the following features:
- [ ] Listing files in a tar archive
- [ ] Extracting files from a tar archive
- [ ] Creating a tar archive
- [ ] Compressing a tar archive using gzip
- [ ] Decompressing a tar archive using gzip

gzip compression and decompression are implemented using the [zlib](https://github.com/madler/zlib/tree/master) library.


## Installation

1. Compile the program using `make`
2. Run the program using `./bin/ctar`
3. Enjoy!

## Usage

### Syntax
The syntax of ctar is the following:

```bash
ctar {-l|-e|-c} ARCHIVE [-d DIR] [-zvh] [FILES...]
```

### Arguments
#### Required arguments:
- One of the following operations (mutually exclusive)
  - `-l, --list ARCHIVE`: List files in archive
  - `-e, --extract ARCHIVE`: Extract files from archive
  - `-c, --create ARCHIVE`: Create archive

#### Optional arguments:
- `-d, --directory DIR`: Change to DIR before performing any operations.  This option is order-sensitive, i.e. it affects all options that follow.
- `-z, --compress`: Compress or decompress the archive using gzip
- `-v, --verbose`: enable *verbose* mode
- `-h, --help`: display help
- `FILES...`: The files to add to the archive

### Examples
- `ctar -l archive.tar`: List files in archive.tar
- `ctar -e archive.tar`: Extract files from archive.tar into the current directory
- `ctar -e archive.tar -d /tmp`: Extract files from archive.tar into the /tmp directory
- `ctar -c archive.tar file1 file2 file3`: Create archive.tar from file1, file2, and file3
- `ctar -c archive.tar -d /tmp file1 file2 file3`: Create archive.tar from /tmp/file1, /tmp/file2, and /tmp/file3

## Authors

- [@samymsa](https://www.github.com/samymsa)
- [@ybokhari](https://www.github.com/ybokhari)
