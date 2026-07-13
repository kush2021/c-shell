#!/bin/sh
# Tests Against the export Built-in
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
# -- Assertion 1: export sets a variable visible to child processes ----------
# A child process (env) should see exactly what was exported.
run "export csh_test_basic=hello
env
exit 0"
echo "$OUT" | grep -qx "csh_test_basic=hello" || fail "assertion 1: expected csh_test_basic=hello in child environment, got: $OUT"
# -- Assertion 2: value may contain additional '=' characters -----------------
# Only the first '=' is a separator; everything after it belongs to the value.
run "export csh_test_eq=a=b=c
env
exit 0"
echo "$OUT" | grep -qx "csh_test_eq=a=b=c" || fail "assertion 2: expected csh_test_eq=a=b=c in child environment, got: $OUT"
# -- Assertion 3: an empty value is allowed -----------------------------------
run "export csh_test_empty=
env
exit 0"
echo "$OUT" | grep -qx "csh_test_empty=" || fail "assertion 3: expected csh_test_empty= (empty value) in child environment, got: $OUT"
# -- Assertion 4: re-exporting overwrites the previous value ------------------
run "export csh_test_overwrite=first
export csh_test_overwrite=second
env
exit 0"
echo "$OUT" | grep -qx "csh_test_overwrite=second" || fail "assertion 4: expected csh_test_overwrite=second, got: $OUT"
echo "$OUT" | grep -qx "csh_test_overwrite=first"  && fail "assertion 4: stale value csh_test_overwrite=first should not still be present"
# -- Assertion 5: missing '=' is a usage error --------------------------------
run "export csh_test_missing_eq"
[ "$STATUS" -eq 2 ] || fail "assertion 5: expected misuse status 2 for missing '=', got: $STATUS"
[ -n "$ERR" ]       || fail "assertion 5: expected an error on stderr for missing '=', got nothing"
# -- Assertion 6: a rejected export must not touch the environment -----------
# Regression test for the earlier null-deref bug: on the error path, the
# shell must bail out before ever calling setenv().
run "export csh_test_missing_eq
env
exit 0"
echo "$OUT" | grep -q "^csh_test_missing_eq" && fail "assertion 6: csh_test_missing_eq must not appear in the environment after a rejected export"
# -- Assertion 7: no arguments at all is a usage error ------------------------
run "export"
[ "$STATUS" -eq 2 ] || fail "assertion 7: expected misuse status 2 for bare 'export', got: $STATUS"
[ -n "$ERR" ]       || fail "assertion 7: expected an error on stderr for bare 'export', got nothing"
# -- Assertion 8: too many arguments is a usage error -------------------------
run "export csh_test_extra=value another_token"
[ "$STATUS" -eq 2 ] || fail "assertion 8: expected misuse status 2 for too many arguments, got: $STATUS"
[ -n "$ERR" ]       || fail "assertion 8: expected an error on stderr for too many arguments, got nothing"
# -- Assertion 9: shell continues after a rejected export --------------------
run "export csh_test_missing_eq
echo still_running
exit 0"
[ "$OUT" = "still_running" ] || fail "assertion 9: expected shell to continue after rejected export, got: '$OUT'"
# -- Cleanup ------------------------------------------------------------------
rm -f /tmp/csh_test_stderr
echo "PASS $0"
exit 0
