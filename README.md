[![Stories in Ready](https://badge.waffle.io/ErichDonGubler/getopt.png?label=ready&title=Ready)](https://waffle.io/ErichDonGubler/getopt)
# Getopt

A quality getopt implementation without strong copyleft concern, licensed under [Boost 1.0](http://www.boost.org/LICENSE_1_0.txt).

## Motivation

Most veteran C/C++ programmers are familiar with [GNU's argument conventions](http://www.gnu.org/software/libc/manual/html_node/Argument-Syntax.html), and frequently interact with programs that implement it as a part of CLI frontend design. Libraries implementing GNU's argument conventions are available -- however, the [`getopt`](http://www.gnu.org/software/libc/manual/html_node/Getopt.html#Getopt) was built for C, which introduces an unneccesarily complex interface for C++. Templates can be used to immensely simplify the interface, and automatically generate the boilerplate logic necessary to use the in the C version.

This repository serves as the development location for [`getopt.h`](https://github.com/ErichDonGubler/getopt/blob/master/include/getopt.h). It is a modern C++ implementation based upon D's [`std.getopt`](http://dlang.org/phobos/std_getopt.html) written by @andralex (Andrei Alexandrescu), which is in turn based upon Perl's [Getopt::Long](http://perldoc.perl.org/Getopt/Long.html).

## Features

* Short options (`-a`, `-r`)
	* Bundling (`-ar`)
* Long options (`--verbosity`, `--help`)
* Special handling for boolean and incremental (integral type) options (i.e., count number of `--quiet`)
* Parsing of options determined at compile time dependent upon the variables you provide for each option
* Builtin help and help printing
* Uses stream operators for parsing, and thus can be extended to parse user-defined types

## Including in your Project

The file of interest for most users will be [`getopt.h`](https://github.com/ErichDonGubler/getopt/blob/master/include/getopt.h), which you can simply include and use in your project. If you prefer stability, download one of the formal releases from the [Github repo](https://github.com/ErichDonGubler/getopt/).

This project has been tested with C++11.

## Usage and Examples 

See the [example](https://github.com/ErichDonGubler/getopt/tree/master/example) folder for sample usage and a description of individual features.
