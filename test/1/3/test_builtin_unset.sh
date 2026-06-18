#!/bin/sh
# Tests Against the unset Built-in
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
# -- Assertion 1: unset removes a previously exported variable ---------------
# Two env calls bracket the unset; if export and unset both work, the exact
# line should appear exactly once total (from the call before unset).
run "export csh_test_unset_basic=value
env
unset csh_test_unset_basic
env
exit 0"
COUNT=$(echo "$OUT" | grep -cx "csh_test_unset_basic=value")
[ "$COUNT" -eq 1 ] || fail "assertion 1: expected csh_test_unset_basic=value exactly once (set, then removed), got $COUNT occurrence(s)"
# -- Assertion 2: unsetting a variable that was never set still succeeds -----
# POSIX unsetenv() is idempotent — removing something that isn't there is
# not an error.
run "unset csh_test_never_set_xyz"
[ "$STATUS" -eq 0 ] || fail "assertion 2: expected status 0 for unsetting a nonexistent variable, got: $STATUS"
[ -z "$ERR" ]       || fail "assertion 2: expected no stderr output, got: $ERR"
# -- Assertion 3: too few arguments is a usage error --------------------------
run "unset"
[ "$STATUS" -eq 2 ] || fail "assertion 3: expected misuse status 2 for bare 'unset', got: $STATUS"
[ -n "$ERR" ]       || fail "assertion 3: expected an error on stderr for bare 'unset', got nothing"
# -- Assertion 4: too many arguments is a usage error -------------------------
run "unset csh_test_a csh_test_b"
[ "$STATUS" -eq 2 ] || fail "assertion 4: expected misuse status 2 for too many arguments, got: $STATUS"
[ -n "$ERR" ]       || fail "assertion 4: expected an error on stderr for too many arguments, got nothing"
# -- Assertion 5: an invalid variable name is rejected ------------------------
# '=' is not legal inside a variable name; unsetenv() should fail and the
# builtin should report it. This only checks for a nonzero status since the
# exact CSH_ERROR value (distinct from CSH_ERROR_MISUSE) wasn't confirmed —
# tighten this to the exact value once you know it.
run "unset csh_test_invalid=name"
[ "$STATUS" -ne 0 ] || fail "assertion 5: expected a nonzero status for an invalid variable name, got: $STATUS"
[ -n "$ERR" ]       || fail "assertion 5: expected an error on stderr for an invalid variable name, got nothing"
# -- Assertion 6: shell continues after a rejected unset ----------------------
run "unset
echo still_running
exit 0"
[ "$OUT" = "still_running" ] || fail "assertion 6: expected shell to continue after rejected unset, got: '$OUT'"
# -- Cleanup ------------------------------------------------------------------
rm -f /tmp/csh_test_stderr
echo "PASS $0"
exit 0
