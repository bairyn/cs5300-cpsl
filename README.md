# Overview

```
Usage: cpsl-cc [OPTION]... -o FILE [-i] FILE

Options:
  -H,
  -?, --help           print usage information and exit.
  -V, --version        print version information and exit.
  -v, --verbose        increase verbosity.
  -i, --input PATH     specify the path to the input file to process.
  -o, --output PATH    specify the path to the output file to write.
      --lexer,
      --scanner        write scanner information after each line and stop after the lexer stage.
      --parser,
      --grammar        indicate if parsing the grammar of the input succeeded and stop after the parsing stage.
      --parser-trace,
      --grammar-trace  print bison tracing information while parsing.
```

Example:

```
cpsl-cc -i checkers.cpsl -o /tmp/mips.asm && java -jar ~/src/mars/Mars4_4.jar /tmp/mips.asm
```

# Notes

Currently, only Unix-style newlines are supported ("\n" only, rather than "\r"
or "\r\n".)

The following commands may be helpful in debugging and in development:

- ```clear && tmux clear-history && make DEBUG=1 && gdb -ex 'break SemanticsError' -ex 'break __assert_fail' -ex 'break runtime_error' -ex run -ex q --args _build/dist/usr/bin/cpsl-cc -i ~/git/usu/cs-5300-compilers/CS5300/TestFiles/array_index.cpsl --output=-```
- ```make DEBUG=1 EXTRA_CXXFLAGS=-fdiagnostics-color=always 2>&1 | less```
