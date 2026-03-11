#!/bin/sh
# Tests Against the REPL

BINARY="./build/csh"

if [ ! -x "$BINARY" ]; then
  echo "FAIL $0: binary not found or not executable: $BINARY"
  exit 1
fi

ACTUAL_STDOUT=$(printf "" | "$BINARY" 2>/dev/null)

# Assertion 1: No Prompt on STDOUT
if [ -n "$ACTUAL_STDOUT" ]; then
  echo "FAIL $0: expected no stdout in non-interactive mode, got: '$ACTUAL_STDOUT'"
  exit 1
fi

# Assertion 2: Exit Status is 0
printf "" | "$BINARY" 2>/dev/null
ACTUAL_STATUS=$?

if [ "$ACTUAL_STATUS" -ne 0 ]; then
  echo "FAIL $0: expected exit status 0 on EOF, got: $ACTUAL_STATUS"
  exit 1
fi

echo "PASS $0"
exit 0
