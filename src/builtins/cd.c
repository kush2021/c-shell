// Copyright (c) 2026 Kush Padalia. All Rights Reserved.

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../include/builtins.h"
#include "../../include/context.h"
#include "../../include/utils.h"

int builtin_cd(int argc, char **argv, struct shell_ctx *ctx) {
  (void) ctx;

  if (argc > 2) {
    fprintf(stderr, "csh: %s: too many arguments\n", argv[0]);
    return CSH_ERROR_MISUSE;
  }

  if (argc == 1) {
    const char *home = getenv("HOME");

    if (!home) {
      fprintf(stderr, "csh: %s: HOME not set\n", argv[0]);
      return CSH_ERROR;
    }

    if (chdir(home) == -1) {
      fprintf(stderr, "csh: %s: %s\n", argv[0], strerror(errno));
      return CSH_ERROR;
    }
    return CSH_SUCCESS;
  }

  if (chdir(argv[1]) == -1) {
    fprintf(stderr, "csh: %s: %s\n", argv[0], strerror(errno));
    return CSH_ERROR;
  }
  return CSH_SUCCESS;
}
