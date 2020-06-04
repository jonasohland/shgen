## shgen

- [Installing](#installing)
- [Usage](Usage)
  - [Output modes]()
  - [Formatting]()
  - [Value-type options]()
  - [SSE intrinsics]()
  - [AVX1/2/512 support]()
  - [Eval functions]()
    - [C-mode]()
    - [C++-mode]()



#### Installing

Currently, the only way to install **shgen** is to build it from source. If you are a macOS and [Homebrew](https://brew.sh) user, you can let homebrew build it for you, by using my tap:

```bash
$ brew tap jonasohland/things
$ brew install shgen
```

To build the tool you will need a working C++11 build environment and [CMake](https://cmake.org). Please clone the repository using git and don't use GitHubs _Dowload ZIP_ function: 

```bash
$ git clone --recurse-submodules https://github.com/jonasohland/shgen
```

Generate build files for your platform by running

```bash
$ cmake $PATH_TO_SHGEN_SOURCE -DCMAKE_BUILD_TYPE=Release
```

in a build directory of your choosing. Avoid running this command directly at the root of the shgen source tree.

You can then build and install shgen:

```bash
$ cmake --build . --target install
```



#### Usage

The basic usage is as follows:

```shell
$ shgen [options] [header_output_file] [source_output_file]
```

where options are:

```
-?, -h, --help                     display usage information
-v, --version                      Print version and exit
-H, --header-only                  Output only a header file
-s, --sse                          Use SSE instructions
-C, --condon-shortley              Include condon shortley phase thingy
-L, --max-L <max-L>                Maximum value for L
-l, --min-L <min-L>                Minimum value for L
-d, --double                       Double precision
-f, --float                        Single precision
-t, --template                     Templated version
-c, --c-mode                       Emit plain and simple c, without all the template stuff
-7, --cxx-17                       Enable C++17 specific stuff
-e, --line-ending <line-ending>    Line endings (LF/CRLF)
```

