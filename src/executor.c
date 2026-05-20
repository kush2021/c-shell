// Copyright (c) 2026 Kush Padalia. All Rights Reserved.

#include "../include/executor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../include/parser.h"

static void execute_command(struct command *c) {
  if (!c->argc) return;

  const pid_t pid = fork();

  if (pid < 0) {
    perror("fork");
    return;
  }

  if (pid > 0) {
    waitpid(pid, nullptr, 0);
    return;
  }

  execvp(c->argv[0], c->argv);
  perror("execvp");
  _exit(EXIT_FAILURE);
}

void execute(struct pipeline *p) {
  for (int i = 0; i < p->count; ++i) execute_command(&p->commands[i]);
}
