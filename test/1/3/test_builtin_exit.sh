#!/bin/sh
# Tests Against the exit Built-in

BINARY="./build/csh"

if [ ! -x "$BINARY" ]; then
  echo "FAIL $0: binary not found or not executable: $BINARY"
  exit 1
fi

# -- Helpers ------------------------------------------------------------------

fail() {
  echo "FAIL $0: $1"
  exit 1
}

run() {
  # run <input> — captures stdout into $OUT, stderr into $ERR, status into $STATUS
  OUT=$(printf "%s\n" "$1" | "$BINARY" 2>/tmp/csh_test_stderr)
  STATUS=$?
  ERR=$(cat /tmp/csh_test_stderr)
}

# -- Assertion 1: exit with no arguments exits 0 ------------------------------
# A bare exit should exit with status 0 when no prior command has run.

run "exit"
[ "$STATUS" -eq 0 ] || fail "assertion 1: expected exit status 0 for bare 'exit', got: $STATUS"

# -- Assertion 2: exit with explicit 0 exits 0 --------------------------------

run "exit 0"
[ "$STATUS" -eq 0 ] || fail "assertion 2: expected exit status 0 for 'exit 0', got: $STATUS"

# -- Assertion 3: exit with explicit non-zero status --------------------------

run "exit 42"
[ "$STATUS" -eq 42 ] || fail "assertion 3: expected exit status 42 for 'exit 42', got: $STATUS"

# -- Assertion 4: exit with max single-byte status ----------------------------

run "exit 255"
[ "$STATUS" -eq 255 ] || fail "assertion 4: expected exit status 255 for 'exit 255', got: $STATUS"

# -- Assertion 5: exit inherits last command status ---------------------------
# A bare exit after a failed command should exit with that command's status.

printf "exit_status_test_bad_command_xyz\nexit\n" | "$BINARY" 2>/dev/null
STATUS=$?
[ "$STATUS" -ne 0 ] || fail "assertion 5: expected non-zero exit after failed command + bare exit, got: $STATUS"

# -- Assertion 6: non-integer argument is rejected ----------------------------
# exit with a non-integer argument must print to stderr and not exit the shell.

run "exit abc"
[ "$STATUS" -eq 2 ]   || fail "assertion 6: shell should not exit on 'exit abc', got status: $STATUS"
[ -n "$ERR" ]         || fail "assertion 6: expected an error on stderr for 'exit abc', got nothing"

# -- Assertion 7: negative integer argument is rejected -----------------------

run "exit -1"
[ "$STATUS" -eq 2 ] || fail "assertion 7: shell should not exit on 'exit -1', got status: $STATUS"
[ -n "$ERR" ]       || fail "assertion 7: expected an error on stderr for 'exit -1', got nothing"

# -- Assertion 8: too many arguments is rejected ------------------------------

run "exit 0 1"
[ "$STATUS" -eq 2 ] || fail "assertion 8: shell should not exit on 'exit 0 1', got status: $STATUS"
[ -n "$ERR" ]       || fail "assertion 8: expected an error on stderr for 'exit 0 1', got nothing"

# -- Assertion 9: shell continues after a rejected exit -----------------------
# After a failed exit call, the shell must still be responsive.

OUT=$(printf "exit abc\necho still_running\n" | "$BINARY" 2>/dev/null)
[ "$OUT" = "still_running" ] || fail "assertion 9: expected shell to continue after bad exit, got: '$OUT'"

# -- Assertion 10: float argument is rejected ---------------------------------

run "exit 1.5"
[ "$STATUS" -eq 2 ] || fail "assertion 10: shell should not exit on 'exit 1.5', got status: $STATUS"
[ -n "$ERR" ]       || fail "assertion 10: expected an error on stderr for 'exit 1.5', got nothing"

# -- Cleanup ------------------------------------------------------------------

rm -f /tmp/csh_test_stderr

echo "PASS $0"
exit 0
