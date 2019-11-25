# GravelBox

GravelBox is a sandbox utility that allows the user to run of a binary executable with a controlled set of permissions.
GravelBox works by suspending the target program when it attempts to access system resources such as writing/reading files and connecting to network.
GravelBox would then give the user an option to allow or deny this access.
GravelBox would also have pre-set configurations that can allow or deny any combination of the permissions automatically.

The configuration file should be signed by a key using HMAC-SHA-512 before GravelBox starts.
When GravelBox starts, it will ask the user for this signing key to verify the integrity and authenticity of the configuration file.
If the configuration is not signed by the key provided by the user, it may suggest that the configuration file might have been tampered with.
The user can also optionally set a password that needs to be entered every time when the user want to allow a system call.

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
- `targets`: example targets to be traced

## Dependencies
Notes: The user is responsible for making sure to download the safe version of JsonCpp. If the user wants to replace our dependency files with their own ones, the user is responsible for using the secure verison.
- boost
  - `program_options`
  - `iostreams`
  - Note: boost versions previous to 1.5.2 are insecure. Please check the version on the user's OS to make sure it's secure.
- JsonCpp
  - Note: JsonCpp versions previous to 1.9.2 are insecure. Please use the most recent version.
- OpenSSL
  - Note: OpenSSL verison 1.0.1g is insecure. Please use the most recent version.
- GnuPG
  - Note: GnuPG versions 1.4.8 and 2.0.8 are insecure. Please avoid these two versions.

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

# build example targets in debug mode
make targets

# build example targets in release mode
make RELEASE=1 targets

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
make SANITIZERS= CXXEXTRA='-Ijsoncpp/include' LDEXTRA='-Ljsoncpp/build/src/lib_json'
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

## Configuring GravelBox

The default path of the configuration file is `gravelbox_config.json` under the working directory of GravelBox.
The configuration should be a JSON file.
The settings include:

- `signature`:
    The path of the signature file.
- `password`:
    a hash of the user decision password that needs to be entered every time when the user want to allow a system call.
    GravelBox will not ask the user for password if this setting is missing or empty.
- `syscall-definition`:
    The path of the system call definition file.
- `pinentry`:
    The pinentry UI program to use.
- `max-string-length`:
    The maximum number of characters to display when displaying a string parameter.
  - Note: The string length should be less than 1 memory page size for GravelBox to work correctly.
- `action-group`:
    A list of action groups, each containing a list of regular expressions and an action if one of the regular expressions matches the system call.
    An action can be "allow", "deny", or "ask".
    Earlier action groups will shadow later action groups.
- `default-action`:
    An action to take if no action group matches a system call.

In the repository, there is a example configuration file.
The configuration file signing key is "key" and the user decision password is "password".

## Configuration File Signing Key and User Decision Password

There are two passwords for GravelBox:

- The configuration file signing key is used to generate a MAC for the configuration file.
  It is not saved anywhere and must be entered every time the user starts GravelBox.
- The user decision password is used to authenticate the user when the user choose to allow a system call by interacting with GravelBox UI.
  The hash of the user decision password is saved in the configuration file.
  The user can choose to not use a user decision password by making the hash empty.

Both the configuration file signature and the password hash are generated using HMAC-SHA-512.
The signature file is a binary file containing exactly the signature (64 bytes).
The password hash in the configuration file is hex encoded and should be 128 characters long.

The user can pass `-n` or `--no-signature` flags to GravelBox to skip signature verification.
However, GravelBox will then assume the configuration file is not protected by a key, which means the password is hashed by an empty key.

## Running GravelBox

Usage:

```text
gravelbox [options] [--] target [args...]
```

Supported options can be found in the help message.
Example commands:

```sh
# Show help message
gravelbox -h
gravelbox --help

# Run commands under GravelBox
gravelbox echo hello world
gravelbox /usr/bin/ping google.com

# Run commands with options
gravelbox -- echo -e "hello\nworld"

# I/O redirections
gravelbox --stdout test.txt echo hello world
gravelbox --stdout test.txt --append-stdout cat
gravelbox --stdin test.txt cat

# specify alternative config file location
gravelbox --config path_to_config.json echo hello world
```
