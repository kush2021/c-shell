// Copyright (c) 2026 Kush Padalia. All Rights Reserved.

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 4096
#define PROMPT "csh> "

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
