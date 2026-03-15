// Copyright (c) 2026 Kush Padalia. All Rights Reserved.

// -- Libraries and Headers ----------------------------------------------------
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

  while (true) {
    if (isatty(STDIN_FILENO)) {
      printf("%s", PROMPT);
      fflush(stdout);
    }

    if (fgets(input, MAX_LINE_LENGTH, stdin) == NULL) {
      if (ferror(stdin)) {
        perror("csh: read error");
        return EXIT_FAILURE;
      }

      break;
    }
  }

  return EXIT_SUCCESS;
}
