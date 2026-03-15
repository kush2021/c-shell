// Copyright (c) 2026 Kush Padalia. All Rights Reserved.

#ifndef PARSER_H
#define PARSER_H

// -- Libraries and Headers ----------------------------------------------------
#include <stdbool.h>

// -- Constants ----------------------------------------------------------------
#define MAX_LINE_LENGTH 4096
#define MAX_ARGS 128

// -- Types --------------------------------------------------------------------

/**
 * @brief Represents a single C Shell command.
 *
 * A command is a single executable with its arguments and any I/O redirection
 * applied to it.
 *
 * Example: `ls -la > output.txt` is a single command with `argv = ["ls",
 * "-la"]` and `output_file = "output.txt"`.
 *
 * @note The maximum supported arguments is `MAX_ARGS`.
 */
struct command {
  int argc;    /**< Number of arguments. */
  char **argv; /**< Argument list. The length of this list is exactly `argc`.
                  Each element is a copy of the source command string. */
};

/**
 * @brief Represents a pipeline of commands.
 *
 * A pipeline contains multiple commands joined by the pipe operator.
 *
 * Example: `cat main.c | pbcopy` is a pipeline with two separate commands.
 */
struct pipeline {
  int count;                /**< The number of commands. */
  struct command *commands; /**< The list of commands. */
};

// -- Function Declarations ----------------------------------------------------

/**
 * @brief Parses a line of input into a pipeline.
 *
 * The caller is responsible for freeing the pipeline. See @ref pipeline_free.
 *
 * @param line The input line.
 * @param out The output `struct pipeline`.
 *
 * @return `0` on success, `-1` on syntax error.
 */
int parse_line(const char *line, struct pipeline *out);

/**
 * @brief Free all memory owned by a pipeline.
 *
 * @param p The `struct pipeline *` to free.
 */
void pipeline_free(struct pipeline *p);

#endif // !PARSER_H
