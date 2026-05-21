// Copyright (c) 2026 Kush Padalia. All Rights Reserved.

#include "../include/builtins.h"

const struct builtin_entry builtins[] = {
  { "exit",  builtin_exit },
  { "cd",    builtin_cd   },
  { "pwd",   builtin_pwd  },
  { nullptr, nullptr      }
};
