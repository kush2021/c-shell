// Copyright (c) 2026 Kush Padalia. All Rights Reserved.

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../../include/builtins.h"
#include "../../include/context.h"
#include "../../include/utils.h"

int builtin_pwd(int argc, char **argv, struct shell_ctx *ctx) {
  (void) ctx;

  if (argc > 1) {
    fprintf(stderr, "csh: %s: too many arguments\n", argv[0]);
    return CSH_ERROR_MISUSE;
  }

  char cwd[PATH_MAX];
  if (!getcwd(cwd, sizeof(cwd))) {
    fprintf(stderr, "csh: %s: %s\n", argv[0], strerror(errno));
    return CSH_ERROR;
  }

  printf("%s\n", cwd);
  return CSH_SUCCESS;
}
