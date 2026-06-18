// Copyright (c) 2026 Kush Padalia. All Rights Reserved.

#include "../include/builtins.h"

const struct builtin_entry builtins[] = {
  { "exit",   builtin_exit   },
  { "cd",     builtin_cd     },
  { "pwd",    builtin_pwd    },
  { "echo",   builtin_echo   },
  { "export", builtin_export },
  { "unset",  builtin_unset  },
  { nullptr,  nullptr        }
};
