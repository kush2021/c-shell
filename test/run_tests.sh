#!/bin/sh

# Runs the Test Suite
#
# Discovers and runs all test scripts (*.sh) under the tests/ directory.
# Prints a final summary of results.

TESTS_DIR="$(cd "$(dirname "$0")" && pwd)"
RUNNER="$(basename "$0")"

passed=0
failed=0

while IFS= read -r test_file; do
  case "$test_file" in
    *"$RUNNER") continue ;;
  esac

  if [ ! -x "$test_file" ]; then
    printf "SKIP %s (not executable - run chmod +x %s)\n" \
      "$test_file" "$test_file"
    continue
  fi

  "$test_file"
  status=$?

  if [ "$status" -eq 0 ]; then
    passed=$((passed + 1))
  else
    failed=$((failed + 1))
  fi

done << EOF
$(find "$TESTS_DIR" -name "*.sh" | sort)
EOF

total=$((passed + failed))
printf "\n%d/%d tests passed" "$passed" "$total"

if [ "$failed" -gt 0 ]; then
  printf "(%d failed)\n" "$failed"
  exit 1
fi

printf "\n"
exit 0
