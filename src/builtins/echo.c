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
  if (argc > 1 && strcmp(argv[1], "-n") == 0) {
    nl = false;
    i = 2;
  }

  for (int arg = i; arg < argc; ++arg) {
    if (arg > i) fputc(' ', stdout);
    fputs(argv[arg], stdout);
  }

  if (nl) fputc('\n', stdout);

  return CSH_SUCCESS;
}
