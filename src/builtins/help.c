// Copyright (c) 2026 Kush Padalia. All Rights Reserved.

#include <stdio.h>
#include <string.h>

#include "../../include/builtins.h"
#include "../../include/context.h"
#include "../../include/utils.h"

int builtin_help(int argc, char **argv, struct shell_ctx *ctx) {
  (void) ctx;

  for (size_t i = 0; i < builtins_count; ++i) {
    if (argc > 1) {
      for (int arg = 1; arg < argc; ++arg) {
        if (strcmp(argv[arg], builtins[i].name) == 0) {
          fprintf(stdout, "%s\n", builtins[i].help);
        }
      }
    } else {
      fprintf(stdout, "%s\n", builtins[i].help);
    }
  }

  return CSH_SUCCESS;
}
