# Overview

To be documented...

# Notes

Currently, only Unix-style newlines are supported ("\n" only, rather than "\r"
or "\r\n".)

The following commands may be helpful in debugging and in development:

- ```clear && tmux clear-history && make DEBUG=1 && gdb -ex 'break SemanticsError' -ex 'break __assert_fail' -ex run -ex q --args _build/dist/usr/bin/cpsl-cc -i ~/git/usu/cs-5300-compilers/CS5300/TestFiles/array_index.cpsl --output=-```
- ```make DEBUG=1 EXTRA_CXXFLAGS=-fdiagnostics-color=always 2>&1 L```
