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
 * @brief Represents a single csh command with its arguments and I/O
 * redirections.
 *
 * A command is a single executable with its arguments and any I/O redirection
 * applied to it.
 *
 * Example: `ls -la > output.txt` is a single command with
 * `argv = ["ls", "-la", NULL]` and `redir_out = "output.txt"`.
 *
 * @note argv is NULL-terminated: argv[argc] == NULL. This satisfies the
 * contract of execve(2) without any further adjustment by the caller.
 *
 * @note The maximum supported argument count is MAX_ARGS.
 */
struct command {
  int argc;    /**< Number of arguments (not counting the NULL sentinel). */
  char **argv; /**< NULL-terminated argument vector (argv[argc] == NULL).
                    Each element is a heap-allocated copy. */

  /* I/O redirections — NULL when not specified for this command. */
  char *redir_in;         /**< Input redirection:          < file           */
  char *redir_out;        /**< Output redirection:         > file (also &>) */
  char *redir_append;     /**< Output append:              >> file          */
  char *redir_err;        /**< Stderr redirection:         2> file          */
  char *redir_err_append; /**< Stderr append:              2>> file         */
  bool redir_both;        /**< true when &> was used: both stdout and
                               stderr are redirected to redir_out.          */
};

/**
 * @brief Represents a pipeline of one or more commands joined by '|'.
 *
 * Example: `cat main.c | wc -l` is a pipeline with two commands.
 *
 * An empty pipeline (count == 0, commands == NULL) is returned for blank
 * lines and comment lines. This is not an error.
 */
struct pipeline {
  int count;                /**< Number of commands (0 for empty pipeline). */
  struct command *commands; /**< Array of commands, length == count.        */
  bool background;          /**< true when the pipeline ends with '&'.      */
};

// -- Function Declarations ----------------------------------------------------

/**
 * @brief Parse a line of shell input into a pipeline.
 *
 * Handles tokenisation, quote processing, escape sequences, I/O redirection
 * operators, pipes, and background '&'. Variable expansion is *not* performed
 * here — that is a separate pass (see §1.5 of the spec).
 *
 * On success the caller owns the pipeline and must free it with
 * pipeline_free(). On failure *out is left clean — no further cleanup is
 * needed by the caller.
 *
 * An empty or comment-only line is not an error; it returns 0 with
 * out->count == 0 and out->commands == NULL.
 *
 * Error messages are returned without a "csh: " prefix so that the REPL can
 * print them as:
 *   fprintf(stderr, "csh: %s\n", errmsg);
 *
 * @param line    NUL-terminated input line (not modified).
 * @param out     Output pipeline struct (must point to valid storage).
 * @param errmsg  On failure, set to a static human-readable error string.
 * @return 0 on success, -1 on syntax or allocation error.
 */
int parse_line(const char *line, struct pipeline *out, const char **errmsg);

/**
 * @brief Free all heap memory owned by a pipeline.
 *
 * Safe to call on a zeroed or partially-constructed pipeline. Zeroes *p
 * after freeing so that accidental double-frees are harmless.
 * Does not free the pipeline struct itself (which is typically stack-allocated
 * in the REPL).
 *
 * @param p Pipeline to clean up. May be NULL.
 */
void pipeline_free(struct pipeline *p);

#endif /* PARSER_H */
