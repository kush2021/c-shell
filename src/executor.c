// Copyright (c) 2026 Kush Padalia. All Rights Reserved.

#include "../include/executor.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/syslimits.h>
#include <sys/unistd.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../include/builtins.h"
#include "../include/context.h"
#include "../include/parser.h"
#include "../include/utils.h"

static void custom_execvp(const char *file, char *const *argv) {
  if (*file == '\0') {
    errno = ENOENT;
    return;
  }

  if (strchr(file, '/') != NULL) {
    execv(file, argv);
  } else {
    char *path = getenv("PATH");
    if (path == nullptr) {
      size_t len = confstr(_CS_PATH, nullptr, 0);
      if (!len) {
        errno = ENOENT;
        return;
      }

      char *buf = malloc(len);
      if (!buf) {
        errno = ENOMEM;
        return;
      }

      confstr(_CS_PATH, buf, len);
      path = buf;
    } else {
      path = strdup(path);
      if (!path) {
        errno = ENOMEM;
        return;
      }
    }

    char *p = path;
    while (p) {
      char *colon = strchr(p, ':');
      if (colon) *colon = '\0';

      char full[PATH_MAX];
      snprintf(full, sizeof(full), "%s/%s", p, file);
      execv(full, argv);

      if (!colon) break;
      p = colon + 1;
    }

    free(path);
  }
}

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

  custom_execvp(c->argv[0], c->argv);

  switch (errno) {
    case ENOENT:
      fprintf(stderr, "csh: command not found: %s\n", c->argv[0]);
      _exit(127);
    case EACCES:
      fprintf(stderr, "csh: permission denied: %s\n", c->argv[0]);
      _exit(126);
    default:
      fprintf(stderr, "csh: unexpected error: %s\n", strerror(errno));
      _exit(1);
  }
}

int execute(struct pipeline *p, struct shell_ctx *ctx) {
  if (!p->count) return CSH_SUCCESS;

  return execute_command(&p->commands[0], ctx);
}
