# CTar

CTar is a simple tar-like program written in C. It supports the following features:
- [x] Listing files in a tar archive
- [X] Extracting files from a tar archive
- [X] Creating a tar archive
- [X] Compressing a tar archive using gzip
- [X] Decompressing a tar archive using gzip

gzip compression and decompression are implemented using the [zlib](https://github.com/madler/zlib/tree/master) library.

## Documentation

The full doxygen generated documentation can be found at [https://samymsa.github.io/ctar/](https://samymsa.github.io/ctar/)

It is deployed automatically using the [Doxygen GitHub Pages Deploy Action](https://github.com/marketplace/actions/doxygen-github-pages-deploy-action) and uses the [Doxygen Awesome](https://github.com/jothepro/doxygen-awesome-css) theme.

## Prerequisites

- [Build Essential](https://packages.ubuntu.com/focal/build-essential) (for compiling) : `sudo apt install build-essential`
- [Doxygen](https://packages.ubuntu.com/focal/doxygen) (optional, for generating documentation) : `sudo apt install doxygen`
- [Graphviz](https://packages.ubuntu.com/focal/graphviz) (optional, for generating documentation graphs) : `sudo apt install graphviz`
- [Lcov](https://packages.ubuntu.com/focal/lcov) (optional, for generating coverage reports) : `sudo apt install lcov`


## Installation

1. Compile the program using `make`
2. Run the program using `./bin/ctar`
3. Enjoy!

## Usage

### Syntax
> **Note:** From now on, the program will be referred to as `ctar` instead of `./bin/ctar` for simplicity.

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
- `-d, --directory DIR`: Change to DIR before performing any operations. Useful for creating or extracting files from/to a different directory than the current one
- `-z, --compress`: Compress or decompress the archive using gzip
- `-v, --verbose`: enable *verbose* mode
- `-h, --help`: display help
- `FILES...`: The files to add to the archive

### Examples
#### List Files in Archive:
- `ctar -l archive.tar`: List files in the archive.tar.

#### Extract Files from Archive:
- `ctar -e archive.tar`: Extract files from archive.tar into the current directory.
- `ctar -e archive.tar -d /tmp`: Extract files from archive.tar into the /tmp directory.

#### Create Archive:
- `ctar -c archive.tar file1 file2 file3`: Create archive.tar from file1, file2, and file3.
- `ctar -c archive.tar -d /tmp file1 file2 file3`: Create archive.tar from /tmp/file1, /tmp/file2, and /tmp/file3.

#### Compress and Decompress:
- `ctar -z -c archive.tar.gz file1 file2 file3`: Create compressed archive.tar.gz from file1, file2, and file3.
- `ctar -z -e archive.tar.gz -d /tmp`: Extract and decompress archive.tar.gz into the /tmp directory.

#### Change Directory Before Operation:
- `ctar -c archive.tar -d /tmp file1 file2 file3`: Create archive.tar from /tmp/file1, /tmp/file2, and /tmp/file3, changing to /tmp before the operation. This will result in the archive containing the files file1, file2, and file3, instead of /tmp/file1, /tmp/file2, and /tmp/file3.

#### Verbose Mode:
- `ctar -l archive.tar -v`: List files in archive.tar in verbose mode. This will print detailed information about the files in the archive.
- `ctar -e archive.tar -v -d /tmp`: Extract files from archive.tar into the /tmp directory in verbose mode. This will print the names of the files as they are extracted.
- `ctar -c archive.tar -v -d /tmp file1 file2 file3`: Create archive.tar from /tmp/file1, /tmp/file2, and /tmp/file3, changing to /tmp before the operation, in verbose mode. This will print the names of the files as they are added to the archive.

#### Help:
- `ctar -h`: Display help.

## Authors

- [@samymsa](https://www.github.com/samymsa)
- [@ybokhari](https://www.github.com/ybokhari)
