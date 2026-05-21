// Copyright (c) 2026 Kush Padalia. All Rights Reserved.

#include "../include/executor.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../include/builtins.h"
#include "../include/context.h"
#include "../include/parser.h"
#include "../include/utils.h"

static int execute_command(struct command *c, struct shell_ctx *ctx) {
  if (!c->argc) return -1;

  for (int i = 0; builtins[i].name != nullptr; ++i) {
    if (strcmp(c->argv[0], builtins[i].name) == 0) {
      return builtins[i].fn(c->argc, c->argv, ctx);
    }
  }

  int status = 0;
  const pid_t pid = fork();

  if (pid < 0) {
    fprintf(stderr, "csh: unexpected error: %s\n", strerror(errno));
    return -1;
  }

  if (pid > 0) {
    waitpid(pid, &status, 0);
    return WEXITSTATUS(status);
  }

  execvp(c->argv[0], c->argv);
  fprintf(stderr, "csh: unexpected error: %s\n", strerror(errno));
  _exit(127);
}

int execute(struct pipeline *p, struct shell_ctx *ctx) {
  if (!p->count) return CSH_SUCCESS;

  return execute_command(&p->commands[0], ctx);
}
