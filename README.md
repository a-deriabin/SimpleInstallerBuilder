# Simple installer builder
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

## Limitations
* Max file size - 4 GB
* Windows only
* Some anti-virus programs might consider generated installers as malicious
