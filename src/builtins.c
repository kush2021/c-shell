// Copyright (c) 2026 Kush Padalia. All Rights Reserved.

#include "../include/builtins.h"

// clang-format off
const struct builtin_entry builtins[] = {
  {
    "exit",
    builtin_exit,
    "exit [status] - Exit the shell.\n"
    "    If STATUS is given, it must be a non-negative integer and becomes\n"
    "    the shell's exit code. If omitted, the exit code of the last\n"
    "    executed command ($?) is used."
  },
  {
    "cd",
    builtin_cd,
    "cd [dir] - Change the working directory.\n"
    "    Changes to DIR, or to $HOME if DIR is omitted.\n"
    "    Prints an error and leaves the directory unchanged if DIR does not exist."
  },
  {
    "pwd",
    builtin_pwd,
    "pwd - Print the current working directory.\n"
    "    Prints the absolute path of the working directory, followed by a newline."
  },
  {
    "echo",
    builtin_echo,
    "echo [-n] [args...] - Print arguments to stdout.\n"
    "    Arguments are printed separated by single spaces.\n"
    "    A trailing newline is printed unless -n is given."
  },
  {
    "export",
    builtin_export,
    "export NAME=VALUE - Set an environment variable for child processes.\n"
    "    export NAME       - Export a variable without changing its value.\n"
    "    The variable becomes visible to all subsequently executed commands."
  },
  {
    "unset",
    builtin_unset,
    "unset NAME - Remove NAME from the environment.\n"
    "    Has no effect if NAME is not currently set."
  },
  {
    "help",
    builtin_help,
    "help [name...] - Display information about built-in commands.\n"
    "    With no arguments. lists all built-ins. With one or more NAMEs,\n"
    "    shows detailed help for just those commands."
  },
  { nullptr, nullptr, nullptr }
};

const size_t builtins_count = (sizeof(builtins) / sizeof(*builtins)) - 1;
