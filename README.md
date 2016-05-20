# Getopt

A quality getopt implementation without strong copyleft concern, licensed under [Boost 1.0](http://www.boost.org/LICENSE_1_0.txt).

## Motivation

Most veteran C/C++ programmers are familiar with [GNU's argument conventions](http://www.gnu.org/software/libc/manual/html_node/Argument-Syntax.html), and frequently interact with programs that implement it as a part their CLI frontend. C++ libraries implementing those conventions are available, but I've always felt them to be lacking after using D's [`std.getopt` module](https://dlang.org/phobos/std_getopt.html) and various ports of [docopt](https://github.com/docopt/docopt.cpp). The original GNU [`getopt`](http://www.gnu.org/software/libc/manual/html_node/Getopt.html#Getopt) library is available, but was built for C, which introduces a strange boilerplate and a need to memorize an infrequently-used (and therefore error-prone) DSL. Can't C++ do better?

The answer is: of course! Templates can be used to automatically generate parsing logic for the user, and let them just focus on what the arguments are -- that's how D does it. This repository serves as the development location for [`getopt.h`](https://github.com/ErichDonGubler/getopt/blob/master/include/getopt.h), which provides a nice API for getopt-style CLI args parsing. It is a modern C++ implementation based upon D's [`std.getopt`](http://dlang.org/phobos/std_getopt.html) written by @andralex (Andrei Alexandrescu), which is in turn based upon Perl's [Getopt::Long](http://perldoc.perl.org/Getopt/Long.html).

## Features

* Short options (`-a`, `-r`)
	* Bundling (`-ar`) (TODO)
* Long options (`--verbosity`, `--help`)
* Special handling for boolean and incremental (integral type) options (i.e., count number of `--quiet`)
* Builtin help and help printing
* Uses stream operators for parsing, and so can be extended to parse user-defined types without any glue code

## Usage and Examples 

See the [example](https://github.com/ErichDonGubler/getopt/tree/master/example) folder for sample usage and a description of individual features.

## Including in your Project

The file of interest for most users will be [`getopt.h`](https://github.com/ErichDonGubler/getopt/blob/master/include/getopt.h), which you can simply include and use in your project. If you prefer stability, download one of the formal releases from the [Github repo](https://github.com/ErichDonGubler/getopt/).

This project has been tested with C++11.
