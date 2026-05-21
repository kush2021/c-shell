// Copyright (c) 2026 Kush Padalia. All Rights Reserved.

#ifndef BUILTINS_H
#define BUILTINS_H

// -- Libraries and Headers ----------------------------------------------------
#include "context.h"

// -- Types --------------------------------------------------------------------

/**
 * @brief Represents a generic built-in command.
 *
 * Since built-in commands all have the same function signature, a single type
 * can be used to represent an arbitrary command. This will be used within a
 * dispatch table to streamline built-in commands and future scalability.
 */
typedef int (*builtin_fn)(int argc, char **argv, struct shell_ctx *ctx);

/**
 * @brief Represents a single entry in the dispatch table.
 */
struct builtin_entry {
  const char *name; /**< The name of the built-in command. */
  builtin_fn fn;    /**< The corresponding built-in function. */
};

/**
 * @brief The dispatch table. Guaranteed to be null-terminated.
 */
extern const struct builtin_entry builtins[];

// -- Function Declarations ----------------------------------------------------

/**
 * @brief Exit the shell.
 *
 * Usage: `exit [status]`
 *
 * Terminates the current shell. If `status` is not provided, exits with the
 * status of the last command. `status` must be a non-negative integer.
 *
 * @param argc The number of arguments. At most two.
 * @param argv The null-terminated argument array.
 * @param ctx The global shell context.
 *
 * @return The command status.
 */
int builtin_exit(int argc, char **argv, struct shell_ctx *ctx);

/**
 * @brief Change the current directory.
 *
 * Usage: `cd [dir]`
 *
 * Changes the current directory to the path provided by `dir`. If `dir` is
 * omitted, it changes to `$HOME`. If `dir` is an invalid path, a corresponding
 * error will print and no change will occur.
 *
 * @param argc The number of arguments. At most two.
 * @param argv The null-terminated argument array.
 * @param ctx the global shell context.
 *
 * @return The command status.
 */
int builtin_cd(int argc, char **argv, struct shell_ctx *ctx);

/**
 * @brief Print the absolute working directory.
 *
 * Usage: `pwd`
 *
 * @param argc The number of arguments. Should be one.
 * @param argv The null-terminated argument array.
 * @param ctx The global shell context.
 *
 * @return The command status.
 */
int builtin_pwd(int argc, char **argv, struct shell_ctx *ctx);

#endif  // BUILTINS_H
