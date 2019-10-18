# GravelBox

GravelBox is a sandbox utility that allows the user to perform a controlled run of a binary executable, suspending it when the program attempts to perform actions such as writing/reading files, connecting to network, using the camera/microphone/speaker.
GravelBox would then give the user an option to allow or deny this access.
GravelBox would also have various configurations that can be set pre-run to allow/deny any combination of the permissions.

## Directories

- `bin`: compiled executable
- `obj`: compiled object files
- `src`: source code
  - `src/trace`: code to trace and intercept the target
  - `src/parser`: code to parse syscall arguments to human readable strings
  - `src/ui`: user interface code
  - `src/config`: code to save/load configuration files
  - `src/test`: test code
- `doc`: documentation

## Build Commands

Build and test GravelBox in release mode:

```bash
# build binaries and doc and test
make

# build in debug mode
make DEBUG=1

# build binaries
make build

# build docs
make doc

# test
make test

# remove all compiled binaries
make clean
```
