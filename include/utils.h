// Copyright (c) 2026 Kush Padalia. All Rights Reserved.

#ifndef UTILS_H
#define UTILS_H

// -- Libraries and Headers ----------------------------------------------------
#include <stddef.h>
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

/**
 * @brief Get the current working directory.
 *
 * This only returns the topmost directory name without the full path.
 *
 * @param buf The output buffer.
 * @param The output size.
 *
 * @return The success status.
 */
int get_cwd(char *buf, size_t size);

#endif /* UTILS_H */
