// Copyright (c) 2026 Kush Padalia. All Rights Reserved.

#include <stdio.h>
#include <string.h>

#include "../../include/builtins.h"
#include "../../include/context.h"
#include "../../include/utils.h"

int builtin_echo(int argc, char **argv, struct shell_ctx *ctx) {
  (void) ctx;
  (void) argv;

  if (argc == 1) {
    fprintf(stdout, "\n");
    return CSH_SUCCESS;
  }

  bool nl = true;
  int i = 1;
  if (strcmp(argv[1], "-n") == 0) {
    nl = false;
    i = 2;
  }

  while (argv[i + 1]) {
    fprintf(stdout, "%s ", argv[i]);
    ++i;
  }
  fprintf(stdout, "%s", argv[i]);

  if (nl) putchar('\n');

  return CSH_SUCCESS;
}
