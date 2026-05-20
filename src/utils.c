// Copyright (c) 2026 Kush Padalia. All Rights Reserved.

// -- Libraries and Headers ----------------------------------------------------
#include "../include/utils.h"

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

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
