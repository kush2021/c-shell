// Copyright (c) 2026 Kush Padalia. All Rights Reserved.

#ifndef EXECUTOR_H
#define EXECUTOR_H

// -- Libraries and Headers ----------------------------------------------------
#include "parser.h"

// -- Function Declarations ----------------------------------------------------

/**
 * @brief Execute a pipeline.
 *
 * Forks one child per command, wires pipes and I/O redirections, and waits for
 * all children to exit (unless the pipeline is backgrounded). Built-in commands
 * are executed without forking.
 *
 * @param p The pipeline to execute.
 */
void execute(struct pipeline *p);

#endif  // EXECUTOR_H
