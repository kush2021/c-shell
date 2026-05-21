#!/bin/sh
# Tests Against the pwd Built-in

BINARY="./build/csh"

if [ ! -x "$BINARY" ]; then
  echo "FAIL $0: binary not found or not executable: $BINARY"
  exit 1
fi

# -- Setup --------------------------------------------------------------------

TESTDIR=$(mktemp -d /tmp/csh_test_pwd.XXXXXX)
TESTDIR=$(cd "$TESTDIR" && pwd -P)
SUBDIR="$TESTDIR/subdir"
NESTEDDIR="$TESTDIR/subdir/nested"

mkdir -p "$NESTEDDIR"

# -- Helpers ------------------------------------------------------------------

fail() {
  echo "FAIL $0: $1"
  rm -rf "$TESTDIR"
  exit 1
}

run() {
  OUT=$(printf "%s\n" "$1" | "$BINARY" 2>/tmp/csh_test_stderr)
  STATUS=$?
  ERR=$(cat /tmp/csh_test_stderr)
}

run_lines() {
  OUT=$(printf "%s\n" "$1" | "$BINARY" 2>/tmp/csh_test_stderr)
  STATUS=$?
  ERR=$(cat /tmp/csh_test_stderr)
}

# -- Assertion 1: pwd prints the current directory ----------------------------

run_lines "cd $TESTDIR
pwd"
[ "$OUT" = "$TESTDIR" ] || fail "assertion 1: expected '$TESTDIR', got: '$OUT'"

# -- Assertion 2: pwd reflects a cd to a subdirectory ------------------------

run_lines "cd $SUBDIR
pwd"
[ "$OUT" = "$SUBDIR" ] || fail "assertion 2: expected '$SUBDIR' after cd, got: '$OUT'"

# -- Assertion 3: pwd reflects a cd into a nested directory ------------------

run_lines "cd $NESTEDDIR
pwd"
[ "$OUT" = "$NESTEDDIR" ] || fail "assertion 3: expected '$NESTEDDIR' after cd, got: '$OUT'"

# -- Assertion 4: pwd at the filesystem root ----------------------------------

run_lines "cd /
pwd"
[ "$OUT" = "/" ] || fail "assertion 4: expected '/' after cd /, got: '$OUT'"

# -- Assertion 5: pwd output matches the system's own pwd -P -----------------
# Both should resolve symlinks the same way.

SYSTEM_PWD=$(cd "$TESTDIR" && pwd -P)
run_lines "cd $TESTDIR
pwd"
[ "$OUT" = "$SYSTEM_PWD" ] || fail "assertion 5: expected '$SYSTEM_PWD' (system pwd -P), got: '$OUT'"

# -- Assertion 6: pwd produces no output on stderr ----------------------------

run_lines "cd $TESTDIR
pwd"
[ -z "$ERR" ] || fail "assertion 6: expected no stderr output from pwd, got: '$ERR'"

# -- Assertion 7: pwd output is a single line ---------------------------------
# If there were extra lines, wc would return > 1.

run_lines "cd $TESTDIR
pwd"
LINE_COUNT=$(echo "$OUT" | wc -l | tr -d ' ')
[ "$LINE_COUNT" -eq 1 ] || fail "assertion 7: expected exactly one line of output, got: $LINE_COUNT"

# -- Assertion 8: too many arguments is rejected ------------------------------

run "pwd /tmp"
[ "$STATUS" -eq 2 ] || fail "assertion 8: expected status 2 for too many arguments, got: $STATUS"
[ -n "$ERR" ]       || fail "assertion 8: expected an error on stderr for too many arguments, got nothing"

# -- Assertion 9: error from too many arguments goes to stderr not stdout -----

run "pwd /tmp"
[ -z "$OUT" ] || fail "assertion 9: expected no stdout on argument error, got: '$OUT'"

# -- Assertion 10: shell continues after a rejected pwd -----------------------

OUT=$(printf "pwd /tmp\necho still_running\n" | "$BINARY" 2>/dev/null)
[ "$OUT" = "still_running" ] || fail "assertion 10: expected shell to continue after bad pwd, got: '$OUT'"

# -- Cleanup ------------------------------------------------------------------

rm -rf "$TESTDIR"
rm -f /tmp/csh_test_stderr

echo "PASS $0"
exit 0
