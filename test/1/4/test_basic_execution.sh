#!/bin/sh
# Tests Against Basic External Command Execution

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
  # run <input> — captures stdout and stderr into $OUT and $ERR
  OUT=$(printf "%s\n" "$1" | "$BINARY" 2>/tmp/csh_test_stderr)
  STATUS=$?
  ERR=$(cat /tmp/csh_test_stderr)
}

# -- Assertion 1: Simple command produces stdout ------------------------------
# A single known command must execute and write to stdout.

run "echo hello"
[ -n "$OUT" ]           || fail "assertion 1: expected stdout from 'echo hello', got nothing"
[ "$OUT" = "hello" ]    || fail "assertion 1: expected 'hello', got: '$OUT'"

# -- Assertion 2: Command with multiple arguments -----------------------------
# Arguments must be passed through to the child process intact.

run "echo foo bar baz"
[ "$OUT" = "foo bar baz" ] || fail "assertion 2: expected 'foo bar baz', got: '$OUT'"

# -- Assertion 3: Unknown command writes to stderr ---------------------------
# An unresolvable command must produce a message on stderr and no stdout.

run "thisisnotarealcommand_xyz"
[ -z "$OUT" ] || fail "assertion 3: expected no stdout for unknown command, got: '$OUT'"
[ -n "$ERR" ] || fail "assertion 3: expected an error on stderr for unknown command, got nothing"

# -- Assertion 4: Shell exits 0 after a successful command -------------------

printf "echo hello\n" | "$BINARY" > /dev/null 2>/dev/null
STATUS=$?
[ "$STATUS" -eq 0 ] || fail "assertion 4: expected exit status 0 after successful command, got: $STATUS"

# -- Assertion 5: Empty lines are silently ignored ---------------------------
# Blank input must not produce any output or error.

run ""
[ -z "$OUT" ] || fail "assertion 5: expected no stdout for empty input, got: '$OUT'"
[ -z "$ERR" ] || fail "assertion 5: expected no stderr for empty input, got: '$ERR'"

# -- Assertion 6: Whitespace-only lines are silently ignored -----------------

run "     "
[ -z "$OUT" ] || fail "assertion 6: expected no stdout for whitespace-only input, got: '$OUT'"
[ -z "$ERR" ] || fail "assertion 6: expected no stderr for whitespace-only input, got: '$ERR'"

# -- Assertion 7: Comment lines are silently ignored -------------------------

run "# this is a comment"
[ -z "$OUT" ] || fail "assertion 7: expected no stdout for comment line, got: '$OUT'"
[ -z "$ERR" ] || fail "assertion 7: expected no stderr for comment line, got: '$ERR'"

# -- Assertion 8: Multiple sequential commands both execute ------------------
# Each command in a multi-line script must run independently.

OUT=$(printf "echo first\necho second\n" | "$BINARY" 2>/dev/null)
FIRST=$(printf "%s" "$OUT" | sed -n '1p')
SECOND=$(printf "%s" "$OUT" | sed -n '2p')
[ "$FIRST" = "first" ]   || fail "assertion 8: expected 'first' on line 1, got: '$FIRST'"
[ "$SECOND" = "second" ] || fail "assertion 8: expected 'second' on line 2, got: '$SECOND'"

# -- Cleanup ------------------------------------------------------------------

rm -f /tmp/csh_test_stderr

echo "PASS $0"
exit 0
