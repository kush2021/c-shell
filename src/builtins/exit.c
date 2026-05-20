// Copyright (c) 2026 Kush Padalia. All Rights Reserved.

#include <stdio.h>
#include <stdlib.h>

#include "../../include/builtins.h"
#include "../../include/context.h"
#include "../../include/utils.h"

int builtin_exit(int argc, char **argv, struct shell_ctx *ctx) {
  if (argc > 2) {
    fprintf(stderr, "csh: %s: too many arguments\n", argv[0]);
    return CSH_ERROR_MISUSE;
  }

  if (argc == 1) {
    exit(ctx->last_status);
  }

  int64_t status = 0;
  if (!is_integer(argv[1], &status)) {
    fprintf(stderr,
            "csh: %s: %s: status must be an integer\n",
            argv[0],
            argv[1]);
    return CSH_ERROR_MISUSE;
  }

  if (status < 0) {
    fprintf(stderr,
            "csh: %s: %s: status must be non-negative\n",
            argv[0],
            argv[1]);
    return CSH_ERROR_MISUSE;
  }

  exit((int) status);
}
