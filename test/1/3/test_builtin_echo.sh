#!/bin/sh
# Tests Against the echo Built-in

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
  OUT=$(printf "%s\n" "$1" | "$BINARY" 2>/tmp/csh_test_stderr)
  STATUS=$?
  ERR=$(cat /tmp/csh_test_stderr)
}

run_lines() {
  OUT=$(printf "%s\n" "$1" | "$BINARY" 2>/tmp/csh_test_stderr)
  STATUS=$?
  ERR=$(cat /tmp/csh_test_stderr)
}

# -- Assertion 1: echo with no arguments prints an empty line -----------------

run "echo"
[ -z "$OUT" ] || fail "assertion 1: expected empty output from bare echo, got: '$OUT'"

# -- Assertion 2: echo with a single argument prints that argument ------------

run "echo hello"
[ "$OUT" = "hello" ] || fail "assertion 2: expected 'hello', got: '$OUT'"

# -- Assertion 3: echo with multiple arguments prints them space-separated ----

run "echo hello world"
[ "$OUT" = "hello world" ] || fail "assertion 3: expected 'hello world', got: '$OUT'"

# -- Assertion 4: echo with more than two arguments ---------------------------

run "echo one two three"
[ "$OUT" = "one two three" ] || fail "assertion 4: expected 'one two three', got: '$OUT'"

# -- Assertion 5: echo -n suppresses the trailing newline ---------------------
# Run echo -n hello followed by echo world — without the newline suppression
# the output would be "hello\nworld"; with it the output is "helloworld".

OUT=$(printf "echo -n hello\necho world\n" | "$BINARY" 2>/dev/null)
[ "$OUT" = "helloworld" ] || fail "assertion 5: expected 'helloworld' with -n, got: '$OUT'"

# -- Assertion 6: echo -n with multiple arguments -----------------------------

OUT=$(printf "echo -n foo bar\necho done\n" | "$BINARY" 2>/dev/null)
[ "$OUT" = "foo bardone" ] || fail "assertion 6: expected 'foo bardone' with -n, got: '$OUT'"

# -- Assertion 7: echo exit status is 0 ---------------------------------------

run "echo hello"
[ "$STATUS" -eq 0 ] || fail "assertion 7: expected exit status 0, got: $STATUS"

# -- Assertion 8: echo produces no stderr output ------------------------------

run "echo hello"
[ -z "$ERR" ] || fail "assertion 8: expected no stderr output from echo, got: '$ERR'"

# -- Assertion 9: echo output is a single line --------------------------------

run "echo hello world"
LINE_COUNT=$(printf "%s\n" "$OUT" | wc -l | tr -d ' ')
[ "$LINE_COUNT" -eq 1 ] || fail "assertion 9: expected exactly one line of output, got: $LINE_COUNT"

# -- Assertion 10: shell continues after echo ---------------------------------

OUT=$(printf "echo first\necho second\n" | "$BINARY" 2>/dev/null)
[ "$OUT" = "$(printf 'first\nsecond')" ] || fail "assertion 10: expected 'first\\nsecond', got: '$OUT'"

# -- Cleanup ------------------------------------------------------------------

rm -f /tmp/csh_test_stderr

echo "PASS $0"
exit 0
