<div align="center">

# ZetaStack

[![license](https://img.shields.io/github/license/Papr3ka/ZetaStack?style=flat-square)](https://github.com/Papr3ka/ZetaStack/blob/main/LICENSE)
[![C++](https://img.shields.io/badge/-C++-00599C?style=flat-square&logo=c)](https://github.com/Papr3ka)
[![Repo Size](https://img.shields.io/github/repo-size/Papr3ka/ZetaStack?style=flat-square)](https://github.com/Papr3ka/ZetaStack)
[![Lines Of Code](https://img.shields.io/tokei/lines/github/Papr3ka/ZetaStack?style=flat-square)](https://github.com/Papr3ka/ZetaStack)

An advanced command-line calculator

</div>

# Prerequisites

- Requires C++11 or later preferably C++17 or later

- [Make](https://www.gnu.org/software/make/) utility

# Usage

```
Usage: ZetaStack [Options] ...

Options:
  -h  --help                   Display this information
  -v  --version                Display version information

  -j <int>                     Sets the maximum thread count

  --bare                       Start with absolute minimum memory usage
  --debug                      Start with debug mode on
  --nobuffer                   Disables variable buffer
  --noexec                     Start with execution disabled
  --nohandle                   Disables signal handling
  --maxrecurse <int>           Sets the maximum recursion depth
 ```

# Functions

Function overloading is supported

### Examples

> Creating

```
=== f(x,y)=x/y
=== fma(a,b,c)=a+b*c
```

> Using

```
=== f(fma(2,4+4,6*12),2/3-2)
=== f(1,7)
```

> Deleting

```
=== /del f(
=== /del fma(
```

# Variables

You can assign, manipulate and delete variables in ZetaStack. Assigning variables are the same as any other programming language.

### Examples

```
=== x = 123
=== var = 2*(1+4)
=== var += 4 // Variable must be defined first
=== /del var
```

> Special Variables

**ans** - the most recent answer

**rand** - a cryptographically secure random number between 0 - 999 inclusive

> Commenting
Commenting can be done with **//** or **/\*\*/**

### Examples

```
=== 64*(/*Comment here*/2+41/*will read as 4125*/25)
=== 256**2 //Everything after here will be gone
```

# Commands

Commands can be accessed and used via the '/' key.

> Available commands

```
   bar                      Toggles progress bar
   buffer <Options>         Commands related to buffer
   cache <Options>          Commands related to cache
   clock                    Displays a clock
   debug                    Toggles debug mode
   del <var>                Deletes a variable
   exec                     Toggles execution
   exit                     Exits the program
   export <var>             Writes a variable to text
   time                     Times calculations
   globals                  Displays all defined functions and variables
   ; <Command>              Executes system command
```

# License

[GNU General Public License v3.0](https://www.gnu.org/licenses/gpl-3.0.en.html)
