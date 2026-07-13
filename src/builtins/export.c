// Copyright (c) 2026 Kush Padalia. All Rights Reserved.

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/builtins.h"
#include "../../include/context.h"
#include "../../include/utils.h"

// TODO: Support local environment variables and export NAME.
// Do as part of variable expansion.
int builtin_export(int argc, char **argv, struct shell_ctx *ctx) {
  (void) ctx;

  if (argc < 2) {
    fprintf(stderr, "csh: %s: too few arguments\n", argv[0]);
    return CSH_ERROR_MISUSE;
  }

  if (argc > 2) {
    fprintf(stderr, "csh: %s: too many arguments\n", argv[0]);
    return CSH_ERROR_MISUSE;
  }

  char *sep = strchr(argv[1], '=');
  if (!sep) {
    fprintf(stderr,
            "csh: %s: argument must be of the form NAME=VALUE\n",
            argv[0]);
    return CSH_ERROR_MISUSE;
  }

  *sep = '\0';
  if (setenv(argv[1], sep + 1, true) != 0) {
    *sep = '=';
    fprintf(stderr, "csh: %s: %s\n", argv[0], strerror(errno));
    return CSH_ERROR;
  }
  *sep = '=';

  return CSH_SUCCESS;
}
