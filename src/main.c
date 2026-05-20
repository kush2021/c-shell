// Copyright (c) 2026 Kush Padalia. All Rights Reserved.

// -- Libraries and Headers ----------------------------------------------------
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/context.h"
#include "../include/executor.h"
#include "../include/parser.h"

// -- Constants ----------------------------------------------------------------
#define PROMPT "csh> "

// -- Functions ----------------------------------------------------------------

/**
 * @brief Entrypoint for the C Shell.
 *
 * @return {@code EXIT_SUCCESS} on successful termination, {@code EXIT_FAILURE}
 * otherwise.
 */
int main(void) {
  char input[MAX_LINE_LENGTH];

  struct shell_ctx *ctx = malloc(sizeof(*ctx));
  ctx->last_status = 0;

  while (true) {
    /* Prompt only when stdin is a terminal (§1.1). */
    if (isatty(STDIN_FILENO)) {
      printf("%s", PROMPT);
      fflush(stdout);
    }

    if (fgets(input, MAX_LINE_LENGTH, stdin) == NULL) {
      if (ferror(stdin)) {
        perror("csh: read error");
        return EXIT_FAILURE;
      }
      /* EOF (Ctrl-D) — exit gracefully. */
      break;
    }

    /* Strip the trailing newline fgets leaves in the buffer. */
    input[strcspn(input, "\n")] = '\0';

    /* Parse the input line into a pipeline. */
    struct pipeline pipeline;
    const char *errmsg = nullptr;

    if (parse_line(input, &pipeline, &errmsg) == -1) {
      fprintf(stderr, "csh: %s\n", errmsg);
      continue;
    }

    /* Blank lines and comments produce an empty pipeline — skip silently. */
    if (pipeline.count == 0) continue;

    ctx->last_status = execute(&pipeline, ctx);

    pipeline_free(&pipeline);
  }

  const int status = ctx->last_status;
  free(ctx);

  return status;
}
