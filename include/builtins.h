// Copyright (c) 2026 Kush Padalia. All Rights Reserved.

#ifndef BUILTINS_H
#define BUILTINS_H

// -- Types --------------------------------------------------------------------

/**
 * @brief Represents a generic built-in command.
 *
 * Since built-in commands all have the same function signature, a single type
 * can be used to represent an arbitrary command. This will be used within a
 * dispatch table to streamline built-in commands and future scalability.
 */
typedef int (*builtin_fn)(int argc, char **argv);

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

#endif  // BUILTINS_H
