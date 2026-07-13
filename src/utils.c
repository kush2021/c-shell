// Copyright (c) 2026 Kush Padalia. All Rights Reserved.

// -- Libraries and Headers ----------------------------------------------------
#include "../include/utils.h"

#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

bool is_integer(const char *s, int64_t *out) {
  if (!s || !*s) return false;

  char *end;
  errno = 0;
  int64_t val = strtol(s, &end, 10);

  if (errno == ERANGE) return false;
  if (end == s) return false;
  if (*end != '\0') return false;

  *out = val;
  return true;
}

int get_cwd(char *buf, size_t size) {
  char cwd[PATH_MAX];
  if (!getcwd(cwd, sizeof(cwd))) {
    fprintf(stderr, "fatal: csh: could not get current working directory\n");
    exit(EXIT_FAILURE);
  }

  const char *name;
  if (cwd[0] == '/' && cwd[1] == '\0') name = "/";
  else {
    char *last_slash = strrchr(cwd, '/');
    name = last_slash ? last_slash + 1 : cwd;
  }

  if (snprintf(buf, size, "%s", name) >= (int) size) return -1;
  return 0;
}
