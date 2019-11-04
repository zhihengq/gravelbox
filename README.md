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

## Dependencies

- Boost Program Options
- JsonCpp

## Building GravelBox

Basic build commands:

```sh
# build in debug mode
make

# build in release mode
make RELEASE=1

# build binaries
make build

# build docs
make doc

# test
make test

# remove all compiled binaries
make clean
```

Flags:

- `RELEASE`: build in release mode
- `SANITIZERS`: sanitizer flags used in debug builds. Default to ASAN+UBSAN (`-fsanitize=address,undefined`).
- `CXXEXTRA`: extra flags passed to compile C++ objects.
- `LDEXTRA`: extra flags passed to link binaries.

## Building GravelBox on `attu`

This section is for people who are not familiar with building C++ projects and just want to build GravelBox on attu as easy as possible.
Our current directory start from the root directory of GravelBox.

GravelBox requires JsonCpp which is not installed on `attu`.
Therefore, we have to download JsonCpp first:

```sh
git clone https://github.com/open-source-parsers/jsoncpp.git
cd jsoncpp
```

We can build JsonCpp using CMake:

```sh
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=release -DBUILD_STATIC_LIBS=ON -DBUILD_SHARED_LIBS=OFF -DARCHIVE_INSTALL_DIR=. -G "Unix Makefiles" ..
make
```

If no error occurred, we have finished building JsonCpp.
We can go back to the root directory of GravelBox:

```sh
cd ../..
```

Since the linker on `attu` cannot find ASAN/UBSAN libraries, we have to disable them by `SANITIZER=`.
We also need to add extra compiler and linker flags to indicate the location of JsonCpp.

We can build the binaries in debug mode:

```sh
make SANITIZER= CXXEXTRA='-Ijsoncpp/include' LDEXTRA='-Ljsoncpp/build/src/lib_json'
```

or in release mode:

```sh
make RELEASE=1 CXXEXTRA='-Ijsoncpp/include' LDEXTRA='-Ljsoncpp/build/src/lib_json'
```

You will see a warning `Linux kernel version < 3.11, PTRACE_O_KILLEXIT is disabled`.
This is expected, since `attu` has a very old kernel that does not support some security features we use.

Finally, we can build the documentation.

```sh
make doc
```

You can find the binary under `bin` directory and the documentation under `doc` directory.
