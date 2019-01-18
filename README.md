# Simple installer builder
## Features
* Generates single-file installers (no additional dependencies required)
* Can generate uninstallers
* Supports directory nesting
* Uses Huffman compression algorithm to reduce executable size

## Build
* `make` to build project
* `make clean` to clean .o and .exe files
* `make test` to run tests

## Usage
* Build a project using `make`
* You will get `InstallerBuilder.exe` in `bin` directory. Copy it whereever you need
* Create a text file with list of installation files.
  + By default, it is called `install_files.txt` and should be located in the same directory as `InstallerBuilder.exe`
  + You can change its path using `-l <new_path>` flag
* You will get a single `Installer.exe` file. Feel free to distribute it.

## Command-line options
* `-l <new_path>` - change path to installer list file
* `-u [Y/N]` - whether installer should generate uninstaller (yes by default)

## Installer list file structure
* Each line is a separate file/directory
* If it is a file, it will be installed into root of user specified directory
* If it is a directory, it will preserve all inner files and subdirectories
* If file/directory not found, InstallerBuilder will fail

## Limitations
* Max file size - 4 GB
* Windows only
* Some anti-virus programs might consider generated installer/uninstaller a malicious file
