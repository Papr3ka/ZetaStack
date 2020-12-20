# ZetaStack

<div align="center">

[![C++](https://img.shields.io/badge/-C++-00599C?style=flat-square&logo=c)](https://github.com/Papr3ka)
[![Files](https://tokei.rs/b1/github/Papr3ka/ZetaStack?category=files)](https://github.com/Papr3ka/ZetaStack)
[![Repo Size](https://img.shields.io/github/repo-size/Papr3ka/ZetaStack?style=flat-square)](https://github.com/Papr3ka/ZetaStack)
[![Lines Of Code](https://tokei.rs/b1/github/Papr3ka/ZetaStack?category=code)](https://github.com/Papr3ka/ZetaStack)

</div>

# Usage

```
Usage: ZetaStack [Options] ...

Options:
  --version                Display version information
  --help                   Display this information

  --debug                  Start with debug mode on
  --nobuffer               Disables variable buffer
  --noexec                 Start with execution disabled
  --nohandle               Disables signal handling
 ```

# Variables

You can assign, manipulate and delete variables in ZetaStack. Assigning variables are the same as any other programming language.

### Examples

```
>>> x = 123
>>> var = 2*(1+4)
>>> var += 4 // Variable must be defined first
>>> /del var
```

### Special Variables

**ans** - the most recent answer

**rand** - a cryptographically secure random number between 1 - 1000

# Commenting
Commenting can be done with **//** or **/\*\*/**

### Example

```
>>> 64*(/*Comment here*/2+41/*will read as 4125*/25)
>>> 256**2 //Everything after here will be gone
```

# Commands

Commands can be accessed and used via the '/' key.

### Available commands

```
   bar                      Toggles progress bar
   buffer <Options>         Commands related to buffer
   cache <Options>          Commands related to cache
   clock                    Displays a clock
   debug                    Toggles debug mode
   del <var>                Deletes a variable
   exit                     Exits the program
   time                     Times calculations
   ; <Command>              Executes system command
```
