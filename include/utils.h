// Copyright (c) 2026 Kush Padalia. All Rights Reserved.

// -- Libraries and Headers ----------------------------------------------------
#include <stdint.h>

// -- Constants ----------------------------------------------------------------
#define CSH_SUCCESS 0
#define CSH_ERROR 1
#define CSH_ERROR_MISUSE 2

// -- Function Declarations ----------------------------------------------------

/**
 * @brief Check if a string value represents an integer.
 *
 * @param s The string value.
 * @param out A memory location where the converted value will be stored.
 *
 * @return Whether the string represents an integer.
 */
bool is_integer(const char *s, int64_t *out);
