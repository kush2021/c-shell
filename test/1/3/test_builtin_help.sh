#!/bin/sh
# Tests Against the help Built-in

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

# -- Assertion 1: bare help exits shell cleanly -------------------------------
# Running help alone should not crash or hang the shell; the session should
# still terminate 0 via EOF after the command completes.

run "help"
[ "$STATUS" -eq 0 ] || fail "assertion 1: expected exit status 0 after 'help', got: $STATUS"

# -- Assertion 2: bare help lists all built-ins on stdout ---------------------

run "help"
for name in exit cd pwd echo export unset; do
  echo "$OUT" | grep -q "$name" || fail "assertion 2: expected 'help' output to mention '$name', got: '$OUT'"
done

# -- Assertion 3: bare help produces no stderr output -------------------------

run "help"
[ -z "$ERR" ] || fail "assertion 3: expected no stderr for bare 'help', got: '$ERR'"

# -- Assertion 4: help <name> shows only that command's help -----------------
# Requesting help for a single built-in should mention that built-in and
# should not dump the full listing of unrelated built-ins.

run "help cd"
echo "$OUT" | grep -q "cd" || fail "assertion 4: expected 'help cd' output to mention 'cd', got: '$OUT'"
echo "$OUT" | grep -q "export" && fail "assertion 4: expected 'help cd' to omit unrelated built-in 'export', got: '$OUT'"

# -- Assertion 5: help <name> exits 0 -----------------------------------------

run "help pwd"
[ "$STATUS" -eq 0 ] || fail "assertion 5: expected exit status 0 for 'help pwd', got: $STATUS"

# -- Assertion 6: help with multiple valid names shows each ------------------

run "help cd pwd"
echo "$OUT" | grep -q "cd"  || fail "assertion 6: expected 'help cd pwd' output to mention 'cd', got: '$OUT'"
echo "$OUT" | grep -q "pwd" || fail "assertion 6: expected 'help cd pwd' output to mention 'pwd', got: '$OUT'"

# -- Assertion 7: help with unknown name does not exit the shell -------------

run "help not_a_real_builtin_xyz"
[ "$STATUS" -eq 0 ] || fail "assertion 8: shell should not exit on unknown 'help' argument, got status: $STATUS"

# -- Assertion 8: help output is non-empty ------------------------------------

run "help"
[ -n "$OUT" ] || fail "assertion 9: expected non-empty output for bare 'help', got nothing"

# -- Assertion 9: shell continues after help -------------------------------
# Confirms 'help' does not consume or corrupt subsequent input processing.

OUT=$(printf "help\necho still_running\n" | "$BINARY" 2>/dev/null)
echo "$OUT" | grep -q "still_running" || fail "assertion 10: expected shell to continue after 'help', got: '$OUT'"

# -- Cleanup ------------------------------------------------------------------

rm -f /tmp/csh_test_stderr

echo "PASS $0"
exit 0
